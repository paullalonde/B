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
#include "DrawerHost.h"

// B headers
#include "BView.h"
#include "BWindowUtils.h"


enum {
	kLeftButtonID = 1,
	kRightButtonID,
	kBottomButtonID
};

// ------------------------------------------------------------------------------------------
DrawerHost::DrawerHost(
	WindowRef		inWindowRef,
	B::AEObjectPtr	inContainer)
		: inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
DrawerHost::Awaken(B::Nib* inFromNib)
{
	B_ASSERT(inFromNib != NULL);
	
	B::View*	button;
	
	// The "Left" button hugs the left edge and is vertically centered.
	button = FindView<B::View>(kLeftButtonID);
	button->SetVerticalPosition(kHILayoutPositionCenter);
	
	// The "Right" button hugs the right edge and is vertically centered.
	button = FindView<B::View>(kRightButtonID);
	button->SetVerticalPosition(kHILayoutPositionCenter);
	
	// The "Bottom" button hugs the bottom edge and is horizontally centered.
	button = FindView<B::View>(kBottomButtonID);
	button->SetHorizontalPosition(kHILayoutPositionCenter);
	
	mLeftDrawer		= InitVerticalDrawer(*inFromNib, kWindowEdgeLeft);
	mRightDrawer	= InitVerticalDrawer(*inFromNib, kWindowEdgeRight);
	mBottomDrawer	= InitHorizontalDrawer(*inFromNib, kWindowEdgeBottom);
}

// ------------------------------------------------------------------------------------------
boost::shared_ptr<B::Drawer>
DrawerHost::InitHorizontalDrawer(
	B::Nib&		inNib,
	OptionBits	inEdge)
{
	boost::shared_ptr<B::Drawer>	drawer(B::Window::CreateFromNib<B::Drawer>(
												inNib, "DrawerH", GetAEObjectPtr()));
	B::Rect		bounds;
	HISize		minSize, maxSize;
	OSStatus	err;
	
	bounds = B::WindowUtils::GetContentViewBounds(drawer->GetWindowRef());
	
	// Set an artificial limit on the resizing of the drawer.
	
	err = GetWindowResizeLimits(drawer->GetWindowRef(), &minSize, &maxSize);
	B_THROW_IF_STATUS(err);
	
	minSize.height = bounds.size.height / 2;
	maxSize.height = bounds.size.height * 2;
	
	err = SetWindowResizeLimits(drawer->GetWindowRef(), &minSize, &maxSize);
	B_THROW_IF_STATUS(err);
	
	// Set up the drawer's location and connect it to its parent.
	
	drawer->SetParent(GetWindowRef());
	drawer->SetPreferredEdge(inEdge);
	drawer->SetOffsets(15, 15);
	
	return drawer;
}

// ------------------------------------------------------------------------------------------
boost::shared_ptr<B::Drawer>
DrawerHost::InitVerticalDrawer(
	B::Nib&		inNib,
	OptionBits	inEdge)
{
	boost::shared_ptr<B::Drawer>	drawer(B::Window::CreateFromNib<B::Drawer>(
												inNib, "DrawerV", GetAEObjectPtr()));
	B::Rect		bounds;
	HISize		minSize, maxSize;
	OSStatus	err;
	
	bounds = B::WindowUtils::GetContentViewBounds(drawer->GetWindowRef());
	
	// Set an artificial limit on the resizing of the drawer.
	
	err = GetWindowResizeLimits(drawer->GetWindowRef(), &minSize, &maxSize);
	B_THROW_IF_STATUS(err);
	
	minSize.width = bounds.size.width / 2;
	maxSize.width = bounds.size.width * 2;
	
	err = SetWindowResizeLimits(drawer->GetWindowRef(), &minSize, &maxSize);
	B_THROW_IF_STATUS(err);
	
	// Set up the drawer's location and connect it to its parent.
	
	drawer->SetParent(GetWindowRef());
	drawer->SetPreferredEdge(inEdge);
	drawer->SetOffsets(0, 15);
	
	return drawer;
}

// ------------------------------------------------------------------------------------------
bool
DrawerHost::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case 'Left':
		if (mLeftDrawer->IsOpen())
			mLeftDrawer->Close();
		else
			mLeftDrawer->Open();
		break;
		
	case 'Righ':
		mRightDrawer->Toggle();
		break;
		
	case 'Bott':
		mBottomDrawer->Toggle();
		break;
		
	default:
		handled = inherited::HandleCommand(inHICommand);
		break;
	}
	
	return (handled);
}
