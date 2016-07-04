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
#include "BUndoPolicyHelpers.h"

// B headers
#include "BBundle.h"


namespace B {

// ==========================================================================================
//	UndoActionGroup

#pragma mark -

// ------------------------------------------------------------------------------------------
UndoActionGroup::GroupPtr
UndoActionGroup::MakeGroup(const String& inName)
{
	return (GroupPtr(new UndoActionGroup(inName)));
}

// ------------------------------------------------------------------------------------------
UndoActionGroup::ActionPtr
UndoActionGroup::MakeActionPtr(std::auto_ptr<UndoAction> inAction)
{
	B_ASSERT(inAction->mRefCount == 0);
	
	return (ActionPtr(inAction.release()));
}

// ------------------------------------------------------------------------------------------
UndoActionGroup::UndoActionGroup(const String& inName)
	: mName(inName)
{
}

// ------------------------------------------------------------------------------------------
/*!	The "top" action is considered to be the first non-group action encountered in a 
	depth-first traversal of the tree of actions rooted in the group.
	
	This makes sense because this function is used to coalesce actions, so filtering 
	action groups is what we want.
*/
UndoActionGroup::ActionPtr
UndoActionGroup::top() const
{
	ActionPtr	actionPtr;
	
	for (ActionStack::const_iterator it = mActions.begin();
		 !actionPtr && (it != mActions.end()); 
		 ++it)
	{
		actionPtr = *it;
		
		if (UndoActionGroup* group = dynamic_cast<UndoActionGroup*>(actionPtr.get()))
			actionPtr = group->top();
		else
			actionPtr = *it;
	}
	
	return (actionPtr);
}

// ------------------------------------------------------------------------------------------
void
UndoActionGroup::Add(ActionPtr inAction)
{
	mActions.push_front(inAction);
}

// ------------------------------------------------------------------------------------------
void
UndoActionGroup::Perform()
{
	while (!mActions.empty())
	{
		ActionPtr	action(mActions.front());
		
		mActions.pop_front();
		
		action->Perform();
	}
}

// ------------------------------------------------------------------------------------------
void
UndoActionGroup::PushSubGroup(const String& inName, GroupStack& ioGroupStack)
{
	GroupPtr	group(MakeGroup(inName));
	
	if (!ioGroupStack.empty())
	{
		ActionPtr	actionPtr(group);
		
		ioGroupStack.top()->Add(actionPtr);
	}
	
	ioGroupStack.push(group);
}

// ------------------------------------------------------------------------------------------
UndoActionGroup::GroupPtr
UndoActionGroup::PopSubGroup(GroupStack& ioGroupStack)
{
	GroupPtr	group;
	
	if (ioGroupStack.empty())
		B_THROW(std::logic_error("UndoActionGroup::PopSubGroup no open group"));
	
	group = ioGroupStack.top();
	
	ioGroupStack.pop();
	
	return (group);
}


// ==========================================================================================
//	UndoStrings

#pragma mark -
#pragma mark UndoStrings

// ------------------------------------------------------------------------------------------
UndoStrings::UndoStrings(const Bundle& inBundle)
	: mSimpleUndoStr(BLocalizedStringFromTableInBundle(
							CFSTR("Undo"), CFSTR("B"), inBundle, 
							"Generic 'Undo' menu item text")),
	  mSimpleRedoStr(BLocalizedStringFromTableInBundle(
							CFSTR("Redo"), CFSTR("B"), inBundle, 
							"Generic 'Redo' menu item text")),
	  mCantUndoStr(BLocalizedStringFromTableInBundle(
							CFSTR("Can't Undo"), CFSTR("B"), inBundle, 
							"Displayed when there's nothing to undo")),
	  mCantRedoStr(BLocalizedStringFromTableInBundle(
							CFSTR("Can't Redo"), CFSTR("B"), inBundle, 
							"Displayed when there's nothing to redo")),
	  mActionUndoStr(BLocalizedStringFromTableInBundle(
							CFSTR("Undo %1%"), CFSTR("B"), inBundle, 
							"Default 'Undo' menu item text when there's a specific action to undo.  %1% holds the name of the action")),
	  mActionRedoStr(BLocalizedStringFromTableInBundle(
							CFSTR("Redo %1%"), CFSTR("B"), inBundle, 
							"Default 'Redo' menu item text when there's a specific action to redo.  %1% holds the name of the action")),
	  mDefaultGroupName(BLocalizedStringFromTableInBundle(
							CFSTR("Action"), CFSTR("B"), inBundle, 
							"The default name for an undoable action."))
{
}


}	// namespace B
