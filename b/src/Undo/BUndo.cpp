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
#include "BUndo.h"

// B headers
#include "BEvent.h"


namespace B {

// ==========================================================================================
//	Undo

// ------------------------------------------------------------------------------------------
void
Undo::BeginGroup(
	const String&	inName /* = String() */,	//!< If empty, a default name is used.
	EventTargetRef	inTarget /* = NULL */)		//!< If @c NULL, the request is sent to the user input focus.
{
	if (inTarget == NULL)
	{
		inTarget = GetUserFocusEventTarget();
	}
	
	Event<kEventClassB, kEventBUndoBegin>	event(inName);
	
	event.Send(inTarget);
}

// ------------------------------------------------------------------------------------------
void
Undo::CommitGroup(
	EventTargetRef	inTarget /* = NULL */)		//!< If @c NULL, the request is sent to the user input focus.
{
	if (inTarget == NULL)
	{
		inTarget = GetUserFocusEventTarget();
	}
	
	Event<kEventClassB, kEventBUndoCommit>	event;
	
	event.Send(inTarget);
}

// ------------------------------------------------------------------------------------------
void
Undo::AbortGroup(
	EventTargetRef	inTarget /* = NULL */)		//!< If @c NULL, the request is sent to the user input focus.
{
	if (inTarget == NULL)
	{
		inTarget = GetUserFocusEventTarget();
	}
	
	Event<kEventClassB, kEventBUndoAbort>	event;
	
	event.Send(inTarget);
}

// ------------------------------------------------------------------------------------------
/*	Each call with @a inEnable = @c false prevents undoable actions from being added 
	to the open action group;  each call with @a inEnable = @c true allows it to 
	happen again.  Each call to Enable(false) must be balanced with a call to 
	Enable(true).
*/
void
Undo::Enable(
	bool			inEnable,					//!< Indicates whether to enable or disable the undo mechanism.
	EventTargetRef	inTarget /* = NULL */)		//!< If @c NULL, the request is sent to the user input focus.
{
	if (inTarget == NULL)
	{
		inTarget = GetUserFocusEventTarget();
	}
	
	Event<kEventClassB, kEventBUndoEnable>	event(inEnable);
	
	event.Send(inTarget);
}

// ------------------------------------------------------------------------------------------
void
Undo::Add(
	FunctorType				inFunctor,				//!< A functor taking no arguments and returning no result.
	EventTargetRef			inTarget /* = NULL */)	//!< If @c NULL, the request is sent to the user input focus.
{
	PrivateAdd(std::auto_ptr<UndoAction>(new FunctorUndoAction(inFunctor)), inTarget);
}

// ------------------------------------------------------------------------------------------
void
Undo::PrivateAdd(
	std::auto_ptr<UndoAction>	inUndoAction, 
	EventTargetRef				inTarget)
{
	if (inTarget == NULL)
	{
		inTarget = GetUserFocusEventTarget();
	}
	
	Event<kEventClassB, kEventBUndoAdd>	event(inUndoAction.get());
	
	event.Send(inTarget);
	
	// If we've gotten here, it's because the Send was succesful, meaning that the 
	// UNDO_POLICY acquired the ownership of the undo action.  So we need to release 
	// it here, since it's no longer ours.
	
	inUndoAction.release();
}


// ==========================================================================================
//	AutoUndo

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Opens an action group called @a inName.  All requests sent to this object are directed 
	towards @a inTarget.
*/
AutoUndo::AutoUndo(
	const String&	inName /* = String() */,	//!< If empty, a default name is used.
	EventTargetRef	inTarget /* = NULL */)		//!< If @c NULL, the user input focus is used.
		: mTarget(inTarget), mCommitted(false)
{
	if (mTarget == NULL)
	{
		mTarget = GetUserFocusEventTarget();
	}
	
	Undo::BeginGroup(inName, mTarget);
}

// ------------------------------------------------------------------------------------------
/*!	Aborts the group if it has not been committed already.
*/
AutoUndo::~AutoUndo()
{
	if (!mCommitted)
	{
		try
		{
			Undo::AbortGroup(mTarget);
		}
		catch (...)
		{
			// It's very bad to throw from a destructor, so just catch any exceptions here.
		}
	}
}

// ------------------------------------------------------------------------------------------
void
AutoUndo::Commit()
{
	if (!mCommitted)
	{
		Undo::CommitGroup(mTarget);
		mCommitted = true;
	}
}

// ------------------------------------------------------------------------------------------
void
AutoUndo::Add(
	FunctorType					inFunctor)	//!< A functor.
{
	Undo::Add(inFunctor, mTarget);
}


// ==========================================================================================
//	AutoDisableUndo

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	The enable/disable requests are sent to the UNDO_POLICY identified by @a inTarget.
*/
AutoDisableUndo::AutoDisableUndo(
	bool			inDisable /* = true */,	//!< Indicates whether to enable or disable the undo mechanism.
	EventTargetRef	inTarget /* = NULL */)	//!< If @c NULL, the request is sent to the user input focus.
	: mTarget(inTarget), mDisable(inDisable)
{
	if (mTarget == NULL)
	{
		mTarget = GetUserFocusEventTarget();
	}
	
	Undo::Enable(!mDisable, mTarget);
}

// ------------------------------------------------------------------------------------------
AutoDisableUndo::~AutoDisableUndo()
{
	try
	{
		Undo::Enable(mDisable, mTarget);
	}
	catch (...)
	{
		// It's very bad to throw from a destructor, so just catch any exceptions here.
	}
}

}	// namespace B
