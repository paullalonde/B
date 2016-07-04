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
#include "BEventTarget.h"

// standard headers
#include <map>

// B headers
#include "BAutoUPP.h"
#include "BEvent.h"
#include "BEventParams.h"


namespace
{
	struct ClassUnregisterer
	{
		typedef std::pair<B::String, HIObjectClassRef>	Value;
		
		void	operator () (const Value& value) const
				{
					OSStatus	err;
					
					err = HIObjectUnregisterClass(value.second);
					// ignore errors
				}
	};
}

namespace B {


// ==========================================================================================
//	EventTarget::Init

class EventTarget::Init
{
public:
	
	// constructor / destructor
	Init();
	~Init();
	
	// adding & removing
	void	Add(
				CFStringRef			inClassID, 
				HIObjectClassRef	inClassRef);
	void	Remove(
				CFStringRef			inClassID);
	HIObjectClassRef
			Find(
				CFStringRef			inClassID);
	
private:
	
	// types
	typedef std::map<String, HIObjectClassRef>	ClassMap;
	
	// member variables
	ClassMap	mRegisteredClasses;
};

// ------------------------------------------------------------------------------------------
EventTarget::Init::Init()
{
}

// ------------------------------------------------------------------------------------------
EventTarget::Init::~Init()
{
	std::for_each(mRegisteredClasses.begin(), mRegisteredClasses.end(), 
				  ClassUnregisterer());
}

// ------------------------------------------------------------------------------------------
void
EventTarget::Init::Add(
	CFStringRef			inClassID, 
	HIObjectClassRef	inClassRef)
{
	mRegisteredClasses.insert(ClassMap::value_type(String(inClassID), inClassRef));
}

// ------------------------------------------------------------------------------------------
void
EventTarget::Init::Remove(
	CFStringRef			inClassID)
{
	ClassMap::iterator	it	= mRegisteredClasses.find(String(inClassID));
	OSStatus			err;
	
	B_ASSERT(it != mRegisteredClasses.end());
	
	err = HIObjectUnregisterClass(it->second);
	// ignore errors
	
	mRegisteredClasses.erase(it);
}

// ------------------------------------------------------------------------------------------
HIObjectClassRef
EventTarget::Init::Find(
	CFStringRef			inClassID)
{
	ClassMap::iterator	it	= mRegisteredClasses.find(String(inClassID));
	
	return ((it != mRegisteredClasses.end()) ? it->second : NULL);
}


// ==========================================================================================
//	EventTarget

#pragma mark -

// static member variables
const CFStringRef	EventTarget::kHIObjectBaseClassID	= NULL;


// ------------------------------------------------------------------------------------------
EventTarget::Init&
EventTarget::GetInit()
{
	static Init sInit;
	
	return (sInit);
}

// ------------------------------------------------------------------------------------------
EventTarget::EventTarget(
	HIObjectRef	inObjectRef)	//!< The underlying toolbox object.
		: mObjectRef(inObjectRef)
{
}

// ------------------------------------------------------------------------------------------
EventTarget::~EventTarget()
{
}

// ------------------------------------------------------------------------------------------
/*!	We don't actually have a real implementation of @c delete, so call through to the 
	global version.
*/
void
EventTarget::operator delete (void* ptr)
{
	::operator delete (ptr);
}

// ------------------------------------------------------------------------------------------
/*!	@a inEvent is the same @c EventRef passed to @c HIObjectCreate() or 
	EventTarget::Create().
	@note If the object supports archiving, then Unarchive() will be called instead.
*/
void
EventTarget::Initialize(
	EventRef			/* inEvent */)	//!< The initialisation event.
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
bool
EventTarget::IsEqualTo(
	HIObjectRef inObjectRef)	//<! The @c HIObjectRef to compare with.
{
	return (mObjectRef == inObjectRef);
}

// ------------------------------------------------------------------------------------------
void
EventTarget::PrintDebugInfo()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
bool
EventTarget::IsAccessible() const
{
	return (!HIObjectIsAccessibilityIgnored(mObjectRef));
}

// ------------------------------------------------------------------------------------------
void
EventTarget::SetAccessible(bool inAccessible)
{
	OSStatus	err;
	
	err = HIObjectSetAccessibilityIgnored(mObjectRef, !inAccessible);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
EventTarget::SetAccessibilityAttribute(
	CFStringRef	inAttributeName, 
	CFTypeRef	inAttributeValue)
{
	SetAccessibilityAttribute(0, inAttributeName, inAttributeValue);
}

// ------------------------------------------------------------------------------------------
void
EventTarget::SetAccessibilityAttribute(
	UInt64		inIdentifier, 
	CFStringRef	inAttributeName, 
	CFTypeRef	inAttributeValue)
{
	OSStatus	err;
	
	err = HIObjectSetAuxiliaryAccessibilityAttribute(mObjectRef, inIdentifier, 
													 inAttributeName, inAttributeValue);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
EventTarget::IsArchivable() const
{
	return (!HIObjectIsArchivingIgnored(mObjectRef));
}

// ------------------------------------------------------------------------------------------
void
EventTarget::SetArchivable(bool inArchivable)
{
	OSStatus	err;
	
	err = HIObjectSetArchivingIgnored(mObjectRef, !inArchivable);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	@note If the object does not support archiving, then Initialize() will be called instead.
*/
void
EventTarget::Unarchive(HIArchiveRef /* ioArchive */)
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
EventTarget::Archive(HIArchiveRef /* ioArchive */) const
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
EventTarget::PrivateRegister(
	CFStringRef					inClassID,		//!< The class's ID.
	CFStringRef					inBaseClassID,	//!< The base class's ID.
	ConstructProcPtr			inConstructor)	//!< A function that will create instances of the class identified by @a inClassID.
{
	static AutoEventHandlerUPP	sEventHandler(EventHandlerProc);
	
	static const EventTypeSpec	kInitEvents[] = {
		{ kEventClassHIObject, kEventHIObjectConstruct		},
		{ kEventClassHIObject, kEventHIObjectInitialize		},
		{ kEventClassHIObject, kEventHIObjectDestruct		},
		{ kEventClassHIObject, kEventHIObjectIsEqual		},
		{ kEventClassHIObject, kEventHIObjectPrintDebugInfo	},
		{ kEventClassHIObject, kEventHIObjectEncode			},
	};
	
	HIObjectClassRef	classRef;
	OSStatus			err;
	
	err = HIObjectRegisterSubclass(inClassID, 
								   inBaseClassID, 
								   0, 
								   sEventHandler, 
								   GetEventTypeCount(kInitEvents), 
								   kInitEvents, 
								   (void *) inConstructor, 
								   &classRef);
	B_THROW_IF_STATUS(err);
	
	GetInit().Add(inClassID, classRef);
}

// ------------------------------------------------------------------------------------------
void
EventTarget::PrivateUnregister(
	CFStringRef					inClassID)		//!< The class's ID.
{
	GetInit().Remove(inClassID);
}

// ------------------------------------------------------------------------------------------
HIObjectClassRef
EventTarget::PrivateFindClassObject(
	CFStringRef					inClassID)		//!< The class's ID.
{
	return (GetInit().Find(inClassID));
}

// ------------------------------------------------------------------------------------------
HIObjectRef
EventTarget::PrivateCreate(
	CFStringRef					inClassID,		//!< The class's ID.
	EventRef					inEvent)		//!< The initialisation event.  This is passed to the newly created object.
{
	HIObjectRef	objRef;
	OSStatus	err;
	
	// If we got an initialisation event, use it;  else, create an empty one.
	
	if (inEvent != NULL)
	{
		err = HIObjectCreate(inClassID, inEvent, &objRef);
	}
	else
	{
		Event<kEventClassHIObject, kEventHIObjectInitialize>	event;
		
		err = HIObjectCreate(inClassID, event, &objRef);
	}
	
	B_THROW_IF_STATUS(err);
	
	return (objRef);
}

// ------------------------------------------------------------------------------------------
pascal OSStatus
EventTarget::EventHandlerProc(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent, 
	void*				inUserData)
{
	EventTarget*	obj	= reinterpret_cast<EventTarget*>(inUserData);
	OSStatus		err	= noErr;
	
	try
	{
		switch (GetEventKind(inEvent))
		{
		case kEventHIObjectConstruct:
			{
				ConstructProcPtr	proc	= (ConstructProcPtr) inUserData;
				HIObjectRef			objRef;
				
				objRef	= EventParamAndType<kEventParamHIObjectInstance, typeHIObjectRef>::Get(inEvent);
				obj		= (*proc)(objRef);
				
				EventParamAndType<kEventParamHIObjectInstance, typeVoidPtr>::Set(inEvent, obj);
			}
			break;

		case kEventHIObjectInitialize:
			err = CallNextEventHandler(inHandlerCallRef, inEvent);
			if (err == noErr)
			{
				bool	archived	= false;
				
				if (obj->IsArchivable())
				{
					OSPtr<CFTypeRef>	typeRef;
					
					if (EventParam<kEventParamHIArchive>::Get(inEvent, typeRef, std::nothrow))
					{
						HIArchiveRef	archive	= reinterpret_cast<HIArchiveRef>(const_cast<void*>(typeRef.get()));
						
						obj->Unarchive(archive);
						archived = true;
					}
				}
				
				if (!archived)
				{
					obj->Initialize(inEvent);
				}
			}
			break;
			
		case kEventHIObjectDestruct:
			delete obj;
			err = eventNotHandledErr;
			break;

		case kEventHIObjectIsEqual:
			{
				HIObjectRef	otherObj	= EventDirectParam<typeHIObjectRef>::Get(inEvent);
				bool		equals		= obj->IsEqualTo(otherObj);
				
				EventParamAndType<kEventParamResult, typeBoolean>::Set(inEvent, equals);
			}
			break;
			
		case kEventHIObjectPrintDebugInfo:
			obj->PrintDebugInfo();
			break;
			
		case kEventHIObjectEncode:
			err = CallNextEventHandler(inHandlerCallRef, inEvent);
			if (err == noErr)
			{
				OSPtr<CFTypeRef>	typeRef	= EventParam<kEventParamHIArchive>::Get(inEvent);
				HIArchiveRef		archive	= reinterpret_cast<HIArchiveRef>(const_cast<void*>(typeRef.get()));
				
				obj->Archive(archive);
			}
			break;
			
		default:
			err = eventNotHandledErr;
			break;
		}
	}
	catch (std::exception& ex)
	{
		err = EventBase::StoreExceptionIntoCarbonEvent(inEvent, ex);
	}
	catch (...)
	{
		err = eventInternalErr;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
template <> EventTargetRef
EventTarget::GetEventTarget(WindowRef inObj)
{
	return HIObjectGetEventTarget(reinterpret_cast<HIObjectRef>(inObj));
}

// ------------------------------------------------------------------------------------------
template <> EventTargetRef
EventTarget::GetEventTarget(HIViewRef inObj)
{
	return HIViewGetEventTarget(inObj);
}

// ------------------------------------------------------------------------------------------
template <> EventTargetRef
EventTarget::GetEventTarget(HIObjectRef inObj)
{
	return HIObjectGetEventTarget(inObj);
}

// ------------------------------------------------------------------------------------------
template <> EventTargetRef
EventTarget::GetEventTarget(EventTargetRef inObj)
{
	return inObj;
}


}	// namespace B
