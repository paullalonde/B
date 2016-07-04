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

// B headers
#include "BAEEvent.h"
#include "BAEObjectSupport.h"
#include "BEvent.h"
#include "BException.h"
#include "BUndoPolicyHelpers.h"


namespace B {

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY>
UndoPolicyEvents<UNDO_POLICY>::UndoPolicyEvents(
	EventTargetRef	inTarget,	//!< The event target onto which the object can install handlers.
	UndoPolicyType&	inPolicy)	//!< The UNDO_POLICY.
		: mEventHandler(inTarget), 
		  mPolicy(inPolicy)
{
	InitEventHandler(mEventHandler);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY>
UndoPolicyEvents<UNDO_POLICY>::~UndoPolicyEvents()
{
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> void
UndoPolicyEvents<UNDO_POLICY>::InitEventHandler(EventHandler& ioHandler)
{
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::CommandProcess);
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::CommandUpdateStatus);
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::BUndoBegin);
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::BUndoCommit);
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::BUndoAbort);
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::BUndoAdd);
	ioHandler.Add(this, &UndoPolicyEvents<UNDO_POLICY>::BUndoEnable);
	
	ioHandler.Init();
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> void
UndoPolicyEvents<UNDO_POLICY>::RegisterAppleEvents(AEObjectSupport& ioObjectSupport)
{
	// Define the Undo event if it isn't already defined.  This way apps don't have to 
	// bother with defining the event in their sdef file.
	
	if (!ioObjectSupport.IsEventDefined(kAEMiscStandards, kAEUndo))
	{
		ioObjectSupport.DefineEvent(kAEMiscStandards, kAEUndo, 
									AEInfo::kEventDOIllegal, 
									AEInfo::kEventResultActionNone);
	}
	
	// Ditto for Redo.
	
	if (!ioObjectSupport.IsEventDefined(kAEMiscStandards, kAERedo))
	{
		ioObjectSupport.DefineEvent(kAEMiscStandards, kAERedo, 
									AEInfo::kEventDOIllegal, 
									AEInfo::kEventResultActionNone);
	}
	
	ioObjectSupport.SetClassEventHandler(cApplication, this, &ImplType::HandleUndoAppleEvent);
	ioObjectSupport.SetClassEventHandler(cApplication, this, &ImplType::HandleRedoAppleEvent);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> void
UndoPolicyEvents<UNDO_POLICY>::HandleUndoAppleEvent(
	AEEvent<kAEMiscStandards, kAEUndo>&	/* event */)
{
	SendUndoCommand(kHICommandUndo);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> void
UndoPolicyEvents<UNDO_POLICY>::HandleRedoAppleEvent(
	AEEvent<kAEMiscStandards, kAERedo>&	/* event */)
{
	SendUndoCommand(kHICommandRedo);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> void
UndoPolicyEvents<UNDO_POLICY>::SendUndoCommand(UInt32 inCommandID)
{
	HICommand	cmd	=	{ 0, inCommandID };
	OSStatus	err;
	
	err = ProcessHICommand(&cmd);
	
	if (err == eventNotHandledErr)
		B_THROW(AEEventNotHandledException());
	else
		B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	bool	handled	= true;
	
	switch (event.mHICommand.commandID)
	{
	case kHICommandUndo:
		mPolicy.Undo();
		break;
		
	case kHICommandRedo:
		mPolicy.Redo();
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	bool	handled	= true;
	
	switch (event.mHICommand.commandID)
	{
	case kHICommandUndo:
		event.mData.SetEnabled(mPolicy.CanUndo());
		event.mData.SetText(mPolicy.GetUndoText());
		break;
		
	case kHICommandRedo:
		event.mData.SetEnabled(mPolicy.CanRedo());
		event.mData.SetText(mPolicy.GetRedoText());
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::BUndoBegin(
	Event<kEventClassB, kEventBUndoBegin>&	event)
{
	mPolicy.BeginGroup(event.mName);
	
	return (true);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::BUndoCommit(
	Event<kEventClassB, kEventBUndoCommit>&	/* event */)
{
	mPolicy.CommitGroup();
	
	return (true);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::BUndoAbort(
	Event<kEventClassB, kEventBUndoAbort>&	/* event */)
{
	mPolicy.AbortGroup();
	
	return (true);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::BUndoAdd(
	Event<kEventClassB, kEventBUndoAdd>&	event)
{
	B_ASSERT(event.mUndoAction != NULL);
	
	mPolicy.Add(std::auto_ptr<UndoAction>(event.mUndoAction));
	
	return (true);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY> bool
UndoPolicyEvents<UNDO_POLICY>::BUndoEnable(
	Event<kEventClassB, kEventBUndoEnable>&	event)
{
	mPolicy.Enable(event.mEnable);
	
	return (true);
}

}	// namespace B
