// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

// file header
#include "BAEUndoAction.h"

// B headers
#include "BAEEvent.h"


#define DEBUG_PERFORM   0   // Make non-zero to get recording & comments in Script Editor.


namespace B {

// ------------------------------------------------------------------------------------------
AEUndoAction::AEUndoAction()
{
}

// ------------------------------------------------------------------------------------------
void
AEUndoAction::Perform()
{
    // Usually, when the user performs an Undo and AppleScript recording is turned on, we 
    // want "undo" to appear in the recording window, and not the sequence of self-sent 
    // Apple Events that the Undo may generate.  For this reason the Apple Event is 
    // nornally sent with recording disabled.  However, if DEBUG_PERFORM is non-zero, then 
    // the event will be recorded anyway.
    
#if DEBUG_PERFORM
    const AESendMode    kSendMode   = 0;
#else
    const AESendMode    kSendMode   = kAEDontRecord;
#endif
    
#if DEBUG_PERFORM
    RecordComment("executing");
#endif
    
    AEEventBase::SendEvent(mEvent, kSendMode);
}

// ------------------------------------------------------------------------------------------
/*! If AppleScript recording is turned on, this will emit a comment in the recording 
    window.  Very convenient for debugging code that sends Apple Events.
*/
void
AEUndoAction::RecordComment(const char* comment)
{
    AEEvent<kASAppleScriptSuite, kASCommentEvent>::Send(comment);
}


}   // namespace B
