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
#include "BMultipleUndoPolicy.h"

// standard headers
#include <limits.h>

// B headers
#include "BBundle.h"
#include "BStringFormatter.h"
#include "BUndoAction.h"
#include "BUtility.h"

// template instantiation
#include "BUndoPolicyHelpers.tpl.h"


namespace B {

// ------------------------------------------------------------------------------------------
MultipleUndoPolicy::MultipleUndoPolicy(
	EventTargetRef	inTarget,	//!< The event target onto which the object can install handlers.
	const Bundle&	inBundle)	//!< The bundle from which to retrieve resources.
		: mImpl(std::auto_ptr<ImplType>(new ImplType(inTarget, *this))), 
		  mStrings(inBundle), 
		  mDisableCount(0), mMaxLevels(UINT_MAX), mUndoing(false), mRedoing(false)
{
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::RegisterAppleEvents(AEObjectSupport& ioObjectSupport)
{
	mImpl->RegisterAppleEvents(ioObjectSupport);
}

// ------------------------------------------------------------------------------------------
String
MultipleUndoPolicy::GetUndoText() const
{
	return (GetText(true, mUndoStack));
}

// ------------------------------------------------------------------------------------------
String
MultipleUndoPolicy::GetRedoText() const
{
	return (GetText(false, mRedoStack));
}

// ------------------------------------------------------------------------------------------
String
MultipleUndoPolicy::GetText(
	bool				inUndo, 
	const UndoStack&	ioStack) const
{
	String	text;
	
	if (ioStack.empty())
	{
		text = mStrings.GetCantString(inUndo);
	}
	else
	{
		const String&	actionName	= ioStack.front()->GetName();
		
		if (!actionName.empty())
		{
			text = Extract(StringFormatter(mStrings.GetActionString(inUndo)) % actionName);
		}
		else
		{
			text = mStrings.GetSimpleString(inUndo);
		}
	}
	
	return (text);
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Enable(bool inEnable)
{
	if (inEnable)
		mDisableCount--;
	else
		mDisableCount++;
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::SetMaxLevels(unsigned inMaxLevels)
{
	mMaxLevels = inMaxLevels;
	
	while (mUndoStack.size() > mMaxLevels)
		mUndoStack.pop_back();
	
	while (mRedoStack.size() > mMaxLevels)
		mRedoStack.pop_back();
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::SetActionGroupName(const String& inName)
{
	if (mOpenGroups.empty())
		B_THROW(std::logic_error("MultipleUndoPolicy::SetActionGroupName no open group"));
	
	mOpenGroups.top()->SetName(inName);
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Undo()
{
	Do(mUndoStack, mUndoing, kAEUndo, mUndoSignal);
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Redo()
{
	Do(mRedoStack, mRedoing, kAERedo, mRedoSignal);
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Do(
	UndoStack&	ioStack, 
	bool&		doingVar, 
	AEEventID	inEventID,
	Signal&		ioNotification)
{
	B_ASSERT(!mUndoing && !mRedoing);
	B_ASSERT(mOpenGroups.empty());
	
	if (!ioStack.empty())
	{
		AutoValue<bool>	autoUndoing(doingVar, true);
		AppleEvent		currentEvent;
		OSStatus		err;
		
		// If an Apple %Event is @e not being handled at the moment, then issue 
		// an Apple %Event for recording purposes.  The reason for the check is that 
		// we don't want to issue an Apple %Event for %Undo when we get called in 
		// the course of handling an %Undo Apple %Event!.
		
		err = AEGetTheCurrentEvent(&currentEvent);
		
		if ((err == noErr) && (currentEvent.descriptorType == typeNull))
		{
			AEWriter	writer(kAEMiscStandards, inEventID, AESelfTarget());
			
			writer.Send(kAEDontExecute);
		}
		
		// When performing an %Undo or a %Redo, we automagically open a group (for 
		// redo and undo actions, respectively) with the same name as we're 
		// excuting.
		
		GroupPtr	group	= ioStack.front();
		
		ioStack.pop_front();
		
		BeginGroup(group->GetName());
		
		try
		{
			group->Perform();
			
			CommitGroup();
		}
		catch (...)
		{
			AbortGroup();
			
			throw;
		}
		
		ioNotification();
	}
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Clear()
{
	B_ASSERT(mOpenGroups.empty());
	
	mUndoStack.clear();
	mRedoStack.clear();
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Add(std::auto_ptr<UndoAction> inUndoAction)
{
	B_ASSERT(inUndoAction.get() != NULL);
	
	if (IsEnabled())
	{
		UndoStack&	stack	= mUndoing ? mRedoStack : mUndoStack;
		ActionPtr	topAction;
		
		if (!mOpenGroups.empty())
			topAction = mOpenGroups.top()->top();
		
		if (!topAction && !stack.empty())
			topAction = stack.front()->top();
		
		if (!topAction || !inUndoAction->Coalesce(topAction.get()))
		{
			ActionPtr	actionPtr	= UndoActionGroup::MakeActionPtr(inUndoAction);
			
			if (mOpenGroups.empty())
				BeginGroup(String());
			
			mOpenGroups.top()->Add(actionPtr);
		}
	}
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::Add(FunctorUndoAction::FunctorType inFunctor)
{
	Add(std::auto_ptr<UndoAction>(new FunctorUndoAction(inFunctor)));
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::BeginGroup(const String& inName)
{
	UndoActionGroup::PushSubGroup(inName, mOpenGroups);
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::CommitGroup()
{
	GroupPtr	group	= UndoActionGroup::PopSubGroup(mOpenGroups);
	
	if (mOpenGroups.empty())
	{
		// We've finished a top-level group.  Move it to correct stack.

		UndoStack&	stack	= mUndoing ? mRedoStack : mUndoStack;
		
		if (stack.size() >= mMaxLevels)
			stack.pop_back();
		
		stack.push_front(group);
		
		// We only broadcast when committing a top-level group that's not being created as 
		// a result of an Undo or Redo.  That's because we want a "normal" addition to the 
		// Undo stack to have the same weight as an Undo or a Redo, in terms of the number 
		// of generated broadcasts.
		
		if (!mUndoing && !mRedoing && mOpenGroups.empty())
			mActionSignal();
	}
}

// ------------------------------------------------------------------------------------------
void
MultipleUndoPolicy::AbortGroup()
{
	GroupPtr	group	= UndoActionGroup::PopSubGroup(mOpenGroups);
	size_t		ngroups	= mOpenGroups.size();
	
	BeginGroup(String());
	
	try
	{
		group->Perform();
		
		while (mOpenGroups.size() > ngroups)
			mOpenGroups.pop();
	}
	catch (...)
	{
		while (mOpenGroups.size() > ngroups)
			mOpenGroups.pop();
		
		throw;
	}
}

}	// namespace B
