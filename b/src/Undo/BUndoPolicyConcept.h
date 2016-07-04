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

#ifndef BUndoPolicyConcept_H_
#define BUndoPolicyConcept_H_

#ifdef __MWERKS__
#	pragma once
#endif

// library headers
#include <boost/signal.hpp>

// B headers
#include "BString.h"
#include "BUndoAction.h"


namespace B {

// forward declarations
class	AEObjectSupport;
class	Bundle;


#if DOXYGEN_SCAN

/*!
	@brief	Template for an Undo Policy class.
	
	UNDO_POLICY isn't a class;  rather, its specifies the required interface for a 
	class in order for it to be useable as a template parameter.  So for example, 
	if a class @c Foo is declared like so:
	
	@code
		template <class UNDO_POLICY> Foo { ... };
	@endcode
	
	and class @c Bar implements the interface described herein, then one can instantiate 
	@c Foo with @c Bar:
	
	@code
		class MyFoo : public Foo<Bar> { ... };
	@endcode
	
	Conceptually, UNDO_POLICY maintains two stacks of action groups, one for %Undo and 
	the other for %Redo.  Both stacks can be arbitrarily deep, or they can have a fixed 
	maximal size.  Attempting to push an undo group onto a stack that is already at 
	its maximum size will cause the action group at the bottom of the stack to be 
	discarded.
	
	The next question is, what's an action group?  It's an ordered collection of one or 
	more undo actions (objects derived from UndoAction).  Action groups may be nested.
	They are populated by having a client call BeginGroup(), which creates a new 
	empty group and "opens" it.  The client then calls Add() one or more times, which 
	has the effect of appending each UndoAction to the open group.  The idea is that 
	as the client is modifying its state, it passes to Add() objects that are capable of 
	restoring that state.  If the client's processing proceeds normally, it closes the 
	group by calling CommitGroup().  This pushes the group onto either the 
	%Undo or %Redo stack.  On the other hand, if an error occurs in the client while 
	it was modifying its state (eg, an exception is thrown), then the client closes 
	the group by calling AbortGroup() instead.  AbortGroup() executes each undo action 
	in the currently open group (as well as nested groups etc) before discarding the 
	group.  Thus the trio of functions BeginGroup() / CommitGroup() / AbortGroup() 
	are analogous to similarly-named transaction functions in the database world.
	
	When a user issues and %Undo or a %Redo menu command, the top-most action group 
	on the corresponding stack is executed, then it is discarded.  Typically, the 
	act of executing the group will trigger further calls to Add(), this time for 
	%Redo actions.
	
	Note that the above description applies to UNDO_POLICY generally.  Some 
	concrete implementations may collapse one or more aspects of the policy.  For 
	example, a "null" policy might have stub implementations for CommitGroup() and 
	AbortGroup(), among others.
	
	In addition to the "class" interface presented below, UNDO_POLICY must also implement 
	two other interfaces:  one for Apple Events and the other for Carbon Events.
	
	<h3>Apple %Event Interface</h3>
	
	This interface is required to support %Undo & %Redo via AppleScript.
	
	<dl>
		<dt><tt>kAEMiscStandards / kAEUndo</tt></dt>
			<dd>Because this Apple %Event doesn't have a direct parameter (which means we 
			don't know who exactly is supposed to receive it), the event is translated 
			into a <tt>kEventClassCommand / kEventCommandProcess</tt> Carbon %Event for 
			the @c kHICommandUndo command and sent to the user input focus.</dd>
			
		<dt><tt>kAEMiscStandards / kAERedo</tt></dt>
			<dd>Because this Apple %Event doesn't have a direct parameter (which means we 
			don't know who exactly is supposed to receive it), the event is translated 
			into a <tt>kEventClassCommand / kEventCommandProcess</tt> Carbon %Event for 
			the @c kHICommandRedo command and sent to the user input focus.</dd>
	</dl>
	
	<H3>Carbon %Event Interface</H3>
	
	This interface is required to support decoupling the %Undo Policy from its clients.
	One will usually want to isolate code that performs undoable state changes from the 
	exact implementation of the %Undo Policy.  This is done by having such code use 
	the functions of the Undo class to open and close groups and add undoable actions, 
	rather than using the Policy's member functions directly.  The Undo class translates 
	its requests into Carbon Events which are then handled in the %Undo Policy.
	
	<dl>
		<dt><tt>kEventClassB / kEventBUndoBegin</tt></dt>
			<dd>
			This is equivalent to calling BeginGroup().  An optional parameter specifies 
			the action group's name.
			
			Parameters:
			
			<dl>
				<dt>@c kEventParamBUndoName (in, @c typeCFStringRef)</dt>
					<dd>The action group's name.  This parameter is optional;  if absent 
					a default name is used.</dd>
			</dl>
			</dd>

		<dt><tt>kEventClassB / kEventBUndoCommit</tt></dt>
			<dd>This is equivalent to calling CommitGroup().</dd>

		<dt><tt>kEventClassB / kEventBUndoAbort</tt></dt>
			<dd>This is equivalent to calling AbortGroup().</dd>

		<dt><tt>kEventClassB / kEventBUndoAdd</tt></dt>
			<dd>
			This is equivalent to calling Add().
			
			Parameters:
			
			<dl>
				<dt>@c kEventParamBUndoAction (in, @c typeBUndoAction)</dt>
					<dd>The undoable action, of a type derived from UndoAction.</dd>
			</dl>
			</dd>

		<dt><tt>kEventClassB / kEventBUndoEnable</tt></dt>
			<dd>
			This is equivalent to calling Enable().
				
			Parameters:
			
			<dl>
				<dt>@c kEventParamBUndoEnable (in, @c typeBoolean)</dt>
					<dd>The argument to pass to Enable().</dd>
			</dl>
			</dd>
	</dl>
    
	Currently, two classes take undo policies as template parameters:  Application and 
	Document.
	
	There are currently 3 examples of implementations of UNDO_POLICY:
	
	- NullUndoPolicy, for applications that don't need to support %Undo.
	- MultipleUndoPolicy, for applications that need separate %Undo and %Redo 
	  stacks of arbitrary size.
	- SingleUndoPolicy, for applications that need only a simple 1-level-deep 
	  %Undo.
		  
	@ingroup	UndoGroup
*/
class UNDO_POLICY
{
public:
	
	//! @name Types
	//@{
	//! Shorthand for the type of signal used by the undo policy.
	typedef boost::signal0<void>	Signal;
	//@}
	
	//! @name Constructor & Destructor
	//@{
	
	/*! @brief	Constructor.
		
		Typically installs Carbon %Event handlers on @a inTarget.
		
		@param	inTarget	The event target onto which the object can install handlers.
		@param	inBundle	The bundle from which to retrieve resources.
	*/
	UNDO_POLICY(EventTargetRef inTarget, const Bundle& inBundle);
	
	//! Destructor.
	~UNDO_POLICY();
	//@}
	
	//! @name Setup
	//@{
	/*! @brief	Installs Apple %Event handlers for Undo-related AppleEvents.
		
		Note that this should only ever be called for the %Undo Policy object that is 
		attached to the Application object.  That's because there is only one 
		application-wide Apple %Event handler for the %Undo and %Redo events.
	*/
	void		RegisterAppleEvents(AEObjectSupport& ioObjectSupport);
	//@}
	
	//! @name Inquiries
	//@{

	//! Returns @c true if we are currently in the process of undoing an action group.
	bool		IsUndoing() const;
	
	//! Returns @c true if we are currently in the process of redoing an action group.
	bool		IsRedoing() const;
	
	/*! @brief	Returns @c true if the undo mechanism is enabled.
	
		"Enabled" in this context means that undo actions can be added to the undo or redo stacks.
	*/
	bool		IsEnabled() const;
	
	//! Returns @c true if there is at least one action in the undo stack.
	bool		CanUndo() const;
	
	//! Returns @c true if there is at least one action in the redo stack.
	bool		CanRedo() const;

	//! Returns the maximum size of the undo/redo stacks.
	unsigned	GetMaxLevels() const;
	
	/*! @brief	Returns a string describing the action group at the top of the undo stack.
	
		The string is in a form suitable for placing in a menu.
	*/
	String		GetUndoText() const;
	
	/*! @brief	Returns a string describing the action group at the top of the redo stack.
	
		The string is in a form suitable for placing in a menu.
	*/
	String		GetRedoText() const;
	//@}
	
	//! @name Modifiers
	//@{
	
	/*! @brief	Enables or disables the undo mechanism.
		
		Each call with @a inEnable = @c false prevents undoable actions from being added 
		to the open action group;  each call with @a inEnable = @c true allows it to 
		happen again.  Each call to Enable(false) must be balanced with a call to 
		Enable(true).
		
		@param	inEnable	@c true to enable the undo mechanism, @c false to disable it.
	*/
	void	Enable(bool inEnable);
	
	/*! @brief	Sets the maximum size of the undo/redo stacks.
		
		If a transaction is committed and there are already @a inMaxLevels transactions 
		stored in the %Undo Policy, the oldest transaction is discarded.
		
		@param	inMaxLevels	The maximum size of the undo & redo stacks.  Value of @c UINT_MAX results in an unlimited stack size.
	*/
	void	SetMaxLevels(unsigned inMaxLevels);
	
	//! Sets the name of the currently open action group.
	void	SetActionGroupName(const String& inName);
	
	//! Empties the undo and redo stacks.
	void	Clear();
	//@}
	
	/*!	@name Action Groups
		
		Action groups allow any number of undoable actions to be committed or aborted as 
		a unit.  This is important in order to make the accumulation of actions resilient 
		in the presence of exceptions.
	*/
	//@{
	
	/*! @brief	Opens an action group called @a inName.
		
		@param	inName	The group's name.  If it's empty, a default value is used.  The name is visible to the user and so should be localised.
	*/
	void	BeginGroup(const String& inName);
	
	/*! @brief	Commits the currently open action group.
		
		@exception	std::logic_error	If there isn't an open action group.
	*/
	void	CommitGroup();
	
	/*! @brief	Aborts the currently open action group.
		
		@exception	std::logic_error	If there isn't an open action group.
	*/
	void	AbortGroup();
	//@}
	
	/*! @name Adding Actions
		
		These functions adds a single undoable action to the currently open action group.  
		If no group is open, a new group with a default name is opened.
	*/
	//@{
	
	/*! @brief	Adds @a inUndoAction to the currently open action group.
		
		@param	inUndoAction	An undoable action.  It's wrapped in an @c std::auto_ptr to emphasise the fact that ownership is transferred to the policy object.
	*/
	void	Add(std::auto_ptr<UndoAction> inUndoAction);
	
	/*! @brief	Adds @a inFunctor to the currently open action group.
		
		@param	inFunctor	A functor taking no arguments and returning no result.
	*/
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
	Signal&	GetUndoSignal();
	//! This signal is sent when an %Redo is performed.
	Signal&	GetRedoSignal();
	//! This signal is sent when an action is added.
	Signal&	GetActionSignal();
	//@}
};

#endif	// DOXYGEN_SCAN


#ifndef DOXYGEN_SKIP

template <class UNDO_POLICY> struct UndoPolicyConcept
{
	void	constraints()
	{
		UNDO_POLICY			policy(mEventTarget, mBundle);
		const UNDO_POLICY&	constPolicy	= policy;
		
		policy.RegisterAppleEvents(mObjectSupport);
		
		mBool		= constPolicy.IsUndoing();
		mBool		= constPolicy.IsRedoing();
		mBool		= constPolicy.IsEnabled();
		mBool		= constPolicy.CanUndo();
		mBool		= constPolicy.CanRedo();
		mUnsigned	= constPolicy.GetMaxLevels();
		mString		= constPolicy.GetUndoText();
		mString		= constPolicy.GetRedoText();
		
		policy.SetMaxLevels(2U);
		policy.SetActionGroupName(mString);
		policy.Clear();
		
		policy.BeginGroup(mString);
		policy.CommitGroup();
		policy.AbortGroup();
		
		policy.Add(mUndoAction);
		policy.Undo();
		policy.Redo();
		
		mSignal = &policy.GetUndoSignal();
		mSignal = &policy.GetRedoSignal();
		mSignal = &policy.GetActionSignal();
	}
	
	EventTargetRef				mEventTarget;
	const Bundle&				mBundle;
	AEObjectSupport&			mObjectSupport;
	bool						mBool;
	unsigned					mUnsigned;
	String						mString;
	std::auto_ptr<UndoAction>	mUndoAction;
	CFStringRef					mNotifStr;
	typename UNDO_POLICY::Signal* mSignal;
};

#endif	// DOXYGEN_SKIP


}	// namespace B


#endif	// BUndoPolicyConcept_H_
