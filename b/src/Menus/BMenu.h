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

#ifndef BMenu_H_
#define BMenu_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BEventHandler.h"
#include "BFwd.h"


namespace B {


/*!	@brief	Wrapper around a %Menu Manager menu.
	
	@todo	%Document this class!
*/
class Menu : public boost::noncopyable
{
public:
	
				Menu(MenuRef inMenuRef);
	virtual		~Menu();
	
	static Menu*	Instantiate(MenuRef inMenuRef);
	static Menu*	FromMenuRef(MenuRef inMenuRef);
					
	// conversions
	//! Returns the object's underlying @c HIViewRef.
	operator		MenuRef () const		{ return (mMenuRef); }
	//! Returns the object's underlying @c HIViewRef.
	MenuRef			GetMenuRef() const		{ return (mMenuRef); }
	//! Returns the object's underlying @c HIObjectRef.
	HIObjectRef		GetObjectRef() const	{ return (reinterpret_cast<HIObjectRef>(mMenuRef)); }
	//! Retrieves the underlying @c EventTargetRef.
	EventTargetRef	GetEventTarget() const	{ return (HIObjectGetEventTarget(GetObjectRef())); }
	
protected:
	
	// Carbon %Event handlers
	virtual void	HandleDispose();

protected:
	
	// member variables
	MenuRef			mMenuRef;

private:
	
	void	InitEventHandler();
	
	// Carbon %Event handlers
	bool	MenuDispose(
				Event<kEventClassMenu, kEventMenuDispose>&		event);
	
	// member variables
	EventHandler	mEventHandler;
};


}	// namespace B


#endif	// BMenu_H_
