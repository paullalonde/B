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

#ifndef BEvent_H_
#define BEvent_H_

#pragma once

// standard headers
#include <new>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// project headers
#include "BCommandData.h"
#include "BEventCustomParams.h"
#include "BMutableArray.h"
#include "BMutableShape.h"
#include "BRect.h"
#include "BShape.h"
#include "BString.h"


namespace B {

// ==========================================================================================
//  EventBase

/*!
    @brief  Common base class for representing Carbon %Events.
    
    This class contains member functions and variables that are independent of the 
    specifics of a particular event.  Its main purpose is to serve as a common base 
    class for the various template instantiations of Event.  It is rarely if ever 
    used by itself.
    
    EventBase can be instantiated as either an incoming or outgoing event.  Incoming 
    events are those given to a Carbon %Event handler;  they have an associated 
    @c EventHandlerCallRef.  They rarely, if ever, need to be instantiated directly 
    because the EventHandler machinery takes care of it.  Outgoing events are 
    instantiated by the developer for sending elsewhere.
    
    @ingroup    CarbonEvents
    @sa         @ref using_events
*/
class EventBase : public boost::noncopyable
{
public:
    
    //! @name Constructors & Destructor
    //@{
    //! Creates a new Carbon %Event.
    EventBase(
        UInt32              inClassID, 
        UInt32              inKind);
    //! Creates a object wrapping an existing Carbon %Event.
    EventBase(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    //! Destructor.  Releases the embedded EventRef.
    virtual ~EventBase();
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns @c true if this is an incoming event.
    bool    IsIncoming() const;
    //@}
    
    //! @name Conversions
    //@{
    //! Returns the @c EventRef embedded within the object.
    operator EventRef ();
    //@}
    
    //! @name Sending
    //@{
    
    //! Sends the event to an event target.
    void        Send(
                    EventTargetRef  inTarget,
                    OptionBits      inOptions = 0);
    //! Sends the event to an event target without throwing an exception.
    OSStatus    Send(
                    EventTargetRef  inTarget,  
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    
    /*! @overload
    */
    void        Send(
                    HIObjectRef     inObject,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    OSStatus    Send(
                    HIObjectRef     inObject,
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    void        Send(
                    WindowRef       inWindow,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    OSStatus    Send(
                    WindowRef       inWindow,
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    void        Send(
                    HIViewRef       inView,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    OSStatus    Send(
                    HIViewRef       inView,
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    void        Send(
                    MenuRef         inMenu,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    OSStatus    Send(
                    MenuRef         inMenu,
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    void        SendToApp(
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    OSStatus    SendToApp(
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    void        SendToUserFocus(
                    OptionBits      inOptions = 0);
    /*! @overload
    */
    OSStatus    SendToUserFocus(
                    const std::nothrow_t&,
                    OptionBits      inOptions = 0);
    //@}
    
    //! @name Posting
    //@{
    //! Posts the event to the main event queue.
    void        Post(EventPriority inPriority = kEventPriorityStandard);
    //@}

    //! @name Forwarding
    //@{
    //! Forwards an incoming event to the next event handler in the chain.
    bool        Forward();
    //! Forwards an incoming event to the next event handler in the chain without throwing an exception.
    OSStatus    Forward(const std::nothrow_t&);
    //@}
    
    //! @name Synchronisation
    //@{
    //! Sets output parameters.
    virtual void    Update();
    //! Retrieves output parameters.
    virtual void    Retrieve();
    //@}
    
    //! @name Utility
    //@{
    static OSStatus StoreExceptionIntoCarbonEvent(
                        EventRef                inEvent, 
                        const std::exception&   ex);
    static void     RethrowExceptionFromCarbonEvent(
                        EventRef                inEvent, 
                        OSStatus                inStatus);
    //@}
    
private:
    
    // member variables
    EventHandlerCallRef mHandlerCallRef;    //!< The caller ref (incoming events only).
    EventRef            mEvent;             //!< The event.
};

// ------------------------------------------------------------------------------------------
/*! @note   Incoming EventBase objects are instantiated as a result of a Carbon %Event 
            handler invocation.
    
    @return Boolean indication of 'incoming' state.
*/
inline bool
EventBase::IsIncoming() const
{
    return (mHandlerCallRef != NULL);
}

// ------------------------------------------------------------------------------------------
inline
EventBase::operator EventRef ()
{
    return (mEvent);
}


// ==========================================================================================
//  Event<CLASS, KIND>

/*!
    @brief  Template class representing a specific Carbon %Event.
    
    Usually this template is specialised in order to add member variables holding event 
    parameters, or to override Update() and/or Retrieve().  However, if a Carbon %Event has 
    no input or output parameters, this class may be instantiated directly.
    
    @ingroup    CarbonEvents
    @sa         @ref using_events
    
    @todo       Flesh out the missing template specialisations for events of these classes:
                - kEventClassKeyboard
                - kEventClassTextInput
                - kEventClassApplication
                - kEventClassAppleEvent
                - kEventClassMenu
                - kEventClassWindow
                - kEventClassControl
                - kEventClassCommand
                - kEventClassTablet
                - kEventClassVolume
                - kEventClassAppearance
                - kEventClassService
                - kEventClassToolbar
                - kEventClassToolbarItem
                - kEventClassToolbarItemView
                - kEventClassAccessibility
                - kEventClassSystem
                - kEventClassInk
                - kEventClassTSMDocumentAccess
*/
template <UInt32 CLASS, UInt32 KIND>
class Event : public EventBase
{
public:

#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef UInt32  CLASS;  //!< Template parameter.  Should be an integral four-char constant representing a Carbon %Event class.
    typedef UInt32  KIND;   //!< Template parameter.  Should be an integral four-char constant representing a Carbon %Event kind in @a CLASS's namespace.
    //@}
#endif
    
    //! @name Constructors
    //@{
    //! Creates a new Carbon %Event.
    Event();
    
    //! Creates a object wrapping an existing Carbon %Event.
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    //@}
};

// ------------------------------------------------------------------------------------------
/*! The event's class and kind come from @a CLASS and @a KIND template parameters, 
    respectively.
    
    Use this constructor to create a Carbon %Event prior to sending or posting it.
*/
template <UInt32 CLASS, UInt32 KIND>
Event<CLASS, KIND>::Event()
    : EventBase(CLASS, KIND)
{
}

// ------------------------------------------------------------------------------------------
/*! The event's class and kind come from @a CLASS and @a KIND template parameters, 
    respectively.
    
    Use this constructor when handling incoming Carbon %Events.
*/
template <UInt32 CLASS, UInt32 KIND>
Event<CLASS, KIND>::Event(
    EventHandlerCallRef inHandlerCallRef,   //!< The caller ref representing a location in the chain of event handlers.
    EventRef            inEvent)            //!< The Carbon %Event.  It gets retained by the constructor.
        : EventBase(inHandlerCallRef, inEvent)
{
}


// ==========================================================================================
//  Event<CLASS, KIND> Template Specialisations

#ifndef DOXYGEN_SKIP

#pragma mark -
#pragma mark * kEventClassMouse *

#pragma mark Event<kEventClassMouse, kEventMouseDown>

/*!
    @note   This event doesn't store the mouse location because it can be in either 
            global or window-relative coordinates, and it seems silly to fetch both 
            when most developers will want only one or the other.
*/
template <>
class Event<kEventClassMouse, kEventMouseDown> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    Point               mGlobalMouseLocation;
    WindowRef           mWindowRef;
    Point               mMouseLocation;
    UInt32              mKeyModifiers;
    EventMouseButton    mMouseButton;
    UInt32              mClickCount;
    UInt32              mMouseChord;
};

#pragma mark Event<kEventClassMouse, kEventMouseUp>

template <>
class Event<kEventClassMouse, kEventMouseUp> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    Point               mGlobalMouseLocation;
    WindowRef           mWindowRef;
    Point               mMouseLocation;
    UInt32              mKeyModifiers;
    EventMouseButton    mMouseButton;
    UInt32              mClickCount;
    UInt32              mMouseChord;
};

#pragma mark Event<kEventClassMouse, kEventMouseMoved>

template <>
class Event<kEventClassMouse, kEventMouseMoved> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    Point               mGlobalMouseLocation;
    WindowRef           mWindowRef;
    Point               mMouseLocation;
    UInt32              mKeyModifiers;
    Point               mMouseDelta;
};


#pragma mark Event<kEventClassMouse, kEventMouseWheelMoved>

template <>
class Event<kEventClassMouse, kEventMouseWheelMoved> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    Point               mGlobalMouseLocation;
    WindowRef           mWindowRef;
    Point               mMouseLocation;
    UInt32              mKeyModifiers;
    EventMouseWheelAxis mMouseWheelAxis;
    SInt32              mMouseWheelDelta;
};


#pragma mark -
#pragma mark * kEventClassTextInput *

// The following events have a lot of parameters and are rarely used.  So instead of having 
// either incomplete implementations, or implementations that are too slow for some needs, 
// I've decided to punt and let framework clients implement them to their liking.
//  
//  - kEventTextInputUpdateActiveInputArea
//  - kEventTextInputOffsetToPos
//  - kEventTextInputPosToOffset
//  - kEventTextInputShowHideBottomWindow
//  - kEventTextInputGetSelectedText
//  - kEventTextInputUnicodeText
//  - kEventTextInputInputModeChanged (??)
//  - kEventTextInputInputMenuChanged (??)

#pragma mark Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent>

template <>
class Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    ~Event();
    
    // event arguments
    const UniChar*  mText;
    size_t          mLength;
    EventRef        mKeyboardEvent;
    
private:
    
    UniChar     mSmallBuffer[256];
    UniChar*    mBigBuffer;
};


//#pragma mark Event<kEventClassTextInput, kEventTextInputFilterText>
//
//template <>
//class Event<kEventClassTextInput, kEventTextInputFilterText> : public EventBase
//{
//public:
//  
//  // constructors
//  Event(
//      EventHandlerCallRef inHandlerCallRef, 
//      EventRef            inEvent);
//  
//  virtual void    Update();
//  
//  // event arguments
//  String  mText;
//};


#pragma mark -
#pragma mark * kEventClassKeyboard *

// The following events are very low-level, so I've decided to punt and let framework 
// clients implement them to their liking.
//  
//  - kEventRawKeyDown
//  - kEventRawKeyRepeat
//  - kEventRawKeyUp

#pragma mark Event<kEventClassKeyboard, kEventRawKeyModifiersChanged>

template <>
class Event<kEventClassKeyboard, kEventRawKeyModifiersChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UInt32  mKeyModifiers;
};


#pragma mark Event<kEventClassKeyboard, kEventHotKeyPressed>

template <>
class Event<kEventClassKeyboard, kEventHotKeyPressed> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    EventHotKeyID   mHotKeyID;
};


#pragma mark Event<kEventClassKeyboard, kEventHotKeyReleased>

template <>
class Event<kEventClassKeyboard, kEventHotKeyReleased> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    EventHotKeyID   mHotKeyID;
};


#pragma mark -
#pragma mark * kEventClassApplication *

#pragma mark Event<kEventClassApplication, kEventAppActivated>

// The following events don't have any input or output parameters, so they don't 
// need a template specialisation:
//
//  - kEventAppDeactivated
//  - kEventAppQuit
//  - kEventAppHidden
//  - kEventAppShown
// 
// The following events don't have a template specialisation because they are unlikely 
// to be used in normal applications:
//
//  - kEventAppAvailableWindowBoundsChanged
 
template <>
class Event<kEventClassApplication, kEventAppActivated> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassApplication, kEventAppLaunchNotification>

template <>
class Event<kEventClassApplication, kEventAppLaunchNotification> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    ProcessSerialNumber mProcessID;
    UInt32              mLaunchRefCon;
    OSStatus            mLaunchErr;
};


#pragma mark Event<kEventClassApplication, kEventAppLaunched>

template <>
class Event<kEventClassApplication, kEventAppLaunched> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    ProcessSerialNumber mProcessID;
};


#pragma mark Event<kEventClassApplication, kEventAppTerminated>

template <>
class Event<kEventClassApplication, kEventAppTerminated> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    ProcessSerialNumber mProcessID;
};


#pragma mark Event<kEventClassApplication, kEventAppFrontSwitched>

template <>
class Event<kEventClassApplication, kEventAppFrontSwitched> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    ProcessSerialNumber mProcessID;
};


#pragma mark Event<kEventClassApplication, kEventAppFocusMenuBar>

template <>
class Event<kEventClassApplication, kEventAppFocusMenuBar> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UInt32  mKeyModifiers;
};


#pragma mark Event<kEventClassApplication, kEventAppFocusNextDocumentWindow>

template <>
class Event<kEventClassApplication, kEventAppFocusNextDocumentWindow> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UInt32  mKeyModifiers;
};


#pragma mark Event<kEventClassApplication, kEventAppFocusNextFloatingWindow>

template <>
class Event<kEventClassApplication, kEventAppFocusNextFloatingWindow> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UInt32  mKeyModifiers;
};


#pragma mark Event<kEventClassApplication, kEventAppFocusToolbar>

template <>
class Event<kEventClassApplication, kEventAppFocusToolbar> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UInt32  mKeyModifiers;
};


#pragma mark Event<kEventClassApplication, kEventAppGetDockTileMenu>

template <>
class Event<kEventClassApplication, kEventAppGetDockTileMenu> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    OSPtr<MenuRef>  mMenuRef;
};


#pragma mark Event<kEventClassApplication, kEventAppSystemUIModeChanged>

template <>
class Event<kEventClassApplication, kEventAppSystemUIModeChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UInt32  mSystemUIMode;
};


#if B_BUILDING_CAN_USE_10_3_APIS

#pragma mark Event<kEventClassApplication, kEventAppActiveWindowChanged>

template <>
class Event<kEventClassApplication, kEventAppActiveWindowChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mPreviousWindow;
    WindowRef   mCurrentWindow;
};

#endif  // B_BUILDING_CAN_USE_10_3_APIS


#pragma mark Event<kEventClassApplication, kEventAppIsEventInInstantMouser>

template <>
class Event<kEventClassApplication, kEventAppIsEventInInstantMouser> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    EventRef    mEvent;
    bool        mInInstantMouser;
};


#pragma mark -
#pragma mark * kEventClassMenu *

#pragma mark Event<kEventClassMenu, kEventMenuDispose>

template <>
class Event<kEventClassMenu, kEventMenuDispose> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    MenuRef mMenuRef;
};


#pragma mark Event<kEventClassMenu, kEventMenuEnableItems>

template <>
class Event<kEventClassMenu, kEventMenuEnableItems> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    MenuRef mMenuRef;
    bool    mForKeyEvent;
    UInt32  mContext;
};


#pragma mark Event<kEventClassMenu, kEventMenuPopulate>

template <>
class Event<kEventClassMenu, kEventMenuPopulate> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    MenuRef     mMenuRef;
};


#pragma mark -
#pragma mark * kEventClassWindow *

#pragma mark Event<kEventClassWindow, kEventWindowUpdate>

template <>
class Event<kEventClassWindow, kEventWindowUpdate> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowDrawContent>

template <>
class Event<kEventClassWindow, kEventWindowDrawContent> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowActivated>

template <>
class Event<kEventClassWindow, kEventWindowActivated> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowDeactivated>

template <>
class Event<kEventClassWindow, kEventWindowDeactivated> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowShowing>

template <>
class Event<kEventClassWindow, kEventWindowShowing> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowHiding>

template <>
class Event<kEventClassWindow, kEventWindowHiding> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowShown>

template <>
class Event<kEventClassWindow, kEventWindowShown> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowHidden>

template <>
class Event<kEventClassWindow, kEventWindowHidden> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowBoundsChanging>

template <>
class Event<kEventClassWindow, kEventWindowBoundsChanging> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    WindowRef   mWindowRef;
    UInt32      mAttributes;    // bounds change attributes, that is
    Rect        mOriginalBounds;
    Rect        mPreviousBounds;
    Rect        mCurrentBounds;
    Rect        mNewBounds;     // output argument if != mCurrentBounds
};


#pragma mark Event<kEventClassWindow, kEventWindowBoundsChanged>

template <>
class Event<kEventClassWindow, kEventWindowBoundsChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
    UInt32      mAttributes;    // bounds change attributes, that is
    Rect        mOriginalBounds;
    Rect        mPreviousBounds;
    Rect        mCurrentBounds;
};


#pragma mark Event<kEventClassWindow, kEventWindowResizeStarted>

template <>
class Event<kEventClassWindow, kEventWindowResizeStarted> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowResizeCompleted>

template <>
class Event<kEventClassWindow, kEventWindowResizeCompleted> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowTransitionCompleted>

template <>
class Event<kEventClassWindow, kEventWindowTransitionCompleted> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
    void*       mUserData;
};


#pragma mark Event<kEventClassWindow, kEventWindowClose>

template <>
class Event<kEventClassWindow, kEventWindowClose> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowClosed>

template <>
class Event<kEventClassWindow, kEventWindowClosed> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowGetIdealSize>

template <>
class Event<kEventClassWindow, kEventWindowGetIdealSize> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    WindowRef   mWindowRef;
    Size        mDimensions;
};


#pragma mark Event<kEventClassWindow, kEventWindowGetMinimumSize>

template <>
class Event<kEventClassWindow, kEventWindowGetMinimumSize> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    WindowRef   mWindowRef;
    Size        mDimensions;
};


#pragma mark Event<kEventClassWindow, kEventWindowGetMaximumSize>

template <>
class Event<kEventClassWindow, kEventWindowGetMaximumSize> : public EventBase
{
public:
    
    // constructors
    Event(
        WindowRef           inWindowRef);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    WindowRef   mWindowRef;
    Size        mDimensions;
};


#pragma mark Event<kEventClassWindow, kEventWindowToolbarSwitchMode>

template <>
class Event<kEventClassWindow, kEventWindowToolbarSwitchMode> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowFocusAcquired>

template <>
class Event<kEventClassWindow, kEventWindowFocusAcquired> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowFocusRelinquish>

template <>
class Event<kEventClassWindow, kEventWindowFocusRelinquish> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowSheetOpening>

template <>
class Event<kEventClassWindow, kEventWindowSheetOpening> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    WindowRef   mWindowRef;
    bool        mCancel;
};


#pragma mark Event<kEventClassWindow, kEventWindowSheetOpened>

template <>
class Event<kEventClassWindow, kEventWindowSheetOpened> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowSheetClosing>

template <>
class Event<kEventClassWindow, kEventWindowSheetClosing> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    WindowRef   mWindowRef;
    bool        mCancel;
};


#pragma mark Event<kEventClassWindow, kEventWindowSheetClosed>

template <>
class Event<kEventClassWindow, kEventWindowSheetClosed> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowDrawerOpening>

template <>
class Event<kEventClassWindow, kEventWindowDrawerOpening> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    WindowRef   mWindowRef;
    bool        mCancel;
};


#pragma mark Event<kEventClassWindow, kEventWindowDrawerOpened>

template <>
class Event<kEventClassWindow, kEventWindowDrawerOpened> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowDrawerClosing>

template <>
class Event<kEventClassWindow, kEventWindowDrawerClosing> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    WindowRef   mWindowRef;
    bool        mCancel;
};


#pragma mark Event<kEventClassWindow, kEventWindowDrawerClosed>

template <>
class Event<kEventClassWindow, kEventWindowDrawerClosed> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark Event<kEventClassWindow, kEventWindowPaint>

template <>
class Event<kEventClassWindow, kEventWindowPaint> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark -
#pragma mark * kEventClassCommand *

#pragma mark Event<kEventClassCommand, kEventCommandProcess>

template <>
class Event<kEventClassCommand, kEventCommandProcess> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef         inHandlerCallRef, 
        EventRef                    inEvent);
    Event(
        const HICommandExtended&    inCommand, 
        UInt32                      inKeyModifiers, 
        UInt32                      inMenuContext = 0);
    
    // event arguments
    HICommandExtended   mHICommand;
    UInt32              mMenuContext;
    UInt32              mKeyModifiers;
};


#pragma mark Event<kEventClassCommand, kEventCommandUpdateStatus>

template <>
class Event<kEventClassCommand, kEventCommandUpdateStatus> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef         inHandlerCallRef, 
        EventRef                    inEvent);
    Event(
        const HICommandExtended&    inCommand, 
        UInt32                      inMenuContext = 0);
    
    virtual void    Update();
    
    // event arguments
    HICommandExtended   mHICommand;
    UInt32              mMenuContext;
    CommandData         mData;
};


#pragma mark -
#pragma mark * kEventClassControl *

#pragma mark Event<kEventClassControl, kEventControlInitialize>

template <>
class Event<kEventClassControl, kEventControlInitialize> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef   mViewRef;
    Collection  mCollection;
    UInt32      mFeatures;
};


#pragma mark Event<kEventClassControl, kEventControlDispose>

template <>
class Event<kEventClassControl, kEventControlDispose> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlGetOptimalBounds>

template <>
class Event<kEventClassControl, kEventControlGetOptimalBounds> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef   mViewRef;
    Rect        mOptimalBounds;
    short       mBaselineOffset;
};


#pragma mark Event<kEventClassControl, kEventControlHit>

template <>
class Event<kEventClassControl, kEventControlHit> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    UInt32          mKeyModifiers;
};


#pragma mark Event<kEventClassControl, kEventControlSimulateHit>

template <>
class Event<kEventClassControl, kEventControlSimulateHit> : public EventBase
{
public:
    
    // constructors
    Event(
        HIViewRef           inViewRef, 
        UInt32              inKeyModifiers, 
        HIViewPartCode      inPartCode);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
    UInt32          mKeyModifiers;
    HIViewPartCode  mPartCode;
};


#pragma mark Event<kEventClassControl, kEventControlHitTest>

template <>
class Event<kEventClassControl, kEventControlHitTest> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    Point           mMouseLocation;
    HIViewPartCode  mPartCode;
};


#pragma mark Event<kEventClassControl, kEventControlDraw>

template <>
class Event<kEventClassControl, kEventControlDraw> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    CGContextRef    mCGContext;
    Shape*          mDrawShape;

private:
    // event arguments
    Shape           mDrawShapeObject;
};


#pragma mark Event<kEventClassControl, kEventControlSetFocusPart>

template <>
class Event<kEventClassControl, kEventControlSetFocusPart> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    bool            mFocusEverything;
};


#pragma mark Event<kEventClassControl, kEventControlGetFocusPart>

template <>
class Event<kEventClassControl, kEventControlGetFocusPart> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
};


#pragma mark Event<kEventClassControl, kEventControlActivate>

template <>
class Event<kEventClassControl, kEventControlActivate> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlDeactivate>

template <>
class Event<kEventClassControl, kEventControlDeactivate> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlClick>

template <>
class Event<kEventClassControl, kEventControlClick> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef           mViewRef;
    Point               mMouseLocation; //!< Window-relative
    UInt32              mKeyModifiers;
    EventMouseButton    mMouseButton;
    UInt32              mClickCount;
    UInt32              mMouseChord;
};


#pragma mark Event<kEventClassControl, kEventControlGetAutoToggleValue>

template <>
class Event<kEventClassControl, kEventControlGetAutoToggleValue> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    SInt32          mNewValue;
};


#pragma mark Event<kEventClassControl, kEventControlGetNextFocusCandidate>

template <>
class Event<kEventClassControl, kEventControlGetNextFocusCandidate> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    HIViewRef       mStartView;
    HIViewRef       mNextView;
};


#pragma mark Event<kEventClassControl, kEventControlInterceptSubviewClick>

template <>
class Event<kEventClassControl, kEventControlInterceptSubviewClick> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    EventRef        mMouseDownEvent;
};


#pragma mark Event<kEventClassControl, kEventControlDragEnter>

template <>
class Event<kEventClassControl, kEventControlDragEnter> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef   mViewRef;
    DragRef     mDragRef;
    bool        mLikesDrag;
};


#pragma mark Event<kEventClassControl, kEventControlDragWithin>

template <>
class Event<kEventClassControl, kEventControlDragWithin> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    DragRef     mDragRef;
};


#pragma mark Event<kEventClassControl, kEventControlDragLeave>

template <>
class Event<kEventClassControl, kEventControlDragLeave> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    DragRef     mDragRef;
};


#pragma mark Event<kEventClassControl, kEventControlDragReceive>

template <>
class Event<kEventClassControl, kEventControlDragReceive> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    DragRef     mDragRef;
};


#pragma mark Event<kEventClassControl, kEventControlGetClickActivation>

template <>
class Event<kEventClassControl, kEventControlGetClickActivation> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef               mViewRef;
    Point                   mMouseLocation; //!< View-relative
    UInt32                  mKeyModifiers;
    EventMouseButton        mMouseButton;
    UInt32                  mClickCount;
    UInt32                  mMouseChord;
    ClickActivationResult   mResult;
};


#pragma mark Event<kEventClassControl, kEventControlTrack>

template <>
class Event<kEventClassControl, kEventControlTrack> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef           mViewRef;
    Point               mMouseLocation; //!< View-relative
    UInt32              mKeyModifiers;
    HIViewPartCode      mHitPart;
};


#pragma mark Event<kEventClassControl, kEventControlGetPartRegion>

template <>
class Event<kEventClassControl, kEventControlGetPartRegion> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    Shape           mPartShape;

private:

    // event arguments
    bool            mPrefersShape;
};


#pragma mark Event<kEventClassControl, kEventControlGetPartBounds>

template <>
class Event<kEventClassControl, kEventControlGetPartBounds> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    Rect            mPartBounds;
};


#pragma mark Event<kEventClassControl, kEventControlSetData>

template <>
class Event<kEventClassControl, kEventControlSetData> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    ResType         mDataTag;
    const void*     mDataPtr;
    size_t          mDataSize;
};


#pragma mark Event<kEventClassControl, kEventControlGetData>

template <>
class Event<kEventClassControl, kEventControlGetData> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPartCode;
    ResType         mDataTag;
    void*           mDataPtr;
    size_t          mDataSize;
    size_t          mOriginalDataSize;
};


#pragma mark Event<kEventClassControl, kEventControlGetSizeConstraints>

template <>
class Event<kEventClassControl, kEventControlGetSizeConstraints> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    HIViewRef   mViewRef;
    Size        mMinimumSize;
    Size        mMaximumSize;
};


#pragma mark Event<kEventClassControl, kEventControlValueFieldChanged>

template <>
class Event<kEventClassControl, kEventControlValueFieldChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlAddedSubControl>

template <>
class Event<kEventClassControl, kEventControlAddedSubControl> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    HIViewRef   mSubViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlRemovingSubControl>

template <>
class Event<kEventClassControl, kEventControlRemovingSubControl> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    HIViewRef   mSubViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlBoundsChanged>

template <>
class Event<kEventClassControl, kEventControlBoundsChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    UInt32      mAttributes;
    Rect        mOriginalBounds;
    Rect        mPreviousBounds;
    Rect        mCurrentBounds;
};


#pragma mark Event<kEventClassControl, kEventControlTitleChanged>

template <>
class Event<kEventClassControl, kEventControlTitleChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlVisibilityChanged>

template <>
class Event<kEventClassControl, kEventControlVisibilityChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlOwningWindowChanged>

template <>
class Event<kEventClassControl, kEventControlOwningWindowChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef   mViewRef;
    WindowRef   mOriginalOwningWindow;
    WindowRef   mCurrentOwningWindow;
};


#pragma mark Event<kEventClassControl, kEventControlHiliteChanged>

template <>
class Event<kEventClassControl, kEventControlHiliteChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
    HIViewPartCode  mPreviousPartCode;
    HIViewPartCode  mCurrentPartCode;
};


#pragma mark Event<kEventClassControl, kEventControlEnabledStateChanged>

template <>
class Event<kEventClassControl, kEventControlEnabledStateChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark Event<kEventClassControl, kEventControlTrackingAreaEntered>

template <>
class Event<kEventClassControl, kEventControlTrackingAreaEntered> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewTrackingAreaRef   mTrackingAreaRef;
    UInt32                  mKeyModifiers;
    Point                   mMouseLocation;
};


#pragma mark Event<kEventClassControl, kEventControlTrackingAreaExited>

template <>
class Event<kEventClassControl, kEventControlTrackingAreaExited> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewTrackingAreaRef   mTrackingAreaRef;
    UInt32                  mKeyModifiers;
    Point                   mMouseLocation;
};


#pragma mark -
#pragma mark * kEventClassService *

#pragma mark Event<kEventClassService, kEventServiceCopy>

template <>
class Event<kEventClassService, kEventServiceCopy> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    PasteboardRef   mPasteboardRef;
};


#pragma mark Event<kEventClassService, kEventServicePaste>

template <>
class Event<kEventClassService, kEventServicePaste> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    PasteboardRef   mPasteboardRef;
};


#pragma mark Event<kEventClassService, kEventServiceGetTypes>

template <>
class Event<kEventClassService, kEventServiceGetTypes> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    MutableArray<CFStringRef>   mCopyTypes;
    MutableArray<CFStringRef>   mPasteTypes;
};


#pragma mark Event<kEventClassService, kEventServicePerform>

template <>
class Event<kEventClassService, kEventServicePerform> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    PasteboardRef   mPasteboardRef;
    String          mMessage;
    String          mUserData;
};


#pragma mark -
#pragma mark * kEventClassToolbar *

#pragma mark Event<kEventClassToolbar, kEventToolbarGetDefaultIdentifiers>

template <>
class Event<kEventClassToolbar, kEventToolbarGetDefaultIdentifiers> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    OSPtr<HIToolbarRef>         mToolbar;
    MutableArray<CFStringRef>   mIdentifiers;
};


#pragma mark Event<kEventClassToolbar, kEventToolbarGetAllowedIdentifiers>

template <>
class Event<kEventClassToolbar, kEventToolbarGetAllowedIdentifiers> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    OSPtr<HIToolbarRef>         mToolbar;
    MutableArray<CFStringRef>   mIdentifiers;
};


#pragma mark Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>

template <>
class Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    OSPtr<HIToolbarRef>     mToolbar;
    String                  mItemIdentifier;
    OSPtr<CFTypeRef>        mItemConfigData;
    OSPtr<HIToolbarItemRef> mToolbarItem;
};


#pragma mark Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>

template <>
class Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    DragRef                 mDragRef;
    OSPtr<HIToolbarItemRef> mToolbarItem;
};


#pragma mark Event<kEventClassToolbar, kEventToolbarItemAdded>

template <>
class Event<kEventClassToolbar, kEventToolbarItemAdded> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIToolbarItemRef    mToolbarItemRef;
    CFIndex             mIndex;
};


#pragma mark Event<kEventClassToolbar, kEventToolbarItemRemoved>

template <>
class Event<kEventClassToolbar, kEventToolbarItemRemoved> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIToolbarItemRef    mToolbarItemRef;
};


#pragma mark Event<kEventClassToolbar, kEventToolbarGetSelectableIdentifiers>

template <>
class Event<kEventClassToolbar, kEventToolbarGetSelectableIdentifiers> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    OSPtr<HIToolbarRef>         mToolbar;
    MutableArray<CFStringRef>   mIdentifiers;
};


#pragma mark -
#pragma mark * kEventClassToolbarItem *

#pragma mark Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>

template <>
class Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    OSPtr<CFTypeRef>    mConfigData;
};


#pragma mark Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>

template <>
class Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    OSPtr<HIViewRef>    mCustomView;
};


#pragma mark Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>

template <>
class Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    DragRef mDragRef;
    bool    mLikesDrag;
};


#pragma mark Event<kEventClassToolbarItem, kEventToolbarItemAcceptDrop>

template <>
class Event<kEventClassToolbarItem, kEventToolbarItemAcceptDrop> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    DragRef mDragRef;
};


#pragma mark Event<kEventClassToolbarItem, kEventToolbarItemSelectedStateChanged>

template <>
class Event<kEventClassToolbarItem, kEventToolbarItemSelectedStateChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindowRef;
};


#pragma mark -
#pragma mark * kEventClassToolbarItemView *

#pragma mark Event<kEventClassToolbarItemView, kEventToolbarItemViewConfigForMode>

template <>
class Event<kEventClassToolbarItemView, kEventToolbarItemViewConfigForMode> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIToolbarDisplayMode    mDisplayMode;
};


#pragma mark Event<kEventClassToolbarItemView, kEventToolbarItemViewConfigForSize>

template <>
class Event<kEventClassToolbarItemView, kEventToolbarItemViewConfigForSize> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIToolbarDisplaySize    mDisplaySize;
};


#pragma mark -
#pragma mark * kEventClassScrollable *

#pragma mark Event<kEventClassScrollable, kEventScrollableGetInfo>

template <>
class Event<kEventClassScrollable, kEventScrollableGetInfo> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    Size    mImageSize;
    Size    mViewSize;
    Size    mLineSize;
    Point   mOrigin;
};


#pragma mark Event<kEventClassScrollable, kEventScrollableScrollTo>

template <>
class Event<kEventClassScrollable, kEventScrollableScrollTo> : public EventBase
{
public:
    
    // constructors
    Event(
        Point               inOrigin);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    Point   mOrigin;
};


#pragma mark -
#pragma mark * kEventClassVolume *

#pragma mark Event<kEventClassVolume, kEventVolumeMounted>

template <>
class Event<kEventClassVolume, kEventVolumeMounted> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    FSVolumeRefNum  mVolume;
};


#pragma mark Event<kEventClassVolume, kEventVolumeUnmounted>

template <>
class Event<kEventClassVolume, kEventVolumeUnmounted> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    FSVolumeRefNum  mVolume;
};


#pragma mark -
#pragma mark * kEventClassAppearance *

#pragma mark Event<kEventClassAppearance, kEventAppearanceScrollBarVariantChanged>

template <>
class Event<kEventClassAppearance, kEventAppearanceScrollBarVariantChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    SInt16  mVariant;
};


#pragma mark -
#pragma mark * kEventClassHIComboBox *

#pragma mark Event<kEventClassHIComboBox, kEventComboBoxListItemSelected>

template <>
class Event<kEventClassHIComboBox, kEventComboBoxListItemSelected> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
    CFIndex         mIndex;
};


#pragma mark -
#pragma mark * kEventClassSearchField *

#pragma mark Event<kEventClassSearchField, kEventSearchFieldCancelClicked>

template <>
class Event<kEventClassSearchField, kEventSearchFieldCancelClicked> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark Event<kEventClassSearchField, kEventSearchFieldSearchClicked>

template <>
class Event<kEventClassSearchField, kEventSearchFieldSearchClicked> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark -
#pragma mark * kEventClassTextField *

#pragma mark Event<kEventClassTextField, kEventTextAccepted>

template <>
class Event<kEventClassTextField, kEventTextAccepted> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark Event<kEventClassTextField, kEventTextShouldChangeInRange>

/*!
    @brief  Text-filtering event.
    
    The various scenarios for text filtering are outlined below:
    
    - If you want the input text to be added as-is to the text field, return false from 
      your handler.
    - If you want the input operation to be ignored completely, set the mCancel flag and 
      return true from your handler.
    - If you want the input text to be changed, give mText a new value and return true from 
      your handler.
    - If you want the selection to be deleted, set mText to an empty string and 
      return true from your handler.
*/
template <>
class Event<kEventClassTextField, kEventTextShouldChangeInRange> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    virtual void    Update();
    virtual void    Retrieve();
    
    // event arguments
    CFRange         mSelection;
    String          mText;
    //! If true, the editing operation is ignored.
    bool            mCancel;
    
private:
    
    CFStringRef     mOriginalText;
};


#pragma mark Event<kEventClassTextField, kEventTextDidChange>

template <>
class Event<kEventClassTextField, kEventTextDidChange> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    bool            mInlineEditing;
};


#pragma mark -
#pragma mark * kEventClassClockView *

#pragma mark Event<kEventClassClockView, kEventClockDateOrTimeChanged>

template <>
class Event<kEventClassClockView, kEventClockDateOrTimeChanged> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    HIViewRef       mViewRef;
};


#pragma mark -
#pragma mark * kEventClassFont *

#pragma mark Event<kEventClassFont, kEventFontPanelClosed>

template <>
class Event<kEventClassFont, kEventFontPanelClosed> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
};

#pragma mark Event<kEventClassFont, kEventMouseUp>

template <>
class Event<kEventClassFont, kEventFontSelection> : public EventBase
{
public:
    
    // constructors
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    ATSUFontID*             mFontIDPtr;     // If NULL, no font ID was present in the event.
    Fixed*                  mFontSizePtr;   // If NULL, no font size was present in the event.
    OSPtr<CFDictionaryRef>  mDictionary;

private:

    ATSUFontID      mFontID;
    Fixed           mFontSize;
};


#pragma mark -
#pragma mark * kEventClassB *

#pragma mark Event<kEventClassB, kEventBCloseDocument>

template <>
class Event<kEventClassB, kEventBCloseDocument> : public EventBase
{
public:
    
    // constructors
    Event(
        EventTargetRef      inDocumentTarget);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
};


#pragma mark Event<kEventClassB, kEventBSaveDocument>

template <>
class Event<kEventClassB, kEventBSaveDocument> : public EventBase
{
public:
    
    // constructors
    Event(
        EventTargetRef      inDocumentTarget, 
        WindowRef           inWindow, 
        bool                inFromUser);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    WindowRef   mWindow;
    bool        mFromUser;
};


#pragma mark Event<kEventClassB, kEventBUndoBegin>

template <>
class Event<kEventClassB, kEventBUndoBegin> : public EventBase
{
public:
    
    // constructors
    Event();
    Event(
        const String&       inName);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    String  mName;
};


#pragma mark Event<kEventClassB, kEventBUndoAdd>

template <>
class Event<kEventClassB, kEventBUndoAdd> : public EventBase
{
public:
    
    // constructors
    Event(
        UndoAction*         inUndoAction);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    UndoAction* mUndoAction;
};


#pragma mark Event<kEventClassB, kEventBUndoEnable>

template <>
class Event<kEventClassB, kEventBUndoEnable> : public EventBase
{
public:
    
    // constructors
    Event(
        bool                inEnable);
    Event(
        EventHandlerCallRef inHandlerCallRef, 
        EventRef            inEvent);
    
    // event arguments
    bool    mEnable;
};

#endif  // DOXYGEN_SKIP


}   // namespace B


#endif  // BEvent_H_
