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

#ifndef BHybridView_H_
#define BHybridView_H_

#pragma once

// B headers
#include "BCustomView.h"
#include "BRect.h"
#include "BView.h"


namespace B {

/*!	@brief	Base class for implementations of <tt>HIView</tt> that can also be used by clients of the view.
	
	@todo		%Document this class!
	@ingroup	ViewGroup
*/
class HybridView : public CustomView, public View
{
public:
	
	//! @name Object Creation
	//@{
	//! Creates an instance of class @a T.
	template <class T>
	static std::pair<OSPtr<HIViewRef>, T*>
			Create(
				const HIViewID&	inViewID,
				HIViewRef		inSuperview, 
				const Rect*		inFrame,
                Nib*			inFromNib,
				Collection		inCollection);
	//! Creates an instance of class @a T.
	template <class T>
	static std::pair<OSPtr<HIViewRef>, T*>
			Create(
				const HIViewID&	inViewID,
				HIViewRef		inSuperview, 
				const Rect*		inFrame,
                Nib*			inFromNib,
				EventRef		inEvent);
	//@}
	
	// inquiries
	HIViewRef	GetViewRef() const					{ return (View::GetViewRef()); }
	Rect		GetBounds() const					{ return (View::GetBounds()); }
	Rect		GetFrame() const					{ return (View::GetFrame()); }
	int			GetValue() const					{ return (View::GetValue()); }
	int			GetMinimum() const					{ return (View::GetMinimum()); }
	int			GetMaximum() const					{ return (View::GetMaximum()); }
	String		GetText() const                     { return (View::GetText()); }
	bool		IsFocused() const					{ return (CustomView::IsFocused()); }
	bool		IsHighlighted() const				{ return (View::IsHighlighted()); }
	bool		IsActive() const					{ return (View::IsActive()); }
	bool		IsEnabled() const					{ return (View::IsEnabled()); }
	bool		IsVisible() const					{ return (View::IsVisible()); }
	bool		IsCommandSentToUserFocus() const	{ return (View::IsCommandSentToUserFocus()); }
	WindowRef	GetOwner() const					{ return (View::GetOwner()); }
	void		DebugPrint() const					{ View::DebugPrint(); }
	HIObjectRef		GetObjectRef() const			{ return (EventTarget::GetObjectRef()); }
	EventTargetRef	GetEventTarget() const			{ return (View::GetEventTarget()); }
	HIViewPartCode	GetFocusPart() const			{ return (CustomView::GetFocusPart()); }
	HIViewPartCode	GetHighlightPart() const		{ return (View::GetHighlightPart()); }
	
	// modifiers
	void	SetFrame(const Rect& fr)				{ View::SetFrame(fr); }
	void	MoveFrameBy(float dx, float dy)			{ View::MoveFrameBy(dx, dy); }
	void	MoveFrameTo(float x, float y)			{ View::MoveFrameTo(x, y); }
	void	ResizeFrameBy(float dw, float dh)		{ View::ResizeFrameBy(dw, dh); }
	void	ResizeFrameTo(float w, float h)			{ View::ResizeFrameTo(w, h); }
	void	SetValue(int inValue)					{ View::SetValue(inValue); }
	void	SetMinimum(int inMininum)				{ View::SetMinimum(inMininum); }
	void	SetMaximum(int inMaximum)				{ View::SetMaximum(inMaximum); }
	void	SetText(const String& inText)			{ View::SetText(inText); }
	void	SetHighlight(HIViewPartCode inPart)		{ View::SetHighlight(inPart); }
	void	Activate(bool inActivate)				{ View::Activate(inActivate); }
	void	Enable(bool inEnable)					{ View::Enable(inEnable); }
	void	Show(bool inShow)						{ View::Show(inShow); }
	void	SendCommandToUserFocus(bool inSendIt)	{ View::SendCommandToUserFocus(inSendIt); }
	
protected:
	
	// constructor / destructor
			HybridView(
				HIObjectRef	inObjectRef, 
				EViewFlags	inViewFlags);
	
	virtual void	Awaken(Nib* inNib);
	
	// Carbon %Event handlers
	virtual void	InitializeView(
						Collection	inCollection);
	virtual void	OwningWindowChanged(
						WindowRef	inOriginalWindow, 
						WindowRef	inCurrentWindow);
};

// ------------------------------------------------------------------------------------------
/*!
	@param	T	The C/C++ type of the view.  Must be HybridView or derived from 
				HybridView.
*/
template <class T>
std::pair<OSPtr<HIViewRef>, T*>
HybridView::Create(
	const HIViewID&	inViewID,		//!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
	HIViewRef		inSuperview,	//!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
	const Rect*		inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
	Nib*			inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
	Collection		inCollection)   //!< Initialisation data for the new view;  may be @c NULL.
{
	OSPtr<HIViewRef>	viewPtr;
	T*					viewObj;
	
	viewPtr = CustomView::Create<T>(inViewID, inSuperview, inFrame, inFromNib, inCollection);
	viewObj = InstanceData<T>(reinterpret_cast<HIObjectRef>(viewPtr.get()));
	
	B_ASSERT(viewObj != NULL);
	
	return (std::pair<OSPtr<HIViewRef>, T*>(viewPtr, viewObj));
}

// ------------------------------------------------------------------------------------------
/*!
	@param	T	The C/C++ type of the view.  Must be HybridView or derived from 
				HybridView.
*/
template <class T>
std::pair<OSPtr<HIViewRef>, T*>
HybridView::Create(
	const HIViewID&	inViewID,		//!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
	HIViewRef		inSuperview,	//!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
	const Rect*		inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
	Nib*			inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
	EventRef		inEvent)        //!< Initialisation data for the new view;  may be @c NULL.
{
	OSPtr<HIViewRef>	viewPtr;
	T*					viewObj;
	
	viewPtr = CustomView::Create<T>(inViewID, inSuperview, inFrame, inFromNib, inEvent);
	viewObj = InstanceData<T>(reinterpret_cast<HIObjectRef>(viewPtr.get()));
	
	B_ASSERT(viewObj != NULL);
	
	return (std::pair<OSPtr<HIViewRef>, T*>(viewPtr, viewObj));
}


}	// namespace B


#endif	// BHybridView_H_
