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
#include "BEventHandler.h"

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BAutoUPP.h"
#include "BEvent.h"
#include "BUtility.h"


namespace B {

// ==========================================================================================
//  EventHandler

// ------------------------------------------------------------------------------------------
EventHandler::EventHandler(
	EventTargetRef	inTarget)
		: mTarget(inTarget), mEventHandlerRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
EventHandler::EventHandler(
	HIObjectRef		inTarget)
		: mTarget(HIObjectGetEventTarget(inTarget)), mEventHandlerRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
EventHandler::EventHandler(
	HIViewRef		inTarget)
		: mTarget(GetControlEventTarget(inTarget)), mEventHandlerRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
EventHandler::EventHandler(
	MenuRef			inTarget)
		: mTarget(GetMenuEventTarget(inTarget)), mEventHandlerRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
EventHandler::EventHandler(
	WindowRef		inTarget)
		: mTarget(GetWindowEventTarget(inTarget)), mEventHandlerRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
EventHandler::~EventHandler()
{
	OSStatus	err;
	
	if (mEventHandlerRef != NULL)
	{
		err = RemoveEventHandler(mEventHandlerRef);
	}
}

// ------------------------------------------------------------------------------------------
void
EventHandler::Init()
{
	OSStatus	err;
	
	try
	{
		static AutoEventHandlerUPP	sEventHandlerUPP(EventHandlerProc);
		
		std::vector<EventTypeSpec>	specs;
		
		specs.reserve(mFunctorMap.size());
		std::transform(mFunctorMap.begin(), mFunctorMap.end(), 
					   std::back_inserter(specs), 
					   MapKey<MapType::key_type, MapType::mapped_type>());
		
		if (specs.size() > 0)
		{
			err = InstallEventHandler(mTarget, 
									  sEventHandlerUPP, 
									  specs.size(), 
									  &specs[0], 
									  reinterpret_cast<void*>(this), 
									  &mEventHandlerRef);
			B_THROW_IF_STATUS(err);
		}
	}
	catch (...)
	{
		if (mEventHandlerRef != NULL)
		{
			err = RemoveEventHandler(mEventHandlerRef);
			mEventHandlerRef = NULL;
		}
		
		throw;
	}
}

// ------------------------------------------------------------------------------------------
void
EventHandler::IssueCommand(
	UInt32		inCommandID)
{
	HICommand	cmd;
	OSStatus	err;
	
	BlockZero(&cmd, sizeof(cmd));
	cmd.commandID = inCommandID;
	
	err = ProcessHICommand(&cmd);
///	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
EventHandler::AddFunctor(
	UInt32		inClass,	//!< The Carbon %Event class of the functor to remove.
	UInt32		inKind,		//!< The Carbon %Event kind of the functor to remove.
	FunctorPtr	inFunctor)	//!< The functor object.
{
	EventTypeSpec	spec	= { inClass, inKind };
	
	if (mEventHandlerRef != NULL)
	{
		OSStatus	err;
		
		err = AddEventTypesToHandler(mEventHandlerRef, 1, &spec);
		B_THROW_IF_STATUS(err);
	}
	
	mFunctorMap.insert(MapType::value_type(spec, inFunctor));
}

// ------------------------------------------------------------------------------------------
void
EventHandler::RemoveFunctor(
	UInt32		inClass,	//!< The Carbon %Event class of the functor to remove.
	UInt32		inKind)		//!< The Carbon %Event kind of the functor to remove.
{
	EventTypeSpec	spec	= { inClass, inKind };
	
	if (mEventHandlerRef != NULL)
	{
		OSStatus	err;
		
		err = RemoveEventTypesFromHandler(mEventHandlerRef, 1, &spec);
		B_THROW_IF_STATUS(err);
	}
	
	mFunctorMap.erase(spec);
}

// ------------------------------------------------------------------------------------------
/*!	@return	The found functor, or @c NULL.
*/
EventHandlerFunctorBase*
EventHandler::FindFunctor(
	UInt32	inClass,	//!< The Carbon %Event class of the functor to retrieve.
	UInt32	inKind)		//!< The Carbon %Event kind of the functor to retrieve.
	const
{
	EventTypeSpec	spec	= { inClass, inKind };
	
	B_ASSERT(mFunctorMap.find(spec) != mFunctorMap.end());
	
	return (mFunctorMap.find(spec)->second.get());
}

// ------------------------------------------------------------------------------------------
bool
EventHandler::Invoke(
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef			inEvent) const
{
	EventHandlerFunctorBase*	base	= FindFunctor(GetEventClass(inEvent), GetEventKind(inEvent));
	
	return ((*base)(inHandlerCallRef, inEvent));
}

// ------------------------------------------------------------------------------------------
pascal OSStatus
EventHandler::EventHandlerProc(
	EventHandlerCallRef		inHandlerCallRef, 
	EventRef				inEvent, 
	void*					inUserData)
{
	EventHandler*	target	= reinterpret_cast<EventHandler*>(inUserData);
	OSStatus		err;
	
	try
	{
		if (target->Invoke(inHandlerCallRef, inEvent))
			err = noErr;
		else
			err = eventNotHandledErr;
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


// ==========================================================================================
//  EventHandlerFunctorBase

#pragma mark -

// ------------------------------------------------------------------------------------------
EventHandlerFunctorBase::~EventHandlerFunctorBase()
{
}


// ==========================================================================================
//  Global Functions

#pragma mark -

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_add_ref(EventHandlerFunctorBase* f)
{
	B_ASSERT(f != NULL);
	
	IncrementAtomic(&f->mRefCount);
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_release(EventHandlerFunctorBase* f)
{
	B_ASSERT(f != NULL);
	B_ASSERT(f->mRefCount > 0);
	
	if (DecrementAtomic(&f->mRefCount) == 1)
	{
		// The ref count is now zero
		delete f;
	}
}


}	// namespace B
