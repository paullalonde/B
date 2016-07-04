// ==========================================================================================
//	
//	Copyright (C) 2005-2006 Paul Lalonde enrg.
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
#include "BClipboard.h"

// B headers
#include "BEvent.h"
#include "BEventHandler.h"
#include "BPasteboard.h"


namespace B {

// ------------------------------------------------------------------------------------------
Pasteboard&
Clipboard::Get()
{
	static Pasteboard						sClipboard(kPasteboardClipboard);
	static boost::scoped_ptr<EventHandler>	sHandler(InitEventHandler());
	
	return (sClipboard);
}

// ------------------------------------------------------------------------------------------
std::auto_ptr<EventHandler>
Clipboard::InitEventHandler()
{
	std::auto_ptr<EventHandler>	handler(new EventHandler(GetApplicationEventTarget()));
	AppActivatedFunction		f(AppActivated);
	
	handler->Add(f);
	handler->Init();
	
	return (handler);
}

// ------------------------------------------------------------------------------------------
bool
Clipboard::AppActivated(
	Event<kEventClassApplication, kEventAppActivated>&	/* event */)
{
	// The app is coming to the front, so update the clipboard.
	
	Get().Synchronize();
	
	return (false);
}


}	// namespace B
