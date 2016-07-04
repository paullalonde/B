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

#ifndef BNullUndoPolicy_H_
#define BNullUndoPolicy_H_

#pragma once

// standard headers
#include <memory>

// library headers
#include <boost/utility.hpp>
#include <boost/signal.hpp>

// B headers
#include "BString.h"
#include "BUndoAction.h"


namespace B {

// forward declarations
class	AEObjectSupport;
class	Bundle;
template <class UNDO_POLICY> class	UndoPolicyEvents;


/*!
	@brief	Do-nothing undo policy.
	
	This class is a stub implementation of UNDO_POLICY.  Most of its member functions 
	do nothing.  There are a few exceptions to this however.  Most notably, 
	the Apple %Event and Carbon %Event interfaces are implemented.
	
	@note	<strong>VERY IMPORTANT:</strong>  NullUndoPolicy does @s not implement the 
			semantics of AbortGroup() as defined by UNDO_POLICY.  If you need the 
			semantics of AbortGroup(), use SingleUndoPolicy or MultipleUndoPolicy 
			instead.
	
	This class can be used as a template parameter to those classes that take an 
	UNDO_POLICY template parameter.
	
	Currently, two classes take undo policies as template parameters:  Application and 
	Document.
	
	@sa			@ref using_undo, UNDO_POLICY
	@ingroup	UndoGroup
*/
class NullUndoPolicy : public boost::noncopyable
{
public:
	
	//! @name Types
	//@{
	//! Shorthand for the type of signal used by the undo policy.
	typedef boost::signal0<void>	Signal;
	//@}
	
	//! @name Constructor & Destructor
	//@{
	//! Constructor.
	NullUndoPolicy(
		EventTargetRef	inTarget,
		const Bundle&	inBundle);
	//@}
	
	//! @name Setup
	//@{
	//! Installs Apple %Event handlers for Undo-related AppleEvents.
	void	RegisterAppleEvents(AEObjectSupport& ioObjectSupport);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns @c true if we are currently in the process of undoing an action group.
	bool		IsUndoing() const		{ return (false); }
	//! Returns @c true if we are currently in the process of redoing an action group.
	bool		IsRedoing() const		{ return (false); }
	//! Returns @c true if the undo mechanism is enabled.
	bool		IsEnabled() const		{ return (false); }
	//! Returns @c true if there is at least one action in the undo stack.
	bool		CanUndo() const			{ return (false); }
	//! Returns @c true if there is at least one action in the redo stack.
	bool		CanRedo() const			{ return (false); }
	//! Returns the maximum size of the undo/redo stacks.
	unsigned	GetMaxLevels() const	{ return (0); }
	//! Returns a string describing the action group at the top of the undo stack.
	String		GetUndoText() const		{ return (mUndoStr); }
	//! Returns a string describing the action group at the top of the redo stack.
	String		GetRedoText() const		{ return (mRedoStr); }
	//@}
	
	//! @name Modifiers
	//@{
	//! Enables or disables the undo mechanism.
	void	Enable(bool)						{}
	//! Sets the maximum size of the undo/redo stacks.
	void	SetMaxLevels(unsigned)				{}
	//! Sets the name of the currently open action group.
	void	SetActionGroupName(const String&)	{}
	//! Empties the undo and redo stacks.
	void	Clear()								{}
	//@}
	
	//!	@name Action Groups
	//@{
	//! Opens an action group called @a inName.
	void	BeginGroup(const String&)	{}
	//! Commits the currently open action group.
	void	CommitGroup()				{}
	//! Aborts the currently open action group.
	void	AbortGroup()				{}
	//@}
	
	//! @name Adding Actions
	//@{
	//! Adds @a inUndoAction to the currently open action group.
	void	Add(std::auto_ptr<UndoAction> inUndoAction);
	//! Adds @a inFunctor to the currently open action group.
	void	Add(FunctorUndoAction::FunctorType inFunctor);
	//@}
	
	//! @name Undo & Redo
	//@{
	//!	Executes the action group at the top of the undo stack.
	void	Undo()	{}
	//!	Executes the action group at the top of the redo stack.
	void	Redo()	{}
	//@}
	
	//! @name Signals
	//@{
	//! This signal is sent when an %Undo is performed.
	Signal&	GetUndoSignal()		{ return (mDummySignal); }
	//! This signal is sent when an %Redo is performed.
	Signal&	GetRedoSignal()		{ return (mDummySignal); }
	//! This signal is sent when an action is added.
	Signal&	GetActionSignal()	{ return (mDummySignal); }
	//@}
	
private:
	
	// types
	typedef UndoPolicyEvents<NullUndoPolicy>	ImplType;
	
	// member variables
	std::auto_ptr<ImplType>	mImpl;
	String					mUndoStr;
	String					mRedoStr;
	Signal					mDummySignal;
};


}	// namespace B


#endif	// BNullUndoPolicy_H_
