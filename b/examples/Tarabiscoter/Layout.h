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

#ifndef Layout_H_
#define Layout_H_

#pragma once

// B headers
#include "BToolboxViews.h"
#include "BWindow.h"


class Layout : public B::Window
{
public:

	// constructor
	Layout(
		WindowRef		inWindowRef,
		B::AEObjectPtr	inContainer);
	
protected:
	
	// overrides from B::Window
	virtual void	Awaken(B::Nib* inFromNib);
	
	// CarbonEvent handlers
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);

private:
	
	// types
	typedef B::Window   inherited;
	
	void		UpdateButtons();
	void		UpdateBindingViews(
					const HISideBinding&	inBinding,
					B::View*				inKindPopup,
					B::View*				inViewPopup);
	void		UpdateScaleViews(
					const HIAxisScale&		inScale,
					B::View*				inViewPopup, 
					B::TextField*			inRatio);
	void		UpdatePositionViews(
					const HIAxisPosition&	inPosition,
					B::View*				inKindPopup,
					B::View*				inViewPopup, 
					B::TextField*			inOffset);
	void		UpdateViewPopup(
					HIViewRef				inView,
					B::View*				inPopup);
	void		SetBindingKind(
					HISideBinding			(HIBinding::*inBinding),
					B::View*				inKindPopup,
					B::View*				inViewPopup);
	void		SetBindingView(
					HISideBinding			(HIBinding::*inBinding),
					B::View*				inViewPopup);
	void		SetScalingView(
					HIAxisScale				(HIScaling::*inScaling),
					B::View*				inViewPopup, 
					B::TextField*			inRatio);
	void		SetPositionKind(
					HIAxisPosition			(HIPositioning::*inPosition),
					B::View*				inKindPopup,
					B::View*				inViewPopup, 
					B::TextField*			inOffset);
	void		SetPositionView(
					HIAxisPosition			(HIPositioning::*inPosition),
					B::View*				inViewPopup, 
					B::TextField*			inOffset);
	HIViewRef	GetViewFromPopup(
					B::View*				inPopup);
	float		GetFloatFromTextField(
					B::TextField*			inTextField);
				
	// member variables
	B::View*		mGroupBox;
	B::View*		mVictim;
	B::View*		mSiblingA;
	B::View*		mSiblingB;
	B::View*		mLeftBindingKind;
	B::View*		mLeftBindingView;
	B::View*		mTopBindingKind;
	B::View*		mTopBindingView;
	B::View*		mRightBindingKind;
	B::View*		mRightBindingView;
	B::View*		mBottomBindingKind;
	B::View*		mBottomBindingView;
	B::View*		mHorizontalPositionKind;
	B::View*		mHorizontalPositionView;
	B::TextField*	mHorizontalPositionOffset;
	B::View*		mVerticalPositionKind;
	B::View*		mVerticalPositionView;
	B::TextField*	mVerticalPositionOffset;
	B::View*		mHorizontalScaleView;
	B::TextField*	mHorizontalScaleRatio;
	B::View*		mVerticalScaleView;
	B::TextField*	mVerticalScaleRatio;
};

#endif	// Layout_H_
