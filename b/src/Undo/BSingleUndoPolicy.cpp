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
#include "BSingleUndoPolicy.h"

// B headers
#include "BStringFormatter.h"
#include "BUndoAction.h"
#include "BUtility.h"

// template instantiation
#include "BUndoPolicyHelpers.tpl.h"


namespace B {

// ------------------------------------------------------------------------------------------
SingleUndoPolicy::SingleUndoPolicy(
	EventTargetRef	inTarget,	//!< The event target onto which the object can install handlers.
	const Bundle&	inBundle)	//!< The bundle from which to retrieve resources.
		: mImpl(std::auto_ptr<ImplType>(new ImplType(inTarget, *this))), 
		  mStrings(inBundle), 
		  mDisableCount(0), mUndoing(false), mUndone(false)
{
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::RegisterAppleEvents(AEObjectSupport& ioObjectSupport)
{
	mImpl->RegisterAppleEvents(ioObjectSupport);
}

// ------------------------------------------------------------------------------------------
String
SingleUndoPolicy::GetUndoText() const
{
	String	text;
	
	if (!mUndoGroup)
	{
		text = mStrings.GetCantString(!mUndone);
	}
	else
	{
		const String&	actionName	= mUndoGroup->GetName();
		
		if (!actionName.empty())
		{
			text = Extract(StringFormatter(mStrings.GetActionString(!mUndone)) % actionName);
		}
		else
		{
			text = mStrings.GetSimpleString(!mUndone);
		}
	}
	
	return (text);
}

// ------------------------------------------------------------------------------------------
String
SingleUndoPolicy::GetRedoText() const
{
	return (GetUndoText());
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::Enable(bool inEnable)
{
	if (inEnable)
		mDisableCount--;
	else
		mDisableCount++;
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::SetActionGroupName(const String& inName)
{
	if (mOpenGroups.empty())
		B_THROW(std::logic_error("SingleUndoPolicy::SetActionGroupName no open group"));
	
	mOpenGroups.top()->SetName(inName);
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::Undo()
{
	B_ASSERT(!mUndoing);
	B_ASSERT(mOpenGroups.empty());
	
	if (mUndoGroup)
	{
		Signal*		sig;
		AEEventID	eventID;
		
		if (mUndone)
		{
			sig		= &mRedoSignal;
			eventID	= kAERedo;
		}
		else
		{
			sig		= &mUndoSignal;
			eventID	= kAEUndo;
		}
		
		AutoValue<bool>	autoUndoing(mUndoing, true);
		AppleEvent		currentEvent;
		OSStatus		err;
		
		// If an Apple %Event is @e not being handled at the moment, then issue 
		// an Apple %Event for recording purposes.  The reason for the check is that 
		// we don't want to issue an Apple %Event for %Undo when we get called in 
		// the course of handling an %Undo Apple %Event!.
		
		err = AEGetTheCurrentEvent(&currentEvent);
		
		if ((err == noErr) && (currentEvent.descriptorType == typeNull))
		{
			AEWriter	writer(kAEMiscStandards, eventID, AESelfTarget());
			
			writer.Send(kAEDontExecute);
		}
		
		// When performing an %Undo or a %Redo, we automagically open a group (for 
		// redo and undo actions, respectively) with the same name as we're 
		// excuting.
		
		GroupPtr	undoGroup;
		
		swap(undoGroup, mUndoGroup);
		
		BeginGroup(undoGroup->GetName());
		
		try
		{
			undoGroup->Perform();
			
			CommitGroup();
		}
		catch (...)
		{
			AbortGroup();
			
			throw;
		}
		
		(*sig)();
	}
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::Redo()
{
	Undo();
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::Clear()
{
	B_ASSERT(mOpenGroups.empty());
	
	mUndoGroup = NULL;
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::Add(std::auto_ptr<UndoAction> inUndoAction)
{
	B_ASSERT(inUndoAction.get() != NULL);
	
	if (IsEnabled())
	{
		ActionPtr	topAction;
		
		if (!mOpenGroups.empty())
			topAction = mOpenGroups.top()->top();
		
		if (!topAction && mUndoGroup)
			topAction = mUndoGroup->top();
		
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
SingleUndoPolicy::Add(FunctorUndoAction::FunctorType inFunctor)
{
	Add(std::auto_ptr<UndoAction>(new FunctorUndoAction(inFunctor)));
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::BeginGroup(const String& inName)
{
	UndoActionGroup::PushSubGroup(inName, mOpenGroups);
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::CommitGroup()
{
	GroupPtr	group	= UndoActionGroup::PopSubGroup(mOpenGroups);
	
	if (mOpenGroups.empty())
	{
		// We've finished a top-level group.  Move it to mUndoGroup.
		
		mUndoGroup = group;
		
		// We only broadcast when committing a top-level group that's not being created as 
		// a result of an Undo or Redo.  That's because we want a "normal" assignment to 
		// mUndoGroup to have the same weight as an Undo or a Redo, in terms of the number 
		// of generated broadcasts.
		
		if (!mUndoing)
		{
			mUndone = false;
			mActionSignal();
		}
		else
		{
			mUndone = !mUndone;
		}
	}
}

// ------------------------------------------------------------------------------------------
void
SingleUndoPolicy::AbortGroup()
{
	try
	{
		GroupPtr	group	= UndoActionGroup::PopSubGroup(mOpenGroups);
		
		group->Perform();
	}
	catch (...)
	{
		// Prevent execution errors from propagating.
	}
}

}	// namespace B
