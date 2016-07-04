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

#ifndef BEventHandler_H_
#define BEventHandler_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// compiler headers
#ifdef __MWERKS__
#	include <hash_map>
#elif defined(__GNUC__)
#	include <ext/hash_map>
#endif

// library headers
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class   EventHandler;
class   EventHandlerFunctorBase;
void	intrusive_ptr_add_ref(EventHandlerFunctorBase* f);
void	intrusive_ptr_release(EventHandlerFunctorBase* f);


// ==========================================================================================
//	EventHandlerFunctorBase

/*!
    @brief	Abstract base class representing a Carbon %Event functor.
    
    @note	This class is for the internal use of EventHandler only.
*/
class EventHandlerFunctorBase : public boost::noncopyable
{
public:
    
    //! Default constructor.
                    EventHandlerFunctorBase()	: mRefCount(0) {}
    //! Destructor.
    virtual			~EventHandlerFunctorBase();
    
    //! Invocation.
    virtual bool	operator () (
                        EventHandlerCallRef	inHandlerCallRef, 
                        EventRef			inEvent) = 0;

private:
    
    // member variables
    SInt32		mRefCount;
    
    // friends
    friend class    EventHandler;
    friend void     intrusive_ptr_add_ref(EventHandlerFunctorBase* f);
    friend void     intrusive_ptr_release(EventHandlerFunctorBase* f);
};


// ==========================================================================================
//	EventHandlerFunctor

#pragma mark -

/*!
    @brief	Template class representing a Carbon %Event functor.
    
    @note	This class is for the internal use of EventHandler only.
*/
template <UInt32 CLASS, UInt32 KIND>
class EventHandlerFunctor : public EventHandlerFunctorBase
{
private:
    
    //! Synonym for the functor's type.
    typedef boost::function1<bool, Event<CLASS, KIND>&> FunctorType;
    
    //! Constructor.
                    EventHandlerFunctor(FunctorType inFunctor);
    
    //! Invocation.
    virtual bool	operator () (
                        EventHandlerCallRef	inHandlerCallRef, 
                        EventRef			inEvent);
    
    // member variables
    FunctorType	mFunctor;
    
    // friends
    friend class    EventHandler;
};

// ------------------------------------------------------------------------------------------
template <UInt32 CLASS, UInt32 KIND>
EventHandlerFunctor<CLASS, KIND>::EventHandlerFunctor(
    FunctorType  inFunctor)
        : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <UInt32 CLASS, UInt32 KIND> bool
EventHandlerFunctor<CLASS, KIND>::operator () (
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent)
{
	Event<CLASS, KIND>	event(inHandlerCallRef, inEvent);
	bool				handled;
	
	handled = mFunctor(event);
	
	event.Update();
	
	return (handled);
}

// ==========================================================================================
//	EventHandler

#pragma mark -

/*!
	@brief	Holds a set of Carbon %Event handlers for a given Carbon %Event target.
	
	EventHandler holds a set of mappings between Carbon %Event class/kind pairs on 
	the one hand, and functions implementing the Carbon %Events on the other.
	
	Often, clients will want to handle a Carbon %Event via a member function.  This it can 
	be done by one of the overloads of Add() which takes an object and a 
	pointer-to-member-function for that object.  For example:
	
	@code
		class MyObject {
		    bool MyHandleEvent(Event<kMyEventClass, kMyEventKind>& event);
		};
		
		MyObject obj;
		
		myEventHandler.Add(&obj, &MyObject::MyHandleEvent);
	@endcode
	
	@ingroup	CarbonEvents
	@sa			@ref using_events
*/
class EventHandler : public boost::noncopyable
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Constructs the event handler with the @c EventTargetRef.
	EventHandler(
		EventTargetRef	inTarget);
	/*!	@overload
	*/
	EventHandler(
		HIObjectRef		inTarget);
	/*!	@overload
	*/
	EventHandler(
		HIViewRef		inTarget);
	/*!	@overload
	*/
	EventHandler(
		MenuRef			inTarget);
	/*!	@overload
	*/
	EventHandler(
		WindowRef		inTarget);
	//! Destructor.
	virtual	~EventHandler();
	//@}
	
	//! @name Initialisation
	//@{
	//! Prepare the event handler to receive events.
	void	Init();
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the @c EventTargetRef that this event handler is attached to.
	EventTargetRef	GetTarget() const	{ return (mTarget); }
	//@}
	
	//! @name Sending HICommands
	//@{
	//! Sends the HICommand given in @a inCommandID to the user input target.
	static void		IssueCommand(UInt32 inCommandID);
	//@}
	
	//! @name Functors
	//@{
	//! Adds a handler that will invoke @a inFunctor in response to a Carbon %Event of the given @a CLASS and @a KIND.
	template <UInt32 CLASS, UInt32 KIND>
	void	Add(
                boost::function1<bool, Event<CLASS, KIND>&> inFunctor);
	//! Adds a handler that will invoke @a inObject->*inFunction in response to a Carbon %Event of the given @a CLASS and @a KIND.
	template <UInt32 CLASS, UInt32 KIND, class T>
	void	Add(
				T*		inObject, 
				bool	(T::*inFunction)(Event<CLASS, KIND>&));
	//! Removes the handler for the Carbon %Event of the given @a CLASS and @a KIND.
	template <UInt32 CLASS, UInt32 KIND>
	void	Remove();
	//@}
	
private:
	
	// nested classes
	
	struct Hash : public std::unary_function<EventTypeSpec, size_t>
	{
		size_t	operator()(const EventTypeSpec& key) const
					{ return (key.eventClass | key.eventKind); }
	};

	struct Equal : public std::binary_function<EventTypeSpec, EventTypeSpec, bool>
	{
		bool	 operator()(const EventTypeSpec& x, const EventTypeSpec& y) const
					{ return ((x.eventClass == y.eventClass) && (x.eventKind == y.eventKind)); }
	};

	struct Less : public std::binary_function<EventTypeSpec, EventTypeSpec, bool>
	{
		bool	 operator()(const EventTypeSpec& x, const EventTypeSpec& y) const
					{ return ((x.eventClass < y.eventClass) || 
							  ((x.eventClass == y.eventClass) && (x.eventKind < y.eventKind))); }
	};
	
	// types
	
	typedef boost::intrusive_ptr<EventHandlerFunctorBase>	FunctorPtr;
	
#ifdef __MWERKS__
	typedef Metrowerks::hash_map<EventTypeSpec, FunctorPtr, Hash, Equal>	MapType;
#elif defined(__GNUC__)
	typedef __gnu_cxx::hash_map<EventTypeSpec, FunctorPtr, Hash, Equal>		MapType;
#else
	typedef std::map<EventTypeSpec, FunctorPtr, Less>	MapType;
#endif
	
	//! Adds a functor with a given Carbon %Event class & kind.
	void		AddFunctor(
					UInt32		inClass, 
					UInt32		inKind, 
					FunctorPtr	inFunctor);
	//! Removes a functor with a given Carbon %Event class & kind.
	void		RemoveFunctor(
					UInt32		inClass, 
					UInt32		inKind);
	//! Retrieves a functor a given Carbon %Event class & kind.
	EventHandlerFunctorBase*
                FindFunctor(
					UInt32		inClass, 
					UInt32		inKind) const;
	
	bool		Invoke(
					EventHandlerCallRef	inHandlerCallRef, 
					EventRef			inEvent) const;
	
	// callbacks
	static pascal OSStatus
					EventHandlerProc(
						EventHandlerCallRef	inHandlerCallRef, 
						EventRef			inEvent, 
						void*				inUserData);
	
	// member variables
	EventTargetRef	mTarget;			//!< The EventTargetRef that this event handler is attached to.
	EventHandlerRef	mEventHandlerRef;	//!< The underlying @c EventHandlerRef for this handler.
	MapType			mFunctorMap;
};

// ------------------------------------------------------------------------------------------
/*!	@param	CLASS	Template parameter.  Should be an integral four-char constant representing a Carbon %Event class.
	@param	KIND	Template parameter.  Should be an integral four-char constant representing a Carbon %Event kind in @a CLASS's namespace.
	@param	FUNCTOR	Template parameter.  Should be callable via <tt>bool operator () (EventHandlerCallRef inHandlerCallRef, EventRef inEvent)</tt>.
*/
template <UInt32 CLASS, UInt32 KIND> void
EventHandler::Add(
	boost::function1<bool, Event<CLASS, KIND>&> inFunctor)	//!< The functor that implements the Carbon %Event.
{
	AddFunctor(CLASS, KIND, FunctorPtr(new EventHandlerFunctor<CLASS, KIND>(inFunctor)));
}

// ------------------------------------------------------------------------------------------
/*!	@param	CLASS	Template parameter.  Should be an integral four-char constant representing a Carbon %Event class.
	@param	KIND	Template parameter.  Should be an integral four-char constant representing a Carbon %Event kind in @a CLASS's namespace.
	@param	T		Template parameter.  Should of class type.
*/
template <UInt32 CLASS, UInt32 KIND, class T> void
EventHandler::Add(
	T*		inObject,								//!< The object that implements the Carbon %Event.
	bool	(T::*inFunction)(Event<CLASS, KIND>&))	//!< The member function of @a T that implements the Carbon %Event.
{
	AddFunctor(CLASS, KIND, FunctorPtr(new EventHandlerFunctor<CLASS, KIND>(
                                                boost::bind(inFunction, inObject, _1))));
}

// ------------------------------------------------------------------------------------------
/*!	@param	CLASS	Template parameter.  Should be an integral four-char constant representing a Carbon %Event class.
	@param	KIND	Template parameter.  Should be an integral four-char constant representing a Carbon %Event kind in @a CLASS's namespace.
*/
template <UInt32 CLASS, UInt32 KIND> inline void
EventHandler::Remove()
{
	RemoveFunctor(CLASS, KIND);
}


}	// namespace B


#endif	// BEventHandler_H_
