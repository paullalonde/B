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

#ifndef BAEUndoAction_H_
#define BAEUndoAction_H_

#pragma once

// B headers
#include "BAEDescriptor.h"
#include "BUndoAction.h"


namespace B {

/*!	@brief	An undoable action that sends an Apple %Event.
	
	This convenience class maintains an AEDescriptor, which is assumed to contain an 
	@c AppleEvent.  When asked to perform its undo action, the object sends the 
	@c AppleEvent to the application.
	
	It is mainly of use for factored applications whose state is already set up to be 
	manipulated via Apple Events.
	
	To use, derive a class from AEUndoAction, and build an AppleEvent in @a mEvent that 
	restores some state.  This can be done in the derived class' constructor.  If can 
	also be done after construction, as long as it's done before Perform() is called!
	
	Instances of AEUndoAction are passed to implementations of UNDO_POLICY and maintained 
	by them until an %Undo or %Redo action is performed.
	
	@sa			Undo, UNDO_POLICY
	@ingroup	UndoGroup
*/
class AEUndoAction : public UndoAction
{
public:
	
	//! Invocation.  The object should perform its action.
	virtual void	Perform();
	
protected:
	
	//! Constructor.
	AEUndoAction();
			
	//! Debugging support.
	void	RecordComment(const char* comment);
	
	// member variables
	AEDescriptor	mEvent;	//!< The Apple %Event.
};


}	// namespace B


#endif	// BAEUndoAction_H_
