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

#ifndef BNavDialogs_H_
#define BNavDialogs_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/concept_check.hpp>
#include <boost/utility.hpp>

// B headers
#include "BBundle.h"
#include "BDialogCallback.h"
#include "BDialogModality.h"
#include "BSaveInfo.h"
#include "BString.h"


namespace B {

// forward declarations
class   AutoNavReply;
class   Nib;
class   Point;
class   Rect;
class   SaveInfo;
class   Size;
class   Url;
class   View;


// ==========================================================================================
//  ProtectedDefaultConstructorConcept

#pragma mark * ProtectedDefaultConstructorConcept *

/*!
    All descendants of NavDialog need a default constructor;  however, to prevent accidental 
    mis-use (by declaring a NavDialog-derived object on the stack, for example), the 
    constructor must be protected.  This concept check verifies that condition indirectly.
*/
template <class TT>
struct ProtectedDefaultConstructorConcept
{
    struct derived : public TT {};
    
    void constraints()
    {
        derived a;               // require default constructor
        boost::ignore_unused_variable_warning(a);
    }
};


// ==========================================================================================
//  NavDialog

#pragma mark * NavDialog *

/*!
    @brief  Abstract base class for Navigation Services dialogs.
    
    NavDialog is an abstract base class.  It provides the common functionality required of 
    all Navigation Services dialogs, but doesn't actually create Navigation Services 
    dialog objects (i.e., @c NavDialogRefs);  that is left to derived classes.
    
    NavDialog's public interface is taken up mostly by configuration functions;  they allow 
    clients to tweak the dialog's behaviour prior to its being displayed.  There is also 
    a much smaller group of functions that allow the dialog to be "entered", i.e. to be 
    displayed according to its modality.
    
    Since NavDialog's main purpose is to be derived from, it supports a protected interface 
    which serves as hooks for derived classes.
    
    @sa     @ref dialogs_alerts_nav
*/
class NavDialog : public boost::noncopyable
{
public:
    
    //! Destructor.  Releases resources.
    virtual ~NavDialog();
    
    /*! @name Setup
        
        Configuration functions meant for clients of the NavDialog classes.
    */
    //@{
    //! Sets the dialog's modality.
    void    SetModality(const DialogModality& inModality);
    //! Determines whether the NavDialog object gets deleted after the dialog is dismissed.
    void    SetDeleteOnDismiss(bool inDeleteOnDismiss);
    //! Sets some of the dialog option flags.
    void    SetFlags(NavDialogOptionFlags flags);
    //! Clears some of the dialog option flags.
    void    ClearFlags(NavDialogOptionFlags flags);
    //! Sets the dialog's location on the screen.
    void    SetLocation(const Point& pt);
    //! Sets the application's name in the dialog's title bar.
    void    SetAppName(const String& name);
    //! Sets the dialog's title.
    void    SetTitle(const String& title);
    //! Sets the dialog's message.
    void    SetMessage(const String& message);
    //! Sets the file name.
    void    SetFileName(const String& fileName);
    //! Sets the title of the "action" button (i.e., Open, Save, etc).
    void    SetActionButtonLabel(const String& label);
    //! Sets the title of the Cancel button.
    void    SetCancelButtonLabel(const String& label);
    //! Sets the custom elements in the show & format pop-up menus.
    void    SetPopupStrings(OSPtr<CFArrayRef> inPopupStrings);
    //! Sets the dialog's prefs key.
    void    SetPrefsKey(UInt32 key);
    //@}
    
    /*! @name Entering Dialogs
        
        In B parlance, "Entering" a dialog means to display it according to its 
        modality, i.e. if it's application-modal then the function doesn't return 
        until the dialog is dismissed.
    */
    //@{
    //! Displays the nav dialog according to its modality.
    void    Enter();
    //! Displays the given nav dialog according to its modality, taking control of the dialog's lifetime.
    template <class NAV_DIALOG>
    static void Enter(std::auto_ptr<NAV_DIALOG> inDialogPtr);
    //@}
    
    //! @name Retrieving Results
    //@{
    //! Returns the action that dismissed the dialog.
    NavUserAction   GetUserAction() const;
    //@}

protected:
    
    //! Protected default constructor to discourage stack-based instantiation.
    NavDialog();
    
    //! @name Overridables
    //@{
    
    /*! @brief Creates the NavDialogRef.
        
        This is a pure virtual function, which all derived classes must override. All of 
        the arguments must be passed through to one of the @c NavCreateXXXDialog functions.
        
        @return The @c NavDialogRef.  Never returns @c NULL;  if an error occurs, an exception is thrown instead.
    */
    virtual NavDialogRef    CreateDialog() = 0;
    
    /*! @brief Action notification.
        
        Override this to handle user actions, such as clicking in dialog-dismissing 
        buttons.
        
        This is a pure virtual function, which all derived classes must override.
    */
    virtual void    HandleUserAction(
                        NavUserAction           inUserAction)       //!< The user-initiated action.
                        = 0;
    
    //! Override this to do some processing after the dialog is created and initialised, but before it's displayed.
    virtual void    DialogStarting(
                        NavCBRec&               ioParams);
    
    //! Override this to do some processing before the dialog is destroyed.
    virtual void    DialogEnding();
    
    //! Low-level function to handle Nav dialog events.  There's normally no need to override this function.
    virtual void    HandleEvent(
                        NavEventCallbackMessage inSelector, 
                        NavCBRec&               ioParams);
    //@}
    
    /*! @name Dialog Control
        These functions allow the developer to manipulate (aka control) the dialog.  They are 
        thin wrappers around @c NavCustomControl().  Most of them are only pertinent to the 
        complex dialogs (get file, put file, etc.) and not the alert-style ones.
    */
    //@{
    //! Returns the options struct to pass to the NavCreateXXXDialog functions.
    const NavDialogCreationOptions&
                    GetCreationOptions() const  { return (mOptions); }
    //! Is the dialog started ?
    bool            IsStarted() const           { return (mStarted); }
    //! Returns the underlying nav dialog object.
    NavDialogRef    GetDialogRef() const        { return (mNavDialog); }
    //! Tells the dialog to dismiss itself as if the user had clicked the Cancel button.
    void            Cancel();
    //! Tells the dialog to dismiss itself as if the user had clicked the "accept" button.
    void            Accept();
    //! Tells the dialog to dismiss itself.
    void            Terminate();
    //! Makes the dialog app-modal, turns delete-on-dismiss off, enter dialog, then returns dismissing action.
    NavUserAction   RunDialog();
    //! Returns the UPP to use for handling dialog events.
    static NavEventUPP
                    GetEventUPP();
    //! Returns the user data to use for handling dialog events.
    void*           GetEventContext()           { return (this); }
    //@}
    
private:
    
    void    HandleStart(NavCBRec& ioParams);
    void    HandleTerminate();
    
    // callbacks
    static pascal void      NavEventProc(
                                NavEventCallbackMessage callBackSelector, 
                                NavCBRecPtr             callBackParms, 
                                void*                   callBackUD);
    
    // member variables
    String          mAppName;
    String          mTitle;
    String          mMessage;
    String          mActionLabel;
    String          mCancelLabel;
    String          mSaveFileName;
    bool            mDeleteOnDismiss;
    NavUserAction   mUserAction;        //!< The user action that terminated the dialog.
    bool            mStarted;           //!< @c true between calls to DialogStarting() and DialogEnding().
    NavDialogRef    mNavDialog;         //!< The underlying NavServices dialog.  This is != @c NULL once CreateDialog() returns.
    NavDialogCreationOptions    mOptions;
    OSPtr<CFArrayRef>           mPopupStrings;
};

// ------------------------------------------------------------------------------------------
template <class NAV_DIALOG> void
NavDialog::Enter(
    std::auto_ptr<NAV_DIALOG>   inDialogPtr)    //!< The dialog to enter.
{
    boost::function_requires< boost::ConvertibleConcept<NAV_DIALOG*, B::NavDialog*> >();
    
    // The caller has given us control over the lifetime of the dialog, so dispose of it 
    // upon dismissal.
    inDialogPtr->SetDeleteOnDismiss(true);
    
    // Enter the dialog.  This may or may not return until the dialog has been dismissed.
    inDialogPtr->Enter();
    
    // We don't need this anymore.
    inDialogPtr.release();
}


// ==========================================================================================
//  NavReplyDialog

#pragma mark -
#pragma mark * NavReplyDialog *

/*!
    @brief  Abstract base class for complex Navigation Services dialogs.
    
    NavReplyDialog derives from NavDialog and is also an abstract base class.  It provides 
    numerous additional services for the more complex NavServices dialogs.  Its public 
    interface supports:

    - Retrieving a @c NavReplyRecord.  The returned structure is wrapped in an AutoNavReply
      object, which will ensure its proper disposal.
    - Setting up the file type pop-up menu.  This menu is used for two purposes.  In 
      PutFileDialog, it allows the user to choose which file format to use when saving the 
      file.  In the other dialogs (eg GetFileDialog) it allows the user to filter the 
      displayed file system contents.  In all cases, the menu is optional.
    - Setting up a custom view area from a Nib.  This allows clients to insert their own 
      UI widgets into the dialog.

    Since NavReplyDialog's main purpose is to be derived from, it supports a large protected 
    interface, part of which serves as hooks for derived classes.  That interface can be 
    subdivided into the following categories:

    - Action notification.  There are numerous virtual functions that are called whenever 
      interesting things happen in the dialog.  For example, PopupMenuSelect() is called 
      when the user chooses an item in the file type pop-up menu.
    - Filtering hooks.  A group of virtual functions can be overridden to customize the 
      filtering that occurs for a given filter type.
    - Dialog control.  There is a large number of functions that allow the nav dialog to 
      be controlled in various ways.  These are almost all wrappers around the 
      @c NavCustomControl() function.
    
    @sa     @ref dialogs_alerts_nav
*/
class NavReplyDialog : public NavDialog
{
public:
    
    //! @name Constants
    //@{
    //!< Types of file system object filtering.
    enum FilterKind
    {
        kFilterName = 1,            //!< Filter is specified in @p mName field of PopupMenuItem struct.
        kFilterAllFiles,            //!< All files, regardless of extension, file type or creator.
        kFilterAllAppFiles,         //!< All files matching a creator code (UNIMPLEMENTED).
        kFilterAllReadableFiles,    //!< All files matching one of a set of extensions and file types.
        kFilterSeparator            //!< Menu item separator.
    };
    //@}
    
    //! @name Types
    //@{
    //! Describes one element of the dialog's filtering pop-up menu.
    class PopupMenuItem
    {
    public:
        
        //! Default constructor.  Makes a kFilterSeparator menu item.
        PopupMenuItem();
        //! Constructs a menu item from the given arguments.
        PopupMenuItem(
            FilterKind                  inFilterKind, 
            const String&               inName, 
            const String&               inLocName = String());
        //! Constructs a menu item from the given document type.
        PopupMenuItem(
            const Bundle::DocumentType& inDocType, 
            const Bundle&               inBundle, 
            CFStringRef                 inTable);
        
        //! Returns the type of filtering applied by this element.
        FilterKind  GetFilterKind() const   { return (mFilterKind); }
        //! Returns the internal name for this filter (valid if GetFilterKind() == kFilterName).
        String      GetName() const         { return (mName); }
        //! Returns the menu item text for this element.
        String      GetLocName() const;
        
    private:
        
        // member variables
        FilterKind  mFilterKind;    //!< The type of filtering done by this item.
        String      mName;          //!< Name used for internal communication (should @b not be localised).
        String      mLocName;       //!< Localised name.  This is what appears in the menu.  If empty, @p mName is used instead.
    };
    
    //! The entire contents of the filtering pop-up menu.
    typedef std::vector<PopupMenuItem>  PopupMenuContentsVector;
    //@}
    
    //! Destructor.
    virtual ~NavReplyDialog();
    
    /*! @name Setup
        Configuration functions meant for clients of the NavDialog classes.
    */
    //@{
    //! Sets the custom elements in the show & format pop-up menus.
    void    SetPopupMenuContents(PopupMenuContentsVector& inContents);
    //! Selects the "All Documents" pop-up menu item.
    void    FilterAllFiles();
    //! Selects the "All Reaadable Documents" pop-up menu item.
    void    FilterAllReadableFiles();
    //! Selects the pop-up menu item for the given filter name.
    void    FilterByName(const String& inFilterName);
    //! Sets the top-level custom view.
    void    SetCustomView(
                Nib&        inNib, 
                const char* inViewName);
    //@}
    
    //! @name Retrieving Results
    //@{
    //! Returns the dialog's nav reply.
    void    GetNavReply(AutoNavReply& outNavReply) const;
    //@}
    
protected:

    //! Protected default constructor to discourage stack-based instantiation.
    NavReplyDialog();
    
    View*   FindCustomViewByID(int inID) const;
    View*   FindCustomViewByID(OSType inSignature, int inID) const;
    
    //! @name Overridables
    //@{
    
    /*! @brief Action notification.
        
        Override this to handle user actions, such as clicking in dialog-dismissing 
        buttons.
        
        This is a pure virtual function, which all derived classes must override.
    */
    virtual void    HandleReply(
                        NavUserAction   inUserAction, 
                        NavReplyRecord& ioNavReply) = 0;
    
    //! Override this function to negotiate an area in the dialog for custom UI elements.
    virtual void    NegotiateCustomArea(Size& ioSize);
    //! Override this to do some processing when the dialog is resized.
    virtual void    DialogResized(Rect& ioCustomBounds, Rect& ioPreviewBounds);
    //! Override this to do some processing when the location displayed in the browser list changes.
    virtual void    LocationChanged(const AEDesc& inNewLocation);
    //! Override this to do some processing when the desktop is displayed in the dialog.
    virtual void    GoneToDesktop();
    //! Override this to do some processing when the user choose an item in the show or format pop-up menu.
    virtual void    PopupMenuSelect(const NavMenuItemSpec& inMenuItemSpec);
    //! Override this to do some processing when the selection in the browser list changes.
    virtual void    SelectionChanged(const AEDescList& inNewSelection);
    //! Override this to do some processing when preview area is turned on or off.
    virtual void    AdjustPreview(bool visible);
    //! Override this to do some processing when the selection in the browser list is opened.
    virtual void    SelectionOpened();
    //! Override this to do some processing when the user performs a confirming action.
    virtual void    DialogAccepted();
    //! Override this to do some processing when the user performs a cancelling action.
    virtual void    DialogCancelled();
    //@}
    
    /*! @name Dialog Control
        These functions allow the developer to manipulate (aka control) the dialog.  They are 
        thin wrappers around @c NavCustomControl().  Most of them are only pertinent to the 
        complex dialogs (get file, put file, etc.) and not the alert-style ones.
    */
    //@{
    //! Tells the dialog to display the desktop.
    void            ShowDesktop();
    //! Tells the dialog to sort the browser list according to @a inField.
    void            SortBy(NavSortKeyField inField);
    //! Tells the dialog to sort the browser list in ascending or descending order, according to @a inOrder.
    void            SortOrder(NavSortOrder inOrder);
    //! Tells the dialog to scroll the browser list to the top.
    void            ScrollHome();
    //! Tells the dialog to scroll the browser list to the bottom.
    void            ScrollEnd();
    //! Tells the dialog to scroll the browser list up a page.
    void            ScrollPageUp();
    //! Tells the dialog to scroll the browser list down a page.
    void            ScrollPageDown();
    //! Tells the dialog to return the location displayed in the browser list.
    void            GetLocation(AEDesc& outLocation);
    //! Tells the dialog to change the location displayed in the browser list.
    void            SetLocation(const AEDesc& inLocation);
    //! Tells the dialog to return the selection in the browser list.
    void            GetSelection(AEDesc& outSelection);
    //! Tells the dialog to change the selection in the browser list.
    void            SetSelection(const AEDesc& inSelection);
    //! Tells the dialog to reveal the selection in the browser list.
    void            ShowSelection();
    //! Tells the dialog to open the selected item in the browser list.
    void            OpenSelection();
    //! Tells the dialog to eject the volume identified by @a inVRefNum.
    void            EjectVolume(short inVRefNum);
    //! Returns true if the preview area is currently available.
    bool            IsPreviewShowing();
    //! Tells the dialog to choose the custom pop-up menu item identified by @a inSpec.
    void            SelectCustomType(const NavMenuItemSpec& inSpec);
    //! Tells the dialog to choose the standard pop-up menu item identified by @a inItem.
    void            SelectAllType(MenuItemIndex inItem);
    //! Tells the dialog to change the selection in the file name field.
    void            SelectEditFileName(const struct ControlEditTextSelectionRec& inSelection);
    //! Tells the dialog to select all items in the browser list.
    void            BrowserSelectAll();
    //! Tells the dialog to navigate to the parent item of the current selection.
    void            GotoParent();
    //! Tells the dialog to prevent the user from performing certain actions, such clicking the Cancel button.
    void            SetActionState(NavActionState inActionState);
    //! Tells the dialog to refresh its browser list.
    void            BrowserRedraw();
    //! Returns the UPP to use for filtering file system objects.
    static NavObjectFilterUPP
                    GetObjectFilterUPP();
    //! Returns the UPP to use for previewing.
    static NavPreviewUPP
                    GetPreviewUPP();
    //! Runs the dialog, returning the nav reply if the user confirmed the dialog.
    bool            RunReplyDialog(NavUserAction inConfirmAction, AutoNavReply& outNavReply);
    //@}
    
    /*! @name File Filtering
        Derived classes may override these to perform more specialised filtering.
    */
    //@{
    //! Determines whether the object denoted by @a inItem, @a inInfo and @a inFilterMode should be displayed.
    virtual bool    FilterObject(
                        const AEDesc&           inItem, 
                        const NavFileOrFolderInfo& inInfo, 
                        NavFilterModes          inFilterMode);
    //! Performs filtering when "All Documents" is selected in the pop-up menu.
    virtual bool    FilterObjectByAllFiles(
                        const FSRef&            inRef, 
                        const LSItemInfoRecord& inInfo, 
                        NavFilterModes          inFilterMode);
    //! Performs filtering when "All Readable Documents" is selected in the pop-up menu.
    virtual bool    FilterObjectByAllReadableFiles(
                        const FSRef&            inRef, 
                        const LSItemInfoRecord& inInfo, 
                        NavFilterModes          inFilterMode);
    //! Performs filtering when a document type is selected in the pop-up menu.
    virtual bool    FilterObjectByFilterName(
                        const FSRef&            inRef, 
                        const LSItemInfoRecord& inInfo, 
                        NavFilterModes          inFilterMode);
    //! Changes the current filter.
    virtual void    SelectFilter(
                        FilterKind              inFilterKind, 
                        const String&           inFilterName = String());
    //@}
    
    //! Preview hook.
    virtual bool    Preview(
                        NavCBRecPtr             ioParams);
    
    //! @name Overrides from NavDialog
    //@{
    virtual void    HandleUserAction(
                        NavUserAction           inUserAction);
    virtual void    HandleEvent(
                        NavEventCallbackMessage inSelector, 
                        NavCBRec&               ioParams);
    virtual void    DialogStarting(
                        NavCBRec&               ioParams);
    //@}
    
    // member variables
    PopupMenuContentsVector mPopupMenuContents; //<! The contents of the file type pop-up menu.
    FilterKind              mCurrentFilterKind; //!< The filter type of the currently selected item in the file type pop-up menu.
    String                  mCurrentFilterName; //!< The filter name of the currently selected item in the file type pop-up menu.
    
private:
    
    void    InitCustomView(const Rect& inFrame);
    void    PlaceCustomView(const Rect& inNewFrame);
    
    // callbacks
    static pascal Boolean   NavPreviewProc(
                                NavCBRecPtr     callBackParms,
                                void*           callBackUD);
    static pascal Boolean   NavFilterObjectProc(
                                AEDesc*         theItem, 
                                void*           info, 
                                void*           callBackUD, 
                                NavFilterModes  filterMode);
    
    // member variables
    WindowRef       mCustomViewWindow;
    Handle          mDITLHandle;
    DialogItemIndex mUserPaneIndex;
    HIViewRef       mUserPane;
    HISize          mLastTrySize;
};


// ==========================================================================================
//  AskSaveChangesDialog

#pragma mark -
#pragma mark * AskSaveChangesDialog *

#ifdef B_ASKSAVECHANGES_CB
#   error "Somebody defined B_ASKSAVECHANGES_CB ..."
#else
    //! Convenience macro to simplify callback declarations
#   define B_ASKSAVECHANGES_CB  boost::function2<void, const ASK_SAVE_CHANGES_DIALOG&, Action>
#endif

/*! @brief  Implements the standard "Save Changes" alert as per the Aqua HIG.

    Beyond the usual DialogModality and callback arguments that are common to all nav 
    dialogs, the Make/Start/Run functions also take as arguments:
    -# The display name of the file that's about to be closed.
    -# An value in the #Context enumeration, which indicates why the 
       file is being closed.
    The dialog's result is a value in the #Action enumeration.  An 
    enum is used here because this dialog has three buttons, not just the usual OK/Cancel 
    button pair.
    
    @sa     @ref dialogs_alerts_nav
*/
class AskSaveChangesDialog : public NavDialog
{
public:
    
    //! @name Constants
    //@{
    //! Enumerates the possible reasons for invoking this dialog.
    enum Context
    {
        kClosing    = kNavSaveChangesClosingDocument,       //!< The dialog is being invoked because a document is being closed.
        kQuitting   = kNavSaveChangesQuittingApplication,   //!< The dialog is being invoked because the application is quitting.
        kOther      = kNavSaveChangesOther                  //!< The dialog is being invoked for some other reason.
    };
    //! Enumerates the possible results of this dialog.
    enum Action
    {
        kSave       = kNavUserActionSaveChanges,        //!< The user clicked the "Save" button.
        kDontSave   = kNavUserActionDontSaveChanges,    //!< The user clicked the "Don't Save" button.
        kCancel     = kNavUserActionCancel              //!< The user clicked the "Cancel" button.
    };
    //@}
    
    //! @name Types
    //@{
    //! Convenience callback that does nothing.
    template <class ASK_SAVE_CHANGES_DIALOG> struct NullCallback
        { void  operator () (const ASK_SAVE_CHANGES_DIALOG&, Action) const {} };
    
    //@}
    
    //! @name Setup
    //@{
    //! Sets the dialog's "context" (i.e. is this for a Close or a Quit).
    void    SetContext(Context inContext);
    //! Sets the dialog's callback.
    template <class ASK_SAVE_CHANGES_DIALOG>
    void    SetCallback(B_ASKSAVECHANGES_CB inFunction);
    //@}
    
    //! @name Retrieving Results
    //@{
    //! Returns the action that dismissed the dialog.  Only useful in app-modal scenario.
    Action  GetAction() const;
    //@}
    
    /*! @name Making Dialogs
        
        In B parlance, "Making" a dialog means to instantiate it, returning a smart 
        pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
    */
    //@{
    //! Instantiates and returns an @a ASK_SAVE_CHANGES_DIALOG with the given arguments.
    template <class ASK_SAVE_CHANGES_DIALOG>
    static std::auto_ptr<ASK_SAVE_CHANGES_DIALOG>
                Make(
                    const DialogModality&   inModality,
                    const String&           inFileName,
                    Context                 inContext, 
                    B_ASKSAVECHANGES_CB     inFunction = NullCallback<ASK_SAVE_CHANGES_DIALOG>());
    //@}
    
    /*! @name Starting Dialogs
        
        In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
    */
    //@{
    //! Instantiates an @a ASK_SAVE_CHANGES_DIALOG with the given arguments, then enters it.
    template <class ASK_SAVE_CHANGES_DIALOG>
    static void Start(
                    const DialogModality&   inModality,
                    const String&           inFileName,
                    Context                 inContext, 
                    B_ASKSAVECHANGES_CB     inFunction = NullCallback<ASK_SAVE_CHANGES_DIALOG>());
    //@}
    
    /*! @name Running Dialogs
        
        These are convenience functions for displaying nav dialogs in an 
        application-modal fashion.  Because they don't return until the nav dialog 
        is dismissed, the chosen action is returned as the function result.
    */
    //@{
    //! Displays the nav dialog in an application-modal fashion.
    Action          Run();
    //! Instantiates an @a ASK_SAVE_CHANGES_DIALOG, then enters a modal event loop for it.
    template <class ASK_SAVE_CHANGES_DIALOG>
    static Action   Run(
                        const String&       inFileName,
                        Context             inContext);
    //@}
    
protected:
    
    //! Protected default constructor to discourage stack-based instantiation.
    AskSaveChangesDialog();
    
    //! @name Overrides from NavDialog
    //@{
    virtual NavDialogRef    CreateDialog();
    virtual void            HandleUserAction(
                                NavUserAction   inUserAction);
    //@}
    
    //! Helper function for configuring the dialog.
    void    Configure(
                const DialogModality&   inModality,
                const String&           inFileName,
                Context                 inContext);
    
private:
    
    typedef DialogCallbackBase1<AskSaveChangesDialog, Action>   CallbackType;
    
    static Action   ConvertUserAction(NavUserAction inUserAction);
    
    // member variables
    Context                     mContext;
    std::auto_ptr<CallbackType> mCallback;
};

// ------------------------------------------------------------------------------------------
/*! @todo                           Complete this!
    @param  ASK_SAVE_CHANGES_DIALOG Template parameter.  The class of the instantiated 
                                    window.  Must be AskSaveChangesDialog or a class derived 
                                    from AskSaveChangesDialog.
*/
template <class ASK_SAVE_CHANGES_DIALOG> void
AskSaveChangesDialog::SetCallback(
    B_ASKSAVECHANGES_CB inFunction)
{
    boost::function_requires< boost::ConvertibleConcept<ASK_SAVE_CHANGES_DIALOG*, B::AskSaveChangesDialog*> >();
    
    mCallback.reset(new DialogCallback1<ASK_SAVE_CHANGES_DIALOG, AskSaveChangesDialog, Action>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_SAVE_CHANGES_DIALOG.  The instance of 
    @a ASK_SAVE_CHANGES_DIALOG is then configured with the given arguments.  Finally the 
    dialog is returned, ready to be displayed via Enter().
    
    @param  ASK_SAVE_CHANGES_DIALOG Template parameter.  The class of the instantiated 
                                    window.  Must be AskSaveChangesDialog or a class derived 
                                    from AskSaveChangesDialog.
    
    @return The @a ASK_SAVE_CHANGES_DIALOG object, wrapped in an @c std::auto_ptr<>.  
            This implies that control over the object's lifetime is given to the caller.
*/
template <class ASK_SAVE_CHANGES_DIALOG>
std::auto_ptr<ASK_SAVE_CHANGES_DIALOG>
AskSaveChangesDialog::Make(
    const DialogModality&   inModality, //!< The dialog's modality.
    const String&           inFileName, //!< The display name of the file we're being asked to save.
    Context                 inContext,  //!< The context in which we're being called.
    B_ASKSAVECHANGES_CB     inFunction /* = NullCallback<ASK_SAVE_CHANGES_DIALOG>() */) //!< A callback that is invoked when the dialog is dismissed.
{
    boost::function_requires< ProtectedDefaultConstructorConcept<ASK_SAVE_CHANGES_DIALOG> >();
    boost::function_requires< boost::ConvertibleConcept<ASK_SAVE_CHANGES_DIALOG*, B::AskSaveChangesDialog*> >();
    
    std::auto_ptr<ASK_SAVE_CHANGES_DIALOG>  dialogPtr(new ASK_SAVE_CHANGES_DIALOG);
    
    dialogPtr->Configure(inModality, inFileName, inContext);
    dialogPtr->SetCallback(inFunction);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_SAVE_CHANGES_DIALOG, as per Make().  The 
    dialog is then displayed via Enter().  Whether the dialog has been dismissed by the 
    time the function returns depends upon @a inModality.  See Enter() for more information.
    
    @note   The dialog is automatically disposed upon dismissal.  If you would rather 
            hang on to the dialog, call Make() then Enter().
    
    @param  ASK_SAVE_CHANGES_DIALOG Template parameter.  The class of the instantiated 
                                    window.  Must be AskSaveChangesDialog or a class derived 
                                    from AskSaveChangesDialog.
    
    @sa     Make(), NavDialog::Enter()
*/
template <class ASK_SAVE_CHANGES_DIALOG> void
AskSaveChangesDialog::Start(
    const DialogModality&   inModality, //!< The dialog's modality.
    const String&           inFileName, //!< The display name of the file we're being asked to save.
    Context                 inContext,  //!< The context in which we're being called.
    B_ASKSAVECHANGES_CB     inFunction /* = NullCallback<ASK_SAVE_CHANGES_DIALOG>() */) //!< A callback that is invoked when the dialog is dismissed.
{
    Enter(Make<ASK_SAVE_CHANGES_DIALOG>(inModality, inFileName, inContext, inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_SAVE_CHANGES_DIALOG, as per Make().  Then the 
    dialog's modality is set to application-modal, and it is entered.  Control does not 
    return from this function until the dialog is dismissed.
    
    @param  ASK_SAVE_CHANGES_DIALOG Template parameter.  The class of the instantiated 
                                    window.  Must be AskSaveChangesDialog or a class derived 
                                    from AskSaveChangesDialog.
    
    @return The Action that dismissed the nav dialog.
    
    @sa     Make(), Run()
*/
template <class ASK_SAVE_CHANGES_DIALOG>
AskSaveChangesDialog::Action
AskSaveChangesDialog::Run(
    const String&           inFileName, //!< The display name of the file we're being asked to save.
    Context                 inContext)  //!< The context in which we're being called.
{
    std::auto_ptr<ASK_SAVE_CHANGES_DIALOG>  dialogPtr;
    
    dialogPtr = Make<ASK_SAVE_CHANGES_DIALOG>(DialogModality::Modal(), inFileName, 
                                              inContext);
    
    // Enter the dialog & wait for dismissal, and return the dismissing command.
    return (dialogPtr->Run());
}

#undef B_ASKSAVECHANGES_CB


// ==========================================================================================
//  AskDiscardChangesDialog

#pragma mark -
#pragma mark * AskDiscardChangesDialog *

#ifdef B_ASKDISCARDCHANGES_CB
#   error "Somebody defined B_ASKDISCARDCHANGES_CB ..."
#else
    //! Convenience macro to simplify callback declarations
#   define B_ASKDISCARDCHANGES_CB   boost::function2<void, const ASK_DISCARD_CHANGES_DIALOG&, bool>
#endif

/*! @brief  Implements the standard "Discard Changes" alert.
    
    This dialog is useful for implementing the Revert command.

    Beyond the usual DialogModality and callback arguments, the Make/Start/Run functions 
    also take as an argument the display name of the file that's about to be closed.  The 
    dialog returns @c true if the user clicked the Discard button; else it returns @c false.
    
    @sa     @ref dialogs_alerts_nav
*/
class AskDiscardChangesDialog : public NavDialog
{
public:
    
    //! @name Types
    //@{
    //! Convenience callback that does nothing.
    template <class ASK_DISCARD_CHANGES_DIALOG> struct NullCallback
        { void  operator () (const ASK_DISCARD_CHANGES_DIALOG&, bool) const {} };
    //@}
    
    //! @name Setup
    //@{
    //! Sets the dialog's callback.
    template <class ASK_DISCARD_CHANGES_DIALOG>
    void    SetCallback(B_ASKDISCARDCHANGES_CB inFunction);
    //@}
    
    //! @name Retrieving Results
    //@{
    //! Returns @c true if the user clicked the Discard button.  Only useful in app-modal scenario.
    bool    GetAction() const;
    //@}
    
    /*! @name Making Dialogs
        
        In B parlance, "Making" a dialog means to instantiate it, returning a smart 
        pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
    */
    //@{
    //! Instantiates and returns a @a ASK_DISCARD_CHANGES_DIALOG with the given arguments.
    template <class ASK_DISCARD_CHANGES_DIALOG>
    static std::auto_ptr<ASK_DISCARD_CHANGES_DIALOG>
                Make(
                    const DialogModality&   inModality,
                    const String&           inFileName,
                    B_ASKDISCARDCHANGES_CB  inFunction = NullCallback<ASK_DISCARD_CHANGES_DIALOG>());
    //@}
    
    /*! @name Starting Dialogs
        
        In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
    */
    //@{
    //! Instantiates an @a ASK_DISCARD_CHANGES_DIALOG with the given arguments, then enters it.
    template <class ASK_DISCARD_CHANGES_DIALOG>
    static void Start(
                    const DialogModality&   inModality,
                    const String&           inFileName,
                    B_ASKDISCARDCHANGES_CB  inFunction = NullCallback<ASK_DISCARD_CHANGES_DIALOG>());
    //@}
    
    /*! @name Running Dialogs
        
        These are convenience functions for displaying nav dialogs in an 
        application-modal fashion.  Because they don't return until the nav dialog 
        is dismissed, the chosen action is returned as the function result.
    */
    //@{
    //! Displays the nav dialog in an application-modal fashion.
    bool        Run();
    //! Instantiates an @a ASK_SAVE_CHANGES_DIALOG, then enters a modal event loop for it.
    template <class ASK_DISCARD_CHANGES_DIALOG>
    static bool Run(
                    const String&           inFileName);
    //@}
    
protected:

    //! Protected default constructor to discourage stack-based instantiation.
    AskDiscardChangesDialog();
    
    //! @name Overrides from NavDialog
    //@{
    virtual NavDialogRef    CreateDialog();
    virtual void            HandleUserAction(
                                NavUserAction   inUserAction);
    //@}
    
    //! Helper function for configuring the dialog.
    void    Configure(
                const DialogModality&   inModality,
                const String&           inFileName);
    
private:
    
    typedef DialogCallbackBase1<AskDiscardChangesDialog, bool>  CallbackType;
    
    static bool ConvertUserAction(NavUserAction inUserAction);
    
    // member variables
    std::auto_ptr<CallbackType> mCallback;
};

// ------------------------------------------------------------------------------------------
/*! @todo                               Complete this!
    @param  ASK_DISCARD_CHANGES_DIALOG  Template parameter.  The class of the instantiated 
                                        window.  Must be AskDiscardChangesDialog or a class 
                                        derived from AskDiscardChangesDialog.
*/
template <class ASK_DISCARD_CHANGES_DIALOG> void
AskDiscardChangesDialog::SetCallback(
    B_ASKDISCARDCHANGES_CB  inFunction)
{
    boost::function_requires< boost::ConvertibleConcept<ASK_DISCARD_CHANGES_DIALOG*, B::AskDiscardChangesDialog*> >();
    
    mCallback.reset(new DialogCallback1<ASK_DISCARD_CHANGES_DIALOG, AskDiscardChangesDialog, bool>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a dialog of class @a ASK_DISCARD_CHANGES_DIALOG.  The instance of 
    @a ASK_DISCARD_CHANGES_DIALOG is then configured with the given arguments.  Finally 
    the dialog is returned, ready to be displayed via Enter().
    
    @param  ASK_DISCARD_CHANGES_DIALOG  Template parameter.  The class of the instantiated 
                                        window.  Must be AskDiscardChangesDialog or a class 
                                        derived from AskDiscardChangesDialog.
*/
template <class ASK_DISCARD_CHANGES_DIALOG>
std::auto_ptr<ASK_DISCARD_CHANGES_DIALOG>
AskDiscardChangesDialog::Make(
    const DialogModality&   inModality, //!< The dialog's modality.
    const String&           inFileName, //!< The display name of the file we're being asked to revert.
    B_ASKDISCARDCHANGES_CB  inFunction /* = NullCallback<ASK_DISCARD_CHANGES_DIALOG>() */)  //!< A callback that is invoked when the dialog is dismissed.
{
    boost::function_requires< ProtectedDefaultConstructorConcept<ASK_DISCARD_CHANGES_DIALOG> >();
    boost::function_requires< boost::ConvertibleConcept<ASK_DISCARD_CHANGES_DIALOG*, B::AskDiscardChangesDialog*> >();
    
    std::auto_ptr<ASK_DISCARD_CHANGES_DIALOG>   dialogPtr(new ASK_DISCARD_CHANGES_DIALOG);
    
    dialogPtr->Configure(inModality, inFileName);
    dialogPtr->SetCallback(inFunction);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_DISCARD_CHANGES_DIALOG, as per Make().  The 
    dialog is then displayed via Enter().  Whether the dialog has been dismissed by the 
    time the function returns depends upon @a inModality.  See Enter() for more information.
    
    @note   The dialog is automatically disposed upon dismissal.  If you would rather 
            hang on to the dialog, call Make() then Enter().
    
    @param  ASK_DISCARD_CHANGES_DIALOG  Template parameter.  The class of the instantiated 
                                        window.  Must be AskDiscardChangesDialog or a class 
                                        derived from AskDiscardChangesDialog.
    
    @sa     Make(), NavDialog::Enter()
*/
template <class ASK_DISCARD_CHANGES_DIALOG> void
AskDiscardChangesDialog::Start(
    const DialogModality&   inModality, //!< The dialog's modality.
    const String&           inFileName, //!< The display name of the file we're being asked to save.
    B_ASKDISCARDCHANGES_CB  inFunction /* = NullCallback<ASK_DISCARD_CHANGES_DIALOG>() */)  //!< A callback that is invoked when the dialog is dismissed.
{
    Enter(Make<ASK_DISCARD_CHANGES_DIALOG>(inModality, inFileName, inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_DISCARD_CHANGES_DIALOG, as per Make().  
    Then the dialog's modality is set to application-modal, and it is entered.  Control 
    does not return from this function until the dialog is dismissed.
    
    @param  ASK_DISCARD_CHANGES_DIALOG  Template parameter.  The class of the instantiated 
                                        window.  Must be AskDiscardChangesDialog or a class 
                                        derived from AskDiscardChangesDialog.
    
    @return @c true if the user clicked the Discard button.
    
    @sa     Make(), Run()
*/
template <class ASK_DISCARD_CHANGES_DIALOG> bool
AskDiscardChangesDialog::Run(
    const String&           inFileName) //!< The display name of the file we're being asked to save.
{
    std::auto_ptr<ASK_DISCARD_CHANGES_DIALOG>   dialogPtr;
    
    dialogPtr = Make<ASK_DISCARD_CHANGES_DIALOG>(DialogModality::Modal(), inFileName);
    
    // Enter the dialog & wait for dismissal, and return the dismissing command.
    return (dialogPtr->Run());
}

#undef B_ASKSAVECHANGES_CB


// ==========================================================================================
//  AskReviewDocumentsDialog

#pragma mark -
#pragma mark * AskReviewDocumentsDialog *

#ifdef B_ASKREVIEWDOCS_CB
#   error "Somebody defined B_ASKREVIEWDOCS_CB ..."
#else
    //! Convenience macro to simplify callback declarations
#   define B_ASKREVIEWDOCS_CB   boost::function2<void, const ASK_REVIEW_DOCUMENTS_DIALOG&, Action>
#endif

/*! @brief  Implements the standard "Review Changes" alert as per the Aqua HIG.
    
    Beyond the usual DialogModality and callback arguments, the Make/Start/Run functions 
    also take as an argument the number of documents containing unsaved changes (i.e., the 
    number of "dirty" documents).  The dialog's result is a value in the 
    #Action enumeration.  An enum is used here because this 
    dialog has three buttons, not just the usual OK/Cancel  button pair.
    
    @note   Developers will rarely need to invoke this dialog, because 
            MultipleDocumentPolicy already does so automatically.
    
    @note   The Aqua HIG specify that this dialog should only be displayed 
            if there is more than one modified document.
    
    @note   The Aqua HIG specify that this alert should always be application-modal;  
            support for sheets is included for completeness only.
    
    @sa     @ref dialogs_alerts_nav
*/
class AskReviewDocumentsDialog : public NavDialog
{
public:
    
    //! @name Constants
    //@{
    //! Enumerates the possible results of this dialog.
    enum Action
    {
        kReview     = kNavUserActionReviewDocuments,    //!< The user clicked the "Review Changes" button.
        kDiscard    = kNavUserActionDiscardDocuments,   //!< The user clicked the "Discard Changes" button.
        kCancel     = kNavUserActionCancel              //!< The user clicked the "Cancel" button.
    };
    //@}
    
    //! @name Types
    //@{
    //! Convenience callback that does nothing.
    template <class ASK_REVIEW_DOCUMENTS_DIALOG> struct NullCallback
        { void  operator () (const ASK_REVIEW_DOCUMENTS_DIALOG&, Action) const {} };
    //@}
    
    //! @name Setup
    //@{
    //! Specifies the number of modified documents.
    void    SetDocumentCount(UInt32 inDocumentCount);
    //! Sets the dialog's callback.
    template <class ASK_REVIEW_DOCUMENTS_DIALOG>
    void    SetCallback(B_ASKREVIEWDOCS_CB inFunction);
    //@}
    
    //! @name Retrieving Results
    //@{
    //! Returns the action that dismissed the dialog.  Only useful in app-modal scenario.
    Action  GetAction() const;
    //@}
    
    /*! @name Making Dialogs
        
        In B parlance, "Making" a dialog means to instantiate it, returning a smart 
        pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
    */
    //@{
    //! Instantiates and returns an @a ASK_REVIEW_DOCUMENTS_DIALOG with the given arguments.
    template <class ASK_REVIEW_DOCUMENTS_DIALOG>
    static std::auto_ptr<ASK_REVIEW_DOCUMENTS_DIALOG>
                Make(
                    const DialogModality&   inModality,
                    UInt32                  inDocumentCount, 
                    B_ASKREVIEWDOCS_CB      inFunction = NullCallback<ASK_REVIEW_DOCUMENTS_DIALOG>());
    //@}
    
    /*! @name Starting Dialogs
        
        In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
    */
    //@{
    //! Instantiates an @a ASK_REVIEW_DOCUMENTS_DIALOG with the given arguments, then enters it.
    template <class ASK_REVIEW_DOCUMENTS_DIALOG>
    static void Start(
                    const DialogModality&   inModality,
                    UInt32                  inDocumentCount, 
                    B_ASKREVIEWDOCS_CB      inFunction = NullCallback<ASK_REVIEW_DOCUMENTS_DIALOG>());
    //@}
    
    /*! @name Running Dialogs
        
        These are convenience functions for displaying nav dialogs in an 
        application-modal fashion.  Because they don't return until the nav dialog 
        is dismissed, the chosen action is returned as the function result.
    */
    //@{
    //! Displays the nav dialog in an application-modal fashion.
    Action          Run();
    //! Instantiates an @a ASK_REVIEW_DOCUMENTS_DIALOG, then enters a modal event loop for it.
    template <class ASK_REVIEW_DOCUMENTS_DIALOG>
    static Action   Run(
                        UInt32                  inDocumentCount);
    //@}
    
protected:

    //! Protected default constructor to discourage stack-based instantiation.
    AskReviewDocumentsDialog();
    
    //! @name Overrides from NavDialog
    //@{
    virtual NavDialogRef    CreateDialog();
    virtual void            HandleUserAction(
                                NavUserAction   inUserAction);
    //@}
    
    //! Helper function for configuring the dialog.
    void    Configure(
                const DialogModality&   inModality,
                UInt32                  inDocumentCount);
    
private:
    
    typedef DialogCallbackBase1<AskReviewDocumentsDialog, Action>   CallbackType;
    
    static Action   ConvertUserAction(NavUserAction inUserAction);
    
    // member variables
    UInt32                      mDocumentCount;
    std::auto_ptr<CallbackType> mCallback;
};

// ------------------------------------------------------------------------------------------
/*! @todo                               Complete this!
    @param  ASK_REVIEW_DOCUMENTS_DIALOG Template parameter.  The class of the instantiated 
                                        window.  Must be AskReviewDocumentsDialog or a class 
                                        derived from AskReviewDocumentsDialog.
*/
template <class ASK_REVIEW_DOCUMENTS_DIALOG> void
AskReviewDocumentsDialog::SetCallback(
    B_ASKREVIEWDOCS_CB      inFunction)
{
    boost::function_requires< boost::ConvertibleConcept<ASK_REVIEW_DOCUMENTS_DIALOG*, B::AskReviewDocumentsDialog*> >();
    
    mCallback.reset(new DialogCallback1<ASK_REVIEW_DOCUMENTS_DIALOG, AskReviewDocumentsDialog, Action>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a dialog of class @a ASK_REVIEW_DOCUMENTS_DIALOG.  The instance of 
    @a ASK_REVIEW_DOCUMENTS_DIALOG is then configured with the given arguments.  Finally 
    the dialog is returned, ready to be displayed via Enter().
    
    @param  ASK_REVIEW_DOCUMENTS_DIALOG Template parameter.  The class of the instantiated 
                                        window.  Must be AskReviewDocumentsDialog or a class 
                                        derived from AskReviewDocumentsDialog.
    
    @return The @a ASK_REVIEW_DOCUMENTS_DIALOG object, wrapped in an @c std::auto_ptr<>.  
            This implies that control over the object's lifetime is given to the caller.
*/
template <class ASK_REVIEW_DOCUMENTS_DIALOG>
std::auto_ptr<ASK_REVIEW_DOCUMENTS_DIALOG>
AskReviewDocumentsDialog::Make(
    const DialogModality&   inModality,         //!< The dialog's modality.
    UInt32                  inDocumentCount,    //!< The number of modified documents.  Should be \> 1.
    B_ASKREVIEWDOCS_CB      inFunction /* = NullCallback<ASK_REVIEW_DOCUMENTS_DIALOG>() */) //!< A callback that is invoked when the dialog is dismissed.
{
    boost::function_requires< ProtectedDefaultConstructorConcept<ASK_REVIEW_DOCUMENTS_DIALOG> >();
    boost::function_requires< boost::ConvertibleConcept<ASK_REVIEW_DOCUMENTS_DIALOG*, B::AskReviewDocumentsDialog*> >();
    
    std::auto_ptr<ASK_REVIEW_DOCUMENTS_DIALOG>  dialogPtr(new ASK_REVIEW_DOCUMENTS_DIALOG);
    
    dialogPtr->Configure(inModality, inDocumentCount);
    dialogPtr->SetCallback(inFunction);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_REVIEW_DOCUMENTS_DIALOG, as per Make().  The 
    dialog is then displayed via Enter().  Whether the dialog has been dismissed by the 
    time the function returns depends upon @a inModality.  See Enter() for more information.
    
    @note   The dialog is automatically disposed upon dismissal.  If you would rather 
            hang on to the dialog, call Make() then Enter().
    
    @param  ASK_REVIEW_DOCUMENTS_DIALOG Template parameter.  The class of the instantiated 
                                        window.  Must be AskReviewDocumentsDialog or a class 
                                        derived from AskReviewDocumentsDialog.
    
    @sa     Make(), NavDialog::Enter()
*/
template <class ASK_REVIEW_DOCUMENTS_DIALOG> void
AskReviewDocumentsDialog::Start(
    const DialogModality&   inModality,         //!< The dialog's modality.
    UInt32                  inDocumentCount,    //!< The number of modified documents.  Should be \> 1.
    B_ASKREVIEWDOCS_CB      inFunction /* = NullCallback<ASK_REVIEW_DOCUMENTS_DIALOG>() */) //!< A callback that is invoked when the dialog is dismissed.
{
    Enter(Make<ASK_REVIEW_DOCUMENTS_DIALOG>(inModality, inDocumentCount, inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a ASK_REVIEW_DOCUMENTS_DIALOG, as per Make().  
    Then the dialog's modality is set to application-modal, and it is entered.  Control 
    does not return from this function until the dialog is dismissed.
    
    @param  ASK_REVIEW_DOCUMENTS_DIALOG Template parameter.  The class of the instantiated 
                                        window.  Must be AskReviewDocumentsDialog or a class 
                                        derived from AskReviewDocumentsDialog.
    
    @return The Action that dismissed the nav dialog.
    
    @sa     Make(), Run()
*/
template <class ASK_REVIEW_DOCUMENTS_DIALOG>
AskReviewDocumentsDialog::Action
AskReviewDocumentsDialog::Run(
    UInt32                  inDocumentCount)    //!< The number of modified documents.  Should be \> 1.
{
    std::auto_ptr<ASK_REVIEW_DOCUMENTS_DIALOG>  dialogPtr;
    
    dialogPtr = Make<ASK_REVIEW_DOCUMENTS_DIALOG>(DialogModality::Modal(), inDocumentCount);
    
    // Enter the dialog & wait for dismissal, and return the dismissing command.
    return (dialogPtr->Run());
}

#undef B_ASKREVIEWDOCS_CB


// ==========================================================================================
//  GetFileDialog

#pragma mark -
#pragma mark * GetFileDialog *

#ifdef B_GETFILEDIALOG_CB
#   error "Somebody defined B_GETFILEDIALOG_CB ..."
#else
    //! Convenience macro to simplify callback declarations
#   define B_GETFILEDIALOG_CB   boost::function3<void, const GET_FILE_DIALOG&, bool, const NavReplyRecord&>
#endif

/*! @brief  Implements the standard "Open" dialog as per the Aqua HIG.
    
    Unlike the simpler nav dialogs derived directly from NavDialog, GetFileDialog doesn't 
    allow for passing arguments (beyond the DialogModality and callback) to the 
    Make/Start/Run functions.  That's because the configuration of this dialog can be 
    quite complex;  so the task has been left to clients.

    GetFileDialog's configuration functions all have to do with the file type pop-up menu.
    This is because Navigation Services' filtering API is inordinately hard to use, since 
    it's based on matching files with creators & file types, and on Mac OS X that's not 
    very useful.

    A number of factors affect the file type pop-up menu:

    - The @c kNavNoTypePopup flag.  This flag controls the visibility of the 
      pop-up menu and takes precedence over other options.  If the flag is set, 
      then each file is matched against all of the entries in the document types 
      array.  The flag can be changed with SetFlag()/ClearFlag().
    - The document types array.  This array is set with SetDocumentTypes().  There are 
      various scenarios, depending on the number of elements in the array:
      -# If the array is empty, then all files will match successfully, regardless 
         of extension or file type.
      -# If the array contains only one element, then the pop-up menu is not displayed 
         (unless other factors cause it to be displayed) and files are matched against 
         the element.
      -# If the array contains more than one element, then the pop-up menu is displayed 
         and contains one item per element.  When one of the items in the pop-up menu 
         is selected, the files are matched against the corresponding document type 
         entry.  In addition, an item called "All Readable Documents" is prepended to 
         the pop-up menu which causes the files to be matched against @e all of the 
         document type entries.
    
    @sa     @ref dialogs_alerts_nav
*/
class GetFileDialog : public NavReplyDialog
{
public:
    
    //! @name Types
    //@{
    //! Convenience callback that does nothing.
    template <class GET_FILE_DIALOG> struct NullCallback
        { void  operator () (const GET_FILE_DIALOG&, bool, const NavReplyRecord&) const {} };
    //@}
    
    //! @name Setup
    //@{
    //! Allows the user to select any of the document types declared in @a inBundle's Info.plist file.
    void    SetDocumentTypes(
                const Bundle&       inBundle);
    //! Allows the user to select any file from the document types contained in the iterator sequence.
    template <class InputIterator>
    void    SetDocumentTypes(
                const Bundle&       inBundle, 
                CFStringRef         inTable,
                InputIterator       inFirst, 
                InputIterator       inLast);
    //! Initially select the pop-up menu item for the given document type.
    void    SelectDocumentType(
                const String&       inDocTypeName);
    //! Sets the dialog's callback.
    template <class GET_FILE_DIALOG>
    void    SetCallback(
                B_GETFILEDIALOG_CB  inFunction);
    //@}
    
    /*! @name Making Dialogs
        
        In B parlance, "Making" a dialog means to instantiate it, returning a smart 
        pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
        
        Unlike the simpler Nav dialogs (eg AskSaveChangesDialog), it's not possible 
        to configure the dialog completely with Make() except in the simplest of 
        cases.  This is because there are many more configuration options in 
        this dialog.
    */
    //@{
    //! Instantiates and returns a @a GET_FILE_DIALOG with the given arguments.
    template <class GET_FILE_DIALOG>
    static std::auto_ptr<GET_FILE_DIALOG>
                Make(
                    const DialogModality&   inModality,
                    B_GETFILEDIALOG_CB      inFunction = NullCallback<GET_FILE_DIALOG>());
    //@}
    
    /*! @name Starting Dialogs
        
        In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
    */
    //@{
    //! Instantiates a @a GET_FILE_DIALOG with the given arguments, then enters it.
    template <class GET_FILE_DIALOG>
    static void Start(
                    const DialogModality&   inModality,
                    B_GETFILEDIALOG_CB      inFunction = NullCallback<GET_FILE_DIALOG>());
    //@}
    
    /*! @name Running Dialogs
        
        These are convenience functions for displaying nav dialogs in an 
        application-modal fashion.  Because they don't return until the nav dialog 
        is dismissed, the chosen action is returned as the function result.
    */
    //@{
    //! Displays the nav dialog in an application-modal fashion.
    bool        Run(
                    AutoNavReply&           outNavReply, 
                    bool                    inUnused);
    //! Instantiates a @a GET_FILE_DIALOG, then enters a modal event loop for it.
    template <class GET_FILE_DIALOG>
    static bool Run(
                    AutoNavReply&           outNavReply);
    //@}
    
protected:
    
    //! Protected default constructor to discourage stack-based instantiation.
    GetFileDialog();
    
    //! @name Overrides from NavReplyDialog
    //@{
    virtual NavDialogRef
                    CreateDialog();
    virtual void    HandleReply(
                        NavUserAction           inUserAction, 
                        NavReplyRecord&         ioNavReply);
    virtual bool    FilterObjectByAllReadableFiles(
                        const FSRef&            inRef, 
                        const LSItemInfoRecord& inInfo, 
                        NavFilterModes          inFilterMode);
    virtual bool    FilterObjectByFilterName(
                        const FSRef&            inRef, 
                        const LSItemInfoRecord& inInfo, 
                        NavFilterModes          inFilterMode);
    virtual void    PopupMenuSelect(
                        const NavMenuItemSpec&  inMenuItemSpec);
    //@}
    
    void    FillPopupMenuWithDocTypes(
                const Bundle&       inBundle, 
                CFStringRef         inTable);
    
private:
    
    typedef DialogCallbackBase2<GetFileDialog, bool, const NavReplyRecord&> CallbackType;
    
    // member variables
    Bundle::DocumentTypeArray   mDocTypes;
    Bundle::Iterator            mCurrentDocType;
    std::auto_ptr<CallbackType> mCallback;
};

// ------------------------------------------------------------------------------------------
/*! Sets the document types array to be equal to the iterator sequence 
    <tt>[inFirst, inLast)</tt>.  The array is then used to populate the file type pop-up 
    menu.  @a inBundle and @a inTable are used to read the localised strings for each 
    document type.  If the array has more than one element, the menu is forced to be 
    displayed;  note that this can be overridden by a subsequent call to 
    @c SetFlags(kNavNoTypePopup).  Finally, the current filter is changed to match files 
    matching any of the doc types.
    
    @param  InputIterator   Template parameter.  An input iterator yielding a Bundle::DocumentType object.
*/
template <class InputIterator> void
GetFileDialog::SetDocumentTypes(
    const Bundle&   inBundle,   //!< The bundle in which to look for localised document type names.
    CFStringRef     inTable,    //!< The table in which to look for localised document type names (may be @c NULL).
    InputIterator   inFirst,    //!< The start of the sequence of document type objects.
    InputIterator   inLast)     //!< The end of the sequence of document type objects.
{
    mDocTypes.assign(inFirst, inLast);
    FillPopupMenuWithDocTypes(inBundle, inTable);
}

// ------------------------------------------------------------------------------------------
/*! @todo                   Complete this!
    @param  GET_FILE_DIALOG Template parameter.  The class of the instantiated dialog.  
                            Must be GetFileDialog or a class derived from GetFileDialog.
*/
template <class GET_FILE_DIALOG> void
GetFileDialog::SetCallback(
    B_GETFILEDIALOG_CB  inFunction)
{
    boost::function_requires< boost::ConvertibleConcept<GET_FILE_DIALOG*, B::GetFileDialog*> >();
    
    mCallback.reset(new DialogCallback2<GET_FILE_DIALOG, GetFileDialog, bool, const NavReplyRecord&>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a dialog of class @a GET_FILE_DIALOG.  The instance of @a GET_FILE_DIALOG 
    is then configured with the given arguments.  Finally the dialog is returned, ready to 
    be displayed via Enter().
    
    @param  GET_FILE_DIALOG Template parameter.  The class of the instantiated window.  
                            Must be GetFileDialog or a class derived from GetFileDialog.
    
    @return The @a GET_FILE_DIALOG object, wrapped in an @c std::auto_ptr<>.  
            This implies that control over the object's lifetime is given to the caller.
*/
template <class GET_FILE_DIALOG>
std::auto_ptr<GET_FILE_DIALOG>
GetFileDialog::Make(
    const DialogModality&   inModality, //!< The dialog's modality.
    B_GETFILEDIALOG_CB      inFunction /*  = NullCallback<GET_FILE_DIALOG>() */)    //!< A callback that is invoked when the dialog is dismissed.
{
    boost::function_requires< ProtectedDefaultConstructorConcept<GET_FILE_DIALOG> >();
    boost::function_requires< boost::ConvertibleConcept<GET_FILE_DIALOG*, B::GetFileDialog*> >();
    
    std::auto_ptr<GET_FILE_DIALOG>  dialogPtr(new GET_FILE_DIALOG);
    
    dialogPtr->SetModality(inModality);
    dialogPtr->SetCallback(inFunction);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a GET_FILE_DIALOG, as per Make().  The 
    dialog is then displayed via Enter().  Whether the dialog has been dismissed by the 
    time the function returns depends upon @a inModality.  See Enter() for more information.
    
    @note   The dialog is automatically disposed upon dismissal.  If you would rather 
            hang on to the dialog, call Make() then Enter().
    
    @param  GET_FILE_DIALOG Template parameter.  The class of the instantiated 
                            window.  Must be GetFileDialog or a class derived 
                            from GetFileDialog.
    
    @sa     Make(), NavDialog::Enter()
*/
template <class GET_FILE_DIALOG> void
GetFileDialog::Start(
    const DialogModality&   inModality, //!< The dialog's modality.
    B_GETFILEDIALOG_CB      inFunction /*  = NullCallback<GET_FILE_DIALOG>() */)    //!< A callback that is invoked when the dialog is dismissed.
{
    Enter(Make<GET_FILE_DIALOG>(inModality, inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a GET_FILE_DIALOG, as per Make().  Then the 
    dialog's modality is set to application-modal, and it is entered.  Control does not 
    return from this function until the dialog is dismissed.
    
    @param  GET_FILE_DIALOG Template parameter.  The class of the instantiated 
                            window.  Must be GetFileDialog or a class derived 
                            from GetFileDialog.
    
    @return @c true if the user selected at least one file.
    
    @sa     Make(), Run()
*/
template <class GET_FILE_DIALOG> bool
GetFileDialog::Run(
    AutoNavReply&           outNavReply)    //!< The nav reply;  only valid if the function returns @c true.
{
    std::auto_ptr<GET_FILE_DIALOG>  dialogPtr;
    
    dialogPtr = Make<GET_FILE_DIALOG>(DialogModality::Modal());
    
    // Enter the dialog, wait for dismissal, & return result.
    return (dialogPtr->Run(outNavReply, true));
}

#undef B_GETFILEDIALOG_CB


// ==========================================================================================
//  PutFileDialog

#pragma mark -
#pragma mark * PutFileDialog *

#ifdef B_PUTFILEDIALOG_CB
#   error "Somebody defined B_PUTFILEDIALOG_CB ..."
#else
    //! Convenience macro to simplify callback declarations
#   define B_PUTFILEDIALOG_CB   boost::function4<void, const PUT_FILE_DIALOG&, bool, NavReplyRecord&, const SaveInfo&>
#endif

/*! @brief  Implements the standard "Save" dialog as per the Aqua HIG.
    
    Beyond the usual DialogModality and callback arguments, the Make/Start/Run functions 
    also take as an argument the suggest name of the file that's about to be saved.  Note 
    that this must be the actual file name, not the file's display name as is the case for 
    AskSaveChangesDialog and AskDiscardChangesDialog.  The dialog returns @c true if the 
    user clicked the Save button; else it returns @c false.  Note that there are important 
    configuration functions that aren't called when invoking Start() or Run(), so a bit 
    more work is required on the part of clients wishing to use them.

    PutFileDialog's configuration functions all have to do with the file type pop-up menu.  
    They are very similar to the functions in GetFileDialog:

    - SetDocumentTypes() allows one to specify the document types displayed in the pop-up 
      menu.  Unlike the version in GetFileDialog however, this one can optionally take an 
      extra string argument containing the "default" document type.  The default document 
      type is placed at the top of the pop-up menu and a separator is inserted between it 
      and the other menu items.
    - SelectDocumentType() allows one to specify the "current" document type.

    Note that unlike GetFileDialog, changing the contents of the pop-up menu or the 
    currently selected item doesn't affect the filtering of file system objects in the 
    browser portion of the dialog.
    
    @sa     @ref dialogs_alerts_nav
*/
class PutFileDialog : public NavReplyDialog
{
public:
    
    //! @name Types
    //@{
    //! Convenience callback that does nothing.
    template <class PUT_FILE_DIALOG> struct NullCallback
        { void  operator () (const PUT_FILE_DIALOG&, bool, NavReplyRecord&, const SaveInfo&) const {} };
    //@}
    
    //! @name Setup
    //@{
    //! Allows the user to select any of the document types declared in @a inBundle's Info.plist file.
    void    SetDocumentTypes(
                const Bundle&       inBundle, 
                const String&       inDefaultType = String());
    //! Allows the user to select any file from the document types contained in the iterator sequence.
    template <class InputIterator>
    void    SetDocumentTypes(
                const Bundle&       inBundle, 
                CFStringRef         inTable,
                InputIterator       inFirst, 
                InputIterator       inLast, 
                const String&       inDefaultType = String());
    //! Initially select the pop-up menu item for the given document type.
    void    SelectDocumentType(
                const String&       inDocTypeName);
    virtual void
            SetSaveInfo(
                const SaveInfo&     inSaveInfo);
    template <class PUT_FILE_DIALOG>
    void    SetCallback(
                B_PUTFILEDIALOG_CB  inFunction);
    //@}
    
    /*! @name Making Dialogs
        
        In B parlance, "Making" a dialog means to instantiate it, returning a smart 
        pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
        
        Unlike the simpler Nav dialogs (eg AskSaveChangesDialog), it's not possible 
        to configure the dialog completely with Make() except in the simplest of 
        cases.  This is because there are many more configuration options in 
        this dialog.
    */
    //@{
    //! Instantiates and returns a @a PUT_FILE_DIALOG with the given arguments.
    template <class PUT_FILE_DIALOG>
    static std::auto_ptr<PUT_FILE_DIALOG>
                Make(
                    const DialogModality&   inModality,
                    const String&           inFileName,
                    B_PUTFILEDIALOG_CB      inFunction = NullCallback<PUT_FILE_DIALOG>());
    //@}
    
    /*! @name Starting Dialogs
        
        In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
    */
    //@{
    //! Instantiates a @a PUT_FILE_DIALOG with the given arguments, then enters it.
    template <class PUT_FILE_DIALOG>
    static void Start(
                    const DialogModality&   inModality,
                    const String&           inFileName,
                    B_PUTFILEDIALOG_CB      inFunction = NullCallback<PUT_FILE_DIALOG>());
    //@}
    
    /*! @name Running Dialogs
        
        These are convenience functions for displaying nav dialogs in an 
        application-modal fashion.  Because they don't return until the nav dialog 
        is dismissed, the chosen action is returned as the function result.
    */
    //@{
    //! Displays the nav dialog in an application-modal fashion.
    bool        Run(
                    AutoNavReply&           outNavReply);
    //! Instantiates a @a PUT_FILE_DIALOG, then enters a modal event loop for it.
    template <class PUT_FILE_DIALOG>
    static bool Run(
                    const String&           inFileName,
                    AutoNavReply&           outNavReply);
    //@}
    
protected:

    //! Protected default constructor to discourage stack-based instantiation.
    PutFileDialog();
    
    //! @name Dialog Control
    //@{
    SaveInfo&       GetSaveInfo()       { return (mSaveInfo); }
    void            HideExtension(bool showIt);
    bool            IsExtensionHidden();
    //! Tells the dialog to return the editable file name.
    String          GetEditFileName();
    //! Tells the dialog to change the editable file name.
    void            SetEditFileName(const String& inFileName);
    //@}
    
    //! @name Overrides from NavDialog
    //@{
    virtual NavDialogRef
                    CreateDialog();
    virtual void    HandleReply(
                        NavUserAction           inUserAction, 
                        NavReplyRecord&         ioNavReply);
    virtual void    PopupMenuSelect(
                        const NavMenuItemSpec&  inMenuItemSpec);
    virtual void    DialogStarting(
                        NavCBRec&               ioParams);
    //@}
    
    void    FillPopupMenuWithDocTypes(
                const Bundle&       inBundle, 
                CFStringRef         inTable, 
                const String&       inDefaultType);
    
private:
    
    typedef DialogCallbackBase3<PutFileDialog, bool, NavReplyRecord&, const SaveInfo&>  CallbackType;
    
    // member variables
    Bundle::DocumentTypeArray   mDocTypes;
    Bundle::Iterator            mCurrentDocType;
    std::auto_ptr<CallbackType> mCallback;
    SaveInfo                    mSaveInfo;
};

// ------------------------------------------------------------------------------------------
/*! Sets the document types array to be equal to the iterator sequence 
    <tt>[inFirst, inLast)</tt>.  The array is then used to populate the file type pop-up 
    menu.  @a inBundle and @a inTable are used to read the localised strings for each 
    document type.  If the array has more than one element, the menu is forced to be 
    displayed;  note that this can be overridden by a subsequent call to 
    @c SetFlags(kNavNoTypePopup).
    
    If @a inDefaultType is non-empty, it's assumed to contain the "default" document type.  
    The document type in question is then moved to the beginning of the doc types array, 
    and a separator is inserted between it and the other document types.
    
    @param  InputIterator   Template parameter.  An input iterator yielding a Bundle::DocumentType object.
*/
template <class InputIterator> void
PutFileDialog::SetDocumentTypes(
    const Bundle&   inBundle,   //!< The bundle in which to look for localised document type names.
    CFStringRef     inTable,    //!< The table in which to look for localised document type names (may be @c NULL).
    InputIterator   inFirst,    //!< The start of the sequence of document type objects.
    InputIterator   inLast,     //!< The end of the sequence of document type objects.
    const String&   inDefaultType /* = String() */) //!< The default document type.
{
    mDocTypes.assign(inFirst, inLast);
    FillPopupMenuWithDocTypes(inBundle, inTable, inDefaultType);
}

// ------------------------------------------------------------------------------------------
/*! @todo                   Complete this!
    @param  PUT_FILE_DIALOG Template parameter.  The class of the instantiated window.  Must 
                            be PutFileDialog or a class derived from PutFileDialog.
*/
template <class PUT_FILE_DIALOG> void
PutFileDialog::SetCallback(
    B_PUTFILEDIALOG_CB  inFunction)
{
    boost::function_requires< boost::ConvertibleConcept<PUT_FILE_DIALOG*, B::PutFileDialog*> >();
    
    mCallback.reset(new DialogCallback3<PUT_FILE_DIALOG, PutFileDialog, bool, NavReplyRecord&, const SaveInfo&>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a dialog of class @a PUT_FILE_DIALOG.  The instance of @a PUT_FILE_DIALOG 
    is then configured with the given arguments.  Finally the dialog is returned, ready to 
    be displayed via Enter().
    
    @param  PUT_FILE_DIALOG Template parameter.  The class of the instantiated window.  Must 
                            be PutFileDialog or a class derived from PutFileDialog.
*/
template <class PUT_FILE_DIALOG>
std::auto_ptr<PUT_FILE_DIALOG>
PutFileDialog::Make(
    const DialogModality&   inModality,     //!< The dialog's modality.
    const String&           inFileName,     //!< The name of the file we're being asked to save.
    B_PUTFILEDIALOG_CB      inFunction /*  = NullCallback<PUT_FILE_DIALOG>() */)    //!< A callback that is invoked when the dialog is dismissed.
{
    boost::function_requires< ProtectedDefaultConstructorConcept<PUT_FILE_DIALOG> >();
    boost::function_requires< boost::ConvertibleConcept<PUT_FILE_DIALOG*, B::PutFileDialog*> >();
    
    std::auto_ptr<PUT_FILE_DIALOG>  dialogPtr(new PUT_FILE_DIALOG);
    
    dialogPtr->SetModality(inModality);
    dialogPtr->SetFileName(inFileName);
    dialogPtr->SetCallback(inFunction);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a PUT_FILE_DIALOG, as per Make().  The 
    dialog is then displayed via Enter().  Whether the dialog has been dismissed by the 
    time the function returns depends upon @a inModality.  See Enter() for more information.
    
    @note   The dialog is automatically disposed upon dismissal.  If you would rather 
            hang on to the dialog, call Make() then Enter().
    
    @param  PUT_FILE_DIALOG Template parameter.  The class of the instantiated window.  Must 
                            be PutFileDialog or a class derived from PutFileDialog.
    
    @sa     Make(), NavDialog::Enter()
*/
template <class PUT_FILE_DIALOG> void
PutFileDialog::Start(
    const DialogModality&   inModality,     //!< The dialog's modality.
    const String&           inFileName,     //!< The name of the file we're being asked to save.
    B_PUTFILEDIALOG_CB      inFunction /*  = NullCallback<PUT_FILE_DIALOG>() */)    //!< A callback that is invoked when the dialog is dismissed.
{
    Enter(Make<PUT_FILE_DIALOG>(inModality, inFileName, inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a PUT_FILE_DIALOG, as per Make().  Then the 
    dialog's modality is set to application-modal, and it is entered.  Control does not 
    return from this function until the dialog is dismissed.
    
    @param  PUT_FILE_DIALOG Template parameter.  The class of the instantiated window.  Must 
                            be PutFileDialog or a class derived from PutFileDialog.
    
    @return @c true if the user confirmed the dialog.
    
    @sa     Make(), Run()
*/
template <class PUT_FILE_DIALOG> bool
PutFileDialog::Run(
    const String&           inFileName,     //!< The name of the file we're being asked to save.
    AutoNavReply&           outNavReply)    //!< The nav reply;  only valid if the function returns @c true.
{
    std::auto_ptr<PUT_FILE_DIALOG>  dialogPtr;
    
    dialogPtr = Make<PUT_FILE_DIALOG>(DialogModality::Modal(), inFileName);
    
    // Enter the dialog, wait for dismissal, & return result.
    return (dialogPtr->Run(outNavReply));
}

#undef B_PUTFILEDIALOG_CB


// ==========================================================================================
//  ChooseFolderDialog

#pragma mark -
#pragma mark * ChooseFolderDialog *

#ifdef B_CHOOSEFOLDERDIALOG_CB
#   error "Somebody defined B_CHOOSEFOLDERDIALOG_CB ..."
#else
    //! Convenience macro to simplify callback declarations
#   define B_CHOOSEFOLDERDIALOG_CB  boost::function3<void, const CHOOSE_FOLDER_DIALOG&, bool, const NavReplyRecord&>
#endif

/*! @brief  Implements the standard "Open" dialog as per the Aqua HIG.
    
    Unlike the simpler nav dialogs derived directly from NavDialog, ChooseFolderDialog doesn't 
    allow for passing arguments (beyond the DialogModality and callback) to the 
    Make/Start/Run functions.  That's because the configuration of this dialog can be 
    quite complex;  so the task has been left to clients.

    GetFileDialog's configuration functions all have to do with the file type pop-up menu.
    This is because Navigation Services' filtering API is inordinately hard to use, since 
    it's based on matching files with creators & file types, and on Mac OS X that's not 
    very useful.

    A number of factors affect the file type pop-up menu:

    - The @c kNavNoTypePopup flag.  This flag controls the visibility of the 
      pop-up menu and takes precedence over other options.  If the flag is set, 
      then each file is matched against all of the entries in the document types 
      array.  The flag can be changed with SetFlag()/ClearFlag().
    - The document types array.  This array is set with SetDocumentTypes().  There are 
      various scenarios, depending on the number of elements in the array:
      -# If the array is empty, then all files will match successfully, regardless 
         of extension or file type.
      -# If the array contains only one element, then the pop-up menu is not displayed 
         (unless other factors cause it to be displayed) and files are matched against 
         the element.
      -# If the array contains more than one element, then the pop-up menu is displayed 
         and contains one item per element.  When one of the items in the pop-up menu 
         is selected, the files are matched against the corresponding document type 
         entry.  In addition, an item called "All Readable Documents" is prepended to 
         the pop-up menu which causes the files to be matched against @e all of the 
         document type entries.
    
    @sa     @ref dialogs_alerts_nav
*/
class ChooseFolderDialog : public NavReplyDialog
{
public:
    
    //! @name Types
    //@{
    //! Convenience callback that does nothing.
    template <class CHOOSE_FOLDER_DIALOG> struct NullCallback
        { void  operator () (const CHOOSE_FOLDER_DIALOG&, bool, const NavReplyRecord&) const {} };
    //@}
    
    //! @name Setup
    //@{
    //! Sets the dialog's callback.
    template <class CHOOSE_FOLDER_DIALOG>
    void    SetCallback(
                B_CHOOSEFOLDERDIALOG_CB inFunction);
    //@}
    
    /*! @name Making Dialogs
        
        In B parlance, "Making" a dialog means to instantiate it, returning a smart 
        pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
        
        Unlike the simpler Nav dialogs (eg AskSaveChangesDialog), it's not possible 
        to configure the dialog completely with Make() except in the simplest of 
        cases.  This is because there are many more configuration options in 
        this dialog.
    */
    //@{
    //! Instantiates and returns a @a CHOOSE_FOLDER_DIALOG with the given arguments.
    template <class CHOOSE_FOLDER_DIALOG>
    static std::auto_ptr<CHOOSE_FOLDER_DIALOG>
                Make(
                    const DialogModality&   inModality,
                    B_CHOOSEFOLDERDIALOG_CB inFunction = NullCallback<CHOOSE_FOLDER_DIALOG>());
    //@}
    
    /*! @name Starting Dialogs
        
        In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
    */
    //@{
    //! Instantiates a @a CHOOSE_FOLDER_DIALOG with the given arguments, then enters it.
    template <class CHOOSE_FOLDER_DIALOG>
    static void Start(
                    const DialogModality&   inModality,
                    B_CHOOSEFOLDERDIALOG_CB inFunction = NullCallback<CHOOSE_FOLDER_DIALOG>());
    //@}
    
    /*! @name Running Dialogs
        
        These are convenience functions for displaying nav dialogs in an 
        application-modal fashion.  Because they don't return until the nav dialog 
        is dismissed, the chosen action is returned as the function result.
    */
    //@{
    //! Displays the nav dialog in an application-modal fashion.
    bool        Run(
                    AutoNavReply&           outNavReply, 
                    bool                    inUnused);
    //! Instantiates a @a CHOOSE_FOLDER_DIALOG, then enters a modal event loop for it.
    template <class CHOOSE_FOLDER_DIALOG>
    static bool Run(
                    AutoNavReply&           outNavReply);
    //@}
    
protected:
    
    //! Protected default constructor to discourage stack-based instantiation.
    ChooseFolderDialog();
    
    //! @name Overrides from NavReplyDialog
    //@{
    virtual NavDialogRef
                    CreateDialog();
    virtual void    HandleReply(
                        NavUserAction           inUserAction, 
                        NavReplyRecord&         ioNavReply);
    //@}
    
private:
    
    typedef DialogCallbackBase2<ChooseFolderDialog, bool, const NavReplyRecord&>    CallbackType;
    
    // member variables
    std::auto_ptr<CallbackType> mCallback;
};

// ------------------------------------------------------------------------------------------
/*! @todo                           Complete this!
    @param  CHOOSE_FOLDER_DIALOG    Template parameter.  The class of the instantiated dialog.  
                                    Must be ChooseFolderDialog or a class derived from ChooseFolderDialog.
*/
template <class CHOOSE_FOLDER_DIALOG> void
ChooseFolderDialog::SetCallback(
    B_CHOOSEFOLDERDIALOG_CB inFunction)
{
    boost::function_requires< boost::ConvertibleConcept<CHOOSE_FOLDER_DIALOG*, B::ChooseFolderDialog*> >();
    
    mCallback.reset(new DialogCallback2<CHOOSE_FOLDER_DIALOG, ChooseFolderDialog, bool, const NavReplyRecord&>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a dialog of class @a CHOOSE_FOLDER_DIALOG.  The instance of @a CHOOSE_FOLDER_DIALOG 
    is then configured with the given arguments.  Finally the dialog is returned, ready to 
    be displayed via Enter().
    
    @param  CHOOSE_FOLDER_DIALOG    Template parameter.  The class of the instantiated window.  
                                    Must be ChooseFolderDialog or a class derived from ChooseFolderDialog.
    
    @return The @a CHOOSE_FOLDER_DIALOG object, wrapped in an @c std::auto_ptr<>.  
            This implies that control over the object's lifetime is given to the caller.
*/
template <class CHOOSE_FOLDER_DIALOG>
std::auto_ptr<CHOOSE_FOLDER_DIALOG>
ChooseFolderDialog::Make(
    const DialogModality&   inModality, //!< The dialog's modality.
    B_CHOOSEFOLDERDIALOG_CB inFunction /*  = NullCallback<CHOOSE_FOLDER_DIALOG>() */)   //!< A callback that is invoked when the dialog is dismissed.
{
    boost::function_requires< ProtectedDefaultConstructorConcept<CHOOSE_FOLDER_DIALOG> >();
    boost::function_requires< boost::ConvertibleConcept<CHOOSE_FOLDER_DIALOG*, B::ChooseFolderDialog*> >();
    
    std::auto_ptr<CHOOSE_FOLDER_DIALOG> dialogPtr(new CHOOSE_FOLDER_DIALOG);
    
    dialogPtr->SetModality(inModality);
    dialogPtr->SetCallback(inFunction);
    
    return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a CHOOSE_FOLDER_DIALOG, as per Make().  The 
    dialog is then displayed via Enter().  Whether the dialog has been dismissed by the 
    time the function returns depends upon @a inModality.  See Enter() for more information.
    
    @note   The dialog is automatically disposed upon dismissal.  If you would rather 
            hang on to the dialog, call Make() then Enter().
    
    @param  CHOOSE_FOLDER_DIALOG    Template parameter.  The class of the instantiated 
                                    window.  Must be ChooseFolderDialog or a class derived 
                                    from ChooseFolderDialog.
    
    @sa     Make(), NavDialog::Enter()
*/
template <class CHOOSE_FOLDER_DIALOG> void
ChooseFolderDialog::Start(
    const DialogModality&   inModality, //!< The dialog's modality.
    B_CHOOSEFOLDERDIALOG_CB inFunction /*  = NullCallback<CHOOSE_FOLDER_DIALOG>() */)   //!< A callback that is invoked when the dialog is dismissed.
{
    Enter(Make<CHOOSE_FOLDER_DIALOG>(inModality, inFunction));
}

// ------------------------------------------------------------------------------------------
/*! Instantiates a nav dialog of class @a CHOOSE_FOLDER_DIALOG, as per Make().  Then the 
    dialog's modality is set to application-modal, and it is entered.  Control does not 
    return from this function until the dialog is dismissed.
    
    @param  CHOOSE_FOLDER_DIALOG    Template parameter.  The class of the instantiated 
                                    window.  Must be ChooseFolderDialog or a class derived 
                                    from ChooseFolderDialog.
    
    @return @c true if the user selected at least one file.
    
    @sa     Make(), Run()
*/
template <class CHOOSE_FOLDER_DIALOG> bool
ChooseFolderDialog::Run(
    AutoNavReply&           outNavReply)    //!< The nav reply;  only valid if the function returns @c true.
{
    std::auto_ptr<CHOOSE_FOLDER_DIALOG> dialogPtr;
    
    dialogPtr = Make<CHOOSE_FOLDER_DIALOG>(DialogModality::Modal());
    
    // Enter the dialog, wait for dismissal, & return result.
    return (dialogPtr->Run(outNavReply, true));
}

#undef B_CHOOSEFOLDERDIALOG_CB


// ==========================================================================================
//  AutoNavReply

#pragma mark -
#pragma mark * AutoNavReply *

/*!
    @brief   Helper class for managing @c NavReplyRecord structures.
    
    @c NavReplyRecord structures are initialised and freed by Navigation Services, but 
    their storage is managed by the application.  This class helps to ensure that an 
    initialised @c NavReplyRecord always gets disposed of properly.
*/
class AutoNavReply : public boost::noncopyable
{
public:
    
    //! Constructor.  Makes an empty object.
    AutoNavReply();
    //! Destructor.  Releases the @c NavReplyRecord if necessary.
    ~AutoNavReply();
    
    //! Retrieving the @c NavReplyRecord.
    const NavReplyRecord&   get() const { return (mNavReply); };
    
    //! Reads the @c NavReplyRecord from @a inNavDialogRef.
    void    reset(NavDialogRef inNavDialogRef);
    //! Releases any currently initialised @c NavReplyRecord.
    void    clear();
    //! Exchanges the contents of two AutoNavReply objects.
    void    swap(AutoNavReply& other);
    
private:
    
    void    init();
    
    NavReplyRecord  mNavReply;
    bool            mHasReply;
};


// ==========================================================================================
//  InteractWithUserDialog

#pragma mark -
#pragma mark * InteractWithUserDialog *

/*!
    @brief  Requests the user's attention.
    
    Use this dialog whenever you need the user to supply some information, but the 
    application may not be in the foreground.  This dialog checks if the app is in the 
    foreground and if so doesn't do anything.  If not, it calls attention to itself in 
    a number of ways (explained below) so that the user brings the app to the foreground.
    If the user doesn't respond within the given timeout period, an 
    InteractionTimeoutException is thrown.
    
    Here's an example.  Assume @c MyDeleteUserData needs the user to confirm the deletion:
    
    @code
        MyDeleteUserData()
        {
            // Notify the user that we need their attention.
            InteractWithUserDialog().Run();
            
            // If we got here, it's because the user brought us to the foreground.
            
            if (MyConfirmDeletionAlert())
            {
                // perform the deletion
            }
        }
    @endcode
            
    @note   This isn't actually a NavServices dialog, but rather a simple interface 
            to the Notification Manager.
    
    @note   Instances of Dialog, Alert, and NavDialog call this class as necessary, so 
            developers will rarely need to call it directly.
*/
class InteractWithUserDialog
{
public:
    
    //! @name Constructor & Destructor
    //@{
    //! Default constructor.  Will use the default settings (see below).
    InteractWithUserDialog();
    //! Constructor.  Will use the given settings.
    InteractWithUserDialog(
        const NMRec&    inNMRec, 
        const String&   inText);
    //@}
    
    //! @name Dialog Display
    //@{
    //! Requests the user's attention.
    void    Run(long timeout = kAEDefaultTimeout);
    //@}
    
    /*! @name Default Values
        
        These functions control how the user will be alerted, in cases where a 
        InteractWithUserDialog is instantiated via its default constructor.  The 
        idea is that the application fills these in once, then calls the default 
        constructor when needed.
    */
    //@{
    //! This will cause an alert to be displayed containing @a inText.
    static void SetText(const String& inText);
    //! This will cause an mark character to appear in the Application menu.
    static void SetMark(bool inShowMark);
    //! This will cause an icon to blink in the Application menu.
    static void SetIcon(Handle inIcon);
    //! This will cause a sound to be played.
    static void SetSound(Handle inSound);
    //@}
    
private:
    
    // member variables
    NMRec   mNMRec;
    Str255  mText;
    
    // static member variables
    static Str255   sText;  //!< By default, the string is empty.
    static bool     sMark;  //!< By default, there is no mark character.
    static Handle   sIcon;
    static Handle   sSound;
};

}   // namespace B


#endif  // BNavDialogs_H_
