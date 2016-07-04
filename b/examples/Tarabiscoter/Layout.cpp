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
#include "Layout.h"

// B headers
#include "BView.h"


enum {
	
	// View IDs
	
	kUserPaneID = 1,
	kVictimID,
	_unused,
	kSiblingAID,
	kSiblingBID,
	
	kLeftBindingKindID,
	kLeftBindingViewID,
	kTopBindingKindID,
	kTopBindingViewID,
	kRightBindingKindID,
	kRightBindingViewID,
	kBottomBindingKindID,
	kBottomBindingViewID,
	
	kHorizontalPositionKindID,
	kHorizontalPositionViewID,
	kHorizontalPositionOffsetID,
	kVerticalPositionKindID,
	kVerticalPositionViewID,
	kVerticalPositionOffsetID,
	
	kHorizontalScaleViewID,
	kHorizontalScaleRatioID,
	kVerticalScaleViewID,
	kVerticalScaleRatioID,
	
	kViewParent = 1,
	kViewSiblingA,
	kViewSiblingB,
	
	kBindingNone = 1,
	kBindingMin,
	kBindingMax,
	
	kPositionNone = 1,
	kPositionMin,
	kPositionCenter,
	kPositionMax
};

// ------------------------------------------------------------------------------------------
Layout::Layout(
	WindowRef		inWindowRef,
	B::AEObjectPtr	inContainer)
		: inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
Layout::Awaken(B::Nib* /* inFromNib */)
{
	mGroupBox					= FindView<B::View>(kUserPaneID);
	mVictim						= FindView<B::View>(kVictimID);
	mSiblingA					= FindView<B::View>(kSiblingAID);
	mSiblingB					= FindView<B::View>(kSiblingBID);
	mLeftBindingKind			= FindView<B::View>(kLeftBindingKindID);
	mLeftBindingView			= FindView<B::View>(kLeftBindingViewID);
	mTopBindingKind				= FindView<B::View>(kTopBindingKindID);
	mTopBindingView				= FindView<B::View>(kTopBindingViewID);
	mRightBindingKind			= FindView<B::View>(kRightBindingKindID);
	mRightBindingView			= FindView<B::View>(kRightBindingViewID);
	mBottomBindingKind			= FindView<B::View>(kBottomBindingKindID);
	mBottomBindingView			= FindView<B::View>(kBottomBindingViewID);
	mHorizontalPositionKind		= FindView<B::View>(kHorizontalPositionKindID);
	mHorizontalPositionView		= FindView<B::View>(kHorizontalPositionViewID);
	mHorizontalPositionOffset	= FindView<B::TextField>(kHorizontalPositionOffsetID);
	mVerticalPositionKind		= FindView<B::View>(kVerticalPositionKindID);
	mVerticalPositionView		= FindView<B::View>(kVerticalPositionViewID);
	mVerticalPositionOffset		= FindView<B::TextField>(kVerticalPositionOffsetID);
	mHorizontalScaleView		= FindView<B::View>(kHorizontalScaleViewID);
	mHorizontalScaleRatio		= FindView<B::TextField>(kHorizontalScaleRatioID);
	mVerticalScaleView			= FindView<B::View>(kVerticalScaleViewID);
	mVerticalScaleRatio			= FindView<B::TextField>(kVerticalScaleRatioID);
	
	mSiblingA->SetHorizontalScale(0.9, mGroupBox);
	mSiblingA->SetHorizontalPosition(kHILayoutPositionCenter, mGroupBox);
	mSiblingA->ApplyLayout();

	UpdateButtons();
}

// ------------------------------------------------------------------------------------------
void
Layout::UpdateButtons()
{
	HILayoutInfo	layout	= mVictim->GetLayout();
	
	UpdateBindingViews(layout.binding.left, mLeftBindingKind, mLeftBindingView);
	UpdateBindingViews(layout.binding.top, mTopBindingKind, mTopBindingView);
	UpdateBindingViews(layout.binding.right, mRightBindingKind, mRightBindingView);
	UpdateBindingViews(layout.binding.bottom, mBottomBindingKind, mBottomBindingView);
	
	UpdateScaleViews(layout.scale.x, mHorizontalScaleView, mHorizontalScaleRatio);
	UpdateScaleViews(layout.scale.y, mVerticalScaleView, mVerticalScaleRatio);
	
	UpdatePositionViews(layout.position.x, mHorizontalPositionKind, mHorizontalPositionView, mHorizontalPositionOffset);
	UpdatePositionViews(layout.position.y, mVerticalPositionKind, mVerticalPositionView, mVerticalPositionOffset);
}

// ------------------------------------------------------------------------------------------
void
Layout::UpdateBindingViews(
	const HISideBinding&	inBinding,
	B::View*				inKindPopup,
	B::View*				inViewPopup)
{
	int value   = 0;
	
	switch (inBinding.kind)
	{
	case kHILayoutBindNone: value = kBindingNone;   break;
	case kHILayoutBindMin:  value = kBindingMin;	break;
	case kHILayoutBindMax:  value = kBindingMax;	break;
	}
	
	inKindPopup->SetValue(value);
	
	if (inBinding.kind == kHILayoutBindNone)
	{
		inViewPopup->Enable(false);
	}
	else
	{
		inViewPopup->Enable(true);
		UpdateViewPopup(inBinding.toView, inViewPopup);
	}
}

// ------------------------------------------------------------------------------------------
void
Layout::UpdateScaleViews(
	const HIAxisScale&		inScale,
	B::View*				inViewPopup, 
	B::TextField*			inRatio)
{
	char	buff[64];
	
	UpdateViewPopup(inScale.toView, inViewPopup);
	
	snprintf(buff, sizeof(buff), "%f", inScale.ratio);
	inRatio->SetText(B::String(buff));
}

// ------------------------------------------------------------------------------------------
void
Layout::UpdatePositionViews(
	const HIAxisPosition&	inPosition,
	B::View*				inKindPopup,
	B::View*				inViewPopup, 
	B::TextField*			inOffset)
{
	int		value   = 0;
	char	buff[64];
	
	switch (inPosition.kind)
	{
	case kHILayoutPositionNone:		value = kPositionNone;		break;
	case kHILayoutPositionCenter:   value = kPositionCenter;	break;
	case kHILayoutPositionMin:		value = kPositionMin;		break;
	case kHILayoutPositionMax:		value = kPositionMax;		break;
	}
	
	inKindPopup->SetValue(value);
	
	snprintf(buff, sizeof(buff), "%f", inPosition.offset);
	
	inViewPopup->Enable(inPosition.kind != kHILayoutPositionNone);
	inOffset->Enable(inPosition.kind != kHILayoutPositionNone);
	inOffset->SetText(B::String(buff));
	UpdateViewPopup(inPosition.toView, inViewPopup);
}

// ------------------------------------------------------------------------------------------
void
Layout::UpdateViewPopup(
	HIViewRef   inView,
	B::View*	inPopup)
{
	int value   = 0;
	
	if ((inView == NULL) || (inView == *mGroupBox))
	{
		value = kViewParent;
	}
	else if (inView == *mSiblingA)
	{
		value = kViewSiblingA;
	}
	else if (inView == *mSiblingB)
	{
		value = kViewSiblingB;
	}
	
	inPopup->SetValue(value);
}

// ------------------------------------------------------------------------------------------
void
Layout::SetBindingKind(
	HISideBinding			(HIBinding::*inBinding),
	B::View*				inKindPopup,
	B::View*				inViewPopup)
{
	HIBindingKind	kind	= kHILayoutBindNone;
	HILayoutInfo	layout;
	HIBinding&		binding = layout.binding;
	
	switch (inKindPopup->GetValue())
	{
	case kBindingNone:  kind = kHILayoutBindNone;   break;
	case kBindingMin:   kind = kHILayoutBindMin;	break;
	case kBindingMax:   kind = kHILayoutBindMax;	break;
	}
	
	layout = mVictim->GetLayout();
	
	(binding.*inBinding).kind = kind;
	
	inViewPopup->Enable(kind != kHILayoutBindNone);
	mVictim->SetLayout(layout);
	mVictim->ApplyLayout();
}

// ------------------------------------------------------------------------------------------
void
Layout::SetBindingView(
	HISideBinding			(HIBinding::*inBinding),
	B::View*				inViewPopup)
{
	HILayoutInfo	layout	= mVictim->GetLayout();
	HIBinding&		binding	= layout.binding;
	
	(binding.*inBinding).toView = GetViewFromPopup(inViewPopup);
	
	mVictim->SetLayout(layout);
	mVictim->ApplyLayout();
}

// ------------------------------------------------------------------------------------------
void
Layout::SetScalingView(
	HIAxisScale				(HIScaling::*inScaling),
	B::View*				inViewPopup, 
	B::TextField*			inRatio)
{
	HILayoutInfo	layout	= mVictim->GetLayout();
	HIScaling&		scaling = layout.scale;
	
	(scaling.*inScaling).toView = GetViewFromPopup(inViewPopup);
	(scaling.*inScaling).ratio	= GetFloatFromTextField(inRatio);
	
	mVictim->SetLayout(layout);
	mVictim->ApplyLayout();
}

// ------------------------------------------------------------------------------------------
void
Layout::SetPositionKind(
	HIAxisPosition			(HIPositioning::*inPosition),
	B::View*				inKindPopup,
	B::View*				inViewPopup, 
	B::TextField*			inOffset)
{
	HIPositionKind	kind		= kHILayoutBindNone;
	HILayoutInfo	layout		= mVictim->GetLayout();
	HIPositioning&	position	= layout.position;
	
	switch (inKindPopup->GetValue())
	{
	case kPositionNone:		kind = kHILayoutPositionNone;	break;
	case kPositionCenter:   kind = kHILayoutPositionCenter;	break;
	case kPositionMin:		kind = kHILayoutPositionMin;	break;
	case kPositionMax:		kind = kHILayoutPositionMax;	break;
	}
	
	(position.*inPosition).kind		= kind;
	(position.*inPosition).offset   = GetFloatFromTextField(inOffset);
	
	inViewPopup->Enable(kind != kHILayoutBindNone);
	inOffset->Enable(kind != kHILayoutBindNone);
	
	mVictim->SetLayout(layout);
	mVictim->ApplyLayout();
}

// ------------------------------------------------------------------------------------------
void
Layout::SetPositionView(
	HIAxisPosition			(HIPositioning::*inPosition),
	B::View*				inViewPopup, 
	B::TextField*			inOffset)
{
	HILayoutInfo	layout		= mVictim->GetLayout();
	HIPositioning&	position	= layout.position;
	
	(position.*inPosition).toView   = GetViewFromPopup(inViewPopup);
	(position.*inPosition).offset   = GetFloatFromTextField(inOffset);
	
	mVictim->SetLayout(layout);
	mVictim->ApplyLayout();
}

// ------------------------------------------------------------------------------------------
HIViewRef
Layout::GetViewFromPopup(
	B::View*				inPopup)
{
	HIViewRef	view	= NULL;
	
	switch (inPopup->GetValue())
	{
	case kViewParent:   view = NULL;		break;
	case kViewSiblingA: view = *mSiblingA;  break;
	case kViewSiblingB: view = *mSiblingB;  break;
	}
	
	return (view);
}

// ------------------------------------------------------------------------------------------
float
Layout::GetFloatFromTextField(
	B::TextField*			inTextField)
{
	return (CFStringGetDoubleValue(inTextField->GetText().cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
Layout::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case 'LBKi':
		SetBindingKind(&HIBinding::left, mLeftBindingKind, mLeftBindingView);
		break;
		
	case 'TBKi':
		SetBindingKind(&HIBinding::top, mTopBindingKind, mTopBindingView);
		break;
		
	case 'RBKi':
		SetBindingKind(&HIBinding::right, mRightBindingKind, mRightBindingView);
		break;
		
	case 'BBKi':
		SetBindingKind(&HIBinding::bottom, mBottomBindingKind, mBottomBindingView);
		break;
		
	case 'LBVi':
		SetBindingView(&HIBinding::left, mLeftBindingView);
		break;
		
	case 'TBVi':
		SetBindingView(&HIBinding::top, mTopBindingView);
		break;
		
	case 'RBVi':
		SetBindingView(&HIBinding::right, mRightBindingView);
		break;
		
	case 'BBVi':
		SetBindingView(&HIBinding::bottom, mBottomBindingView);
		break;
		
	case 'HPKi':
		SetPositionKind(&HIPositioning::x, mHorizontalPositionKind, mHorizontalPositionView, mHorizontalPositionOffset);
		break;
		
	case 'VPKi':
		SetPositionKind(&HIPositioning::y, mVerticalPositionKind, mVerticalPositionView, mVerticalPositionOffset);
		break;
		
	case 'HPVi':
		SetPositionView(&HIPositioning::x, mHorizontalPositionView, mHorizontalPositionOffset);
		break;
		
	case 'VPVi':
		SetPositionView(&HIPositioning::y, mVerticalPositionView, mVerticalPositionOffset);
		break;
		
	case 'HSVi':
		SetScalingView(&HIScaling::x, mHorizontalScaleView, mHorizontalScaleRatio);
		break;
		
	case 'VSVi':
		SetScalingView(&HIScaling::y, mVerticalScaleView, mVerticalScaleRatio);
		break;
		
	case 'SibA':
		ResizeContentBy(0.0, -10.0);
		break;
		
	default:
		handled = inherited::HandleCommand(inHICommand);
		break;
	}
	
	return (handled);
}
