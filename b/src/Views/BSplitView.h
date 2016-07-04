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

#ifndef BSplitView_H_
#define BSplitView_H_

#pragma once

// B headers
#include "BAutoTrackingArea.h"
#include "BCursor.h"
#include "BCustomView.h"
#include "BString.h"
#include "BUrl.h"
#include "BViewData.h"
#include "BViewUtils.h"


namespace B {

/*! @brief  A custom view implementing a Help button.
    
    @note       This view requires a composited window.
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class SplitView : public CustomView
{
public:
    
    // constants
    enum    { kViewPartViewA    = 1,
              kViewPartSplitter, 
              kViewPartViewB
            };
    enum    { kControlKindSig   = kEventClassB, 
              kControlKind      = FOUR_CHAR_CODE('Spli')
            };
    enum    {
              kSizeTag          = kControlSizeTag,
              kOrientationTag   = FOUR_CHAR_CODE('Orie'),
              kSubviewTag       = FOUR_CHAR_CODE('Subv'),
              kContentTag       = FOUR_CHAR_CODE('cont'),   // same as kControlBevelButtonContentTag, etc.
              kCursorTag        = FOUR_CHAR_CODE('Curs')
            };
    enum    {
              // Collection tags -- only used at initialisation time.
              kSubviewAIDTag    = FOUR_CHAR_CODE('SvwA'),
              kSubviewBIDTag    = FOUR_CHAR_CODE('SvwB')
            };
    
    //! @name Object Creation
    //@{
    //! Create an object and its HIToolbox view, from the given parameters.
    static OSPtr<HIViewRef>
            Create(
                const HIViewID& inViewID,
                HIViewRef       inSuperview,
                const Rect*     inFrame,
                Nib*            inFromNib,
                ControlSize     inSize,
                bool            inIsVertical,
                SInt32          inValue,
                SInt32          inMinimum,
                SInt32          inMaximum);
    //@}
    
    //! @name Instantiation
    //@{
    //! Create a C++ object given an @c HIObjectRef.
    static EventTarget* Instantiate(HIObjectRef inObjectRef);
    //@}
    
    // class ID
    static const CFStringRef    kHIObjectClassID;
    
protected:
    
    // constructors / destructor
                    SplitView(
                        HIObjectRef     inObjectRef,
                        EViewFlags      inViewFlags);
    
    virtual const ViewPartVector&
                    GetViewParts() const;
    virtual ControlKind
                    GetKind() const;
    virtual void    Initialize(
                        EventRef        inEvent);
    virtual void    InitializeView(
                        Collection      inCollection);
    virtual void    Awaken(
                        Nib*            inFromNib);
    virtual Shape   GetOpaqueShape() const;
    virtual Shape   GetPartShape(
                        HIViewPartCode  inPart) const;
    virtual Rect    GetPartBounds(
                        HIViewPartCode  inPart) const;
    virtual void    Draw(
                        HIViewPartCode  inPart, 
                        CGContextRef    inContext, 
                        const Shape*    inDrawShape);
    virtual HIViewPartCode
                    Track(
                        const Point&    inMouseLoc, 
                        UInt32          inClickCount,
                        UInt32&         ioKeyModifiers);
    virtual bool    SetData(
                        HIViewPartCode  inPart, 
                        ResType         inDataTag, 
                        const void*     inDataPtr, 
                        size_t          inDataSize);
    virtual bool    GetData(
                        HIViewPartCode  inPart, 
                        ResType         inDataTag, 
                        void*           inDataPtr, 
                        size_t&         ioDataSize) const;
    virtual void    OwningWindowChanged(
                        WindowRef       inOriginalWindow, 
                        WindowRef       inCurrentWindow);
    virtual bool    BoundsChanged(
                        UInt32          inAttributes, 
                        const Rect&     inPreviousBounds, 
                        const Rect&     inCurrentBounds);
    virtual void    ValueFieldChanged();
    virtual void    TrackingAreaEntered(
                        HIViewTrackingAreaRef inTrackingRef,
                        const Point&    inMouseLocation, 
                        UInt32          inKeyModifiers);
    virtual void    TrackingAreaExited(
                        HIViewTrackingAreaRef inTrackingRef,
                        const Point&    inMouseLocation, 
                        UInt32          inKeyModifiers);

    // overrides from EventTarget
    virtual void    PrintDebugInfo();
    
private:
    
    // types
    template <ResType TAG> struct Data : public ViewData<kControlKindSig, kControlKind, TAG> {};
    typedef float (HIPoint::*AxisPoint);
    typedef float (HISize::*AxisSize);
    
    static SInt32   GetMetric(ThemeMetric inMetric, SInt32 inDefault);
    void            SetSubview(
                        SInt32              inID, 
                        OSPtr<HIViewRef>&   ioSubviewPtr);
    void            SetSubview(
                        HIViewRef           inSubview, 
                        OSPtr<HIViewRef>&   ioSubviewPtr);
    void            ResizeSubviews();
    bool            ResizeSubview(
                        HIViewRef       inSubview, 
                        const Rect&     inFrame);
    void            CalcPartBounds(
                        Rect&           outViewABounds, 
                        Rect&           outSplitterBounds, 
                        Rect&           outViewBBounds) const;
    void            CalcPartSizes(
                        float           inTotalSize,
                        float&          outViewAPos, 
                        float&          outViewASize, 
                        float&          outSplitterPos, 
                        float&          outSplitterSize,
                        float&          outViewBPos, 
                        float&          outViewBSize) const;
    void            SetOrientation(
                        bool            inVertical);
    float           ValueToMouse(
                        float           inSize) const;
    SInt32          MouseToValue(
                        float           inSize,
                        float           inMouseDelta,
                        float           inMouseLoc) const;
    void            SetupTracker(
                        WindowRef       inOwner);
    void            UpdateTracker();
    Shape           CalcTrackerShape() const;
    float           GetSplitterSize() const;
    AxisPoint       GetAxisPoint() const;
    AxisSize        GetAxisSize() const;
    AxisPoint       GetContraAxisPoint() const;
    AxisSize        GetContraAxisSize() const;
    
    // member variables
    ControlSize             mSize;
    bool                    mVertical;
    OSPtr<HIViewRef>        mSubviewA;
    OSPtr<HIViewRef>        mSubviewB;
    ViewUtils::ImageContent mSplitterImage;
    AutoTrackingArea        mSplitTracker;
    Cursor                  mCursor;
    SInt32                  mInitialViewAID;
    SInt32                  mInitialViewBID;
    
    // static member variables
    static const float  kNormalSplitterSize;
    static const float  kSmallSplitterSize;
};

// ------------------------------------------------------------------------------------------
inline float
SplitView::GetSplitterSize() const
{
    return ((mSize == kControlSizeSmall) ? kSmallSplitterSize : kNormalSplitterSize);
}

// ------------------------------------------------------------------------------------------
inline SplitView::AxisPoint
SplitView::GetAxisPoint() const
{
    return (mVertical ? &HIPoint::x : &HIPoint::y);
}

// ------------------------------------------------------------------------------------------
inline SplitView::AxisSize
SplitView::GetAxisSize() const
{
    return (mVertical ? &HISize::width : &HISize::height);
}

// ------------------------------------------------------------------------------------------
inline SplitView::AxisPoint
SplitView::GetContraAxisPoint() const
{
    return (mVertical ? &HIPoint::y : &HIPoint::x);
}

// ------------------------------------------------------------------------------------------
inline SplitView::AxisSize
SplitView::GetContraAxisSize() const
{
    return (mVertical ? &HISize::height : &HISize::width);
}


#ifndef DOXYGEN_SKIP

template <> struct ViewDataDef<
    SplitView::kControlKindSig, 
    SplitView::kControlKind, 
    SplitView::kSizeTag>            { typedef ViewDataTrait<ControlSize>    Trait;  };

template <> struct ViewDataDef<
    SplitView::kControlKindSig, 
    SplitView::kControlKind, 
    SplitView::kOrientationTag>     { typedef ViewDataTrait<Boolean>        Trait;  };

template <> struct ViewDataDef<
    SplitView::kControlKindSig, 
    SplitView::kControlKind, 
    SplitView::kSubviewTag>         { typedef ViewDataTrait<HIViewRef>      Trait;  };

template <> struct ViewDataDef<
    SplitView::kControlKindSig, 
    SplitView::kControlKind, 
    SplitView::kContentTag>         { typedef ViewDataTrait<HIViewImageContentInfo> Trait;  };

template <> struct ViewDataDef<
    SplitView::kControlKindSig, 
    SplitView::kControlKind, 
    SplitView::kCursorTag>          { typedef ViewDataTrait<ThemeCursor>    Trait;  };

#endif  // DOXYGEN_SKIP


}   // namespace B

#endif  // BSplitView_H_
