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

// Library headers
#include "MoreProcesses.h"

// B headers
#include "BAboutBox.h"
#include "BAEEvent.h"
#include "BAEWriter.h"
#include "BApplication.h"
#include "BEvent.h"
#include "BException.h"
#include "BHelpUtilities.h"
#include "BWindow.h"


namespace
{
	struct null_deleter
	{
		void	operator () (void const*) const  {}
	};
}

namespace B {

template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>*
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::sInstance	= NULL;

// ------------------------------------------------------------------------------------------
/*!	The constructor puts the Application object in the following state:
		- Its bundle is the main bundle.
		- Its nib is that specified in the bundle's Info.plist file.
		- It installs Carbon %Event handlers.
		- Its document policy is an instance of @a DOC_POLICY.
		- Its print policy is an instance of @a PRINT_POLICY.
		- Its undo manager is taken from @a inUndoManager, or an instance of UndoManager if 
		  inUndoManager is @c NULL.
		- Its "frontmost" status is determined from the OS.
	
	@note	If @a inUndoManager is not @c NULL, then ownership of it is transferred 
			to the Application object.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> 
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::Application()
	: AEObject(AEObjectPtr(), cApplication), 
	  mSelfPtr(this, null_deleter()),
	  mBundle(Bundle::Main()), 
	  mAppNib(new Nib(mBundle.MainNib())), 
	  mEventHandler(GetApplicationEventTarget()), 
	  mDocumentPolicy(GetAEObjectPtr(), mBundle, mAppNib.get()), 
	  mUndoPolicy(GetApplicationEventTarget(), mBundle), 
	  mPrintPolicy(GetApplicationEventTarget(), mBundle), 
	  mQuitting(false)
{
	B_ASSERT(sInstance == NULL);
	sInstance = this;
	
	// Prime the error handler.
	ErrorHandler::Get();
	
	SetDefaultObject(GetAEObjectPtr());
	
	OSStatus	err;
	
	err = GetCurrentProcess(&mPSN);
	B_THROW_IF_STATUS(err);
	
	InitEventHandler(mEventHandler);
	mPrintPolicy.EnablePrintCommand(false);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> 
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::~Application()
{
	B_ASSERT(sInstance == this);
	sInstance = NULL;
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::InitEventHandler(EventHandler& ioHandler)
{
	ioHandler.Add(this, &Application::AppActivated);
	ioHandler.Add(this, &Application::AppDeactivated);
	ioHandler.Add(this, &Application::AppQuit);
	ioHandler.Add(this, &Application::CommandProcess);
	ioHandler.Add(this, &Application::CommandUpdateStatus);
	ioHandler.Add(this, &Application::MenuEnableItems);
	
	ioHandler.Init();
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> String
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetName() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = CopyProcessName(&mPSN, &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
/*!	The function does not return until the application has quit the event loop.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::Run()
{
	try
	{
		Initialize();
		
		::RunApplicationEventLoop();
	}
	catch (...)
	{
		B_SIGNAL("App Initialization failed.");
	}
}

// ------------------------------------------------------------------------------------------
/*!	This function is called immediately prior to entering the event loop.  It calls each of 
	the more specialised initialisation functions (eg, RegisterDocuments()) in turn.
	
	Derived classes will normally override the more specialised functions.  However, if they 
	have initialisation needs that aren't covered by the base class, they may override this 
	function instead.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::Initialize()
{
	// Documents
	RegisterDocuments();
	
	// Views
	ViewFactory&	factory(ViewFactory::Default());
	
	factory.SetDefaultSignature(mBundle.PackageCreator());
	RegisterViews(factory);
	
	// Menu bar -- some of the later initialisation stages access or modify menus, so the 
	// menu bar needs to be in place before then.
	LoadMenuBar("MenuBar");
	
	// help book
	RegisterHelp();
	
	// AppleEvents & AEOM
	mObjectSupport.RegisterScriptingDefinitions(mBundle);
	AEObject::RegisterClassEventHandlers(mObjectSupport);
	RegisterAppleEvents(mObjectSupport);
	
	// Document policy
	mDocumentPolicy.Init();
	
	PostprocessMenuBar();
	
	::InitCursor();
}

// ------------------------------------------------------------------------------------------
/*!	Applications that support documents may override this to register their document 
	classes with the document policy's document factory object.  Currently only 
	MultipleDocumentFactory supports document registration.
	
	Assuming the application's document policy contains an instance of 
	MultipleDocumentFactory, a document class can be registered like this:
	
	@code
		GetDocumentPolicy().GetFactory().RegisterDocumentClass<MyDocument, cMyDocumentClass>();
	@endcode
	
	where @p MyDocument is the C++ class of the document, and @p cMyDocumentClass is the 
	AEOM identifier for the class.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::RegisterDocuments()
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
/*!	Applications that contain classes derived from PredefinedView may register their 
	derived classes by overriding this member function.  Once a class is registered, then 
	any time a Window object is instantiated from a nib, any HIView within it that matches 
	the ControlKind & HIViewID specified at registration time will automatically have a 
	corresponding view object created for it.
	
	Views are typically registered like this:
	
	@code
		ioFactory.Register<MyViewClass>(kMyViewObjectID);
	@endcode
	
	where @p MyViewClass is the C++ class of the view, and @p kMyViewObjectID is the 
	integer identifier for a particular instance of the view in a nib.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::RegisterViews(
	ViewFactory& /* ioFactory */)	//!< The view factory with which to register views.
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
/*!	The standard Apple Events are already routed properly to the correct object.  However, 
	in some cases some additional work is required.  First, for events that don't have 
	a direct parameter;  a callback needs to be installed for each of them.  Second, for 
	events that have non-standard direct parameters, i.e. that aren't object specifiers. 
	That is the case of the Open Document event.
	
	Derived classes which override this function should call the base class before 
	performing their own registrations.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::RegisterAppleEvents(
	AEObjectSupport&	ioObjectSupport)
{
	// Install event handlers for events lacking a direct parameter.
	
	// The Open Application event is required.
	B_ASSERT(ioObjectSupport.IsEventDefined(kCoreEventClass, kAEOpenApplication));
	ioObjectSupport.SetClassEventHandler(this, 
		&Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleOpenApplicationAppleEvent);
	
	// The Reopen Application event is required.
	B_ASSERT(ioObjectSupport.IsEventDefined(kCoreEventClass, kAEReopenApplication));
	ioObjectSupport.SetClassEventHandler(this, 
		&Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleReopenApplicationAppleEvent);
	
	// The Quit Application event is required.
	B_ASSERT(ioObjectSupport.IsEventDefined(kCoreEventClass, kAEQuitApplication));
	ioObjectSupport.SetClassEventHandler(this, 
		&Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleQuitApplicationAppleEvent);

	// The Show Preferences event isn't required.
	if (ioObjectSupport.IsEventDefined(kCoreEventClass, kAEShowPreferences))
	{
		ioObjectSupport.SetClassEventHandler(this, 
			&Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleShowPreferencesAppleEvent);
	}
	
	// Give the document policy a chance to register AppleEvents for itself.
	mDocumentPolicy.RegisterAppleEvents(ioObjectSupport);
	
	// Ditto for the Undo policy.
	mUndoPolicy.RegisterAppleEvents(ioObjectSupport);
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation calls AHRegisterHelpBook() if the application bundle's 
	Info.plist contains a @c CFBundleHelpBookFolder key, which is required to support 
	Apple Help.
	
	Derived classes may override this function to provide extra Help menu items.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::RegisterHelp()
{
	if (HelpUtilities::HasHelpBook())
	{
		HelpUtilities::RegisterHelpBook();
	}
}

// ------------------------------------------------------------------------------------------
/*!	Loads the named menu bar, and makes it the current menu bar.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::LoadMenuBar(
	const char* inMenuBarName)	// The name of the menu bar to load.
{
	mAppNib->SetMenuBar(inMenuBarName);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::PostprocessMenuBar()
{
	OSPtr<MenuRef>			rootMenu(AcquireRootMenu(), from_copy);
	std::vector<MenuRef>	appMenus;
	OSPtr<CFSetRef>			appMenusSet;
	MenuRef					applicationMenu, windowMenu, helpMenu;
	OSStatus				err;
	
	// We consider three menus to be "system" menus:  Application, Window, and Help.
	// 
	// How do we identify a particular menu?  By searching for commands that reside 
	// in the menu...

	// The Application menu contains the "About" command.
	err = GetIndMenuItemWithCommandID(NULL, kHICommandAbout, 1, &applicationMenu, NULL);
	if (err != noErr)
		applicationMenu = NULL;

	// The Window menu contains special markers for the window list.
	err = GetIndMenuItemWithCommandID(NULL, kHICommandWindowListSeparator, 1, &windowMenu, NULL);
	if (err != noErr)
		windowMenu = NULL;

	// The Help menu is obtained through a dedicated API.
	err = HMGetHelpMenu(&helpMenu, NULL);
	if (err != noErr)
		helpMenu = NULL;
	
	MenuItemIndex	count	= CountMenuItems(rootMenu);
	
	for (MenuItemIndex i = 1; i <= count; i++)
	{
		MenuRef	menu;
		
		err = GetMenuItemHierarchicalMenu(rootMenu, i, &menu);
		
		// Skip if this isn't a menu.
		if ((err != noErr) || (menu == NULL))
			continue;
		
		// Skip if this is a system menu.
		if ((menu == applicationMenu) || (menu == windowMenu) || (menu == helpMenu))
			continue;
		
		appMenus.push_back(menu);
		AddSubmenusToVector(menu, appMenus);
	}
	
	if (!appMenus.empty())
	{
		appMenusSet.reset(CFSetCreate(NULL, 
									  (const void **) &appMenus[0], 
									  appMenus.size(), 
									  &kCFTypeSetCallBacks), 
						  from_copy);
	}
	
	mAppMenus = appMenusSet;
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::AddSubmenusToVector(
	MenuRef					inMenu,
	std::vector<MenuRef>&	ioVector)
{
	MenuItemIndex	count	= CountMenuItems(inMenu);
	
	for (MenuItemIndex i = 1; i <= count; i++)
	{
		MenuRef		subMenu;
		OSStatus	err;
		
		err = GetMenuItemHierarchicalMenu(inMenu, i, &subMenu);
		
		if ((err == noErr) && (subMenu != NULL))
		{
			ioVector.push_back(subMenu);
			AddSubmenusToVector(subMenu, ioVector);
		}
	}
}

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to implement a custom About Box.
	
	The default implementation brings up the B framework's built-in About Box, which 
	follows the Aqua Human Interface Guidelines.
	
	In order to display a description of the application in the About Box, you need to 
	add a key called "B::AboutBoxDescription" to your InfoPlist.strings file (or to your 
	Info.plist, if you aren't localising your application).
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::ShowAboutBox()
{
	B::AboutBox::Display(GetAEObjectPtr(), GetBundle(), 
						 GetBundle().InfoString(CFSTR("B::AboutBoxDescription")));
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to implement the application's 
	Preferences window.
	
	The default implementation does nothing.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::ShowPreferences()
{
}


#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to do some processing whenever the 
	application becomes active.  The override should call the base class before doing its own 
	processing.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleActivated(
	WindowRef					/* inClickedWindow */)	//!< If non @c NULL, then the activation occurred as a result of the user clicking in this window.
{
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to do some processing whenever the 
	application is no longer frontmost.  The override should call the base class after doing 
	its own processing.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleDeactivated()
{
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to do some processing when the 
	main event loop is exited.  The override should call the base class before doing its own 
	processing.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleQuit()
{
	mQuitting = true;
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function to handle application-wide commands (i.e. 
	commands that aren't handled by other entities such as windows or documents).
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandAbout:
		ShowAboutBox();
		break;
		
	case kHICommandPreferences:
		ShowPreferences();
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function to update the status of application-wide 
	commands (i.e. commands that aren't handled by other entities such as windows or 
	documents).
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	CommandData&				ioCmdData)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandAbout:
	case kHICommandQuit:
		ioCmdData.SetEnabled(true);
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
/*!	This function is a solution to the problem of menu items not being enabled/disabled 
	properly if no handler in the chain leading up from the user input focus knows about 
	them.  For example, suppose there is a menu item "Foo" that is only handled by a 
	custom HIView called FooView.  When a FooView is in the focus chain, it enables the 
	Foo menu item.  However, when the focus moves elsewhere, there aren't any handlers in 
	the chain that "know" Foo needs to be disabled.
	
	So, what this function does is to disable all menu items in non-system menus.  After it 
	returns, the kEventCommandUpdateStatus events are sent, which give handlers the 
	chance to re-renable menu items as appropriate.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleEnableItems(
	MenuRef	inMenu)
{
	if ((mAppMenus != NULL) && CFSetContainsValue(mAppMenus, inMenu))
	{
		DisableAllMenuItems(inMenu);
	}
	
	return (false);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::AppActivated(
	Event<kEventClassApplication, kEventAppActivated>&	event)
{
	HandleActivated(event.mWindowRef);
	
	return (false);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::AppDeactivated(
	Event<kEventClassApplication, kEventAppDeactivated>&	/* event */)
{
	HandleDeactivated();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::AppQuit(
	Event<kEventClassApplication, kEventAppQuit>&	/* event */)
{
	HandleQuit();
	
	return (false);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	return (this->HandleCommand(event.mHICommand));
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	return (this->HandleUpdateStatus(event.mHICommand, event.mData));
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> bool
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::MenuEnableItems(
	Event<kEventClassMenu, kEventMenuEnableItems>&	event)
{
	return (this->HandleEnableItems(event.mMenuRef));
}


#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to change the application's 
	behaviour upon receipt of an Open Application (@c 'oapp') event.
	
	By default, the application delegates to its document policy.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::OpenApplication()
{
	mDocumentPolicy.OpenApplication();
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to change the application's 
	behaviour upon receipt of an Reopen Application (@c 'rapp') event.
	
	By default, the application delegates to its document policy.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::ReopenApplication()
{
	mDocumentPolicy.ReopenApplication();
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function in order to change the application's 
	behaviour upon receipt of an Quit Application (@c 'quit') event.
	
	By default, the application delegates to its document policy.
*/
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::QuitApplication(
	OSType			inSaveOption /* = kAEAsk */)
{
	mDocumentPolicy.QuitApplication(inSaveOption);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleOpenApplicationAppleEvent(
	AEEvent<kCoreEventClass, kAEOpenApplication>&	/* event */)
{
	OpenApplication();
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleReopenApplicationAppleEvent(
	AEEvent<kCoreEventClass, kAEReopenApplication>&	/* event */)
{
	ReopenApplication();
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleQuitApplicationAppleEvent(
	AEEvent<kCoreEventClass, kAEQuitApplication>&	event)
{
	QuitApplication(event.mSaveOption);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::HandleShowPreferencesAppleEvent(
	AEEvent<kCoreEventClass, kAEShowPreferences>&	/* event */)
{
	ShowPreferences();
}

#pragma mark -

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> size_t
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::CountElements(
	DescType		inElementType) const
{
	size_t	count   = 0;
	
	if (DoesClassInheritFrom(inElementType, cWindow))
	{
		count = Window::CountWindowsOfClass(inElementType);
	}
	else if (DoesClassInheritFrom(inElementType, cDocument))
	{
		count = mDocumentPolicy.CountDocuments(inElementType);
	}
	else
	{
		count = AEObject::CountElements(inElementType);
	}
	
	return (count);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> AEObjectPtr
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetElementByIndex(
	DescType		inElementType, 
	size_t			inIndex) const
{
	AEObjectPtr	obj;
	
	if (DoesClassInheritFrom(inElementType, cWindow))
	{
		Window*	window	= Window::GetWindowOfClassByIndex(inElementType, inIndex);
		
		if (window != NULL)
			obj = window->GetAEObjectPtr();
	}
	else if (DoesClassInheritFrom(inElementType, cDocument))
	{
		obj = mDocumentPolicy.GetDocumentByIndex(inElementType, inIndex);
	}
	else
	{
		obj = AEObject::GetElementByIndex(inElementType, inIndex);
	}
	
	return (obj);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> AEObjectPtr
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetElementByName(
	DescType		inElementType, 
	const String&	inName) const
{
	AEObjectPtr	obj;
	
	if (DoesClassInheritFrom(inElementType, cWindow))
	{
		Window*	window	= Window::GetWindowOfClassByName(inElementType, inName);
		
		if (window != NULL)
			obj = window->GetAEObjectPtr();
	}
	else if (DoesClassInheritFrom(inElementType, cDocument))
	{
		obj = mDocumentPolicy.GetDocumentByName(inElementType, inName);
	}
	else
	{
		obj = AEObject::GetElementByName(inElementType, inName);
	}
	
	return (obj);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> AEObjectPtr
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::GetElementByUniqueID(
	DescType		inElementType, 
	SInt32			inUniqueID) const
{
	AEObjectPtr	obj;
	
	if (DoesClassInheritFrom(inElementType, cWindow))
	{
		Window*	window	= Window::GetWindowOfClassByUniqueID(inElementType, inUniqueID);
		
		if (window != NULL)
			obj = window->GetAEObjectPtr();
	}
	else if (DoesClassInheritFrom(inElementType, cDocument))
	{
		obj = mDocumentPolicy.GetDocumentByUniqueID(inElementType, inUniqueID);
	}
	else
	{
		obj = AEObject::GetElementByUniqueID(inElementType, inUniqueID);
	}
	
	return (obj);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> AEObjectPtr
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::CreateObject(
	DescType		inObjectClass,
	DescType		/* inPosition */,
	AEObjectPtr		/* inTarget */, 
	const AERecord&	inProperties,
	const AEDesc&	inData)
{
	return (mDocumentPolicy.OpenNewDocument(inObjectClass, inProperties, inData));
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::ActivateObject()
{
	// "Activating" an application object means bringing that application 
	// "to the front".
	
	OSStatus	err;
	
	err = ::SetFrontProcess(&mPSN);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::WriteProperty(
	DescType		inPropertyID, 
	AEWriter&		ioWriter) const
{
	switch (inPropertyID)
	{
	case pBestType:
		ioWriter.template Write<typeType>(GetClassID());
		break;
		
	case pName:
		ioWriter.template Write<typeUTF16ExternalRepresentation>(GetName());
		break;
		
	case pVersion:
		ioWriter.template Write<typeUTF16ExternalRepresentation>(mBundle.ShortVersionString());
		break;
		
	case pIsFrontProcess:
		ioWriter.template Write<typeBoolean>(MoreProcIsProcessAtFront(NULL));
		break;
		
	default:
		AEObject::WriteProperty(inPropertyID, ioWriter);
		break;
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_POLICY, class UNDO_POLICY, class PRINT_POLICY> void
Application<DOC_POLICY, UNDO_POLICY, PRINT_POLICY>::MakeSpecifier(
	AEWriter&		ioWriter) const
{
	AEDesc	nullDescriptor;
	
	AEInitializeDescInline(&nullDescriptor);
	
	ioWriter.WriteDesc(nullDescriptor);
}

}	// namespace B
