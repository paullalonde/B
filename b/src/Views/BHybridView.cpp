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
#include "BHybridView.h"

// B headers
#include "BEvent.h"


namespace B {

// ------------------------------------------------------------------------------------------
HybridView::HybridView(
	HIObjectRef			inObjectRef, 
	EViewFlags			inViewFlags)
		: CustomView(inObjectRef, inViewFlags), 
		  View(reinterpret_cast<HIViewRef>(inObjectRef))
{
}

// ------------------------------------------------------------------------------------------
/*!	Perform any initialisation of the view.  Derived classes may override 
	this to set up the view's initial state.  Such overrides should call 
	the base class implementation prior to performing work of their own.
*/
void
HybridView::InitializeView(
	Collection		inCollection)
{
	CustomView::InitializeView(inCollection);
}

// ------------------------------------------------------------------------------------------
void
HybridView::OwningWindowChanged(
	WindowRef		inOriginalWindow, 
	WindowRef		inCurrentWindow)
{
	CustomView::OwningWindowChanged(inOriginalWindow, inCurrentWindow);
}

// ------------------------------------------------------------------------------------------
void
HybridView::Awaken(
	Nib*	/* inNib */)
{
	View::mAwakened = CustomView::mAwakened = true;
}

}	// namespace B
