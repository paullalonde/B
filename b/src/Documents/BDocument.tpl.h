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

// standard headers
#include <fstream>

// library headers
#include <boost/bind.hpp>

// B headers
#include "BBundle.h"
#include "BDocument.h"
#include "BDocumentWindow.h"
#include "BEvent.h"
#include "BException.h"
#include "BFileUtilities.h"
#include "BNavDialogs.h"
#include "BPrintSession.h"
#include "BPrintSettings.h"
#include "BSaveInfo.h"
#include "BUtility.h"
#include "BWindow.h"


namespace B {

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY>
Document<UNDO_POLICY, PRINT_POLICY>::Document(
    AEObjectPtr     inContainer,
    DescType        inClassID, 
    SInt32          inUniqueID)
        : AbstractDocument(inContainer, inClassID, inUniqueID), 
          mWindow(NULL), 
          mQuittingSaveOption(kAEAsk), mShowingCloseUI(false), mSaveClearsUndo(true), 
          mEventHandler(GetEventTarget()), 
          mUndoPolicy(GetEventTarget(), Bundle::Main()),
          mPrintPolicy(GetEventTarget(), Bundle::Main())
{
    InitEventHandler(mEventHandler);
    
    // register for signals
    
    mUndoPolicy.GetUndoSignal().connect(boost::bind(&DocType::UndoManagerUnmodified, this));
    mUndoPolicy.GetRedoSignal().connect(boost::bind(&DocType::UndoManagerModified, this));
    mUndoPolicy.GetActionSignal().connect(boost::bind(&DocType::UndoManagerModified, this));
    
    mPrintPolicy.SetPageSetupDialogInitCallback(
                    boost::bind(&DocType::PageSetupDialogInitCallback, this, _1));
    mPrintPolicy.SetPageSetupDialogResultCallback(
                    boost::bind(&DocType::PageSetupDialogResultCallback, this, _1, _2));
    mPrintPolicy.SetPrintDialogInitCallback(
                    boost::bind(&DocType::PrintDialogInitCallback, this, _1));
    mPrintPolicy.SetPrintDialogResultCallback(
                    boost::bind(&DocType::PrintDialogResultCallback, this, _1, _2));
    mPrintPolicy.GetPageFormatChangedSignal().connect(
                    boost::bind(&DocType::PageFormatChangedCallback, this, _1));
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY>
Document<UNDO_POLICY, PRINT_POLICY>::~Document()
{
    // Note:    We don't have to worry about disconnecting ourselves from the undo 
    //          policy's signals, because it has the same lifetime as us.
    //
    //          Ditto for the print policy's signals.
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::InitEventHandler(EventHandler& ioHandler)
{
    ioHandler.Add(this, &Document<UNDO_POLICY, PRINT_POLICY>::BCloseDocument);
    ioHandler.Add(this, &Document<UNDO_POLICY, PRINT_POLICY>::BSaveDocument);
    
    ioHandler.Init();
}

#pragma mark -

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> Url
Document<UNDO_POLICY, PRINT_POLICY>::GetUrl() const
{
    return (mUrl);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> String
Document<UNDO_POLICY, PRINT_POLICY>::GetDocumentType() const
{
    return (mDocumentType);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> String
Document<UNDO_POLICY, PRINT_POLICY>::GetDisplayName() const
{
    return (mDisplayName);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::RecalcDisplayName()
{
    OSStatus    err;
    
    if (!IsNew())
    {
        CFStringRef cfstr;
        
        err = LSCopyDisplayNameForURL(mUrl.cf_ref(), &cfstr);
        B_THROW_IF_STATUS(err);
        
        mDisplayName.assign(cfstr, from_copy);
    }
    else
    {
        mDisplayName = B::GetDisplayName(mName, !DoesFinderAlwaysDisplayExtensions());
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> Icon
Document<UNDO_POLICY, PRINT_POLICY>::GetIcon() const
{
    Url     url = GetUrl();
    Icon    icon;
    
    if (!url.Empty() && url.IsFileUrl())
    {
        // We have a file URL, so retrieve the icon directly.
        
        icon = url;
    }
    else
    {
        OSType              creator     = GetCreator();
        OSType              filetype    = 0;
        CFStringRef         extension   = NULL;
        const Bundle&       mainBundle  = Bundle::Main();
        Bundle::Iterator    it;
        
        it = mainBundle.FindDocumentTypeForTypeName(GetDocumentType());
        
        if (it != mainBundle.end())
        {
            if (!it->mExtensions.empty())
                extension = it->mExtensions[0].cf_ref();
            
            if (!it->mOSTypes.empty())
                filetype = it->mOSTypes[0];
        }
        
        icon.Assign(creator, filetype, extension, NULL);
    }
    
    return (icon);
}

// ------------------------------------------------------------------------------------------
/*! Make this Document the current one.

    Override to do what makes sense for a particular kind of Document. In
    most cases, this means selecting the main window for a Document.
*/
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::MakeCurrent()
{
    Window* window  = GetFrontMostDocumentWindow();
    
    B_ASSERT(window != NULL);
    
    window->Select();
}

// ------------------------------------------------------------------------------------------
/*! Return whether the document owns the given window.
*/
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::OwnsWindow(
    const Window*   inWindow) const
{
    return (mWindow == inWindow);
}


// ------------------------------------------------------------------------------------------
/*! Return whether the document owns the currently active document window.
*/
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::OwnsFrontmostDocumentWindow() const
{
    bool        ownsIt      = false;
    WindowRef   windowRef   = ActiveNonFloatingWindow();
    
    if (windowRef != NULL)
    {
        Window*     window  = Window::FromWindowRef(windowRef);
        
        if (window != NULL)
        {
            ownsIt = OwnsWindow(window);
        }
    }
    
    return (ownsIt);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> B::Window*
Document<UNDO_POLICY, PRINT_POLICY>::GetFrontMostDocumentWindow() const
{
    return (mWindow);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> const char*
Document<UNDO_POLICY, PRINT_POLICY>::GetWindowNibName() const
{
    return ("MainWindow");
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::MakeWindows(
    Nib*            /* inNib */)
{
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::ShowWindows()
{
    if (mWindow != NULL)
    {
        mWindow->Select();
        mWindow->Show(true);
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::InitDocument(
    const String&   inName,
    const String&   inDocumentType, 
    const AEDesc&   /* inProperties */, 
    const AEDesc&   /* inData */)
{
    mName           = inName;
    mDocumentType   = inDocumentType;
    
    RecalcDisplayName();
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::ReadDocument(
    const Url&      inDocumentUrl, 
    String&         ioDocumentType)
{
    // We only have built-in support for file URLs.  If you want to read from other kinds 
    // of URLs, you will have to override this function.
    
    if (inDocumentUrl.IsFileUrl())
    {
        ReadDocumentFromFile(inDocumentUrl, ioDocumentType);
    }
    else
    {
        std::auto_ptr<std::istream> istr(MakeDocumentReadStream(inDocumentUrl, 
                                                                ioDocumentType));
        
        ReadDocumentFromStream(*istr, ioDocumentType);
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::DoneReading(
    const Url&      inDocumentUrl, 
    const String&   inDocumentType)
{
    mUrl            = inDocumentUrl;
    mName           = mUrl.GetFilename();
    mDocumentType   = inDocumentType;
    
    mUndoPolicy.Clear();
    
    RecalcDisplayName();
    ClearModified();
}

// ------------------------------------------------------------------------------------------
/*! Resolves @a inFileUrl into a file system path, instantiates an @c std::ifstream 
    with the path, then calls ReadDocumentFromStream() with the stream.
    
    Derived classes may override this function to read the file some other way, such as 
    with Carbon File Manager APIs ( @c FSOpenFork() etc) or UNIX APIs ( @c open() etc).
*/
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::ReadDocumentFromFile(
    const Url&      inFileUrl,      //!< The document's URL, which must be a file: URL.
    String&         ioDocumentType) //!< The document's type.
{
    std::auto_ptr<std::istream> istr(MakeDocumentReadStream(inFileUrl, ioDocumentType));
    
    ReadDocumentFromStream(*istr, ioDocumentType);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> std::auto_ptr<std::istream>
Document<UNDO_POLICY, PRINT_POLICY>::MakeDocumentReadStream(
    const Url&      inDocumentUrl,          //!< The document's URL.
    const String&   /* inDocumentType */)   //!< The document's type.
    const
{
    std::auto_ptr<std::ifstream>    stream;
    
    if (inDocumentUrl.IsFileUrl())
    {
        Url         absUrl(inDocumentUrl.Absolute());
        FSRef       ref;
        std::string path;
        
        // This will throw if the path doesn't exist.
        absUrl.Copy(ref);
        
        inDocumentUrl.CopyPath(path);
        
        stream.reset(new std::ifstream(path.c_str()));
        
        if (!stream->is_open())
            B_THROW(OpenException());
    }
    else
    {
        B_THROW(UnsupportedUrlSchemeException());
    }
    
    return std::auto_ptr<std::istream>(stream);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> std::auto_ptr<std::ostream>
Document<UNDO_POLICY, PRINT_POLICY>::MakeDocumentWriteStream(
    SaveInfo&       ioSaveInfo)
    const
{
    std::auto_ptr<std::ofstream>    stream;
    Url                             documentUrl(ioSaveInfo.GetUrl());
    
    if (documentUrl.IsFileUrl())
    {
        std::string path;
        
        documentUrl.CopyPath(path);

        stream.reset(new std::ofstream(path.c_str()));
        
        if (!stream->is_open())
            B_THROW(OpenException());
    }
    else
    {
        B_THROW(UnsupportedUrlSchemeException());
    }
    
    return std::auto_ptr<std::ostream>(stream);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentToFile(
    SaveInfo&       ioSaveInfo)
{
    Url         fileUrl(ioSaveInfo.GetUrl());
    FSRef       fileRef;
    OSStatus    err;
    
    B_ASSERT(fileUrl.IsFileUrl());
    
    if (fileUrl.Copy(fileRef, std::nothrow))
    {
        WriteDocumentToExistingFile(ioSaveInfo);
    }
    else
    {
        bool    wasCreated  = CreateDocumentFile(fileUrl, fileRef);
        
        try
        {
            ioSaveInfo.SetFileRef(fileRef);
            WriteDocumentToNewFile(ioSaveInfo);
        }
        catch (...)
        {
            if (wasCreated)
                err = FSDeleteObject(&fileRef);
            
            throw;
        }
    }
    
    if (ioSaveInfo.GetWriteAttributes())
    {
        WriteFileAttributes(ioSaveInfo);
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentToNewFile(
    SaveInfo&       ioSaveInfo)
{
    WriteDocumentCommon(ioSaveInfo);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentToExistingFile(
    SaveInfo&       ioSaveInfo)
{
    B::Url  savedUrl    = ioSaveInfo.GetUrl();
    
    try
    {
        FSRef   fileRef = ioSaveInfo.GetFileRef();
        FSRef   tempRef;
        bool    extensionHidden;
        
        // If the file exists and we mustn't clobber it, then we need to perform 
        // a safe-save.
        
        StartSafeFileSave(fileRef, tempRef, extensionHidden);
        
        B::Url  tempUrl(tempRef);
        
        ioSaveInfo.SetUrl(tempUrl);
        
        WriteDocumentCommon(ioSaveInfo);
    
        ioSaveInfo.SetUrl(savedUrl);
        
        CompleteSafeFileSave(fileRef, tempRef, extensionHidden);
    }
    catch (...)
    {
        ioSaveInfo.SetUrl(savedUrl);
        throw;
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentCommon(
    SaveInfo&       ioSaveInfo)
{
    std::auto_ptr<std::ostream> ostr(MakeDocumentWriteStream(ioSaveInfo));

    WriteDocumentToStream(*ostr, ioSaveInfo);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::CreateDocumentFile(
    const Url&      inFileUrl, 
    FSRef&          outFileRef)
{
    FSRef           folderRef;
    HFSUniStr255    hfsName;
    bool            wasCreated;
    OSStatus        err;
    
    make_hfsstring(inFileUrl.TopPath().cf_ref(), hfsName);
    inFileUrl.PopPath().Copy(folderRef);
    
    err = FSMakeFSRefUnicode(&folderRef, hfsName.length, hfsName.unicode, 
                             GetApplicationTextEncoding(), &outFileRef);
    if (err != fnfErr)
        B_THROW_IF_STATUS(err);
        
    if (err == noErr)
    {
        wasCreated = false;
    }
    else
    {
        // The file exists already.
        err = FSCreateFileUnicode(&folderRef, hfsName.length, hfsName.unicode, 
                                  kFSCatInfoNone, NULL, &outFileRef, NULL);
        B_THROW_IF_STATUS(err);
        
        wasCreated = true;
    }
    
    return (wasCreated);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::StartSafeFileSave(
    const FSRef&                inDstRef, 
    FSRef&                      outTempFileRef, 
    bool&                       outExtensionHidden)
{
    // We need to save to a temporary file, then swap the two files' identities.
    // This works best when the two files are on the same volume, so we create the 
    // temporary file on the same volume as the destination file.
    
    FSVolumeRefNum      dstVolume;
    FSRef               tempItemsRef;
    LSItemInfoRecord    lsInfo;
    OSStatus            err;
    
    err = LSCopyItemInfoForRef(&inDstRef, kLSRequestExtensionFlagsOnly, &lsInfo);
    B_THROW_IF_STATUS(err);
    
    outExtensionHidden = ((lsInfo.flags & kLSItemInfoExtensionIsHidden) != 0);
    
    err = FSGetVRefNum(&inDstRef, &dstVolume);
    B_THROW_IF_STATUS(err);
    
    err = FSFindFolder(dstVolume, kTemporaryFolderType, true, &tempItemsRef);
    B_THROW_IF_STATUS(err);
    
    // Create a file in the temp folder that (1) has a unique name, and 
    // (2) has the pertinent attributes copied from the old version of 
    // the file.
    
    do
    {
        static bool     goodSeed    = false;
        char            tempBuff[64];
        String          tempStr;
        HFSUniStr255    tempName;
        
        if (!goodSeed)
        {
            srand(time(NULL));
            goodSeed = true;
        }
        
        snprintf(tempBuff, sizeof(tempBuff), "%u", (unsigned) (rand() % INT_MAX));
        tempStr.assign(tempBuff, kCFStringEncodingASCII);
        tempName.length = tempStr.copy(tempName.unicode, 255);
        
        err = FSCreateFileUnicode(&tempItemsRef, 
                                  tempName.length, tempName.unicode, 
                                  kFSCatInfoNone, NULL, 
                                  &outTempFileRef, NULL);
        if ((err != noErr) && (err != dupFNErr))
            B_THROW_STATUS(err);
        
    } while (err == dupFNErr);
    
    // Copy as many attributes as we can from the original file.
    
    err = FSCopyFileMgrAttributes(&inDstRef, &outTempFileRef, false);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::CompleteSafeFileSave(
    const FSRef&                inDstRef, 
    const FSRef&                inTempFileRef, 
    bool                        inExtensionHidden)
{
    // Now we need to exchange the temp file with the destination file.
    
    FSRef       newTempRef, newDstRef;
    OSStatus    err;
    
    err = FSExchangeObjectsCompat(&inTempFileRef, &inDstRef, &newTempRef, &newDstRef);
    B_THROW_IF(err != noErr, FSExchangeObjectsException(err, newTempRef, newDstRef));
    
    err = FSDeleteObject(&newTempRef);
    B_THROW_IF_STATUS(err);
    
    err = LSSetExtensionHiddenForRef(&inDstRef, inExtensionHidden);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteFileAttributes(
    SaveInfo&       ioSaveInfo)
{
    OSType              creator     = GetCreator();
    OSType              filetype    = 0;
    Bundle::Iterator    it;
    
    // Determine the file type from the document type.  We just grab the first file 
    // type in the document type's array of file types.  If the document type doesn't 
    // have any associated file types, we leave the file type as is (usually 0).
    
    it = Bundle::Main().FindDocumentTypeForTypeName(ioSaveInfo.GetContentType());
    
    if (it != Bundle::Main().end())
    {
        if (it->mOSTypes.size() > 0)
            filetype = it->mOSTypes[0];
    }
    
    if ((creator != 0) || (filetype != 0))
    {
        // Set the file type and creator.  Note that we allow this operation to fail 
        // silently, since this information isn't crucial for document binding 
        // under Mac OS X.
        
        FSRef       fileRef = ioSaveInfo.GetFileRef();
        OSStatus    err;
        
        err = FSChangeCreatorType(&fileRef, creator, filetype);
    }
}

// ------------------------------------------------------------------------------------------
/*! Reads the entire contents of the stream into an @c std::vector, then calls 
    ReadDocumentFromBuffer().
    
    Derived classes may override this function to read the stream in a manner better 
    adapted to the document's contents.
*/
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::ReadDocumentFromStream(
    std::istream&   ioStream,       //!< The document's input stream.
    String&         ioDocumentType) //!< The document's type.
{
    std::streamoff      length  = 0;
    std::vector<UInt8>  buffer;
    std::streampos      startPos(0);
    
    ioStream.clear();
    
    startPos = ioStream.tellg();
    
    if (ioStream.good())
    {
        // The stream supports seeking -- I hope...
        
        ioStream.seekg(0, std::ios_base::end);
        
        if (ioStream.good())
        {
            std::streampos  endPos  = ioStream.tellg();
            
            ioStream.seekg(startPos);
            
            if (ioStream.good())
                length = endPos - startPos;
        }
    }
    
    if (ioStream.good())
    {
        // The length is valid, so read the entire stream at once.
        
        buffer.resize(length);
        ioStream.read(reinterpret_cast<char*>(&buffer[0]), length);
        
        if (!ioStream.good())
        {
            if (ioStream.eof())
                B_THROW(EOFException());
            else
                B_THROW(ReadException());
        }
    }
    else
    {
        // The stream probably doesn't support seeking.  Read it in in chunks.
        
        B_ASSERT(0);    // UNIMPLEMENTED
    }
    
    ReadDocumentFromBuffer(buffer, ioDocumentType);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentToStream(
    std::ostream&   ioStream, 
    SaveInfo&       ioSaveInfo)     //!< The document's save information.
{
    std::vector<UInt8>  buffer;
    
    WriteDocumentToBuffer(buffer, ioSaveInfo);
    
    ioStream.write(reinterpret_cast<char*>(&buffer[0]), buffer.size());
    
    if (!ioStream.good())
        B_THROW(WriteException());
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::ReadDocumentFromBuffer(
    const std::vector<UInt8>&   /* inBuffer */,         //!< The document's contents.
    String&                     /* ioDocumentType */)   //!< The document's type.
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentToBuffer(
    std::vector<UInt8>& /* outBuffer */, 
    SaveInfo&           /* ioSaveInfo */)   //!< The document's save information.
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::AskUserToClose(
    bool                    inFromUser, 
    Window*                 inWindow, 
    NavAskSaveChangesAction inContext)
{
    DialogModality      modality(GetDocumentDialogModality(inWindow));
    B::AutoValue<bool>  savedShowingCloseUI(mShowingCloseUI, true);
    
    NavDialog::Enter(
        CreateAskSaveChangesDialog(
            modality, inFromUser, 
            static_cast<AskSaveChangesDialog::Context>(inContext)));
    
    return (modality.IsModal());
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY>
std::auto_ptr<AskSaveChangesDialog>
Document<UNDO_POLICY, PRINT_POLICY>::CreateAskSaveChangesDialog(
    const DialogModality&   inModality, 
    bool                    inFromUser, 
    AskSaveChangesDialog::Context   inContext)
{
    return (AskSaveChangesDialog::Make<AskSaveChangesDialog>(
                    inModality, GetDisplayName(), inContext,
                    boost::bind(
                        &DocType::AskSaveChangesDialogCallback, this, _1, _2,
                        DocumentSheetState(inModality.GetParent(), inFromUser))));
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::AskSaveChangesDialogCallback(
    const AskSaveChangesDialog&     /* inDialog */,
    AskSaveChangesDialog::Action    inAction, 
    const DocumentSheetState&       inState)
{
    HandleAskSaveChangesResult(inAction, inState);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandleAskSaveChangesResult(
    AskSaveChangesDialog::Action    inAction, 
    const DocumentSheetState&       inState)
{
    switch (inAction)
    {
    case AskSaveChangesDialog::kSave:
        {
            Event<kEventClassB, kEventBSaveDocument>    event(GetEventTarget(), 
                                                              inState.mWindow, 
                                                              inState.mFromUser);
            
            event.Post(kEventPriorityHigh);
        }
        break;
    
    case AskSaveChangesDialog::kDontSave:
        {
            if (inState.mFromUser)
                SendCloseAppleEvent(kAENo, Url(), kAEDontExecute);
            
            Event<kEventClassB, kEventBCloseDocument>   event(GetEventTarget());
            
            event.Post(kEventPriorityHigh);
            mShowingCloseUI = false;
        }
        break;
        
    case AskSaveChangesDialog::kCancel:
        NotifyActionCancelled();
        mShowingCloseUI = false;
        break;
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::AskUserToSave(
    SaveInfo&       ioInfo)
{
    DialogModality  modality(GetDocumentDialogModality(ioInfo.GetInvokingWindow()));
    
    ioInfo.SetFileName(GetName());
    NavDialog::Enter(CreateSaveDialog(modality, ioInfo));
    
    return (modality.IsModal());
}

// ------------------------------------------------------------------------------------------
/*! Factory function that creates the "Save" (ie PutFile) sheet for this 
    document.
*/
template <class UNDO_POLICY, class PRINT_POLICY>
std::auto_ptr<PutFileDialog>
Document<UNDO_POLICY, PRINT_POLICY>::CreateSaveDialog(
    const DialogModality&   inModality, 
    const SaveInfo&         inInfo)
{
    std::auto_ptr<PutFileDialog>    dialogPtr;
    
    dialogPtr = PutFileDialog::Make<PutFileDialog>(
                    inModality, String(), 
                    boost::bind(
                        &DocType::PutFileDialogCallback, this, _1, _2, _3, _4));
    
    dialogPtr->SetDocumentTypes(B::Bundle::Main(), inInfo.GetContentType());
    dialogPtr->SetSaveInfo(inInfo);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PutFileDialogCallback(
    const PutFileDialog&        /* inDialog */,
    bool                        inGoodSave,
    NavReplyRecord&             ioNavReply, 
    const SaveInfo&             inSaveInfo)
{
    HandleSaveResult(inGoodSave, ioNavReply, inSaveInfo);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandleSaveResult(
    bool                        inGoodSave,
    NavReplyRecord&             ioNavReply, 
    const SaveInfo&             inSaveInfo)
{
    mShowingCloseUI = false;
    
    if (inGoodSave)
    {
        SaveDocumentWithNavReply(ioNavReply, inSaveInfo);
    
        if (inSaveInfo.GetFromUser())
        {
            if (inSaveInfo.GetClosing())
                SendCloseAppleEvent(kAEYes, inSaveInfo.GetUrl(), kAEDontExecute);
            else
                SendSaveAppleEvent(inSaveInfo.GetUrl(), inSaveInfo.GetContentType(), 
                                   kAEDontExecute);
        }
        
        if (inSaveInfo.GetClosing())
        {
            Event<kEventClassB, kEventBCloseDocument>   event(GetEventTarget());
            
            event.Post(kEventPriorityHigh);
        }
    }
    else
    {
        NotifyActionCancelled();
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocumentToUrl(
    SaveInfo&       ioSaveInfo)
{
    ioSaveInfo.SetWriteAttributes(true);
    
    WriteDocument(ioSaveInfo);
    
    mUrl            = ioSaveInfo.GetUrl();
    mDocumentType   = ioSaveInfo.GetContentType();
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteDocument(
    SaveInfo&       ioSaveInfo)
{
    // We only have built-in support for file URLs.  If you want to read from other kinds 
    // of URLs, you will have to override this function.
    
    if (ioSaveInfo.GetUrl().IsFileUrl())
    {
        WriteDocumentToFile(ioSaveInfo);
    }
    else
    {
        B_THROW(UnsupportedUrlSchemeException());
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::SaveDocumentWithNavReply(
    NavReplyRecord& ioReply, 
    const SaveInfo& inSaveInfo)
{
    B_ASSERT(ioReply.validRecord);
    
    SaveInfo    saveInfo(inSaveInfo);
    Url         oldUrl(GetUrl());
    String      oldDocumentType(GetDocumentType());
    OSStatus    err;
    
    WriteDocumentToUrl(saveInfo);
    
    err = NavCompleteSave(&ioReply, kNavTranslateInPlace);
    B_THROW_IF_STATUS(err);
    
    WriteComplete(oldUrl, oldDocumentType);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WriteComplete(
    const Url&              inOldUrl, 
    const String&           inOldDocumentType)
{
    if (mSaveClearsUndo)
    {
        mUndoPolicy.Clear();
    }
    
    // Re-compute the display name.
    RecalcDisplayName();
    
    // If our URL represents a file, set its creator and file type.
    
    AbstractDocument::WriteComplete(inOldUrl, inOldDocumentType);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::AskUserToRevert(
    Window*                 inWindow)
{
    DialogModality  modality(GetDocumentDialogModality(inWindow));
    
    NavDialog::Enter(CreateAskDiscardChangesDialog(modality, true));
    
    return (modality.IsModal());
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY>
std::auto_ptr<AskDiscardChangesDialog>
Document<UNDO_POLICY, PRINT_POLICY>::CreateAskDiscardChangesDialog(
    const DialogModality&   inModality, 
    bool                    inFromUser)
{
    return (AskDiscardChangesDialog::Make<AskDiscardChangesDialog>(
                    inModality, GetDisplayName(),
                    boost::bind(
                        &DocType::AskDiscardChangesDialogCallback, this, _1, _2, 
                        DocumentSheetState(inModality.GetParent(), inFromUser))));
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::AskDiscardChangesDialogCallback(
    const AskDiscardChangesDialog&  /* inDialog */,
    bool                            inDiscard, 
    const DocumentSheetState&       inState)
{
    HandleAskDiscardChangesResult(inDiscard, inState);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandleAskDiscardChangesResult(
    bool                        inDiscard, 
    const DocumentSheetState&   /* inState */)
{
    if (inDiscard)
    {
        SendRevertAppleEvent();
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::BCloseDocument(
    Event<kEventClassB, kEventBCloseDocument>&      /* event */)
{
    Close();
    
    return (true);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> bool
Document<UNDO_POLICY, PRINT_POLICY>::BSaveDocument(
    Event<kEventClassB, kEventBSaveDocument>&       event)
{
    SaveInfo    saveInfo;
    
    saveInfo.SetInvokingWindow(Window::FromWindowRef(event.mWindow));
    
    if (IsNew())
    {
        // We don't have a file.  Display a UI asking the user to 
        // supply a file name.  Once that's done, we'll send ourselves 
        // a "Save" Apple %Event for the new file.
        
        saveInfo.SetFromUser(event.mFromUser);
        saveInfo.SetClosing(true);
        saveInfo.SetContentType(GetDocumentType());
        
        AskUserToSave(saveInfo);
    }
    else
    {
        // We have a file already, so there's no need to display a UI.
        
        SendCloseAppleEvent(kAEYes, Url(), kAEDontExecute);
        SaveDocument(saveInfo);
        
        Event<kEventClassB, kEventBCloseDocument>   closeEvent(GetEventTarget());
        
        closeEvent.Post(kEventPriorityHigh);
    }
    
    return (true);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::Quitting(
    OSType                  inSaveOption)
{
    B_ASSERT((inSaveOption == kAEAsk) || (inSaveOption == kAEYes));
    
    AbstractDocument::Quitting(inSaveOption);
    
    mQuittingSaveOption = inSaveOption;
    
    if (!mShowingCloseUI && IsModified() && OwnsFrontmostDocumentWindow())
    {
        CloseObject(inSaveOption);
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> DialogModality
Document<UNDO_POLICY, PRINT_POLICY>::GetDocumentDialogModality(
    Window*         inWindowHint)   //!< A hint as to the top-most document window.  May be @c NULL.
{
    if (inWindowHint == NULL)
        inWindowHint = GetFrontMostDocumentWindow();
    
    B_ASSERT(inWindowHint != NULL);
    
//  inWindowHint->Show(true);
//  inWindowHint->Select();
    
    return (DialogModality::Sheet(*inWindowHint));
}

#pragma mark -

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WindowActivated(Window* inWindow)
{
    if (!mShowingCloseUI && IsModified() && IsQuitting())
    {
        CloseDocument(mQuittingSaveOption, Url(), inWindow);
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::WindowDeactivated(Window* /* inWindow */)
{
}


#pragma mark -

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> String
Document<UNDO_POLICY, PRINT_POLICY>::GetName() const
{
    return (mName);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> size_t
Document<UNDO_POLICY, PRINT_POLICY>::CountElements(
    DescType    inElementType) const
{
    size_t  count;
    
    if (AEObject::DoesClassInheritFrom(inElementType, cWindow))
    {
        count = 1;
    }
//  else if (AEObject::DoesClassInheritFrom(inElementType, cFile))
//  {
//      count = HasFile() ? 1 : 0;
//  }
    else
    {
        count = AbstractDocument::CountElements(inElementType);
    }
    
    return (count);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> AEObjectPtr
Document<UNDO_POLICY, PRINT_POLICY>::GetElementByIndex(
    DescType        inElementType,
    size_t          inIndex) const
{
    AEObjectPtr elementObj;
    
    if (AEObject::DoesClassInheritFrom(inElementType, cWindow))
    {
        if (inIndex != 0)
            B_THROW(AENoSuchObjectException());
        elementObj = mWindow->GetAEObjectPtr();
    }
//  else if (AEObject::DoesClassInheritFrom(inElementType, cFile))
//  {
//      ### ACCESS FILE ELEMENTS HERE ###
//  }
    else
    {
        elementObj = AbstractDocument::GetElementByIndex(inElementType, inIndex);
    }
    
    return (elementObj);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PrintDocument(
    PrintSettings*  inPrintSettings,
    Printer*        inPrinter,
    bool            inShowDialog)
{
    mPrintPolicy.Print(inPrintSettings, inPrinter, inShowDialog);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PageSetupDialogInitCallback(
    PrintSession&                   ioPrintSession)
{
    HandlePageSetupDialogInit(ioPrintSession);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandlePageSetupDialogInit(
    PrintSession&                   ioPrintSession)
{
    ioPrintSession.SetModality(GetDocumentDialogModality(NULL));
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PageSetupDialogResultCallback(
    PrintSession&                   ioPrintSession,
    bool                            inAccepted)
{
    HandlePageSetupDialogResult(ioPrintSession, inAccepted);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandlePageSetupDialogResult(
    PrintSession&                   /* ioPrintSession */,
    bool                            /* inAccepted */)
{
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PrintDialogInitCallback(
    PrintSession&                   ioPrintSession)
{
    HandlePrintDialogInit(ioPrintSession);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandlePrintDialogInit(
    PrintSession&                   ioPrintSession)
{
    PrintSettings&  settings    = GetPrintPolicy().GetPrintSettings();
    size_t          minPage, maxPage;
    
    GetPrintingPageRange(minPage, maxPage, true);
    settings.SetPageRange(minPage, maxPage);
    settings.SetJobName(GetDisplayName());
    
    ioPrintSession.SetModality(GetDocumentDialogModality(NULL));
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PrintDialogResultCallback(
    PrintSession&                   ioPrintSession,
    bool                            inAccepted)
{
    HandlePrintDialogResult(ioPrintSession, inAccepted);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandlePrintDialogResult(
    PrintSession&                   ioPrintSession,
    bool                            inAccepted)
{
    if (inAccepted)
    {
        StartPrinting(ioPrintSession, GetPrintPolicy().GetPrintSettings(), 
                      GetPrintPolicy().GetPageFormat());
        DonePrinting();
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::GetPrintingPageRange(
    size_t&     outMinPage,
    size_t&     outMaxPage,
    bool        inForDialog)
{
    outMinPage  = 1;
    outMaxPage  = inForDialog ? static_cast<size_t>(kPMPrintAllPages) : 1;
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::CalcPrintingPageRange(
    PrintSession&               /* ioPrintSession */,
    const PrintSettings&        inPrintSettings,
    const PageFormat&           /* inPageFormat */,
    size_t&                     outStartPage,
    size_t&                     outEndPage)
{
    GetPrintingPageRange(outStartPage, outEndPage, false);
    
    outStartPage    = std::max(outStartPage, inPrintSettings.GetFirstPage());
    outEndPage      = std::min(outEndPage, inPrintSettings.GetLastPage());
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::StartPrinting(
    PrintSession&               ioPrintSession,
    const PrintSettings&        inPrintSettings,
    const PageFormat&           inPageFormat)
{
    size_t  startPage, endPage;
    
    CalcPrintingPageRange(ioPrintSession, inPrintSettings, inPageFormat, 
                          startPage, endPage);
    
    BeginPrintingDocument(ioPrintSession, inPrintSettings, inPageFormat, 
                          startPage, endPage);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::BeginPrintingDocument(
    PrintSession&               ioPrintSession, 
    const PrintSettings&        inPrintSettings,
    const PageFormat&           inPageFormat, 
    size_t                      inStartPage,
    size_t                      inEndPage)
{
    AutoPrintDocument   printDocument(ioPrintSession, inPrintSettings, inPageFormat);
    
    OSStatus    err = noErr;
    
    for (size_t page = inStartPage; (err == noErr) && (page <= inEndPage); ++page)
    {
        BeginPrintingPage(ioPrintSession, inPageFormat, page);
    }
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::BeginPrintingPage(
    PrintSession&               ioPrintSession, 
    const PageFormat&           inPageFormat, 
    size_t                      inPage)
{
    AutoPrintPage   printPage(ioPrintSession, inPageFormat);
    
    PrintPage(ioPrintSession, inPageFormat, inPage);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PrintPage(
    PrintSession&               /* ioPrintSession */, 
    const PageFormat&           /* inPageFormat */, 
    size_t                      /* inPage */)
{
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::PageFormatChangedCallback(
    PageFormat&                     ioPageFormat)
{
    HandlePageFormatChanged(ioPageFormat);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::HandlePageFormatChanged(
    PageFormat&                     /* ioPageFormat */)
{
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::UndoManagerModified()
{
    Modified();
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY> void
Document<UNDO_POLICY, PRINT_POLICY>::UndoManagerUnmodified()
{
    Unmodified();
}


}   // namespace B
