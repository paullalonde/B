// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

// file header
#include "BCustomView.h"

// B headers
#include "BEvent.h"
#include "BEventParams.h"
#include "BNib.h"
#include "BRect.h"
#include "BViewData.h"
#include "BViewFactory.h"
#include "BViewProperty.h"
#include "BViewUtils.h"


namespace
{
    struct FindPartCode : public std::unary_function<bool, B::CustomView::ViewPart>
    {
                FindPartCode(HIViewPartCode inPartCode)
                    : mPartCode(inPartCode) {}
        
        bool    operator () (const B::CustomView::ViewPart& inPart) const
                    { return (inPart.first == mPartCode); }

    private:
        
        HIViewPartCode  mPartCode;
    };

    struct FindFocusablePartCode : public std::unary_function<bool, B::CustomView::ViewPart>
    {
                FindFocusablePartCode(bool inFindAll)
                    : mFindAll(inFindAll) {}
        
        bool    operator () (const B::CustomView::ViewPart& inPart) const
                    {
                        return ((inPart.second == B::CustomView::kFocusAlways) || 
                                ((inPart.second == B::CustomView::kFocusFull) && mFindAll));
                    }

    private:
        
        bool    mFindAll;
    };
    
}   // anonymous namespace


namespace B {

typedef ViewProperty<kEventClassB, 'CVew', CustomView*> CustomViewObjectProperty;

const CFStringRef   CustomView::kHIObjectBaseClassID    = kHIViewClassID;

// ------------------------------------------------------------------------------------------
CustomView*
CustomView::FromViewRef(HIViewRef inViewRef)
{
    CustomView* customView;
    
    if (!CustomViewObjectProperty::Get(inViewRef, customView, std::nothrow))
        customView = NULL;
    
    return (customView);
}

// ------------------------------------------------------------------------------------------
/*! Typically the constructor is called indirectly from @c HIObjectCreate.
    
    @a inSignature and @a inKind are combined to form the view's @a ControlKind, which can 
    later be returned from @c GetControlData().
    
    @a inViewParts is a vector of part definitions.  This vector is used in two ways.  First, 
    when hit-testing, 
    
    Each element of the vector contains 
    a part code and 
*/
CustomView::CustomView(
    HIObjectRef         inObjectRef,    //!< The view's @c HIObjectRef.
    EViewFlags          inViewFlags)    //!< The view's flags.
        : EventTarget(inObjectRef), 
          mViewFlags(inViewFlags), mAwakened(false), 
          mViewRef(reinterpret_cast<HIViewRef>(inObjectRef)), mEventHandler(inObjectRef), 
          mFocusPart(kHIViewNoPart), mLastClickCount(0)
{
    CustomView* customView  = this;
    
    CustomViewObjectProperty::Set(mViewRef, customView);
    
    InitEventHandler();
}

// ------------------------------------------------------------------------------------------
/*! Fills in the view's EventHandler.  The view's flags affect the set of Carbon %Events 
    the view will respond to.
*/
void
CustomView::InitEventHandler()
{
    mEventHandler.Add(this, &CustomView::ControlInitialize);
    mEventHandler.Add(this, &CustomView::ControlDispose);
    mEventHandler.Add(this, &CustomView::ControlGetPartRegion);
    mEventHandler.Add(this, &CustomView::ControlGetPartBounds);
    mEventHandler.Add(this, &CustomView::ControlHitTest);
    mEventHandler.Add(this, &CustomView::ControlSetData);
    mEventHandler.Add(this, &CustomView::ControlGetData);
    mEventHandler.Add(this, &CustomView::ControlGetOptimalBounds);
    mEventHandler.Add(this, &CustomView::ControlGetSizeConstraints);
    mEventHandler.Add(this, &CustomView::ControlBoundsChanged);
#if B_BUILDING_CAN_USE_10_3_APIS
    mEventHandler.Add(this, &CustomView::ControlVisibilityChanged);
#endif
    mEventHandler.Add(this, &CustomView::ControlOwningWindowChanged);
    mEventHandler.Add(this, &CustomView::ControlActivate);
    mEventHandler.Add(this, &CustomView::ControlDeactivate);
    mEventHandler.Add(this, &CustomView::ControlValueFieldChanged);
    mEventHandler.Add(this, &CustomView::ControlTitleChanged);
    mEventHandler.Add(this, &CustomView::ControlHiliteChanged);
    mEventHandler.Add(this, &CustomView::ControlEnabledStateChanged);
    
    if (!(mViewFlags & kViewNoDrawing))
    {
        mEventHandler.Add(this, &CustomView::ControlDraw);
    }
    
    if (mViewFlags & kViewFocus)
    {
        mEventHandler.Add(this, &CustomView::ControlSetFocusPart);
        mEventHandler.Add(this, &CustomView::ControlGetFocusPart);
        mEventHandler.Add(this, &CustomView::TextInputUnicodeForKeyEvent);
    }
    
    if (mViewFlags & kViewContainer)
    {
        mEventHandler.Add(this, &CustomView::ControlAddedSubControl);
        mEventHandler.Add(this, &CustomView::ControlRemovingSubControl);
    }
    
    if (mViewFlags & kViewDragAndDrop)
    {
        mEventHandler.Add(this, &CustomView::ControlDragEnter);
        mEventHandler.Add(this, &CustomView::ControlDragWithin);
        mEventHandler.Add(this, &CustomView::ControlDragLeave);
        mEventHandler.Add(this, &CustomView::ControlDragReceive);
    }
    
    if (mViewFlags & kViewScrollable)
    {
        mEventHandler.Add(this, &CustomView::ScrollableGetInfo);
        mEventHandler.Add(this, &CustomView::ScrollableScrollTo);
    }
    
    if (mViewFlags & kViewCommands)
    {
        mEventHandler.Add(this, &CustomView::CommandProcess);
        mEventHandler.Add(this, &CustomView::CommandUpdateStatus);
    }
    
    if (mViewFlags & kViewHit)
    {
        mEventHandler.Add(this, &CustomView::ControlHit);
    }
    
    if (mViewFlags & kViewTracking)
    {
        mEventHandler.Add(this, &CustomView::ControlTrack);
        mEventHandler.Add(this, &CustomView::ControlClick);
    }
    
    if (mViewFlags & kViewTrackingArea)
    {
        mEventHandler.Add(this, &CustomView::ControlTrackingAreaEntered);
        mEventHandler.Add(this, &CustomView::ControlTrackingAreaExited);
    }
    
    if (mViewFlags & kViewClickActivation)
    {
        mEventHandler.Add(this, &CustomView::ControlGetClickActivation);
    }
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
/*! Perform any initialisation of the view.  Derived classes may override this to set up 
    the view's initial state.  Such overrides should call the base class implementation 
    prior to performing work of their own.
    
    @a inCollection contains any initialisation parameters.  If the collection contains any 
    of the Apple-defined initialisation tags (@c kControlCollectionTagValue etc) they will 
    be used to initialise the view's state.  The collection may contain other tags as well, 
    defined and interpreted by the derived class.
    
    Note that Interface Builder currently always instantiates custom views via 
    @c CreateCustomControl(), and fills in the collection with both standard and custom 
    tags prior to instantiation.
    
    The default implementation sets some internal flags, changes the view's 
    @c HIViewFeatures to reflect the view flags (Panther only), and enables drag & drop on 
    the view if its @c kViewDragAndDrop view flag is set.
*/
void
CustomView::InitializeView(
    Collection      /* inCollection */) //!< A collection containing initialisation parameters.
{
    OSStatus    err;
    
    UpdateCachedValuesForNewOwner(GetControlOwner(mViewRef));
    
    if (mViewFlags & kViewDragAndDrop)
    {
        err = SetControlDragTrackingEnabled(mViewRef, true);
        B_THROW_IF_STATUS(err);
    }
}

// ------------------------------------------------------------------------------------------
/*! Perform any cleanup of the view.  Derived classes may override this to release 
    resources.  Such overrides should call the base class implementation *after* 
    performing work of their own.
    
    The default implementation does nothing.
*/
void
CustomView::DisposeView()
{
}

#if B_BUILDING_CAN_USE_10_3_APIS
// ------------------------------------------------------------------------------------------
/*! The default implementation converts the view's flags into flags understood by the 
    @c HIView system (@c kHIViewDoesNotDraw etc).
    
    Derived classes may override this function to add additional flags which aren't 
    currently handled by CustomView (eg, @c kHIViewIdlesWithTimer).
    
    This function is only called on 10.3 (aka Panther) and up.
    
    @return HIViewFeatures  HIView Feature Bits
*/
HIViewFeatures
CustomView::CalcViewFeatures() const
{
    HIViewFeatures  features    = 0;
    
    if (mViewFlags & kViewFocus)
    {
        if (mViewFlags & kViewFocusOnClick)
            features |= kHIViewFeatureGetsFocusOnClick;
    }
    
    if (mViewFlags & kViewContainer)
        features |= kHIViewFeatureAllowsSubviews;
    
    if (mViewFlags & kViewLiveFeedback)
        features |= kHIViewFeatureSupportsLiveFeedback;
    
    if (mViewFlags & kViewNoDrawing)
        features |= kHIViewFeatureDoesNotDraw;
    
    if (mViewFlags & kViewNoClicking)
        features |= kHIViewFeatureIgnoresClicks;
    
    if (mViewFlags & kViewOpaque)
        features |= kHIViewFeatureIsOpaque;
    
    return (features);
}
#endif  // B_BUILDING_CAN_USE_10_3_APIS

// ------------------------------------------------------------------------------------------
/*! The default implementation converts the view's flags into flags understood by the 
    Control Manager (@c kControlSupportsDataAccess etc).
    
    Derived classes may override this function to add additional flags which aren't 
    currently handled by CustomView (eg, @c kControlIdlesWithTimer).
    
    This function is used as a fallback when the @c HIViewFeatures functions can't be 
    called, either because of build-time constraints (ie the build can't take advantage 
    of Panther features) or run-time constraints (ie the application is running on a 
    pre-Panther system).
    
    @return UInt32  Control Feature Bits
*/
UInt32
CustomView::CalcControlFeatures() const
{
    UInt32  features    = kControlSupportsDataAccess 
                        | kControlWantsActivate 
                        | kControlSupportsCalcBestRect 
                        | kControlSupportsGetRegion;
    
    if (mViewFlags & kViewFocus)
    {
        features |= kControlSupportsFocus;
        
        if (mViewFlags & kViewFocusOnClick)
            features |= kControlGetsFocusOnClick;
    }
    
    if (mViewFlags & kViewContainer)
        features |= kControlSupportsEmbedding;
    
    if (mViewFlags & kViewLiveFeedback)
        features |= kControlSupportsLiveFeedback;
    
    if (mViewFlags & kViewDragAndDrop)
        features |= kControlSupportsDragAndDrop;
    
    if (mViewFlags & kViewTracking)
        features |= kControlHandlesTracking;
    
    if (mViewFlags & kViewClickActivation)
        features |= kControlSupportsClickActivation;
    
    return (features);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes need to override this function in order to return their vector of 
    view parts.
    
    The vector is used in two ways.  First, it is used for hit-testing.  The default 
    implementation of HitTest() iterates over the vector, generating the shape of each 
    part, and hit-testing the shape.  Thus it's fairly important that the ordering of the 
    parts in the vector take this into consideration.  Second, it's used when advancing 
    the focus.  The ordering of parts in the vector is the order in which they will 
    acquire the focus (or the reverse ordering, if the user is shift-tabbing).
    
    @return CustomView::PartVector& A reference to the view's part vector.
*/
const CustomView::ViewPartVector&
CustomView::GetViewParts() const
{
    // The default implementation doesn't contain any parts, and therefore can never 
    // succeed hit-testing nor acquire the focus.
    
    static ViewPartVector   sParts;
    
    return (sParts);
}

// ------------------------------------------------------------------------------------------
/*! A view's @c ControlKind identifies the type of view (eg push button, static text etc) 
    to the toolbox and to the rest of B.  It is @e very important that all custom views 
    have unique @c ControlKinds.  The @c ControlKind contains a @c signature field, so the 
    first step is to use a unique registered-with-Apple creator code for that.  You can 
    then attribute unique integer values to each of your custom views, and put them in 
    the @c kind field of the @c ControlKind structure.
    
    This is a pure virtual function, which all derived classes must override.
*/
ControlKind
CustomView::GetKind() const
{
    return (ViewUtils::MakeControlKind(0, 0));
}

// ------------------------------------------------------------------------------------------
/*! @note   This function is @b not recursive.
*/
void
CustomView::AwakenView(
    HIViewRef   inView,         //!< The view to awaken.
    Nib*        inFromNib)      //!< The nib from which to load other resources.  May be @c NULL.
{
    // If this is a custom view, awaken it.
    
    CustomView* custom  = CustomView::FromViewRef(inView);
    
    if ((custom != NULL) && !custom->mAwakened)
    {
        custom->mAwakened = true;
        custom->Awaken(inFromNib);
    }
}

// ------------------------------------------------------------------------------------------
/*! At the time this function is called, all subviews of the view have already been awaken, 
    so it's safe to access them.
    
    Derived classes may override this if they need to perform some processing once their 
    child views are accessible, or if they need access to the nib from which the view 
    was loaded.
    
    The default implementation does nothing.  There's no point in calling it from other 
    classes, so it's been made private.
    
    @note   @a inNib is not @c NULL if the view was loaded from a nib.  If the view was 
            created programatically, it will be @c NULL.  Also, @a inNib is only valid for 
            the duration of this function invocation.  Derived classes should @b not 
            save a reference to it.
*/
void
CustomView::Awaken(
    Nib*    /* inFromNib */)    //!< The nib from which to load other resources.  May be @c NULL.
{
}

// ------------------------------------------------------------------------------------------
void
CustomView::UpdateCachedValuesForNewOwner(WindowRef inOwner)
{
    if (inOwner != NULL)
    {
        // If we're using drag & drop, make sure it's enabled for the window.
        
        if (mViewFlags & kViewDragAndDrop)
        {
            OSStatus    err;
            
            err = SetAutomaticControlDragTrackingEnabledForWindow(inOwner, true);
            B_THROW_IF_STATUS(err);
        }
    }
}

// ------------------------------------------------------------------------------------------
HIViewID
CustomView::GetViewID() const
{
    HIViewID    viewID;
    OSStatus    err;
    
    err = HIViewGetID(mViewRef, &viewID);
    B_THROW_IF_STATUS(err);
    
    return (viewID);
}

// ------------------------------------------------------------------------------------------
Rect
CustomView::GetBounds() const
{
    Rect        r;
    OSStatus    err;
    
    err = HIViewGetBounds(mViewRef, &r);
    B_THROW_IF_STATUS(err);
    
    return r;
}

// ------------------------------------------------------------------------------------------
Rect
CustomView::GetFrame() const
{
    Rect        r;
    OSStatus    err;
    
    err = HIViewGetFrame(mViewRef, &r);
    B_THROW_IF_STATUS(err);
    
    return r;
}

// ------------------------------------------------------------------------------------------
String
CustomView::GetTitle() const
{
    CFStringRef cfstr;
    OSStatus    err;
    
    err = CopyControlTitleAsCFString(mViewRef, &cfstr);
    B_THROW_IF_STATUS(err);
    
    return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
bool
CustomView::IsCommandSentToUserFocus() const
{
    OptionBits  attributes;
    OSStatus    err;
    
    err = HIViewGetAttributes(mViewRef, &attributes);
    B_THROW_IF_STATUS(err);
    
    return ((attributes & kHIViewSendCommandToUserFocus ) != 0);
}

// ------------------------------------------------------------------------------------------
void
CustomView::DebugPrint() const
{
    HIObjectPrintDebugInfo(reinterpret_cast<HIObjectRef>(mViewRef));
}

// ------------------------------------------------------------------------------------------
void
CustomView::SetFrame(const Rect& inNewFrame)
{
    OSStatus    err;
    
    err = HIViewSetFrame(mViewRef, &inNewFrame);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::MoveFrameBy(float deltaX, float deltaY)
{
    OSStatus    err;
    
    err = HIViewMoveBy(mViewRef, deltaX, deltaY);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::MoveFrameTo(float x, float y)
{
    OSStatus    err;
    
    err = HIViewPlaceInSuperviewAt(mViewRef, x, y);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::ResizeFrameBy(float deltaWidth, float deltaHeight)
{
    B::Rect frame   = GetFrame();
    
    frame.size.width    += deltaWidth;
    frame.size.height   += deltaHeight;
    
    SetFrame(frame);
}

// ------------------------------------------------------------------------------------------
void
CustomView::ResizeFrameTo(float width, float height)
{
    B::Rect frame   = GetFrame();
    
    frame.size.width    = width;
    frame.size.height   = height;
    
    SetFrame(frame);
}

// ------------------------------------------------------------------------------------------
void
CustomView::SetOrigin(const HIPoint& inOrigin)
{
    OSStatus    err;
    
    err = HIViewSetBoundsOrigin(mViewRef, inOrigin.x, inOrigin.y);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::SetTitle(const String& inTitle)
{
    OSStatus    err;
    
    err = SetControlTitleWithCFString(mViewRef, inTitle.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Activate(bool inActivate)
{
    OSStatus    err;
    
    if (inActivate)
        err = ActivateControl(mViewRef);
    else
        err = DeactivateControl(mViewRef);
        
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Enable(bool inEnable)
{
    OSStatus    err;
    
    if (inEnable)
        err = EnableControl(mViewRef);
    else
        err = DisableControl(mViewRef);
        
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Show(bool inShow)
{
    OSStatus    err;
    
    err = HIViewSetVisible(mViewRef, inShow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::SendCommandToUserFocus(bool inSendIt)
{
    OptionBits  setAttributes, clearAttributes;
    OSStatus    err;
    
    if (inSendIt)
    {
        setAttributes   = kHIViewSendCommandToUserFocus;
        clearAttributes = 0;
    }
    else
    {
        setAttributes   = 0;
        clearAttributes = kHIViewSendCommandToUserFocus;
    }
    
    err = HIViewChangeAttributes(mViewRef, setAttributes, clearAttributes);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Reshape()
{
    OSStatus    err;
    
    err = HIViewReshapeStructure(mViewRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Invalidate()
{
    OSStatus    err;
    
    err = HIViewSetNeedsDisplay(mViewRef, true);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Invalidate(const Rect& inRect)
{
    OSStatus    err = noErr;
    
    err = HIViewSetNeedsDisplayInRect(mViewRef, &inRect, true);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Invalidate(const Shape& inShape)
{
    OSStatus    err = noErr;
    
    err = HIViewSetNeedsDisplayInShape(mViewRef, inShape.cf_ref(), true);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::Invalidate(const MutableShape& inShape)
{
    OSStatus    err = noErr;
    
    err = HIViewSetNeedsDisplayInShape(mViewRef, inShape.cf_ref(), true);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::InvalidatePart(HIViewPartCode inPart)
{
    OSStatus    err;
    
    err = HIViewRegionChanged(mViewRef, inPart);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::AddSubview(HIViewRef subview)
{
    B_ASSERT(subview != NULL);
    
    OSStatus    err;
    
    err = HIViewAddSubview(mViewRef, subview);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
CustomView::RemoveSubview(HIViewRef subview)
{
    OSStatus    err;
    
    err = HIViewRemoveFromSuperview(subview);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes need to override this function in order to draw their content.  Note 
    that if the @c kViewNoDrawing view flag is turned on, then this function won't be 
    called at all.
    
    The default implementation does nothing.
*/
void
CustomView::Draw(
    HIViewPartCode  /* inPart */,       //!< The view part to draw, or @c kHIViewNoPart to draw everything.
    CGContextRef    /* inContext */,    //!< The CoreGraphics context in which to draw, or @c NULL.
    const Shape*    /* inDrawShape */)  //!< A shape for constraining drawing.  May be @c NULL.
{
}

// ------------------------------------------------------------------------------------------
/*! The structure shape needs to encompass the view in its entirety, including any 
    translucent areas.  Derived classes will normally need to override this function.
    
    The default implementation returns a shape that matches the view bounds.
*/
Shape
CustomView::GetStructureShape() const
{
    Rect    bounds  = GetBounds();
    
    return Shape(bounds);
}

// ------------------------------------------------------------------------------------------
/*! The content shape is the area of the view in which subviews may be placed.  Container 
    views may want to override this function.
    
    This function is only called if the @c kViewContainer view flag is set.
    
    The default implementation returns a shape that matches the structure shape.
*/
Shape
CustomView::GetContentShape() const
{
    return GetStructureShape();
}

// ------------------------------------------------------------------------------------------
/*! The opaque shape is the area of the view where no part of any view underneath 
    shows through.  Derived classes with completely opaque areas may want to override this 
    function.
    
    This function is only called if the @c kViewOpaque view flag is @b not set.
    
    The default implementation returns a shape that matches the structure shape.
*/
Shape
CustomView::GetOpaqueShape() const
{
    return GetStructureShape();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes won't normally need to override this function.  It's here for 
    completeness.
    
    The default implementation returns a shape that matches the structure shape.
*/
Shape
CustomView::GetClickableShape() const
{
    return GetStructureShape();
}

// ------------------------------------------------------------------------------------------
/*! This function is only called for parts that are contained in the view's part vector.  
    Any non-trivial view will need to override this function.
    
    The default implementation returns a shape that matches the structure shape.
*/
Shape
CustomView::GetPartShape(
    HIViewPartCode  /* inPart */)       //!< The part for which to return the shape.
    const
{
    return GetStructureShape();
}

// ------------------------------------------------------------------------------------------
/*! This is a template method that dispatches to one of the other shape-retrieval functions, 
    depending on the value of @a inPart.
    
    Derived classes will rarely need to override this function.
    
    @return bool    Indicates whether the request was handled succesfully.
*/
bool
CustomView::GetViewShape(
    HIViewPartCode  inPart,             //!< The part for which to return the shape.
    Shape&          outShape)           //!< The output shape.
    const
{
    bool    handled = true;
    
    switch (inPart)
    {
    case kHIViewStructureMetaPart:
        outShape = GetStructureShape();
        break;
        
    case kHIViewContentMetaPart:
        if (mViewFlags & kViewContainer)
            outShape = GetContentShape();
        else
            handled = false;
        break;
        
#if B_BUILDING_CAN_USE_10_3_APIS
    case kHIViewOpaqueMetaPart:
        if (!(mViewFlags & kViewOpaque))
            outShape = GetOpaqueShape();
        else
            handled = false;
        break;
        
    case kHIViewClickableMetaPart:
        outShape = GetClickableShape();
        break;
#endif  // B_BUILDING_CAN_USE_10_3_APIS
        
    case kHIViewDisabledPart:
    case kHIViewInactivePart:
        handled = false;
        break;
        
    default:
        // Before trying to retrieve a part's shape, check that the view actually knows 
        // about this part.
        if (FindPart(inPart))
            outShape = GetPartShape(inPart);
        else
            handled = false;
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! The structure bounds needs to encompass the view in its entirety, including any 
    translucent areas.
    
    The default implementation simply retrieves the structure shape, then extracts the 
    shape's bounds.  Derived classes may override this behaviour if they have a simpler 
    way of obtaining the same information.
*/
Rect
CustomView::GetStructureBounds() const
{
    return GetStructureShape().bounds();
}

// ------------------------------------------------------------------------------------------
/*! The content bounds is the area of the view in which subviews may be placed.  Container 
    views may want to override this function.
    
    This function is only called if the @c kViewContainer view flag is set.
    
    The default implementation simply retrieves the content shape, then extracts the 
    shape's bounds.  Derived classes may override this behaviour if they have a simpler 
    way of obtaining the same information.
*/
Rect
CustomView::GetContentBounds() const
{
    return GetContentShape().bounds();
}

// ------------------------------------------------------------------------------------------
/*! The opaque bounds is the area of the view where no part of any view underneath 
    shows through.  Derived classes with completely opaque areas may want to override this 
    function.
    
    This function is only called if the @c kViewOpaque view flag is @b not set.
    
    The default implementation simply retrieves the opaque shape, then extracts the 
    shape's bounds.  Derived classes may override this behaviour if they have a simpler 
    way of obtaining the same information.
*/
Rect
CustomView::GetOpaqueBounds() const
{
    return GetOpaqueShape().bounds();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes won't normally need to override this function.  It's here for 
    completeness.
    
    The default implementation simply retrieves the clickable shape, then extracts the 
    shape's bounds.  Derived classes may override this behaviour if they have a simpler 
    way of obtaining the same information.
*/
Rect
CustomView::GetClickableBounds() const
{
    return GetClickableShape().bounds();
}

// ------------------------------------------------------------------------------------------
/*! This function is only called for parts that are contained in the view's part vector.
    
    The default implementation simply retrieves the part's shape, then extracts the 
    shape's bounds.  Derived classes may override this behaviour if they have 
    a simpler way of obtaining the same information.
*/
Rect
CustomView::GetPartBounds(
    HIViewPartCode  inPart)             //!< The part for which to return the bounds.
    const
{
    return GetPartShape(inPart).bounds();
}

// ------------------------------------------------------------------------------------------
/*! This is a template method that dispatches to one of the other bounds-retrieval functions, 
    depending on the value of @a inPart.
    
    Derived classes will rarely need to override this function.
    
    @return bool    Indicates whether the request was handled succesfully.
*/
bool
CustomView::GetViewBounds(
    HIViewPartCode  inPart,             //!< The part for which to return the bounds.
    Rect&           outBounds)          //!< The output bounds.
    const
{
    bool    handled = true;
    
    switch (inPart)
    {
    case kHIViewStructureMetaPart:
        outBounds = GetStructureBounds();
        break;
        
    case kHIViewContentMetaPart:
        if (mViewFlags & kViewContainer)
            outBounds = GetContentBounds();
        else
            handled = false;
        break;
        
#if B_BUILDING_CAN_USE_10_3_APIS
    case kHIViewOpaqueMetaPart:
        if (!(mViewFlags & kViewOpaque))
            outBounds = GetOpaqueBounds();
        else
            handled = false;
        break;
        
    case kHIViewClickableMetaPart:
        outBounds = GetClickableBounds();
        break;
#endif  // B_BUILDING_CAN_USE_10_3_APIS
        
    case kHIViewDisabledPart:
    case kHIViewInactivePart:
        handled = false;
        break;
        
    default:
        // Before trying to retrieve a part's bounds, check that the view actually knows 
        // about this part.
        if (FindPart(inPart))
            outBounds = GetPartBounds(inPart);
        else
            handled = false;
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation iterates over all of the view's parts, retrieves the shape 
    of each one and hit-test against the shape.
    
    Derived classes may override this function if necessary.
    
    @return HIViewPartCode  The view part containing @a inMouseLoc, or @c kHIViewNoPart.
*/
HIViewPartCode
CustomView::HitTest(
    const Point&    inMouseLoc)     //!< The mouse location to test for.
    const
{
    HIViewPartCode          hitPart     = kHIViewNoPart;
    const ViewPartVector&   viewParts   = GetViewParts();
    
    for (ViewPartVector::const_iterator it = viewParts.begin(); 
         it != viewParts.end(); 
         ++it)
    {
        if (GetPartShape(it->first).contains(inMouseLoc))
        {
            hitPart = it->first;
            break;
        }
    }
    
    return (hitPart);
}

// ------------------------------------------------------------------------------------------
/*! This function is called after a click has been succesfully tracked.
    
    This function is only called if the @c kViewHit view flag is turned on.  Note that it's 
    only necessary to turn the flag on if the HIToolbox's default behaviour (i.e. sending 
    the view's @c CommandID if it has one to either itself or the user input focus) is 
    inappropriate.
    
    The default implementation does nothing.
*/
void
CustomView::Hit(
    HIViewPartCode  /* inPart */,           //!< The view part that was hit.
    UInt32          /* inKeyModifiers */)   //!< The keyboard modifiers that were pressed at the time the mouse was released.
{
}

// ------------------------------------------------------------------------------------------
/*! This function is called to perform mouse tracking.  This will usually involve:
    
    - Determining which view part was clicked.
    - Entering a tracking loop for the view part, highlighting and unhighlighting the 
      part as appropriate.
    - Returning the part if the mouse button is raised in the original part, or 
      @c kHIViewNoPart otherwise.
    
    This function is only called if the @c kViewTracking view flag is turned on.  Note that 
    for simple views (i.e., views whose view part shapes don't change during tracking, or 
    that don't need to track at a finer granularity than a view part), the HIToolbox's 
    default behaviour is probably sufficient.
    
    @return HIViewPartCode  The view part where the tracking occurred, or @c kHIViewNoPart.
*/
HIViewPartCode
CustomView::Track(
    const Point&        /* inMouseLoc */, 
    UInt32              /* inClickCount */,
    UInt32&             /* ioKeyModifiers */)
{
    return (kHIViewNoPart);
}

// ------------------------------------------------------------------------------------------
/*! This function allows the view to determine what happens when it is clicked while its 
    containing window is inactive.
    
    The default implementation behaves the same was as if there wasn't a handler for this 
    event.  Namely, the window is activated and the click is swallowed.
*/
ClickActivationResult
CustomView::GetClickActivation(
    const Point&    /* inMouseLoc */, 
    UInt32          /* inKeyModifiers */, 
    EventMouseButton /* inMouseButton */, 
    UInt32          /* inClickCount */, 
    UInt32          /* inMouseChord */)
{
    return (kActivateAndIgnoreClick);
}

// ------------------------------------------------------------------------------------------
/*! This is a template method that dispatches to one of the other focus-retrieval functions, 
    depending on the value of @a inPart.  Derived classes may override this function if necessary.
    
    This function is only called if the @c kViewFocus view flag is turned on.
    
    The default implementation dispatches to some utility routines, depending on the value 
    of @a inPart.
*/
HIViewPartCode
CustomView::SetFocusPart(
    HIViewPartCode  inPart,             //!< The part to focus.  May be one of the focusing meta-parts (i.e., @c kHIViewFocusNextPart or @c kHIViewFocusPrevPart).
    bool            inFocusEverything)  //!< If @c true, full keyboard access is turned on.
{
    HIViewPartCode  newFocusPart;
    
    switch (inPart)
    {
    case kHIViewFocusNextPart:
        newFocusPart = FindNextPart(mFocusPart, inFocusEverything);
        break;
        
    case kHIViewFocusPrevPart:
        newFocusPart = FindPreviousPart(mFocusPart, inFocusEverything);
        break;
        
    case kHIViewFocusNoPart:
        // Focus is being removed.
        newFocusPart = kHIViewNoPart;
        break;
        
    default:
        newFocusPart = (FindPart(inPart) ? inPart : kHIViewNoPart);
        break;
    }
    
    return (newFocusPart);
}

// ------------------------------------------------------------------------------------------
HIViewPartCode
CustomView::FindNextPart(
    HIViewPartCode  inStartPart,        //!< The currently focused part, if any.
    bool            inFocusEverything)  //!< If @c true, full keyboard access is turned on.
    const
{
    HIViewPartCode                  newFocusPart    = kHIViewNoPart;
    const ViewPartVector&           viewParts       = GetViewParts();
    ViewPartVector::const_iterator  it;
    
    // Find the view part after @a inStartPart.  If inStartPart is @c, kHIViewNoPart, 
    // find the first view part.
    
    if (inStartPart == kHIViewNoPart)
    {
        it = viewParts.begin();
    }
    else
    {
        it = std::find_if(viewParts.begin(), viewParts.end(), 
                          FindPartCode(inStartPart));
        if (it != viewParts.end())
            ++it;
    }
    
    // Find the next focusable view part.
    
    it = std::find_if(it, viewParts.end(), 
                      FindFocusablePartCode(inFocusEverything));
    
    if (it != viewParts.end())
        newFocusPart = it->first;
    
    return (newFocusPart);
}

// ------------------------------------------------------------------------------------------
HIViewPartCode
CustomView::FindPreviousPart(
    HIViewPartCode  inStartPart,        //!< The currently focused part, if any.
    bool            inFocusEverything)  //!< If @c true, full keyboard access is turned on.
    const
{
    HIViewPartCode                          newFocusPart    = kHIViewNoPart;
    const ViewPartVector&                   viewParts       = GetViewParts();
    ViewPartVector::const_reverse_iterator  rit;
    
    // Find the view part before @a inStartPart.  If inStartPart is @c, kHIViewNoPart, 
    // find the last view part.
    
    if (inStartPart == kHIViewNoPart)
    {
        rit = viewParts.rbegin();
    }
    else
    {
        rit = std::find_if(viewParts.rbegin(), viewParts.rend(), 
                          FindPartCode(inStartPart));
        if (rit != viewParts.rend())
            ++rit;
    }
    
    // Find the previous focusable view part.
    
    rit = std::find_if(rit, viewParts.rend(), 
                       FindFocusablePartCode(inFocusEverything));
    
    if (rit != viewParts.rend())
        newFocusPart = rit->first;
    
    return (newFocusPart);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::FindPart(
    HIViewPartCode  inPart)     //!< The view part to check.
    const
{
    const ViewPartVector&   viewParts   = GetViewParts();
    
    return (std::find_if(viewParts.begin(), viewParts.end(), 
                         FindPartCode(inPart)) != viewParts.end());
}

// ------------------------------------------------------------------------------------------
/*! The default implementation does nothing.  Derived classes may override this function, 
    but should call the base class implementation for data tags they don't recognise.
*/
bool
CustomView::SetData(
    HIViewPartCode  /* inPart */,       //!< The view part to change, or @c kHIViewEntireView.
    ResType         /* inDataTag */,    //!< The tag identifying the view data.
    const void*     /* inDataPtr */,    //!< A pointer to the view data.
    size_t          /* inDataSize */)   //!< The view data's size.
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation only handles @c kControlKindTag.  Derived classes may 
    override this function, but should call the base class implementation for data tag 
    they don't recognise.
*/
bool
CustomView::GetData(
    HIViewPartCode  /* inPart */,       //!< The view part to change, or @c kHIViewEntireView.
    ResType         inDataTag,          //!< The tag identifying the view data.
    void*           inDataPtr,          //!< A pointer to receive the view data.
    size_t&         ioDataSize)         //!< The view data's size.
    const
{
    bool    handled = true;
    
    switch (inDataTag)
    {
    case kControlKindTag:
        {
            ControlKind kind(GetKind());
            
            if ((kind.signature == 0) || (kind.kind == 0))
                B_THROW_STATUS(controlPropertyNotFoundErr);
            
            B_THROW_IF_STATUS(ViewDataTrait<ControlKind>::Set(inDataPtr, ioDataSize, kind));
        }
        break;
        
    default:
        handled = false;
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation returns the view's current bounds.
    
    Derived classes may override this function to return something more appropriate.
*/
Rect
CustomView::GetOptimalBounds() const
{
    return GetBounds();
}

// ------------------------------------------------------------------------------------------
/*! The default implementation returns the view's current size.
    
    Derived classes may override this function to return something more appropriate.
*/
std::pair<Size, Size>
CustomView::GetMinMaxSize() const
{
    Rect    bounds  = GetBounds();
    
    return std::pair<Size, Size>(bounds.size, bounds.size);
}

// ------------------------------------------------------------------------------------------
/*! The default implementation returns 0.0.
    
    Derived classes may override this function to return something more appropriate.
*/
float
CustomView::GetBaseline() const
{
    return (0.0f);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    bounds is changed.  If they do so, they should call the base class implementation prior 
    to performing their own processing.
    
    The default implementation currently does nothing.
    
    @return @c true if the object handled the bounds change completely;  else @c false.
*/
bool
CustomView::BoundsChanged(
    UInt32          /* inAttributes */, 
    const Rect&     /* inPreviousBounds */, 
    const Rect&     /* inCurrentBounds */)
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    owning window is changed.  If they do so, they should call the base class implementation 
    prior to performing their own processing.
    
    The default implementation refreshes some internal flags.
*/
void
CustomView::OwningWindowChanged(
    WindowRef       /* inOriginalWindow */, 
    WindowRef       inCurrentWindow)
{
    UpdateCachedValuesForNewOwner(inCurrentWindow);
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    highlight changes.  If they do so, they should call the base class implementation prior 
    to performing their own processing.
    
    The default implementation invalidates the view if the @c kViewInvalOnHilite view 
    flag is set.
*/
void
CustomView::HighlightChanged(
    HIViewPartCode  /* inOldHilitePart */, 
    HIViewPartCode  /* inNewHilitePart */)
{
    if (mViewFlags & kViewInvalOnHilite)
        Invalidate();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    focused part changes.  If they do so, they should call the base class implementation 
    prior to performing their own processing.
    
    The default implementation changes the focused part, then invalidates the view if the 
    @c kViewInvalOnFocus view flag is set.
*/
void
CustomView::FocusChanging(
    HIViewPartCode  /* inOldFocusPart */, 
    HIViewPartCode  inNewFocusPart)
{
    mFocusPart = inNewFocusPart;
    
    if (mViewFlags & kViewInvalOnFocus)
        Invalidate();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    active state changes.  If they do so, they should call the base class implementation 
    prior to performing their own processing.
    
    The default implementation invalidates the view if the @c kViewInvalOnActivate view 
    flag is set.
*/
void
CustomView::ActivateStateChanged(
    bool    /* inActive */)
{
    if (mViewFlags & kViewInvalOnActivate)
        Invalidate();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    visible state changes.  If they do so, they should call the base class implementation 
    prior to performing their own processing.
*/
void
CustomView::VisibilityChanged()
{
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever one of 
    a view's value fields changes.  If they do so, they should call the base class 
    implementation prior to performing their own processing.
    
    The default implementation invalidates the view if the @c kViewInvalOnValue view 
    flag is set.
*/
void
CustomView::ValueFieldChanged()
{
    if (mViewFlags & kViewInvalOnValue)
        Invalidate();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    title changes.  If they do so, they should call the base class implementation 
    prior to performing their own processing.
    
    The default implementation invalidates the view if the @c kViewInvalOnTitle view 
    flag is set.
*/
void
CustomView::TitleChanged()
{
    if (mViewFlags & kViewInvalOnTitle)
        Invalidate();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a view's 
    enabled state changes.  If they do so, they should call the base class implementation 
    prior to performing their own processing.
    
    The default implementation invalidates the view if the @c kViewInvalOnEnable view 
    flag is set.
*/
void
CustomView::EnabledStateChanged()
{
    if (mViewFlags & kViewInvalOnEnable)
        Invalidate();
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a subview 
    is added to it.  If they do so, they should call the base class implementation prior to 
    performing their own processing.
    
    The default implementation does nothing.
*/
void
CustomView::AddedSubview(HIViewRef /* inSubview */)
{
}

// ------------------------------------------------------------------------------------------
/*! Derived classes may override this function to perform some processing whenever a subview 
    is removed from it.  If they do so, they should call the base class implementation prior 
    to performing their own processing.
    
    The default implementation does nothing.
*/
void
CustomView::RemovingSubview(HIViewRef /* inSubview */)
{
}

// ------------------------------------------------------------------------------------------
/*! This function is called to determine if a drag is of interest to a view.  If it is, 
    the function should return @c true.  The other drag-related functions (DragWithin(), 
    DragLeave(), and possibly DragReceive()) will then be called.  If one the other hand 
    the function returns @c false, then none of those functions will be called.
    
    Note that in order for this function to be called in the first place, the 
    @c kViewDragAndDrop view flag must be set.
    
    Derived classes that support drag & drop need to override this function.  The default 
    implementation always returns @c false.
    
    @return bool    An indication of whether the view is interested in this drag.
*/
bool
CustomView::DragEnter(DragRef /* inDragRef */)  //!< The drag to query.
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if (1) the @c kViewDragAndDrop view flag is turned on, 
    and (2) a previous call to DragEnter() for the same @c DragRef returned @c true.
    
    Derived classes that support drag & drop may want to override this function.  The 
    default implementation does nothing.
*/
void
CustomView::DragWithin(DragRef /* inDragRef */) //!< The drag to query.
{
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if (1) the @c kViewDragAndDrop view flag is turned on, 
    and (2) a previous call to DragEnter() for the same @c DragRef returned @c true.
    
    Derived classes that support drag & drop may want to override this function.  The 
    default implementation does nothing.
*/
void
CustomView::DragLeave(DragRef /* inDragRef */)  //!< The drag to query.
{
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if (1) the @c kViewDragAndDrop view flag is turned on, 
    and (2) a previous call to DragEnter() for the same @c DragRef returned @c true.
    
    Derived classes that support drag & drop may want to override this function.  The 
    default implementation always returns @c false.
    
    @return bool    An indication of whether the drop was successful.
*/
bool
CustomView::DragReceive(DragRef /* inDragRef */)    //!< The drag to query.
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! This function is normally called only if the @c kViewFocus view flag is turned on, 
    and if the view is currently within the chain of Carbon %Event handlers leading up 
    from the user input focus.  Practically speaking, this means that either the view 
    or one of its subviews has the focus.
    
    Derived classes that support text input need to override this function.  It's very 
    important that overrides return @c false for text they do not handle.  This lets 
    the view's superview handle the text instead.
    
    The default implementation always returns @c false.
    
    @return bool    An indication of whether the text was handled.
*/
bool
CustomView::TextInput(
    const UniChar*  /* inText */,           //!< The entered characters.
    size_t          /* inLength */,         //!< The number of entered characters.
    EventRef        /* inKeyboardEvent */)  //!< The keyboard event that generated the characters.
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! This function checks that the text input is a single space character and if so 
    simulates a click on view part @a inPart of the view.
    
    Derived classes that support text input may call this to support clicking buttons via 
    the keyboard.
*/
bool
CustomView::HandleChooseKey(
    HIViewPartCode  inPart,             //!< The view part to "click".
    const UniChar*  inText,             //!< The entered characters.
    size_t          inLength,           //!< The number of entered characters.
    EventRef        inKeyboardEvent)    //!< The keyboard event that generated the characters.
{
    bool    handled = false;
    
    if ((inLength == 1) && (inText[0] == L' ') && (inPart != kHIViewNoPart))
    {
        UInt32          modifiers   = EventParam<kEventParamKeyModifiers>::Get(inKeyboardEvent);
        HIViewPartCode  clickedPart;
        OSStatus        err;
        
        err = HIViewSimulateClick(mViewRef, inPart, modifiers, &clickedPart);
        B_THROW_IF_STATUS(err);
        
        handled = true;
    }
    
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if the @c kViewScrollable view flag is turned on.  It 
    needs to return scrolling-related information.
    
    The default implementation returns reasonable values, but derived classes that support 
    scrolling will need to override it.
*/
void
CustomView::GetScrollInfo(
    Size&           outImageSize,   //!< The size of the view's model.
    Size&           outViewSize,    //!< The size of the view's visible area.  This is normally the size of its bounds.
    Size&           outLineSize,    //!< The size of scrolling increment, in both X and Y directions.
    Point&          outOrigin)      //!< The view's current origin.
    const
{
    Rect    bounds  = GetBounds();
    
    outImageSize    = bounds.size;
    outViewSize     = bounds.size;
    outLineSize     = Size(1, 1);
    outOrigin       = Point(0, 0);
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if the @c kViewScrollable view flag is turned on.
    
    The default implementation does nothing.  Derived classes that support scrolling will 
    need to override it.
*/
void
CustomView::ScrollTo(
    const Point&    /* inOrigin */) //!< The new origin for the view.
{
}

// ------------------------------------------------------------------------------------------
/*! Derived classes that support scrolling may call this when one of their image size, 
    view size, line size, or origin changes.
*/
void
CustomView::ScrollInfoChanged() const
{
    HIViewRef   superView   = HIViewGetSuperview(mViewRef);
    
    if (superView != NULL)
    {
        Event<kEventClassScrollable, kEventScrollableInfoChanged>   event;
        
        event.Send(superView, std::nothrow);
    }
}

// ------------------------------------------------------------------------------------------
HIViewTrackingAreaID
CustomView::GetTrackingAreaID(
    HIViewTrackingAreaRef   inTrackingRef)
{
    HIViewTrackingAreaID    areaID;
    OSStatus                err;
    
    err = HIViewGetTrackingAreaID(inTrackingRef, &areaID);
    B_THROW_IF_STATUS(err);
    
    return (areaID);
}

// ------------------------------------------------------------------------------------------
void
CustomView::SetTrackingAreaShape(
    HIViewTrackingAreaRef   inTrackingRef, 
    const Shape&            inShape)
{
    OSStatus    err;
    
    err = HIViewChangeTrackingArea(inTrackingRef, inShape.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if the @c kViewCommands view flag is turned on.
    
    The default implementation does nothing.  Derived classes that support commands will 
    need to override it.
*/
bool
CustomView::HandleCommand(
    const HICommandExtended&    /* inHICommand */)  //!< The command to execute.
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if the @c kViewCommands view flag is turned on.
    
    The default implementation does nothing.  Derived classes that support commands will 
    need to override it.
*/
bool
CustomView::HandleUpdateStatus(
    const HICommandExtended&    /* inHICommand */,  //!< The command about which to return status.
    CommandData&                /* ioCmdData */)    //!< The command's status.
{
    return (false);
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if the @c kViewTrackingArea view flag is turned on.
    
    The default implementation does nothing.  Derived classes that support mouse-tracking 
    regions will need to override it.
*/
void
CustomView::TrackingAreaEntered(
    HIViewTrackingAreaRef   /* inTrackingRef */, 
    const Point&            /* inMouseLocation */, 
    UInt32                  /* inKeyModifiers */)
{
}

// ------------------------------------------------------------------------------------------
/*! This function is only called if the @c kViewTrackingArea view flag is turned on.
    
    The default implementation does nothing.  Derived classes that support mouse-tracking 
    regions will need to override it.
*/
void
CustomView::TrackingAreaExited(
    HIViewTrackingAreaRef   /* inTrackingRef */, 
    const Point&            /* inMouseLocation */, 
    UInt32                  /* inKeyModifiers */)
{
}

// ------------------------------------------------------------------------------------------
OSPtr<HIViewRef>
CustomView::PrivateCreate(
    HIObjectClassRef    inClassObject,  //!< The new view's class.
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    Collection          inCollection)   //!< Initialisation data for the new view;  may be @c NULL.
{
    B_ASSERT(inClassObject != NULL);
    
    WindowRef               window      = (inSuperview != NULL) ? GetControlOwner(inSuperview) : NULL;
    ::Rect                  qdBounds    = { 0, 0, 100, 100 };
    B::OSPtr<Collection>    collPtr(inCollection);
    ControlDefSpec          def;
    HIViewRef               viewRef;
    OSPtr<HIViewRef>        viewPtr;
    OSStatus                err;
    
    if (inCollection == NULL)
        collPtr.reset(NewCollection(), from_copy);
    
    if (inFrame != NULL)
        qdBounds = *inFrame;
    
    def.defType     = kControlDefObjectClass;
    def.u.classRef  = inClassObject;
    
    err = CreateCustomControl(window, &qdBounds, &def, collPtr, &viewRef);
    B_THROW_IF_STATUS(err);
    
    viewPtr.reset(viewRef, from_copy);
    
    // Hide the view.
    
    if (HIViewIsVisible(viewRef))
    {
        err = HIViewSetVisible(viewRef, false);
        B_THROW_IF_STATUS(err);
    }
    
    // Set the view's ID.
    
    if ((inViewID.signature != 0) || (inViewID.id != 0))
    {
        err = HIViewSetID(viewRef, inViewID);
        B_THROW_IF_STATUS(err);
    }
    
    // Put the view inside inSuperview.
    
    if (inSuperview != NULL)
    {
        err = HIViewAddSubview(inSuperview, viewRef);
        B_THROW_IF_STATUS(err);
        
        // HIViewAddSubview doesn't retain the subview, so we need to perform a retain 
        // ourselves, because if we don't the superview will be left with a dangling 
        // reference when our OSPtr goes out of scope.
        
        CFRetain(viewRef);
    }
    
    // Move the view to the correct location.
    
    if (inFrame != NULL)
    {
        Rect    oldFrame;
        
        err = HIViewGetFrame(viewRef, &oldFrame);
        B_THROW_IF_STATUS(err);
        
        if (oldFrame != *inFrame)
        {
            err = HIViewSetFrame(viewRef, inFrame);
            B_THROW_IF_STATUS(err);
        }
    }
    
    AwakenView(viewRef, inFromNib);
    
    return (viewPtr);
}

// ------------------------------------------------------------------------------------------
OSPtr<HIViewRef>
CustomView::Create(
    CFStringRef     inClassID,      //!< The new view's class.
    const HIViewID& inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    EventRef        inEvent)        //!< Initialisation data for the new view;  may be @c NULL.
{
    B_ASSERT(inClassID != NULL);
    
    HIObjectRef                 objRef;
    HIViewRef                   viewRef;
    OSPtr<HIViewRef>            viewPtr;
    std::auto_ptr<EventBase>    initEvent;
    OSStatus                    err;
    
    if (inEvent == NULL)
    {
        // We need an initialisation event, but the caller didn't give us one.  
        // Create one ourselves.
        
        initEvent.reset(new EventBase(kEventClassHIObject, kEventHIObjectInitialize));
        inEvent = *initEvent;
    }
    
    // Set the frame, if requested.
    
    if (inFrame != NULL)
    {
        EventParam<kEventParamBounds>::Set(inEvent, *inFrame);
    }
    
    // Create the HIView.
    
    err = HIObjectCreate(inClassID, inEvent, &objRef);
    B_THROW_IF_STATUS(err);
    
    viewRef = reinterpret_cast<HIViewRef>(objRef);
    viewPtr.reset(viewRef, from_copy);
    
    // Hide the view.
    
    if (HIViewIsVisible(viewRef))
    {
        err = HIViewSetVisible(viewRef, false);
        B_THROW_IF_STATUS(err);
    }
    
    // Set the view's ID.
    
    if ((inViewID.signature != 0) || (inViewID.id != 0))
    {
        err = HIViewSetID(viewRef, inViewID);
        B_THROW_IF_STATUS(err);
    }
    
    // Put the view inside inSuperview.
    
    if (inSuperview != NULL)
    {
        err = HIViewAddSubview(inSuperview, viewRef);
        B_THROW_IF_STATUS(err);
        
        // HIViewAddSubview doesn't retain the subview, so we need to perform a retain 
        // ourselves, because if we don't the superview will be left with a dangling 
        // reference when our OSPtr goes out of scope.
        
        CFRetain(viewRef);
    }
    
    // Move the view to the correct location.
    
    if (inFrame != NULL)
    {
        Rect    oldFrame;
        
        err = HIViewGetFrame(viewRef, &oldFrame);
        B_THROW_IF_STATUS(err);
        
        if (oldFrame != *inFrame)
        {
            err = HIViewSetFrame(viewRef, inFrame);
            B_THROW_IF_STATUS(err);
        }
    }
    
    AwakenView(viewRef, inFromNib);
    
    return (viewPtr);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlInitialize(
    Event<kEventClassControl, kEventControlInitialize>& event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        InitializeView(event.mCollection);
        
#if B_BUILDING_CAN_USE_10_3_APIS_ONLY

        // HITheme only
        
        OSStatus    err;
        
        err = HIViewChangeFeatures(mViewRef, CalcViewFeatures(), 0);
        B_THROW_IF_STATUS(err);
        
        event.mFeatures = 0;

#elif B_BUILDING_CAN_USE_10_3_APIS

        // HITheme or control features
        
        OSStatus    err;
        
        if (HIViewChangeFeatures != NULL)
        {
            err = HIViewChangeFeatures(mViewRef, CalcViewFeatures(), 0);
            B_THROW_IF_STATUS(err);
        }
        else
        {
            event.mFeatures = CalcControlFeatures();
        }
    
#else
    
        // Control features only
        event.mFeatures = CalcControlFeatures();

#endif
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDispose(
    Event<kEventClassControl, kEventControlDispose>&    event)
{
    if (event.mViewRef == mViewRef)
    {
        DisposeView();
    }
    
    event.Forward(std::nothrow);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDraw(
    Event<kEventClassControl, kEventControlDraw>&   event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        // Tell the view to draw.
        
        Draw(event.mPartCode, event.mCGContext, event.mDrawShape);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetPartRegion(
    Event<kEventClassControl, kEventControlGetPartRegion>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = GetViewShape(event.mPartCode, event.mPartShape);
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetPartBounds(
    Event<kEventClassControl, kEventControlGetPartBounds>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = GetViewBounds(event.mPartCode, event.mPartBounds);
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlHitTest(
    Event<kEventClassControl, kEventControlHitTest>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        event.mPartCode = HitTest(event.mMouseLocation);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlHit(
    Event<kEventClassControl, kEventControlHit>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        Hit(event.mPartCode, event.mKeyModifiers);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlClick(
    Event<kEventClassControl, kEventControlClick>&  event)
{
    if (event.mViewRef == mViewRef)
    {
        mLastClickCount = event.mClickCount;
    }
    
    return false;
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetClickActivation(
    Event<kEventClassControl, kEventControlGetClickActivation>& event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        event.mResult = GetClickActivation(event.mMouseLocation, event.mKeyModifiers, 
                                           event.mMouseButton, event.mClickCount, 
                                           event.mMouseChord);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlTrack(
    Event<kEventClassControl, kEventControlTrack>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        event.mHitPart = Track(event.mMouseLocation, mLastClickCount, event.mKeyModifiers);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlSetData(
    Event<kEventClassControl, kEventControlSetData>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = SetData(event.mPartCode, event.mDataTag, event.mDataPtr, event.mDataSize);
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetData(
    Event<kEventClassControl, kEventControlGetData>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = GetData(event.mPartCode, event.mDataTag, event.mDataPtr, event.mDataSize);
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlSetFocusPart(
    Event<kEventClassControl, kEventControlSetFocusPart>&   event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        HIViewPartCode  oldFocusPart    = GetFocusPart();
        
        event.mPartCode = SetFocusPart(event.mPartCode, event.mFocusEverything);
        
        if (oldFocusPart != event.mPartCode)
            FocusChanging(oldFocusPart, event.mPartCode);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetFocusPart(
    Event<kEventClassControl, kEventControlGetFocusPart>&   event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        event.mPartCode = GetFocusPart();
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetOptimalBounds(
    Event<kEventClassControl, kEventControlGetOptimalBounds>&   event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        event.mOptimalBounds    = GetOptimalBounds();
        event.mBaselineOffset   = static_cast<short>(GetBaseline());
        
        handled = (!event.mOptimalBounds.null() || (event.mBaselineOffset != 0));
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlGetSizeConstraints(
    Event<kEventClassControl, kEventControlGetSizeConstraints>& event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        boost::tie(event.mMinimumSize, event.mMaximumSize) = GetMinMaxSize();
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlBoundsChanged(
    Event<kEventClassControl, kEventControlBoundsChanged>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = BoundsChanged(event.mAttributes, event.mPreviousBounds, 
                                event.mCurrentBounds);
    }
    
    return (handled);
}

#if B_BUILDING_CAN_USE_10_3_APIS
// ------------------------------------------------------------------------------------------
bool
CustomView::ControlVisibilityChanged(
    Event<kEventClassControl, kEventControlVisibilityChanged>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        VisibilityChanged();
        
        handled = true;
    }
    
    return (handled);
}
#endif  // B_BUILDING_CAN_USE_10_3_APIS

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlOwningWindowChanged(
    Event<kEventClassControl, kEventControlOwningWindowChanged>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        OwningWindowChanged(event.mOriginalOwningWindow, event.mCurrentOwningWindow);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlActivate(
    Event<kEventClassControl, kEventControlActivate>&   event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        ActivateStateChanged(true);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDeactivate(
    Event<kEventClassControl, kEventControlDeactivate>& event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        ActivateStateChanged(false);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlValueFieldChanged(
    Event<kEventClassControl, kEventControlValueFieldChanged>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        ValueFieldChanged();
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlTitleChanged(
    Event<kEventClassControl, kEventControlTitleChanged>&   event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        TitleChanged();
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlHiliteChanged(
    Event<kEventClassControl, kEventControlHiliteChanged>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        HighlightChanged(event.mPreviousPartCode, event.mCurrentPartCode);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlEnabledStateChanged(
    Event<kEventClassControl, kEventControlEnabledStateChanged>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        EnabledStateChanged();
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDragEnter(
    Event<kEventClassControl, kEventControlDragEnter>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        event.mLikesDrag = DragEnter(event.mDragRef);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDragWithin(
    Event<kEventClassControl, kEventControlDragWithin>& event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        DragWithin(event.mDragRef);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDragLeave(
    Event<kEventClassControl, kEventControlDragLeave>&  event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        DragLeave(event.mDragRef);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlDragReceive(
    Event<kEventClassControl, kEventControlDragReceive>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        DragReceive(event.mDragRef);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlAddedSubControl(
    Event<kEventClassControl, kEventControlAddedSubControl>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        AddedSubview(event.mSubViewRef);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlRemovingSubControl(
    Event<kEventClassControl, kEventControlRemovingSubControl>& event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        RemovingSubview(event.mSubViewRef);
        
        handled = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlTrackingAreaEntered(
    Event<kEventClassControl, kEventControlTrackingAreaEntered>&    event)
{
    TrackingAreaEntered(event.mTrackingAreaRef, event.mMouseLocation, event.mKeyModifiers);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ControlTrackingAreaExited(
    Event<kEventClassControl, kEventControlTrackingAreaExited>& event)
{
    TrackingAreaExited(event.mTrackingAreaRef, event.mMouseLocation, event.mKeyModifiers);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::TextInputUnicodeForKeyEvent(
    Event<kEventClassTextInput, kEventTextInputUnicodeForKeyEvent>& event)
{
    return (TextInput(event.mText, event.mLength, event.mKeyboardEvent));
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ScrollableGetInfo(
    Event<kEventClassScrollable, kEventScrollableGetInfo>&  event)
{
    GetScrollInfo(event.mImageSize, event.mViewSize, event.mLineSize, event.mOrigin);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::ScrollableScrollTo(
    Event<kEventClassScrollable, kEventScrollableScrollTo>& event)
{
    ScrollTo(event.mOrigin);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
CustomView::CommandProcess(
    Event<kEventClassCommand, kEventCommandProcess>&    event)
{
    return (HandleCommand(event.mHICommand));
}

// ------------------------------------------------------------------------------------------
bool
CustomView::CommandUpdateStatus(
    Event<kEventClassCommand, kEventCommandUpdateStatus>&   event)
{
    return (HandleUpdateStatus(event.mHICommand, event.mData));
}

}   // namespace B
