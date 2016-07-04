// ==========================================================================================
//  
//  Copyright (C) 2004-2006 Paul Lalonde enrg.
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
#include "BDialog.h"

// B headers
#include "BEvent.h"
#include "BException.h"
#include "BFwd.h"
#include "BNavDialogs.h"
#include "BUtility.h"
#include "BViewUtils.h"
#include "BWindowUtils.h"


namespace B {

// ==========================================================================================
//  Dialog

// ------------------------------------------------------------------------------------------
Dialog::Dialog(
    WindowRef   inWindowRef,
    AEObjectPtr inContainer,
    DescType    inClassID /* = cWindow */)
        : inherited(inWindowRef, inContainer, inClassID), 
          mDismissingCommand(0), mCloseOnDismiss(false)
{
}

// ------------------------------------------------------------------------------------------
void
Dialog::SetModality(
    const DialogModality&   inModality) //!< The dialog's modality.
{
    B_ASSERT((mModalityHandler.get() == NULL) || !mModalityHandler->IsDisplayed());
    
    switch (inModality.GetModality())
    {
    case kWindowModalityNone:
        mModalityHandler.reset(new ModelessHandler());
        break;
        
    case kWindowModalityAppModal:
        mModalityHandler.reset(new AppModalHandler());
        break;
        
    case kWindowModalityWindowModal:
        mModalityHandler.reset(new SheetHandler(inModality.GetParent()));
        break;
        
    default:
        B_THROW(std::logic_error("B::Dialog unsupported modality"));
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
Dialog::SetCloseOnDismiss(bool inCloseOnDismiss)
{
    mCloseOnDismiss = inCloseOnDismiss;
}

// ------------------------------------------------------------------------------------------
/*! If the dialog has window-modal modality (i.e., it's a sheet), then a value of @c true 
    for @a inAnimate will cause the sheet to hide in the normal way.  If @a inAnimate is 
    @c false however, the sheet will hide itself without the normal animation, and the 
    parent window will @b not be notified that the sheet has been hidden.  This is meant 
    for cases where dismissing the dialog will have the effect of closing the parent window;  it such cases it's undesirable to have the user wait 
    for the animation to complete when the parent window is about to go away anyway.  
    This function is specifically meant to be called from within the dialog callback.
    
    If the dialog is not a sheet, this function does nothing.
*/
void
Dialog::SetAnimateOnDismiss(bool inAnimate)
{
    if (SheetHandler* handler = dynamic_cast<SheetHandler*>(mModalityHandler.get()))
        handler->SetAnimateOnHide(inAnimate);
}

// ------------------------------------------------------------------------------------------
/*! The exact action performed depends on the dialog's modality:
    - For a modeless dialog, the dialog is shown and made active.  The function returns 
      immediately.
    - For a modal dialog, the dialog is shown and a modal event loop is entered.  The 
      loop can only be exited by calling DismissDialog(), which will happend automatically 
      if the user clicks an OK or Cancel button.
    - For a sheet, the dialog is shown attached to its parent.  The function returns 
      immediately.
    
    For modal dialogs and sheets, if an interaction timeout exception occurs, the dialog 
    callback (if any) is called immediately with @c kHICommandCancel, then the function 
    returns.
    
    Derived classes wishing to perform some processing prior to the dialog becoming visible 
    should override Window::HandleShowing().
*/
void
Dialog::Enter()
{
    // If no modality has previously been set, we get a modal dialog.
    
    if (mModalityHandler.get() == NULL)
        SetModality(DialogModality::Modal());
    
    // Display the dialog.
    
    try
    {
        mModalityHandler->Display(this);
    }
    catch (InteractionTimeoutException&)
    {
        // If the user-interaction dialog times out (meaning that the dialog wasn't 
        // even displayed), treat it as a 'Cancel'.
        
        mDismissingCommand = kHICommandCancel;
        
        if (mCallback.get() != NULL)
            (*mCallback)(*this, mDismissingCommand);
    }
}

// ------------------------------------------------------------------------------------------
/*! This function is shorthand for SetModality() / Enter() / GetDismissingCommand().
    Refer to those functions' documentation for more information.
                
    @return     The @c HICommand that dismissed the dialog.
    
    @note       Unlike Run() and RunWithState(), this function does not dynamically change 
                the dialog's window class to match its modality.
*/
UInt32
Dialog::Run()
{
    // Force the modality to application-modal.
    SetModality(DialogModality::Modal());
    
    // Ensure the dialog doesn't get released before Run() returns.
    SetCloseOnDismiss(false);
    
    // Enter the dialog & wait for dismissal.
    Enter();
    
    // Return the dismissing command.
    return (GetDismissingCommand());
}

// ------------------------------------------------------------------------------------------
/*! @deprecated This function is here for backward compatibility only, 
                and will be removed in a future release.
*/
UInt32
Dialog::EnterAppModalLoop()
{
    return (Run());
}

// ------------------------------------------------------------------------------------------
/*! This function calls the dialog callback, if any.  It then performs actions depending 
    on the dialog's modality:
    - For a modeless dialog, the dialog is hidden.
    - For a modal dialog, the dialog's modal event loop is exited.  The 
      loop can only be exited by calling DismissDialog().
    - For a sheet, the dialog is shown attached to its parent.  The function returns 
      immediately.
    
    In all cases, if the dialog is set to be closed upon hiding, it will finally be closed.
    
    The exact action performed depends on whether the dialog is in a modal event loop.  
    If so, the loop is exited.
    
    Derived classes may override this function to perform additional processing whenever 
    the dialog is dismissed.  They should call the base class after doing their work.
*/
void
Dialog::DismissDialog(UInt32 inCommand)
{
    mDismissingCommand = inCommand;
    
    if (mCallback.get() != NULL)
        (*mCallback)(*this, mDismissingCommand);
    
    mModalityHandler->Dismiss(mCloseOnDismiss);
}

// ------------------------------------------------------------------------------------------
/*! React to a dialog-confirming action on the part of the user.  @a inCommand is the 
    user-generated command (usually @c kHICommandOK).
    
    The default implementation dismisses the dialog;  @a inCommand becomes the dismissing 
    command, which is returned from EnterModalLoop().
    
    Derived classes may override this function to perform additional processing when a 
    dialog is confirmed.  They may assume that Validate() will already have been called.
*/
void
Dialog::ConfirmDialog(UInt32 inCommand)
{
    DismissDialog(inCommand);
}

// ------------------------------------------------------------------------------------------
/*! React to a dialog-infirming action on the part of the user.  @a inCommand is the 
    user-generated command (usually @c kHICommandCancel).
    
    The default implementation dismisses the dialog;  @a inCommand becomes the dismissing 
    command, which is returned from EnterModalLoop().
    
    Derived classes may override this function to perform additional processing when a 
    dialog is infirmed.
*/
void
Dialog::CancelDialog(UInt32 inCommand)
{
    DismissDialog(inCommand);
}

// ------------------------------------------------------------------------------------------
/*! Perform dialog-wide validation in reaction to the user-generated command @a inCommand.
    
    Derived classes may override this function to perform their own input validation.  
    They are responsible for signalling any invalid input to the user.
    
    The default implementation always returns @c true.
    
    @return     @c true if the dialog's input is valid;  else @c false.
*/
bool
Dialog::Validate(UInt32 /* inCommand */)
{
    return (true);
}

// ------------------------------------------------------------------------------------------
/*! Dialog overrides this function to intercept the @c kHICommandOK and @c kHICommandCancel 
    commands.
    
    Derived classes may override this function to intercept additional dialog-dismissing 
    commands.
*/
bool
Dialog::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case kHICommandOK:
        if (Validate(inHICommand.commandID))
            ConfirmDialog(inHICommand.commandID);
        break;
        
    case kHICommandCancel:
        CancelDialog(inHICommand.commandID);
        break;
        
    default:
        handled = inherited::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! Checks if @a inWindow's window class matches @a inModality.  If it matches, the 
    function returns @a inWindow.  Else, creates a new window with a default window 
    class for @a inModality, copies IB-settable attributes from the original window 
    to the new one, and moves all HIViews.
*/
OSPtr<WindowRef>
Dialog::MutateWindowForModality(
    WindowRef               inWindow, 
    const DialogModality&   inModality)
{
    bool        goodClass   = false;
    WindowClass windowClass = WindowUtils::GetClass(inWindow);
    OSStatus    err;
    
    // Determine if inWindow already has the right class for the modality we want.
    
    switch (inModality.GetModality())
    {
    case kWindowModalityNone:
        goodClass = ((windowClass == kFloatingWindowClass) || 
                     (windowClass == kDocumentWindowClass) || 
                     (windowClass == kUtilityWindowClass)  || 
                     (windowClass == kPlainWindowClass)    || 
                     (windowClass == kAltPlainWindowClass) || 
#if B_BUILDING_CAN_USE_10_3_APIS
                     (windowClass == kSimpleWindowClass)   || 
#endif
                     (windowClass == kDrawerWindowClass));
        break;
        
    case kWindowModalityAppModal:
        goodClass = ((windowClass == kAlertWindowClass)        || 
                     (windowClass == kMovableAlertWindowClass) || 
                     (windowClass == kModalWindowClass)        || 
                     (windowClass == kMovableModalWindowClass));
        break;
        
    case kWindowModalityWindowModal:
        goodClass = ((windowClass == kSheetWindowClass) || 
                     (windowClass == kSheetAlertWindowClass));
        break;
        
    default:
        B_ASSERT(0);
        break;
    }
    
    if (goodClass)
    {
        // Good, no more work required.
        
        return (OSPtr<WindowRef>(inWindow));
    }
    
    // inWindow's class doesn't match the requested modality, so we need to create 
    // a new one.  Determine the characteristics of the new window.
    
    WindowAttributes        windowAttrs = WindowUtils::GetAttributes(inWindow);
    bool                    composited  = (windowAttrs & kWindowCompositingAttribute);
    bool                    metal       = (windowAttrs & kWindowMetalAttribute);
    WindowPositionMethod    targetPos   = 0;
    ThemeBrush              targetBrush = 0;
    WindowClass             targetClass;
    
    switch (inModality.GetModality())
    {
    case kWindowModalityNone:
        targetClass = kDocumentWindowClass;
        targetBrush = kThemeBrushModelessDialogBackgroundActive;
        // Metal appearance needs to override the theme brush, or else we get 
        // a Metal window frame with an Aqua interior.
        if (metal)
            targetBrush = 0;
        break;
        
    case kWindowModalityAppModal:
    default:
        if (windowClass == kSheetAlertWindowClass)
        {
            // Take the fact that we have an alert class as a hint.
            targetClass = kMovableAlertWindowClass;
            targetBrush = kThemeBrushAlertBackgroundActive;
        }
        else
        {
            targetClass = kMovableModalWindowClass;
            targetBrush = kThemeBrushMovableModalBackground;
        }
        // Metal appearance needs to override the theme brush, or else we get 
        // a Metal window frame with an Aqua interior.
        if (metal)
            targetBrush = 0;
        targetPos = kWindowAlertPositionOnParentWindowScreen;
        break;
        
    case kWindowModalityWindowModal:
        if ((windowClass == kAlertWindowClass) || 
            (windowClass == kMovableAlertWindowClass))
        {
            // Take the fact that we have an alert class as a hint.
            targetClass = kSheetAlertWindowClass;
        }
        else
        {
            targetClass = kSheetWindowClass;
        }
        // It would seem that sheets never get the Metal appearance.
        if (composited)
        {
            targetBrush = kThemeBrushSheetBackgroundTransparent;
        }
        else
        {
            targetBrush = kThemeBrushSheetBackgroundOpaque;
        }
        break;
    }
    
    // Create a new window with a proper class for the requested modality.  Add a 
    // root control if necessary.
    
    WindowAttributes    targetAttrs     = GetAvailableWindowAttributes(targetClass);
    ::Rect              qdBounds        = WindowUtils::GetContentBounds(inWindow);
    WindowRef           newWindowRef;
    
    targetAttrs &= windowAttrs;
    
    err = CreateNewWindow(targetClass, targetAttrs, &qdBounds, &newWindowRef);
    B_THROW_IF_STATUS(err);
    
    OSPtr<WindowRef>    newWindow(newWindowRef, from_copy);
    
    if (!composited)
    {
        // We're not composited, so need to prepare the window to host an HIView hierarchy.
        
        HIViewRef   contentView;
        
        err = GetRootControl(newWindowRef, &contentView);
        
        if ((err != noErr) || (contentView == NULL))
        {
            err = CreateRootControl(newWindowRef, &contentView);
            B_THROW_IF_STATUS(err);
        }
    }
    
    // Position the window.
    
    if (targetPos != 0)
    {
        err = RepositionWindow(newWindowRef, FrontNonFloatingWindow(), targetPos);
        B_THROW_IF_STATUS(err);
    }
    
    // Copy the title, if any.
    
    CFStringRef titleRef;
    
    err = CopyWindowTitleAsCFString(inWindow, &titleRef);
    
    if (err == noErr)
    {
        OSPtr<CFStringRef>  titlePtr(titleRef, from_copy);
        
        err = SetWindowTitleWithCFString(newWindowRef, titleRef);
        B_THROW_IF_STATUS(err);
    }
    
    // Set the window's background brush, if any.
    
    if (targetBrush != 0)
    {
        err = SetThemeWindowBackground(newWindowRef, targetBrush, false);
        B_THROW_IF_STATUS(err);
    }
    
    // Copy any help tags.
    
    HMHelpContentRec    helpContent;
    
    err = HMGetWindowHelpContent(inWindow, &helpContent);
    
    if (err == noErr)
    {
        err = HMSetWindowHelpContent(newWindowRef, &helpContent);
        B_THROW_IF_STATUS(err);
    }
    
    // Save the default and cancel buttons, if any.
    
    HIViewRef   defaultBtn, cancelBtn;
    
    if ((err = GetWindowDefaultButton(inWindow, &defaultBtn)) != noErr)
        defaultBtn = NULL;
    
    if ((err = GetWindowCancelButton(inWindow, &cancelBtn)) != noErr)
        cancelBtn = NULL;
    
    // Move all of inWindow's HIViews to the new window.  Save & restore the 
    // default and cancel buttons, if any.
    
    HIViewRef   oldContent  = WindowUtils::GetContentView(inWindow);
    HIViewRef   newContent  = WindowUtils::GetContentView(newWindowRef);
    ViewUtils::ReverseSubviewIterator   it;
    
    while ((it = ViewUtils::rbegin(oldContent)) != ViewUtils::rend(oldContent))
    {
        HIViewRef   subview = *it;
        
        err = HIViewAddSubview(newContent, subview);
        B_THROW_IF_STATUS(err);
    }
    
    // Restore the default & cancel buttons.
    
    if (defaultBtn != NULL)
    {
        err = SetWindowDefaultButton(newWindowRef, defaultBtn);
        B_THROW_IF_STATUS(err);
    }
    
    if (cancelBtn != NULL)
    {
        err = SetWindowCancelButton(newWindowRef, cancelBtn);
        B_THROW_IF_STATUS(err);
    }
    
    return (newWindow);
}


// ==========================================================================================
//  Dialog::ModalityHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
Dialog::ModalityHandler::~ModalityHandler()
{
}


// ==========================================================================================
//  Dialog::ModelessHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
void
Dialog::ModelessHandler::Display(Dialog* inDialog)
{
    WindowRef   windowRef   = inDialog->GetWindowRef();
    
    // The dialog needs to be initially hidden.
    B_ASSERT(!IsWindowVisible(windowRef));
    // We mustn't be already displayed.
    B_ASSERT(!IsDisplayed());
    
    // Set ourselves up as being displayed.
    
    mDialog = inDialog;
    
    // Display the modeless dialog.
    
    SelectWindow(windowRef);
    ShowWindow(windowRef);
    
    // Add the dialog to the global window list, so it doesn't disappear when 
    // the shared pointer goes out of scope.
    
    mDialog->AddToWindowList();
}

// ------------------------------------------------------------------------------------------
void
Dialog::ModelessHandler::Dismiss(bool inClose)
{
    // We must be already displayed.
    B_ASSERT(IsDisplayed());
    
    // This is a modeless dialog, so just hide the window.
    
    Dialog* dialog  = NULL;
    
    std::swap(mDialog, dialog);
    
    HideWindow(dialog->GetWindowRef());
    
    if (inClose)
        dialog->Close();
}


// ==========================================================================================
//  AppModalHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
void
Dialog::AppModalHandler::Display(Dialog* inDialog)
{
    WindowRef   windowRef   = inDialog->GetWindowRef();
    
    // The dialog needs to be initially hidden.
    B_ASSERT(!IsWindowVisible(windowRef));
    // We mustn't be already displayed.
    B_ASSERT(!IsDisplayed());
    
    mClose  = false;
    mDialog = inDialog;
    
    try
    {
        OSStatus    err;
        
        // Ensure the user knows we need them before continuing.
        
        InteractWithUserDialog().Run();
        
        // Make the dialog visible & frontmost.
        
        SelectWindow(windowRef);
        ShowWindow(windowRef);
        
        // Enter the modal event loop.
        
        err = RunAppModalLoopForWindow(windowRef);
        B_THROW_IF_STATUS(err);
        
        // Hide the dialog.
        
        mDialog = NULL;
        HideWindow(windowRef);
        
        if (mClose)
            inDialog->Close();
    }
    catch (...)
    {
        mDialog = NULL;
        HideWindow(windowRef);
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
void
Dialog::AppModalHandler::Dismiss(bool inClose)
{
    // We must be already displayed.
    B_ASSERT(IsDisplayed());
    
    mClose = inClose;
    
    Dialog* dialog  = NULL;
    
    std::swap(mDialog, dialog);
    
    WindowRef   windowRef   = dialog->GetWindowRef();
    OSStatus    err;
    
    err = QuitAppModalLoopForWindow(windowRef);
    B_THROW_IF_STATUS(err);
    
    // Hide the dialog.
    
    HideWindow(windowRef);
}


// ==========================================================================================
//  Dialog::SheetHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
Dialog::SheetHandler::SheetHandler(
    WindowRef   inParent)
        : mParent(inParent), mAnimate(true)
{
}

// ------------------------------------------------------------------------------------------
void
Dialog::SheetHandler::Display(Dialog* inDialog)
{
    WindowRef   windowRef   = inDialog->GetWindowRef();
    
    // The dialog needs to be initially hidden.
    B_ASSERT(!IsWindowVisible(windowRef));
    // We mustn't be already displayed.
    B_ASSERT(!IsDisplayed());
    
    mAnimate    = true;
    mDialog     = inDialog;
    
    try
    {
        OSStatus    err;
        
        // Ensure the user knows we need them before continuing.
        
        InteractWithUserDialog().Run();
        
        // Bring the parent to the front, making it visible if necessary.
        
        SelectWindow(mParent);
        if (!IsWindowVisible(mParent))
            ShowWindow(mParent);
        
        // Display the sheet.
        
        err = ShowSheetWindow(windowRef, mParent);
        B_THROW_IF_STATUS(err);
        
        // Add the dialog to the global window list, so it doesn't disappear when 
        // the shared pointer goes out of scope.
        
        mDialog->AddToWindowList();
    }
    catch (...)
    {
        mDialog = NULL;
        HideWindow(windowRef);
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
void
Dialog::SheetHandler::Dismiss(bool inClose)
{
    // We must be already displayed.
    B_ASSERT(IsDisplayed());
    
    Dialog* dialog  = NULL;
    
    std::swap(mDialog, dialog);
    
    WindowRef   windowRef   = dialog->GetWindowRef();
    OSStatus    err;
    
    // If we're animating, hide the sheet normally;  else, detach it instead.
    
    if (mAnimate)
    {
        err = HideSheetWindow(windowRef);
        B_THROW_IF_STATUS(err);
    }
    else
    {
        err = DetachSheetWindow(windowRef);
        B_THROW_IF_STATUS(err);
        
        HideWindow(windowRef);
    }
    
    if (inClose)
    {
        dialog->Close();
    }
}

}   // namespace B
