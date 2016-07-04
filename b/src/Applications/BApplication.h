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

#ifndef BApplication_H_
#define BApplication_H_

#pragma once

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BAEObject.h"
#include "BAEObjectSupport.h"
#include "BBundle.h"
#include "BEventHandler.h"

#include "BDocumentPolicyConcept.h"
#include "BPrintPolicyConcept.h"
#include "BUndoPolicyConcept.h"


namespace B {

// forward declarations
class	CommandData;
class	Nib;
class	ViewFactory;

/*!
	@brief	The application object.
	
	This is the topmost object in the application.  An application is typically started by 
	creating an instance of Application from within @c main():
	
	@code
		int main()
		{
			B::Application<B::NullDocumentPolicy, B::NullPrintPolicy>	theApp;
			
			theApp.Run();
			
			return 0;
		}
	@endcode
	
	The class' main responsibilities are:
	
	- Application initialisation.  This can be subdivided into the following areas:
		- Document class registration.
		- View registration.
		- Apple %Event registration.
		- AEOM class registration.
		- Document policy initialisation.
		- UI initialisation.
	- Support for required AppleEvents.
	- Implements the AEOM top-level container.
	- Allows access to the Application singleton.
	- Runs the application's event loop.
	
	Application has three template parameters, which modify its behaviour:
	
	- @a DOC_POLICY, which determines how the application supports documents.  This parameter 
	  is of class type and should follow the @a DOC_POLICY policy.  There are three supplied 
	  implementations of this policy:
		- NullDocumentPolicy, for applications that don't use documents.
		- SingleDocumentPolicy, for applications that only open one document at a time.
		- MultipleDocumentPolicy, for applications that can open more than one document 
		  at a time.
	- @a UNDO_POLICY, which determines how the application supports %Undo.  This parameter 
	  is of class type and should implement the @a UNDO_POLICY policy.  There are three supplied 
	  implementations of this policy:
		- NullUndoPolicy, for applications that don't need to support %Undo.
		- MultipleUndoPolicy, for applications that need separate %Undo and %Redo 
		  stacks of arbitrary size.
		- SingleUndoPolicy, for applications that need only a simple 1-level-deep 
		  %Undo.
	- @a PRINT_POLICY, which determines how the application supports printing.
	
	@todo	Complete documentation.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
class Application : public AEObject
{
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef class	DOC_POLICY;		//!< Template parameter.  Should be a class that implements the DOC_POLICY policy.
	typedef class	UNDO_POLICY;	//!< Template parameter.  Should be a class that implements the UNDO_POLICY policy.
	typedef class	PRINT_POLICY;	//!< Template parameter.  Should be a class that implements the PRINT_POLICY policy.
	//@}
#endif
	
	// Concept checks.
	BOOST_CLASS_REQUIRE(DOC_POLICY, B, DocumentPolicyConcept);
	BOOST_CLASS_REQUIRE(UNDO_POLICY, B, UndoPolicyConcept);
	BOOST_CLASS_REQUIRE(PRINT_POLICY, B, PrintPolicyConcept);
	
	//! @name Types
	//@{
	typedef DOC_POLICY		DocPolicyType;		//!< An easy-to-access synonym for @a DOC_POLICY.
	typedef UNDO_POLICY		UndoPolicyType;		//!< An easy-to-access synonym for @a UNDO_POLICY.
	typedef PRINT_POLICY	PrintPolicyType;	//!< An easy-to-access synonym for @a PRINT_POLICY.
	//@}
	
	//! @name Singleton Access
	//@{
	//! Returns the application object.
	static Application&	Get();
	//@}
	
	//! @name Constructor / Destructor
	//@{
	//! Constructor.
					Application();
	//! Destructor.
	virtual			~Application();
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns true if the application is in the process of quitting the main event loop.
	bool			IsQuitting() const;
	//! Returns the application bundle.
	const Bundle&	GetBundle() const;
	//! Returns the application's main nib.
	Nib&			GetNib() const;
	//! Returns the application's document policy.
	DocPolicyType&	GetDocumentPolicy();
	//! Returns the application's undo policy.
	UndoPolicyType& GetUndoPolicy();
	//! Returns the application's print policy.
	PrintPolicyType& GetPrintPolicy();
	//! Returns the application's process' name.
	String			GetName() const;
	//@}
	
	//! @name Event Loop
	//@{
	//! Initialises the application, then enters the event loop.
	virtual void	Run();
	//@}
	
protected:
	
	//! @name Initialisation
	//@{
	//! Main initialisation routine.
	virtual void	Initialize();
	//! Register the application's document classes, if any.
	virtual void	RegisterDocuments();
	//! Register the application's view classes, if any.
	virtual void	RegisterViews(ViewFactory& ioFactory);
	//! Register the application's Apple Events, if necessary.
	virtual void	RegisterAppleEvents(AEObjectSupport& ioObjectSupport);
	//! Register the application's help book, if any.
	virtual void	RegisterHelp();
	//! Loads the named menu bar.
	virtual void	LoadMenuBar(const char* inMenuBarName);
	//! Performs post-processing on the menu bar, once all of its items have been added.
	virtual void	PostprocessMenuBar();
	//@}
	
	//! @name User Interface
	//@{
	//! Displays the application's About box.
	virtual void	ShowAboutBox();
	//! Displays the application's Preferences dialog.
	virtual void	ShowPreferences();
	//@}
	
	//! @name Carbon %Event Handlers
	//@{
	//! Handles the kEventAppActivated event.
	virtual void	HandleActivated(
						WindowRef					inClickedWindow);
	//! Handles the kEventAppDeactivated event.
	virtual void	HandleDeactivated();
	//! Handles the kEventAppQuit event.
	virtual void	HandleQuit();
	//! Handles the kEventCommandProcess event.
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);
	//! Handles the kEventCommandUpdateStatus event.
	virtual bool	HandleUpdateStatus(
						const HICommandExtended&	inHICommand, 
						CommandData&				ioCmdData);
	//! Handles the kEventMenuEnableItems event.
	virtual bool	HandleEnableItems(
						MenuRef						inMenu);
	//@}
	
	//! @name Apple %Event Handlers
	//@{
	//! Handles the Open Application (@c 'oapp') event.
	virtual void	OpenApplication();
	//! Handles the Reopen Application (@c 'rapp') event.
	virtual void	ReopenApplication();
	//! Handles the Quit Application (@c 'quit') event.
	virtual void	QuitApplication(
						OSType			inSaveOption = kAEAsk);
	//@}
	
	//! @name Overrides from AEObject
	//@{
	virtual size_t		CountElements(
							DescType		inElementType) const;
	virtual AEObjectPtr	GetElementByIndex(
							DescType		inElementType, 
							size_t			inIndex) const;
	virtual AEObjectPtr	GetElementByName(
							DescType		inElementType, 
							const String&	inName) const;
	virtual AEObjectPtr	GetElementByUniqueID(
							DescType		inElementType, 
							SInt32			inUniqueID) const;
	virtual AEObjectPtr	CreateObject(
							DescType		inObjectClass,
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							const AERecord&	inProperties,
							const AEDesc&	inData);
	virtual void		ActivateObject();
	virtual void		MakeSpecifier(
							AEWriter&		ioWriter) const;
	virtual void		WriteProperty(
							DescType		inPropertyID, 
							AEWriter&		ioWriter) const;
	//@}
	
	static void	AddSubmenusToVector(
					MenuRef					inMenu,
					std::vector<MenuRef>&	ioVector);
private:
	
	typedef Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>	ApplicationType;
	
	// Carbon %Event handlers
	bool	AppActivated(
				Event<kEventClassApplication, kEventAppActivated>&		event);
	bool	AppDeactivated(
				Event<kEventClassApplication, kEventAppDeactivated>&	event);
	bool	AppQuit(
				Event<kEventClassApplication, kEventAppQuit>&			event);
	bool	CommandProcess(
				Event<kEventClassCommand, kEventCommandProcess>&		event);
	bool	CommandUpdateStatus(
				Event<kEventClassCommand, kEventCommandUpdateStatus>&	event);
	bool	MenuEnableItems(
				Event<kEventClassMenu, kEventMenuEnableItems>&			event);
	
	// Apple %Event handlers
	void	HandleOpenApplicationAppleEvent(
				AEEvent<kCoreEventClass, kAEOpenApplication>&	event);
	void	HandleReopenApplicationAppleEvent(
				AEEvent<kCoreEventClass, kAEReopenApplication>&	event);
	void	HandleQuitApplicationAppleEvent(
				AEEvent<kCoreEventClass, kAEQuitApplication>&	event);
	void	HandleShowPreferencesAppleEvent(
				AEEvent<kCoreEventClass, kAEShowPreferences>&	event);
	
	void	InitEventHandler(EventHandler& ioHandler);
	
	// member variables
	boost::shared_ptr<ApplicationType>	mSelfPtr;
	Bundle						mBundle;
	AEObjectSupport				mObjectSupport;
	boost::intrusive_ptr<Nib>	mAppNib;
	EventHandler				mEventHandler;
	DOC_POLICY					mDocumentPolicy;
	UNDO_POLICY					mUndoPolicy;
	PRINT_POLICY				mPrintPolicy;
	ProcessSerialNumber			mPSN;
	bool						mQuitting;
	OSPtr<CFSetRef>				mAppMenus;
	
	// static member variables
	static Application*	sInstance;
};

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>&
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::Get()
{
	B_ASSERT(sInstance != NULL);
	
	return (*sInstance);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
inline bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::IsQuitting() const
{
	return (mQuitting);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
inline const Bundle&
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetBundle() const
{
	return (mBundle);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
inline typename Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::DocPolicyType&
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetDocumentPolicy()
{
	return (mDocumentPolicy);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
inline typename Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::UndoPolicyType&
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetUndoPolicy()
{
	return (mUndoPolicy);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
inline typename Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::PrintPolicyType&
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetPrintPolicy()
{
	return (mPrintPolicy);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY>
inline Nib&
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetNib() const
{
	return (*mAppNib);
}

}	// namespace B


#endif	// BApplication_H_
