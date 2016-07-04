// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
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

#ifndef BAlert_H_
#define BAlert_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/function.hpp>
#include <boost/utility.hpp>

// B headers
#include "BDialogCallback.h"
#include "BDialogModality.h"
#include "BFwd.h"
#include "BString.h"
#include "BUrl.h"


namespace B {

// forward declarations
class	EventHandler;


#ifdef B_ALERT_CB
#	error "Somebody defined B_ALERT_CB ..."
#else
	//! Convenience macro to simplify callback declarations
#	define B_ALERT_CB	boost::function2<void, const ALERT&, Button>
#endif

/*!
	@brief	Alert dialogs and sheets.
	
	This class is a wrapper around standard HIToolbox alerts and alert sheets.
	It presents a uniform interface regardless of an alert's modality (i.e. 
	application-modal vs. sheet).
	
	@sa		@ref dialogs_alerts_nav
	@nosubgrouping
*/
class Alert : public boost::noncopyable
{
public:
	
	//! @name Types
	//@{
	
	//! Enumerates an alert's buttons according to their position in the alert.
	enum Button
	{
		kOK		= kHICommandOK,		//!< The OK button (rightmost).
		kCancel	= kHICommandCancel,	//!< The Cancel button (second from right).
		kOther	= kHICommandOther,	//!< The Other (sometimes called "Don't Save") button (third from right).
		kHelp	= 'help',			//!< The Help button.
		kNone	= 0					//!< Sentinel value.
	};
	
	//! Shorthand for %Alert configuration struct.
	typedef AlertStdCFStringAlertParamRec	Params;
	
	//! Convenience callback that does nothing.
	template <class ALERT> struct NullCallback
		{ void	operator () (const ALERT&, Button) const {} };
	
	//@}
	
	//! @name Constructors & Destructor
	//@{
	//! Default constructor.
			Alert();
	//! Destructor.
	virtual	~Alert();
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the alert's params struct.
	const Params&	GetParams() const			{ return (mParams); }
	//! Returns the alert's type.
	AlertType		GetType() const				{ return (mType); }
	//! Returns the alert's message text.
	const String&	GetMessageText() const		{ return (mMessageText); }
	//! Returns the alert's informative text.
	const String&	GetInformativeText() const	{ return (mInformativeText); }
	//! Returns the alert's dismissing button.  Only valid in a modal scenario.
	Button			GetButtonHit() const		{ return (mButtonHit); }
	//@}
	
	//! @name Setup
	//@{
	//! Sets the error text, which is the highlighted text in the alert.
	void	SetMessageText(const String& inMessageText);
	//! Sets the explanatory text, which is the secondary text in the alert.
	void	SetInformativeText(const String& inInformativeText);
	//! Sets the alert's modality.
	void	SetModality(const DialogModality& inModality);
	//! Determines whether the dialog gets closed (i.e. destroyed) after it's dismissed.
	void	SetCloseOnDismiss(bool inCloseOnDismiss);
	//! Installs a callback that will be called once the alert is dismissed.
	template <class ALERT>
	void	SetCallback(B_ALERT_CB inFunction);
	//! Controls the visibility of one of the alert's buttons.
	void	ShowButton(Button whichButton, bool showIt);
	//! Sets the caption of one of the alert's buttons.
	void	SetButtonText(Button whichButton, const String& inText);
	//! Identifies the default button, which is drawn highlighted and reacts to the Return and Enter keys.
	void	SetDefaultButton(Button whichButton);
	//! Identifies the cancel button, which reacts to the Escape and Command-period keys.
	void	SetCancelButton(Button whichButton);
	//! Sets the help book name in which to look for help.
	void	SetHelpBook(const String& inHelpBook);
	//! Sets the url of the help page to display.
	void	SetHelpUrl(const Url& inHelpUrl);
	//! Sets some flags controlling the alert.
	void	SetParamFlags(OptionBits flags);
	//! Clears some flags controlling the alert.
	void	ClearParamFlags(OptionBits flags);
	//! Places the alert on the screen (app-modal only).
	void	SetPosition(UInt16 inPosition);
	//! Sets the alert type, which determines the icon.
	void	SetAlertType(AlertType inAlertType);
	//@}
	
	/*! @name Making Alerts
		
		In B parlance, "Making" an alert means to instantiate it, returning a smart 
		pointer (in this case, an @c std::auto_ptr<>) that controls its lifetime.
	*/
	//@{
	
	//! Instantiates and returns an @a ALERT with the given arguments.
	template <class ALERT>
	static std::auto_ptr<ALERT>
				Make(
					const String&			inMessageText, 
					const String&			inInformativeText,
					const DialogModality&	inModality,
					B_ALERT_CB				inFunction = NullCallback<ALERT>());
	/*! @overload
	*/
	template <class ALERT>
	static std::auto_ptr<ALERT>
				Make(
					const String&			inMessageText, 
					const String&			inInformativeText,
					const DialogModality&	inModality,
					bool					inIsCautionAlert,
					bool					inShowCancelButton,
					B_ALERT_CB				inFunction = NullCallback<ALERT>());
	/*! @overload
	*/
	template <class ALERT>
	static std::auto_ptr<ALERT>
				Make(
					const String&			inMessageText, 
					const String&			inInformativeText,
					const DialogModality&	inModality,
					bool					inIsCautionAlert,
					bool					inShowCancelButton,
					bool					inShowOtherButton,
					Button					inDefaultButton,
					Button					inCancelButton,
					const String&			inOKButtonText,
					const String&			inCancelButtonText,
					const String&			inOtherButtonText,
					B_ALERT_CB				inFunction = NullCallback<ALERT>());
	//@}
	
	/*! @name Entering Alerts
		
		In B parlance, "Entering" an alert means to display it according to its 
		modality, i.e. if it's application-modal then the function doesn't return 
		until the alert is dismissed.
	*/
	//@{
	
	//! Displays the alert according to its modality.
	void		Enter();
	
	//! Displays the given alert according to its modality, taking control of the alert's lifetime.
	template <class ALERT>
	static void	Enter(std::auto_ptr<ALERT> inAlertPtr);
	//@}
	
	/*! @name Starting Alerts
		
		In B parlance, "Starting" is shorthand for "Making" followed by "Entering".
	*/
	//@{
	
	//! Instantiates an @a ALERT with the given arguments, then enters it.
	template <class ALERT>
	static void	Start(
					const String&			inMessageText, 
					const String&			inInformativeText,
					const DialogModality&	inModality,
					B_ALERT_CB				inFunction = NullCallback<ALERT>());
	/*! @overload
	*/
	template <class ALERT>
	static void	Start(
					const String&			inMessageText, 
					const String&			inInformativeText,
					const DialogModality&	inModality,
					bool					inIsCautionAlert,
					bool					inShowCancelButton,
					B_ALERT_CB				inFunction = NullCallback<ALERT>());
	/*! @overload
	*/
	template <class ALERT>
	static void	Start(
					const String&			inMessageText, 
					const String&			inInformativeText,
					const DialogModality&	inModality,
					bool					inIsCautionAlert,
					bool					inShowCancelButton,
					bool					inShowOtherButton,
					Button					inDefaultButton,
					Button					inCancelButton,
					const String&			inOKButtonText,
					const String&			inCancelButtonText,
					const String&			inOtherButtonText,
					B_ALERT_CB				inFunction = NullCallback<ALERT>());
	//@}
	
	/*! @name Running Alerts
		
		These are convenience functions for displaying alerts in an 
		application-modal fashion.  Because they don't return until the 
		alert is dismissed, the dismissing button is returned as the 
		function result.
	*/
	//@{
	
	//! Displays the alert in an application-modal fashion.
	Button			Run();
	
	//! Instantiates an @a ALERT, then enters a modal event loop for it.
	template <class ALERT>
	static Button	Run(
						const String&		inMessageText, 
						const String&		inInformativeText);
	/*! @overload
	*/
	template <class ALERT>
	static Button	Run(
						const String&		inMessageText, 
						const String&		inInformativeText,
						bool				inIsCautionAlert,
						bool				inShowCancelButton);
	/*! @overload
	*/
	template <class ALERT>
	static Button	Run(
						const String&		inMessageText, 
						const String&		inInformativeText,
						bool				inIsCautionAlert,
						bool				inShowCancelButton,
						bool				inShowOtherButton,
						Button				inDefaultButton,
						Button				inCancelButton,
						const String&		inOKButtonText,
						const String&		inCancelButtonText,
						const String&		inOtherButtonText);
	//@}
	
protected:
	
	//! @name Overridables
	//@{
	//! Called in response to click on the help button.
	virtual void	ShowHelp();
	//! Called in response to click in a dismissing button.
	virtual void	DismissAlert(Button inButton);
	//@}
	
private:
	
	typedef DialogCallbackBase1<Alert, Button>	CallbackType;
	
	// Abstract base class for modality-specific functionality.
	class ModalityHandler : public boost::noncopyable
	{
	public:
		
		virtual			~ModalityHandler();
		virtual bool	IsDisplayed() const = 0;
		virtual void	Display() = 0;
		virtual void	Dismiss() = 0;
		
	protected:
		
		ModalityHandler(Alert& inAlert);
		
		void	ButtonClicked(Alert::Button inButton, bool inWillDismiss = true);
		
		CFStringRef	GetMessageText() const;
		CFStringRef	GetInformativeText() const;
		
		Alert&	mAlert;
	};

	//! Modality handler for application-modal dialogs.
	class AppModalHandler : public ModalityHandler
	{
	public:
		
		AppModalHandler(Alert& inAlert);
		
		virtual bool	IsDisplayed() const;
		virtual void	Display();
		virtual void	Dismiss();
		
	private:
		
		// member variables
		bool	mDisplayed;
	};

	//! Modality handler for sheets.
	class SheetHandler : public ModalityHandler
	{
	public:
		
		SheetHandler(Alert& inAlert, WindowRef inParent);
		
		virtual bool	IsDisplayed() const;
		virtual void	Display();
		virtual void	Dismiss();
		
	private:
		
		void	Create();
		
		// Carbon %Event handlers
		bool	CommandProcess(
					Event<kEventClassCommand, kEventCommandProcess>&	event);
		bool	WindowClosed(
					Event<kEventClassWindow, kEventWindowClosed>&		event);
		
		// member variables
		WindowRef					mParent;
		OSPtr<WindowRef>			mWindow;
		std::auto_ptr<EventHandler>	mEventHandler;
		bool						mDisplayed;
	};
	
	void	ButtonClicked(Button inButton, bool inWillDismiss);
	
	//! Utility function for configuring the alert.
	void	Configure(
				const String&			inMessageText, 
				const String&			inInformativeText,
				const DialogModality&	inModality);
	/*! @overload
	*/
	void	Configure(
				bool					inIsCautionAlert,
				bool					inShowCancelButton);
	/*! @overload
	*/
	void	Configure(
				bool					inIsCautionAlert,
				bool					inShowCancelButton,
				bool					inShowOtherButton,
				Button					inDefaultButton,
				Button					inCancelButton,
				const String&			inOKButtonText,
				const String&			inCancelButtonText,
				const String&			inOtherButtonText);
	
	// member variables
	Params		mParams;			//!< Alert display configuration.
	AlertType	mType;				//!< The type of alert.
	String		mMessageText;		//!< The alert's main text.
	String		mInformativeText;	//!< The alert's secondary text.
	String		mOKText;			//!< The caption on the alert's rightmost button.
	String		mCancelText;		//!< The caption on the alert's middle button.
	String		mOtherText;			//!< The caption on the alert's leftmost button.
	String		mHelpBook;
	Url			mHelpUrl;
	bool		mShowHelp;
	bool		mCloseOnDismiss;
	Button		mButtonHit;
	std::auto_ptr<CallbackType>     mCallback;
	std::auto_ptr<ModalityHandler>	mModalityHandler;
	
	// friends
	friend class	ModalityHandler;
};

// ------------------------------------------------------------------------------------------
/*!	@todo			Complete this!
	@param	ALERT	Template parameter.  The class of the instantiated alert.  Must be 
					Alert or a class derived from Alert.
*/
template <class ALERT> void
Alert::SetCallback(
	B_ALERT_CB	inFunction)
{
	boost::function_requires< boost::ConvertibleConcept<ALERT*, B::Alert*> >();
	
    mCallback.reset(new DialogCallback1<ALERT, Alert, Button>(inFunction));
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates an alert of class @a ALERT.  The instance of @a ALERT is then configured 
	with the given arguments.  Finally the alert is returned, ready to be displayed via 
	Enter().
	
	The function performs only basic configuration of the alert.  In particular, the alert 
	will only display the OK button (which will also be the default button).  If the caller 
	requires that other buttons be displayed, or that the default button not be the OK 
	button, then it should perform additional configuration prior to display.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@return			The @a ALERT object, wrapped in an @c std::auto_ptr<>.  This implies 
					that control over the object's lifetime is given to the caller.
*/
template <class ALERT>
std::auto_ptr<ALERT>
Alert::Make(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	const DialogModality&	inModality,			//!< The alert's modality.
	B_ALERT_CB				inFunction /* = NullCallback<ALERT>() */)	//!< A callback that is invoked when the alert is dismissed.
{
	boost::function_requires< boost::DefaultConstructibleConcept<ALERT> >();
	boost::function_requires< boost::ConvertibleConcept<ALERT*, B::Alert*> >();
	
	// Instantiate the alert object.
	std::auto_ptr<ALERT>	alertPtr(new ALERT);
	
	// Configure the alert.
	alertPtr->Configure(inMessageText, inInformativeText, inModality);
	alertPtr->SetCallback(inFunction);
	
	return (alertPtr);
}

// ------------------------------------------------------------------------------------------
/*!	This variant performs additional configuration of the alert.  With it one can generate 
	any of the three basic alert types (note, caution & stop), as long as the button 
	text and keyboard equivalents don't need to be customised.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@return			The @a ALERT object, wrapped in an @c std::auto_ptr<>.  This implies 
					that control over the object's lifetime is given to the caller.
*/
template <class ALERT>
std::auto_ptr<ALERT>
Alert::Make(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	const DialogModality&	inModality,			//!< The alert's modality.
	bool					inIsCautionAlert,	//!< Is this a "caution" alert?
	bool					inShowCancelButton,	//!< Should the Cancel button be displayed?
	B_ALERT_CB				inFunction /* = NullCallback<ALERT>() */)	//!< A callback that is invoked when the alert is dismissed.
{
	// Instantiate the alert object.
	std::auto_ptr<ALERT>	alertPtr(Make<ALERT>(inMessageText, inInformativeText, 
												 inModality, inFunction));
	
	alertPtr->Configure(inIsCautionAlert, inShowCancelButton);
	
	return (alertPtr);
}

// ------------------------------------------------------------------------------------------
/*!	This variant performs additional configuration of the alert.  With it one can generate 
	almost any alert that doesn't require a help button.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@return			The @a ALERT object, wrapped in an @c std::auto_ptr<>.  This implies 
					that control over the object's lifetime is given to the caller.
*/
template <class ALERT>
std::auto_ptr<ALERT>
Alert::Make(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	const DialogModality&	inModality,			//!< The alert's modality.
	bool					inIsCautionAlert,	//!< Is this a "caution" alert?
	bool					inShowCancelButton,	//!< Should the Cancel button be displayed?
	bool					inShowOtherButton,	//!< Should the Other button be displayed?
	Button					inDefaultButton,	//!< Which is the default button (i.e. the one that reacts to the Return and Enter keys)?
	Button					inCancelButton,		//!< Which is the cancel button (i.e. the one that reacts to the Escape and Command-period keys)?
	const String&			inOKButtonText,		//!< The text of the OK button.  If empty, the default text is used instead.
	const String&			inCancelButtonText,	//!< The text of the Cancel button.  Ignored unless @a inShowCancelButton is @c true.
	const String&			inOtherButtonText,	//!< The text of the Other button.  Ignored unless @a inShowOtherButton is @c true.
	B_ALERT_CB				inFunction /* = NullCallback<ALERT>() */)	//!< A callback that is invoked when the alert is dismissed.
{
	// Instantiate the alert object.
	std::auto_ptr<ALERT>	alertPtr(Make<ALERT>(inMessageText, inInformativeText, 
												 inModality, inFunction));
	
	alertPtr->Configure(inIsCautionAlert, inShowCancelButton, inShowOtherButton, 
						inDefaultButton, inCancelButton, inOKButtonText, 
						inCancelButtonText, inOtherButtonText);
	
	return (alertPtr);
}

// ------------------------------------------------------------------------------------------
template <class ALERT> void
Alert::Enter(
	std::auto_ptr<ALERT>	inAlertPtr)	//!< The alert to enter.
{
	boost::function_requires< boost::ConvertibleConcept<ALERT*, B::Alert*> >();
	
	// The caller has given us control over the lifetime of the alert, so dispose of it 
	// upon dismissal.
	inAlertPtr->SetCloseOnDismiss(true);
	
	// Enter the alert.  This may or may not return until the alert has been dismissed.
	inAlertPtr->Enter();
	
	// We don't need this anymore.
	inAlertPtr.release();
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates an alert of class @a ALERT, as per Make().  The alert is then displayed 
	via Enter().  Whether the alert has been dismissed by the time the function 
	returns depends upon @a inModality.  See Enter() for more information.
	
	@note	The alert is automatically disposed upon dismissal.  If you would rather 
			hang on to the alert (to reuse it later without having to read it in from 
			the nib file again), call Make(), then Enter().
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@sa		Make(), Enter()
*/
template <class ALERT> void
Alert::Start(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	const DialogModality&	inModality,			//!< The alert's modality.
	B_ALERT_CB				inFunction /* = NullCallback<ALERT>() */)	//!< A callback that is invoked when the alert is dismissed.
{
	Enter(Make<ALERT>(inMessageText, inInformativeText, inModality, inFunction));
}

// ------------------------------------------------------------------------------------------
/*!	This variant performs additional configuration of the alert.  With it one can generate 
	any of the three basic alert types (note, caution & stop), as long as the button 
	text and keyboard equivalents don't need to be customised.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@sa		Make()
*/
template <class ALERT> void
Alert::Start(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	const DialogModality&	inModality,		//!< The alert's modality.
	bool					inIsCautionAlert,	//!< Is this a "caution" alert?
	bool					inShowCancelButton,	//!< Should the Cancel button be displayed?
	B_ALERT_CB				inFunction /* = NullCallback<ALERT>() */)	//!< A callback that is invoked when the alert is dismissed.
{
	Enter(Make<ALERT>(inMessageText, inInformativeText, inModality, 
					  inIsCautionAlert, inShowCancelButton, inFunction));
}

// ------------------------------------------------------------------------------------------
/*!	This variant performs additional configuration of the alert.  With it one can generate 
	any of the three basic alert types (note, caution & stop), as long as the button 
	text and keyboard equivalents don't need to be customised.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@sa		Make()
*/
template <class ALERT> void
Alert::Start(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	const DialogModality&	inModality,		//!< The alert's modality.
	bool					inIsCautionAlert,	//!< Is this a "caution" alert?
	bool					inShowCancelButton,	//!< Should the Cancel button be displayed?
	bool					inShowOtherButton,	//!< Should the Other button be displayed?
	Button					inDefaultButton,	//!< Which is the default button (i.e. the one that reacts to the Return and Enter keys)?
	Button					inCancelButton,		//!< Which is the cancel button (i.e. the one that reacts to the Escape and Command-period keys)?
	const String&			inOKButtonText,		//!< The text of the OK button.  If empty, the default text is used instead.
	const String&			inCancelButtonText,	//!< The text of the Cancel button.  Ignored unless @a inShowCancelButton is @c true.
	const String&			inOtherButtonText,	//!< The text of the Other button.  Ignored unless @a inShowOtherButton is @c true.
	B_ALERT_CB				inFunction /* = NullCallback<ALERT>() */)	//!< A callback that is invoked when the alert is dismissed.
{
	Enter(Make<ALERT>(inMessageText, inInformativeText, inModality, 
					  inIsCautionAlert, inShowCancelButton, inShowOtherButton, 
					  inDefaultButton, inCancelButton, inOKButtonText, 
					  inCancelButtonText, inOtherButtonText, inFunction));
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates an alert object of class @a ALERT.  The instance of @a ALERT is then 
	configured with the given arguments.
	
	The alert's modality is set to application-modal.  After instantiation, the alert 
	is entered.  Control does not return from this function until the alert is dismissed.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@return			The Button that dismissed the alert.
*/
template <class ALERT> Alert::Button
Alert::Run(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText)	//!< The alert's secondary text.  May be empty.
{
	std::auto_ptr<ALERT>	alertPtr(Make<ALERT>(inMessageText, inInformativeText, 
												 DialogModality::Modal()));
	
	// Enter the alert, wait for dismissal, and return the dismissing button.
	return (alertPtr->Run());
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates an alert object of class @a ALERT.  The instance of @a ALERT is then 
	configured with the given arguments.
	
	The alert's modality is set to application-modal.  After instantiation, the alert 
	is entered.  Control does not return from this function until the alert is dismissed.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@return			The Button that dismissed the alert.
*/
template <class ALERT> Alert::Button
Alert::Run(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	bool					inIsCautionAlert,	//!< Is this a "caution" alert?
	bool					inShowCancelButton)	//!< Should the Cancel button be displayed?
{
	std::auto_ptr<ALERT>	alertPtr(Make<ALERT>(inMessageText, inInformativeText, 
												 DialogModality::Modal(), 
												 inIsCautionAlert, inShowCancelButton));
	
	// Enter the alert, wait for dismissal, and return the dismissing button.
	return (alertPtr->Run());
}

// ------------------------------------------------------------------------------------------
/*!	Instantiates an alert object of class @a ALERT.  The instance of @a ALERT is then 
	configured with the given arguments.
	
	The alert's modality is set to application-modal.  After instantiation, the alert 
	is entered.  Control does not return from this function until the alert is dismissed.
	
	@param	ALERT	Template parameter.  The class of the instantiated window.  Must be 
					Alert or a class derived from Alert.
	
	@return			The Button that dismissed the alert.
*/
template <class ALERT> Alert::Button
Alert::Run(
	const String&			inMessageText,		//!< The alert's principal text.
	const String&			inInformativeText,	//!< The alert's secondary text.  May be empty.
	bool					inIsCautionAlert,	//!< Is this a "caution" alert?
	bool					inShowCancelButton,	//!< Should the Cancel button be displayed?
	bool					inShowOtherButton,	//!< Should the Other button be displayed?
	Button					inDefaultButton,	//!< Which is the default button (i.e. the one that reacts to the Return and Enter keys)?
	Button					inCancelButton,		//!< Which is the cancel button (i.e. the one that reacts to the Escape and Command-period keys)?
	const String&			inOKButtonText,		//!< The text of the OK button.  If empty, the default text is used instead.
	const String&			inCancelButtonText,	//!< The text of the Cancel button.  Ignored unless @a inShowCancelButton is @c true.
	const String&			inOtherButtonText)	//!< The text of the Other button.  Ignored unless @a inShowOtherButton is @c true.
{
	std::auto_ptr<ALERT>	alertPtr(Make<ALERT>(inMessageText, inInformativeText, 
												 DialogModality::Modal(), 
												 inIsCautionAlert, inShowCancelButton, 
												 inShowOtherButton, inDefaultButton,
												 inCancelButton, inOKButtonText,
												 inCancelButtonText, inOtherButtonText));
	
	// Enter the alert, wait for dismissal, and return the dismissing button.
	return (alertPtr->Run());
}


// We don't need this anymore
#undef B_ALERT_CB

}	// namespace B


#endif	// BAlert_H_
