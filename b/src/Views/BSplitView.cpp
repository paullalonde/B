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
#include "BSplitView.h"

// B headers
#include "BCollectionItem.h"
#include "BEvent.h"
#include "BEventParams.h"
#include "BMutableShape.h"
#include "BRect.h"
#include "BViewData.h"
#include "BViewUtils.h"
#include "BWindowUtils.h"


#define VIEW_FLAGS  (kViewTracking | kViewTrackingArea | kViewContainer | kViewInvalOnValue)


namespace {

    typedef B::CollectionItem<
        B::SplitView::kControlKindSig, 
        B::SplitView::kSizeTag, 
        ControlSize>                        SizeCollectionItem;

    typedef B::CollectionItem<
        B::SplitView::kControlKindSig, 
        B::SplitView::kOrientationTag, 
        Boolean>                            OrientationCollectionItem;

    typedef B::EventParamAndType<kControlCollectionTagValue,    typeSInt32>     ValueParam;
    typedef B::EventParamAndType<kControlCollectionTagMinimum,  typeSInt32>     MinimumParam;
    typedef B::EventParamAndType<kControlCollectionTagMaximum,  typeSInt32>     MaximumParam;
    typedef B::EventParamAndType<B::SplitView::kSizeTag,        typeSInt32>     SizeParam;
    typedef B::EventParamAndType<B::SplitView::kOrientationTag, typeBoolean>    OrientationParam;
    typedef B::EventParamAndType<B::SplitView::kSubviewAIDTag,  typeSInt32>     SubviewAIDParam;
    typedef B::EventParamAndType<B::SplitView::kSubviewBIDTag,  typeSInt32>     SubviewBIDParam;
    
    typedef B::StandardViewCollectionItem<
        kControlCollectionTagValue>         ValueCollectionItem;

    typedef B::StandardViewCollectionItem<
        kControlCollectionTagMinimum>       MinimumCollectionItem;

    typedef B::StandardViewCollectionItem<
        kControlCollectionTagMaximum>       MaximumCollectionItem;

}   // anonymous namespace


namespace B {

#if !B_BUILDING_ON_10_3_OR_LATER
// Missing declarations from pre-Panther headers.
enum {
    kThemeMetricSmallPaneSplitterHeight = 127   // 10.3 and up
};
#endif

const float         SplitView::kNormalSplitterSize  = SplitView::GetMetric(kThemeMetricPaneSplitterHeight, 9);
const float         SplitView::kSmallSplitterSize   = SplitView::GetMetric(kThemeMetricSmallPaneSplitterHeight, 7);
const CFStringRef   SplitView::kHIObjectClassID     = CFSTR("ca.paullalonde.b.view.splitview");

// ------------------------------------------------------------------------------------------
SInt32
SplitView::GetMetric(ThemeMetric inMetric, SInt32 inDefault)
{
    SInt32      value;
    OSStatus    err;
    
    err = GetThemeMetric(inMetric, &value);
    
    if (err != noErr)
        value = inDefault;
    
    return (value);
}

// ------------------------------------------------------------------------------------------
OSPtr<HIViewRef>
SplitView::Create(
    const HIViewID& inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    ControlSize     inSize, 
    bool            inIsVertical, 
    SInt32          inValue,
    SInt32          inMinimum,
    SInt32          inMaximum)
{
    OSPtr<Collection>   collPtr(NewCollection(), from_copy);
    
    SizeCollectionItem       ::Set(collPtr, inSize);
    OrientationCollectionItem::Set(collPtr, inIsVertical);
    ValueCollectionItem      ::Set(collPtr, inValue);
    MinimumCollectionItem    ::Set(collPtr, inMinimum);
    MaximumCollectionItem    ::Set(collPtr, inMaximum);
    
    return (CustomView::Create<SplitView>(inViewID, inSuperview, inFrame, inFromNib, 
                                          collPtr));
}

// ------------------------------------------------------------------------------------------
EventTarget*
SplitView::Instantiate(HIObjectRef inObjectRef)
{
    return (new SplitView(inObjectRef, VIEW_FLAGS));
}

// ------------------------------------------------------------------------------------------
SplitView::SplitView(
    HIObjectRef     inObjectRef, 
    EViewFlags      inViewFlags)
        : CustomView(inObjectRef, inViewFlags | VIEW_FLAGS), 
          mSize(kControlSizeNormal), mVertical(true), mCursor(kThemeResizeLeftRightCursor), 
          mInitialViewAID(0), mInitialViewBID(0)
{
}

// ------------------------------------------------------------------------------------------
void
SplitView::Initialize(
    EventRef        inEvent)
{
    SInt32  size, subviewID, maxval, minval, curval;
    bool    vertical;
    
    // Read the size, if present.
    
    if (SizeParam::Get(inEvent, size, std::nothrow))
        mSize = size;
    
    // Read the orientation, if present.
    
    if (OrientationParam::Get(inEvent, vertical, std::nothrow))
        SetOrientation(vertical);
    
    // Read the ID of subview A, if present.
    
    if (SubviewAIDParam::Get(inEvent, subviewID, std::nothrow))
        mInitialViewAID = subviewID;
    
    // Read the ID of subview B, if present.
    
    if (SubviewBIDParam::Get(inEvent, subviewID, std::nothrow))
        mInitialViewBID = subviewID;
    
    // Read the maximum value, if present.
    
    if (MaximumParam::Get(inEvent, maxval, std::nothrow))
        SetControl32BitMaximum(GetViewRef(), maxval);
    
    // Read the minimum value, if present.
    
    if (MinimumParam::Get(inEvent, minval, std::nothrow))
        SetControl32BitMinimum(GetViewRef(), minval);
    
    // Read the current value, if present.
    
    if (ValueParam::Get(inEvent, curval, std::nothrow))
        SetControl32BitValue(GetViewRef(), curval);
}

// ------------------------------------------------------------------------------------------
void
SplitView::InitializeView(
    Collection      inCollection)
{
    CustomView::InitializeView(inCollection);
    
    if (inCollection != NULL)
    {
        ControlSize size;
        bool        vertical;
        
        // Read the control size out of the collection, if present.
        
        if (SizeCollectionItem::Get(inCollection, size, std::nothrow))
        {
            mSize = size;
        }
        
        // Read the orientation out of the collection, if present.
        
        if (OrientationCollectionItem::Get(inCollection, vertical, std::nothrow))
        {
            mVertical = vertical;
        }
    }
    
    SetupTracker(GetOwner());
}

// ------------------------------------------------------------------------------------------
void
SplitView::Awaken(
    Nib*    /* inFromNib */)
{
    if (mInitialViewAID != 0)
        SetSubview(mInitialViewAID, mSubviewA);
    
    if (mInitialViewBID != 0)
        SetSubview(mInitialViewBID, mSubviewB);
}

// ------------------------------------------------------------------------------------------
const CustomView::ViewPartVector&
SplitView::GetViewParts() const
{
    static ViewPartVector   sViewParts;
    
    if (sViewParts.empty())
    {
        sViewParts.push_back(ViewPart(kViewPartViewA,    kFocusNever));
        sViewParts.push_back(ViewPart(kViewPartSplitter, kFocusNever));
        sViewParts.push_back(ViewPart(kViewPartViewB,    kFocusNever));
    }
    
    return (sViewParts);
}

// ------------------------------------------------------------------------------------------
ControlKind
SplitView::GetKind() const
{
    ControlKind kind    = { kControlKindSig, kControlKind };
    
    return (kind);
}

// ------------------------------------------------------------------------------------------
void
SplitView::Draw(
    HIViewPartCode  inPart, 
    CGContextRef    inContext, 
    const Shape*    /* inDrawShape */)
{
    if ((inPart == kHIViewEntireView) || (inPart == kViewPartSplitter))
    {
        Rect    viewABounds, splitterBounds, viewBBounds;
        
        CalcPartBounds(viewABounds, splitterBounds, viewBBounds);
        
        if (mSplitterImage.HasContent())
        {
            Rect    imageBounds;
            
            imageBounds.center(splitterBounds.center(), 
                               mSplitterImage.GetIdealSize(splitterBounds.size));
            
            mSplitterImage.Draw(inContext, imageBounds, kAlignAbsoluteCenter, 
                                kTransformNone);
        }
        else if (HIThemeDrawPaneSplitter != NULL)
        {
            HIThemeSplitterDrawInfo splitterInfo;
            WindowRef               owner;
            OSStatus                err;

            splitterInfo.version = 0;
            
            switch (GetControlHilite(GetViewRef()))
            {
            case kHIViewInactivePart:
                // inactive, may or may not be enabled
                if (IsControlEnabled(GetViewRef()))
                    splitterInfo.state = kThemeStateInactive;
                else
                    splitterInfo.state = kThemeStateUnavailableInactive;
                break;
                
            case kHIViewDisabledPart:
                // active but not enabled
                splitterInfo.state = kThemeStateUnavailable;
                break;
                
            default:
                splitterInfo.state = kThemeStateActive;
                break;
            }
            
            if (((owner = GetControlOwner(GetViewRef())) != NULL) && 
                WindowUtils::IsMetal(owner))
            {
                splitterInfo.adornment = kHIThemeSplitterAdornmentMetal;
            }
            else
            {
                splitterInfo.adornment = kHIThemeSplitterAdornmentNone;
            }
            
            err = HIThemeDrawPaneSplitter(&splitterBounds, &splitterInfo, inContext, 
                                          kHIThemeOrientationNormal);
            B_THROW_IF_STATUS(err);
        }
    }
}

// ------------------------------------------------------------------------------------------
/*! The default implementation does nothing.
*/
HIViewPartCode
SplitView::Track(
    const Point&        inMouseLoc, 
    UInt32              /* inClickCount */,
    UInt32&             /* ioKeyModifiers */)
{
    HIViewPartCode  currPart    = HitTest(inMouseLoc);
    HIViewPartCode  lastPart    = kHIViewNoPart;
    Point           currPt      = inMouseLoc;
    AxisPoint       axisPt      = GetAxisPoint();
    AxisSize        axisSz      = GetAxisSize();
    bool            done        = false;
    Size            mouseDelta;
    Rect            bounds      = GetBounds();
    WindowRef       owner;
    GrafPtr         ownerPort;
    
    if (currPart != kViewPartSplitter)
        return (kHIViewNoPart);
    
    currPt.*axisPt = ValueToMouse(bounds.size.*axisSz);
    
    mouseDelta.width    = inMouseLoc.x - currPt.x;
    mouseDelta.height   = inMouseLoc.y - currPt.y;
    
    owner = GetOwner();
    B_ASSERT(owner != NULL);
    ownerPort = GetWindowStructurePort(owner);
    B_ASSERT(ownerPort != NULL);
    
    while (true)
    {
        MouseTrackingResult result;
        ::Point             qdPt;
        OSStatus            err;
        
        if (currPart != lastPart)
            SetHighlight(currPart);
        
        if (done)
            break;
        
        err = TrackMouseLocation(ownerPort, &qdPt, &result);
        B_THROW_IF_STATUS(err);
        
        switch (result)
        {
        case kMouseTrackingMouseUp:
        case kMouseTrackingUserCancelled:
            currPart = kHIViewNoPart;
            done = true;
            break;
            
        default:
            {
                currPt = qdPt;
                currPt = ViewUtils::Convert(Point(qdPt), GetOwner(), GetViewRef());
                
                SetValue(MouseToValue(bounds.size.*axisSz, mouseDelta.*axisSz, currPt.*axisPt));
                
                lastPart = currPart;
                currPart = HitTest(currPt);
            }
            break;
        }
    }
    
    return (kViewPartSplitter);
}

// ------------------------------------------------------------------------------------------
Shape
SplitView::GetOpaqueShape() const
{
    return Shape();
}

// ------------------------------------------------------------------------------------------
Shape
SplitView::GetPartShape(
    HIViewPartCode  inPart) const
{
    return Shape(GetPartBounds(inPart));
}

// ------------------------------------------------------------------------------------------
Rect
SplitView::GetPartBounds(
    HIViewPartCode  inPart) const
{
    Rect    viewABounds, splitterBounds, viewBBounds, partBounds;
    
    CalcPartBounds(viewABounds, splitterBounds, viewBBounds);
    
    switch (inPart)
    {
    case kViewPartViewA:    partBounds = viewABounds;       break;
    case kViewPartSplitter: partBounds = splitterBounds;    break;
    case kViewPartViewB:    partBounds = viewBBounds;       break;
    
    default:
        partBounds = CGRectZero;
        break;
    }
    
    return partBounds;
}

// ------------------------------------------------------------------------------------------
void
SplitView::SetSubview(
    SInt32              inID, 
    OSPtr<HIViewRef>&   ioSubviewPtr)
{
    HIViewID    viewID;
    HIViewRef   subview;
    OSStatus    err;
    
    err = GetControlID(GetViewRef(), &viewID);
    B_THROW_IF_STATUS(err);
    
    viewID.id = inID;
    
    err = HIViewFindByID(GetViewRef(), viewID, &subview);
    B_THROW_IF_STATUS(err);
    
    SetSubview(subview, ioSubviewPtr);
}

// ------------------------------------------------------------------------------------------
void
SplitView::SetSubview(
    HIViewRef           inSubview, 
    OSPtr<HIViewRef>&   ioSubviewPtr)
{
    ioSubviewPtr.reset(inSubview);
    if (ioSubviewPtr != NULL)
        AddSubview(ioSubviewPtr);
    ResizeSubviews();
    Invalidate();
}

// ------------------------------------------------------------------------------------------
void
SplitView::ResizeSubviews()
{
    Rect    viewABounds, splitterBounds, viewBBounds;
    bool    invalA, invalB;

    CalcPartBounds(viewABounds, splitterBounds, viewBBounds);
    
    invalA = ResizeSubview(mSubviewA, viewABounds);
    invalB = ResizeSubview(mSubviewB, viewBBounds);

    if (invalA || invalB)
    {
        UpdateTracker();
        Invalidate();
    }
}

// ------------------------------------------------------------------------------------------
bool
SplitView::ResizeSubview(
    HIViewRef   inSubview, 
    const Rect& inFrame)
{
    bool    invalidated = false;
    
    if (inSubview != NULL)
    {
        if ((inFrame.size.width == 0) || (inFrame.size.height == 0))
        {
            // The view is shrunk down so much as to have disappeared entirely.
            // Hide the view if it's currently visible.
            
            if (HIViewIsVisible(inSubview))
            {
                HIViewSetVisible(inSubview, false);
                invalidated = true;
            }
        }
        else
        {
            // The view should be visible.
            
            if (!HIViewIsVisible(inSubview))
            {
                HIViewSetVisible(inSubview, true);
                invalidated = true;
            }
            
            Rect        currFrame;
            OSStatus    err;
            
            err = HIViewGetFrame(inSubview, &currFrame);
            B_THROW_IF_STATUS(err);
            
            if (currFrame != inFrame)
            {
                err = HIViewSetFrame(inSubview, &inFrame);
                B_THROW_IF_STATUS(err);
                
                invalidated = true;
            }
        }
    }
    
    return (invalidated);
}

// ------------------------------------------------------------------------------------------
void
SplitView::CalcPartBounds(
    Rect&   outViewABounds, 
    Rect&   outSplitterBounds, 
    Rect&   outViewBBounds) const
{
    AxisPoint   axisPt  = GetAxisPoint();
    AxisSize    axisSz  = GetAxisSize();
    Rect        bounds  = GetBounds();
    
    outViewABounds = outSplitterBounds = outViewBBounds = bounds;
    
    CalcPartSizes(bounds.size.*axisSz, 
                  outViewABounds.origin.*axisPt, outViewABounds.size.*axisSz, 
                  outSplitterBounds.origin.*axisPt, outSplitterBounds.size.*axisSz, 
                  outViewBBounds.origin.*axisPt, outViewBBounds.size.*axisSz);
}

// ------------------------------------------------------------------------------------------
void
SplitView::CalcPartSizes(
    float   inTotalSize,
    float&  outViewAPos, 
    float&  outViewASize, 
    float&  outSplitterPos, 
    float&  outSplitterSize,
    float&  outViewBPos, 
    float&  outViewBSize) const
{
    SInt32  value   = GetValue();
    SInt32  minval  = GetMinimum();
    SInt32  maxval  = GetMaximum();
    
    outViewAPos     = 0.0f;
    outSplitterSize = GetSplitterSize();
    
    if (value <= minval)
    {
        outViewASize    = 0.0f;
        outSplitterPos  = 0.0f;
        outViewBPos     = outSplitterSize;
        outViewBSize    = inTotalSize - outSplitterSize;
    }
    else if (value >= maxval)
    {
        outViewBPos     = inTotalSize;
        outViewBSize    = 0.0f;
        outSplitterPos  = inTotalSize - outSplitterSize;
        outViewASize    = inTotalSize - outSplitterSize;
    }
    else
    {
        float   pos = value;
        
        pos /= maxval - minval;
        pos *= (inTotalSize - outSplitterSize);
        pos  = round(pos);
        
        outViewASize    = pos;
        outSplitterPos  = pos;
        outViewBPos     = pos + outSplitterSize;
        outViewBSize    = inTotalSize - outViewASize - outSplitterSize;
    }
}

// ------------------------------------------------------------------------------------------
void
SplitView::SetOrientation(
    bool        inVertical)
{
    mVertical = inVertical;
    
    if (mVertical)
        mCursor = kThemeResizeLeftRightCursor;
    else
        mCursor = kThemeResizeUpDownCursor;
    
    Reshape();
    Invalidate();
}

// ------------------------------------------------------------------------------------------
float
SplitView::ValueToMouse(
    float           inSize) const
{
    SInt32  value   = GetValue();
    SInt32  minval  = GetMinimum();
    SInt32  maxval  = GetMaximum();
    float   loc;
    
    if (value <= minval)
    {
        loc = 0.0f;
    }
    else if (value >= maxval)
    {
        loc = inSize;
    }
    else
    {
        loc  = value;
        loc /= maxval - minval;
        loc *= inSize;
        loc  = roundf(loc);
    }
    
    return (loc);
}

// ------------------------------------------------------------------------------------------
SInt32
SplitView::MouseToValue(
    float           inSize,
    float           inMouseDelta,
    float           inMouseLoc) const
{
    float   pos = (inMouseLoc + inMouseDelta) / inSize;
    
    pos *= (GetMaximum() - GetMinimum());
    
    return (static_cast<SInt32>(roundf(pos)));
}

// ------------------------------------------------------------------------------------------
bool
SplitView::SetData(
    HIViewPartCode  inPart, 
    ResType         inDataTag, 
    const void*     inDataPtr, 
    size_t          inDataSize)
{
    bool    handled = true;
    
    switch (inDataTag)
    {
    case kSizeTag:
        mSize = Data<kSizeTag>::Get(inDataPtr, inDataSize);
        Reshape();
        Invalidate();
        break;
        
    case kOrientationTag:
        SetOrientation(Data<kOrientationTag>::Get(inDataPtr, inDataSize));
        break;
        
    case kContentTag:
        {
            ControlButtonContentInfo    content;
            
            Data<kContentTag>::Get(inDataPtr, inDataSize, content);
            
            mSplitterImage = content;
            InvalidatePart(kViewPartSplitter);
        }
        break;
        
    case kSubviewTag:
        if (inPart == kViewPartViewA)
        {
            SetSubview(Data<kSubviewTag>::Get(inDataPtr, inDataSize), mSubviewA);
        }
        else if (inPart == kViewPartViewB)
        {
            SetSubview(Data<kSubviewTag>::Get(inDataPtr, inDataSize), mSubviewB);
        }
        else
        {
            B_THROW_STATUS(errInvalidPartCode);
        }
        break;
        
    default:
        handled = CustomView::SetData(inPart, inDataTag, inDataPtr, inDataSize);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
SplitView::GetData(
    HIViewPartCode  inPart, 
    ResType         inDataTag, 
    void*           inDataPtr, 
    size_t&         ioDataSize) const
{
    bool    handled = true;
    
    switch (inDataTag)
    {
    case kSizeTag:
        Data<kSizeTag>::Set(inDataPtr, ioDataSize, mSize);
        break;
        
    case kOrientationTag:
        Data<kOrientationTag>::Set(inDataPtr, ioDataSize, mVertical);
        break;
        
    case kContentTag:
        {
            ControlButtonContentInfo    content;
            
            mSplitterImage.GetContentInfo(content);
            Data<kContentTag>::Set(inDataPtr, ioDataSize, content);
        }
        break;
        
    case kSubviewTag:
        if (inPart == kViewPartViewA)
        {
            if (mSubviewA != NULL)
                CFRetain(mSubviewA.get());
            Data<kSubviewTag>::Set(inDataPtr, ioDataSize, mSubviewA);
        }
        else if (inPart == kViewPartViewB)
        {
            if (mSubviewB != NULL)
                CFRetain(mSubviewB.get());
            Data<kSubviewTag>::Set(inDataPtr, ioDataSize, mSubviewB);
        }
        else
        {
            B_THROW_STATUS(errInvalidPartCode);
        }
        break;
        
    default:
        handled = CustomView::GetData(inPart, inDataTag, inDataPtr, ioDataSize);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
void
SplitView::OwningWindowChanged(
    WindowRef       inOriginalWindow, 
    WindowRef       inCurrentWindow)
{
    CustomView::OwningWindowChanged(inOriginalWindow, inCurrentWindow);
    SetupTracker(inCurrentWindow);
}

// ------------------------------------------------------------------------------------------
bool
SplitView::BoundsChanged(
    UInt32          /* inAttributes */, 
    const Rect&     /* inPreviousBounds */, 
    const Rect&     /* inCurrentBounds */)
{
    ResizeSubviews();
    
    return (true);
}

// ------------------------------------------------------------------------------------------
void
SplitView::ValueFieldChanged()
{
    ResizeSubviews();
    CustomView::ValueFieldChanged();
}

// ------------------------------------------------------------------------------------------
void
SplitView::TrackingAreaEntered(
    HIViewTrackingAreaRef   inTrackingRef, 
    const Point&            /* inMouseLocation */, 
    UInt32                  /* inKeyModifiers */)
{
    if (inTrackingRef != mSplitTracker)
        return;
    
//  printf("MouseEntered %p\n", inTrackingRef);
    
    mCursor.Set();
}

// ------------------------------------------------------------------------------------------
void
SplitView::TrackingAreaExited(
    HIViewTrackingAreaRef   inTrackingRef, 
    const Point&            /* inMouseLocation */, 
    UInt32                  /* inKeyModifiers */)
{
    if (inTrackingRef != mSplitTracker)
        return;
    
//  printf("MouseExited %p\n", inTrackingRef);

    OSStatus    err;
    
    err = SetThemeCursor(kThemeArrowCursor);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
SplitView::SetupTracker(
    WindowRef   inOwner)
{
    if (mSplitTracker != NULL)
    {
        mSplitTracker.reset();
    }
    
    if (inOwner != NULL)
    {
        mSplitTracker.assign(GetViewRef(), CalcTrackerShape().cf_ref());
    }
}

// ------------------------------------------------------------------------------------------
void
SplitView::UpdateTracker()
{
    if (mSplitTracker != NULL)
    {
        mSplitTracker.SetShape(CalcTrackerShape().cf_ref());
    }
}

// ------------------------------------------------------------------------------------------
Shape
SplitView::CalcTrackerShape() const
{
    return GetPartShape(kViewPartSplitter);
}

// ------------------------------------------------------------------------------------------
void
SplitView::PrintDebugInfo()
{
    printf("B::SplitView\n");
    
    CustomView::PrintDebugInfo();
}


#ifndef DOXYGEN_SKIP
B_VAR_EXPORT EventTargetRegistrar<SplitView>    gRegisterSplitView;
#endif


}   // namespace B
