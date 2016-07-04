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

// file header
#include "BEvent.h"

// standard headers
#include <sstream>

// B headers
#include "BAbstractDocument.h"
#include "BEventParams.h"
#include "BExceptionStreamer.h"
#include "BWindowUtils.h"


namespace B {

// ==========================================================================================
//	EventBase

#pragma mark * EventBase *

// ------------------------------------------------------------------------------------------
/*!	Use this constructor to create a Carbon %Event prior to sending or posting it.
*/
EventBase::EventBase(
	UInt32				inClassID,	//!< The Carbon %Event's class ID.
	UInt32				inKind)		//!< The Carbon %Event's kind.
		: mHandlerCallRef(NULL), mEvent(NULL)
{
	OSStatus	err;
	
	err = CreateEvent(NULL, inClassID, inKind, 
					  GetCurrentEventTime(), 
					  kEventAttributeNone, 
					  &mEvent);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this constructor when handling incoming Carbon %Events.
*/
EventBase::EventBase(
	EventHandlerCallRef	inHandlerCallRef,	//!< The caller ref representing a location in the chain of event handlers.
	EventRef			inEvent)			//!< The Carbon %Event.  It gets retained by the constructor.
		: mHandlerCallRef(inHandlerCallRef), mEvent(inEvent)
{
	RetainEvent(inEvent);
}

// ------------------------------------------------------------------------------------------
/*!	Releases the embedded EventRef.
*/
EventBase::~EventBase()
{
	ReleaseEvent(mEvent);
}

// ------------------------------------------------------------------------------------------
/*!	There are a number of overloads of this function.  They only differ in the type 
	of the argument, which always implicitly refers to an event target.
	
	@a inOptions may be any value acceptable to @c SendEventToEventTargetWithOptions().
	Currently these are 0 (default propagation rule), @c kEventTargetDontPropagate and 
	@c kEventTargetSendToAllHandlers.
*/
void
EventBase::Send(
	EventTargetRef	inTarget,				//!< The event target to which the event is initially sent.
	OptionBits		inOptions /* = 0 */)	//!< The options to use when sending.
{
	OSStatus	err;
	
	err = SendEventToEventTargetWithOptions(mEvent, inTarget, inOptions);
	
	if (err != noErr)
	{
		RethrowExceptionFromCarbonEvent(mEvent, err);
	}
	
	Retrieve();
}

// ------------------------------------------------------------------------------------------
/*!	There are a number of overloads of this function.  They only differ in the type 
	of the first argument, which always implicitly refers to an event target.
	
	@a inOptions may be any value acceptable to @c SendEventToEventTargetWithOptions().
	Currently these are 0 (default propagation rule), @c kEventTargetDontPropagate and 
	@c kEventTargetSendToAllHandlers.
	
	@return		An OS status code.
	@exception	none
*/
OSStatus
EventBase::Send(
	EventTargetRef	inTarget,				//!< The event target to which the event is initially sent.
	const std::nothrow_t&,					//!< Indication that the caller doesn't want to have the function throw an exception.
	OptionBits		inOptions /* = 0 */)	//!< The options to use when sending.
{
	OSStatus	err;
	
	err = SendEventToEventTargetWithOptions(mEvent, inTarget, inOptions);
	
	if (err == noErr)
	{
		Retrieve();
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
EventBase::Send(
	HIObjectRef		inObject,
	OptionBits		inOptions /* = 0 */)
{
	Send(HIObjectGetEventTarget(inObject), inOptions);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::Send(
	HIObjectRef				inObject,
	const std::nothrow_t&	nt,
	OptionBits				inOptions /* = 0 */)
{
	return (Send(HIObjectGetEventTarget(inObject), nt, inOptions));
}

// ------------------------------------------------------------------------------------------
void
EventBase::Send(
	WindowRef		inWindow,
	OptionBits		inOptions /* = 0 */)
{
	Send(GetWindowEventTarget(inWindow), inOptions);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::Send(
	WindowRef				inWindow,
	const std::nothrow_t&	nt,
	OptionBits				inOptions /* = 0 */)
{
	return (Send(GetWindowEventTarget(inWindow), nt, inOptions));
}

// ------------------------------------------------------------------------------------------
void
EventBase::Send(
	HIViewRef		inView,
	OptionBits		inOptions /* = 0 */)
{
	Send(GetControlEventTarget(inView), inOptions);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::Send(
	HIViewRef				inView,
	const std::nothrow_t&	nt,
	OptionBits				inOptions /* = 0 */)
{
	return (Send(GetControlEventTarget(inView), nt, inOptions));
}

// ------------------------------------------------------------------------------------------
void
EventBase::Send(
	MenuRef			inMenu,
	OptionBits		inOptions /* = 0 */)
{
	Send(GetMenuEventTarget(inMenu), inOptions);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::Send(
	MenuRef					inMenu,
	const std::nothrow_t&	nt,
	OptionBits				inOptions /* = 0 */)
{
	return (Send(GetMenuEventTarget(inMenu), nt, inOptions));
}

// ------------------------------------------------------------------------------------------
void
EventBase::SendToApp(
	OptionBits		inOptions /* = 0 */)
{
	Send(GetApplicationEventTarget(), inOptions);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::SendToApp(
	const std::nothrow_t&	nt,
	OptionBits				inOptions /* = 0 */)
{
	return (Send(GetApplicationEventTarget(), nt, inOptions));
}

// ------------------------------------------------------------------------------------------
void
EventBase::SendToUserFocus(
	OptionBits		inOptions /* = 0 */)
{
	Send(GetUserFocusEventTarget(), inOptions);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::SendToUserFocus(
	const std::nothrow_t&	nt,
	OptionBits				inOptions /* = 0 */)
{
	return (Send(GetUserFocusEventTarget(), nt, inOptions));
}

// ------------------------------------------------------------------------------------------
/*!	The event will only be dispatched once the event loop is run.
*/
void
EventBase::Post(
	EventPriority inPriority /* = kEventPriorityStandard */)	//!< The event's priority in the event queue.
{
	OSStatus	err;
	
	err = PostEventToQueue(GetMainEventQueue(), mEvent, inPriority);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	@return	A boolean indication of whether an event handler further up the chain handled the event.
*/
bool
EventBase::Forward()
{
	bool		good;
	OSStatus	err;
	
	B_ASSERT(IsIncoming());
	
	err = CallNextEventHandler(mHandlerCallRef, mEvent);
	
	if ((err != noErr) && (err != eventNotHandledErr))
		B_THROW_STATUS(err);
	
	good = (err == noErr);
	
	if (good)
	{
		Retrieve();
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
/*!	@return		An OS status code.
	@exception	none
*/
OSStatus
EventBase::Forward(
	const std::nothrow_t&)	//!< Indication that the caller doesn't want to have the function throw an exception.
{
	OSStatus	err;
	
	B_ASSERT(IsIncoming());
	
	err = CallNextEventHandler(mHandlerCallRef, mEvent);
	
	if (err == noErr)
	{
		Retrieve();
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
/*!	This function is called after a Carbon %Event has been handled, but prior to returning 
	control to the toolbox.
	
	Derived classes should override this if they need to return output parameters to the 
	caller.  The idea is that the outputs are cached as member variables of the derived 
	class, and written to the Carbon %Event via @c SetEventParameter() from within this 
	function.
*/
void
EventBase::Update()
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
/*!	This function is called after a Carbon %Event has been sent, but prior to returning 
	control to the sender.
	
	Derived classes should override this if they need to return output parameters to the 
	caller.  The idea is that the outputs from the Carbon %Event are obtained via 
	@c GetEventParameter() and stored as member variables of the derived class from within 
	this function.
*/
void
EventBase::Retrieve()
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
OSStatus
EventBase::StoreExceptionIntoCarbonEvent(
	EventRef				inEvent, 
	const std::exception&	ex)
{
	OSStatus	err;
	
	err = ErrorHandler::GetStatus(ex, eventInternalErr);
	
	try
	{
		std::ostringstream	ostr;
		
		ExceptionStreamer::Get()->Externalize(ex, ostr);
		
		std::string	blob(ostr.str());
		
		// ignore any errors -- after all, we're already handling an exception.
		
		SetEventParameter(inEvent, keyBExceptionState, typeBExceptionState, 
						  blob.size(), blob.data());
	}
	catch (...)
	{
		// Just prevent exceptions from propagating.
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
EventBase::RethrowExceptionFromCarbonEvent(
	EventRef	inEvent, 
	OSStatus	inStatus)
{
	UInt32		size;
	OSStatus	err;
	
	err = GetEventParameter(inEvent, keyBExceptionState, typeBExceptionState, NULL, 
							0, &size, NULL);
	
	if (err == noErr)
	{
		std::vector<char>	buffer(size);
		
		err = GetEventParameter(inEvent, keyBExceptionState, typeBExceptionState, NULL, 
								buffer.size(), NULL, &buffer[0]);
		B_THROW_IF_STATUS(err);
		
		std::string			str(&buffer[0], buffer.size());
		std::istringstream	istr(str);
		
		// If all goes well, this will throw an exception.
		ExceptionStreamer::Get()->Rethrow(istr);
		
		// If we're here, then something went wrong.  Fall through to the fall-back 
		// behaviour.
	}
	
	ErrorHandler::Get()->ThrowStatusException(inStatus, String());
}


#ifndef DOXYGEN_SKIP

#pragma mark -
#pragma mark * kEventClassMouse *

// ==========================================================================================
//	Event<kEventClassMouse, kEventMouseDown>

// ------------------------------------------------------------------------------------------
Event<kEventClassMouse, kEventMouseDown>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mWindowRef(NULL)
{
	mGlobalMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
	mMouseLocation			= mGlobalMouseLocation;
	mKeyModifiers			= EventParam<kEventParamKeyModifiers>::Get(*this);
	mMouseButton			= EventParam<kEventParamMouseButton>::Get(*this);
	mClickCount				= EventParam<kEventParamClickCount>::Get(*this);
	mMouseChord				= EventParam<kEventParamMouseChord>::Get(*this);
	
	if (EventParam<kEventParamWindowRef>::Get(*this, mWindowRef, std::nothrow))
	{
		mMouseLocation = EventParam<kEventParamWindowMouseLocation>::Get(*this);
	}
}

// ==========================================================================================
//	Event<kEventClassMouse, kEventMouseUp>

// ------------------------------------------------------------------------------------------
Event<kEventClassMouse, kEventMouseUp>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mWindowRef(NULL)
{
	mGlobalMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
	mMouseLocation			= mGlobalMouseLocation;
	mKeyModifiers			= EventParam<kEventParamKeyModifiers>::Get(*this);
	mMouseButton			= EventParam<kEventParamMouseButton>::Get(*this);
	mClickCount				= EventParam<kEventParamClickCount>::Get(*this);
	mMouseChord				= EventParam<kEventParamMouseChord>::Get(*this);
	
	if (EventParam<kEventParamWindowRef>::Get(*this, mWindowRef, std::nothrow))
	{
		mMouseLocation = EventParam<kEventParamWindowMouseLocation>::Get(*this);
	}
}

// ==========================================================================================
//	Event<kEventClassMouse, kEventMouseMoved>

// ------------------------------------------------------------------------------------------
Event<kEventClassMouse, kEventMouseMoved>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mWindowRef(NULL)
{
	mGlobalMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
	mMouseLocation			= mGlobalMouseLocation;
	mKeyModifiers			= EventParam<kEventParamKeyModifiers>::Get(*this);
	mMouseDelta				= EventParam<kEventParamMouseDelta>::Get(*this);
	
	// Unlike the kEventMouseDown and kEventMouseUp events, kEventMouseMoved apparently 
	// didn't get the kEventParamWindowRef and kEventParamWindowMouseLocation parameters 
	// until 10.3.  So if they aren't present, compute them ourselves.
	
	if (EventParam<kEventParamWindowRef>::Get(*this, mWindowRef, std::nothrow))
	{
		// We got 'em.
		mMouseLocation = EventParam<kEventParamWindowMouseLocation>::Get(*this);
	}
	else
	{
		// Need to do some work.
		
		::Point		qdPt	= mGlobalMouseLocation;
		OSStatus	err;
		
		err = FindWindowOfClass(&qdPt, kAllWindowClasses, &mWindowRef, NULL);
		
		if ((err == noErr) && (mWindowRef != NULL))
		{
			mGlobalMouseLocation = B::ViewUtils::ConvertFromGlobal(mGlobalMouseLocation, 
																   mWindowRef);
		}
	}
}

// ==========================================================================================
//	Event<kEventClassMouse, kEventMouseWheelMoved>

// ------------------------------------------------------------------------------------------
Event<kEventClassMouse, kEventMouseWheelMoved>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mGlobalMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
	mWindowRef				= EventParam<kEventParamWindowRef>::Get(*this);
	mMouseLocation			= EventParam<kEventParamWindowMouseLocation>::Get(*this);
	mKeyModifiers			= EventParam<kEventParamKeyModifiers>::Get(*this);
	mMouseWheelAxis			= EventParam<kEventParamMouseWheelAxis>::Get(*this);
	mMouseWheelDelta		= EventParam<kEventParamMouseWheelDelta>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassTextInput *

// ==========================================================================================
//	Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent>

// ------------------------------------------------------------------------------------------
Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mText(mSmallBuffer), mBigBuffer(NULL)
{
	ByteCount	size;
	OSStatus	err;
	
	mKeyboardEvent = EventParam<kEventParamTextInputSendKeyboardEvent>::Get(*this);
	
	// If the text is smaller than or equal to our built-in buffer, read it into that 
	// buffer;  else, allocate a buffer of the correct size.
	
	err = GetEventParameter(*this, kEventParamTextInputSendText, typeUnicodeText, 
							NULL, 0, &size, NULL);
	B_THROW_IF_STATUS(err);
	
	mLength = size / sizeof(UniChar);
	
	if (size > sizeof(mSmallBuffer))
	{
		mBigBuffer = new UniChar [mLength];
		mText = mBigBuffer;
	}
	
	err = GetEventParameter(*this, kEventParamTextInputSendText, typeUnicodeText, 
							NULL, mLength * sizeof(UniChar), NULL, 
							const_cast<UniChar*>(mText));
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent>::~Event()
{
	delete [] mBigBuffer;
}


//// ==========================================================================================
////	Event<kEventClassTextInput, kEventTextInputFilterText>
//
//// ------------------------------------------------------------------------------------------
////Event<kEventClassTextInput, kEventTextInputFilterText>::Event(
//	EventHandlerCallRef	inHandlerCallRef, 
//	EventRef			inEvent)
//		: EventBase(inHandlerCallRef, inEvent)
//{
//	ByteCount				size;
//	std::vector<UniChar>	buffer;
//	OSStatus				err;
//	
//	// Resize the text array so it's big enough to accomodate the chars in the event.
//	
//	err = GetEventParameter(*this, kEventParamTextInputSendText, typeUnicodeText, 
//							NULL, 0, &size, NULL);
//	B_THROW_IF_STATUS(err);
//	
//	buffer.resize(size / sizeof(UniChar));
//	
//	err = GetEventParameter(*this, kEventParamTextInputSendText, typeUnicodeText, 
//							NULL, buffer.size() * sizeof(UniChar), NULL, &buffer[0]);
//	B_THROW_IF_STATUS(err);
//	
//	mText.assign(&buffer[0], buffer.size());
//}
//
//// ------------------------------------------------------------------------------------------
//void
//Event<kEventClassTextInput, kEventTextInputFilterText>::Update()
//{
//	std::vector<UniChar>	buffer;
//	OSStatus				err;
//	
//	buffer.resize(mText.size());
//	mText.copy(&buffer[0], buffer.size());
//	
//	err = SetEventParameter(*this, kEventParamTextInputReplyText, typeUnicodeText, 
//							buffer.size() * sizeof(UniChar), &buffer[0]);
//	B_THROW_IF_STATUS(err);
//}

#pragma mark -
#pragma mark * kEventClassKeyboard *

// ==========================================================================================
//	Event<kEventClassKeyboard, kEventRawKeyModifiersChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassKeyboard, kEventRawKeyModifiersChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mKeyModifiers = EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassKeyboard, kEventHotKeyPressed>

// ------------------------------------------------------------------------------------------
Event<kEventClassKeyboard, kEventHotKeyPressed>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mHotKeyID = EventDirectParam<typeEventHotKeyID>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassKeyboard, kEventHotKeyReleased>

// ------------------------------------------------------------------------------------------
Event<kEventClassKeyboard, kEventHotKeyReleased>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mHotKeyID = EventDirectParam<typeEventHotKeyID>::Get(*this);
}


#pragma mark -
#pragma mark * kEventClassApplication *

// ==========================================================================================
//	Event<kEventClassApplication, kEventAppActivated>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppActivated>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mWindowRef(NULL)
{
	EventParam<kEventParamWindowRef>::Get(*this, mWindowRef, std::nothrow);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppLaunchNotification>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppLaunchNotification>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mProcessID		= EventParam<kEventParamProcessID>::Get(*this);
	mLaunchRefCon	= EventParam<kEventParamLaunchRefCon>::Get(*this);
	mLaunchErr		= EventParam<kEventParamLaunchErr>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppLaunched>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppLaunched>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mProcessID		= EventParam<kEventParamProcessID>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppTerminated>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppTerminated>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mProcessID		= EventParam<kEventParamProcessID>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppFrontSwitched>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppFrontSwitched>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mProcessID		= EventParam<kEventParamProcessID>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppFocusMenuBar>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppFocusMenuBar>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mKeyModifiers = EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppFocusNextDocumentWindow>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppFocusNextDocumentWindow>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mKeyModifiers = EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppFocusNextFloatingWindow>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppFocusNextFloatingWindow>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mKeyModifiers = EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppFocusToolbar>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppFocusToolbar>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mKeyModifiers = EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppGetDockTileMenu>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppGetDockTileMenu>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mMenuRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassApplication, kEventAppGetDockTileMenu>::Update()
{
	if (mMenuRef != NULL)
	{
		EventParam<kEventParamMenuRef>::Set(*this, mMenuRef);
	}
}


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppSystemUIModeChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppSystemUIModeChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mSystemUIMode = EventParam<kEventParamSystemUIMode>::Get(*this);
}


#if B_BUILDING_CAN_USE_10_3_APIS

// ==========================================================================================
//	Event<kEventClassApplication, kEventAppActiveWindowChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppActiveWindowChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mPreviousWindow(NULL), mCurrentWindow(NULL)
{
	EventParam<kEventParamPreviousWindow>::Get(*this, mPreviousWindow, std::nothrow);
	EventParam<kEventParamCurrentWindow>::Get(*this, mCurrentWindow, std::nothrow);
}

#endif  // B_BUILDING_CAN_USE_10_3_APIS


// ==========================================================================================
//	Event<kEventClassApplication, kEventAppIsEventInInstantMouser>

// ------------------------------------------------------------------------------------------
Event<kEventClassApplication, kEventAppIsEventInInstantMouser>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mInInstantMouser(false)
{
	mEvent = EventParam<kEventParamEventRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassApplication, kEventAppIsEventInInstantMouser>::Update()
{
	EventParam<kEventParamIsInInstantMouser>::Set(*this, mInInstantMouser);
}

#pragma mark -
#pragma mark * kEventClassMenu *

// ==========================================================================================
//	Event<kEventClassMenu, kEventMenuDispose>

// ------------------------------------------------------------------------------------------
Event<kEventClassMenu, kEventMenuDispose>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mMenuRef	= EventDirectParam<typeMenuRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassMenu, kEventMenuEnableItems>

// ------------------------------------------------------------------------------------------
Event<kEventClassMenu, kEventMenuEnableItems>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mMenuRef		= EventDirectParam<typeMenuRef>::Get(*this);
	mForKeyEvent	= EventParam<kEventParamEnableMenuForKeyEvent>::Get(*this);
	mContext		= EventParam<kEventParamMenuContext>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassMenu, kEventMenuPopulate>

// ------------------------------------------------------------------------------------------
Event<kEventClassMenu, kEventMenuPopulate>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mMenuRef	= EventDirectParam<typeMenuRef>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassWindow *

// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowUpdate>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowUpdate>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowUpdate), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowUpdate>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowDrawContent>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDrawContent>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowDrawContent), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDrawContent>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowActivated>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowActivated>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowActivated), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowActivated>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowDeactivated>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDeactivated>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowDeactivated), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDeactivated>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowShowing>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowShowing>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowShowing), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowShowing>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowHiding>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowHiding>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowHiding), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowHiding>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowShown>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowShown>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowShown), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowShown>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowHidden>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowHidden>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowHidden), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowHidden>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowBoundsChanging>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowBoundsChanging>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
	mAttributes		= EventParam<kEventParamAttributes>::Get(*this);
	mOriginalBounds	= EventParam<kEventParamOriginalBounds>::Get(*this);
	mPreviousBounds	= EventParam<kEventParamPreviousBounds>::Get(*this);
	mCurrentBounds	= EventParam<kEventParamCurrentBounds>::Get(*this);
	mNewBounds		= mCurrentBounds;
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowBoundsChanging>::Update()
{
	if (mCurrentBounds != mNewBounds)
	{
		EventParam<kEventParamCurrentBounds>::Set(*this, mNewBounds);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowBoundsChanging>::Retrieve()
{
	mCurrentBounds	= EventParam<kEventParamCurrentBounds>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowBoundsChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowBoundsChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
	mAttributes		= EventParam<kEventParamAttributes>::Get(*this);
	mOriginalBounds	= EventParam<kEventParamOriginalBounds>::Get(*this);
	mPreviousBounds	= EventParam<kEventParamPreviousBounds>::Get(*this);
	mCurrentBounds	= EventParam<kEventParamCurrentBounds>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowResizeStarted>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowResizeStarted>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowResizeCompleted>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowResizeCompleted>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowTransitionCompleted>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowTransitionCompleted>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
	mUserData		= EventParam<kEventParamUserData>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowClose>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowClose>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowClose), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowClose>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowClosed>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowClosed>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowGetIdealSize>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowGetIdealSize>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowGetIdealSize), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowGetIdealSize>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mDimensions(CGSizeZero)
{
	mWindowRef = EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowGetIdealSize>::Update()
{
	Point	pt(mDimensions.width, mDimensions.height);
	
	EventParam<kEventParamDimensions>::Set(*this, pt);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowGetIdealSize>::Retrieve()
{
	mDimensions = EventParam<kEventParamDimensions>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowGetMinimumSize>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowGetMinimumSize>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowGetMinimumSize), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowGetMinimumSize>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mDimensions(CGSizeZero)
{
	mWindowRef = EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowGetMinimumSize>::Update()
{
	Point	pt(mDimensions.width, mDimensions.height);
	
	EventParam<kEventParamDimensions>::Set(*this, pt);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowGetMinimumSize>::Retrieve()
{
	mDimensions = EventParam<kEventParamDimensions>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowGetMaximumSize>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowGetMaximumSize>::Event(
	WindowRef			inWindowRef)
		: EventBase(kEventClassWindow, kEventWindowGetMaximumSize), 
		  mWindowRef(inWindowRef)
{
	EventDirectParam<typeWindowRef>::Set(*this, mWindowRef);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowGetMaximumSize>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mDimensions(CGSizeZero)
{
	mWindowRef = EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowGetMaximumSize>::Update()
{
	Point	pt(mDimensions.width, mDimensions.height);
	
	EventParam<kEventParamDimensions>::Set(*this, pt);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowGetMaximumSize>::Retrieve()
{
	mDimensions = EventParam<kEventParamDimensions>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowToolbarSwitchMode>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowToolbarSwitchMode>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowFocusAcquired>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowFocusAcquired>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowFocusRelinquish>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowFocusRelinquish>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowSheetOpening>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowSheetOpening>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mCancel(false)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowSheetOpening>::Update()
{
	if (mCancel)
	{
		AutoThrowHandler	dontShowAlert;
		
		// Returning userCanceledErr causes the drawer to not open.
		B_THROW(RuntimeOSStatusException(userCanceledErr));
	}
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowSheetOpened>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowSheetOpened>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowSheetClosing>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowSheetClosing>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mCancel(false)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowSheetClosing>::Update()
{
	if (mCancel)
	{
		AutoThrowHandler	dontShowAlert;
		
		// Returning userCanceledErr causes the drawer to not close.
		B_THROW(RuntimeOSStatusException(userCanceledErr));
	}
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowSheetClosed>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowSheetClosed>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowDrawerOpening>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDrawerOpening>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mCancel(false)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowDrawerOpening>::Update()
{
	if (mCancel)
	{
		AutoThrowHandler	dontShowAlert;
		
		// Returning userCanceledErr causes the drawer to not open.
		B_THROW(RuntimeOSStatusException(userCanceledErr));
	}
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowDrawerOpened>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDrawerOpened>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowDrawerClosing>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDrawerClosing>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mCancel(false)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassWindow, kEventWindowDrawerClosing>::Update()
{
	if (mCancel)
	{
		AutoThrowHandler	dontShowAlert;
		
		// Returning userCanceledErr causes the drawer to not close.
		B_THROW(RuntimeOSStatusException(userCanceledErr));
	}
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowDrawerClosed>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowDrawerClosed>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassWindow, kEventWindowPaint>

// ------------------------------------------------------------------------------------------
Event<kEventClassWindow, kEventWindowPaint>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindowRef		= EventDirectParam<typeWindowRef>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassCommand *

// ==========================================================================================
//	Event<kEventClassCommand, kEventCommandProcess>

// ------------------------------------------------------------------------------------------
Event<kEventClassCommand, kEventCommandProcess>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mMenuContext(0), mKeyModifiers(0)
{
	mHICommand = EventDirectParam<typeHICommand>::Get(*this);
	
	EventParam<kEventParamMenuContext>::Get(*this, mMenuContext, std::nothrow);
	EventParam<kEventParamKeyModifiers>::Get(*this, mKeyModifiers, std::nothrow);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassCommand, kEventCommandProcess>::Event(
	const HICommandExtended&	inCommand, 
	UInt32						inKeyModifiers, 
	UInt32						inMenuContext /* = 0 */)
		: EventBase(kEventClassCommand, kEventCommandProcess), 
		  mHICommand(inCommand), mMenuContext(inMenuContext), 
		  mKeyModifiers(inKeyModifiers)
{
	EventDirectParam<typeHICommand>::Set(*this, mHICommand);
	EventParam<kEventParamKeyModifiers>::Set(*this, mKeyModifiers);
	
	if (mMenuContext != 0)
		EventParam<kEventParamMenuContext>::Set(*this, mMenuContext);
}


// ==========================================================================================
//	Event<kEventClassCommand, kEventCommandUpdateStatus>

// ------------------------------------------------------------------------------------------
Event<kEventClassCommand, kEventCommandUpdateStatus>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mHICommand = EventDirectParam<typeHICommand>::Get(*this);
	
	EventParam<kEventParamMenuContext>::Get(*this, mMenuContext, std::nothrow);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassCommand, kEventCommandUpdateStatus>::Event(
	const HICommandExtended&	inCommand, 
	UInt32						inMenuContext /* = 0 */)
		: EventBase(kEventClassCommand, kEventCommandUpdateStatus), 
		  mHICommand(inCommand), mMenuContext(inMenuContext)
{
	EventDirectParam<typeHICommand>::Set(*this, mHICommand);
	
	if (mMenuContext != 0)
		EventParam<kEventParamMenuContext>::Set(*this, mMenuContext);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassCommand, kEventCommandUpdateStatus>::Update()
{
	if (mData.HasData())
	{
		mData.RefreshData(mHICommand);
	}
}

#pragma mark -
#pragma mark * kEventClassControl *

// ==========================================================================================
//	Event<kEventClassControl, kEventControlInitialize>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlInitialize>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mFeatures(0)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mCollection	= EventParam<kEventParamInitCollection>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlInitialize>::Update()
{
	if (mFeatures != 0)
	{
		EventParam<kEventParamControlFeatures>::Set(*this, mFeatures);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlInitialize>::Retrieve()
{
	EventParam<kEventParamControlFeatures>::Get(*this, mFeatures, std::nothrow);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDispose>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDispose>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetOptimalBounds>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetOptimalBounds>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mBaselineOffset(0)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetOptimalBounds>::Update()
{
	EventParam<kEventParamControlOptimalBounds>::Set(*this, mOptimalBounds);
	
	if (mBaselineOffset != 0)
	{
		EventParam<kEventParamControlOptimalBaselineOffset>::Set(*this, mBaselineOffset);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetOptimalBounds>::Retrieve()
{
	mOptimalBounds = EventParam<kEventParamControlOptimalBounds>::Get(*this);
	
	EventParam<kEventParamControlOptimalBaselineOffset>::Get(*this, mBaselineOffset, std::nothrow);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlHit>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlHit>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode		= EventParam<kEventParamControlPart>::Get(*this);
	mKeyModifiers	= EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlSimulateHit>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlSimulateHit>::Event(
	HIViewRef			inViewRef, 
	UInt32				inKeyModifiers, 
	ControlPartCode		inPartCode)
		: EventBase(kEventClassControl, kEventControlSimulateHit), 
		  mViewRef(inViewRef), mKeyModifiers(inKeyModifiers), 
		  mPartCode(inPartCode)
{
	EventDirectParam<typeControlRef>::Set(*this, mViewRef);
	EventParam<kEventParamKeyModifiers>::Set(*this, mKeyModifiers);
	EventParam<kEventParamControlPart>::Set(*this, mPartCode);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlSimulateHit>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mKeyModifiers	= EventParam<kEventParamKeyModifiers>::Get(*this);
	mPartCode		= EventParam<kEventParamControlPart>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlHitTest>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlHitTest>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mPartCode(kHIViewNoPart)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlHitTest>::Update()
{
	EventParam<kEventParamControlPart>::Set(*this, mPartCode);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlHitTest>::Retrieve()
{
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDraw>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDraw>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mPartCode(kHIViewEntireView), 
		  mCGContext(NULL), mDrawShape(NULL)
{
	mViewRef = EventDirectParam<typeControlRef>::Get(*this);
	
	EventParam<kEventParamControlPart>::Get(*this, mPartCode, std::nothrow);
	EventParam<kEventParamCGContextRef>::Get(*this, mCGContext, std::nothrow);
	
    if (EventParam<kEventParamShape>::Get(*this, mDrawShapeObject, std::nothrow))
        mDrawShape = &mDrawShapeObject;
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlSetFocusPart>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlSetFocusPart>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode		= EventParam<kEventParamControlPart>::Get(*this);
	
	EventParam<kEventParamControlFocusEverything>::Get(*this, mFocusEverything, std::nothrow);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlSetFocusPart>::Update()
{
	EventParam<kEventParamControlPart>::Set(*this, mPartCode);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlSetFocusPart>::Retrieve()
{
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetFocusPart>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetFocusPart>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mPartCode(kHIViewNoPart)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetFocusPart>::Update()
{
	EventParam<kEventParamControlPart>::Set(*this, mPartCode);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetFocusPart>::Retrieve()
{
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlActivate>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlActivate>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDeactivate>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDeactivate>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlClick>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlClick>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mMouseLocation	= EventParam<kEventParamWindowMouseLocation>::Get(*this);
	mKeyModifiers	= EventParam<kEventParamKeyModifiers>::Get(*this);
	mMouseButton	= EventParam<kEventParamMouseButton>::Get(*this);
	mClickCount		= EventParam<kEventParamClickCount>::Get(*this);
	mMouseChord		= EventParam<kEventParamMouseChord>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetAutoToggleValue>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetAutoToggleValue>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode		= EventParam<kEventParamControlPart>::Get(*this);
	mNewValue		= ((GetControl32BitValue(mViewRef)) == 1 ? 0 : 1);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetAutoToggleValue>::Update()
{
	EventParam<kEventParamControlValue>::Set(*this, mNewValue);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetAutoToggleValue>::Retrieve()
{
	mNewValue = EventParam<kEventParamControlValue>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetNextFocusCandidate>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetNextFocusCandidate>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mNextView(NULL)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode		= EventParam<kEventParamControlPart>::Get(*this);
	
	if (!EventParam<kEventParamStartControl>::Get(*this, mStartView, std::nothrow))
		mStartView = NULL;
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetNextFocusCandidate>::Update()
{
	if (mNextView != NULL)
	{
		EventParam<kEventParamNextControl>::Set(*this, mNextView);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetNextFocusCandidate>::Retrieve()
{
	if (!EventParam<kEventParamNextControl>::Get(*this, mNextView, std::nothrow))
		mNextView = NULL;
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlInterceptSubviewClick>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlInterceptSubviewClick>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mMouseDownEvent	= EventParam<kEventParamEventRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDragEnter>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDragEnter>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mLikesDrag(false)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mDragRef	= EventParam<kEventParamDragRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlDragEnter>::Update()
{
	if (mLikesDrag)
	{
		EventParam<kEventParamControlWouldAcceptDrop>::Set(*this, mLikesDrag);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlDragEnter>::Retrieve()
{
	mLikesDrag	= EventParam<kEventParamControlWouldAcceptDrop>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDragWithin>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDragWithin>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mDragRef	= EventParam<kEventParamDragRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDragLeave>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDragLeave>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mDragRef	= EventParam<kEventParamDragRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlDragReceive>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlDragReceive>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mDragRef	= EventParam<kEventParamDragRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetClickActivation>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetClickActivation>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mResult(kDoNotActivateAndIgnoreClick)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
	mKeyModifiers	= EventParam<kEventParamKeyModifiers>::Get(*this);
	mMouseButton	= EventParam<kEventParamMouseButton>::Get(*this);
	mClickCount		= EventParam<kEventParamClickCount>::Get(*this);
	mMouseChord		= EventParam<kEventParamMouseChord>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetClickActivation>::Update()
{
	EventParam<kEventParamClickActivation>::Set(*this, mResult);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetClickActivation>::Retrieve()
{
	mResult			= EventParam<kEventParamClickActivation>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlTrack>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlTrack>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mHitPart(kHIViewNoPart)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mMouseLocation	= EventParam<kEventParamMouseLocation>::Get(*this);
	mKeyModifiers	= EventParam<kEventParamKeyModifiers>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlTrack>::Update()
{
	EventParam<kEventParamKeyModifiers>::Set(*this, mKeyModifiers);
	EventParam<kEventParamControlPart>::Set(*this, mHitPart);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlTrack>::Retrieve()
{
	mKeyModifiers	= EventParam<kEventParamKeyModifiers>::Get(*this);
	mHitPart		= EventParam<kEventParamControlPart>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetPartRegion>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetPartRegion>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
    
    if (!EventParam<kEventParamControlPrefersShape>::Get(*this, mPrefersShape, std::nothrow))
        mPrefersShape = false;
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetPartRegion>::Update()
{
    if (mPrefersShape)
    {
        EventParam<kEventParamShape>::Set(*this, mPartShape);
    }
    else
    {
        RgnHandle   rgn = EventParam<kEventParamControlRegion>::Get(*this);
        OSStatus    err;
        
        err = HIShapeGetAsQDRgn(mPartShape.cf_ref(), rgn);
        B_THROW_IF_STATUS(err);
    }
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetPartBounds>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetPartBounds>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	OSStatus	err;
	
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
	
	err = HIViewGetBounds(mViewRef, &mPartBounds);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetPartBounds>::Update()
{
	EventParam<kEventParamControlPartBounds>::Set(*this, mPartBounds);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetPartBounds>::Retrieve()
{
	mPartBounds	= EventParam<kEventParamControlPartBounds>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlSetData>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlSetData>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
	mDataTag	= EventParam<kEventParamControlDataTag>::Get(*this);
	mDataPtr	= EventParam<kEventParamControlDataBuffer>::Get(*this);
	mDataSize	= EventParam<kEventParamControlDataBufferSize>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetData>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetData>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mPartCode	= EventParam<kEventParamControlPart>::Get(*this);
	mDataTag	= EventParam<kEventParamControlDataTag>::Get(*this);
	mDataPtr	= EventParam<kEventParamControlDataBuffer>::Get(*this);
	mDataSize	= EventParam<kEventParamControlDataBufferSize>::Get(*this);
	
	mOriginalDataSize = mDataSize;
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetData>::Update()
{
	if (mDataSize != mOriginalDataSize)
	{
		EventParam<kEventParamControlDataBufferSize>::Set(*this, mDataSize);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetData>::Retrieve()
{
	mDataSize	= EventParam<kEventParamControlDataBufferSize>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlGetSizeConstraints>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlGetSizeConstraints>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mMinimumSize(CGSizeZero), mMaximumSize(CGSizeZero)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetSizeConstraints>::Update()
{
	EventParam<kEventParamMinimumSize>::Set(*this, mMinimumSize);
	EventParam<kEventParamMaximumSize>::Set(*this, mMaximumSize);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassControl, kEventControlGetSizeConstraints>::Retrieve()
{
	mMinimumSize	= EventParam<kEventParamMinimumSize>::Get(*this);
	mMaximumSize	= EventParam<kEventParamMaximumSize>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlValueFieldChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlValueFieldChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlAddedSubControl>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlAddedSubControl>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mSubViewRef	= EventParam<kEventParamControlSubControl>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlRemovingSubControl>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlRemovingSubControl>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mSubViewRef	= EventParam<kEventParamControlSubControl>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlBoundsChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlBoundsChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
	mAttributes		= EventParam<kEventParamAttributes>::Get(*this);
	mOriginalBounds	= EventParam<kEventParamOriginalBounds>::Get(*this);
	mPreviousBounds	= EventParam<kEventParamPreviousBounds>::Get(*this);
	mCurrentBounds	= EventParam<kEventParamCurrentBounds>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlTitleChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlTitleChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlVisibilityChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlVisibilityChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlOwningWindowChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlOwningWindowChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef				= EventDirectParam<typeControlRef>::Get(*this);
	mOriginalOwningWindow	= EventParam<kEventParamControlOriginalOwningWindow>::Get(*this);
	mCurrentOwningWindow	= EventParam<kEventParamControlCurrentOwningWindow>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlHiliteChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlHiliteChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef			= EventDirectParam<typeControlRef>::Get(*this);
	mPreviousPartCode	= EventParam<kEventParamControlPreviousPart>::Get(*this);
	mCurrentPartCode	= EventParam<kEventParamControlCurrentPart>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlEnabledStateChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlEnabledStateChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef		= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlTrackingAreaEntered>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlTrackingAreaEntered>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
    mTrackingAreaRef    = EventParam<kEventParamHIViewTrackingArea>::Get(*this);
	mMouseLocation      = EventParam<kEventParamMouseLocation>::Get(*this);
	mKeyModifiers		= EventParam<kEventParamKeyModifiers>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassControl, kEventControlTrackingAreaExited>

// ------------------------------------------------------------------------------------------
Event<kEventClassControl, kEventControlTrackingAreaExited>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
    mTrackingAreaRef    = EventParam<kEventParamHIViewTrackingArea>::Get(*this);
	mMouseLocation      = EventParam<kEventParamMouseLocation>::Get(*this);
	mKeyModifiers		= EventParam<kEventParamKeyModifiers>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassService *

// ==========================================================================================
//	Event<kEventClassService, kEventServiceCopy>

// ------------------------------------------------------------------------------------------
Event<kEventClassService, kEventServiceCopy>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mPasteboardRef	= EventParam<kEventParamPasteboardRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassService, kEventServicePaste>

// ------------------------------------------------------------------------------------------
Event<kEventClassService, kEventServicePaste>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mPasteboardRef	= EventParam<kEventParamPasteboardRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassService, kEventServiceGetTypes>

// ------------------------------------------------------------------------------------------
Event<kEventClassService, kEventServiceGetTypes>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mCopyTypes(EventParam<kEventParamServiceCopyTypes>::Get(*this), true), 
		  mPasteTypes(EventParam<kEventParamServicePasteTypes>::Get(*this), true)
{
}


// ==========================================================================================
//	Event<kEventClassService, kEventServicePerform>

// ------------------------------------------------------------------------------------------
Event<kEventClassService, kEventServicePerform>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mPasteboardRef	= EventParam<kEventParamPasteboardRef>::Get(*this);
	mMessage		= EventParam<kEventParamServiceMessageName>::Get(*this);
	mUserData		= EventParam<kEventParamServiceUserData>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassToolbar *

// ==========================================================================================
//	Event<kEventClassToolbar, kEventToolbarGetDefaultIdentifiers>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbar, kEventToolbarGetDefaultIdentifiers>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mToolbar(EventParam<kEventParamToolbar>::Get(*this)), 
		  mIdentifiers(EventParam<kEventParamMutableArray>::Get(*this), true)
{
}


// ==========================================================================================
//	Event<kEventClassToolbar, kEventToolbarGetAllowedIdentifiers>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbar, kEventToolbarGetAllowedIdentifiers>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mToolbar(EventParam<kEventParamToolbar>::Get(*this)), 
		  mIdentifiers(EventParam<kEventParamMutableArray>::Get(*this), true)
{
}


// ==========================================================================================
//	Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mToolbar(EventParam<kEventParamToolbar>::Get(*this)), 
		  mItemIdentifier(EventParam<kEventParamToolbarItemIdentifier>::Get(*this))
{
	EventParam<kEventParamToolbarItemConfigData>::Get(*this, mItemConfigData, std::nothrow);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>::Update()
{
	if (mToolbarItem != NULL)
	{
		EventParam<kEventParamToolbarItem>::Set(*this, mToolbarItem);
		mToolbarItem.release();
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>::Retrieve()
{
	EventParam<kEventParamToolbarItem>::Get(*this, mToolbarItem, std::nothrow);
}


// ==========================================================================================
//	Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mDragRef = EventParam<kEventParamDragRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>::Update()
{
	if (mToolbarItem != NULL)
	{
		EventParam<kEventParamToolbarItem>::Set(*this, mToolbarItem);
		mToolbarItem.release();
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>::Retrieve()
{
	EventParam<kEventParamToolbarItem>::Get(*this, mToolbarItem, std::nothrow);
}


// ==========================================================================================
//	Event<kEventClassToolbar, kEventToolbarGetSelectableIdentifiers>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbar, kEventToolbarGetSelectableIdentifiers>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mToolbar(EventParam<kEventParamToolbar>::Get(*this)), 
		  mIdentifiers(EventParam<kEventParamMutableArray>::Get(*this), true)
{
}

#pragma mark -
#pragma mark * kEventClassToolbarItem *

// ==========================================================================================
//	Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>::Update()
{
	if (mConfigData != NULL)
	{
		EventParam<kEventParamToolbarItemConfigData>::Set(*this, mConfigData);
		mConfigData.release();
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>::Retrieve()
{
	EventParam<kEventParamToolbarItemConfigData>::Get(*this, mConfigData, std::nothrow);
}


// ==========================================================================================
//	Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>::Update()
{
	if (mCustomView != NULL)
	{
		EventParam<kEventParamControlRef>::Set(*this, mCustomView);
		mCustomView.release();
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>::Retrieve()
{
	HIViewRef	viewRef	= NULL;
	
	EventParam<kEventParamControlRef>::Get(*this, viewRef, std::nothrow);
	
	if (viewRef != NULL)
		mCustomView.reset(viewRef, from_copy);
}


// ==========================================================================================
//	Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mLikesDrag(false)
{
	mDragRef	= EventParam<kEventParamDragRef>::Get(*this);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>::Update()
{
	if (mLikesDrag)
	{
		EventParamAndType<kEventParamResult, typeBoolean>::Set(*this, mLikesDrag);
	}
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>::Retrieve()
{
	mLikesDrag	= EventParamAndType<kEventParamResult, typeBoolean>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassToolbarItem, kEventToolbarItemAcceptDrop>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbarItem, kEventToolbarItemAcceptDrop>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mDragRef	= EventParam<kEventParamDragRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassToolbarItem, kEventToolbarItemSelectedStateChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassToolbarItem, kEventToolbarItemSelectedStateChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent),
		  mWindowRef(NULL)
{
	EventParam<kEventParamWindowRef>::Get(*this, mWindowRef, std::nothrow);
}

#pragma mark -
#pragma mark * kEventClassScrollable *

// ==========================================================================================
//	Event<kEventClassScrollable, kEventScrollableGetInfo>

// ------------------------------------------------------------------------------------------
Event<kEventClassScrollable, kEventScrollableGetInfo>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassScrollable, kEventScrollableGetInfo>::Update()
{
	EventParam<kEventParamImageSize>::Set(*this, mImageSize);
	EventParam<kEventParamViewSize>::Set(*this, mViewSize);
	EventParam<kEventParamLineSize>::Set(*this, mLineSize);
	EventParam<kEventParamOrigin>::Set(*this, mOrigin);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassScrollable, kEventScrollableGetInfo>::Retrieve()
{
	mImageSize	= EventParam<kEventParamImageSize>::Get(*this);
	mViewSize	= EventParam<kEventParamViewSize>::Get(*this);
	mLineSize	= EventParam<kEventParamLineSize>::Get(*this);
	mOrigin		= EventParam<kEventParamOrigin>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassScrollable, kEventScrollableScrollTo>

// ------------------------------------------------------------------------------------------
Event<kEventClassScrollable, kEventScrollableScrollTo>::Event(
	Point				inOrigin)
		: EventBase(kEventClassScrollable, kEventScrollableScrollTo)
{
	EventParam<kEventParamOrigin>::Set(*this, inOrigin);
}

// ==========================================================================================
//	Event<kEventClassScrollable, kEventScrollableScrollTo>

// ------------------------------------------------------------------------------------------
Event<kEventClassScrollable, kEventScrollableScrollTo>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mOrigin		= EventParam<kEventParamOrigin>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassVolume *

// ==========================================================================================
//	Event<kEventClassVolume, kEventVolumeMounted>

// ------------------------------------------------------------------------------------------
Event<kEventClassVolume, kEventVolumeMounted>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mVolume = EventDirectParam<typeFSVolumeRefNum>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassVolume, kEventVolumeUnmounted>

// ------------------------------------------------------------------------------------------
Event<kEventClassVolume, kEventVolumeUnmounted>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mVolume = EventDirectParam<typeFSVolumeRefNum>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassAppearance *

// ==========================================================================================
//	Event<kEventClassAppearance, kEventAppearanceScrollBarVariantChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassAppearance, kEventAppearanceScrollBarVariantChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mVariant = EventParam<kEventParamNewScrollBarVariant>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassHIComboBox *

// ==========================================================================================
//	Event<kEventClassHIComboBox, kEventComboBoxListItemSelected>

// ------------------------------------------------------------------------------------------
Event<kEventClassHIComboBox, kEventComboBoxListItemSelected>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
	mIndex      = EventParam<kEventParamComboBoxListSelectedItemIndex>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassSearchField *

// ==========================================================================================
//	Event<kEventClassSearchField, kEventSearchFieldCancelClicked>

// ------------------------------------------------------------------------------------------
Event<kEventClassSearchField, kEventSearchFieldCancelClicked>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassSearchField, kEventSearchFieldSearchClicked>

// ------------------------------------------------------------------------------------------
Event<kEventClassSearchField, kEventSearchFieldSearchClicked>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassTextField *

// ==========================================================================================
//	Event<kEventClassTextField, kEventTextAccepted>

// ------------------------------------------------------------------------------------------
Event<kEventClassTextField, kEventTextAccepted>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassTextField, kEventTextShouldChangeInRange>

// ------------------------------------------------------------------------------------------
Event<kEventClassTextField, kEventTextShouldChangeInRange>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mCancel(false)
{
    mSelection		= EventParam<kEventParamTextSelection>::Get(*this);
    mText			= EventParam<kEventParamCandidateText>::Get(*this);
	mOriginalText	= mText.cf_ref();
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassTextField, kEventTextShouldChangeInRange>::Update()
{
	if (mCancel)
		throw UserCanceledException();
	
	if ((mText.cf_ref() != mOriginalText) && (mText.size() > 0))
        EventParam<kEventParamReplacementText>::Set(*this, mText);
}

// ------------------------------------------------------------------------------------------
void
Event<kEventClassTextField, kEventTextShouldChangeInRange>::Retrieve()
{
	if (!EventParam<kEventParamReplacementText>::Get(*this, mText, std::nothrow))
		mText.clear();
}


// ==========================================================================================
//	Event<kEventClassTextField, kEventTextDidChange>

// ------------------------------------------------------------------------------------------
Event<kEventClassTextField, kEventTextDidChange>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
    CFRange dummy;
    
    mInlineEditing  = EventParam<kEventParamUnconfirmedRange>::Get(*this, dummy, std::nothrow);
}

#pragma mark -
#pragma mark * kEventClassClockView *

// ==========================================================================================
//	Event<kEventClassClockView, kEventClockDateOrTimeChanged>

// ------------------------------------------------------------------------------------------
Event<kEventClassClockView, kEventClockDateOrTimeChanged>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mViewRef	= EventDirectParam<typeControlRef>::Get(*this);
}

#pragma mark -
#pragma mark * kEventClassFont *

// ==========================================================================================
//	Event<kEventClassFont, kEventFontPanelClosed>

// ------------------------------------------------------------------------------------------
Event<kEventClassFont, kEventFontPanelClosed>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
}


// ==========================================================================================
//	Event<kEventClassFont, kEventFontSelection>

// ------------------------------------------------------------------------------------------
Event<kEventClassFont, kEventFontSelection>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent), 
		  mFontIDPtr(NULL), mFontSizePtr(NULL)
{
	if (EventParam<kEventParamATSUFontID>::Get(*this, mFontID, std::nothrow))
		mFontIDPtr = &mFontID;
	
	if (EventParam<kEventParamATSUFontSize>::Get(*this, mFontSize, std::nothrow))
		mFontSizePtr = &mFontSize;
	
	EventParam<kEventParamDictionary>::Get(*this, mDictionary, std::nothrow);
}

#pragma mark -
#pragma mark * kEventClassB *

// ==========================================================================================
//	Event<kEventClassB, kEventBCloseDocument>

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBCloseDocument>::Event(
	EventTargetRef		inDocumentTarget)
		: EventBase(kEventClassB, kEventBCloseDocument)
{
	EventParam<kEventParamPostTarget>::Set(*this, inDocumentTarget);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBCloseDocument>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
}


// ==========================================================================================
//	Event<kEventClassB, kEventBSaveDocument>

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBSaveDocument>::Event(
	EventTargetRef		inDocumentTarget, 
	WindowRef			inWindow,
	bool				inFromUser)
		: EventBase(kEventClassB, kEventBSaveDocument), 
		  mWindow(inWindow), mFromUser(inFromUser)
{
	EventParam<kEventParamPostTarget>::Set(*this, inDocumentTarget);
	EventParam<kEventParamWindowRef>::Set(*this, inWindow);
	EventParam<kEventParamBFromUser>::Set(*this, inFromUser);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBSaveDocument>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mWindow		= EventParam<kEventParamWindowRef>::Get(*this);
	mFromUser	= EventParam<kEventParamBFromUser>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassB, kEventBUndoBegin>

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoBegin>::Event()
	: EventBase(kEventClassB, kEventBUndoBegin)
{
}

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoBegin>::Event(
	const String&		inName)
		: EventBase(kEventClassB, kEventBUndoBegin), 
		  mName(inName)
{
	EventParam<kEventParamBUndoName>::Set(*this, mName);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoBegin>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	if (!EventParam<kEventParamBUndoName>::Get(*this, mName, std::nothrow))
		mName.clear();
}


// ==========================================================================================
//	Event<kEventClassB, kEventBUndoAdd>

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoAdd>::Event(
	UndoAction*			inUndoAction)
		: EventBase(kEventClassB, kEventBUndoAdd), 
		  mUndoAction(inUndoAction)
{
	EventParam<kEventParamBUndoAction>::Set(*this, mUndoAction);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoAdd>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mUndoAction	= EventParam<kEventParamBUndoAction>::Get(*this);
}


// ==========================================================================================
//	Event<kEventClassB, kEventBUndoEnable>

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoEnable>::Event(
	bool				inEnable)
		: EventBase(kEventClassB, kEventBUndoEnable), 
		  mEnable(inEnable)
{
	EventParam<kEventParamBUndoEnable>::Set(*this, mEnable);
}

// ------------------------------------------------------------------------------------------
Event<kEventClassB, kEventBUndoEnable>::Event(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
		: EventBase(inHandlerCallRef, inEvent)
{
	mEnable	= EventParam<kEventParamBUndoEnable>::Get(*this);
}

#endif	// DOXYGEN_SKIP

}	// namespace B
