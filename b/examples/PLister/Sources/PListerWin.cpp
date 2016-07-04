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
#include "PListerWin.h"

// library headers
#include <boost/bind.hpp>

// B headers
#include "BDataBrowser.h"
#include "BEvent.h"

// project headers
#include "Browser.h"
#include "BrowserItem.h"

// for template instantiation
#include "BDocumentWindow.tpl.h"


namespace {
	
	const int		kBrowserID		= 1;
	const int		kNewSiblingID	= 2;
	const int		kDeleteID		= 3;
	const int		kNewChildID		= 4;
	const int		kMoveDownID		= 5;
	const int		kMoveUpID		= 6;
	
}	// anonymous namespace


// ------------------------------------------------------------------------------------------
PListerWin::PListerWin(
	WindowRef		inWindowRef,
	PListerDoc*		inDocument)
		: inherited(inWindowRef, inDocument), 
		  mPListerDoc(inDocument), mBrowser(NULL)
{
}

// ------------------------------------------------------------------------------------------
void
PListerWin::RegisterViews()
{
	inherited::RegisterViews();
	
	Register<Browser>(kHIDataBrowserClassID, kBrowserID);
}

// ------------------------------------------------------------------------------------------
void
PListerWin::Awaken(B::Nib* inFromNib)
{
	inherited::Awaken(inFromNib);
	
	::Rect	bounds;
	B::Size	size;
	
	GetWindowPortBounds(GetWindowRef(), &bounds);

	size.width	= (bounds.right - bounds.left) / 2;
	size.height	= (bounds.bottom - bounds.top) / 2;
	SetMinimumSize(size);

	size.width	= (bounds.right - bounds.left) * 3;
	size.height	= (bounds.bottom - bounds.top) * 3;
	SetMaximumSize(size);
	
	mNewChildButton		= FindView<B::View>(kNewChildID);
	mNewSiblingButton   = FindView<B::View>(kNewSiblingID);
	mDeleteButton		= FindView<B::View>(kDeleteID);
	mMoveDownButton		= FindView<B::View>(kMoveDownID);
	mMoveUpButton		= FindView<B::View>(kMoveUpID);
	mBrowser			= FindView<Browser>(kBrowserID);
	
	mMoveDownButton->SetRightBinding(kHILayoutBindRight);
	mMoveUpButton->SetRightBinding(kHILayoutBindRight);
	mBrowser->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
						  kHILayoutBindRight, kHILayoutBindBottom);
	
	mBrowser->SetUndoTarget(mDocument->GetEventTarget());
	
	// Register for signals
	
	mBrowser->GetSelectionChangedSignal().connect(
				boost::bind(&PListerWin::BrowserSelectionChanged, this));
	
	mBrowser->GetUserStateChangedSignal().connect(
				boost::bind(&PListerWin::BrowserUserStateChanged, this));
	
	B::OSPtr<CFDictionaryRef>	browserState	= mPreferences.GetPtr<CFDictionaryRef>("Browser State");
	
	if (browserState != NULL)
		mBrowser->SetUserState(browserState);
}

// ------------------------------------------------------------------------------------------
void
PListerWin::LoadContent()
{
	mBrowser->RemoveAllItems();
	mBrowser->MakeRootItem(mPListerDoc->GetRootItem());
	
	UpdateButtons();
}

// ------------------------------------------------------------------------------------------
B::Size
PListerWin::GetIdealSize() const
{
	B::Rect	bestRect;
	
	boost::tie(bestRect, boost::tuples::ignore) = mBrowser->GetBestBounds();
	
	bestRect.size.width		+= bestRect.origin.x;
	bestRect.size.height	+= bestRect.origin.y;
	
	return (B::Size(bestRect.size));
}

// ------------------------------------------------------------------------------------------
bool
PListerWin::HandleFocusAcquired()
{
//	mBrowser->SetFocus(kControlDataBrowserPart);
	
	return (true);
}

// ------------------------------------------------------------------------------------------
void
PListerWin::UpdateButtons()
{
	mNewChildButton->Enable(mBrowser->CanAddChildToSelection());
	mNewSiblingButton->Enable(mBrowser->CanAddSiblingToSelection());
	mDeleteButton->Enable(mBrowser->IsSelectionDeletable());
	mMoveDownButton->Enable(mBrowser->IsSelectionMovable());
	mMoveUpButton->Enable(mBrowser->IsSelectionMovable());
}

// ------------------------------------------------------------------------------------------
bool
PListerWin::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case Browser::kNewSiblingCmdID:
		mBrowser->NewSibling();
		break;
		
	case Browser::kNewChildCmdID:
		mBrowser->NewChild();
		break;
		
	case Browser::kDuplicateCmdID:
		mBrowser->CloneSelection();
		break;
		
	case kHICommandCut:
		mBrowser->CutSelection();
		break;
		
	case kHICommandCopy:
		mBrowser->CopySelection();
		break;
		
	case kHICommandClear:
		mBrowser->DeleteSelection();
		break;
		
	default:
		handled = inherited::HandleCommand(inHICommand);
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
PListerWin::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	B::CommandData&				ioCmdData)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case Browser::kNewChildCmdID:
		ioCmdData.SetEnabled(mBrowser->CanAddChildToSelection());
		break;
	
	case Browser::kNewSiblingCmdID:
		ioCmdData.SetEnabled(mBrowser->CanAddSiblingToSelection());
		break;
	
	case Browser::kDuplicateCmdID:
		ioCmdData.SetEnabled(mBrowser->IsSelectionClonable());
		break;
		
	// Edit menu commands
	case kHICommandCut:
		ioCmdData.SetEnabled(mBrowser->CanCutSelection());
		break;
		
	case kHICommandCopy:
		ioCmdData.SetEnabled(mBrowser->CanCopySelection());
		break;
		
	case kHICommandClear:
		ioCmdData.SetEnabled(mBrowser->IsSelectionDeletable());
		break;
		
//	case kHICommandPaste:
		
//	case kHICommandCut:
//	case kHICommandCopy:
//	case kHICommandPaste:
//		if (!IsEditSessionOpen())
//		{
//			switch (inHICommand.commandID)
//			{
//			case kHICommandCut:
//			case kHICommandClear:
//				ioCmdData.SetEnabled(IsSelectionDeletable());
//				break;
//				
//			case kHICommandCopy:
//				ioCmdData.SetEnabled(GetFirstSelectedItem() != NULL);
//				break;
//				
//			case kHICommandPaste:
//				break;
//			}
//		}
//		else
//		{
//			handled = inherited::HandleUpdateStatus(inHICommand, ioCmdData);
//		}
//		break;
		
	default:
		handled = inherited::HandleUpdateStatus(inHICommand, ioCmdData);
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
void
PListerWin::BrowserSelectionChanged()
{
	UpdateButtons();
}

// ------------------------------------------------------------------------------------------
void
PListerWin::BrowserUserStateChanged()
{
	mPreferences.SetPtr("Browser State", mBrowser->GetUserState());
	mPreferences.Flush();
}
