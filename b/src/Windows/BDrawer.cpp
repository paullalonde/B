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
#include "BDrawer.h"

// B headers
#include "BEvent.h"


namespace B {

// ------------------------------------------------------------------------------------------
Drawer::Drawer(
	WindowRef		inWindowRef,
	B::AEObjectPtr	inContainer,
	DescType		inClassID /* = cWindow */)
		: Window(inWindowRef, inContainer, inClassID), 
		  mEventHandler(inWindowRef)
{
#ifndef NDEBUG
	WindowClass windowClass;
	OSStatus	err;
	
	err = GetWindowClass(inWindowRef, &windowClass);
	B_THROW_IF_STATUS(err);
	
	B_ASSERT(windowClass == kDrawerWindowClass);
	B_ASSERT(IsComposited());
#endif

	InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
Drawer::InitEventHandler()
{
	mEventHandler.Add(this, &Drawer::WindowDrawerOpening);
	mEventHandler.Add(this, &Drawer::WindowDrawerOpened);
	mEventHandler.Add(this, &Drawer::WindowDrawerClosing);
	mEventHandler.Add(this, &Drawer::WindowDrawerClosed);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
void
Drawer::SetParent(WindowRef inParent)
{
	OSStatus	err;
	
	err = SetDrawerParent(GetWindowRef(), inParent);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drawer::SetPreferredEdge(OptionBits inEdge)
{
	OSStatus	err;
	
	err = SetDrawerPreferredEdge(GetWindowRef(), inEdge);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drawer::SetOffsets(float inLeadingOffset, float inTrailingOffset)
{
	OSStatus	err;
	
	err = SetDrawerOffsets(GetWindowRef(), inLeadingOffset, inTrailingOffset);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drawer::GetOffsets(float& outLeadingOffset, float& outTrailingOffset) const
{
	OSStatus	err;
	
	err = GetDrawerOffsets(GetWindowRef(), &outLeadingOffset, &outTrailingOffset);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drawer::Open()
{
	OSStatus	err;
	
	err = ::OpenDrawer(GetWindowRef(), kWindowEdgeDefault, true);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drawer::Close()
{
	OSStatus	err;

	err = ::CloseDrawer(GetWindowRef(), true);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drawer::Toggle()
{
	OSStatus	err;
	
	err = ::ToggleDrawer(GetWindowRef());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever the 
	drawer is about to open.
	
	The default implementation always returns @c true.
	
	@return @c true to allow the drawer to open, else @c false.
*/
bool
Drawer::DrawerOpening()
{
	return (true);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever the 
	drawer is has opened.
	
	The default implementation does nothing.
*/
void
Drawer::DrawerOpened()
{
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever the 
	drawer is about to close.
	
	The default implementation always returns @c true.
	
	@return @c true to allow the drawer to close, else @c false.
*/
bool
Drawer::DrawerClosing()
{
	return (true);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever the 
	drawer is has closed.
	
	The default implementation does nothing.
*/
void
Drawer::DrawerClosed()
{
}

// ------------------------------------------------------------------------------------------
bool
Drawer::WindowDrawerOpening(
	Event<kEventClassWindow, kEventWindowDrawerOpening>&	event)
{
	bool	handled = false;
	
	if (event.mWindowRef == GetWindowRef())
	{
		event.mCancel   = !DrawerOpening();
		handled			= true;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
Drawer::WindowDrawerOpened(
	Event<kEventClassWindow, kEventWindowDrawerOpened>&	event)
{
	bool	handled = false;
	
	if (event.mWindowRef == GetWindowRef())
	{
		DrawerOpened();
		
		handled = true;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
Drawer::WindowDrawerClosing(
	Event<kEventClassWindow, kEventWindowDrawerClosing>&	event)
{
	bool	handled = false;
	
	if (event.mWindowRef == GetWindowRef())
	{
		event.mCancel   = !DrawerClosing();
		handled			= true;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
Drawer::WindowDrawerClosed(
	Event<kEventClassWindow, kEventWindowDrawerClosed>&	event)
{
	bool	handled = false;
	
	if (event.mWindowRef == GetWindowRef())
	{
		DrawerClosed();
		
		handled = true;
	}
	
	return (handled);
}


}	// namespace B
