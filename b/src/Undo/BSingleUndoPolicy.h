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

#ifndef BSingleUndoPolicy_H_
#define BSingleUndoPolicy_H_

#pragma once

// standard headers
#include <memory>

// library headers
#include <boost/utility.hpp>
#include <boost/signal.hpp>

// B headers
#include "BString.h"
#include "BUndoPolicyHelpers.h"


namespace B {

// forward declarations
class	AEObjectSupport;
class	Bundle;


/*!
	@brief	Single undo policy.
	
	This is a partial implementation of UNDO_POLICY.  It supports a one-level undo 
	similar to what was commonly done in older Macintosh applications.  %Undo and 
	%Redo are treatly as is they were the same command.
	
	When requested to supply undo text, the text will say "Undo This" or "Redo That", 
	depending on the last action.  For this reason, applications using this 
	class probably won't want to have to distinct %Undo and %Redo menu items.
	
	This class can be used as a template parameter to those classes that take an 
	UNDO_POLICY template parameter.
	
	Currently, two classes take undo policies as template parameters:  Application and 
	Document.
	
	@sa			@ref using_undo, UNDO_POLICY
	@ingroup	UndoGroup
*/
class SingleUndoPolicy : public boost::noncopyable
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
	SingleUndoPolicy(
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
	bool		IsUndoing() const		{ return (mUndoing); }
	//! Returns @c true if we are currently in the process of redoing an action group.
	bool		IsRedoing() const		{ return (mUndoing); }
	//! Returns @c true if the undo mechanism is enabled.
	bool		IsEnabled() const		{ return (mDisableCount == 0); }
	//! Returns @c true if there is at least one action in the undo stack.
	bool		CanUndo() const			{ return (mUndoGroup); }
	//! Returns @c true if there is at least one action in the redo stack.
	bool		CanRedo() const			{ return (mUndoGroup); }
	//! Returns the maximum size of the undo/redo stacks.
	unsigned	GetMaxLevels() const	{ return (1); }
	//! Returns a string describing the action group at the top of the undo stack.
	String		GetUndoText() const;
	//! Returns a string describing the action group at the top of the redo stack.
	String		GetRedoText() const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Enables or disables the undo mechanism.
	void	Enable(bool inEnable);
	//! Sets the maximum size of the undo/redo stacks.
	void	SetMaxLevels(unsigned)	{}
	//! Sets the name of the currently open action group.
	void	SetActionGroupName(const String& inName);
	//! Empties the undo and redo stacks.
	void	Clear();
	//@}
	
	//!	@name Action Groups
	//@{
	//! Opens an action group called @a inName.
	void	BeginGroup(const String& inName);
	//! Commits the currently open action group.
	void	CommitGroup();
	//! Aborts the currently open action group.
	void	AbortGroup();
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
	void	Undo();
	//!	Executes the action group at the top of the redo stack.
	void	Redo();
	//@}
	
	//! @name Signals
	//@{
	//! This signal is sent when an %Undo is performed.
	Signal&	GetUndoSignal()		{ return (mUndoSignal); }
	//! This signal is sent when an %Redo is performed.
	Signal&	GetRedoSignal()		{ return (mRedoSignal); }
	//! This signal is sent when an action is added.
	Signal&	GetActionSignal()	{ return (mActionSignal); }
	//@}
	
private:
	
	// types
	typedef UndoPolicyEvents<SingleUndoPolicy>	ImplType;
	typedef UndoActionGroup::ActionPtr			ActionPtr;
	typedef UndoActionGroup::GroupPtr			GroupPtr;
	typedef UndoActionGroup::GroupStack			GroupStack;
	
	// member variables
	std::auto_ptr<ImplType>	mImpl;
	UndoStrings				mStrings;
	GroupPtr				mUndoGroup;
	GroupStack				mOpenGroups;
	Signal					mUndoSignal;
	Signal					mRedoSignal;
	Signal					mActionSignal;
	unsigned				mDisableCount;
	bool					mUndoing;
	bool					mUndone;
};


}	// namespace B


#endif	// BSingleUndoPolicy_H_
