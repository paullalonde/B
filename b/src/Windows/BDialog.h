// ==========================================================================================
//	
//	Copyright (C) 2004-2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

#ifndef BDialog_H_
#define BDialog_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BDialogCallback.h"
#include "BDialogModality.h"
#include "BWindow.h"


namespace B {


#ifdef B_DIALOG_CB
#	error "Somebody defined B_DIALOG_CB ..."
#else
	//! Convenience macro to simplify callback declarations
#	define B_DIALOG_CB	boost::function2<void, const DIALOG&, UInt32>
#endif


/*!
	@brief  Modal Dialogs, Modeless Dialogs, and Sheets
	
	Dialog is a derivative of window that closes itself when it receives 
	@c kHICommandOK or @c kHICommandCancel.  Crucially, it can also display itself 
	as a modeless dialog, an application-modal dialog, or a sheet.
	
	Dialog (and any derived classes) should be instantiated via Make() or 
	MakeWithState();  this will ensure that the dialog's window class matches 
	the requested modality.
	
	Once instantiated, the dialog may be entered via Enter().
	
	The Run() and RunWithState() functions are available as a convenience for cases 
	where the caller wants to display the dialog in an application-modal fashion.
	
	@sa		@ref dialogs_alerts_nav
	@nosubgrouping
*/
class Dialog : public Window
{
public:
	
	//! @name Types
	//@{
	//! Convenience callback that does nothing.
	template <class DIALOG> struct NullCallback
		{ void	operator () (const DIALOG&, UInt32) const {} };
	//@}
	
	//! Constructor.
			Dialog(
				WindowRef		inWindowRef,
				AEObjectPtr		inContainer,
				DescType		inClassID = cWindow);
	
	//! @name Inquiries
	//@{
	//! Returns the dialog's dismissing command.
	UInt32  GetDismissingCommand() const	{ return (mDismissingCommand); }
	//@}
	
	//! @name Modifiers
	//@{
	//! Sets the dialog's modality, which determines its behaviour when it is displayed and dismissed.
	void	SetModality(const DialogModality& inModality);
	//! Determines whether the dialog gets closed (i.e. destroyed) after it's dismissed.
	void	SetCloseOnDismiss(bool inCloseOnDismiss);
	//! Determines whether the dialog will animate when it's dismissed.
	void	SetAnimateOnDismiss(bool inAnimate);
	//! Installs a callback that will be called once the dialog is dismissed.
	template <class DIALOG>
	void	SetCallback(B_DIALOG_CB inFunction);
	//@}
	
	/*! @name Making Dialogs
		
		In B parlance, "Making" a dialog means to instantiate it, returning a smart 
		pointer (in this case, an OSPtr<WindowRef>) that controls its lifetime.
	*/
	//@{
	
	//! Instantiates and returns a @a DIALOG with the given modality and callback.
	template <class DIALOG>
	static boost::shared_ptr<DIALOG>
				Make(
					Nib&					inNib,
					const char* 			inDialogName,
					AEObjectPtr				inContainer,
					const DialogModality&	inModality,
					B_DIALOG_CB				inFunction = NullCallback<DIALOG>());
	
	//! Instantiates and returns a @a DIALOG with the given modality and callback, passing @a inState to its constructor.
	template <class DIALOG, typename STATE>
	static boost::shared_ptr<DIALOG>
				MakeWithState(
					Nib&					inNib,
					const char* 			inDialogName,
					AEObjectPtr				inContainer,
					STATE					inState, 
					const DialogModality&	inModality,
					B_DIALOG_CB				inFunction = NullCallback<DIALOG>());
	//@}
	
	/*! @name Entering Dialogs
		
		In B parlance, "Entering" a dialog means to display it according to its 
		modality, i.e. if it's application-modal then the function doesn't return 
		until the dialog is dismissed.
	*/
	//@{
	
	//! Displays the dialog according to its modality.
	void		Enter();
	
	//! Displays the given dialog according to its modality, taking control of the dialog's lifetime.
	template <class DIALOG>
	static void	Enter(boost::shared_ptr<DIALOG> inDialogPtr);
	//@}
	
	/*! @name Starting Dialogs
		
		In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
	*/
	//@{
	
	//! Instantiates a @a DIALOG with the given modality and callback, then enters it.
	template <class DIALOG>
	static void	Start(
					Nib&					inNib,
					const char* 			inDialogName,
					AEObjectPtr				inContainer,
					const DialogModality&	inModality,
					B_DIALOG_CB				inFunction = NullCallback<DIALOG>());
	//@}
	
	/*! @name Running Dialogs
		
		These are convenience functions for displaying dialogs in an 
		application-modal fashion.  Because they don't return until the 
		dialog is dismissed, the dismissing command is returned as the 
		function result.
	*/
	//@{
	
	//! Displays the dialog in an application-modal fashion.
	UInt32			Run();
	
	//! Instantiates a @a DIALOG, then enters a modal event loop for it.
	template <class DIALOG, typename STATE>
	static UInt32   RunWithState(
						Nib&		inNib,
						const char*	inDialogName,
						AEObjectPtr	inContainer,
						STATE		inState);
	/*! @overload
	*/
	template <class DIALOG>
	static UInt32   Run(
						Nib&		inNib,
						const char*	inDialogName,
						AEObjectPtr	inContainer);
	
	//! Deprecated.  Use Run() instead.
	UInt32			EnterAppModalLoop();
	//@}
	
protected:
	
	//! @name Overridables
	//@{
	//! Called in response to dialog-confirming action on the part of the user.
	virtual void	ConfirmDialog(UInt32 inCommand);
	//! Called in response to dialog-infirming action on the part of the user.
	virtual void	CancelDialog(UInt32 inCommand);
	//! Hook to perform input validation.
	virtual bool	Validate(UInt32 inCommand);
	//! Dismisses the dialog.
	virtual void	DismissDialog(UInt32 inCommand);
	//@}
	
	// overrides from B::Window
	virtual bool	HandleCommand(const HICommandExtended& inHICommand);
	
private:
	
	typedef DialogCallbackBase1<Dialog, UInt32>	CallbackType;

	// Abstract base class for modality-specific functionality.
	class ModalityHandler : public boost::noncopyable
	{
	public:
		
		virtual	~ModalityHandler();
		
		bool	IsDisplayed() const		{ return (mDialog != NULL); }
		
		virtual void	Display(Dialog* inDialog) = 0;
		virtual void	Dismiss(bool inClose) = 0;
		
	protected:
		
	    //! Constructor.
	    ModalityHandler() : mDialog(NULL) {}
	    
	    // member variables
		Dialog*	mDialog;
	};
	
	//! Modality handler for modeless dialogs.
	class ModelessHandler : public ModalityHandler
	{
	public:
		
		virtual void	Display(Dialog* inDialog);
		virtual void	Dismiss(bool inClose);
	};

	//! Modality handler for application-modal dialogs.
	class AppModalHandler : public ModalityHandler
	{
	public:
		
		AppModalHandler() : mClose(false) {}
		
		virtual void	Display(Dialog* inDialog);
		virtual void	Dismiss(bool inClose);
		
	private:
		
		bool	mClose;
	};

	//! Modality handler for sheets.
	class SheetHandler : public ModalityHandler
	{
	public:
		
		explicit	SheetHandler(WindowRef inParent);
		
		void			SetAnimateOnHide(bool inAnimate)	{ mAnimate = inAnimate; }
		
		virtual void	Display(Dialog* inDialog);
		virtual void	Dismiss(bool inClose);
		
	private:
		
		WindowRef	mParent;
		bool		mAnimate;
	};
	
	// types
	typedef Window   inherited;
	
	static OSPtr<WindowRef>
			MutateWindowForModality(
				WindowRef				inWindow,
				const DialogModality&	inModality);
	
	// member variables
	UInt32                          mDismissingCommand;	//!< The @c HICommand that last caused the dialog to be dismissed.
	bool                            mCloseOnDismiss;
	std::auto_ptr<CallbackType>     mCallback;
	std::auto_ptr<ModalityHandler>	mModalityHandler;
};

// ------------------------------------------------------------------------------------------
/*!	@todo			Complete this!
	@param	DIALOG	Template parameter.  The class of the instantiated window.  Must be 
					Dialog or a class derived from Dialog.
*/
template <class DIALOG> void
Dialog::SetCallback(
	B_DIALOG_CB	inFunction)
{
	boost::function_requires< boost::ConvertibleConcept<DIALOG*, B::Dialog*> >();
	
    mCallback.reset(new DialogCallback1<DIALOG, Dialog, UInt32>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates a dialog of class @a DIALOG, from the window definition @a inDialogName in 
	nib file @a inNib.
	
	This function checks that the window class (as returned from the nib) matches the 
	modality requested in @a inModality.  If they do not match, a new window is created 
	with reasonable default values for @a inModality, and the original window's contents 
	(including HIViews) are moved to the new window.  An instance of @a DIALOG is then 
	created with the new window.  This mechanism allows callers to define a single 
	window in a nib file, and use it as a modeless dialog, modal dialog, or sheet, 
	depending on circumstances.
	
	The instance of @a DIALOG is then configured with @a inModality, @a inParent, and 
	@a inFunction.  The dialog is then returned, ready to be displayed via Enter().
	
	The function result type needs some explaining.  It's an @c std::pair, with the 
	first element holding a smart pointer to a @c WindowRef, and the second holding a 
	pointer to the @a DIALOG object.  The reason for this is that the dialog's lifetime is 
	controlled by the smart pointer;  one can't destroy a dialog object by calling 
	@c operator @c delete on it.  So if an exception occurs further up the call 
	chain, the dialog will be disposed properly when the smart pointer is destructed.
	Obviously, the second part of the pair (the @a DIALOG pointer) is there for convenient 
	type-safe access to the dialog object.
	
	@param	DIALOG	Template parameter.  The class of the instantiated window.  Must be 
					Dialog or a class derived from Dialog.
*/
template <class DIALOG>
boost::shared_ptr<DIALOG>
Dialog::Make(
	Nib&					inNib,			//!< The nib file from which to read the dialog.
	const char*				inDialogName,	//!< The dialog's name in the nib file.
	AEObjectPtr				inContainer,	//!< The AEOM container of the dialog.  This will usually be the application or a document.
	const DialogModality&	inModality,		//!< The dialog's modality.
	B_DIALOG_CB				inFunction /* = NullCallback<DIALOG>() */)	//!< A callback that is invoked when the dialog is dismissed.
{
	boost::function_requires< boost::ConvertibleConcept<DIALOG*, B::Dialog*> >();
	
	// Read in the window from the nib file.
	OSPtr<WindowRef>	windowRef(inNib.CreateWindow(inDialogName));
	
	// Mutate the window if necessary.  If a mutation is necessary, windowRef (being 
	// a smart pointer) will take care of releasing the original window.
	windowRef = MutateWindowForModality(windowRef, inModality);
	
	// Instantiate the dialog object.
	boost::shared_ptr<DIALOG>	dialogPtr(new DIALOG(windowRef, inContainer), 
										  WindowDeleter());
	
	// Finish initialising the Window part of the object.
	dialogPtr->PostCreateWindow(&inNib);
	
	// Initialise the Dialog part of the object.
	dialogPtr->SetModality(inModality);
	dialogPtr->SetCallback(inFunction);
	
	return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
/*!	This function is identical to Make(), except that the @a DIALOG constructor will be 
	passed an object of type @a STATE.  This can be used to communicate information 
	between the dialog and the code that instantiates it.
	
	@param	DIALOG	Template parameter.  The class of the instantiated window.  Must be 
					Dialog or a class derived from Dialog.
	@param	STATE	Template parameter.  Any C/C++ type.  It is passed to @a DIALOG's 
					constructor but is otherwise uninterpreted.
	
	@sa	Make()
*/
template <class DIALOG, typename STATE>
boost::shared_ptr<DIALOG>
Dialog::MakeWithState(
	Nib&					inNib,			//!< The nib file from which to read the dialog.
	const char*				inDialogName,	//!< The dialog's name in the nib file.
	AEObjectPtr				inContainer,	//!< The AEOM container of the dialog.  This will usually be the application or a document.
	STATE					inState,		//!< Some state that is passed to @a DIALOG's constructor.
	const DialogModality&	inModality,		//!< The dialog's modality.
	B_DIALOG_CB				inFunction /* = NullCallback<DIALOG>() */)	//!< A callback that is invoked when the dialog is dismissed.
{
	boost::function_requires< boost::ConvertibleConcept<DIALOG*, B::Dialog*> >();
	boost::function_requires< boost::CopyConstructibleConcept<STATE> >();
	
	// Read in the window from the nib file.
	OSPtr<WindowRef>	windowRef(inNib.CreateWindow(inDialogName));
	
	// Mutate the window if necessary.  If a mutation is necessary, windowRef (being 
	// a smart pointer) will take care of releasing the original window.
	windowRef = MutateWindowForModality(windowRef, inModality);
	
	// Instantiate the dialog object.
	boost::shared_ptr<DIALOG>	dialogPtr(new DIALOG(windowRef, inContainer, inState), 
										  WindowDeleter());
	
	// Finish initialising the Window part of the object.
	dialogPtr->PostCreateWindow(&inNib);
	
	// Initialise the Dialog part of the object.
	dialogPtr->SetModality(inModality);
	dialogPtr->SetCallback(inFunction);
	
	return (dialogPtr);
}

// ------------------------------------------------------------------------------------------
template <class DIALOG> void
Dialog::Enter(
	boost::shared_ptr<DIALOG>	inDialogPtr)	//!< The dialog to enter.
{
	boost::function_requires< boost::ConvertibleConcept<DIALOG*, B::Dialog*> >();
	
	// The caller doesn't have any way of identifying the dialog, so dispose of it 
	// upon dismissal.
	inDialogPtr->SetCloseOnDismiss(true);
	
	// Enter the dialog.  This may or may not return until it has been dismissed.
	inDialogPtr->Enter();
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates a dialog of class @a DIALOG, as per Make().  The dialog is then displayed 
	via Enter().  Whether the dialog has been dismissed by the time the function 
	returns depends upon @a inModality.  See Enter() for more information.
	
	@note	The dialog is automatically disposed upon dismissal.  If you would rather 
			hang on to the dialog (to reuse it later without having to read it in from 
			the nib file again), call Make() or MakeWithState(), then Enter().
	
	@note	The dialog's constructor is not passed a @a STATE object.  If your constructor 
			requires one, call MakeWithState() and Enter() directly.
	
	@param	DIALOG	Template parameter.  The class of the instantiated window.  Must be 
					Dialog or a class derived from Dialog.
	
	@sa		Make(), Enter()
*/
template <class DIALOG> void
Dialog::Start(
	Nib&					inNib,			//!< The nib file from which to read the dialog.
	const char*				inDialogName,	//!< The dialog's name in the nib file.
	AEObjectPtr				inContainer,	//!< The AEOM container of the dialog.  This will usually be the application or a document.
	const DialogModality&	inModality,		//!< The dialog's modality.
	B_DIALOG_CB				inFunction /* = NullCallback<DIALOG>() */)	//!< A callback that is invoked when the dialog is dismissed.
{
	Enter(Make<DIALOG>(inNib, inDialogName, inContainer, inModality, inFunction));
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates a dialog of class @a DIALOG, from the window definition @a inDialogName in 
	nib file @a inNib.  The @a DIALOG constructor will be passed an object of 
	type @a STATE.  This can be used to communicate information between the window and 
	the code that instantiates it.
	
	The dialog's modality is set to application-modal.  After instantiation, the dialog 
	is entered.  Control does not return from this function until the dialog is dismissed.
	
	@param	DIALOG	Template parameter.  The class of the instantiated window.  Must be 
					Dialog or a class derived from Dialog.
	@param	STATE	Template parameter.  Any C/C++ type.  It is passed to @a DIALOG's 
					constructor but is otherwise uninterpreted.
	
	@return			The @c HICommand that dismissed the dialog.
*/
template <class DIALOG, typename STATE> UInt32
Dialog::RunWithState(
	Nib&		inNib,			//!< The nib file from which to read the dialog.
	const char*	inDialogName,	//!< The dialog's name in the nib file.
	AEObjectPtr	inContainer,	//!< The AEOM container of the dialog.  This will usually be the application or a document.
	STATE		inState)		//!< Some state that is passed to @a DIALOG's constructor.
{
	boost::shared_ptr<DIALOG>	dialogPtr;
	
	dialogPtr = MakeWithState<DIALOG>(inNib, inDialogName, inContainer, inState, 
									  DialogModality::Modal());
	
	// Enter the dialog, wait for dismissal, and return the dismissing command.
	return (dialogPtr->Run());
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates a dialog of class @a DIALOG, from the window definition @a inDialogName in 
	nib file @a inNib.  Then the dialog's modality is set to application-modal, and it is 
	entered.  Control does not return from this function until the dialog is dismissed.
	
	@param	DIALOG	Template parameter.  The class of the instantiated window.  Must be 
					Dialog or a class derived from Dialog.
	
	@return			The @c HICommand that dismissed the dialog.
*/
template <class DIALOG> UInt32
Dialog::Run(
	Nib&		inNib,			//!< The nib file from which to read the dialog.
	const char*	inDialogName,	//!< The dialog's name in the nib file.
	AEObjectPtr	inContainer)	//!< The AEOM container of the dialog.  This will usually be the application or a document.
{
	boost::shared_ptr<DIALOG>	dialogPtr;
	
	dialogPtr = Make<DIALOG>(inNib, inDialogName, inContainer, 
							 DialogModality::Modal());
	
	// Enter the dialog, wait for dismissal, and return the dismissing command.
	return (dialogPtr->Run());
}


// We don't need this anymore
#undef B_DIALOG_CB

}	// namespace B


#endif	// BDialog_H_
