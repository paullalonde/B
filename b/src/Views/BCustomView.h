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

#ifndef BCustomView_H_
#define BCustomView_H_

#pragma once

// B headers
#include "BEventHandler.h"
#include "BEventCustomParams.h"
#include "BEventTarget.h"
#include "BString.h"


namespace B {

// forward declarations
class	MutableShape;
class	Nib;
class	Point;
class	Rect;
class	Shape;
class	Size;


/*!
	@brief	Base class for custom derivatives of HIView.
	
	This class regroups a lot of the functionality commonly required to implement a 
	custom HIView (i.e., an object known to the HIToolbox which "derives" from the 
	HIToolbox class @c kHIViewClassID).  The supported functionality includes:
	
	- Drawing
	- Hit-testing
	- Mouse tracking
	- Hit handling
	- Retrieving part shapes & bounds
	- Retrieving min/max/optimal view sizes.
	- Focus, including text input
	- Getting & setting view data
	- Automatic view invalidation when certain state changes occur.
	- Drag & drop
	- Scrolling
	- Commands
	- Mouse-region tracking.
	
	Objects of classes derived from CustomView may be instantiated via @c HIObjectCreate() 
	or @c CreateCustomControl().
	
	CustomView doesn't have much in the way of a public interface.  That's because 
	once a CustomView is instantiated, its public interface is via its @c HIViewRef, 
	and not the C++ object.  Communication between the view and its clients is therefore 
	done via the HIView & Control Manager APIs.  View-specific information may be 
	communicated via @c GetControlData() & @c SetControlData().
	
	The most important piece of information that derived classes provide to CustomView 
	is the EViewFlags.  This is bitfield that tells CustomView what kind of functionality 
	is required by the derived class.  For example, if the @c kViewDragAndDrop flag is 
	set, the drag&drop-related Carbon %Event handlers will be installed on the HIView.  
	The view flags also tell CustomView which state changes will cause the view to be 
	invalidated.  For example, if the @c kViewInvalOnValue flag is set, then changing the 
	view's value will automatically cause the view to redraw itself.
	
	Obviously, if a derived class needs to handle a Carbon %Event that isn't supported 
	directly by CustomView, then it can declare an EventHandler member variable and add 
	handlers to it.
	
	Besides overriding functions that define the view's behaviour with respect to user 
	interface events, there are some informational functions that will often need to 
	be overriden as well.  These include @c GetControlKind(), @c GetViewParts(), 
	and possibly also @c CalcViewFeatures() and @c CalcControlFeatures().
	
	@ingroup	ViewGroup
*/
class CustomView : public EventTarget
{
public:
	
	/*!	@brief Flags controlling view behaviour
		
		Many of these flags affect the Carbon %Events handled by the view.  Others 
		determine which state changes cause the view to be redrawn.
		
		These flags may be combined in a bitwise fashion.
	*/
	enum EViewFlags
	{
		kViewNone				= 0x00000000,	//!< Default flag value.
		kViewOpaque				= 0x00000001,	//!< The view contains no translucent areas.
		kViewFocus				= 0x00000002,	//!< The view can acquire the user input focus.
		kViewFocusOnClick		= 0x00000004,	//!< The view can acquire the user input focus.
		kViewContainer			= 0x00000008,	//!< The view can embed other views within itself.
		kViewDragAndDrop		= 0x00000010,	//!< The view supports drag & drop.
		kViewScrollable			= 0x00000020,	//!< The view can be placed in an @c HIScrollView.
		kViewCommands			= 0x00000040,	//!< The view supports @c HICommands.  This usually (but not necessarily) only makes sense if the view can also get the focus.
		kViewHit				= 0x00000080,	//!< The view wants the ControlHit event.
		kViewTracking			= 0x00000100,	//!< The view performs its own mouse tracking.
		kViewTrackingArea		= 0x00000200,	//!< The view makes use of view tracking areas.
		kViewLiveFeedback		= 0x00000400,	//!< The view implements the live feedback protocol.
		kViewClickActivation	= 0x00000800,	//!< The view wants to be asked what to do when it's clicked when its containing window is inactive.
		kViewNoDrawing			= 0x00010000,	//!< The view doesn't draw at all.
		kViewNoClicking			= 0x00020000,	//!< The view doesn't respond to clicks.
		kViewInvalOnActivate	= 0x00100000,	//!< Automatically invalidate the view when its active state changes.
		kViewInvalOnHilite		= 0x00200000,	//!< Automatically invalidate the view when its hilited state changes.
		kViewInvalOnEnable		= 0x00400000,	//!< Automatically invalidate the view when its enabled state changes.
		kViewInvalOnValue		= 0x00800000,	//!< Automatically invalidate the view when one of its value/min/max fields changes.
		kViewInvalOnTitle		= 0x01000000,	//!< Automatically invalidate the view when its title changes.
		kViewInvalOnFocus		= 0x02000000	//!< Automatically invalidate the view when its focused part changes.
	};
	
	/*!	@brief The focus behaviour of a view part
		
		The focus behaviour of each part of a view can be specified individually 
		via one of the EPartFocus enums.
	*/
	enum EViewPartFocus
	{
		kFocusAlways = 1,	//!< The part is always focusable.
		kFocusNever,		//!< The part is never focusable.
		kFocusFull			//!< The part is only focusable in full keyboard access mode.
	};
	
	//! Associates a view part (identified by its part code) and a focus behaviour.
	typedef std::pair<HIViewPartCode, EViewPartFocus>	ViewPart;
	//! Defines all of a view's parts and their focus behaviour.
	typedef std::vector<ViewPart>						ViewPartVector;
	
	//! The default @c HIObject base class for views.
	static const CFStringRef	kHIObjectBaseClassID;
	
	//! @name Object Creation
	//@{
	//! Creates an instance of class @a T.
	template <class T>
	static OSPtr<HIViewRef>
			Create(
				const HIViewID&	inViewID,
				HIViewRef		inSuperview, 
				const Rect*		inFrame,
                Nib*			inFromNib,
				Collection		inCollection);
	//! Creates an instance of class @a T.
	template <class T>
	static OSPtr<HIViewRef>
			Create(
				const HIViewID&	inViewID,
				HIViewRef		inSuperview, 
				const Rect*		inFrame,
                Nib*			inFromNib,
				EventRef		inEvent);
	//! Creates an instance of class @a inClassID.
	static OSPtr<HIViewRef>
			Create(
				CFStringRef		inClassID,
				const HIViewID&	inViewID,
				HIViewRef		inSuperview, 
				const Rect*		inFrame,
                Nib*			inFromNib,
				EventRef		inEvent);
	//@}
	
	//! @name Conversions
	//@{
	//! Returns the CustomView object belonging to @a inViewRef, or @c NULL.
	static CustomView*	FromViewRef(HIViewRef inViewRef);
	//@}
	
	//! @name Utility
	//@{
	//! Tell @a inView to awaken.
	static void	AwakenView(
					HIViewRef	inView,
					Nib*		inFromNib);
	//@}
	
protected:
	
	//! @name Constructor / Destructor
	//@{
	//! Constructor.
			CustomView(
				HIObjectRef	inObjectRef, 
				EViewFlags	inViewFlags);
	//@}
	
	/*! @name Inquiries
		
		These functions are for the benefit of derived classes, and may be 
		used to retrieve a part of the view's state that is managed by 
		the HIToolbox.
	*/
	//@{
	//! Returns the view's underlying @c HIViewRef.
	HIViewRef	GetViewRef() const;
	//! Returns the view's full @c HIViewID.
	HIViewID	GetViewID() const;
	//! Returns the view's bounds.
	Rect		GetBounds() const;
	//! Returns the view's bounds.
	Rect		GetFrame() const;
	//! Returns the view's current value.
	int			GetValue() const;
	//! Returns the view's minimum value.
	int			GetMinimum() const;
	//! Returns the view's maximum value.
	int			GetMaximum() const;
	//! Returns the view's title, if any.
	String		GetTitle() const;
	//! Returns whether the view is focused.
	bool		IsFocused() const;
	//! Returns whether the view is highlighted.
	bool		IsHighlighted() const;
	//! Returns whether the view is active.
	bool		IsActive() const;
	//! Returns whether the view is enabled.
	bool		IsEnabled() const;
	//! Returns whether the view is visible.
	bool		IsVisible() const;
	//! Returns whether the view sends commands to the user focus.
	bool		IsCommandSentToUserFocus() const;
	//! Returns whether the view may contain other views.
	bool		IsContainer() const;
	//! Returns whether the view is currently in a composited window.
	bool		IsCompositing() const;
	//! Returns the view's enclosing window, if any.
	WindowRef	GetOwner() const;
	//! Returns the view's currently focused part, if any.
	HIViewPartCode	GetFocusPart() const;
	//! Returns the view's currently highlighted part, if any.
	HIViewPartCode	GetHighlightPart() const;
	//! Prints debugging information to @c stdout.
	void		DebugPrint() const;
	//@}
	
	/*! @name Modifiers
		
		These functions are for the benefit of derived classes, and may be 
		used to change a part of the view's state that is managed by 
		the HIToolbox.
	*/
	//@{
	void	SetFrame(const Rect& inNewFrame);
	void	MoveFrameBy(float deltaX, float deltaY);
	void	MoveFrameTo(float x, float y);
	void	ResizeFrameBy(float deltaWidth, float deltaHeight);
	void	ResizeFrameTo(float width, float height);
	//! Changes the view's origin (useful for scrollable views).
	void	SetOrigin(const HIPoint& inOrigin);
	//! Changes the view's current value.
	void	SetValue(int inValue);
	//! Changes the view's minimum value.
	void	SetMinimum(int inMininum);
	//! Changes the view's maximum value.
	void	SetMaximum(int inMaximum);
	//! Changes the view's title.
	void	SetTitle(const String& inTitle);
	//! Changes the view's highlighted part.
	void	SetHighlight(HIViewPartCode inPart);
	//! Changes the view's active state.
	void	Activate(bool inActivate);
	//! Changes the view's enabled state.
	void	Enable(bool inEnable);
	//! Changes the view's visible state.
	void	Show(bool inShow);
	//! Changes the destination of @c HICommands generated by the view.
	void	SendCommandToUserFocus(bool inSendIt);
	//! Notifies the @c HIView system that the view's contours have changed.
	void	Reshape();
	//! Invalidates the entire view.
	void	Invalidate();
	//! Invalidates a portion of the view.
	void	Invalidate(const Rect& inRect);
	//! Invalidates a portion of the view.
	void	Invalidate(const Shape& inShape);
	//! Invalidates a portion of the view.
	void	Invalidate(const MutableShape& inShape);
	//! Invalidates one of the view's parts.
	void	InvalidatePart(HIViewPartCode inPart);
	//! Adds a subview to this view.
	void	AddSubview(HIViewRef subview);
	//! Removes a subview.
	static void	RemoveSubview(HIViewRef subview);
	//@}
	
	//! @name Overridables
	//@{
	//! View initialisation.
	virtual void	InitializeView(
						Collection		inCollection);
	//! View cleanup.
	virtual void	DisposeView();
	//! Returns the view parts defined by this view.
	virtual const ViewPartVector&
					GetViewParts() const;
	//! Returns the view's @c ControlKind.
	virtual ControlKind
					GetKind() const = 0;
#if B_BUILDING_CAN_USE_10_3_APIS
	//! Computes the @c HIViewFeatures for this view.
	virtual HIViewFeatures
					CalcViewFeatures() const;
#endif
	//! Computes the Control Manager control features for this view.
	virtual UInt32	CalcControlFeatures() const;
	//! Draws the view.
	virtual void	Draw(
						HIViewPartCode	inPart, 
						CGContextRef	inContext, 
						const Shape*    inDrawShape);
	//! Returns the view's structure shape.
	virtual Shape	GetStructureShape() const;
	//! Returns the view's content shape.
	virtual Shape	GetContentShape() const;
	//! Returns the view's opaque shape.
	virtual Shape	GetOpaqueShape() const;
	//! Returns the view's clickable shape.
	virtual Shape	GetClickableShape() const;
	//! Returns the shape of one of the view's parts.
	virtual Shape	GetPartShape(
						HIViewPartCode	inPart) const;
	//! High-level function that returns the shape of one of the view's parts.
	virtual bool	GetViewShape(
						HIViewPartCode	inPart,
                        Shape&          outShape) const;
	//! Returns the bounds of the view's structure shape.
	virtual Rect	GetStructureBounds() const;
	//! Returns the bounds of the view's content shape.
	virtual Rect	GetContentBounds() const;
	//! Returns the bounds of the view's opaque shape.
	virtual Rect	GetOpaqueBounds() const;
	//! Returns the bounds of the view's clickable shape.
	virtual Rect	GetClickableBounds() const;
	//! Returns the bounds of the shape of one of the view's parts.
	virtual Rect	GetPartBounds(
						HIViewPartCode	inPart) const;
	//! High-level function that returns the bounds of the shape of one of the view's parts.
	virtual bool	GetViewBounds(
						HIViewPartCode	inPart, 
						Rect&			outBounds) const;
	//! Returns the view part containing @a inMouseLoc, if any.
	virtual HIViewPartCode
					HitTest(
						const Point&	inMouseLoc) const;
	//! The view was hit in @a inPart.
	virtual void	Hit(
						HIViewPartCode	inPart, 
						UInt32			inKeyModifiers);
	//! Perform mouse tracking.
	virtual HIViewPartCode
					Track(
						const Point&	inMouseLoc, 
						UInt32			inClickCount,
						UInt32&			ioKeyModifiers);
	//! Determines whether the view's containing window should be activated when clicked in the background.
	virtual ClickActivationResult
					GetClickActivation(
						const Point&	inMouseLoc, 
						UInt32			inKeyModifiers, 
						EventMouseButton inMouseButton, 
						UInt32			inClickCount, 
						UInt32			inMouseChord);
	//! High-level function that handles focus-change requests.
	virtual HIViewPartCode
					SetFocusPart(
						HIViewPartCode	inPart, 
						bool			inFocusEverything);
	//! Changes a piece of view data.
	virtual bool	SetData(
						HIViewPartCode	inPart, 
						ResType			inDataTag, 
						const void*		inDataPtr, 
						size_t			inDataSize);
	//! Retrieves a piece of view data.
	virtual bool	GetData(
						HIViewPartCode	inPart, 
						ResType			inDataTag, 
						void*			inDataPtr, 
						size_t&			ioDataSize) const;
	//! Retrieves the view's optimal bounds.
	virtual Rect	GetOptimalBounds() const;
	//! Retrieves the view's minimal and maximal sizes.
	virtual std::pair<Size, Size>
					GetMinMaxSize() const;
	//! Retrieves the view's text baseline.
	virtual float	GetBaseline() const;
	//! Notifies the view that its bounds have changed.
	virtual bool	BoundsChanged(
						UInt32			inAttributes, 
						const Rect&		inPreviousBounds, 
						const Rect&		inCurrentBounds);
	//! Notifies the view that its owning window has changed.
	virtual void	OwningWindowChanged(
						WindowRef		inOriginalWindow, 
						WindowRef		inCurrentWindow);
	//! Notifies the view that its highlight state has changed.
	virtual void	HighlightChanged(
						HIViewPartCode	inOldHilitePart, 
						HIViewPartCode	inNewHilitePart);
	//! Notifies the view that its focus state has changed.
	virtual void	FocusChanging(
						HIViewPartCode	inOldFocusPart, 
						HIViewPartCode	inNewFocusPart);
	//! Notifies the view that its active state has changed.
	virtual void	ActivateStateChanged(
						bool			inActive);
	//! Notifies the view that its visibility has changed.
	virtual void	VisibilityChanged();
	//! Notifies the view that one of its value/minimum/maximum fields has changed.
	virtual void	ValueFieldChanged();
	//! Notifies the view that its title has changed.
	virtual void	TitleChanged();
	//! Notifies the view that its enabled state has changed.
	virtual void	EnabledStateChanged();
	//! Notifies the view that a view has been added as one of its children.
	virtual void	AddedSubview(
						HIViewRef		inSubview);
	//! Notifies the view that one of its subviews is about to be removed.
	virtual void	RemovingSubview(
						HIViewRef		inSubview);
	//! Notifies the view that the user has dragged something into it.
	virtual bool	DragEnter(
						DragRef			inDragRef);
	//! Notifies the view that the user is moving within it during a drag.
	virtual void	DragWithin(
						DragRef			inDragRef);
	//! Notifies the view that the user is moving out of it during a drag.
	virtual void	DragLeave(
						DragRef			inDragRef);
	//! Notifies the view that the user has dropped something onto it.
	virtual bool	DragReceive(
						DragRef			inDragRef);
	//! Notifies the view that the user has entered some text.
	virtual bool	TextInput(
						const UniChar*	inText, 
						size_t			inLength, 
						EventRef		inKeyboardEvent);
	//! Retrieves the view's scroll information.
	virtual void	GetScrollInfo(
						Size&			outImageSize,
						Size&			outViewSize,
						Size&			outLineSize,
						Point&			outOrigin) const;
	//! Tells the view to move its origin.
	virtual void	ScrollTo(
						const Point&	inOrigin);
	//! Handles an @c HICommand.
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);
	//! Returns the status of an @c HICommand.
	virtual bool	HandleUpdateStatus(
						const HICommandExtended&	inHICommand, 
						CommandData&				ioCmdData);
	//! The mouse has just entered the tracking region identified by @a inTrackingRef.
	virtual void	TrackingAreaEntered(
                        HIViewTrackingAreaRef		inTrackingRef,
						const Point&				inMouseLocation, 
						UInt32						inKeyModifiers);
	//! The mouse has just left the tracking region identified by @a inTrackingRef.
	virtual void	TrackingAreaExited(
                        HIViewTrackingAreaRef		inTrackingRef,
						const Point&				inMouseLocation, 
						UInt32						inKeyModifiers);
	//@}
	
	//! @name Utility
	//@{
	//! Handles translating a "space" key press into a click.
	bool			HandleChooseKey(
						HIViewPartCode	inPart, 
						const UniChar*	inText, 
						size_t			inLength, 
						EventRef		inKeyboardEvent);
	//! Returns the next focusable part after @a inStartPart, if any.
	HIViewPartCode	FindNextPart(
						HIViewPartCode	inStartPart, 
						bool			inFocusEverything) const;
	//! Returns the focusable part before @a inStartPart, if any.
	HIViewPartCode	FindPreviousPart(
						HIViewPartCode	inStartPart, 
						bool			inFocusEverything) const;
	//! Returns @c true if @a inPart is a known part.
	bool			FindPart(
						HIViewPartCode	inPart) const;
	//! Notifies the view's superview that its scroll information has changed.
	void			ScrollInfoChanged() const;
    //! Returns the given tracking area's ID.
    static HIViewTrackingAreaID
                    GetTrackingAreaID(
                        HIViewTrackingAreaRef   inTrackingRef);
    //! Changes the given tracking area's shape.
    static void     SetTrackingAreaShape(
                        HIViewTrackingAreaRef   inTrackingRef, 
                        const Shape&            inShape);
	//@}
	
	// member variables
	const EViewFlags	mViewFlags;	//!< The view's flags.
	bool				mAwakened;
	
private:
	
	//! @name Private Overridables
	//@{
	//! The view has just been loaded.
	virtual void	Awaken(Nib* inFromNib);
	//@}
	
	void	InitEventHandler();
	void	UpdateCachedValuesForNewOwner(WindowRef inOwner);
	
	static OSPtr<HIViewRef>
			PrivateCreate(
				HIObjectClassRef	inClassObject,
				const HIViewID&		inViewID,
				HIViewRef			inSuperview, 
				const Rect*			inFrame,
                Nib*				inFromNib,
				Collection			inCollection);
	
	// Carbon %Event handlers
	bool	ControlInitialize(
				Event<kEventClassControl, kEventControlInitialize>&					event);
	bool	ControlDispose(
				Event<kEventClassControl, kEventControlDispose>&					event);
	bool	ControlDraw(
				Event<kEventClassControl, kEventControlDraw>&						event);
	bool	ControlGetPartRegion(
				Event<kEventClassControl, kEventControlGetPartRegion>&				event);
	bool	ControlGetPartBounds(
				Event<kEventClassControl, kEventControlGetPartBounds>&				event);
	bool	ControlHitTest(
				Event<kEventClassControl, kEventControlHitTest>&					event);
	bool	ControlHit(
				Event<kEventClassControl, kEventControlHit>&						event);
//	bool	ControlSimulateHit(
//				Event<kEventClassControl, kEventControlSimulateHit>&				event);
//	bool	ControlApplyBackground(
//				Event<kEventClassControl, kEventControlApplyBackground>&			event);
//	bool	ControlApplyTextColor(
//				Event<kEventClassControl, kEventControlApplyTextColor>&				event);
//	bool	ControlSetCursor(
//				Event<kEventClassControl, kEventControlSetCursor>&					event);
//	bool	ControlContextualMenuClick(
//				Event<kEventClassControl, kEventControlContextualMenuClick>&		event);
	bool	ControlClick(
				Event<kEventClassControl, kEventControlClick>&						event);
//	bool	ControlGetAutoToggleValue(
//				Event<kEventClassControl, kEventControlGetAutoToggleValue>&			event);
	bool	ControlGetClickActivation(
				Event<kEventClassControl, kEventControlGetClickActivation>&			event);
	bool	ControlTrack(
				Event<kEventClassControl, kEventControlTrack>&						event);
//	bool	ControlGetScrollToHereStartPoint(
//				Event<kEventClassControl, kEventControlGetScrollToHereStartPoint>&	event);
//	bool	ControlGetIndicatorDragConstraint(
//				Event<kEventClassControl, kEventControlGetIndicatorDragConstraint>&	event);
//	bool	ControlIndicatorMoved(
//				Event<kEventClassControl, kEventControlIndicatorMoved>&				event);
//	bool	ControlGhostingFinished(
//				Event<kEventClassControl, kEventControlGhostingFinished>&			event);
//	bool	ControlGetActionProcPart(
//				Event<kEventClassControl, kEventControlGetActionProcPart>&			event);
	bool	ControlSetData(
				Event<kEventClassControl, kEventControlSetData>&					event);
	bool	ControlGetData(
				Event<kEventClassControl, kEventControlGetData>&					event);

	bool	ControlSetFocusPart(
				Event<kEventClassControl, kEventControlSetFocusPart>&				event);
	bool	ControlGetFocusPart(
				Event<kEventClassControl, kEventControlGetFocusPart>&				event);

	bool	ControlGetOptimalBounds(
				Event<kEventClassControl, kEventControlGetOptimalBounds>&			event);
	bool	ControlGetSizeConstraints(
				Event<kEventClassControl, kEventControlGetSizeConstraints>&			event);

	bool	ControlBoundsChanged(
				Event<kEventClassControl, kEventControlBoundsChanged>&				event);
#if B_BUILDING_CAN_USE_10_3_APIS
	bool	ControlVisibilityChanged(
				Event<kEventClassControl, kEventControlVisibilityChanged>&			event);
#endif
	bool	ControlOwningWindowChanged(
				Event<kEventClassControl, kEventControlOwningWindowChanged>&		event);
	bool	ControlActivate(
				Event<kEventClassControl, kEventControlActivate>&					event);
	bool	ControlDeactivate(
				Event<kEventClassControl, kEventControlDeactivate>&					event);
	bool	ControlValueFieldChanged(
				Event<kEventClassControl, kEventControlValueFieldChanged>&			event);
	bool	ControlTitleChanged(
				Event<kEventClassControl, kEventControlTitleChanged>&				event);
	bool	ControlHiliteChanged(
				Event<kEventClassControl, kEventControlHiliteChanged>&				event);
	bool	ControlEnabledStateChanged(
				Event<kEventClassControl, kEventControlEnabledStateChanged>&		event);

	bool	ControlAddedSubControl(
				Event<kEventClassControl, kEventControlAddedSubControl>&			event);
	bool	ControlRemovingSubControl(
				Event<kEventClassControl, kEventControlRemovingSubControl>&			event);
	bool	ControlDragEnter(
				Event<kEventClassControl, kEventControlDragEnter>&					event);
	bool	ControlDragWithin(
				Event<kEventClassControl, kEventControlDragWithin>&					event);
	bool	ControlDragLeave(
				Event<kEventClassControl, kEventControlDragLeave>&					event);
	bool	ControlDragReceive(
				Event<kEventClassControl, kEventControlDragReceive>&				event);
	bool	ControlTrackingAreaEntered(
				Event<kEventClassControl, kEventControlTrackingAreaEntered>&		event);
	bool	ControlTrackingAreaExited(
				Event<kEventClassControl, kEventControlTrackingAreaExited>&			event);

	bool	TextInputUnicodeForKeyEvent(
				Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent>&		event);
	bool	ScrollableGetInfo(
				Event<kEventClassScrollable, kEventScrollableGetInfo>&				event);
	bool	ScrollableScrollTo(
				Event<kEventClassScrollable, kEventScrollableScrollTo>&				event);
	bool	CommandProcess(
				Event<kEventClassCommand, kEventCommandProcess>&					event);
	bool	CommandUpdateStatus(
				Event<kEventClassCommand, kEventCommandUpdateStatus>&				event);
	
	// member variables
	const HIViewRef	mViewRef;
	EventHandler	mEventHandler;
	HIViewPartCode	mFocusPart;
	UInt32			mLastClickCount;
};

// ------------------------------------------------------------------------------------------
/*!
	@param	T	The C/C++ type of the view.  Must be CustomView or derived from 
				CustomView.
*/
template <class T>
OSPtr<HIViewRef>
CustomView::Create(
	const HIViewID&	inViewID,		//!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
	HIViewRef		inSuperview,	//!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
	const Rect*		inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
	Nib*			inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
	Collection		inCollection)   //!< Initialisation data for the new view;  may be @c NULL.
{
	return (PrivateCreate(GetObjectClass<T>(), inViewID, inSuperview, inFrame, 
                          inFromNib, inCollection));
}

// ------------------------------------------------------------------------------------------
/*!
	@param	T	The C/C++ type of the view.  Must be CustomView or derived from 
				CustomView.
*/
template <class T>
OSPtr<HIViewRef>
CustomView::Create(
	const HIViewID&	inViewID,		//!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
	HIViewRef		inSuperview,	//!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
	const Rect*		inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
	Nib*			inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
	EventRef		inEvent)        //!< Initialisation data for the new view;  may be @c NULL.
{
	return (Create(T::kHIObjectClassID, inViewID, inSuperview, inFrame, inFromNib, inEvent));
}

// ------------------------------------------------------------------------------------------
inline HIViewRef
CustomView::GetViewRef() const
{
	return (mViewRef);
}

// ------------------------------------------------------------------------------------------
inline int
CustomView::GetValue() const
{
	return (GetControl32BitValue(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline int
CustomView::GetMinimum() const
{
	return (GetControl32BitMinimum(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline int
CustomView::GetMaximum() const
{
	return (GetControl32BitMaximum(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsFocused() const
{
	return (mFocusPart != kHIViewFocusNoPart);
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsHighlighted() const
{
	return (IsControlHilited(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsActive() const
{
	return (IsControlActive(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsEnabled() const
{
	return (IsControlEnabled(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsVisible() const
{
	return (HIViewIsVisible(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline WindowRef
CustomView::GetOwner() const
{
	return (GetControlOwner(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline HIViewPartCode
CustomView::GetFocusPart() const
{
	return (mFocusPart);
}

// ------------------------------------------------------------------------------------------
inline HIViewPartCode
CustomView::GetHighlightPart() const
{
	return (GetControlHilite(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsContainer() const
{
	return ((mViewFlags & kViewContainer) != 0);
}

// ------------------------------------------------------------------------------------------
inline bool
CustomView::IsCompositing() const
{
	return (HIViewIsCompositingEnabled(mViewRef));
}

// ------------------------------------------------------------------------------------------
inline void
CustomView::SetValue(int inValue)
{
	SetControl32BitValue(mViewRef, inValue);
}

// ------------------------------------------------------------------------------------------
inline void
CustomView::SetMinimum(int inMininum)
{
	SetControl32BitMinimum(mViewRef, inMininum);
}

// ------------------------------------------------------------------------------------------
inline void
CustomView::SetMaximum(int inMaximum)
{
	SetControl32BitMaximum(mViewRef, inMaximum);
}

// ------------------------------------------------------------------------------------------
inline void
CustomView::SetHighlight(HIViewPartCode inPart)
{
	HiliteControl(mViewRef, inPart);
}


// ==========================================================================================
//	CustomView::EViewFlags Global Functions

/*!	@addtogroup	Views
	@{
*/

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise AND of two EViewFlags.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator & (CustomView::EViewFlags f1, CustomView::EViewFlags f2)
{
	return (static_cast<CustomView::EViewFlags>(static_cast<int>(f1) & static_cast<int>(f2)));
}

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise OR of two EViewFlags.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator | (CustomView::EViewFlags f1, CustomView::EViewFlags f2)
{
	return (static_cast<CustomView::EViewFlags>(static_cast<int>(f1) | static_cast<int>(f2)));
}

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise XOR of two EViewFlags.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator ^ (CustomView::EViewFlags f1, CustomView::EViewFlags f2)
{
	return (static_cast<CustomView::EViewFlags>(static_cast<int>(f1) ^ static_cast<int>(f2)));
}

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise NOT of an EViewFlags.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator ~ (CustomView::EViewFlags f1)
{
	return (static_cast<CustomView::EViewFlags>(~static_cast<int>(f1)));
}

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise AND-assignment of one EViewFlags value to another.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator &= (CustomView::EViewFlags& f1, CustomView::EViewFlags f2)
{
	return (f1 = f1 & f2);
}

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise OR-assignment of one EViewFlags value to another.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator |= (CustomView::EViewFlags& f1, CustomView::EViewFlags f2)
{
	return (f1 = f1 | f2);
}

// ------------------------------------------------------------------------------------------
/*!	Performs a bitwise XOR-assignment of one EViewFlags value to another.
	
	@relates	CustomView::EViewFlags
*/
inline CustomView::EViewFlags
operator ^= (CustomView::EViewFlags& f1, CustomView::EViewFlags f2)
{
	return (f1 = f1 ^ f2);
}

//@}


}	// namespace B


#endif	// BCustomView_H_
