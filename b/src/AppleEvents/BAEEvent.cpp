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
#include "BAEEvent.h"

// system headers
#include <Carbon/Carbon.h>

// library headers
#include "MoreAppleEvents.h"

// B headers
#include "BAEObject.h"
#include "BAEObjectSupport.h"
#include "BAEReader.h"
#include "BAEWriter.h"
#include "BException.h"
#include "BPrinter.h"
#include "BPrintSession.h"
#include "BPrintSettings.h"
#include "BStringUtilities.h"


static void ReadInsertionLoc(
    const AERecord& inRecord, 
    DescType&       outPosition, 
    B::AEObjectPtr& outObject)
{
    // We don't use AEReader here because MOSL's implicit type coercions 
    // conflict with AppleScript's built-in typeObjectSpecifier => 
    // typeInsertionLoc coercion handler.
    
    B::AEDescriptor insloc, aeinsloc;
    OSStatus        err;
    
    err = AEGetKeyDesc(&inRecord, keyAEInsertHere, 
                       typeInsertionLoc, insloc);
    B_THROW_IF_STATUS(err);
    
    err = AECoerceDesc(insloc, typeAERecord, aeinsloc);
    B_THROW_IF_STATUS(err);
    
    B::DescParamHelper::ReadKey<typeEnumeration>(aeinsloc, keyAEPosition, outPosition);
    B::DescParamHelper::ReadKey<typeObjectSpecifier>(aeinsloc, keyAEObject, outObject);
}


namespace B {


namespace AEEventFunctor {

// ==========================================================================================
//  OptionalAEDesc

// ------------------------------------------------------------------------------------------
AEWriter&
OptionalAEDesc::operator () (
    AEWriter& writer)
    const
{
    if ((mDesc != NULL) && (mDesc->descriptorType != typeNull))
        writer.WriteDesc(*mDesc);
    else
        writer.CacheNextKeyword(0);
    
    return (writer);
}

// ==========================================================================================
//  AEDescResult

// ------------------------------------------------------------------------------------------
/*! Copies the data in @a inResult into the area referenced by @a mDescRef.
*/
void
AEDescResult::operator () (
    const AEDesc&   inResult)       //!< The result descriptor.
    const
{
    OSStatus    err;
    
    err = AEDuplicateDesc(&inResult, &mDescRef);
    B_THROW_IF_STATUS(err);
}

}   // namespace AEEventFunctor


// ==========================================================================================
//  AEEventBase

// ------------------------------------------------------------------------------------------
AEEventBase::AEEventBase(
    const AppleEvent&   inAppleEvent,   //!< The incoming Apple %Event.
    AEWriter&           ioWriter)       //!< The reponse to @a inAppleEvent.  May be null (eg, if the caller doesn't want a reply).
        : mAppleEvent(inAppleEvent), mWriter(ioWriter)
{
}

// ------------------------------------------------------------------------------------------
AEEventBase::~AEEventBase()
{
}

// ------------------------------------------------------------------------------------------
/*! This function is called after an Apple %Event has been handled, but prior to returning 
    control to the toolbox.
    
    Derived classes should override this if they need to return output parameters to the 
    caller.  The idea is that the outputs are cached as member variables of the derived 
    class, and written to the Apple %Event reply (aka @a mResult) from within this 
    function.
*/
void
AEEventBase::Update()
{
    AutoAEWriterDesc    autoDesc(mWriter, typeNull);
}

// ------------------------------------------------------------------------------------------
void
AEEventBase::CheckRequiredParams()
{
    OSStatus    err;
    
    err = MoreAEGotRequiredParams(&mAppleEvent);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*  The recipient of the event is told not to return a reply.  @a inEvent must already be 
    fully constructed.
    
    For a list of possible values for @a inMode, consult 
    \<ApplicationServices/AEDataModel.h\>.
*/
void
AEEventBase::SendEvent(
    const AppleEvent&   inEvent,            //!< The Apple %Event to send.
    AESendMode          inMode /* = 0 */)   //!< The event's send mode.
{
    // This function never waits for a reply.
    B_ASSERT(!(inMode & (kAEQueueReply | kAEWaitReply)));
    
    AEDescriptor    reply;
    OSStatus        err;
    
    inMode |= kAENoReply;
    
    err = AESend(&inEvent, reply, inMode, kAENormalPriority, 
                 kAEDefaultTimeout, NULL, NULL);
    
    if (err != noErr)
    {
        AEObjectSupport::RethrowExceptionFromAppleEventReply(reply, err);
    }
}

// ------------------------------------------------------------------------------------------
/*  @a inEvent must already be fully constructed.
    
    For a list of possible values for @a inMode, consult 
    \<ApplicationServices/AEDataModel.h\>.
*/
void
AEEventBase::SendEvent(
    const AppleEvent&   inEvent,            //!< The Apple %Event to send.
    AEDesc&             outResult,          //!< The event's result.
    AESendMode          inMode /* = 0 */)   //!< The event's send mode.
{
    AEDescriptor    reply;
    OSStatus        err;
    
    err = AESend(&inEvent, reply, inMode, kAENormalPriority, 
                 kAEDefaultTimeout, NULL, NULL);
    
    if (err != noErr)
    {
        AEObjectSupport::RethrowExceptionFromAppleEventReply(reply, err);
    }
    
    err = AEGetKeyDesc(reply, keyAEResult, typeWildCard, &outResult);
    B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//  Event<kCoreEventClass, kAEQuitApplication>

// ------------------------------------------------------------------------------------------
AEEvent<kCoreEventClass, kAEQuitApplication>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mSaveOption(kAEAsk)
{
    AEReader    reader(inAppleEvent);
    
    reader.ReadKey<typeEnumeration>(keyAESaveOptions, mSaveOption, std::nothrow);
}


// ==========================================================================================
//  Event<kAECoreSuite, kAEClone>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAEClone>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mInsertPosition(kAEEnd)
{
    AEReader    reader(inAppleEvent);
    
    if (reader.Contains(keyAEInsertHere))
    {
        ReadInsertionLoc(inAppleEvent, mInsertPosition, mTarget);
    }
    
    reader.ReadDescKey<typeAERecord>(keyAEPropData, mProperties, std::nothrow);
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kAECoreSuite, kAEClone>::Update()
{   
    if (mNewObject != NULL)
    {
        mNewObject->MakeSpecifier(mWriter);
    }
}


// ==========================================================================================
//  Event<kAECoreSuite, kAEClose>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAEClose>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mSaveOption(kAEAsk)
{
    AEReader    reader(inAppleEvent);
    
    reader.ReadKey<typeFileURL>(keyAEFile, mObjectUrl, std::nothrow);
    reader.ReadKey<typeEnumeration>(keyAESaveOptions, mSaveOption, std::nothrow);
}


// ==========================================================================================
//  Event<kAECoreSuite, kAECountElements>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAECountElements>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mCount(0)
{
    AEReader    reader(inAppleEvent);
    
    if (!reader.ReadKey<typeType>(keyAEObjectClass, mObjectClass, std::nothrow))
        mObjectClass = cObject;
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kAECoreSuite, kAECountElements>::Update()
{
    mWriter.Write<typeSInt32>(mCount);
}


// ==========================================================================================
//  Event<kAECoreSuite, kAECreateElement>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAECreateElement>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mInsertPosition(kAEEnd)
{
    AEReader    reader(inAppleEvent);
    
    reader.ReadKey<typeType>(keyAEObjectClass, mObjectClass);
    
    if (reader.Contains(keyAEInsertHere))
    {
        ReadInsertionLoc(inAppleEvent, mInsertPosition, mTarget);
    }
    
    reader.ReadDescKey<typeAERecord>(keyAEPropData, mProperties, std::nothrow);
    reader.ReadDescKey<typeWildCard>(keyAEData, mData, std::nothrow);
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kAECoreSuite, kAECreateElement>::Update()
{   
    if (mNewObject != NULL)
    {
        mNewObject->MakeSpecifier(mWriter);
    }
}


// ==========================================================================================
//  Event<kAECoreSuite, kAEDelete>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAEDelete>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  Event<kAECoreSuite, kAEDoObjectsExist>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAEDoObjectsExist>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mExists(false)
{
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kAECoreSuite, kAEDoObjectsExist>::Update()
{
    mWriter.Write<typeBoolean>(mExists);
}


// ==========================================================================================
//  Event<kAECoreSuite, kAEGetData>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAEGetData>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
    AEReader    reader(inAppleEvent);
    
    if (!reader.ReadKey<typeType>(keyAERequestedType, mRequestedType, std::nothrow))
        mRequestedType = typeWildCard;
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kAECoreSuite, kAEGetData>::Update()
{
    // This function is overriden in this class in order to prevent the base class 
    // implementation (which write a null descriptor) from being called.
}


// ==========================================================================================
//  Event<kAECoreSuite, kAEMove>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAEMove>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mInsertPosition(kAEEnd)
{
    AEReader    reader(inAppleEvent);
    
    if (reader.Contains(keyAEInsertHere))
    {
        ReadInsertionLoc(inAppleEvent, mInsertPosition, mTarget);
    }
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kAECoreSuite, kAEMove>::Update()
{
    if (mNewObject != NULL)
    {
        mNewObject->MakeSpecifier(mWriter);
    }
}


// ==========================================================================================
//  Event<kCoreEventClass, kAEOpen>

// ------------------------------------------------------------------------------------------
AEEvent<kCoreEventClass, kAEOpen>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  Event<kCoreEventClass, kAEPrint>

// ------------------------------------------------------------------------------------------
/*! We attempt to read the new Panther-defined optional parameters, as documented in 
    Apple Tech Note 2082.
    
    The parameters are set up to be synthesized via a coercion handler.  Because of this, 
    we first attempt to retrieve the parameters via raw Apple %Event Manager calls, 
    instead of higher-level B calls, in order to avoid conflicting coercions.
    
    If the attempt fails, there can be two reasons:  (1) the parameters are absent, or 
    (2) they are present but the coercion failed.  The latter will occur on Jaguar;  in 
    this case we attempt to provide the coercion ourselves.
*/
AEEvent<kCoreEventClass, kAEPrint>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter), 
          mShowDialog(false)
{
    DescType    junkType;
    Size        junkSize;
    OSStatus    err;
    
    // Retrieve the print settings.
    
    PMPrintSettings settings;
    
    err = AEGetParamPtr(&inAppleEvent, keyAEPropData, kPMPrintSettingsAEType, 
                        &junkType, &settings, sizeof(settings), &junkSize);
    
    if (err == noErr)
    {
        mPrintSettings.reset(new PrintSettings(settings, from_copy));
    }
    
    // Retrieve the printer.
    
    PMPrinter   printer;
    
    err = AEGetParamPtr(&inAppleEvent, keyAEPropData, kPMPrinterAEType, 
                        &junkType, &printer, sizeof(printer), &junkSize);
    
    if (err == noErr)
    {
        mPrinter.reset(new Printer(printer, from_copy));
    }
    
    // Retrieve the "show dialog" flag.
    
    Boolean showDlg;
    
    err = AEGetParamPtr(&inAppleEvent, kPMShowPrintDialogAEType, typeBoolean, 
                        &junkType, &showDlg, sizeof(showDlg), &junkSize);
    
    if (err == noErr)
    {
        mShowDialog = showDlg;
    }
}

// ------------------------------------------------------------------------------------------
AEEvent<kCoreEventClass, kAEPrint>::~AEEvent()
{
}

#if 0
// ------------------------------------------------------------------------------------------
void
AEEvent<kCoreEventClass, kAEPrint>::ReadProperties(
    const AERecord& inProperties)
{
    AEReader        reader(inProperties);
    PrintSession    session;
    SInt32          copies, firstPage, lastPage, pagesAcross, pagesDown;
    bool            collate;
    
    if (mPrintSettings == NULL)
    {
        mPrintSettings.reset(new PrintSettings);
        
        session.Default(*mPrintSettings);
        
        // copies
        
        if (reader.ReadKey<kPMCopieAEType>(kPMCopiesAEKey, copies, std::nothrow))
            mPrintSettings->SetCopies(copies);
        
        // collating
        
        if (reader.ReadKey<kPMCollateAEType>(kPMCollateAEKey, collate, std::nothrow))
            mPrintSettings->SetCollate(collate);
        
        // starting page
        
        if (reader.ReadKey<kPMFirstPageAEType>(kPMFirstPageAEKey, firstPage, std::nothrow))
            mPrintSettings->SetFirstPage(firstPage);
        
        // ending page
        
        if (reader.ReadKey<kPMLastPageAEType>(kPMLastPageAEKey, lastPage, std::nothrow))
            mPrintSettings->SetLastPage(lastPage);
        
        // pages across
        
        if (reader.ReadKey<kPMLayoutAcrossAEType>(kPMLayoutAcrossAEKey, pagesAcross, std::nothrow))
            mPrintSettings->SetPagesAcross(pagesAcross);
        
        // pages down
        
        if (reader.ReadKey<kPMLayoutDownAEType>(kPMLayoutDownAEKey, pagesDown, std::nothrow))
            mPrintSettings->SetPagesDown(pagesDown);
        
        // requested print time
        
//      if (reader.ReadKey<kPMPrintTimeAEType>(kPMPrintTimeAEKey, printTime, std::nothrow))
//          mPrintSettings->SetPrintTime(printTime);
        
        // error handling
        
//      if (reader.ReadKey<kPMErrorHandlingAEType>(kPMErrorHandlingAEKey, errorHandling, std::nothrow))
//          mPrintSettings->SetErrorHandling(errorHandling);
        
        // fax number
        
//      if (reader.ReadKey<kPMFaxNumberAEType>(kPMFaxNumberAEKey, faxNumber, std::nothrow))
//          mPrintSettings->SetFaxNumber(faxNumber);
    }
    
    if (mPrinter == NULL)
    {
        // Note: We use typeUnicodeText instead of kPMTargetPrinterAEType in order to 
        //       avoid performing our own type coercion.
        
        String  name;
        
        if (reader.ReadKey<typeUTF16ExternalRepresentation>(kPMTargetPrinterAEKey, name, std::nothrow))
        {
            session.SetPrinter(name.cf_ref());
            
            mPrinter = new Printer(session.GetPrinter());
        }
    }
}
#endif


// ==========================================================================================
//  Event<kAECoreSuite, kAESave>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAESave>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
    AEReader    reader(inAppleEvent);
    
    reader.ReadKey<typeFileURL>(keyAEFile, mObjectUrl, std::nothrow);
    reader.ReadKey<typeUTF16ExternalRepresentation>(keyAEFileType, mObjectType, std::nothrow);
}


// ==========================================================================================
//  Event<kAECoreSuite, kAESetData>

// ------------------------------------------------------------------------------------------
AEEvent<kAECoreSuite, kAESetData>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
    AEReader    reader(inAppleEvent);
    
    reader.ReadDescKey<typeWildCard>(keyAEData, mData);
}


// ==========================================================================================
//  Event<kAEMiscStandards, kAERedo>

// ------------------------------------------------------------------------------------------
AEEvent<kAEMiscStandards, kAERedo>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  Event<kAEMiscStandards, kAERevert>

// ------------------------------------------------------------------------------------------
AEEvent<kAEMiscStandards, kAERevert>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  Event<kAEMiscStandards, kAESelect>

// ------------------------------------------------------------------------------------------
AEEvent<kAEMiscStandards, kAESelect>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  Event<kAEMiscStandards, kAEUndo>

// ------------------------------------------------------------------------------------------
AEEvent<kAEMiscStandards, kAEUndo>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  Event<kASAppleScriptSuite, kASCommentEvent>

// ------------------------------------------------------------------------------------------
AEEvent<kASAppleScriptSuite, kASCommentEvent>::AEEvent(
    const AppleEvent&   inAppleEvent, 
    AEWriter&           ioWriter)
        : AEEventBase(inAppleEvent, ioWriter)
{
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kASAppleScriptSuite, kASCommentEvent>::Send(
    const char*     inComment,                      //!< The comment.
    AESendMode      inMode /* = kAEDontExecute */)  //!< The event's send mode.
{
    Send(String(std::string(inComment), kCFStringEncodingASCII), inMode);
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kASAppleScriptSuite, kASCommentEvent>::Send(
    const String&   inComment,                      //!< The comment.
    AESendMode      inMode /* = kAEDontExecute */)  //!< The event's send mode.
{
    AEDescriptor    event;
    
    Make(inComment, event);
    
    SendEvent(event, inMode);
}

// ------------------------------------------------------------------------------------------
void
AEEvent<kASAppleScriptSuite, kASCommentEvent>::Make(
    const String&   inComment,      //!< The comment.
    AppleEvent&     outEvent)       //!< The constructed event.
{
    B::AEWriter writer(kASAppleScriptSuite, kASCommentEvent);
    
    writer << AEKey(keyDirectObject) << inComment;
    
    writer.Close(outEvent);
}


}   // namespace B
