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
#include "BNavDialogs.h"

// system headers
#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

// project headers
#include "BAutoUPP.h"
#include "BAEDescriptor.h"
#include "BEvent.h"
#include "BEventCustomParams.h"
#include "BEventParams.h"
#include "BException.h"
#include "BFileUtilities.h"
#include "BNib.h"
#include "BUrl.h"
#include "BView.h"
#include "BWindowUtils.h"
#include "CFUtils.h"


// ### IMPORTANT: THE DEFINITION BELOW NEEDS TO MATCH THE ONE IN B.r ###
#define B_NAV_CUSTOM_AREA_ID    200

namespace
{
    struct MatchFilterName
    {
    public:
        
        MatchFilterName(const B::String& inFilterName)
            : mFilterName(inFilterName) {}
        
        bool    operator () (const B::NavReplyDialog::PopupMenuItem& value) const
        {
            return ((value.GetFilterKind() == B::NavReplyDialog::kFilterName) && 
                    (CFStringCompare(value.GetName().cf_ref(), mFilterName.cf_ref(), kCFCompareCaseInsensitive) == 0));
        }
        
        bool    operator () (const B::Bundle::DocumentType& value) const
        {
            return (CFStringCompare(value.mName.cf_ref(), mFilterName.cf_ref(), kCFCompareCaseInsensitive) == 0);
        }
        
    private:
        const B::String&    mFilterName;
    };


    class AppendDocType
    {
    public:
        
        // constructor
        AppendDocType(
            const B::Bundle&    inBundle, 
            CFStringRef         inTable)
                : mBundle(inBundle), mTable(inTable)  {}
        
        // invocation
        B::NavReplyDialog::PopupMenuItem
        operator () (const B::Bundle::DocumentType& value) const;
        
    private:
        
        // member variables
        const B::Bundle&    mBundle;
        CFStringRef         mTable;
    };

    B::NavReplyDialog::PopupMenuItem
    AppendDocType::operator () (const B::Bundle::DocumentType& value) const
    {
        return (B::NavReplyDialog::PopupMenuItem(value, mBundle, mTable));
    }

    static CFStringRef
    ExtractPopupMenuItemText(
        const B::NavReplyDialog::PopupMenuItem& value)
    {
        return (value.GetLocName().cf_ref());
    }
}


namespace B {


// ==========================================================================================
//  NavDialog

#pragma mark -
#pragma mark * NavDialog *

// ------------------------------------------------------------------------------------------
/*! Doesn't actually create the dialog (that is done in Display()), but sets up all 
    member variables to reasonable values.
*/
NavDialog::NavDialog()
    : mDeleteOnDismiss(false), mUserAction(kNavUserActionNone), mStarted(false), 
      mNavDialog(NULL)
{
    OSStatus    err;
    
    err = NavGetDefaultDialogCreationOptions(&mOptions);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
NavDialog::~NavDialog()
{
    // The @c NavDialogRef may have already been disposed of (within HandleTerminate()), 
    // so we need to watch out for that.
    
    if (mNavDialog != NULL)
        NavDialogDispose(mNavDialog);
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetModality(
    const DialogModality&   inModality) //!< The dialog's modality.
{
    mOptions.modality       = inModality.GetModality();
    mOptions.parentWindow   = inModality.GetParent();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetDeleteOnDismiss(bool inDeleteOnDismiss)
{
    mDeleteOnDismiss = inDeleteOnDismiss;
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetFlags(
    NavDialogOptionFlags    flags)  //!< The flags to set.
{
    mOptions.optionFlags |= flags;
}

// ------------------------------------------------------------------------------------------
void
NavDialog::ClearFlags(
    NavDialogOptionFlags    flags)  //!< The flags to clear.
{
    mOptions.optionFlags &= ~flags;
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetLocation(
    const Point&    pt) //!< The location of the dialog's top left corner.
{
    mOptions.location = pt;
}

// ------------------------------------------------------------------------------------------
/*! @note   It's not clear whether this is ever used under Aqua.
*/
void
NavDialog::SetAppName(
    const String&   name)   //!< The name.  Should be localised.
{
    mAppName = name;
    mOptions.clientName = mAppName.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetTitle(
    const String&   title)  //!< The title.  Should be localised.
{
    mTitle = title;
    mOptions.windowTitle = mTitle.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetMessage(
    const String&   message)    //!< The message.  Should be localised.
{
    mMessage = message;
    mOptions.message = mMessage.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetFileName(
    const String&   fileName)   //!< The default file name.  Should be localised.
{
    mSaveFileName = fileName;
    mOptions.saveFileName = mSaveFileName.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetActionButtonLabel(
    const String&   label)  //!< The label.  Should be localised.
{
    mActionLabel = label;
    mOptions.actionButtonLabel = mActionLabel.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetCancelButtonLabel(
    const String&   label)  //!< The label.  Should be localised.
{
    mCancelLabel = label;
    mOptions.cancelButtonLabel = mCancelLabel.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
NavDialog::SetPopupStrings(
    OSPtr<CFArrayRef>   inPopupStrings) //!< The contents of the pop-up menu.  Should be localised.
{
    mPopupStrings = inPopupStrings;
    mOptions.popupExtension = inPopupStrings;
}

// ------------------------------------------------------------------------------------------
/*! The prefs key is used to distinguish between dialogs for the purpose of storing 
    configuration information, so that for example the size & location of an "Open File" 
    dialog can be different from the size & location of a "Open Dictionary" dialog.
*/
void
NavDialog::SetPrefsKey(
    UInt32  key)    //!< The key identifying a particular instance of a Nav dialog.
{
    mOptions.preferenceKey = key;
}

// ------------------------------------------------------------------------------------------
NavEventUPP
NavDialog::GetEventUPP()
{
    static AutoNavEventUPP  upp(NavReplyDialog::NavEventProc);
    
    return (upp);
}

// ------------------------------------------------------------------------------------------
void
NavDialog::Enter()
{
    B_ASSERT(mNavDialog == NULL);
    
    try
    {
        OSStatus    err;
        
        mUserAction = kNavUserActionNone;
        mNavDialog  = CreateDialog();
        
        err = NavDialogRun(mNavDialog);
        B_THROW_IF_STATUS(err);
    }
    catch (...)
    {
        if (mNavDialog != NULL)
        {
            NavDialogDispose(mNavDialog);
            mNavDialog = NULL;
        }
        
        mStarted = false;
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
NavUserAction
NavDialog::RunDialog()
{
    // Force the modality to application-modal.
    SetModality(DialogModality::Modal());
    
    // Ensure the dialog doesn't get released before Run() returns.
    SetDeleteOnDismiss(false);
    
    // Enter the dialog & wait for dismissal.
    Enter();
    
    // Return the dismissing action.
    return (GetUserAction());
}

// ------------------------------------------------------------------------------------------
/*! Prior to dialog dismissal, this function will return @c kNavUserActionNone.
*/
NavUserAction
NavDialog::GetUserAction() const
{
    return (mUserAction);
}

// ------------------------------------------------------------------------------------------
void
NavDialog::DialogStarting(NavCBRec& /* ioParams */)
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavDialog::DialogEnding()
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavDialog::HandleStart(NavCBRec& ioParams)
{
    try
    {
        mStarted = true;
        
        // Any kind of modality (except of course kWindowModalityNone) implies 
        // that processing must halt for user input.
        
        if (mOptions.modality != kWindowModalityNone)
        {
            InteractWithUserDialog().Run();
        }
        
        // If we are a sheet, bring our parent window to the front, then notify it 
        // so it can save its state.
        
        if (mOptions.modality == kWindowModalityWindowModal)
        {
            SelectWindow(mOptions.parentWindow);
            if (!IsWindowVisible(mOptions.parentWindow))
                ShowWindow(mOptions.parentWindow);
        }
        
        DialogStarting(ioParams);
    }
    catch (InteractionTimeoutException&)
    {
        Cancel();
        Terminate();
        
        mStarted = false;
        
        throw;
    }
    catch (...)
    {
        Terminate();
        
        mStarted = false;
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
void
NavDialog::HandleTerminate()
{
    DialogEnding();
    NavDialogDispose(mNavDialog);
    
    mNavDialog  = NULL;
    mStarted    = false;
    
    if (mDeleteOnDismiss)
    {
        delete this;
    }
}

// ------------------------------------------------------------------------------------------
void
NavDialog::HandleUserAction(
    NavUserAction   )   //!< The user action.
{
}

// ------------------------------------------------------------------------------------------
void
NavDialog::HandleEvent(
    NavEventCallbackMessage inSelector, 
    NavCBRec&               ioParams)
{
    switch (inSelector)
    {
    case kNavCBEvent:
        break;
    
    case kNavCBStart:
        HandleStart(ioParams);
        break;
    
    case kNavCBTerminate:
        HandleTerminate();
        break;
    
    case kNavCBUserAction:
        mUserAction = ioParams.userAction;
        HandleUserAction(mUserAction);
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
NavDialog::Cancel()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlCancel, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavDialog::Accept()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlAccept, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavDialog::Terminate()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlTerminate, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
pascal void
NavDialog::NavEventProc(
    NavEventCallbackMessage callBackSelector,
    NavCBRecPtr             callBackParms,
    NavCallBackUserData     callBackUD)
{
    NavDialog*  dialog  = reinterpret_cast<NavDialog*>(callBackUD);
    
    try
    {
        dialog->HandleEvent(callBackSelector, *callBackParms);
    }
    catch (...)
    {
        // Prevent exceptions from propagating
    }
}


// ==========================================================================================
//  NavReplyDialog

#pragma mark -
#pragma mark * NavReplyDialog *

// ------------------------------------------------------------------------------------------
/*! Doesn't actually create the dialog (that is done in Display()), but sets up all 
    member variables to reasonable values.
*/
NavReplyDialog::NavReplyDialog()
    : mCurrentFilterKind(kFilterAllFiles), mCustomViewWindow(NULL), 
      mDITLHandle(NULL), mUserPaneIndex(0), mUserPane(NULL)
{
    // The filter array is empty, so hide the file type pop-up for now.
    SetFlags(kNavNoTypePopup);
}

// ------------------------------------------------------------------------------------------
NavReplyDialog::~NavReplyDialog()
{
    if (mCustomViewWindow != NULL)
        DisposeWindow(mCustomViewWindow);
    
    if (mDITLHandle != NULL)
        HPurge(mDITLHandle);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SetPopupMenuContents(PopupMenuContentsVector& inContents)
{
    mPopupMenuContents = inContents;
    
    if (!mPopupMenuContents.empty())
    {
        std::vector<CFStringRef>    strVec;
        
        strVec.reserve(inContents.size());
        std::transform(inContents.begin(), inContents.end(), std::back_inserter(strVec), 
                       ExtractPopupMenuItemText);
        
        OSPtr<CFArrayRef>   extensions(CFArrayCreate(NULL, 
                                            reinterpret_cast<const void **>(&strVec[0]), 
                                            strVec.size(), 
                                            &kCFTypeArrayCallBacks), 
                                       from_copy);
        
        SetPopupStrings(extensions);
        
        // Make the pop-up menu visible if there's more than one item in it.
        
        if (mPopupMenuContents.size() > 1)
            ClearFlags(kNavNoTypePopup);
        else
            SetFlags(kNavNoTypePopup);
    }
    else
    {
        SetFlags(kNavNoTypePopup);
        SetPopupStrings(OSPtr<CFArrayRef>());
    }
    
    // Initially filter on all items in the menu.
    FilterAllReadableFiles();
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::FilterAllFiles()
{
    SelectFilter(kFilterAllFiles);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::FilterAllReadableFiles()
{
    SelectFilter(kFilterAllReadableFiles);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::FilterByName(
    const String&   inFilterName)   //!< The filter name to use for matching of files.
{
    SelectFilter(kFilterName, inFilterName);
}

// ------------------------------------------------------------------------------------------
/*! In the current implementation, @a inViewName is the name of a window defined 
    in @a inNib.  The "custom view" in the Nav dialog is constructed by moving all 
    subviews of the window's content view into a user pane control residing in the 
    dialog.  The window must be sized to the minimum size of the custom area, since 
    this size is used during custom area negotiation.  Needless to say, all subviews 
    must react properly when moved from one window to another.
    
    @note   This function must be called @b before the dialog is displayed.
*/
void
NavReplyDialog::SetCustomView(
    Nib&        inNib,      //!< The nib from which to load the view.
    const char* inViewName) //!< The name of the view to load.
{
    B_ASSERT(!IsStarted());
    
    Handle      ditlH   = NULL;
    WindowRef   window  = NULL;
    
    try
    {
        OSStatus    err;
        
        if ((ditlH = GetResource('DITL', B_NAV_CUSTOM_AREA_ID)) == NULL)
        {
            if ((err = ResError()) == noErr)
                err = resNotFound;
            B_THROW_STATUS(err);
        }
        
        HNoPurge(ditlH);
        
        window = inNib.CreateWindow(inViewName);
        
        mDITLHandle         = ditlH;
        mCustomViewWindow   = window;
    }
    catch (...)
    {
        if (window != NULL)
            DisposeWindow(window);
        
        if (ditlH != NULL)
            HPurge(ditlH);
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
/*! This is the main object-filtering function.  It basically just dispatches to the more 
    specialised filtering functions, depending on the current file filter.
*/
bool
NavReplyDialog::FilterObject(
    const AEDesc&               inItem,         //!< Contains one object to filter.  This may or may not be a file system object.
    const NavFileOrFolderInfo&  /* inInfo */,   //!< Old-style NavServices information about the file.  Unfortunately, it doesn't have all the information we need so we must call @c LSCopyItemInfoForRef anyway.
    NavFilterModes              inFilterMode)   //!< Specifies where this object will appear in the dialog.
{
    FSRef       ref;
    OSStatus    err;
    
    // If the retrieval fails, then in all probability inItem doesn't represent a file 
    // system object, so return true since we don't know how to deal with it.
    
    err = DescParam<typeFSRef>::Get(inItem, ref, std::nothrow);
    if (err != noErr)
        return (true);
    
    bool    showIt  = false;
    
    try
    {
        // Retrieve pertinent info about the file, namely extension, file type, and flags.
        
        const LSRequestedInfo   kReqInfo    = kLSRequestExtension | 
                                              kLSRequestTypeCreator | 
                                              kLSRequestAllFlags;
        LSItemInfoRecord        info;
        OSPtr<CFStringRef>      extensionStr;
        
        err = LSCopyItemInfoForRef(&ref, kReqInfo, &info);
        B_THROW_IF_STATUS(err);
        
        if (info.extension != NULL)
            extensionStr.reset(info.extension, from_copy);
        
        // Dispatch to the other filtering functions.
        
        switch (mCurrentFilterKind)
        {
        case kFilterAllFiles:
            showIt = FilterObjectByAllFiles(ref, info, inFilterMode);
            break;
            
        case kFilterAllAppFiles:
        case kFilterAllReadableFiles:
            showIt = FilterObjectByAllReadableFiles(ref, info, inFilterMode);
            break;
            
        case kFilterName:
            showIt = FilterObjectByFilterName(ref, info, inFilterMode);
            break;
            
        default:
            showIt = false;
            break;
        }
    }
    catch (...)
    {
        showIt = false;
    }
    
    return (showIt);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation always returns true.
*/
bool
NavReplyDialog::FilterObjectByAllFiles(
    const FSRef&                /* inRef */,        //!< Contains the file system object to filter.
    const LSItemInfoRecord&     /* inInfo */,       //!< Pertinent info about @a inRef.
    NavFilterModes              /* inFilterMode */) //!< Specifies where this object will appear in the dialog.
{
    return (true);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation always returns true.
*/
bool
NavReplyDialog::FilterObjectByAllReadableFiles(
    const FSRef&                /* inRef */,        //!< Contains the file system object to filter.
    const LSItemInfoRecord&     /* inInfo */,       //!< Pertinent info about @a inRef.
    NavFilterModes              /* inFilterMode */) //!< Specifies where this object will appear in the dialog.
{
    return (true);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation always returns true.
*/
bool
NavReplyDialog::FilterObjectByFilterName(
    const FSRef&                /* inRef */,        //!< Contains the file system object to filter.
    const LSItemInfoRecord&     /* inInfo */,       //!< Pertinent info about @a inRef.
    NavFilterModes              /* inFilterMode */) //!< Specifies where this object will appear in the dialog.
{
    return (true);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SelectFilter(
    FilterKind              inFilterKind, 
    const String&           inFilterName /* = String() */)
{
    // The dialog is started and we have a pop-up menu, so tell NavServices to 
    // update the chosen item in the menu.  It will end up calling our 
    // PopupMenuSelect() function, from which we'll update our notion of the 
    // current filter.
    
    bool    canSelectMenuItem   = (IsStarted() && 
                                  !(GetCreationOptions().optionFlags & kNavNoTypePopup) && 
                                  !mPopupMenuContents.empty());
    
    PopupMenuContentsVector::const_iterator it  = mPopupMenuContents.end();
    NavMenuItemSpec                         spec;
    
    BlockZero(&spec, sizeof(spec));
    
    spec.version = kNavMenuItemSpecVersion;
    
    switch (inFilterKind)
    {
    case kFilterAllFiles:
        if (canSelectMenuItem)
        {
            SelectAllType(kNavAllFiles);
        }
        else
        {
            PopupMenuSelect(spec);
            if (IsStarted())
                BrowserRedraw();
        }
        break;
        
    case kFilterAllAppFiles:
        // UNIMPLEMENTED
        break;
        
    case kFilterAllReadableFiles:
        // We implement "All Readable Documents" ourselves, so can't just call 
        // NavServices.
        it = std::find_if(mPopupMenuContents.begin(), mPopupMenuContents.end(), 
                    boost::bind(
                        std::equal_to<FilterKind>(),
                        boost::bind(&PopupMenuItem::GetFilterKind, _1),
                        kFilterAllReadableFiles));
        break;
        
    case kFilterName:
        it = std::find_if(mPopupMenuContents.begin(), mPopupMenuContents.end(), 
                          MatchFilterName(inFilterName));
        break;
        
    default:
        it = mPopupMenuContents.end();
        break;
    }
    
    if (it != mPopupMenuContents.end())
    {
        spec.menuCreator    = 'extn';
        spec.menuType       = it - mPopupMenuContents.begin();
        
        if (canSelectMenuItem)
        {
            SelectCustomType(spec);
        }
        else
        {
            PopupMenuSelect(spec);
            if (IsStarted())
                BrowserRedraw();
        }
    }
}

// ------------------------------------------------------------------------------------------
/*! This function is really just a hook to implement previewing.  We don't really know at 
    this point what previewing entails, so it's pointless to try something more elaborate 
    for now.
*/
bool
NavReplyDialog::Preview(
    NavCBRecPtr     /* ioParams */)
{
    return (false);
}

// ------------------------------------------------------------------------------------------
View*
NavReplyDialog::FindCustomViewByID(int inID) const
{
    return (FindCustomViewByID(Bundle::Main().PackageCreator(), inID));
}

// ------------------------------------------------------------------------------------------
View*
NavReplyDialog::FindCustomViewByID(OSType inSignature, int inID) const
{
    B_ASSERT(IsStarted());
    B_ASSERT(mUserPaneIndex != 0);
    
    WindowRef   window  = NavDialogGetWindow(GetDialogRef());
    DialogRef   dialog  = GetDialogFromWindow(window);
    HIViewID    viewID  = { inSignature, inID };
    HIViewRef   userPane, viewRef;
    View*       view;
    OSStatus    err;
    
    GetDialogItemAsControl(dialog, mUserPaneIndex, &userPane);
    
    err = HIViewFindByID(userPane, viewID, &viewRef);
    B_THROW_IF_STATUS(err);
    
    B_ASSERT(viewRef != NULL);
    
    view = View::GetViewFromRef(viewRef, std::nothrow);
    
    if (view == NULL)
    {
        view = ViewFactory::Default().Instantiate(viewRef);
        B_ASSERT(view != NULL);
    }
    
    return (view);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::NegotiateCustomArea(Size& ioSize)
{
    if (mCustomViewWindow != NULL)
    {
        ::Rect      qdBounds;
        OSStatus    err;
        
        err = GetWindowBounds(mCustomViewWindow, kWindowGlobalPortRgn, &qdBounds);
        B_THROW_IF_STATUS(err);
        
        Rect    bounds(qdBounds);
        
        // check to see if this is the first round of negotiations:
        if ((ioSize.width == 0) && (ioSize.height == 0))
        {
            // it is, so tell NavServices what dimensions we want:
            ioSize = bounds.size;
        }
        else
        {
            // we are in the middle of negotiating:
            if (mLastTrySize.width != ioSize.width)
                if (ioSize.width < bounds.size.width)   // is NavServices width too small for us?
                    ioSize.width = bounds.size.width;

            if (mLastTrySize.width != ioSize.height)    // is NavServices height too small for us?
                if (ioSize.height < bounds.size.height)
                    ioSize.height = bounds.size.height;
        }
        
        // remember our last size so the next time we can re-negotiate:
        mLastTrySize = ioSize;
    }
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::DialogResized(Rect& ioCustomBounds, Rect& /* ioPreviewBounds */)
{
    if (IsStarted() && (mUserPaneIndex != 0))
    {
        PlaceCustomView(ioCustomBounds);
    }
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::PlaceCustomView(const Rect& inNewFrame)
{
    B_ASSERT(mUserPaneIndex != 0);
    
    WindowRef   window  = NavDialogGetWindow(GetDialogRef());
    DialogRef   dialog  = GetDialogFromWindow(window);
    
    B_ASSERT(dialog != NULL);

    MoveDialogItem(dialog, mUserPaneIndex, static_cast<short>(inNewFrame.origin.x), static_cast<short>(inNewFrame.origin.y));
    SizeDialogItem(dialog, mUserPaneIndex, static_cast<short>(inNewFrame.size.width), static_cast<short>(inNewFrame.size.height));
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::LocationChanged(const AEDesc& /* inNewLocation */)
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::GoneToDesktop()
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::PopupMenuSelect(const NavMenuItemSpec& inMenuItemSpec)
{
    switch (inMenuItemSpec.menuCreator)
    {
    case 'extn':
        // Custom item.  inMenuItemSpec.menuType contains the index into our array of 
        // doc type indices.
        B_ASSERT((inMenuItemSpec.menuType >= 0) && (inMenuItemSpec.menuType < mPopupMenuContents.size()));
        mCurrentFilterKind = mPopupMenuContents[inMenuItemSpec.menuType].GetFilterKind();
        if (mCurrentFilterKind == kFilterName)
            mCurrentFilterName = mPopupMenuContents[inMenuItemSpec.menuType].GetName();
        break;
        
    case 0:
        // Standard item.  Currently we only support kNavAllFiles, so hard-code it.
        mCurrentFilterKind = kFilterAllFiles;
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SelectionChanged(const AEDescList& /* inNewSelection */)
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::AdjustPreview(bool /* visible */)
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SelectionOpened()
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::DialogAccepted()
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::DialogCancelled()
{
    // Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
bool
NavReplyDialog::RunReplyDialog(
    NavUserAction   inConfirmAction, 
    AutoNavReply&   outNavReply)
{
    NavUserAction   action  = RunDialog();
    bool            good    = (action == inConfirmAction);
    
    if (good)
    {
        GetNavReply(outNavReply);
        
        if (!outNavReply.get().validRecord)
        {
            // The returned action says "OK", but the nav reply isn't valid.  Fallback 
            // to "Cancel".
            
            outNavReply.clear();
            good = false;
        }
    }
    else
    {
        outNavReply.clear();
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::HandleUserAction(
    NavUserAction   inUserAction)   //!< The user action.
{
    AutoNavReply    autoNavReply;
    
    switch (inUserAction)
    {
    case kNavUserActionOpen:
    case kNavUserActionSaveAs:
    case kNavUserActionChoose:
    case kNavUserActionNewFolder:
        GetNavReply(autoNavReply);
        break;
        
    default:
        break;
    }
    
    HandleReply(inUserAction, const_cast<NavReplyRecord&>(autoNavReply.get()));
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::GetNavReply(
    AutoNavReply&   outNavReply)    //!< The nav reply.
    const
{
    B_ASSERT(GetDialogRef() != NULL);
    
    outNavReply.reset(GetDialogRef());
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::HandleEvent(
    NavEventCallbackMessage inSelector, 
    NavCBRec&               ioParams)
{
    switch (inSelector)
    {
    case kNavCBCustomize:
        {
            Size    size(ioParams.customRect.right, ioParams.customRect.bottom);
            
            NegotiateCustomArea(size);
            
            ioParams.customRect.right   = static_cast<short>(size.width);
            ioParams.customRect.bottom  = static_cast<short>(size.height);
        }
        break;
    
    case kNavCBAdjustRect:
        {
            Rect    custom(ioParams.customRect);
            Rect    preview(ioParams.previewRect);
            
            DialogResized(custom, preview);
            
            ioParams.customRect     = custom;
            ioParams.previewRect    = preview;
        }
        break;
        
    case kNavCBNewLocation:
        LocationChanged(*reinterpret_cast<AEDesc*>(ioParams.eventData.eventDataParms.param));
        break;
        
    case kNavCBShowDesktop:
        GoneToDesktop();
        break;
        
    case kNavCBSelectEntry:
        SelectionChanged(*reinterpret_cast<AEDescList*>(ioParams.eventData.eventDataParms.param));
        break;
        
    case kNavCBPopupMenuSelect:
        PopupMenuSelect(*reinterpret_cast<const NavMenuItemSpec*>(ioParams.eventData.eventDataParms.param));
        break;
        
    case kNavCBAdjustPreview:
        AdjustPreview(*reinterpret_cast<Boolean*>(ioParams.eventData.eventDataParms.param));
        break;
        
    case kNavCBAccept:
        DialogAccepted();
        break;
        
    case kNavCBCancel:
        DialogCancelled();
        break;
        
    case kNavCBOpenSelection:
        SelectionOpened();
        break;
    
    default:
        NavDialog::HandleEvent(inSelector, ioParams);
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::DialogStarting(NavCBRec& ioParams)
{
    NavDialog::DialogStarting(ioParams);
    
    if (mCustomViewWindow != NULL)
    {
        Rect    frame(ioParams.customRect);
        
        InitCustomView(frame);
    }
    
    // The dialog has started, so we can issue NavCustomControl calls to tell the dialog 
    // which pop-up menu item to select, and also how to filter files in the browser 
    // list.
    
    SelectFilter(mCurrentFilterKind, mCurrentFilterName);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::InitCustomView(const Rect& inFrame)
{
    WindowRef   window  = NavDialogGetWindow(GetDialogRef());
    DialogRef   dialog  = GetDialogFromWindow(window);
    OSStatus    err;
    
    B_ASSERT(window != NULL);
    B_ASSERT(mDITLHandle != NULL);
    
    // Load the DITL containing the user pane control.
    
    err = NavCustomControl(GetDialogRef(), kNavCtlAddControlList, mDITLHandle);
    B_THROW_IF_STATUS(err);
    
    err = NavCustomControl(GetDialogRef(), kNavCtlGetFirstControlID, &mUserPaneIndex);
    B_THROW_IF_STATUS(err);
    
    mUserPaneIndex++;
    GetDialogItemAsControl(dialog, mUserPaneIndex, &mUserPane);
    
    // Move & resize the user pane so that it fills the custom area.
    
    PlaceCustomView(inFrame);
    
    // Move all subview of the custom view window's content view under the 
    // user pane (and therefore in the Nav dialog).
    
    HIViewRef   contentView = WindowUtils::GetContentView(mCustomViewWindow);
    Rect        userPaneFrame;
    
    err = HIViewGetFrame(mUserPane, &userPaneFrame);
    B_THROW_IF_STATUS(err);
    
    ViewUtils::ReverseSubviewIterator   it;
    
    while ((it = ViewUtils::rbegin(contentView)) != ViewUtils::rend(contentView))
    {
        Rect    frame;
        
        err = HIViewGetFrame(*it, &frame);
        B_THROW_IF_STATUS(err);
        
        frame += userPaneFrame.origin;
        
        err = HIViewRemoveFromSuperview(*it);
        B_THROW_IF_STATUS(err);
        
        err = HIViewAddSubview(mUserPane, *it);
        B_THROW_IF_STATUS(err);
        
        err = HIViewSetFrame(*it, &frame);
        B_THROW_IF_STATUS(err);
    }
    
    View::InstantiateSubviews(mUserPane, ViewFactory::Default());
    
    DisposeWindow(mCustomViewWindow);
    mCustomViewWindow = NULL;
}

// ------------------------------------------------------------------------------------------
NavObjectFilterUPP
NavReplyDialog::GetObjectFilterUPP()
{
    static AutoNavObjectFilterUPP   upp(NavReplyDialog::NavFilterObjectProc);
    
    return (upp);
}

// ------------------------------------------------------------------------------------------
NavPreviewUPP
NavReplyDialog::GetPreviewUPP()
{
    static AutoNavPreviewUPP    upp(NavReplyDialog::NavPreviewProc);
    
    return (upp);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::ShowDesktop()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlShowDesktop, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SortBy(NavSortKeyField inField)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSortBy, reinterpret_cast<void*>(inField));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SortOrder(NavSortOrder inOrder)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSortOrder, reinterpret_cast<void*>(inOrder));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::ScrollHome()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlScrollHome, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::ScrollEnd()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlScrollEnd, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::ScrollPageUp()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlPageUp, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::ScrollPageDown()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlPageDown, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::GetLocation(AEDesc& outDesc)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlGetLocation, &outDesc);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SetLocation(const AEDesc& inDesc)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSetLocation, const_cast<AEDesc*>(&inDesc));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::GetSelection(AEDesc& outSelection)
{
    B_ASSERT(GetDialogRef() != NULL);
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlGetSelection, &outSelection);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SetSelection(const AEDesc& inSelection)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSetSelection, const_cast<AEDesc*>(&inSelection));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::ShowSelection()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlShowSelection, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::OpenSelection()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlOpenSelection, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::EjectVolume(short inVRefNum)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlEjectVolume, reinterpret_cast<void*>(inVRefNum));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
NavReplyDialog::IsPreviewShowing()
{
    B_ASSERT(IsStarted());
    
    Boolean     showing;
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlIsPreviewShowing, &showing);
    B_THROW_IF_STATUS(err);
    
    return (showing);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SelectCustomType(const NavMenuItemSpec& inSpec)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSelectCustomType, const_cast<NavMenuItemSpec*>(&inSpec));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SelectAllType(MenuItemIndex inItem)
{
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSelectAllType, &inItem);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SelectEditFileName(const ControlEditTextSelectionRec& inSelection)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSelectEditFileName, const_cast<ControlEditTextSelectionRec*>(&inSelection));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::BrowserSelectAll()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlBrowserSelectAll, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::GotoParent()
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlGotoParent, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::SetActionState(NavActionState inActionState)
{
    B_ASSERT(IsStarted());
    
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlSetActionState, reinterpret_cast<void*>(inActionState));
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
NavReplyDialog::BrowserRedraw()
{
    OSStatus    err;
    
    err = NavCustomControl(GetDialogRef(), kNavCtlBrowserRedraw, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
pascal Boolean
NavReplyDialog::NavPreviewProc(
    NavCBRecPtr     callBackParms,
    void*           callBackUD)
{
    NavReplyDialog* dialog  = reinterpret_cast<NavReplyDialog*>(callBackUD);
    bool            success = false;
    
    try
    {
        success = dialog->Preview(callBackParms);
    }
    catch (...)
    {
        // Prevent exceptions from propagating
    }
    
    return (success);
}

// ------------------------------------------------------------------------------------------
pascal Boolean
NavReplyDialog::NavFilterObjectProc(
    AEDesc*         theItem, 
    void*           info, 
    void*           callBackUD, 
    NavFilterModes  filterMode)
{
    NavReplyDialog* dialog  = reinterpret_cast<NavReplyDialog*>(callBackUD);
    bool            showIt  = true;
    
    try
    {
        showIt = dialog->FilterObject(*theItem, 
                                      *reinterpret_cast<NavFileOrFolderInfo*>(info), 
                                      filterMode);
    }
    catch (...)
    {
        // Prevent exceptions from propagating
    }
    
    return (showIt);
}


// ==========================================================================================
//  NavReplyDialog::PopupMenuItem

#pragma mark -
#pragma mark * NavReplyDialog::PopupMenuItem *

// ------------------------------------------------------------------------------------------
NavReplyDialog::PopupMenuItem::PopupMenuItem()
    : mFilterKind(kFilterSeparator), mName(String(CFSTR("-")))
{
}

// ------------------------------------------------------------------------------------------
NavReplyDialog::PopupMenuItem::PopupMenuItem(
    FilterKind      inFilterKind,               //!< The type of filter.
    const String&   inName,                     //!< The filter's name.
    const String&   inLocName /* = String() */) //!< The menu item text.
        : mFilterKind(inFilterKind), mName(inName), mLocName(inLocName)
{
}

// ------------------------------------------------------------------------------------------
NavReplyDialog::PopupMenuItem::PopupMenuItem(
    const Bundle::DocumentType& inDocType,  //!< The document type.
    const Bundle&               inBundle,   //!< A bundle from which to extract localised strings.
    CFStringRef                 inTable)    //!< The table (i.e. a ".strings" file) within @a inBundle from which to read the localised strings.
{
    mName = inDocType.mName;
    
    if (inDocType.mName != CFSTR("-"))
    {
        mFilterKind = kFilterName;
        
        CFStringRef localisedNameRef;
        
        localisedNameRef = CFBundleCopyLocalizedString(inBundle.cf_ref(), 
                                                       inDocType.mName.cf_ref(), 
                                                       NULL, inTable);
        
        if (localisedNameRef != NULL)
        {
            mLocName.assign(localisedNameRef, from_copy);
        }
    }
    else
    {
        // Interpret a document type name of "-" as a menu item separator.  
        // This allows clients to create groups within the menu.
        
        mFilterKind = kFilterSeparator;
    }
}

// ------------------------------------------------------------------------------------------
String
NavReplyDialog::PopupMenuItem::GetLocName() const
{
    return (mLocName.empty() ? mName : mLocName);
}


// ==========================================================================================
//  GetFileDialog

#pragma mark -
#pragma mark * GetFileDialog *

// ------------------------------------------------------------------------------------------
GetFileDialog::GetFileDialog()
{
    mCurrentDocType = mDocTypes.end();
}

// ------------------------------------------------------------------------------------------
/*! Sets the document types array to all of the document types in @a inBundle.  The array 
    is then used to populate the file type pop-up menu.  The document types' localised 
    names are assumed to reside in the "InfoPlist.strings" file within @a inBundle.
    If the array has more than one element, the menu is forced to be displayed;  note that 
    this can be overridden by a subsequent call to @c SetFlags(kNavNoTypePopup).  Finally, 
    the current filter is changed to match files matching any of the doc types.
    
    @param  InputIterator   Template parameter.  An input iterator yielding a Bundle::DocumentType object.
*/
void
GetFileDialog::SetDocumentTypes(
    const Bundle&   inBundle)   //!< The bundle in which to look for localised document type names.
{
    SetDocumentTypes(inBundle, CFSTR("InfoPlist"), inBundle.begin(), inBundle.end());
}

// ------------------------------------------------------------------------------------------
/*! Uses @c mDocTypes to populate the file type pop-up menu.  @a inBundle and @a inTable are 
    used to read the localised strings for each document type.  If the array has more than 
    one element, the menu is forced to be displayed;  note that this can be overridden by 
    a subsequent call to @c SetFlags(kNavNoTypePopup).  Finally, the current filter is 
    changed to match files matching any of the doc types.
*/
void
GetFileDialog::FillPopupMenuWithDocTypes(
    const Bundle&       inBundle,   //!< The bundle in which to look for localised document type names.
    CFStringRef         inTable)    //!< The table in which to look for localised document type names (may be @c NULL).
{
    PopupMenuContentsVector menuContents;
    
    std::transform(mDocTypes.begin(), mDocTypes.end(), std::back_inserter(menuContents), 
                   AppendDocType(inBundle, inTable));
    
    if (mDocTypes.size() > 1)
    {
        // Prepend the separator.
        menuContents.insert(menuContents.begin(), PopupMenuItem());
        
        // Prepend the "all readable files" item.  Note:  we mustn't put the two 
        // "All Readable Files" literals into a single variable, because that would 
        // break gentrings' reading of the B LocalizedStringFromTable macro.
        menuContents.insert(menuContents.begin(), 
                            PopupMenuItem(kFilterAllReadableFiles, 
                                          String(CFSTR("All Readable Files")), 
                                          BLocalizedStringFromTable(
                                            CFSTR("All Readable Files"), CFSTR("B"), 
                                            "Menu item text that allows user to choose all files that can be read by the application.")));
    }
    
    SetPopupMenuContents(menuContents);
}

// ------------------------------------------------------------------------------------------
void
GetFileDialog::SelectDocumentType(
    const String&   inDocTypeName)
{
    FilterByName(inDocTypeName);
}

// ------------------------------------------------------------------------------------------
NavDialogRef
GetFileDialog::CreateDialog()
{
    NavDialogRef    dialog;
    OSStatus        err;
    
    // We aren't returning any translation info, so there's no point 
    // in allowing translation in the UI.
    SetFlags(kNavDontAutoTranslate | kNavDontAddTranslateItems);
    
    // We don't support this, because our custom pop-up menu code subsumes its 
    // functionality.
    ClearFlags(kNavSelectAllReadableItem);
    
    err = NavCreateGetFileDialog(&GetCreationOptions(), 
                                 NULL /*mTypeList*/, 
                                 GetEventUPP(), 
                                 GetPreviewUPP(), 
                                 GetObjectFilterUPP(), 
                                 GetEventContext(), 
                                 &dialog);
    B_THROW_IF_STATUS(err);
    
    return (dialog);
}

// ------------------------------------------------------------------------------------------
void
GetFileDialog::HandleReply(
    NavUserAction           inUserAction, 
    NavReplyRecord&         ioNavReply)
{
    if (mCallback.get() != NULL)
        (*mCallback)(*this, (inUserAction == kNavUserActionOpen), ioNavReply);
}

// ------------------------------------------------------------------------------------------
bool
GetFileDialog::Run(
    AutoNavReply&   outNavReply, 
    bool            /* inUnused */)
{
    return (RunReplyDialog(kNavUserActionOpen, outNavReply));
}

// ------------------------------------------------------------------------------------------
void
GetFileDialog::PopupMenuSelect(const NavMenuItemSpec& inMenuItemSpec)
{
    NavReplyDialog::PopupMenuSelect(inMenuItemSpec);
    
    if ((inMenuItemSpec.menuCreator == 'extn') && 
        (mCurrentFilterKind == kFilterName))
    {
        mCurrentDocType = std::find_if(mDocTypes.begin(), mDocTypes.end(), 
                                       MatchFilterName(mCurrentFilterName));
        
        B_ASSERT(mCurrentDocType != mDocTypes.end());
    }
}

// ------------------------------------------------------------------------------------------
/*! This implementation attempts to match the file against all of the document type 
    entries given in SetDocType().
    
    Directories are always shown.  However, nothing special is done currently with aliases, 
    packages, etc.
*/
bool
GetFileDialog::FilterObjectByAllReadableFiles(
    const FSRef&                /* inRef */,        //!< Contains the file system object to filter.
    const LSItemInfoRecord&     inInfo,             //!< Pertinent info about @a inRef.
    NavFilterModes              /* inFilterMode */) //!< Specifies where this object will appear in the dialog.
{
    bool    showIt;
    
    if (inInfo.flags & kLSItemInfoIsContainer)
    {
        // Always show directories
        showIt = true;
    }
    else
    {
        Bundle::Iterator    it;
        
        it = Bundle::FindDocumentTypeForFileInfo(mDocTypes.begin(), mDocTypes.end(), 
                                                 inInfo.extension, inInfo.filetype);
        
        showIt = (it != mDocTypes.end());
    }
    
    return (showIt);
}

// ------------------------------------------------------------------------------------------
/*! Directories are always shown.  However, nothing special is done currently with aliases, 
    packages, etc.
*/
bool
GetFileDialog::FilterObjectByFilterName(
    const FSRef&                /* inRef */,        //!< Contains the file system object to filter.
    const LSItemInfoRecord&     inInfo,             //!< Pertinent info about @a inRef.
    NavFilterModes              /* inFilterMode */) //!< Specifies where this object will appear in the dialog.
{
    bool    showIt;
    
    if (inInfo.flags & kLSItemInfoIsContainer)
    {
        // Always show directories
        showIt = true;
    }
    else
    {
        B_ASSERT(mCurrentDocType != mDocTypes.end());
        
        showIt = Bundle::DoesFileInfoMatchDocumentType(inInfo.extension, inInfo.filetype, 
                                                       *mCurrentDocType);
    }
    
    return (showIt);
}


// ==========================================================================================
//  PutFileDialog

#pragma mark -
#pragma mark * PutFileDialog *

// ------------------------------------------------------------------------------------------
PutFileDialog::PutFileDialog()
{
    mCurrentDocType = mDocTypes.end();
    
    SetFlags(kNavPreserveSaveFileExtension);
}

// ------------------------------------------------------------------------------------------
/*! Sets the document types array to all of the document types in @a inBundle.  The array 
    is then used to populate the file type pop-up menu.  The document types' localised 
    names are assumed to reside in the "InfoPlist.strings" file within @a inBundle.
    If the array has more than one element, the menu is forced to be displayed;  note that 
    this can be overridden by a subsequent call to @c SetFlags(kNavNoTypePopup).
    
    If @a inDefaultType is non-empty, it's assumed to contain the "default" document type.  
    The document type in question is then moved to the beginning of the doc types array, 
    and a separator is inserted between it and the other document types.
*/
void
PutFileDialog::SetDocumentTypes(
    const Bundle&   inBundle,   //!< The bundle in which to look for localised document type names.
    const String&   inDefaultType /* = String() */) //!< The default document type.
{
    SetDocumentTypes(inBundle, CFSTR("InfoPlist"), inBundle.begin(), inBundle.end(), 
                     inDefaultType);
}

// ------------------------------------------------------------------------------------------
/*! Uses @c mDocTypes to populate the file type pop-up menu.  @a inBundle and @a inTable are 
    used to read the localised strings for each document type.  If the array has more than 
    one element, the menu is forced to be displayed;  note that this can be overridden by 
    a subsequent call to @c SetFlags(kNavNoTypePopup).
    
    If @a inDefaultType is non-empty, it's assumed to contain the "default" document type.  
    The document type in question is then moved to the beginning of the doc types array, 
    and a separator is inserted between it and the other document types.
*/
void
PutFileDialog::FillPopupMenuWithDocTypes(
    const Bundle&       inBundle,       //!< The bundle in which to look for localised document type names.
    CFStringRef         inTable,        //!< The table in which to look for localised document type names (may be @c NULL).
    const String&       inDefaultType)  //!< The default type, which appears at the top of the pop-up menu.
{
    if (!inDefaultType.empty())
    {
        // The caller specified a default document type, so locate the type 
        // in the doc types array and move it to the top.
        
        Bundle::DocumentTypeArray::iterator it;
        
        it  = std::find_if(mDocTypes.begin(), mDocTypes.end(), 
                           MatchFilterName(inDefaultType));
        
        B_ASSERT(it != mDocTypes.end());
        
        Bundle::DocumentType    tempDocType = *it;
        Bundle::DocumentType    separator;
        
        mDocTypes.erase(it);
        
        separator.mName = "-";
        mDocTypes.insert(mDocTypes.begin(), separator);
        mDocTypes.insert(mDocTypes.begin(), tempDocType);
    }
                                          
    PopupMenuContentsVector menuContents;
    
    std::transform(mDocTypes.begin(), mDocTypes.end(), std::back_inserter(menuContents), 
                   AppendDocType(inBundle, inTable));
    
    SetPopupMenuContents(menuContents);
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::SelectDocumentType(
    const String&   inDocTypeName)
{
    FilterByName(inDocTypeName);
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::SetSaveInfo(
    const SaveInfo&     inSaveInfo)
{
    mSaveInfo = inSaveInfo;
    
    String  contentType = mSaveInfo.GetContentType();
    
    if (!contentType.empty())
        SelectDocumentType(contentType);
    
    String  fileName    = mSaveInfo.GetFileName();
    
    if (!fileName.empty())
        SetFileName(fileName);
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::HideExtension(bool hideIt)
{
    B_ASSERT(GetDialogRef() != NULL);

    // As of Mac OS X 10.4.3, NavDialogSetSaveFileExtensionHidden returns an error 
    // if we tell it to hide the extension when the file name doesn't in fact have 
    // an extension.  Sigh ...
    //
    // So, to avoid this error, we only call the function if (1) the file name has 
    // an extension, OR the caller does NOT want the extension hidden.
    
    String  extension;
    
    if (GetFileNameExtension(GetEditFileName(), extension) || !hideIt)
    {
        OSStatus    err;
        
        err = NavDialogSetSaveFileExtensionHidden(GetDialogRef(), true);
        B_THROW_IF_STATUS(err);
    }   
}

// ------------------------------------------------------------------------------------------
bool
PutFileDialog::IsExtensionHidden()
{
    B_ASSERT(GetDialogRef() != NULL);
    
    return (NavDialogGetSaveFileExtensionHidden(GetDialogRef()));
}

// ------------------------------------------------------------------------------------------
String
PutFileDialog::GetEditFileName()
{
    return (String(NavDialogGetSaveFileName(GetDialogRef())));
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::SetEditFileName(const String& inFileName)
{
    OSStatus    err;
    
    err = NavDialogSetSaveFileName(GetDialogRef(), inFileName.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
NavDialogRef
PutFileDialog::CreateDialog()
{
    NavDialogRef    dialog;
    OSStatus        err;
    
    err = NavCreatePutFileDialog(&GetCreationOptions(), 
                                 '\?\?\?\?' /* mFileType */, 
                                 kNavGenericSignature /* mCreator */, 
                                 GetEventUPP(), GetEventContext(), &dialog);
    B_THROW_IF_STATUS(err);
    
    return (dialog);
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::DialogStarting(NavCBRec& ioParams)
{
    NavReplyDialog::DialogStarting(ioParams);
    
    HideExtension(mSaveInfo.GetExtensionHidden());
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::HandleReply(
    NavUserAction           inUserAction, 
    NavReplyRecord&         ioNavReply)
{
    if ((inUserAction == kNavUserActionSaveAs) && !ioNavReply.validRecord)
        inUserAction = kNavUserActionCancel;
    
    bool    good    = (inUserAction == kNavUserActionSaveAs);
    
    if (good)
    {
        // Retrieve the document type.
        
        if (mCurrentDocType != mDocTypes.end())
            mSaveInfo.SetContentType(mCurrentDocType->mName);
        
        // Retrieve the parent folder's FSRef.
        
        FSRef   folderRef;
        
        DescParamHelper::ReadParam<typeFSRef>(ioNavReply.selection, 1, folderRef);
        
        mSaveInfo.SetFolderRef(folderRef);
        
        // Retrieve the selected file name.
        
        String  fileName(ioNavReply.saveFileName);
        
        mSaveInfo.SetFileName(fileName);
        
        // Construct the selected file's URL from its folder FSRef and name.

        Url folderUrl(folderRef);
        
        mSaveInfo.SetUrl(folderUrl.PushPath(fileName, false));
        
        // Retrieve the extension-hidden flag.
        
        mSaveInfo.SetExtensionHidden(IsExtensionHidden());
        
        // If the file already exists, store its FSRef.
        
        FSRef           fileRef;
        HFSUniStr255    hfsName;
        OSStatus        err;
        
        make_hfsstring(fileName.cf_ref(), hfsName);
        
        err = FSMakeFSRefUnicode(&folderRef, hfsName.length, hfsName.unicode, 
                                 GetApplicationTextEncoding(), &fileRef);
        if (err != fnfErr)
            B_THROW_IF_STATUS(err);
        
        if (err == noErr)
        {
            mSaveInfo.SetFileRef(fileRef);
        }
        
        // Should we replace any existing file ?
        
        mSaveInfo.SetReplacing(ioNavReply.replacing);
    }
    
    if (mCallback.get() != NULL)
        (*mCallback)(*this, good, ioNavReply, mSaveInfo);
}

// ------------------------------------------------------------------------------------------
bool
PutFileDialog::Run(
    AutoNavReply&   outNavReply)
{
    return (RunReplyDialog(kNavUserActionSaveAs, outNavReply));
}

// ------------------------------------------------------------------------------------------
void
PutFileDialog::PopupMenuSelect(const NavMenuItemSpec& inMenuItemSpec)
{
    NavReplyDialog::PopupMenuSelect(inMenuItemSpec);
    
    if ((inMenuItemSpec.menuCreator == 'extn') && 
        (mCurrentFilterKind == kFilterName))
    {
        mCurrentDocType = std::find_if(mDocTypes.begin(), mDocTypes.end(), 
                                       MatchFilterName(mCurrentFilterName));
        
        B_ASSERT(mCurrentDocType != mDocTypes.end());
    }
}


// ==========================================================================================
//  ChooseFolderDialog

#pragma mark -
#pragma mark * ChooseFolderDialog *

// ------------------------------------------------------------------------------------------
ChooseFolderDialog::ChooseFolderDialog()
{
}

// ------------------------------------------------------------------------------------------
NavDialogRef
ChooseFolderDialog::CreateDialog()
{
    NavDialogRef    dialog;
    OSStatus        err;
    
    err = NavCreateChooseFolderDialog(&GetCreationOptions(), 
                                      GetEventUPP(), 
                                      GetObjectFilterUPP(), 
                                      GetEventContext(), 
                                      &dialog);
    B_THROW_IF_STATUS(err);
    
    return (dialog);
}

// ------------------------------------------------------------------------------------------
void
ChooseFolderDialog::HandleReply(
    NavUserAction           inUserAction, 
    NavReplyRecord&         ioNavReply)
{
    if (mCallback.get() != NULL)
        (*mCallback)(*this, (inUserAction == kNavUserActionChoose), ioNavReply);
}

// ------------------------------------------------------------------------------------------
bool
ChooseFolderDialog::Run(
    AutoNavReply&   outNavReply, 
    bool            /* inUnused */)
{
    return (RunReplyDialog(kNavUserActionChoose, outNavReply));
}


// ==========================================================================================
//  AskSaveChangesDialog

#pragma mark -
#pragma mark * AskSaveChangesDialog *

// ------------------------------------------------------------------------------------------
AskSaveChangesDialog::AskSaveChangesDialog()
    : mContext(kClosing)
{
}

// ------------------------------------------------------------------------------------------
/*! It's important to call this function because it affects the text displayed in the 
    alert.
*/
void
AskSaveChangesDialog::SetContext(
    Context inContext)  //!< The context in which we're being called.
{
    mContext = inContext;
}

// ------------------------------------------------------------------------------------------
AskSaveChangesDialog::Action
AskSaveChangesDialog::GetAction() const
{
    return (ConvertUserAction(GetUserAction()));
}

// ------------------------------------------------------------------------------------------
NavDialogRef
AskSaveChangesDialog::CreateDialog()
{
    NavDialogRef    dialog;
    OSStatus        err;
    
    err = NavCreateAskSaveChangesDialog(&GetCreationOptions(), mContext, GetEventUPP(), 
                                        GetEventContext(), &dialog);
    B_THROW_IF_STATUS(err);
    
    return (dialog);
}

// ------------------------------------------------------------------------------------------
void
AskSaveChangesDialog::HandleUserAction(
    NavUserAction   inUserAction)   //!< The user action.
{
    if (mCallback.get() != NULL)
        (*mCallback)(*this, ConvertUserAction(inUserAction));
}

// ------------------------------------------------------------------------------------------
AskSaveChangesDialog::Action
AskSaveChangesDialog::Run()
{
    return (ConvertUserAction(RunDialog()));
}

// ------------------------------------------------------------------------------------------
void
AskSaveChangesDialog::Configure(
    const DialogModality&   inModality, //!< The dialog's modality.
    const String&           inFileName, //!< The display name of the file we're being asked to save.
    Context                 inContext)  //!< The context in which we're being called.
{
    SetModality(inModality);
    SetFileName(inFileName);
    SetContext(inContext);
}

// ------------------------------------------------------------------------------------------
AskSaveChangesDialog::Action
AskSaveChangesDialog::ConvertUserAction(NavUserAction inUserAction)
{
    Action  action;
    
    switch (inUserAction)
    {
    default:
    case kNavUserActionCancel:          action = kCancel;   break;
    case kNavUserActionSaveChanges:     action = kSave;     break;
    case kNavUserActionDontSaveChanges: action = kDontSave; break;
    }
    
    return (action);
}


// ==========================================================================================
//  AskDiscardChangesDialog

#pragma mark -
#pragma mark * AskDiscardChangesDialog *

// ------------------------------------------------------------------------------------------
AskDiscardChangesDialog::AskDiscardChangesDialog()
{
}

// ------------------------------------------------------------------------------------------
/*! Only useful in app-modal scenario.
*/
bool
AskDiscardChangesDialog::GetAction() const
{
    return (ConvertUserAction(GetUserAction()));
}

// ------------------------------------------------------------------------------------------
NavDialogRef
AskDiscardChangesDialog::CreateDialog()
{
    NavDialogRef    dialog;
    OSStatus        err;
    
    err = NavCreateAskDiscardChangesDialog(&GetCreationOptions(), GetEventUPP(), 
                                           GetEventContext(), &dialog);
    B_THROW_IF_STATUS(err);
    
    return (dialog);
}

// ------------------------------------------------------------------------------------------
void
AskDiscardChangesDialog::HandleUserAction(
    NavUserAction   inUserAction)   //!< The user action.
{
    if (mCallback.get() != NULL)
        (*mCallback)(*this, ConvertUserAction(inUserAction));
}

// ------------------------------------------------------------------------------------------
/*! @return @c true if the user clicked the Discard button.
*/
bool
AskDiscardChangesDialog::Run()
{
    return (ConvertUserAction(RunDialog()));
}

// ------------------------------------------------------------------------------------------
void
AskDiscardChangesDialog::Configure(
    const DialogModality&   inModality, //!< The dialog's modality.
    const String&           inFileName) //!< The display name of the file we're being asked to revert.
{
    SetModality(inModality);
    SetFileName(inFileName);
}

// ------------------------------------------------------------------------------------------
bool
AskDiscardChangesDialog::ConvertUserAction(NavUserAction inUserAction)
{
    return (inUserAction == kNavUserActionDiscardChanges);
}


// ==========================================================================================
//  AskReviewDocumentsDialog

#pragma mark -
#pragma mark * AskReviewDocumentsDialog *

// ------------------------------------------------------------------------------------------
AskReviewDocumentsDialog::AskReviewDocumentsDialog()
    : mDocumentCount(0)
{
}

// ------------------------------------------------------------------------------------------
void
AskReviewDocumentsDialog::SetDocumentCount(
    UInt32  inDocumentCount)    //!< The number of modified documents.  Should be \> 1.
{
    mDocumentCount = inDocumentCount;
}

// ------------------------------------------------------------------------------------------
AskReviewDocumentsDialog::Action
AskReviewDocumentsDialog::GetAction() const
{
    return (ConvertUserAction(GetUserAction()));
}

// ------------------------------------------------------------------------------------------
NavDialogRef
AskReviewDocumentsDialog::CreateDialog()
{
    NavDialogRef    dialog;
    OSStatus        err;
    
    err = NavCreateAskReviewDocumentsDialog(&GetCreationOptions(), mDocumentCount, 
                                            GetEventUPP(), GetEventContext(), &dialog);
    B_THROW_IF_STATUS(err);
    
    return (dialog);
}

// ------------------------------------------------------------------------------------------
void
AskReviewDocumentsDialog::HandleUserAction(
    NavUserAction   inUserAction)   //!< The user action.
{
    if (mCallback.get() != NULL)
        (*mCallback)(*this, ConvertUserAction(inUserAction));
}

// ------------------------------------------------------------------------------------------
AskReviewDocumentsDialog::Action
AskReviewDocumentsDialog::Run()
{
    return (ConvertUserAction(RunDialog()));
}

// ------------------------------------------------------------------------------------------
void
AskReviewDocumentsDialog::Configure(
    const DialogModality&   inModality,         //!< The dialog's modality.
    UInt32                  inDocumentCount)    //!< The number of modified documents.  Should be \> 1.
{
    SetModality(inModality);
    SetDocumentCount(inDocumentCount);
}

// ------------------------------------------------------------------------------------------
AskReviewDocumentsDialog::Action
AskReviewDocumentsDialog::ConvertUserAction(NavUserAction inUserAction)
{
    Action  action;
    
    switch (inUserAction)
    {
    default:
    case kNavUserActionCancel:              action = kCancel;   break;
    case kNavUserActionReviewDocuments:     action = kReview;   break;
    case kNavUserActionDiscardDocuments:    action = kDiscard;  break;
    }
    
    return (action);
}


// ==========================================================================================
//  AutoNavReply

#pragma mark -
#pragma mark * AutoNavReply *

// ------------------------------------------------------------------------------------------
AutoNavReply::AutoNavReply()
{
    init();
}

// ------------------------------------------------------------------------------------------
AutoNavReply::~AutoNavReply()
{
    try
    {
        clear();
    }
    catch (...)
    {
        // clear() can throw, but we can't
    }
}

// ------------------------------------------------------------------------------------------
//! Sets the reply record to an initial 'empty' state.
void
AutoNavReply::init()
{
    BlockZero(&mNavReply, sizeof(mNavReply));
    mNavReply.version = kNavReplyRecordVersion;
    
    mHasReply = false;
}

// ------------------------------------------------------------------------------------------
void
AutoNavReply::reset(
    NavDialogRef    inNavDialogRef) //!< The nav dialog from which to retrieve the reply.
{
    NavReplyRecord  tempReply;
    OSStatus        err;
    
    err = NavDialogGetReply(inNavDialogRef, &tempReply);
    B_THROW_IF_STATUS(err);
    
    // If we have a currently initialised @c NavReplyRecord, this will move it into a 
    // temporary object whose destructor will clean it up without throwing.  This object 
    // will also be cleared.
    AutoNavReply().swap(*this);
    
    // Clobber our nav reply with the one we just retrieved.
    mNavReply   = tempReply;
    mHasReply   = true;
}

// ------------------------------------------------------------------------------------------
void
AutoNavReply::clear()
{
    if (mHasReply)
    {
        OSStatus    err;
        
        err = NavDisposeReply(&mNavReply);
        B_THROW_IF_STATUS(err);
        
        init();
    }
}

// ------------------------------------------------------------------------------------------
void
AutoNavReply::swap(AutoNavReply& other)
{
    std::swap(mHasReply, other.mHasReply);
    std::swap(mNavReply, other.mNavReply);
}


// ==========================================================================================
//  InteractWithUserDialog

#pragma mark -
#pragma mark * InteractWithUserDialog *

// static member variables
Str255  InteractWithUserDialog::sText   = "\p";
bool    InteractWithUserDialog::sMark   = true;
Handle  InteractWithUserDialog::sIcon   = NULL;
Handle  InteractWithUserDialog::sSound  = reinterpret_cast<Handle>(-1L);

// ------------------------------------------------------------------------------------------
/*! The object's internal Notification Manager data structure is initialised from the 
    default settings.
*/
InteractWithUserDialog::InteractWithUserDialog()
{
    BlockZero(&mNMRec, sizeof(mNMRec));
    
    PLstrcpy(mText, sText);
    
    mNMRec.qType    = nmType;
    mNMRec.nmMark   = sMark ? 1 : 0;
    mNMRec.nmStr    = (StrLength(mText) > 0) ? mText : NULL;
    mNMRec.nmSound  = sSound;
    mNMRec.nmIcon   = sIcon;
}

// ------------------------------------------------------------------------------------------
/*! This constructor allows the caller to override the default settings.  This would be 
    useful if most of the time the default settings are acceptable, but in certain cases 
    a different way of notifying the user is required.
    
    @note   @a inText must be convertible to the application text encoding (as returned 
            by @c GetApplicationTextEncoding()), because the Notification Manager doesn't 
            understand Unicode.
*/
InteractWithUserDialog::InteractWithUserDialog(
    const NMRec&    inNMRec,    //!< Holds most of the settings except the text.  This structure is copied into the object.
    const String&   inText)     //!< The string to display, if any.
        : mNMRec(inNMRec)
{
    inText.copy(mText, sizeof(mText), GetApplicationTextEncoding());
    
    mNMRec.nmStr = (StrLength(mText) > 0) ? mText : NULL;
}

// ------------------------------------------------------------------------------------------
/*! Checks if the application is already in the foreground, and if so returns 
    immediately.  If it's @e not in the foreground, then it will attempt to obtain the 
    user's attention so that the user brings the application to the foreground.  The 
    exact method by which this is done is determined by the object's settings:
        
        - In all cases, the application's icon will start bouncing in the dock.
        - If the settings contain a string, an alert containing the string will be 
          displayed.
        - If the settings contain a sound, the sound is played.
    
    If the user brings the application to the foreground within the alloted timeout period, 
    then the function returns and the application can perform its normal processing.
    If on the other hand the timeout expires first, then function throws an 
    IteractionTimeoutException;  the application must then abandon its processing.
*/
void
InteractWithUserDialog::Run(
    long timeout /* = kAEDefaultTimeout */) //!< The amount of time the application is willing to wait.
{
    ProcessSerialNumber currPSN, frontPSN;
    Boolean             isFrontProcess;
    OSStatus            err;
    
    err = GetCurrentProcess(&currPSN);
    B_THROW_IF_STATUS(err);
    
    err = GetFrontProcess(&frontPSN);
    B_THROW_IF_STATUS(err);
    
    err = SameProcess(&currPSN, &frontPSN, &isFrontProcess);
    B_THROW_IF_STATUS(err);
    
    if (!isFrontProcess)
    {
        err = AEInteractWithUser(timeout, &mNMRec, NULL);
        B_THROW_IF(err == errAENoUserInteraction, InteractionTimeoutException());
        B_THROW_IF_STATUS(err);
    }
}

// ------------------------------------------------------------------------------------------
/*! Calling this function with a non-empty string will cause an alert containing the 
    string to be displayed whenever Run() is called.
    
    @note   @a inText must be convertible to the application text encoding (as returned 
            by @c GetApplicationTextEncoding()), because the Notification Manager doesn't 
            understand Unicode.
*/
void
InteractWithUserDialog::SetText(
    const String&   inText) //!< The string to display.
{
    inText.copy(sText, sizeof(sText), GetApplicationTextEncoding());
}

// ------------------------------------------------------------------------------------------
/*! @deprecated It's not clear that this function has any effect under Aqua.
*/
void
InteractWithUserDialog::SetMark(bool inShowMark)
{
    sMark = inShowMark;
}

// ------------------------------------------------------------------------------------------
/*! @deprecated It's not clear that this function has any effect under Aqua.
*/
void
InteractWithUserDialog::SetIcon(Handle inIcon)
{
    sIcon = inIcon;
}

// ------------------------------------------------------------------------------------------
void
InteractWithUserDialog::SetSound(
    Handle  inSound)    //!< The sound to play.  Must be in sound resource (@c 'snd ') format.
{
    sSound = inSound;
}


}   // namespace B
