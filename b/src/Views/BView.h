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

#ifndef BView_H_
#define BView_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BString.h"
#include "BViewUtils.h"


namespace B {


// forward declarations
class	EventBase;
class	MutableShape;
class	Nib;
class	Rect;
class	ViewFactory;

// ==========================================================================================
//	View

#pragma mark View

/*!	@brief	A client's-eye-view of an <tt>HIViewRef</tt>.
	
	@todo		%Document this class!
	@ingroup	ViewGroup
*/
class View : public boost::noncopyable
{
public:
	
	//! @name types
	//@{
	//! A constant iterator that moves forward through a view's subviews.
	typedef ViewUtils::SubviewIterator			const_iterator;
	//! A constant iterator that moves backward through a view's subviews.
	typedef ViewUtils::ReverseSubviewIterator	const_reverse_iterator;
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the view's text.
	virtual String		GetText() const;
	//! Returns the view's kind.  This is an immutable property of the view.
	HIViewKind	GetKind() const;
	//! Returns the view's full @c HIViewID.
	HIViewID	GetViewID() const;
	//! Returns the view's @c HIViewID::id field.
	SInt32		GetID() const;
	UInt32		GetCommandID() const;
	Rect		GetBounds() const;
	Rect		GetFrame() const;
	std::pair<Rect, float>
				GetBestBounds() const;
	//! Returns the view's current value.
	int			GetValue() const;
	//! Returns the view's minimum value.
	int			GetMinimum() const;
	//! Returns the view's maximum value.
	int			GetMaximum() const;
	//! Returns the view's currently focused part.
	HIViewPartCode	GetFocusPart() const;
	//! Returns whether the view has the focus.
	bool		IsFocused() const;
	//! Returns the view's currently highlighted part.
	HIViewPartCode	GetHighlightPart() const;
	//! Returns whether the view has a highlighted part.
	bool		IsHighlighted() const;
	//! Returns whether the view is active.
	bool		IsActive() const;
	//! Returns whether the view is active, independently of the active state of its parent views.
	bool		IsViewActive() const;
	//! Returns whether the view is enabled.
	bool		IsEnabled() const;
	//! Returns whether the view is enabled, independently of the enabled state of its parent views.
	bool		IsViewEnabled() const;
	//! Returns whether the view is visible.
	bool		IsVisible() const;
    //! Returns whether the view is visible, independently of the visible state of its parent views.
	bool		IsViewVisible() const;
	bool		IsCommandSentToUserFocus() const;
	WindowRef	GetOwner() const;
	void		DebugPrint() const;
	//@}
	
	// modifiers
	virtual void	SetText(const String& inText);
	//! Returns the view's full @c HIViewID.
	void		SetViewID(const HIViewID& inID);
	//! Sets the view's @c HIViewID::id field while preserving its @c HIViewID::signature.
	void		SetID(SInt32 inID);
	void		SetCommandID(UInt32 inCommandID);
	void		SetFrame(const Rect& inNewFrame);
	void		MoveFrameBy(float deltaX, float deltaY);
	void		MoveFrameTo(float x, float y);
	void		ResizeFrameBy(float deltaWidth, float deltaHeight);
	void		ResizeFrameTo(float width, float height);
	void		SetValue(int inValue);
	void		SetMinimum(int inMininum);
	void		SetMaximum(int inMaximum);
	void		SetFocus(HIViewPartCode part);
	void		SetHighlight(HIViewPartCode part);
	void		Activate(bool inActivate);
	void		Enable(bool inEnable);
	void		Show(bool inShow);
	void		SendCommandToUserFocus(bool inSendIt);
	
	// embedding
	void		AddSubview(View* subview);
	void		RemoveFromSuperview();
	
	/*! @name Layout
		
		Binding is the most common way of specifying view layout, so there are 
		a number of convenience functions for specifying the binding of each 
		of a view's sides, or for all sides at once.  These functions work 
		on both Jaguar and Panther, in both composited and non-composited 
		windows.
		
		The GetLayout and SetLayout functions allow access to the full 
		HIViewLayout feature set.  Note that under Jaguar, and in non-composited 
		windows under Panther, only a subset of the layout facility is avaiable.
		In particular, binding to a non-parent view isn't supported.
	*/
	//@{
	//! Sets the binding of the view's left side.
	void		SetLeftBinding(HIBindingKind inBinding);
	//! Sets the binding of the view's top side.
	void		SetTopBinding(HIBindingKind inBinding);
	//! Sets the binding of the view's right side.
	void		SetRightBinding(HIBindingKind inBinding);
	//! Sets the binding of the view's bottom side.
	void		SetBottomBinding(HIBindingKind inBinding);
	//! Sets the bindings of all of the view's sides at once.
	void		SetBindings(
					HIBindingKind	inLeftBinding, 
					HIBindingKind	inTopBinding, 
					HIBindingKind	inRightBinding, 
					HIBindingKind	inBottomBinding);
	//! Sets the view's horizontal scaling.
	void		SetHorizontalScale(
					float			inRatio,
					View*			inView = NULL);
	//! Sets the view's vertical scaling.
	void		SetVerticalScale(
					float			inRatio,
					View*			inView = NULL);
	//! Sets the view's horizontal position.
	void		SetHorizontalPosition(
					HIPositionKind  inPosition, 
					View*			inView = NULL, 
					float			inOffset = 0.0f);
	//! Sets the view's vertical position.
	void		SetVerticalPosition(
					HIPositionKind  inPosition, 
					View*			inView = NULL, 
					float			inOffset = 0.0f);
	//! Sets the view's full layout info.
	void		SetLayout(const HILayoutInfo& inLayout);
	//! Gets the view's full layout info.
	HILayoutInfo	GetLayout() const;
	//! Changes the view's size & location to match its layout information.
	void		ApplyLayout();
	//! Disables layout for this view and its children.
	void		SuspendLayout();
	//! Enables layout for this view and its children.
	void		ResumeLayout();
	//! Returns whether layout is enabled for this view.
	bool		IsLayoutActive() const;
	//! Returns whether layout is enabled for this view, independently of the layout state of its parent views.
	bool		IsViewLayoutActive() const;
	//@}
	
	// conversions
	//! Returns the object's underlying @c HIViewRef.
	operator		HIViewRef () const		{ return (mViewRef); }
	//! Returns the object's underlying @c HIViewRef.
	HIViewRef		GetViewRef() const		{ return (mViewRef); }
	//! Returns the object's underlying @c HIObjectRef.
	HIObjectRef		GetObjectRef() const	{ return (reinterpret_cast<HIObjectRef>(mViewRef)); }
	//! Retrieves the underlying @c EventTargetRef.
	EventTargetRef	GetEventTarget() const	{ return (HIViewGetEventTarget(mViewRef)); }
	
	// common control data
	//! Returns the view's size variant.
	SInt32      GetSize() const;
	//! Sets the view's size variant.
	void		SetSize(SInt32 inSize);
	//! Returns the view's font/style attributes.
	ControlFontStyleRec
				GetFontStyle() const;
	//! Sets the view's font/style attributes.
	void		SetFontStyle(const ControlFontStyleRec& inFontStyle);
	//! Returns the view's action proc.
	ControlActionUPP
				GetActionProc() const;
	//! Sets the view's action proc.
	void		SetActionProc(ControlActionUPP inActionProc);
	
	//! @name Iterators
	//@{
	//! Returns an iterator pointing to the beginning of the view's subviews.
	const_iterator			begin() const;
	//! Returns an iterator pointing to the end of the view's subviews.
	const_iterator			end() const;
	//! Returns an reverse iterator pointing to the beginning of the view's subviews.
	const_reverse_iterator	rbegin() const;
	//! Returns an reverse iterator pointing to the end of the view's subviews.
	const_reverse_iterator	rend() const;
	//@}
	
	//! @name Finding Views
	//@{
	
	//! Returns the View object associated with @a inViewRef.
	static View*	GetViewFromRef(
						HIViewRef		inViewRef);
	//! Returns the View object associated with @a inViewRef, without throwing an exception.
	static View*	GetViewFromRef(
						HIViewRef		inViewRef, 
						const std::nothrow_t&);
	
	//! Returns the properly typed View object associated with @a inViewRef.
	template <class VIEW>
	static VIEW*	GetTypedViewFromRef(
						HIViewRef		inViewRef);
	//! Returns the properly typed View object associated with @a inViewRef, without throwing an exception.
	template <class VIEW>
	static VIEW*	GetTypedViewFromRef(
						HIViewRef		inViewRef, 
						const std::nothrow_t&);
	
	//! Returns the view's subview that matches the main bundle's signature and @a inID.
	HIViewRef		FindSubview(
						int				inID) const;
	/*! @overload
	*/
	HIViewRef		FindSubview(
						const HIViewID&	inID) const;
	//! Returns the view's subview that matches the main bundle's signature and @a inID, or @c NULL.
	HIViewRef		FindSubview(
						int				inID, 
						const std::nothrow_t&) const;
	/*! @overload
	*/
	HIViewRef		FindSubview(
						const HIViewID&	inID, 
						const std::nothrow_t&) const;

	//! Returns the view's properly typed subview that matches the main bundle's signature and @a inID.
	template <class VIEW>
	VIEW*			FindTypedSubview(
						int				inID) const;
	/*! @overload
	*/
	template <class VIEW>
	VIEW*			FindTypedSubview(
						const HIViewID&	inID) const;
	//! Returns the properly typed subview of @a inSuperview that matches the main bundle's signature and @a inID.
	template <class VIEW>
	static VIEW*	FindTypedSubview(
						HIViewRef		inSuperview,
						int				inID);
	/*! @overload
	*/
	template <class VIEW>
	static VIEW*	FindTypedSubview(
						HIViewRef		inSuperview,
						const HIViewID&	inID);
	//@}
	
	//! @name Utility
	//@{
	//! Create instances of View for each subview of @a inSuperView.
	static void	InstantiateSubviews(
					HIViewRef			inSuperView,
					const ViewFactory&	inFactory);
	//! Tell @a inView and its subviews to awaken.
	static void	AwakenViewAndChildren(
					HIViewRef           inView,
					Nib*				inFromNib);
	//@}
	
protected:
	
	// constructor / destructor
	explicit	View(HIViewRef inViewRef);
	virtual		~View();
	
	// member variables
	bool        mAwakened;
	
private:
	
	//! @name Private Overridables
	//@{
	//! The view has just been loaded.
	virtual void	Awaken(Nib* inFromNib);
	//@}
					
	// member variables
	const HIViewRef	mViewRef;
};

// ------------------------------------------------------------------------------------------
/*!	In many cases (ie when the developer completely controls the contents of a window), 
	this function is sufficient for uniquely identifying a view in a window.  This is 
	because all views will presumably have the same @c HIViewID::signature field.
*/
inline SInt32
View::GetID() const
{
	return (GetViewID().id);
}

// ------------------------------------------------------------------------------------------
inline View::const_iterator
View::end() const
{
	const_iterator  it;
	
	return (it);
}

// ------------------------------------------------------------------------------------------
inline View::const_reverse_iterator
View::rend() const
{
	const_reverse_iterator  it;
	
	return (it);
}

// ------------------------------------------------------------------------------------------
/*!	Once the view is found, it is downcast to type @a VIEW.  If the downcast fails, an 
	exception is thrown.
	
	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View 	
					or a class derived from View.
	@return			The requested view.  Never returns @c NULL (an exception is thrown if 
					something goes wrong).
	
	@note	If the caller doesn't care about the specific type of the returned view, 
			GetViewFromRef() may be called instead.
*/
template <class VIEW> VIEW*
View::GetTypedViewFromRef(
	HIViewRef	inViewRef)	//!< The HIView.
{
	boost::function_requires< boost::ConvertibleConcept<VIEW*, B::View*> >();
	
	View*	view		= GetViewFromRef(inViewRef);
	VIEW*	typedView	= dynamic_cast<VIEW*>(view);
	
	if (typedView == NULL)
		B_THROW(std::logic_error("GetTypedViewFromRef : view has wrong type"));
	
	return (typedView);
}

// ------------------------------------------------------------------------------------------
/*!	If the view is found, it is downcast to type @a VIEW.
	
	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View 
					or a class derived from View.
	@return			The requested view, or @c NULL if the view wasn't found or was not of 
					the requested type.
	
	@note	If the caller doesn't care about the specific type of the returned view, 
			GetViewFromRef() may be called instead.
*/
template <class VIEW> VIEW*
View::GetTypedViewFromRef(
	HIViewRef				inViewRef,	//!< The HIView.
	const std::nothrow_t&	nt)			//!< An indication that the caller doesn't want the function to throw.
{
	boost::function_requires< boost::ConvertibleConcept<VIEW*, B::View*> >();
	
	View*	view	= GetViewFromRef(inViewRef, nt);
	
	return ((view != NULL) ? dynamic_cast<VIEW*>(view) : NULL);
}

// ------------------------------------------------------------------------------------------
/*!	@return	The requested view.  Never returns @c NULL (an exception is thrown if 
			something goes wrong).
*/
inline HIViewRef
View::FindSubview(
	const HIViewID&	inID)	//!< The identity of the view we want.
	const
{
	return (ViewUtils::FindSubview(mViewRef, inID));
}

// ------------------------------------------------------------------------------------------
/*!	The signature portion of the HIViewID is taken from the main bundle.
	
	@return	The requested view.  Never returns @c NULL (an exception is thrown if 
			something goes wrong).
*/
inline HIViewRef
View::FindSubview(
	int				inID)	//!< The identity of the view we want.
	const
{
	return (ViewUtils::FindSubview(mViewRef, inID));
}

// ------------------------------------------------------------------------------------------
/*!	@return	The requested subview of @a inSuperview, or @c NULL if the view wasn't found.
*/
inline HIViewRef
View::FindSubview(
	const HIViewID&			inID,	//!< The identity of the view we want.
	const std::nothrow_t&	nt)		//!< An indication that the caller doesn't want the function to throw.
	const
{
	return (ViewUtils::FindSubview(mViewRef, inID, nt));
}

// ------------------------------------------------------------------------------------------
/*!	The signature portion of the HIViewID is taken from the main bundle.
	
	@return	The requested subview of @a inSuperview, or @c NULL if the view wasn't found.
*/
inline HIViewRef
View::FindSubview(
	int						inID,	//!< The identity of the view we want.
	const std::nothrow_t&	nt)		//!< An indication that the caller doesn't want the function to throw.
	const
{
	return (ViewUtils::FindSubview(mViewRef, inID, nt));
}

// ------------------------------------------------------------------------------------------
/*!	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
	@return			The requested view.  Never returns @c NULL (an exception is thrown if something goes wrong).
	
	@note	If the caller doesn't care about the specific type of the returned view, 
			FindSubview() followed by GetViewFromRef() may be called instead.
*/
template <class VIEW> inline VIEW*
View::FindTypedSubview(
	HIViewRef		inSuperview,	//!< The root of the search.
	const HIViewID&	inID)			//!< The identity of the view we want.
{
	return (GetTypedViewFromRef<VIEW>(ViewUtils::FindSubview(inSuperview, inID)));
}

// ------------------------------------------------------------------------------------------
/*!	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
	@return			The requested view.  Never returns @c NULL (an exception is thrown if something goes wrong).
	
	@note	If the caller doesn't care about the specific type of the returned view, 
			FindSubview() followed by GetViewFromRef() may be called instead.
*/
template <class VIEW> inline VIEW*
View::FindTypedSubview(
	HIViewRef		inSuperview,	//!< The root of the search.
	int				inID)			//!< The identity of the view we want.
{
	return (GetTypedViewFromRef<VIEW>(ViewUtils::FindSubview(inSuperview, inID)));
}

// ------------------------------------------------------------------------------------------
/*!	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
	@return			The requested view.  Never returns @c NULL (an exception is thrown if something goes wrong).
	
	@note	If the caller doesn't care about the specific type of the returned view, 
			FindSubview() followed by GetViewFromRef() may be called instead.
*/
template <class VIEW> inline VIEW*
View::FindTypedSubview(
	const HIViewID&	inID)	//!< The identity of the view we want.
	const
{
	return (FindTypedSubview<VIEW>(*this, inID));
}

// ------------------------------------------------------------------------------------------
/*!	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
	@return			The requested view.  Never returns @c NULL (an exception is thrown if something goes wrong).
	
	@note	If the caller doesn't care about the specific type of the returned view, 
			FindSubview() followed by GetViewFromRef() may be called instead.
*/
template <class VIEW> inline VIEW*
View::FindTypedSubview(
	int				inID)	//!< The identity of the view we want.
	const
{
	return (FindTypedSubview<VIEW>(*this, inID));
}


// ==========================================================================================
//	PredefinedView

#pragma mark -
#pragma mark PredefinedView

/*!	@brief	A client's-eye-view of HIToolbox-defined <tt>HIViewRef</tt>s (eg, push button, tabs, etc.)
	
	@todo		%Document this class!
	@ingroup	ViewGroup
*/
class PredefinedView : public View
{
protected:
	
	//! Constructor.
	explicit	PredefinedView(HIViewRef inViewRef);
	
	//! Helper function to set up a programatically-created views.
	static View*	SetupView(
						const HIViewID&		inViewID,
						HIViewRef			inViewRef, 
						HIViewRef			inSuperview, 
						const Rect*			inFrame, 
						Nib*				inFromNib, 
						const ViewFactory*	inFactory);
	
	//! Helper function to set up a programatically-created views.
	template <class VIEW>
	static VIEW*	SetupTypedView(
						const HIViewID&		inViewID,
						HIViewRef			inViewRef, 
						HIViewRef			inSuperview, 
						const Rect*			inFrame, 
						Nib*				inFromNib, 
						const ViewFactory*	inFactory);
	
private:
	
	void	InitEventHandler();
	
	// Carbon %Event handlers
	bool	ControlDispose(
				Event<kEventClassControl, kEventControlDispose>&	event);
	
	// member variables
	EventHandler	mEventHandler;
	
	// friends
	friend class	ViewFactory;
};

// ------------------------------------------------------------------------------------------
/*!	@param	VIEW	Template parameter.  The C/C++ class of the view object.  Must be View 
					or a class derived from View.
	@return			The new view.  Never returns @c NULL (an exception is thrown if 
					something goes wrong).
*/
template <class VIEW> VIEW*
PredefinedView::SetupTypedView(
	const HIViewID&		inViewID,		//!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
	HIViewRef			inViewRef,		//!< The newly created view.
	HIViewRef			inSuperview,	//!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
	const Rect*			inFrame,		//!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
	Nib*				inFromNib,		//!< The nib from which to load other resources.  May be @c NULL.
	const ViewFactory*	inFactory)		//!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
	View*	view;
	VIEW*	typedView;
	
	view		= SetupView(inViewID, inViewRef, inSuperview, inFrame, inFromNib, inFactory);
	typedView	= dynamic_cast<VIEW*>(view);
	
	// If we hit this assert, it means that the type of the view doesn't match the VIEW 
	// template parameter.  This is a logic error and should never happen in the course 
	// of normal program execution.
	
	B_ASSERT(typedView != NULL);
	
	// Throw instead of continuing.
	if (typedView == NULL)
	{
		// Release the view.  This will usually free the View object (since there is 
		// normally only one reference to the view at this point).
		CFRelease(inViewRef);
		
		B_THROW(std::logic_error("PredefinedView::SetupTypedView type mismatch"));
	}
	
	return (typedView);
}


}	// namespace B


#endif	// BView_H_
