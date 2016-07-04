// ==========================================================================================
//	
//	Copyright (C) 2006 Paul Lalonde enrg.
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

#ifndef BPrintPolicyConcept_H_
#define BPrintPolicyConcept_H_

#pragma once

// B headers
#include "BDialogModality.h"
#include "BPageFormat.h"


namespace B {


#if DOXYGEN_SCAN

/*!
	@brief	Template for a Print Policy class.
	
	PRINT_POLICY isn't a class;  rather, its specifies the required interface for a 
	class in order for it to be useable as a template parameter.  So for example, 
	if a class @c Foo is declared like so:
	
	@code
		template <class PRINT_POLICY> Foo { ... };
	@endcode
	
	and class @c Bar implements the interface described herein, then one can instantiate 
	@c Foo with @c Bar:
	
	@code
		class MyFoo : public Foo<Bar> { ... };
	@endcode
	
	Conceptually, PRINT_POLICY maintains two stacks of action groups, one for %Undo and 
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
	
	Note that the above description applies to PRINT_POLICY generally.  Some 
	concrete implementations may collapse one or more aspects of the policy.  For 
	example, a "null" policy might have stub implementations for CommitGroup() and 
	AbortGroup(), among others.
	
	In addition to the "class" interface presented below, PRINT_POLICY must also implement 
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
	
	There are currently 3 examples of implementations of PRINT_POLICY:
	
	- NullUndoPolicy, for applications that don't need to support %Undo.
	- MultipleUndoPolicy, for applications that need separate %Undo and %Redo 
	  stacks of arbitrary size.
	- SingleUndoPolicy, for applications that need only a simple 1-level-deep 
	  %Undo.
		  
	@ingroup	UndoGroup
*/
class PRINT_POLICY
{
public:
	
	//! @name Types
	//@{
	//! The type of the callback that is invoked before the Page Setup dialog is displayed.
	typedef boost::function1<void, PrintSession&>		DialogInitCallback;
	//! The type of the callback that is invoked after the Page Setup dialog has been displayed.
	typedef boost::function2<void, PrintSession&, bool>	DialogResultCallback;
	//! The type of the signal that is triggered whenever the page format is changed.
	typedef boost::signal1<void, PageFormat&>			PageFormatChangedSignal;
	//! The type of the signal that is triggered whenever the print settings are changed.
	typedef boost::signal1<void, PrintSettings&>		PrintSettingsChangedSignal;
	//@}
	
	//! @name Constructor & Destructor
	//@{
	
	/*! @brief	Constructor.
		
		Typically installs Carbon %Event handlers on @a inTarget.
		
		@param	inTarget	The event target onto which the object can install handlers.
		@param	inBundle	The bundle from which to retrieve resources.
	*/
	PRINT_POLICY(
		EventTargetRef	inTarget, 
		const Bundle&	inBundle);
	
	//! Destructor.
	~PRINT_POLICY();
	//@}
	
	//! @name Setup
	//@{
	//! Sets the callback that is invoked before the Page Setup dialog is displayed.
	void	SetPageSetupDialogInitCallback(DialogInitCallback inCallback);
	//! Sets the callback that is invoked after the Page Setup dialog has been displayed.
	void	SetPageSetupDialogResultCallback(DialogResultCallback inCallback);
	//! Sets the callback that is invoked before the Print dialog is displayed.
	void	SetPrintDialogInitCallback(DialogInitCallback inCallback);
	//! Sets the callback that is invoked after the Print dialog has been displayed.
	void	SetPrintDialogResultCallback(DialogResultCallback inCallback);
	//! Returns a signal that is triggered whenever the page format is changed.
	PageFormatChangedSignal&
			GetPageFormatChangedSignal();
	//! Returns a signal that is triggered whenever the print settings are changed.
	PrintSettingsChangedSignal&
			GetPrintSettingsChangedSignal();
	
	/*! @brief	Enables the Print menu command.
		
		By disabling the Print menu command, one gets a print policy that only stores a 
		PageFormat object.
	*/
	void	EnablePrintCommand(bool enable);
	//@}
	
	//! @name Inquiries
	//@{

	/*! @brief	Returns the policy's page format object.
		
		@note This is a modifiable reference.  Changes to it will change the page 
		format embedded in the policy.
	*/
	PageFormat&	GetPageFormat();

	/*! @brief	Returns the policy's print settings object.
		
		@note This is a modifiable reference.  Changes to it will change the page 
		format embedded in the policy.
	*/
	PrintSettings&	GetPrintSettings();
	
	//@}
	
	//! @name Validation
	//@{

	/*! @brief	Validates the embedded page format object.
		
		@note If validation results in changes to the page format object, the 
		PageFormatChanged signal is triggered.
	*/
	void	ValidatePageFormat();
	
	/*! @brief	Validates the embedded print settings object.
		
		@note If validation results in changes to the print settings object, the 
		PrintSettingsChanged signal is triggered.
	*/
	void	ValidatePrintSettings();

	//@}
	
	//! @name Printing
	//@{
	
	/*! @brief	Initiates the printing process.
		
		@note If @a inShowDialog is @c true, displays the Print dialog, then invokes 
		the print dialog's result callback; else, invokes the callback without displaying 
		the dialog.
	*/
	void	Print(
				PrintSettings*	inPrintSettings,
				Printer*		inPrinter,
				bool			inShowDialog);
	//@}
};

#endif	// DOXYGEN_SCAN


#ifndef DOXYGEN_SKIP

template <class PRINT_POLICY> struct PrintPolicyConcept
{
	typedef typename PRINT_POLICY::DialogInitCallback				DialogInitCallback;
	typedef typename PRINT_POLICY::DialogResultCallback				DialogResultCallback;
	typedef typename PRINT_POLICY::PageFormatChangedSignal			PageFormatChangedSignal;
	typedef typename PRINT_POLICY::PrintSettingsChangedSignal		PrintSettingsChangedSignal;
	typedef typename PageFormatChangedSignal::slot_function_type	PageFormatChangedSlot;
	typedef typename PrintSettingsChangedSignal::slot_function_type	PrintSettingsChangedSlot;
	
	void	constraints()
	{
		DialogInitCallback			f1(TestDialogInitCallback);
		DialogResultCallback		f2(TestDialogResultCallback);
		PageFormatChangedSlot		f3(TestPageFormatChangedSignal);
		PrintSettingsChangedSlot	f4(TestPrintSettingsChangedSignal);
		
		PRINT_POLICY	policy(mEventTarget, mBundle);
		
		policy.SetPageSetupDialogInitCallback(mDialogInitCallback);
		policy.SetPageSetupDialogResultCallback(mDialogResultCallback);
		policy.SetPrintDialogInitCallback(mDialogInitCallback);
		policy.SetPrintDialogResultCallback(mDialogResultCallback);
		policy.GetPageFormatChangedSignal().connect(mPageFormatChangedSlot);
		policy.GetPrintSettingsChangedSignal().connect(mPrintSettingsChangedSlot);
		
		policy.EnablePrintCommand(false);
		
		policy.ValidatePageFormat();
		policy.ValidatePrintSettings();
		
		PageFormatReferenceFunction(policy.GetPageFormat());
		PrintSettingsReferenceFunction(policy.GetPrintSettings());
		
		PrintSettings*	settings	= NULL;
		Printer*		printer		= NULL;
		
		policy.Print(settings, printer, true);
	}
	
	void	PageFormatReferenceFunction(PageFormat&);
	void	PrintSettingsReferenceFunction(PrintSettings&);
	
	static void	TestDialogInitCallback(PrintSession&);
	static void	TestDialogResultCallback(PrintSession&, bool);
	static void	TestPageFormatChangedSignal(PageFormat&);
	static void	TestPrintSettingsChangedSignal(PrintSettings&);
	
	PrintSession&				mPrintSession;
	PrintSettings&				mPrintSettings;
	PageFormat&					mPageFormat;
	const PageFormat&			mConstPageFormat;
	EventTargetRef				mEventTarget;
	const Bundle&				mBundle;
	WindowRef					mParentWindowRef;
	DialogInitCallback			mDialogInitCallback;
	DialogResultCallback		mDialogResultCallback;
	PageFormatChangedSlot		mPageFormatChangedSlot;
	PrintSettingsChangedSlot	mPrintSettingsChangedSlot;
};

#endif	// DOXYGEN_SKIP


}	// namespace B


#endif	// BPrintPolicyConcept_H_
