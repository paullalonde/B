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
#include "BAlert.h"

// system headers
#include <Carbon/Carbon.h>

// project headers
#include "BAutoUPP.h"
#include "BBundle.h"
#include "BEvent.h"
#include "BEventHandler.h"
#include "BException.h"
#include "BFwd.h"
#include "BHelpUtilities.h"
#include "BNavDialogs.h"


namespace B {

// ==========================================================================================
//  Alert

// ------------------------------------------------------------------------------------------
/*! Sets default values for everything, which gives us a note alert with no error or 
    explanatory text, using the default buttons and button titles.
*/
Alert::Alert()
    : mType(kAlertNoteAlert), mShowHelp(false), mCloseOnDismiss(false), 
      mButtonHit(kNone)
{
    OSStatus    err;
    
    mParams.version = kStdCFStringAlertVersionOne;
    err = GetStandardAlertDefaultParams(&mParams, kStdCFStringAlertVersionOne);
    B_THROW_IF_STATUS(err);
    
    // We always do moveable alerts, and Panther ignores this flag anyway.
    mParams.movable = true;
    
    if (HelpUtilities::HasHelpBook())
        mHelpBook = Bundle::Main().GetHelpBook();
}

// ------------------------------------------------------------------------------------------
Alert::~Alert()
{
}

// ------------------------------------------------------------------------------------------
void
Alert::SetAlertType(
    AlertType   inAlertType)    //!< The alert type.
{
    mType = inAlertType;
}

// ------------------------------------------------------------------------------------------
void
Alert::SetMessageText(
    const String&   inMessageText)  //!< The message text, which is the principal text in the alert.
{
    mMessageText = inMessageText;
}

// ------------------------------------------------------------------------------------------
void
Alert::SetInformativeText(
    const String&   inInformativeText)  //!< The informative text, which is the secondary text in the alert.  May be empty, in which case no informative text is displayed.
{
    mInformativeText = inInformativeText;
}

// ------------------------------------------------------------------------------------------
/*! Only application-modal alerts and alert sheets are supported.  An exception is thrown 
    if any other modality (eg modeless) is passed in.
*/
void
Alert::SetModality(
    const DialogModality&   inModality) //!< The alert's modality.
{
    B_ASSERT((mModalityHandler.get() == NULL) || !mModalityHandler->IsDisplayed());
    
    switch (inModality.GetModality())
    {
    case kWindowModalityAppModal:
        mModalityHandler.reset(new AppModalHandler(*this));
        break;
        
    case kWindowModalityWindowModal:
        mModalityHandler.reset(new SheetHandler(*this, inModality.GetParent()));
        break;
        
    default:
        B_THROW(std::logic_error("B::Alert unsupported modality"));
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
Alert::SetCloseOnDismiss(bool inCloseOnDismiss)
{
    mCloseOnDismiss = inCloseOnDismiss;
}

// ------------------------------------------------------------------------------------------
/*! When first made visible, the button uses its default caption.  By default, only the 
    OK button is visible.
    
    If @a whichButton is kNone, the function does nothing.
*/
void
Alert::ShowButton(Button whichButton, bool showIt)
{
    CFStringRef*    paramsPtr;
    int             text;
    
    switch (whichButton)
    {
    case kOK:
        text        = kAlertDefaultOKText;
        paramsPtr   = &mParams.defaultText;
        goto SetButtonText;
        
    case kCancel:
        text        = kAlertDefaultCancelText;
        paramsPtr   = &mParams.cancelText;
        goto SetButtonText;
        
    case kOther:
        text        = kAlertDefaultOtherText;
        paramsPtr   = &mParams.otherText;
        // fall through
        
    SetButtonText:
        *paramsPtr = showIt ? reinterpret_cast<CFStringRef>(text) : NULL;
        break;
        
    case kHelp:
        mParams.helpButton = showIt;
        break;
        
    default:
        // Ignore other values such as kNone.
        break;
    }
}

// ------------------------------------------------------------------------------------------
/*! This function makes @a whichButton visible if it isn't already.
    
    If @a whichButton is kHelp or kNone, the function does nothing.
*/
void
Alert::SetButtonText(Button whichButton, const String& inText)
{
    B_ASSERT(whichButton != kHelp);
    B_ASSERT(!inText.empty());
    
    String*         stringPtr;
    CFStringRef*    paramsPtr;
    
    switch (whichButton)
    {
    case kOK:
        stringPtr   = &mOKText;
        paramsPtr   = &mParams.defaultText;
        goto SetButtonText;
        
    case kCancel:
        stringPtr   = &mCancelText;
        paramsPtr   = &mParams.cancelText;
        goto SetButtonText;
        
    case kOther:
        stringPtr   = &mOtherText;
        paramsPtr   = &mParams.otherText;
        // fall through
        
    SetButtonText:
        *stringPtr  = inText;
        *paramsPtr  = stringPtr->cf_ref();
        break;
        
    default:
        // Ignore other values such as kHelp and kNone.
        break;
    }
}

// ------------------------------------------------------------------------------------------
/*! If @a whichButton is kNone, the alert won't react to the Return and Enter keys.
*/
void
Alert::SetDefaultButton(Button whichButton)
{
    switch (whichButton)
    {
    case kOK:       mParams.defaultButton = kAlertStdAlertOKButton;     break;
    case kCancel:   mParams.defaultButton = kAlertStdAlertCancelButton; break;
    case kOther:    mParams.defaultButton = kAlertStdAlertOtherButton;  break;
    case kHelp:     mParams.defaultButton = kAlertStdAlertHelpButton;   break;
    case kNone:     mParams.defaultButton = 0;                          break;
    }
}

// ------------------------------------------------------------------------------------------
/*! If @a whichButton is kNone, the alert won't react to the Escape and Command-period keys.
*/
void
Alert::SetCancelButton(Button whichButton)
{
    switch (whichButton)
    {
    case kOK:       mParams.cancelButton = kAlertStdAlertOKButton;      break;
    case kCancel:   mParams.cancelButton = kAlertStdAlertCancelButton;  break;
    case kOther:    mParams.cancelButton = kAlertStdAlertOtherButton;   break;
    case kHelp:     mParams.cancelButton = kAlertStdAlertHelpButton;    break;
    case kNone:     mParams.cancelButton = 0;                           break;
    }
}

// ------------------------------------------------------------------------------------------
/*! By default, the help book is set to the main bundle's help book, if any.
    
    Calling this function changes the alert in the following ways:
    
    - It implicitly makes the Help button visible.
    - If the user clicks the Help button, then the Help Viewer application is 
      automatically opened in order to display the help pointed to by the 
      help book / help url pair.
*/
void
Alert::SetHelpBook(const String& inHelpBook)
{
    ShowButton(kHelp, true);
    
    mHelpBook   = inHelpBook;
    mShowHelp   = true;
}

// ------------------------------------------------------------------------------------------
/*! Calling this function changes the alert in the following ways:
    
    - It implicitly makes the Help button visible.
    - If the user clicks the Help button, then the Help Viewer application is 
      automatically opened in order to display the help pointed to by the 
      help book / help url pair.
*/
void
Alert::SetHelpUrl(const Url& inHelpUrl)
{
    ShowButton(kHelp, true);
    
    mHelpUrl    = inHelpUrl;
    mShowHelp   = true;
}

// ------------------------------------------------------------------------------------------
void
Alert::ShowHelp()
{
    if (mShowHelp)
    {
        HelpUtilities::GotoPage(mHelpBook, mHelpUrl);
    }
}

// ------------------------------------------------------------------------------------------
void
Alert::SetParamFlags(OptionBits flags)
{
    mParams.flags |= flags;
}

// ------------------------------------------------------------------------------------------
void
Alert::ClearParamFlags(OptionBits flags)
{
    mParams.flags &= ~flags;
}

// ------------------------------------------------------------------------------------------
/*!
    @param  inPosition  Enumerated value denoting the alert's location on the screen.  
                        May take on one of the following values:
                            - @c kWindowDefaultPosition
                            - @c kWindowCenterMainScreen
                            - @c kWindowAlertPositionMainScreen
                            - @c kWindowStaggerMainScreen
                            - @c kWindowCenterParentWindow
                            - @c kWindowAlertPositionParentWindow
                            - @c kWindowStaggerParentWindow
                            - @c kWindowCenterParentWindowScreen
                            - @c kWindowAlertPositionParentWindowScreen
                            - @c kWindowStaggerParentWindowScreen
*/
void
Alert::SetPosition(UInt16 inPosition)
{
    mParams.position = inPosition;
}

// ------------------------------------------------------------------------------------------
void
Alert::Enter()
{
    // If no modality has previously been set, we get a modal alert.
    
    if (mModalityHandler.get() == NULL)
        SetModality(DialogModality::Modal());
    
    try
    {
        // We never use a one-shot alert sheet, 'cause lifecycle issues are just 
        // too much of a pain.
        
        mParams.flags |= kStdAlertDoNotDisposeSheet;
        
        // Display the dialog.
        
        mModalityHandler->Display();
    }
    catch (InteractionTimeoutException&)
    {
        // If the user-interaction dialog times out (meaning that the dialog wasn't 
        // even displayed), treat it as a 'Cancel'.
        
        mButtonHit = kCancel;
        
        if (mCallback.get() != NULL)
            (*mCallback)(*this, mButtonHit);
    }
}

// ------------------------------------------------------------------------------------------
/*! Sets the alert to be application-modal, enters is, then returns the dismissing button 
    once the alert has been dismissed.
    
    @return An enumerated value of type Button, indicating which button dismissed the 
            alert dialog.
*/
Alert::Button
Alert::Run()
{
    // Force the modality to application-modal.
    SetModality(DialogModality::Modal());
    
    // Ensure the dialog doesn't get released before Run() returns.
    SetCloseOnDismiss(false);
    
    // Enter the dialog & wait for dismissal.
    Enter();
    
    // Return the dismissing button.
    return (GetButtonHit());
}

// ------------------------------------------------------------------------------------------
/*! @todo   Complete this!
*/
void
Alert::DismissAlert(Button inButton)
{
    B_ASSERT(inButton != kNone);
    
    mButtonHit = inButton;

    if (mCallback.get() != NULL)
        (*mCallback)(*this, mButtonHit);
    
    mModalityHandler->Dismiss();
    
    if (mCloseOnDismiss)
        delete this;
}

// ------------------------------------------------------------------------------------------
/*! @todo   Complete this!
*/
void
Alert::ButtonClicked(Button inButton, bool inWillDismiss)
{
    B_ASSERT(inButton != kNone);
    
    if (inButton == kHelp)
        ShowHelp();
    
    if (inWillDismiss)
        DismissAlert(inButton);
}

// ------------------------------------------------------------------------------------------
void
Alert::Configure(
    const String&           inMessageText,      //!< The alert's principal text.
    const String&           inInformativeText,  //!< The alert's secondary text.  May be empty.
    const DialogModality&   inModality)         //!< The alert's modality.
{
    SetMessageText(inMessageText);
    SetInformativeText(inInformativeText);
    SetModality(inModality);
}

// ------------------------------------------------------------------------------------------
void
Alert::Configure(
    bool            inIsCautionAlert,   //!< Is this a "caution" alert?
    bool            inShowCancelButton) //!< Should the Cancel button be displayed?
{
    SetAlertType(inIsCautionAlert ? kAlertCautionAlert : kAlertNoteAlert);
    
    // Display the Cancel button if requested.
    
    if (inShowCancelButton)
    {
        ShowButton(kCancel, true);
        SetCancelButton(kCancel);
    }
}

// ------------------------------------------------------------------------------------------
void
Alert::Configure(
    bool            inIsCautionAlert,   //!< Is this a "caution" alert?
    bool            inShowCancelButton, //!< Should the Cancel button be displayed?
    bool            inShowOtherButton,  //!< Should the Other button be displayed?
    Button          inDefaultButton,    //!< Which is the default button (i.e. the one that reacts to the Return and Enter keys)?
    Button          inCancelButton,     //!< Which is the cancel button (i.e. the one that reacts to the Escape and Command-period keys)?
    const String&   inOKButtonText,     //!< The text of the OK button.  If empty, the default text is used instead.
    const String&   inCancelButtonText, //!< The text of the Cancel button.  Ignored unless @a inShowCancelButton is @c true.
    const String&   inOtherButtonText)  //!< The text of the Other button.  Ignored unless @a inShowOtherButton is @c true.
{
    SetAlertType(inIsCautionAlert ? kAlertCautionAlert : kAlertNoteAlert);
    ShowButton(kCancel, inShowCancelButton);
    ShowButton(kOther, inShowOtherButton);
    SetDefaultButton(inDefaultButton);
    SetCancelButton(inCancelButton);
    
    if (!inOKButtonText.empty())
        SetButtonText(kOK, inOKButtonText);
    
    if (inShowCancelButton && !inCancelButtonText.empty())
        SetButtonText(kCancel, inCancelButtonText);
    
    if (inShowOtherButton && !inOtherButtonText.empty())
        SetButtonText(kOther, inOtherButtonText);
}


// ==========================================================================================
//  Alert::ModalityHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
Alert::ModalityHandler::ModalityHandler(Alert& inAlert)
    : mAlert(inAlert)
{
}

// ------------------------------------------------------------------------------------------
Alert::ModalityHandler::~ModalityHandler()
{
}

// ------------------------------------------------------------------------------------------
void
Alert::ModalityHandler::ButtonClicked(Alert::Button inButton, bool inWillDismiss /* = true */)
{
    mAlert.ButtonClicked(inButton, inWillDismiss);
}

// ------------------------------------------------------------------------------------------
CFStringRef
Alert::ModalityHandler::GetMessageText() const
{
    const String&   text(mAlert.GetMessageText());
    
    return (!text.empty() ? text.cf_ref() : NULL);
}

// ------------------------------------------------------------------------------------------
CFStringRef
Alert::ModalityHandler::GetInformativeText() const
{
    const String&   text(mAlert.GetInformativeText());
    
    return (!text.empty() ? text.cf_ref() : NULL);
}


// ==========================================================================================
//  Alert::AppModalHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
Alert::AppModalHandler::AppModalHandler(Alert& inAlert)
    : ModalityHandler(inAlert), 
      mDisplayed(false)
{
}

// ------------------------------------------------------------------------------------------
bool
Alert::AppModalHandler::IsDisplayed() const
{
    return (mDisplayed);
}

// ------------------------------------------------------------------------------------------
void
Alert::AppModalHandler::Display()
{
    // We mustn't be already displayed.
    B_ASSERT(!IsDisplayed());
    
    DialogRef       dialog  = NULL;
    DialogItemIndex itemHit = kAlertStdAlertCancelButton;
    
    try
    {
        OSStatus    err;
        
        mDisplayed = true;
        
        // Create the alert.

        err = CreateStandardAlert(mAlert.GetType(), GetMessageText(), GetInformativeText(), 
                                  &mAlert.GetParams(), &dialog);
        B_THROW_IF_STATUS(err);
        
        // Ensure the user knows we need them before continuing.
        
        InteractWithUserDialog().Run();
        
        // Enter the modal event loop.
        
        err = RunStandardAlert(dialog, NULL, &itemHit);
        B_THROW_IF_STATUS(err);
        
        dialog = NULL;
    }
    catch (...)
    {
        if (dialog != NULL)
            DisposeDialog(dialog);
        
        mDisplayed = false;
        
        throw;
    }
    
    switch (itemHit)
    {
    default:
    case kAlertStdAlertCancelButton:    ButtonClicked(Alert::kCancel);  break;
    case kAlertStdAlertOKButton:        ButtonClicked(Alert::kOK);      break;
    case kAlertStdAlertOtherButton:     ButtonClicked(Alert::kOther);   break;
    case kAlertStdAlertHelpButton:      ButtonClicked(Alert::kHelp);    break;
    }
    
    // Beyond this point, we mustn't touch any member variables or functions, 'cause we 
    // might have been deleted.
}

// ------------------------------------------------------------------------------------------
void
Alert::AppModalHandler::Dismiss()
{
    // We must be already displayed.
    B_ASSERT(IsDisplayed());
    
    mDisplayed = false;
}


// ==========================================================================================
//  Alert::SheetHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
Alert::SheetHandler::SheetHandler(
    Alert&      inAlert,
    WindowRef   inParent)
        : ModalityHandler(inAlert), 
          mParent(inParent), mDisplayed(false)
{
}

// ------------------------------------------------------------------------------------------
bool
Alert::SheetHandler::IsDisplayed() const
{
    return (mDisplayed);
}

// ------------------------------------------------------------------------------------------
void
Alert::SheetHandler::Display()
{
    // We mustn't be already displayed.
    B_ASSERT(!IsDisplayed());
    
    // Create the alert if it doesn't already exist.
    
    if (mWindow.get() == NULL)
    {
        Create();
    }
    
    try
    {
        OSStatus    err;
        
        mDisplayed = true;
        
        // Ensure the user knows we need them before continuing.
        
        InteractWithUserDialog().Run();
        
        // Install our event handler on the parent window.  This will intercept the 
        // kEventProcessCommand events sent by the alert.
        
        mEventHandler.reset(new EventHandler(mParent));
        mEventHandler->Add(this, &Alert::SheetHandler::CommandProcess);
        mEventHandler->Init();
        
        // Bring the parent to the front, making it visible if necessary.
        
        SelectWindow(mParent);
        if (!IsWindowVisible(mParent))
            ShowWindow(mParent);
        
        // Display the sheet.
        
        err = ShowSheetWindow(mWindow, mParent);
        B_THROW_IF_STATUS(err);
    }
    catch (...)
    {
        mEventHandler.reset();
        
        mDisplayed = false;
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
void
Alert::SheetHandler::Dismiss()
{
    // We must be already displayed.
    B_ASSERT(IsDisplayed());
    
    mEventHandler.reset();
    
    mDisplayed = false;
}

// ------------------------------------------------------------------------------------------
void
Alert::SheetHandler::Create()
{
    DialogRef   dialog;
    OSStatus    err;
    
    // Create the alert.
    
    err = CreateStandardSheet(mAlert.GetType(), GetMessageText(), GetInformativeText(), 
                              &mAlert.GetParams(), GetWindowEventTarget(mParent), &dialog);
    B_THROW_IF_STATUS(err);
    
    mWindow.reset(GetDialogWindow(dialog), from_copy);
}

// ------------------------------------------------------------------------------------------
bool
Alert::SheetHandler::CommandProcess(
    Event<kEventClassCommand, kEventCommandProcess>&    event)
{
    switch (event.mHICommand.commandID)
    {
    case Alert::kCancel:    ButtonClicked(Alert::kCancel);      break;
    case Alert::kOK:        ButtonClicked(Alert::kOK);          break;
    case Alert::kOther:     ButtonClicked(Alert::kOther);       break;
    case Alert::kHelp:      ButtonClicked(Alert::kHelp, false); break;
    }
    
    return (true);
}

}   // namespace B
