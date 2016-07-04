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

#ifndef BUndo_H_
#define BUndo_H_

#pragma once

// standard headers
#include <memory>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BString.h"
#include "BUndoAction.h"


namespace B {

// forward declarations
class	AEObjectSupport;
class	CommandData;


// ==========================================================================================
//	Undo

/*!	@brief	Client interface to the Undo mechanism.
	
	All member functions take an @c EventTargetRef as an argument.  Because implementations 
	of UNDO_POLICY are attached to an @c EventTargetRef, this argument serves to direct 
	requests to a given UNDO_POLICY implementation.  By default, the request is sent to 
	the user input focus, and travels up the handler chain from there.
	
	@sa			@ref using_undo, Undo, UNDO_POLICY
	@ingroup	UndoGroup
*/
class Undo : public boost::noncopyable
{
public:
	
	//! @name Types
	//@{
	//! Shorthand for FunctorUndoAction's functor type.
	typedef FunctorUndoAction::FunctorType	FunctorType;
	//@}
	
	/*!	@name Action Groups
		
		Action groups allow any number of undoable actions to be committed or aborted as 
		a unit.  This is important in order to make the accumulation of actions resilient 
		in the presence of exceptions.
	*/
	//@{
	//! Opens an action group called @a inName.
	static void	BeginGroup(
					const String&	inName = String(), 
					EventTargetRef	inTarget = NULL);
	//! Commits the currently open action group.
	static void	CommitGroup(
					EventTargetRef	inTarget = NULL);
	//! Aborts the currently open action group.
	static void	AbortGroup(
					EventTargetRef	inTarget = NULL);
	//@}
	
	/*! @name Adding Actions
		
		These functions add a single undoable action to the currently open group 
		of undoable actions.  If no group is open, a new group with a default name 
		is opened.
	*/
	//@{
	//! Adds @a inUndoAction to the currently open action group.
	template <class ACTION>
	static void	Add(
					std::auto_ptr<ACTION>	inUndoAction, 
					EventTargetRef			inTarget = NULL);
	//! Adds @a inFunctor to the currently open action group.
	static void	Add(
					FunctorType				inFunctor, 
					EventTargetRef			inTarget = NULL);
	//! DEPRECATED.  Adds the adapable unary functor @a inFunctor to the currently open action group.
	template <class FUNCTOR>
	static void	Add(
					FUNCTOR									inFunctor,
					typename FUNCTOR::argument_type			inArg,
					EventTargetRef							inTarget = NULL);
	//! DEPRECATED.  Adds the adapable binary functor @a inFunctor to the currently open action group.
	template <class FUNCTOR>
	static void	Add(
					FUNCTOR									inFunctor,
					typename FUNCTOR::first_argument_type	inArg,
					typename FUNCTOR::second_argument_type	inArg2,
					EventTargetRef							inTarget = NULL);
	//@}
	
	//! @name Modifiers
	//@{
	//! Enables or disables the undo mechanism.
	static void	Enable(
					bool			inEnable, 
					EventTargetRef	inTarget = NULL);
	//@}
	
private:
	
	static void	PrivateAdd(
					std::auto_ptr<UndoAction>	inUndoAction, 
					EventTargetRef				inTarget);
	
	// illegal operations
	Undo();
};

// ------------------------------------------------------------------------------------------
/*!	Note that the action is wrapped in an @c std::auto_ptr.  This means that ownership of 
	@a inUndoAction is tranferred to this function;  it will either be added succesfully 
	to the currently open action group, or will be deleted.
*/
template <class ACTION> void
Undo::Add(
	std::auto_ptr<ACTION>	inUndoAction,			//!< The undoable action, wrapped inside an @c std::auto_ptr.
	EventTargetRef			inTarget /* = NULL */)	//!< If @c NULL, the request is sent to the user input focus.
{
	PrivateAdd(std::auto_ptr<UndoAction>(inUndoAction.release()), inTarget);
}

// ------------------------------------------------------------------------------------------
/*!	@a FUNCTOR must be an adaptable unary function object.  Its <tt>operator ()</tt> must 
	take one argument of type @a FUNCTOR::argument_type.  Its result is discarded, so a 
	return type of @c void is acceptable.  The value of the argument is given in 
	@a inArg.
	
	@deprecated	This function will be removed in a future release.  Clients can replace 
				calls to this function with:
				@code Add(Undo::FunctorType(boost::bind(inFunctor, inArg)), inTarget) @endcode
*/
template <class FUNCTOR> void
Undo::Add(
	FUNCTOR							inFunctor,				//!< An adaptable unary function object.
	typename FUNCTOR::argument_type	inArg,					//!< The argument to be passed to @a inFunctor.
	EventTargetRef					inTarget /* = NULL */)	//!< If @c NULL, the request is sent to the user input focus.
{
	Add(FunctorType(boost::bind(inFunctor, inArg)), inTarget);
}

// ------------------------------------------------------------------------------------------
/*!	@a FUNCTOR must be an adaptable binnary function object.  Its <tt>operator ()</tt> must 
	take two arguments; the first of type @a FUNCTOR::first_argument_type and the second of 
	type @a FUNCTOR::second_argument_type.  Its result is discarded, so a return type of 
	@c void is acceptable.  The values of the arguments are given in @a inArg1 and 
	@a inArg2, respectively.
	
	@deprecated	This function will be removed in a future release.  Clients can replace 
				calls to this function with:
				@code Add(Undo::FunctorType(boost::bind(inFunctor, inArg1, inArg2)), inTarget) @endcode
*/
template <class FUNCTOR> void
Undo::Add(
	FUNCTOR									inFunctor,				//!< An adaptable unary function object.
	typename FUNCTOR::first_argument_type	inArg1,					//!< The first argument to be passed to @a inFunctor.
	typename FUNCTOR::second_argument_type	inArg2,					//!< The second argument to be passed to @a inFunctor.
	EventTargetRef							inTarget /* = NULL */)	//!< If @c NULL, the request is sent to the user input focus.
{
	Add(FunctorType(boost::bind(inFunctor, inArg1, inArg2)), inTarget);
}


// ==========================================================================================
//	AutoUndo

#pragma mark -

/*!
	@brief	Automatic management of undo action groups.
	
	The class' constructor opens an action group.  Once open, the group can be closed in 
	two ways.  The normal way is to call the Commit() member function, which as the name 
	implies commits the group.  The other way to close the group is to destroy the 
	AutoUndo object without having called Commit();  in this case, the object notices 
	that Commit() hasn't been called and so calls Abort() instead.  This means that an 
	exception propagating through the scope where the AutoUndo object was declared will 
	automatically end up abort the group.
	
	An example will show how simple this is in practice:
	
	@code
	MyAction()
	{
		// Constructing the AutoUndo object will open a group.
		
		B::AutoUndo	autoUndo(B::String("Defrobilisation"));
		
		// do stuff...
		
		autoUndo.Add( ... );
		
		// do more stuff...
		
		autoUndo.Commit();
		
		// Because we committed, destroying the AutoUndo object won't do anything.
		// If on the other an exception had been thrown prior to the Commit() call, 
		// then the group would have been aborted automatically.
	}
	@endcode
	
	@sa			@ref using_undo, Undo
	@ingroup	UndoGroup
*/
class AutoUndo
{
public:
	
	//! Shorthand for FunctorUndoAction's functor type.
	typedef FunctorUndoAction::FunctorType	FunctorType;
	
	//! Constructor.
	AutoUndo(
		const String&			inName = String(), 
		EventTargetRef			inTarget = NULL);
	//! Constructor that adds an initial @a inUndoAction.
	template <class ACTION>
	AutoUndo(
		std::auto_ptr<ACTION>	inUndoAction, 
		EventTargetRef			inTarget = NULL);
	
	//! Destructor.
	~AutoUndo();
	
	//! Commits the currently open action group.
	void	Commit();
	
	//! Adds @a inUndoAction to the currently open action group.
	template <class ACTION>
	void	Add(
				std::auto_ptr<ACTION>	inUndoAction);
	
	//! Adds @a inFunctor to the currently open action group.
	void	Add(
				FunctorType				inFunctor);
	
	//! DEPRECATED.  Adds @a inFunctor to the currently open action group.
	template <class FUNCTOR>
	void	Add(
				FUNCTOR							inFunctor, 
				typename FUNCTOR::argument_type	inArg);
	
	//! DEPRECATED.  Adds @a inFunctor to the currently open action group.
	template <class FUNCTOR>
	void	Add(
				FUNCTOR									inFunctor, 
				typename FUNCTOR::first_argument_type	inArg1, 
				typename FUNCTOR::second_argument_type	inArg2);
	
private:
	
	// member variables
	EventTargetRef	mTarget;
	bool			mCommitted;
};

// ------------------------------------------------------------------------------------------
/*!	Opens an action group named after @a inUndoAction.  All requests sent to this object 
	are directed towards @a inTarget.
*/
template <class ACTION>
AutoUndo::AutoUndo(
	std::auto_ptr<ACTION>	inUndoAction,			//!< The initial undo action.
	EventTargetRef			inTarget /* = NULL */)	//!< If @c NULL, the user input focus is used.
		: mTarget(inTarget), mCommitted(false)
{
	if (mTarget == NULL)
	{
		mTarget = GetUserFocusEventTarget();
	}
	
	Undo::BeginGroup(inUndoAction->GetActionName(), mTarget);
	Undo::Add<ACTION>(inUndoAction, mTarget);
}

// ------------------------------------------------------------------------------------------
/*!	Note that the action is wrapped in an @c std::auto_ptr.  This means that ownership of 
	@a inUndoAction is tranferred to this function;  it will either be added succesfully 
	to the currently open action group, or will be deleted.
*/
template <class ACTION> inline void
AutoUndo::Add(
	std::auto_ptr<ACTION>	inUndoAction)	//!< The undoable action, wrapped inside an @c std::auto_ptr.
{
	Undo::Add<ACTION>(inUndoAction, mTarget);
}

// ------------------------------------------------------------------------------------------
/*!	@a FUNCTOR must be an adaptable unary function object.  Its <tt>operator ()</tt> must 
	take one argument of type @a FUNCTOR::argument_type.  Its result is discarded, so a 
	return type of @c void is acceptable.  The value of the argument is given in 
	@a inArg.
	
	@deprecated	This function will be removed in a future release.  Clients can replace 
				calls to this function with:
				@code Add(AutoUndo::FunctorType(boost::bind(inFunctor, inArg)), inTarget) @endcode
*/
template <class FUNCTOR> inline void
AutoUndo::Add(
	FUNCTOR							inFunctor,	//!< An adaptable unary function object.
	typename FUNCTOR::argument_type	inArg)		//!< The argument to be passed to @a inFunctor.
{
	Undo::Add<FUNCTOR>(inFunctor, inArg, mTarget);
}

// ------------------------------------------------------------------------------------------
/*!	@a FUNCTOR must be an adaptable binnary function object.  Its <tt>operator ()</tt> must 
	take two arguments; the first of type @a FUNCTOR::first_argument_type and the second of 
	type @a FUNCTOR::second_argument_type.  Its result is discarded, so a return type of 
	@c void is acceptable.  The values of the arguments are given in @a inArg1 and 
	@a inArg2, respectively.
	
	@deprecated	This function will be removed in a future release.  Clients can replace 
				calls to this function with:
				@code Add(AutoUndo::FunctorType(boost::bind(inFunctor, inArg1, inArg2)), inTarget) @endcode
*/
template <class FUNCTOR> inline void
AutoUndo::Add(
	FUNCTOR									inFunctor,	//!< An adaptable unary function object.
	typename FUNCTOR::first_argument_type	inArg1,		//!< The first argument to be passed to @a inFunctor.
	typename FUNCTOR::second_argument_type	inArg2)		//!< The second argument to be passed to @a inFunctor.
{
	Undo::Add<FUNCTOR>(inFunctor, inArg1, inArg2, mTarget);
}


// ==========================================================================================
//	AutoDisableUndo

#pragma mark -

/*!	@brief	Automatic enabling/disabling of undo mechanism.
	
	This class changes an UNDO_POLICY's enabled state in its constructor, and restores 
	it in its destructor.
	
	@sa			@ref using_undo, Undo
	@ingroup	UndoGroup
*/
class AutoDisableUndo
{
public:
	
	//! Constructor.
	AutoDisableUndo(bool inDisable = true, EventTargetRef inTarget = NULL);

	//! Destructor.
	~AutoDisableUndo();
	
private:
	
	// member variables
	EventTargetRef	mTarget;
	bool			mDisable;
};


}	// namespace B


#endif	// BUndo_H_
