// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

// file header
#include "BPrintSession.h"

// B headers
#include "BContext.h"
#include "BErrorHandler.h"
#include "BException.h"
#include "BPageFormat.h"
#include "BPrinter.h"
#include "BPrintSettings.h"
#include "BString.h"
#include "BUrl.h"
#include "CFUtils.h"


namespace B {

// ==========================================================================================
//  PrintSession

#pragma mark PrintSession

#define kPrintSessionObject CFSTR("ca.paullalonde.b.printsession.object")

const AutoPMSheetDoneUPP    PrintSession::sPageSetupSheetDoneUPP(PageSetupSheetDoneProc);
const AutoPMSheetDoneUPP    PrintSession::sPrintSheetDoneUPP(PrintSheetDoneProc);

// ------------------------------------------------------------------------------------------
PrintSession::PrintSession()
    : mPrintSession(NULL), mInDocument(false), mInPage(false), 
      mModality(DialogModality::Modal()), 
      mBeginDocument(PMSessionBeginCGDocument), mEndDocument(PMSessionEndDocument), 
      mBeginPage(PMSessionBeginPage), mEndPage(PMSessionEndPage)
{
    try
    {
        OSStatus    err;
        
        err = PMCreateSession(&mPrintSession);
        B_THROW_IF_STATUS(err);
        
        AddSessionToPMSession();
    }
    catch (...)
    {
        if (mPrintSession != NULL)
            PMRelease(mPrintSession);
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
PrintSession::~PrintSession()
{   
    PMRelease(mPrintSession);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::GetPrinters(
    std::vector<String>&    outNames,   //!< An array of printer names.
    size_t&                 outCurrent) //!< The index of the current printer in @a outNames.
    const
{
    PMPrinter   junkPrinter;
    CFArrayRef  printersArray;
    CFIndex     currentIndex;
    OSStatus    err;
    
    err = PMSessionCreatePrinterList(mPrintSession, &printersArray, &currentIndex, 
                                     &junkPrinter);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CFArrayRef>   printersPtr(printersArray, from_copy);
    CFIndex             numPrinters = CFArrayGetCount(printersArray);
    std::vector<String> printerNames;
    
    printerNames.reserve(numPrinters);
    
    for (CFIndex i = 0; i < numPrinters; i++)
    {
        CFStringRef aName   = CFStringRef(CFArrayGetValueAtIndex(printersArray, i));
        
        printerNames.push_back(String(aName));
    }
    
    printerNames.swap(outNames);
    outCurrent = currentIndex;
}

// ------------------------------------------------------------------------------------------
Printer
PrintSession::GetPrinter() const
{
    PMPrinter   printer;
    OSStatus    err;
    
    err = PMSessionGetCurrentPrinter(mPrintSession, &printer);
    B_THROW_IF_STATUS(err);
    
    return (Printer(printer));
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetPrinter(
    CFStringRef inPrinterName)  //!< The new printer's name.
{
    OSStatus    err;
    
    err = PMSessionSetCurrentPrinter(mPrintSession, inPrinterName);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetPrinter(
    const Printer& inPrinter)   //!< The new printer.
{
    OSStatus    err;
    
    err = PMSessionSetCurrentPMPrinter(mPrintSession, inPrinter.mPrinter);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::GetPageFormats(
    const Printer&              inPrinter,      //!< The printer whose page formats we want.
    std::vector<PageFormat>&    outPageFormats) //!< An array of page formats.
    const
{
    CFArrayRef  formatsArray;
    OSStatus    err;
    
    err = PMSessionCreatePageFormatList(mPrintSession, inPrinter.mPrinter, &formatsArray);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CFArrayRef>       formatsPtr(formatsArray, from_copy);
    CFIndex                 numFormats  = CFArrayGetCount(formatsArray);
    std::vector<PageFormat> pageFormats;
    
    pageFormats.reserve(numFormats);
    
    for (CFIndex i = 0; i < numFormats; i++)
    {
        PMPageFormat    aFormat = PMPageFormat(CFArrayGetValueAtIndex(formatsArray, i));
        
        pageFormats.push_back(PageFormat(aFormat));
    }
    
    pageFormats.swap(outPageFormats);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::GetDocumentFormats(
    std::vector<String>&    outFormats) //!< An array of spool file format names.
    const
{
    CFArrayRef  formatsArray;
    OSStatus    err;
    
    err = PMSessionGetDocumentFormatGeneration(mPrintSession, &formatsArray);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CFArrayRef>   formatsPtr(formatsArray, from_copy);
    CFIndex             numFormats  = CFArrayGetCount(formatsArray);
    std::vector<String> docFormats;
    
    docFormats.reserve(numFormats);
    
    for (CFIndex i = 0; i < numFormats; i++)
    {
        CFStringRef aFormat = CFStringRef(CFArrayGetValueAtIndex(formatsArray, i));
        
        docFormats.push_back(String(aFormat));
    }
    
    docFormats.swap(outFormats);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetDocumentFormat(
    CFStringRef inDocFormat,    //!< The desired spool file format
    CFStringRef inGraphicsType) //!< The desired graphics context type.
{
    B_ASSERT(!mInDocument); // Can't call this after BeginDocument().
    
    const void*         graphicsTypesStore[] = { inGraphicsType };
    OSPtr<CFArrayRef>   graphicsTypes(CFArrayCreate(NULL, 
                                                    graphicsTypesStore, 
                                                    1, 
                                                    &kCFTypeArrayCallBacks), 
                                         from_copy);
    OSStatus            err;
    
    err = PMSessionSetDocumentFormatGeneration(mPrintSession, inDocFormat, 
                                               graphicsTypes, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
PMDestinationType
PrintSession::GetDestinationType(
    const PrintSettings&    inPrintSettings)    //!< The print settings object.
    const
{
    PMDestinationType   destinationType;
    OSStatus            err;
    
    err = PMSessionGetDestinationType(mPrintSession, inPrintSettings.mPrintSettings, 
                                      &destinationType);
    B_THROW_IF_STATUS(err);
    
    return (destinationType);
}

// ------------------------------------------------------------------------------------------
String
PrintSession::GetDestinationFormat(
    const PrintSettings&    inPrintSettings)    //!< The print settings object.
    const
{
    CFStringRef cfstr;
    OSStatus    err;
    
    err = PMSessionCopyDestinationFormat(mPrintSession, inPrintSettings.mPrintSettings, 
                                         &cfstr);
    B_THROW_IF_STATUS(err);
    
    return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
Url
PrintSession::GetDestinationLocation(
    const PrintSettings&    inPrintSettings)    //!< The print settings object.
    const
{
    CFURLRef    urlRef  = NULL;
    Url         url;
    OSStatus    err;
    
    err = PMSessionCopyDestinationLocation(mPrintSession, inPrintSettings.mPrintSettings, 
                                           &urlRef);
    B_THROW_IF_STATUS(err);
    
    if (urlRef != NULL)
        url = OSPtr<CFURLRef>(urlRef, from_copy);
    
    return (url);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::GetOutputFormats(
    PMDestinationType       inDestinationType,  //!< The destination type.
    std::vector<String>&    outOutputFormats)   //!< An array of output format names.
    const
{
    CFArrayRef  formatsArray;
    OSStatus    err;
    
    err = PMSessionCopyOutputFormatList(mPrintSession, inDestinationType, &formatsArray);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CFArrayRef>   formatsPtr(formatsArray, from_copy);
    CFIndex             numFormats  = CFArrayGetCount(formatsArray);
    std::vector<String> outputFormats;
    
    outputFormats.reserve(numFormats);
    
    for (CFIndex i = 0; i < numFormats; i++)
    {
        CFStringRef aFormat = CFStringRef(CFArrayGetValueAtIndex(formatsArray, i));
        
        outputFormats.push_back(String(aFormat));
    }
    
    outputFormats.swap(outOutputFormats);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetDestination(
    const PrintSettings&    inPrintSettings,    //!< The print settings object.
    PMDestinationType       inDestinationType,  //!< The destination type.
    const String&           inOutputFormat,     //!< The destination output format.
    const Url&              inLocation)         //!< The destination location.
{
    CFStringRef format  = inOutputFormat.empty() ? inOutputFormat.cf_ref() : NULL;
    CFURLRef    url     = inLocation.Empty() ? inLocation.cf_ref() : NULL;
    OSStatus    err;
    
    err = PMSessionSetDestination(mPrintSession, inPrintSettings.mPrintSettings, 
                                  inDestinationType, format, url);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetError(
    OSStatus    inError)    //!< The new result code.
{
    OSStatus    err;
    
    err = PMSessionSetError(mPrintSession, inError);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetModality(const DialogModality& inModality)
{
    mModality = inModality;
}

// ------------------------------------------------------------------------------------------
void
PrintSession::ShowPrinterPresets(bool showThem)
{
    B_ASSERT(!mInDocument); // Can't call this after BeginDocument().
    
    OSStatus    err;
    
    if (showThem)
    {
        err = PMSessionEnablePrinterPresets(mPrintSession, CFSTR("Photo"));
    }
    else
    {
        err = PMSessionDisablePrinterPresets(mPrintSession);
    }
    
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::ShowStatusDialog(bool showIt)
{
    B_ASSERT(!mInDocument); // Can't call this after BeginDocument().
    
    if (showIt)
    {
        mBeginDocument  = PMSessionBeginCGDocument;
        mEndDocument    = PMSessionEndDocument;
        mBeginPage      = PMSessionBeginPage;
        mEndPage        = PMSessionEndPage;
    }
    else
    {
        mBeginDocument  = PMSessionBeginCGDocumentNoDialog;
        mEndDocument    = PMSessionEndDocumentNoDialog;
        mBeginPage      = PMSessionBeginPageNoDialog;
        mEndPage        = PMSessionEndPageNoDialog;
    }
}

// ------------------------------------------------------------------------------------------
CFTypeRef
PrintSession::GetData(
    CFStringRef inKey)      //!< CF string uniquely identifying the data of interest.
    const
{
    CFTypeRef   ref;
    OSStatus    err;
    
    err = PMSessionGetDataFromSession(mPrintSession, inKey, &ref);
    B_THROW_IF_STATUS(err);
    
    return (ref);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::SetData(
    CFStringRef inKey,      //!< CF string uniquely identifying the data of interest.
    CFTypeRef   inData)     //!< The new value.
{
    OSStatus    err;
    
    err = PMSessionSetDataInSession(mPrintSession, inKey, inData);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::Default(
    PageFormat&     ioPageFormat)       //!< The page format object.
{
    OSStatus    err;
    
    err = PMSessionDefaultPageFormat(mPrintSession, ioPageFormat.mPageFormat);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::Default(
    PrintSettings& ioPrintSettings)     //!< The print settings object.
{
    OSStatus    err;
    
    err = PMSessionDefaultPrintSettings(mPrintSession, ioPrintSettings.mPrintSettings);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! @return @c true if the page format had to be changed.
*/
bool
PrintSession::Validate(
    PageFormat&     ioPageFormat)       //!< The page format object.
{
    PageFormat  oldFormat   = ioPageFormat.copy();
    Boolean     formatChanged;
    OSStatus    err;
    
    err = PMSessionValidatePageFormat(mPrintSession, ioPageFormat.mPageFormat, 
                                      &formatChanged);
    B_THROW_IF_STATUS(err);
    
    if (!formatChanged)
    {
        formatChanged = !ioPageFormat.Equals(oldFormat);
    }
    
    return (formatChanged);
}

// ------------------------------------------------------------------------------------------
/*! @return @c true if the print settings had to be changed.
*/
bool
PrintSession::Validate(
    PrintSettings& ioPrintSettings)     //!< The print settings object.
{
    PrintSettings   oldSettings = ioPrintSettings.copy();
    Boolean         formatChanged;
    OSStatus        err;
    
    err = PMSessionValidatePrintSettings(mPrintSession, ioPrintSettings.mPrintSettings, 
                                         &formatChanged);
    B_THROW_IF_STATUS(err);
    
    if (!formatChanged)
    {
        formatChanged = !ioPrintSettings.Equals(oldSettings);
    }
    
    return (formatChanged);
}

// ------------------------------------------------------------------------------------------
PageFormat
PrintSession::MakePageFormat()
{
    PageFormat  pageFormat;
    
    Default(pageFormat);
    
    return (pageFormat);
}

// ------------------------------------------------------------------------------------------
PrintSettings
PrintSession::MakePrintSettings()
{
    PrintSettings   printSettings;
    
    Default(printSettings);
    
    return (printSettings);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::BeginDocument(
    const PrintSettings&    inPrintSettings,    //!< The print settings object.
    const PageFormat&       inPageFormat)       //!< The page format object.
{
    B_ASSERT(!mInDocument); // Can't call this after BeginDocument().
    
    OSStatus    err;
    
    err = (*mBeginDocument)(mPrintSession, inPrintSettings.mPrintSettings, 
                            inPageFormat.mPageFormat);
    B_THROW_IF_STATUS(err);
    
    mInDocument = true;
}

// ------------------------------------------------------------------------------------------
void
PrintSession::EndDocument()
{
    B_ASSERT(mInDocument);  // Can only call this after BeginDocument().
    
    OSStatus    err;
    
    mInDocument = false;
    
    err = (*mEndDocument)(mPrintSession);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::BeginPage(
    const PageFormat&       inPageFormat)   //!< The page format object.
{
    B_ASSERT(mInDocument);  // Can only call this after BeginDocument().
    B_ASSERT(!mInPage);     // Can't call this after BeginPage().
    
    OSStatus    err;
    
    err = (*mBeginPage)(mPrintSession, inPageFormat.mPageFormat, NULL);
    B_THROW_IF_STATUS(err);
    
    mInPage = true;
}

// ------------------------------------------------------------------------------------------
void
PrintSession::EndPage()
{
    B_ASSERT(mInDocument);  // Can only call this after BeginDocument().
    B_ASSERT(mInPage);      // Can only call this after BeginPage().
    
    OSStatus    err;
    
    mInPage = false;
    
    err = (*mEndPage)(mPrintSession);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Graphics::Context
PrintSession::GetPageContext()
{
    B_ASSERT(mInDocument);  // Can only call this after BeginDocument().
    B_ASSERT(mInPage);      // Can only call this after BeginPage().
    
    CGContextRef    contextRef;
    OSStatus        err;
    
    err = PMSessionGetCGGraphicsContext(mPrintSession, &contextRef);
    B_THROW_IF_STATUS(err);
    
    return Graphics::Context(contextRef);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::PageSetupDialog(
    PageFormat&         ioPageFormat, 
    DialogDoneCallback  inCallback)
{
    B_ASSERT(inCallback != NULL);
    
    Boolean     accepted    = false;
    OSStatus    err;
    
    mPageSetupDialogCallback = inCallback;
    
    if (mModality.IsSheet())
    {
        err = PMSessionUseSheets(mPrintSession, mModality.GetParent(), 
                                 sPageSetupSheetDoneUPP);
        B_THROW_IF_STATUS(err);
    }
    
    err = PMSessionPageSetupDialog(mPrintSession, ioPageFormat.mPageFormat, 
                                   &accepted);
    B_THROW_IF_STATUS(err);
    
    if (!mModality.IsSheet())
    {
        PageSetupDialogDone(accepted);
    }
}

// ------------------------------------------------------------------------------------------
void
PrintSession::PageSetupDialogDone(Boolean accepted)
{
    B_ASSERT(mPageSetupDialogCallback != NULL);
    
    DialogDoneCallback  cb  = mPageSetupDialogCallback;
    
    mPageSetupDialogCallback = NULL;
    
    cb(*this, accepted);
}

// ------------------------------------------------------------------------------------------
void
PrintSession::PrintDialog(
    PrintSettings&      ioPrintSettings, 
    const PageFormat&   inPageFormat, 
    DialogDoneCallback  inCallback)
{
    B_ASSERT(inCallback != NULL);
    
    Boolean     accepted    = false;
    OSStatus    err;
    
    mPrintDialogCallback = inCallback;
    
    if (mModality.IsSheet())
    {
        err = PMSessionUseSheets(mPrintSession, mModality.GetParent(), 
                                 sPrintSheetDoneUPP);
        B_THROW_IF_STATUS(err);
    }
    
    err = PMSessionPrintDialog(mPrintSession, ioPrintSettings.mPrintSettings, 
                               inPageFormat.mPageFormat, &accepted);
    B_THROW_IF_STATUS(err);
    
    if (!mModality.IsSheet())
    {
        PrintDialogDone(accepted);
    }
}

// ------------------------------------------------------------------------------------------
void
PrintSession::PrintDialogDone(Boolean accepted)
{
    B_ASSERT(mPrintDialogCallback != NULL);
    
    DialogDoneCallback  cb  = mPrintDialogCallback;
    
    mPrintDialogCallback = NULL;
    
    cb(*this, accepted);
}

// ------------------------------------------------------------------------------------------
PrintSession*
PrintSession::GetSessionFromPMSession(
    PMPrintSession  printSession)
{
    CFTypeRef   typeRef;
    OSStatus    err;
    
    err = PMSessionGetDataFromSession(printSession, kPrintSessionObject, &typeRef);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CFDataRef>    dataRef(CFUMakeTypedValue<CFDataRef>(typeRef));
    
    if (dataRef.get() == NULL)
        B_THROW(ConstantOSStatusException<paramErr>());
    
    PrintSession*   session;
    
    if (CFDataGetLength(dataRef) != sizeof(session))
        B_THROW(ConstantOSStatusException<paramErr>());
    
    BlockMoveData(CFDataGetBytePtr(dataRef), &session, sizeof(session));
    
    if (session == NULL)
        B_THROW(ConstantOSStatusException<paramErr>());
    
    return session;
}

// ------------------------------------------------------------------------------------------
void
PrintSession::AddSessionToPMSession()
{
    PrintSession*       session = this;
    OSPtr<CFDataRef>    data(CFDataCreate(NULL, 
                                          reinterpret_cast<const UInt8*>(&session), 
                                          sizeof(session)), 
                             from_copy);
    OSStatus            err;
    
    err = PMSessionSetDataInSession(mPrintSession, kPrintSessionObject, data.get());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
pascal void
PrintSession::PageSetupSheetDoneProc(
    PMPrintSession  printSession, 
    WindowRef       /* documentWindow */, 
    Boolean         accepted)
{
    try
    {
        PrintSession*   session = GetSessionFromPMSession(printSession);
        
        session->PageSetupDialogDone(accepted);
    }
    catch (...)
    {
        // Just prevent the exception from propagating.
    }
}

// ------------------------------------------------------------------------------------------
pascal void
PrintSession::PrintSheetDoneProc(
    PMPrintSession  printSession, 
    WindowRef       /* documentWindow */, 
    Boolean         accepted)
{
    try
    {
        PrintSession*   session = GetSessionFromPMSession(printSession);
        
        session->PrintDialogDone(accepted);
    }
    catch (...)
    {
        // Just prevent the exception from propagating.
    }
}


// ==========================================================================================
//  AutoPrintDocument

#pragma mark AutoPrintDocument

// ------------------------------------------------------------------------------------------
AutoPrintDocument::AutoPrintDocument(
    PrintSession&           ioPrintSession,
    const PrintSettings&    inPrintSettings, 
    const PageFormat&       inPageFormat)
        : mPrintSession(ioPrintSession)
{
    mPrintSession.BeginDocument(inPrintSettings, inPageFormat);
}

// ------------------------------------------------------------------------------------------
AutoPrintDocument::~AutoPrintDocument()
{   
    mPrintSession.EndDocument();
}


// ==========================================================================================
//  AutoPrintPage

#pragma mark AutoPrintPage

// ------------------------------------------------------------------------------------------
AutoPrintPage::AutoPrintPage(
    PrintSession&       ioPrintSession, 
    const PageFormat&   inPageFormat)
        : mPrintSession(ioPrintSession)
{
    mPrintSession.BeginPage(inPageFormat);
}

// ------------------------------------------------------------------------------------------
AutoPrintPage::~AutoPrintPage()
{   
    mPrintSession.EndPage();
}

}   // namespace B
