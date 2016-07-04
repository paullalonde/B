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

#ifndef BEventTarget_H_
#define BEventTarget_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/concept_check.hpp>
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"
#include "BOSPtr.h"


namespace B {


// forward declarations
class	EventTarget;


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//	EventTargetConcept

#pragma mark EventTargetConcept

/*!
	@brief	Concept check for classes that need to be registerable as custom event targets.
*/
template <class T> struct EventTargetConcept
{
	typedef EventTarget*	(*InstantiationProc)(HIObjectRef);
	
	void	constraints()
	{
		mClassID			= &T::kHIObjectClassID;
		mBaseClassID		= &T::kHIObjectBaseClassID;
		mInstantiationProc	= T::Instantiate;
	}
	
	const CFStringRef*	mClassID;
	const CFStringRef*	mBaseClassID;
	InstantiationProc	mInstantiationProc;
};

#endif	// DOXYGEN_SKIP


// ==========================================================================================
//	EventTarget

#pragma mark -
#pragma mark EventTarget

/*!
	@brief	Base class for custom derivatives of HIObject.
	
	Once an EventTarget is instantiated, its lifetime is tied to that of its underlying 
	@c HIObjectRef.  Its lifetime should be controlled using the normal CF mechanisms 
	(@c CFRetain & @c CFRelease) rather than @c operator @c delete.
	
	@ingroup	CarbonEvents
*/
class EventTarget : public boost::noncopyable
{
public:
	
	//! @name Class Registration
	//@{
	//! Registers class @a T (which must derive from EventTarget) with the HIToolbox.
	template <class T> static void	Register();
	//! Unregisters class @a T (which must derive from EventTarget) from the HIToolbox.
	template <class T> static void	Unregister();
	//! Returns the object class associated with type @a T (which must derive from EventTarget), or @c NULL.
	template <class T> static HIObjectClassRef	GetObjectClass();
	//@}
	
	//! @name Object Creation
	//@{
	//! Creates an instance of class @a T.
	template <class T> static OSPtr<HIObjectRef>	Create(EventRef inEvent = NULL);
	//! Creates an instance of class @a inClassID.
	static OSPtr<HIObjectRef>	Create(CFStringRef inClassID, EventRef inEvent = NULL);
	//@}
	
	//! @name Inquiries
	//@{
	//! Retrieves the C++ object of type @a T associated with the @c HIObjectRef @a objRef.
	template <class T> static T*	InstanceData(HIObjectRef objRef);
	//! Is the object accessible?
	bool			IsAccessible() const;
	//! Is the object archivable?
	bool			IsArchivable() const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Make the object archivable.
	void			SetAccessible(
						bool		inAccessible);
	//! Set a non-user-modifiable accessibility attribute.
	void			SetAccessibilityAttribute(
						CFStringRef	inAttributeName, 
						CFTypeRef	inAttributeValue);
	//! Set a non-user-modifiable accessibility attribute.
	void			SetAccessibilityAttribute(
						UInt64		inIdentifier, 
						CFStringRef	inAttributeName, 
						CFTypeRef	inAttributeValue);
	//! Make the object archivable.
	void			SetArchivable(
						bool		inArchivable);
	//@}
	
	//! @name Conversions
	//@{
	//! Retrieves the @a HIObjectRef associated with this C++ object.
	operator		HIObjectRef () const	{ return (mObjectRef); }
	//! Retrieves the @a HIObjectRef associated with this C++ object.
	HIObjectRef		GetObjectRef () const	{ return (mObjectRef); }
	//! Retrieves the underlying @c EventTargetRef.
	EventTargetRef	GetEventTarget() const	{ return (HIObjectGetEventTarget(mObjectRef)); }
	//! Retrieves the @a EventTargetRef associated with @a inObject.
	template <typename T>
	static EventTargetRef	GetEventTarget(T inObject);
	//@}
	
	//! The default base class ID, representing @c HIObject.
	static const CFStringRef	kHIObjectBaseClassID;
	
protected:
	
	//! @name Types
	//@{
	//! Object construction callback.
	typedef EventTarget*	(*ConstructProcPtr)(HIObjectRef inObjectRef);
	//@}
	
	//! @name Class Registration / Unregistration / Instantiation
	//@{
	//! Registers class @a inClassID.
	static void	PrivateRegister(
					CFStringRef			inClassID, 
					CFStringRef			inBaseClassID, 
					ConstructProcPtr	inConstructor);
	//! Unregisters class @a inClassID.
	static void	PrivateUnregister(
					CFStringRef			inClassID);
	//! Returns the object class associated with type @a inClassID, or @c NULL.
	static HIObjectClassRef
				PrivateFindClassObject(
					CFStringRef			inClassID);
	//! Creates an instance of class @a inClassID.
	static HIObjectRef
				PrivateCreate(
					CFStringRef			inClassID, 
					EventRef			inEvent);
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Constructor.
			EventTarget(HIObjectRef inObjectRef);
	//! Destructor.
	virtual	~EventTarget();
	//@}
	
	//! @name Free Store
	//@{
	//! Protected delete prevents external clients from deleting instances directly.
	void	operator delete (void* ptr);
	//@}
	
	//! @name Handling Events
	//@{
	//! Override to extract initialisation arguments from @a inEvent.
	virtual void	Initialize(EventRef inEvent);
	//! Returns @c true if @a this is "equal" to @a inObjectRef.
	virtual bool	IsEqualTo(HIObjectRef inObjectRef);
	//! Override to print debugging information to @c stdout.
	virtual void	PrintDebugInfo();
	//! Decode the object into the given archive.  This is called instead of Initialize().
	virtual void	Unarchive(HIArchiveRef ioArchive);
	//! Encode the object into the given archive.
	virtual void	Archive(HIArchiveRef ioArchive) const;
	//@}
	
	//! The underlying toolbox object.
	const HIObjectRef	mObjectRef;
	
private:
	
	// nested classes
	class	Init;
	
	static Init&	GetInit();
	
	// callbacks
	static pascal OSStatus
					EventHandlerProc(
						EventHandlerCallRef	inHandlerCallRef, 
						EventRef			inEvent, 
						void*				inUserData);
	
	// friends
	friend class	Init;
};

// ------------------------------------------------------------------------------------------
/*!	The template parameter @a T must have the following characterstics:
		
	- It must contain a static member variable of type @c CFStringRef called 
	  @c kHIObjectClassID.  This variable contains the class's unique identifier.
	- It may contain a static member variable of type @c CFStringRef called 
	  @c kHIObjectBaseClassID.  This variable contains the base class's unique 
	  identifier.  If this variable isn't present, a default value defined in 
	  EventTarget is used, in which case @a T is assumed to derived directly 
	  from @c HIObject.
	- It must contain a public static member function called @c Instantiate which 
	  has the same signature as @c ConstructProcPtr.  The function needs to instantiate 
	  a @a T and return it as an EventTarget.
		
	@param	T	Template parameter.  Should be a class that derives from EventTarget.
*/
template <class T> inline void
EventTarget::Register()
{
	boost::function_requires< EventTargetConcept<T> >();
	
	PrivateRegister(T::kHIObjectClassID, 
					T::kHIObjectBaseClassID, 
					T::Instantiate);
}

// ------------------------------------------------------------------------------------------
/*!	The template parameter @a T must have the following characterstics:
		
	- It must contain a static member variable of type @c CFStringRef called 
	  @c kHIObjectClassID.  This variable contains the class's unique identifier.
	
	@param	T	Template parameter.  Should be a class that derives from EventTarget.
*/
template <class T> inline void
EventTarget::Unregister()
{
	boost::function_requires< EventTargetConcept<T> >();
	
	PrivateRegister(T::kHIObjectClassID);
}

// ------------------------------------------------------------------------------------------
/*!	The template parameter @a T must have the following characterstics:
		
	- It must contain a static member variable of type @c CFStringRef called 
	  @c kHIObjectClassID.  This variable contains the class's unique identifier.
	
	@param	T	Template parameter.  Should be a class that derives from EventTarget.
*/
template <class T> inline HIObjectClassRef
EventTarget::GetObjectClass()
{
	boost::function_requires< EventTargetConcept<T> >();
	
	return (PrivateFindClassObject(T::kHIObjectClassID));
}

// ------------------------------------------------------------------------------------------
/*!	The template parameter @a T must have the following characterstics:
		
	- It must contain a static member variable of type @c CFStringRef called 
	  @c kHIObjectClassID.  This variable contains the class's unique identifier.
	
	@param	T	Template parameter.  Should be a class that derives from EventTarget.
*/
template <class T> inline OSPtr<HIObjectRef>
EventTarget::Create(
	EventRef	inEvent /* = NULL */)	//!< Initialisation event for the new object.
{
	boost::function_requires< EventTargetConcept<T> >();
	
	return (OSPtr<HIObjectRef>(PrivateCreate(T::kHIObjectClassID, inEvent), from_copy));
}

// ------------------------------------------------------------------------------------------
inline OSPtr<HIObjectRef>
EventTarget::Create(
	CFStringRef	inClassID,
	EventRef	inEvent /* = NULL */)	//!< Initialisation event for the new object.
{
	return (OSPtr<HIObjectRef>(PrivateCreate(inClassID, inEvent), from_copy));
}

// ------------------------------------------------------------------------------------------
/*!	The template parameter @a T must have the following characterstics:
	
	- It must contain a static member variable of type @c CFStringRef called 
	  @c kHIObjectClassID.  This variable contains the class's unique identifier.
	
	@param	T	Template parameter.  Must be a class that derives from EventTarget.
	@return		A @a T* if @a objRef is of the proper type, else @c NULL.
*/
template <class T> inline T*
EventTarget::InstanceData(HIObjectRef objRef)
{
	boost::function_requires< EventTargetConcept<T> >();
	
	void*			data	= HIObjectDynamicCast(objRef, T::kHIObjectClassID);
	EventTarget*	target	= reinterpret_cast<EventTarget*>(data);
	T*				obj		= dynamic_cast<T*>(target);
	
	return (obj);
}


// ==========================================================================================
//	EventTargetRegistrar

/*!
	@brief	Utility class for automagically registering derivatives of EventTarget.
	
	The constructor of objects of this class register the class of template parameter T.
	
	@ingroup	CarbonEvents
*/
template <class T> class EventTargetRegistrar
{
public:

#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	T;		//!< The template parameter.  Must be a class deriving from EventTarget.
	//@}
#endif
	
	//! Constructor.  Registers class @a T.
	EventTargetRegistrar()	{ EventTarget::Register<T>(); }
};


}	// namespace B

#endif	// BEventTarget_H_
