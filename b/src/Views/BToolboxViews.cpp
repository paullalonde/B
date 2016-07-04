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
#include "BToolboxViews.h"

// standard headers
#include <stdint.h>

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BColor.h"
#include "BEvent.h"
#include "BViewFactory.h"
#include "BWindowUtils.h"


namespace
{
    template <class T> class ToolboxViewRegistrar
    {
    public:
        ToolboxViewRegistrar(CFStringRef inClassID)
            {
                B::ViewFactory::Default().template Register<T>(inClassID);
            }
    };
    
    /*! This function returns a @c WindowRef suitable for use as the first argument to the 
        CreateXXXControl functions.  It's meant to be used for controls exhibiting the 
        bug mentionned in Apple's Q&A #1211.  According to that document, certain controls 
        can't be created with a @c NULL @c WindowRef.  So the Create() functions for those 
        controls call this function to get a proper @c WindowRef, which may be that of a
        substitute window.
    */
    WindowRef   GetSuperviewWindow(HIViewRef inSuperview)
    {
        WindowRef   windowRef   = NULL;
        
        // If we have a superview, get the superview's owning window.
        
        if (inSuperview != NULL)
            windowRef = GetControlOwner(inSuperview);
        
        if (windowRef == NULL)
        {
            // NULL window;  we need to get our substitute window.
            
            static B::OSPtr<WindowRef>  windowPtr;  // will release at app shutdown.
            
            if (windowPtr == NULL)
            {
                // First time 'round:  create the window.
                
                ::Rect      bounds  = { 0, 0, 100, 100 };
                WindowRef   ref;
                OSStatus    err;
                
                err = CreateNewWindow(kPlainWindowClass, kWindowNoAttributes,
                                      &bounds, &ref);
                B_THROW_IF_STATUS(err);
                
                windowPtr.reset(ref, B::from_copy);
            }
            
            windowRef = windowPtr;
        }
        
        return (windowRef);
    }
}


namespace B {

// ==========================================================================================
//  BevelButton

// ------------------------------------------------------------------------------------------
View*
BevelButton::Instantiate(HIViewRef inViewRef)
{
    return (new BevelButton(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
BevelButton*
BevelButton::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const String&                   inTitle, 
    ControlBevelThickness           inThickness, 
    ControlBevelButtonBehavior      inBehavior, 
    const HIViewImageContentInfo&   inContent, 
    MenuRef                         inMenuRef, 
    bool                            inViewOwnsMenu, 
    ControlBevelButtonMenuBehavior  inMenuBehavior,
    ControlBevelButtonMenuPlacement inMenuPlacement)
{
    HIViewImageContentInfo  content = inContent;
    SInt16                  menuID  = (inMenuRef != NULL) ? -12345 : 0;
    ::Rect                  qdRect  = { 0, 0, 100, 100 };
    HIViewRef               viewRef;
    BevelButton*            viewObj;
    OSStatus                err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateBevelButtonControl(NULL, &qdRect, 
                                   inTitle.cf_ref(), inThickness, inBehavior, &content, 
                                   menuID, inMenuBehavior, inMenuPlacement, 
                                   &viewRef);
    B_THROW_IF_STATUS(err);
    
    viewObj = SetupTypedView<BevelButton>(
                    inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory);
    
    if (inMenuRef != NULL)
    {
        // Ensure the view is released if we get an exception.
        OSPtr<HIViewRef>    viewPtr(*viewObj, from_copy);
        
        viewObj->SetMenu(inMenuRef, inViewOwnsMenu);
        
        viewPtr.release();
    }
    
    return (viewObj);
}

// ------------------------------------------------------------------------------------------
BevelButton::BevelButton(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
ThemeButtonKind
BevelButton::GetButtonKind() const
{
    return (Data<kControlBevelButtonKindTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetButtonKind(ThemeButtonKind inButtonKind)
{
    Data<kControlBevelButtonKindTag>::Set(*this, inButtonKind);
}

// ------------------------------------------------------------------------------------------
ControlButtonTextAlignment
BevelButton::GetTextAlignment() const
{
    return (Data<kControlBevelButtonTextAlignTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetTextAlignment(ControlButtonTextAlignment inAlignment)
{
    Data<kControlBevelButtonTextAlignTag>::Set(*this, inAlignment);
}

// ------------------------------------------------------------------------------------------
SInt16
BevelButton::GetTextOffset() const
{
    return (Data<kControlBevelButtonTextOffsetTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetTextOffset(SInt16 inOffset)
{
    Data<kControlBevelButtonTextOffsetTag>::Set(*this, inOffset);
}

// ------------------------------------------------------------------------------------------
ControlButtonTextPlacement
BevelButton::GetTextPlacement() const
{
    return (Data<kControlBevelButtonTextPlaceTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetTextPlacement(ControlButtonTextPlacement inPlacement)
{
    OSStatus    err;
    
    err = SetBevelButtonTextPlacement(*this, inPlacement);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
BevelButton::GetGraphic(HIViewImageContentInfo& outContent) const
{
    OSStatus    err;
    
    err = GetBevelButtonContentInfo(*this, &outContent);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetGraphic(const HIViewImageContentInfo& inContent)
{
    HIViewImageContentInfo  content = inContent;
    OSStatus                err;
    
    err = SetBevelButtonContentInfo(*this, &content);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
IconTransformType
BevelButton::GetGraphicTransform() const
{
    return (Data<kControlBevelButtonTransformTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetGraphicTransform(IconTransformType inTransform)
{
    OSStatus    err;
    
    err = SetBevelButtonTransform(*this, inTransform);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
ControlButtonGraphicAlignment
BevelButton::GetGraphicAlignment() const
{
    return (Data<kControlBevelButtonGraphicAlignTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetGraphicAlignment(ControlButtonGraphicAlignment inAlignment)
{
    Data<kControlBevelButtonGraphicAlignTag>::Set(*this, inAlignment);
}

// ------------------------------------------------------------------------------------------
Point
BevelButton::GetGraphicOffset() const
{
    return (Data<kControlBevelButtonGraphicOffsetTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetGraphicOffset(const Point& inOffset)
{
    Data<kControlBevelButtonGraphicOffsetTag>::Set(*this, inOffset);
}

// ------------------------------------------------------------------------------------------
bool
BevelButton::GetScaleIcon() const
{
    return (Data<kControlBevelButtonScaleIconTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetScaleIcon(bool scaleIt)
{
    Data<kControlBevelButtonScaleIconTag>::Set(*this, scaleIt);
}

// ------------------------------------------------------------------------------------------
MenuRef
BevelButton::GetMenu() const
{
    MenuRef     menu;
    OSStatus    err;
    
    err = GetBevelButtonMenuHandle(*this, &menu);
    B_THROW_IF_STATUS(err);
    
    return (menu);
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetMenu(MenuRef inMenu, bool inViewOwnsMenu)
{
    if (inViewOwnsMenu)
        Data<kControlBevelButtonOwnedMenuRefTag>::Set(*this, inMenu);
    else
        Data<kControlBevelButtonMenuRefTag>::Set(*this, inMenu);
}

// ------------------------------------------------------------------------------------------
SInt16
BevelButton::GetMenuValue() const
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    MenuItemIndex   value;
#else
    SInt16          value;
#endif
    OSStatus        err;

    err = GetBevelButtonMenuValue(*this, &value);
    B_THROW_IF_STATUS(err);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetMenuValue(SInt16 inValue)
{
    OSStatus    err;
    
    err = SetBevelButtonMenuValue(*this, inValue);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
SInt32
BevelButton::GetMenuDelay() const
{
    return (Data<kControlBevelButtonMenuDelayTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetMenuDelay(SInt32 inDelay)
{
    Data<kControlBevelButtonMenuDelayTag>::Set(*this, inDelay);
}

// ------------------------------------------------------------------------------------------
bool
BevelButton::GetMenuMultiValued() const
{
    return (Data<kControlBevelButtonIsMultiValueMenuTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetMenuMultiValued(bool isMultivalued)
{
    Data<kControlBevelButtonIsMultiValueMenuTag>::Set(*this, isMultivalued);
}

// ------------------------------------------------------------------------------------------
bool
BevelButton::GetCenterPopupGlyph() const
{
    return (Data<kControlBevelButtonCenterPopupGlyphTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
BevelButton::SetCenterPopupGlyph(bool centerIt)
{
    Data<kControlBevelButtonCenterPopupGlyphTag>::Set(*this, centerIt);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<BevelButton>  gRegisterBevelButton(kHIBevelButtonClassID);
#endif


// ==========================================================================================
//  Slider

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
Slider::Instantiate(HIViewRef inViewRef)
{
    return (new Slider(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
Slider*
Slider::Create(
    const HIViewID&             inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                   inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                 inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                        inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*          inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    SInt32                      inValue, 
    SInt32                      inMinimum, 
    SInt32                      inMaximum, 
    ControlSliderOrientation    inOrientation, 
    UInt16                      inNumTickMarks, 
    bool                        inLiveTracking, 
    ControlActionUPP            inLiveTrackingUPP)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateSliderControl(NULL, &qdRect, 
                              inValue, inMinimum, inMaximum, inOrientation, 
                              inNumTickMarks, inLiveTracking, inLiveTrackingUPP, 
                              &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<Slider>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
Slider::Slider(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<Slider>   gRegisterSlider(kHISliderClassID);
#endif


// ==========================================================================================
//  DisclosureTriangle

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
DisclosureTriangle::Instantiate(HIViewRef inViewRef)
{
    return (new DisclosureTriangle(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
DisclosureTriangle*
DisclosureTriangle::Create(
    const HIViewID&                         inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                               inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                             inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                                    inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*                      inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    ControlDisclosureTriangleOrientation    inOrientation, 
    const String&                           inTitle, 
    SInt32                                  inInitialValue, 
    bool                                    inDrawTitle, 
    bool                                    inAutoToggle)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateDisclosureTriangleControl(NULL, &qdRect, 
                                          inOrientation, inTitle.cf_ref(), inInitialValue, 
                                          inDrawTitle, inAutoToggle, 
                                          &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<DisclosureTriangle>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
DisclosureTriangle::DisclosureTriangle(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
SInt16
DisclosureTriangle::GetLastValue() const
{
    return (Data<kControlTriangleLastValueTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
DisclosureTriangle::SetLastValue(SInt16 inLastValue)
{
    OSStatus    err;
    
    err = SetDisclosureTriangleLastValue(*this, inLastValue);
    B_THROW_IF_STATUS(err);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<DisclosureTriangle>   gRegisterDisclosureTriangle(kHIDisclosureTriangleClassID);
#endif


// ==========================================================================================
//  ProgressBar

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ProgressBar::Instantiate(HIViewRef inViewRef)
{
    return (new ProgressBar(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
ProgressBar*
ProgressBar::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    SInt32              inValue, 
    SInt32              inMinimum, 
    SInt32              inMaximum, 
    bool                inIndeterminate)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateProgressBarControl(NULL, &qdRect, 
                                   inValue, inMinimum, inMaximum, inIndeterminate, 
                                   &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ProgressBar>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ProgressBar::ProgressBar(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
bool
ProgressBar::GetIndeterminate() const
{
    return (Data<kControlProgressBarIndeterminateTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
bool
ProgressBar::GetAnimating() const
{
    return (Data<kControlProgressBarAnimatingTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ProgressBar::SetIndeterminate(bool inIndeterminate)
{
    Data<kControlProgressBarIndeterminateTag>::Set(*this, inIndeterminate);
}

// ------------------------------------------------------------------------------------------
void
ProgressBar::SetAnimating(bool inAnimating)
{
    Data<kControlProgressBarAnimatingTag>::Set(*this, inAnimating);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ProgressBar>  gRegisterProgressBar(kHIProgressBarClassID);
#endif


// ==========================================================================================
//  RelevanceBar

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
RelevanceBar::Instantiate(HIViewRef inViewRef)
{
    return (new RelevanceBar(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
RelevanceBar*
RelevanceBar::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    SInt32              inValue, 
    SInt32              inMinimum, 
    SInt32              inMaximum)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateRelevanceBarControl(NULL, &qdRect, 
                                   inValue, inMinimum, inMaximum, 
                                   &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<RelevanceBar>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
RelevanceBar::RelevanceBar(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<RelevanceBar> gRegisterRelevanceBar(kHIRelevanceBarClassID);
#endif


// ==========================================================================================
//  LittleArrows

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
LittleArrows::Instantiate(HIViewRef inViewRef)
{
    return (new LittleArrows(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
LittleArrows*
LittleArrows::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    SInt32              inValue, 
    SInt32              inMinimum, 
    SInt32              inMaximum, 
    SInt32              inIncrement)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateLittleArrowsControl(NULL, &qdRect, 
                                    inValue, inMinimum, inMaximum, inIncrement, 
                                    &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<LittleArrows>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
LittleArrows::LittleArrows(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
SInt32
LittleArrows::GetIncrement() const
{
    return (Data<kControlLittleArrowsIncrementValueTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
LittleArrows::SetIncrement(SInt32 inIncrement)
{
    Data<kControlLittleArrowsIncrementValueTag>::Set(*this, inIncrement);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<LittleArrows> gRegisterLittleArrows(kHILittleArrowsClassID);
#endif


// ==========================================================================================
//  ChasingArrows

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ChasingArrows::Instantiate(HIViewRef inViewRef)
{
    return (new ChasingArrows(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
ChasingArrows*
ChasingArrows::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory)      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateChasingArrowsControl(NULL, &qdRect, 
                                     &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ChasingArrows>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ChasingArrows::ChasingArrows(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
bool
ChasingArrows::GetAnimating() const
{
    return (Data<kControlChasingArrowsAnimatingTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ChasingArrows::SetAnimating(bool inAnimating)
{
    Data<kControlChasingArrowsAnimatingTag>::Set(*this, inAnimating);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ChasingArrows>    gRegisterChasingArrows(kHIChasingArrowsClassID);
#endif


// ==========================================================================================
//  TabbedView

#pragma mark -

namespace {
    
    /*! @brief  Functor that sets up a tabbed view's subviews.
    */
    class SetUpTabbedViewSubview : public std::unary_function<void, HIViewRef>
    {
    public:
                SetUpTabbedViewSubview(
                    const Rect&         inContentRect,
                    std::vector<View*>& ioUserPanes);
        
        void    operator () (HIViewRef viewRef) const;
        
    private:
        const Rect&         mContentRect;
        std::vector<View*>& mUserPanes;
    };
    
    SetUpTabbedViewSubview::SetUpTabbedViewSubview(
        const Rect&         inContentRect,
        std::vector<View*>& ioUserPanes)
            : mContentRect(inContentRect), mUserPanes(ioUserPanes)
    {
    }
    
    void
    SetUpTabbedViewSubview::operator () (HIViewRef viewRef) const
    {
        View*   subview = View::GetViewFromRef(viewRef, std::nothrow);
        
        if (subview != NULL)
        {
            mUserPanes.push_back(subview);
            HIViewSetFrame(*subview, &mContentRect);
            
            subview->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
                                 kHILayoutBindRight, kHILayoutBindBottom);
        }
    }
}

// ------------------------------------------------------------------------------------------
View*
TabbedView::Instantiate(HIViewRef inViewRef)
{
    return (new TabbedView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
TabbedView*
TabbedView::Create(
    const HIViewID&                     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*                  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    ControlTabSize                      inSize,
    ControlTabDirection                 inDirection, 
    const std::vector<ControlTabEntry>& inTabs)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateTabsControl(NULL, &qdRect, 
                            inSize, inDirection, inTabs.size(), &inTabs[0], 
                            &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<TabbedView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
TabbedView::TabbedView(HIViewRef inViewRef)
    : PredefinedView(inViewRef), 
      mEventHandler(inViewRef),
      mLastTabIndex(0)
{
    InitEventHandler();
    
    Rect        contentRect = GetContentRect();
    UInt16      numTabs;
    OSStatus    err;
    
    err = CountSubControls(*this, &numTabs);
    B_THROW_IF_STATUS(err);

    mUserPanes.reserve(numTabs);
    
    std::for_each(begin(), end(), 
                  SetUpTabbedViewSubview(contentRect, mUserPanes));
    
    ShowTab(GetValue());
}

// ------------------------------------------------------------------------------------------
void
TabbedView::InitEventHandler()
{
    mEventHandler.Add(this, &TabbedView::ControlValueFieldChanged);
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
Rect
TabbedView::GetContentRect() const
{
    ::Rect      qdRect;
    OSStatus    err;
    
    err = GetTabContentRect(*this, &qdRect);
    B_THROW_IF_STATUS(err);
    
    return Rect(qdRect);
}

// ------------------------------------------------------------------------------------------
void
TabbedView::SetTabID(int inTabIndex, int inID)
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    if (mUserPanes.size() < static_cast<size_t>(inTabIndex))
        mUserPanes.resize(inTabIndex, NULL);
    
    View*   userPane    = FindTypedSubview<View>(inID);
    
    B_ASSERT(userPane != NULL);
    
    mUserPanes[inTabIndex-1] = userPane;
}

// ------------------------------------------------------------------------------------------
bool
TabbedView::IsTabEnabled(int inTabIndex) const
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    return (Data<kControlTabEnabledFlagTag>::Get(*this, inTabIndex));
}

// ------------------------------------------------------------------------------------------
void
TabbedView::EnableTab(int inTabIndex, bool inEnable)
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    OSStatus    err;
    
    err = SetTabEnabled(*this, inTabIndex, inEnable);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
TabbedView::GetTabFont(int inTabIndex, ControlFontStyleRec& outFont) const
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    outFont = Data<kControlTabFontStyleTag>::Get(*this, inTabIndex);
}

// ------------------------------------------------------------------------------------------
void
TabbedView::SetTabFont(int inTabIndex, const ControlFontStyleRec& inFont)
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    Data<kControlTabFontStyleTag>::Set(*this, inFont, inTabIndex);
}

// ------------------------------------------------------------------------------------------
String
TabbedView::GetTabName(int inTabIndex) const
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    ControlTabInfoRecV1 info;
    String              name;
    
#if 1
    
    Data<kControlTabInfoTag>::Get(*this, info, inTabIndex);
    if (info.name != NULL)
        name.assign(info.name, from_copy);
    
#else

    // First, try version 1 tab info record.
    
    BlockZero(&info, sizeof(info));
    info.version = kControlTabInfoVersionOne;
    
    if (Data<kControlTabInfoTag>::Get(*this, info, inTabIndex))
    {
        name.Adopt(info.name, true);
    }
    else
    {
        // Couldn't get version 1 record;  try version 0.
        
        ControlTabInfoRec   info0;
        size_t              size    = sizeof(info0);
        
        BlockZero(&info0, sizeof(info0));
        info.version = kControlTabInfoVersionZero;
        
        Data<kControlTabInfoTag>::GetData(*this, size, &info0, inTabIndex);
        
        name.Adopt(CFStringCreateWithPascalString(NULL, info0.name, 
                                                  GetApplicationTextEncoding()), 
                   true);
    }
#endif
    
    return (name);
}

// ------------------------------------------------------------------------------------------
void
TabbedView::SetTabName(int inTabIndex, const String& inName)
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
#if 1

    ControlTabInfoRecV1 info;
    
    Data<kControlTabInfoTag>::Get(*this, info, inTabIndex);
    if (info.name != NULL)
        CFRelease(info.name);
    
    info.name = inName.cf_ref();
    
    Data<kControlTabInfoTag>::Set(*this, info, inTabIndex);

#else
    ControlTabInfoRecV1 info0;
    ControlTabInfoRecV1 info1;
    size_t              size0   = sizeof(info0);
    
    BlockZero(&info0, sizeof(info0));
    info0.version = kControlTabInfoVersionZero;
    
    Data<kControlTabInfoTag>::GetData(*this, size0, &info0, inTabIndex);
    
    info1.version       = kControlTabInfoVersionOne;
    info1.iconSuiteID   = info0.iconSuiteID;
    info1.name          = inName;
    
    Data<kControlTabInfoTag>::Set(*this, info1, inTabIndex);
#endif
}

// ------------------------------------------------------------------------------------------
void
TabbedView::GetTabImageContent(int inTabIndex, HIViewImageContentInfo& outContent) const
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    Data<kControlTabImageContentTag>::Get(*this, outContent, inTabIndex);
}

// ------------------------------------------------------------------------------------------
void
TabbedView::SetTabImageContent(int inTabIndex, const HIViewImageContentInfo& inContent)
{
    B_ASSERT((inTabIndex >= 1) && (inTabIndex <= GetMaximum()));
    
    Data<kControlTabImageContentTag>::Set(*this, inContent, inTabIndex);
}

// ------------------------------------------------------------------------------------------
void
TabbedView::ShowTab(int inTabIndex)
{
    if (inTabIndex != mLastTabIndex)
    {
        View*   selectedTab = NULL;
        
        mLastTabIndex = inTabIndex;
        
        for (size_t i = 0; i < mUserPanes.size(); i++)
        {
            View*   userPane    = mUserPanes[i];
            
            if (i + 1 == static_cast<size_t>(inTabIndex))
            {
                selectedTab = userPane;
            }
            else
            {
                userPane->Show(false);
                userPane->Enable(false);
            }
        }
        
        if (selectedTab != NULL)
        {
            selectedTab->Enable(true);
            selectedTab->Show(true);
        }
    }
}

// ------------------------------------------------------------------------------------------
bool
TabbedView::ControlValueFieldChanged(
    Event<kEventClassControl, kEventControlValueFieldChanged>&  /* event */)
{
    ShowTab(GetValue());
    
    return (false);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<TabbedView>   gRegisterTabbedView(kHITabbedViewClassID);
#endif


// ==========================================================================================
//  VisualSeparator

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
VisualSeparator::Instantiate(HIViewRef inViewRef)
{
    return (new VisualSeparator(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
VisualSeparator*
VisualSeparator::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory)      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateSeparatorControl(NULL, &qdRect, 
                                 &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<VisualSeparator>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
VisualSeparator::VisualSeparator(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<VisualSeparator>  gRegisterVisualSeparator(kHIVisualSeparatorClassID);
#endif


// ==========================================================================================
//  GroupBox

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
GroupBox::Instantiate(HIViewRef inViewRef)
{
    return (new GroupBox(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
GroupBox*
GroupBox::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const String&       inTitle, 
    bool                inPrimary)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateGroupBoxControl(NULL, &qdRect, 
                                inTitle.cf_ref(), inPrimary, 
                                &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<GroupBox>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
GroupBox::GroupBox(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
Rect
GroupBox::GetTitleRect() const
{
    Rect    r;
    
    Data<kControlGroupBoxTitleRectTag>::Get(*this, r);
    
    return r;
}

// ------------------------------------------------------------------------------------------
Rect
GroupBox::GetContentFrameRect() const
{
    Rect    r;
    
    Data<kControlGroupBoxFrameRectTag>::Get(*this, r);
    
    return r;
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<GroupBox> gRegisterGroupBox(kHIGroupBoxClassID);
#endif


// ==========================================================================================
//  CheckBoxGroup

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
CheckBoxGroup::Instantiate(HIViewRef inViewRef)
{
    return (new CheckBoxGroup(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
CheckBoxGroup*
CheckBoxGroup::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const String&       inTitle, 
    SInt32              inInitialValue,
    bool                inPrimary, 
    bool                inAutoToggle)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateCheckGroupBoxControl(NULL, &qdRect, 
                                     inTitle.cf_ref(), inInitialValue, inPrimary, 
                                     inAutoToggle, 
                                     &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<CheckBoxGroup>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
CheckBoxGroup::CheckBoxGroup(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
Rect
CheckBoxGroup::GetTitleRect() const
{
    Rect    r;
    
    Data<kControlGroupBoxTitleRectTag>::Get(*this, r);
    
    return r;
}

// ------------------------------------------------------------------------------------------
Rect
CheckBoxGroup::GetContentFrameRect() const
{
    Rect    r;
    
    Data<kControlGroupBoxFrameRectTag>::Get(*this, r);
    
    return r;
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<CheckBoxGroup>    gRegisterCheckBoxGroup(kHICheckBoxGroupClassID);
#endif


// ==========================================================================================
//  ImageWell

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ImageWell::Instantiate(HIViewRef inViewRef)
{
    return (new ImageWell(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
ImageWell*
ImageWell::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const HIViewImageContentInfo&   inContent)
{
    ::Rect                  qdRect  = { 0, 0, 100, 100 };
    HIViewImageContentInfo  content = inContent;
    HIViewRef               viewRef;
    OSStatus                err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateImageWellControl(NULL, &qdRect, 
                                 &content, 
                                 &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ImageWell>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ImageWell::ImageWell(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
void
ImageWell::GetContent(HIViewImageContentInfo& outContent) const
{
    OSStatus    err;
    
    err = GetImageWellContentInfo(*this, &outContent);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ImageWell::SetContent(const HIViewImageContentInfo& inContent)
{
    HIViewImageContentInfo  content = inContent;
    OSStatus                err;
    
    err = SetImageWellContentInfo(*this, &content);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
IconTransformType
ImageWell::GetTransform() const
{
    return (Data<kControlImageWellTransformTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ImageWell::SetTransform(IconTransformType inTransform)
{
    OSStatus    err;
    
    err = SetImageWellTransform(*this, inTransform);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
ImageWell::GetIsDragDestination() const
{
    return (Data<kControlImageWellIsDragDestinationTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ImageWell::SetIsDragDestination(bool inIsDestination)
{
    Data<kControlImageWellIsDragDestinationTag>::Set(*this, inIsDestination);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ImageWell>    gRegisterImageWell(kHIImageWellClassID);
#endif


// ==========================================================================================
//  PopupArrow

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
PopupArrow::Instantiate(HIViewRef inViewRef)
{
    return (new PopupArrow(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
PopupArrow*
PopupArrow::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    ControlPopupArrowOrientation    inOrientation, 
    ControlPopupArrowSize           inSize)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreatePopupArrowControl(GetSuperviewWindow(inSuperview), &qdRect, 
                                  inOrientation, inSize, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<PopupArrow>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
PopupArrow::PopupArrow(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<PopupArrow>   gRegisterPopupArrow(CFSTR("com.apple.hipopuparrow"));
#endif


// ==========================================================================================
//  PlacardView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
PlacardView::Instantiate(HIViewRef inViewRef)
{
    return (new PlacardView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
PlacardView*
PlacardView::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory)      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreatePlacardControl(GetSuperviewWindow(inSuperview), &qdRect, 
                               &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<PlacardView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
PlacardView::PlacardView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<PlacardView>  gRegisterPlacardView(kHIPlacardViewClassID);
#endif


// ==========================================================================================
//  ClockView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ClockView::Instantiate(HIViewRef inViewRef)
{
    return (new ClockView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
ClockView*
ClockView::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    ControlClockType    inType, 
    ControlClockFlags   inFlags)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateClockControl(NULL, &qdRect, 
                             inType, inFlags, 
                             &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ClockView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ClockView::ClockView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
void
ClockView::GetDate(LongDateRec& outDate) const
{
    Data<kControlClockLongDateTag>::Get(*this, outDate);
}

// ------------------------------------------------------------------------------------------
void
ClockView::SetDate(const LongDateRec& inDate)
{
    Data<kControlClockLongDateTag>::Set(*this, inDate);
}

// ------------------------------------------------------------------------------------------
bool
ClockView::GetAnimating() const
{
    return (Data<kControlClockAnimatingTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ClockView::SetAnimating(bool inAnimating)
{
    Data<kControlClockAnimatingTag>::Set(*this, inAnimating);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ClockView>    gRegisterClockView(kHIClockViewClassID);
#endif


// ==========================================================================================
//  UserPane

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
UserPane::Instantiate(HIViewRef inViewRef)
{
    return (new UserPane(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
UserPane*
UserPane::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    UInt32              inFeatures)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateUserPaneControl(NULL, &qdRect, 
                                inFeatures, 
                                &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<UserPane>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
UserPane::UserPane(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<UserPane> gRegisterUserPane(kHIUserPaneClassID);
#endif


// ==========================================================================================
//  StaticTextView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
StaticTextView::Instantiate(HIViewRef inViewRef)
{
    return (new StaticTextView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
StaticTextView*
StaticTextView::Create(
    const HIViewID&             inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                   inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                 inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                        inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*          inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const String&               inText,
    const ControlFontStyleRec&  inFontStyle)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateStaticTextControl(NULL, &qdRect, 
                                  inText.cf_ref(), &inFontStyle, 
                                  &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<StaticTextView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
StaticTextView::StaticTextView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
String
StaticTextView::GetText() const
{
    return (Data<kControlStaticTextCFStringTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
StaticTextView::SetText(const String& inTitle)
{
    Data<kControlStaticTextCFStringTag>::Set(*this, inTitle);
}

// ------------------------------------------------------------------------------------------
TruncCode
StaticTextView::GetTruncation() const
{
    return (Data<kControlStaticTextTruncTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
StaticTextView::SetTruncation(TruncCode inTruncation)
{
    Data<kControlStaticTextTruncTag>::Set(*this, inTruncation);
}

// ------------------------------------------------------------------------------------------
bool
StaticTextView::GetIsMultiline() const
{
    return (Data<kControlStaticTextIsMultilineTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
StaticTextView::SetIsMultiline(bool inIsMultiline)
{
    Data<kControlStaticTextIsMultilineTag>::Set(*this, inIsMultiline);
}

// ------------------------------------------------------------------------------------------
SInt16
StaticTextView::GetTextHeight() const
{
    return (Data<kControlStaticTextTextHeightTag>::Get(*this));
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<StaticTextView>   gRegisterStaticTextView(kHIStaticTextViewClassID);
#endif


// ==========================================================================================
//  PictureControl

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
PictureControl::Instantiate(HIViewRef inViewRef)
{
    return (new PictureControl(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
PictureControl*
PictureControl::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const HIViewImageContentInfo&   inContent, 
    bool                            inDontTrack)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreatePictureControl(GetSuperviewWindow(inSuperview), &qdRect, 
                               &inContent, inDontTrack, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<PictureControl>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
PictureControl::PictureControl(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
PicHandle
PictureControl::GetPicture() const
{
    return (Data<kControlPictureHandleTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
PictureControl::SetPicture(PicHandle inPicture)
{
    Data<kControlPictureHandleTag>::Set(*this, inPicture);
}

#ifndef DOXYGEN_SKIP
#ifdef kHIPictureViewClassID
B_VAR_EXPORT ToolboxViewRegistrar<PictureControl>   gRegisterPictureControl(kHIPictureViewClassID);
#else
B_VAR_EXPORT ToolboxViewRegistrar<PictureControl>   gRegisterPictureControl(CFSTR("com.apple.HIPictureView"));
#endif
#endif


// ==========================================================================================
//  IconView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
IconView::Instantiate(HIViewRef inViewRef)
{
    return (new IconView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
IconView*
IconView::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const HIViewImageContentInfo&   inContent, 
    bool                            inDontTrack)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateIconControl(GetSuperviewWindow(inSuperview), &qdRect, 
                            &inContent, inDontTrack, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<IconView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
IconView::IconView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
void
IconView::GetContent(HIViewImageContentInfo& outContent) const
{
    Data<kControlIconContentTag>::Get(*this, outContent);
}

// ------------------------------------------------------------------------------------------
void
IconView::SetContent(const HIViewImageContentInfo& inContent)
{
    Data<kControlIconContentTag>::Set(*this, inContent);
}

// ------------------------------------------------------------------------------------------
IconTransformType
IconView::GetTransform() const
{
    return (Data<kControlIconTransformTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
IconView::SetTransform(IconTransformType inTransform)
{
    Data<kControlIconTransformTag>::Set(*this, inTransform);
}

// ------------------------------------------------------------------------------------------
IconAlignmentType
IconView::GetAlignment() const
{
    return (Data<kControlIconAlignmentTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
IconView::SetAlignment(IconAlignmentType inAlignment)
{
    Data<kControlIconAlignmentTag>::Set(*this, inAlignment);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<IconView> gRegisterIconView(kHIIconViewClassID);
#endif


// ==========================================================================================
//  WindowHeaderView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
WindowHeaderView::Instantiate(HIViewRef inViewRef)
{
    return (new WindowHeaderView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
WindowHeaderView*
WindowHeaderView::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    bool                inIsListHeader)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateWindowHeaderControl(NULL, &qdRect, 
                                    inIsListHeader, 
                                    &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<WindowHeaderView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
WindowHeaderView::WindowHeaderView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
bool
WindowHeaderView::IsListHeader() const
{
    return (Data<kControlWindowHeaderIsListHeaderTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
WindowHeaderView::SetListHeader(bool inIsListHeader)
{
    Data<kControlWindowHeaderIsListHeaderTag>::Set(*this, inIsListHeader);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<WindowHeaderView> gRegisterWindowHeaderView(kHIWindowHeaderViewClassID);
#endif


// ==========================================================================================
//  PushButton

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
PushButton::Instantiate(HIViewRef inViewRef)
{
    return (new PushButton(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
PushButton*
PushButton::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    String&                         inTitle,
    const HIViewImageContentInfo&   inContent, 
    ControlPushButtonIconAlignment  inAlignment)
{
    HIViewImageContentInfo  content = inContent;
    ::Rect                  qdRect  = { 0, 0, 100, 100 };
    HIViewRef               viewRef;
    OSStatus                err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    if (content.contentType != kHIViewContentNone)
    {
        err = CreatePushButtonWithIconControl(NULL, &qdRect, 
                                              inTitle.cf_ref(), &content, inAlignment, 
                                              &viewRef);
    }
    else
    {
        err = CreatePushButtonControl(NULL, &qdRect, 
                                      inTitle.cf_ref(), 
                                      &viewRef);
    }
    
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<PushButton>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
PushButton::PushButton(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
bool
PushButton::IsDefault() const
{
    return (Data<kControlPushButtonDefaultTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
PushButton::SetDefault(bool inSetIt)
{
    Data<kControlPushButtonDefaultTag>::Set(*this, inSetIt);
}

// ------------------------------------------------------------------------------------------
bool
PushButton::IsCancel() const
{
    return (Data<kControlPushButtonCancelTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
PushButton::SetCancel(bool inSetIt)
{
    Data<kControlPushButtonCancelTag>::Set(*this, inSetIt);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<PushButton>       gRegisterPushButton(kHIPushButtonClassID);
#endif


// ==========================================================================================
//  GenericToggle

#pragma mark -

// ------------------------------------------------------------------------------------------
template <OSType KIND> View*
GenericToggle<KIND>::Instantiate(HIViewRef inViewRef)
{
    return (new GenericToggle<KIND>(inViewRef));
}

// ------------------------------------------------------------------------------------------
template <OSType KIND>
GenericToggle<KIND>::GenericToggle(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
template <OSType KIND> bool
GenericToggle<KIND>::AutoToggles() const
{
    UInt32      features;
    OSStatus    err;
    
    err = GetControlFeatures(*this, &features);
    B_THROW_IF_STATUS(err);
    
    return ((features & kControlAutoToggles) != 0);
}

// ------------------------------------------------------------------------------------------
template <OSType KIND> bool
GenericToggle<KIND>::SupportsMixed() const
{
    return (GetMaximum() == kControlCheckBoxMixedValue);
}

// ------------------------------------------------------------------------------------------
template <OSType KIND> void
GenericToggle<KIND>::SupportsMixed(bool inCanBeMixed)
{
    SetMaximum(inCanBeMixed ? kControlCheckBoxMixedValue : kControlCheckBoxCheckedValue);
}

// ------------------------------------------------------------------------------------------
template <OSType KIND> boost::logic::tribool
GenericToggle<KIND>::GetTriState() const
{
    switch (GetValue())
    {
    case kControlCheckBoxCheckedValue:
        return true;
        
    case kControlCheckBoxUncheckedValue:
        return false;
        
    default:
        return boost::logic::indeterminate;
    }
}

// ------------------------------------------------------------------------------------------
template <OSType KIND> void
GenericToggle<KIND>::SetTriState(boost::logic::tribool tb)
{
    if (tb)
        SetValue(kControlCheckBoxCheckedValue);
    else if (!tb)
        SetValue(kControlCheckBoxUncheckedValue);
    else
        SetValue(kControlCheckBoxMixedValue);
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
template <> GenericToggle<kControlKindCheckBox>*
GenericToggle<kControlKindCheckBox>::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    String&             inTitle,
    SInt32              inInitialValue, 
    bool                inAutoToggle)
{
    typedef GenericToggle<kControlKindCheckBox> ViewType;
    
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateCheckBoxControl(NULL, &qdRect, 
                                inTitle.cf_ref(), inInitialValue, inAutoToggle, 
                                &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ViewType>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
template <> GenericToggle<kControlKindRadioButton>*
GenericToggle<kControlKindRadioButton>::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    String&             inTitle,
    SInt32              inInitialValue, 
    bool                inAutoToggle)
{
    typedef GenericToggle<kControlKindRadioButton>  ViewType;
    
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateRadioButtonControl(NULL, &qdRect, 
                                   inTitle.cf_ref(), inInitialValue, inAutoToggle, 
                                   &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ViewType>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}


template class  GenericToggle<kControlKindCheckBox>;
template class  GenericToggle<kControlKindRadioButton>;

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar< GenericToggle<kControlKindCheckBox> >    gRegisterCheckBox(kHICheckBoxClassID);
B_VAR_EXPORT ToolboxViewRegistrar< GenericToggle<kControlKindRadioButton> > gRegisterRadioButton(kHIRadioButtonClassID);
#endif


// ==========================================================================================
//  ScrollBar

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ScrollBar::Instantiate(HIViewRef inViewRef)
{
    return (new ScrollBar(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
ScrollBar*
ScrollBar::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    SInt32              inValue, 
    SInt32              inMinimum, 
    SInt32              inMaximum, 
    SInt32              inViewSize, 
    bool                inLiveTracking, 
    ControlActionUPP    inLiveTrackingUPP)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateScrollBarControl(NULL, &qdRect, 
                                 inValue, inMinimum, inMaximum, inViewSize, 
                                 inLiveTracking, inLiveTrackingUPP, 
                                 &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ScrollBar>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ScrollBar::ScrollBar(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
SInt32
ScrollBar::GetViewSize() const
{
    return (GetControlViewSize(*this));
}

// ------------------------------------------------------------------------------------------
void
ScrollBar::SetViewSize(SInt32 inViewSize)
{
    SetControlViewSize(*this, inViewSize);
}

// ------------------------------------------------------------------------------------------
bool
ScrollBar::GetShowsArrows() const
{
    return (Data<kControlScrollBarShowsArrowsTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ScrollBar::SetShowsArrows(bool inShowsArrows)
{
    Data<kControlScrollBarShowsArrowsTag>::Set(*this, inShowsArrows);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ScrollBar>    gRegisterScrollBar(kHIScrollBarClassID);
#endif


// ==========================================================================================
//  PopupButton

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
PopupButton::Instantiate(HIViewRef inViewRef)
{
    return (new PopupButton(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
PopupButton*
PopupButton::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const String&       inTitle, 
    MenuRef             inMenuRef, 
    bool                inViewOwnsMenu, 
    bool                inVariableWidth, 
    SInt16              inTitleWidth, 
    SInt16              inJustification, 
    Style               inStyle)
{
    SInt16          menuID  = (inMenuRef != NULL) ? -12345 : 0;
    ::Rect          qdRect  = { 0, 0, 100, 100 };
    HIViewRef       viewRef;
    PopupButton*    viewObj;
    OSStatus        err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreatePopupButtonControl(GetSuperviewWindow(inSuperview), &qdRect, 
                                   inTitle.cf_ref(), menuID, inVariableWidth, 
                                   inTitleWidth, inJustification, inStyle, 
                                   &viewRef);
    B_THROW_IF_STATUS(err);
    
    viewObj = SetupTypedView<PopupButton>(
                    inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory);
    
    if (inMenuRef != NULL)
    {
        // Ensure the view is released if we get an exception.
        OSPtr<HIViewRef>    viewPtr(*viewObj, from_copy);
        
        viewObj->SetMenu(inMenuRef, inViewOwnsMenu);
        
        viewPtr.release();
    }
    
    return (viewObj);
}

// ------------------------------------------------------------------------------------------
PopupButton::PopupButton(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
MenuRef
PopupButton::GetMenu() const
{
    return (Data<kControlPopupButtonMenuRefTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
PopupButton::SetMenu(MenuRef inMenu, bool inViewOwnsMenu)
{
    if (inViewOwnsMenu)
        Data<kControlPopupButtonOwnedMenuRefTag>::Set(*this, inMenu);
    else
        Data<kControlPopupButtonMenuRefTag>::Set(*this, inMenu);
}

// ------------------------------------------------------------------------------------------
SInt16
PopupButton::GetExtraHeight() const
{
    return (Data<kControlPopupButtonExtraHeightTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
PopupButton::SetExtraHeight(SInt16 inExtraHeight)
{
    Data<kControlPopupButtonExtraHeightTag>::Set(*this, inExtraHeight);
}

// ------------------------------------------------------------------------------------------
bool
PopupButton::GetCheckCurrent() const
{
    return (Data<kControlPopupButtonCheckCurrentTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
PopupButton::SetCheckCurrent(bool inCheckCurrent)
{
    Data<kControlPopupButtonCheckCurrentTag>::Set(*this, inCheckCurrent);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<PopupButton>  gRegisterPopupButton(kHIPopupButtonClassID);
#endif


// ==========================================================================================
//  RadioGroup

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
RadioGroup::Instantiate(HIViewRef inViewRef)
{
    return (new RadioGroup(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
RadioGroup*
RadioGroup::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory)      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateRadioGroupControl(NULL, &qdRect, 
                                  &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<RadioGroup>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
RadioGroup*
RadioGroup::CreateAndEmbedSubviews(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    UInt32              inCommandID,
    HIViewRef           inSuperview,    //!< The new view's superview.
    const std::vector<HIViewID> inSubviewIDs,
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory)      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
    B_ASSERT(inSuperview != NULL);
    B_ASSERT(!inSubviewIDs.empty());
    
    std::vector<HIViewRef>  views;
    
    std::transform(inSubviewIDs.begin(), inSubviewIDs.end(), std::back_inserter(views), 
                   boost::bind(FindEmbeddedView, inSuperview, _1));
    
    Rect    groupFrame = CGRectZero;
    
    std::for_each(views.begin(), views.end(), 
                  boost::bind(MergeEmbeddedViewFrame, _1, boost::ref(groupFrame)));
    
    RadioGroup* group = Create(inViewID, inSuperview, &groupFrame, inFromNib, inFactory);
    OSStatus    err;
    
    err = HIViewSetZOrder(group->GetViewRef(), kHIViewZOrderBelow, views[0]);
    B_THROW_IF_STATUS(err);
    
    if (inCommandID != 0)
    {
        group->SetCommandID(inCommandID);
    }
    
    std::for_each(views.begin(), views.end(), 
                  boost::bind(MoveEmbeddedView, _1, groupFrame.origin, group));
    
    return group;
}

// ------------------------------------------------------------------------------------------
HIViewRef
RadioGroup::FindEmbeddedView(
    HIViewRef   inSuperview,
    HIViewID    inViewID)
{
    return ViewUtils::FindSubview(inSuperview, inViewID);
}

// ------------------------------------------------------------------------------------------
void
RadioGroup::MergeEmbeddedViewFrame(
    HIViewRef   inSubview,
    B::Rect&    ioGroupFrame)
{
    B::Rect     frame;
    OSStatus    err;
    
    err = HIViewGetFrame(inSubview, &frame);
    B_THROW_IF_STATUS(err);
    
    if (ioGroupFrame.empty())
        ioGroupFrame = frame;
    else
        ioGroupFrame |= frame;
}

// ------------------------------------------------------------------------------------------
void
RadioGroup::MoveEmbeddedView(
    HIViewRef   inSubview,
    CGPoint     inGroupOrigin,
    RadioGroup* inGroup)
{
    OSStatus    err;
    
    err = HIViewRemoveFromSuperview(inSubview);
    B_THROW_IF_STATUS(err);
    
    err = HIViewAddSubview(inGroup->GetViewRef(), inSubview);
    B_THROW_IF_STATUS(err);
    
    err = HIViewMoveBy(inSubview, -inGroupOrigin.x, -inGroupOrigin.y);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
RadioGroup::RadioGroup(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<RadioGroup>   gRegisterRadioGroup(kHIRadioGroupClassID);
#endif


// ==========================================================================================
//  DisclosureButton

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
DisclosureButton::Instantiate(HIViewRef inViewRef)
{
    return (new DisclosureButton(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
DisclosureButton*
DisclosureButton::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    SInt32              inValue, 
    bool                inAutoToggle)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateDisclosureButtonControl(NULL, &qdRect, 
                                        inValue, inAutoToggle, 
                                        &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<DisclosureButton>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
DisclosureButton::DisclosureButton(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<DisclosureButton> gRegisterDisclosureButton(kHIDisclosureButtonClassID);
#endif


// ==========================================================================================
//  RoundButton

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
RoundButton::Instantiate(HIViewRef inViewRef)
{
    return (new RoundButton(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
RoundButton*
RoundButton::Create(
    const HIViewID&                 inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*              inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    ControlRoundButtonSize          inSize, 
    const HIViewImageContentInfo&   inContent)
{
    HIViewImageContentInfo  content = inContent;
    ::Rect                  qdRect  = { 0, 0, 100, 100 };
    HIViewRef               viewRef;
    OSStatus                err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateRoundButtonControl(GetSuperviewWindow(inSuperview), 
                                   &qdRect, 
                                   inSize, &content, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<RoundButton>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
RoundButton::RoundButton(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
void
RoundButton::GetContent(HIViewImageContentInfo& outContent) const
{
    Data<kControlRoundButtonContentTag>::Get(*this, outContent);
}

// ------------------------------------------------------------------------------------------
void
RoundButton::SetContent(const HIViewImageContentInfo& inContent)
{
    Data<kControlRoundButtonContentTag>::Set(*this, inContent);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<RoundButton>  gRegisterRoundButton(kHIRoundButtonClassID);
#endif


// ==========================================================================================
//  TextFieldPart

#pragma mark -

// ------------------------------------------------------------------------------------------
TextFieldPart::TextFieldPart(HIViewRef inViewRef)
    : mViewRef(inViewRef), mEventHandler(inViewRef)
{
    InitEventHandler();
}

// ------------------------------------------------------------------------------------------
TextFieldPart::~TextFieldPart()
{
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::InitEventHandler()
{
    mEventHandler.Add(this, &TextFieldPart::TextAccepted);
    mEventHandler.Add(this, &TextFieldPart::TextShouldChangeInRange);
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
String
TextFieldPart::GetText() const
{
    String  text;
    
    ViewDataTrait<CFStringRef>::Get(mViewRef, kHIViewEntireView, 
                                    kControlEditTextCFStringTag, text);
    
    return (text);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SetText(const String& inText)
{
    ViewDataTrait<CFStringRef>::Set(mViewRef, kHIViewEntireView, 
                                    kControlEditTextCFStringTag, inText);
}

// ------------------------------------------------------------------------------------------
size_t
TextFieldPart::GetTextSize() const
{
    UInt32  size;
    
    ViewDataTrait<UInt32>::Get(mViewRef, kHIViewEntireView, 
                               kControlEditTextCharCount, size);
    
    return (size);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::GetSelection(size_t& outSelStart, size_t& outSelEnd) const
{
    ControlEditTextSelectionRec selection;
    
    ViewDataTrait<ControlEditTextSelectionRec>::Get(mViewRef, kHIViewEntireView, 
                                                    kControlEditTextSelectionTag, 
                                                    selection);
    
    outSelStart = selection.selStart;
    outSelEnd   = selection.selEnd;
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SetSelection(size_t inSelStart, size_t inSelEnd)
{
    B_ASSERT(inSelStart < INT16_MAX);
    B_ASSERT(inSelEnd < INT16_MAX);
    
    ControlEditTextSelectionRec selection   = { inSelStart, inSelEnd };
    
    ViewDataTrait<ControlEditTextSelectionRec>::Set(mViewRef, kHIViewEntireView, 
                                                    kControlEditTextSelectionTag, 
                                                    selection);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SelectAll()
{
    SetSelection(0, GetTextSize());
}

// ------------------------------------------------------------------------------------------
ControlKeyScriptBehavior
TextFieldPart::GetKeyScriptBehavior() const
{
    ControlKeyScriptBehavior    behavior;
    
    ViewDataTrait<ControlKeyScriptBehavior>::Get(mViewRef, kHIViewEntireView, 
                                                 kControlEditTextKeyScriptBehaviorTag, 
                                                 behavior);
    
    return (behavior);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SetKeyScriptBehavior(ControlKeyScriptBehavior inKeyScriptBehavior)
{
    ViewDataTrait<ControlKeyScriptBehavior>::Set(mViewRef, kHIViewEntireView, 
                                                 kControlEditTextKeyScriptBehaviorTag, 
                                                 inKeyScriptBehavior);
}

// ------------------------------------------------------------------------------------------
bool
TextFieldPart::GetFixedText() const
{
    bool    fixed;
    
    ViewDataTrait<Boolean>::Get(mViewRef, kHIViewEntireView, 
                                kControlEditTextFixedTextTag, fixed);
    
    return (fixed);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SetFixedText(bool inFixedText)
{
    ViewDataTrait<Boolean>::Set(mViewRef, kHIViewEntireView, 
                                kControlEditTextFixedTextTag, inFixedText);
}

// ------------------------------------------------------------------------------------------
ControlEditTextValidationUPP
TextFieldPart::GetValidationProc() const
{
    ControlEditTextValidationUPP    upp;
    
    ViewDataTrait<ControlEditTextValidationUPP>::Get(mViewRef, kHIViewEntireView, 
                                                     kControlEditTextValidationProcTag, 
                                                     upp);
    
    return (upp);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SetValidationProc(ControlEditTextValidationUPP inValidationProc)
{
    ViewDataTrait<ControlEditTextValidationUPP>::Set(mViewRef, kHIViewEntireView, 
                                                     kControlEditTextValidationProcTag, 
                                                     inValidationProc);
}

// ------------------------------------------------------------------------------------------
EditUnicodePostUpdateUPP
TextFieldPart::GetPostUpdateProc() const
{
    EditUnicodePostUpdateUPP    upp;
    
    ViewDataTrait<EditUnicodePostUpdateUPP>::Get(mViewRef, kHIViewEntireView, 
                                                 kControlEditUnicodeTextPostUpdateProcTag, 
                                                 upp);
    
    return (upp);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::SetPostUpdateProc(EditUnicodePostUpdateUPP inPostUpdateProc)
{
    ViewDataTrait<EditUnicodePostUpdateUPP>::Set(mViewRef, kHIViewEntireView, 
                                                 kControlEditUnicodeTextPostUpdateProcTag, 
                                                 inPostUpdateProc);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::ConnectTextChanged(TextChangedFunction inFunction)
{
    mTextChangedSignal.connect(inFunction);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::ConnectValidationFailed(ValidationFailedFunction inFunction)
{
    mValidationFailedSignal.connect(inFunction);
}

// ------------------------------------------------------------------------------------------
bool
TextFieldPart::Validate()
{
    try
    {
        mValidator.Validate(*this);
        
        return true;
    }
    catch (const TextValidator::Exception& ex)
    {
        mValidationFailedSignal(*this, false, ex);

        return false;
    }
}

// ------------------------------------------------------------------------------------------
bool
TextFieldPart::TextChanging(
    const CFRange&  inSelection,
    const String&   inNewText, 
    String&         outReplacementText,
    bool&           outCancel)
{
    bool    handled = false;
    
    try
    {
        TextValidator::Action   action;
        
        action = mValidator.Filter(*this, inNewText, inSelection, outReplacementText);
        
        switch (action)
        {
        case TextValidator::Accept:
            break;
            
        case TextValidator::Replace:
            handled = true;
            break;
            
        case TextValidator::Delete:
            outReplacementText.clear();
            handled = true;
            break;
        }
    }
    catch (const TextValidator::Exception& ex)
    {
        mValidationFailedSignal(*this, true, ex);

        handled = outCancel = true;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
void
TextFieldPart::TextChanged()
{
    mTextChangedSignal(*this);
}

// ------------------------------------------------------------------------------------------
bool
TextFieldPart::TextAccepted(
    Event<kEventClassTextField, kEventTextAccepted>&    /* event */)
{
    TextChanged();
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
TextFieldPart::TextShouldChangeInRange(
    Event<kEventClassTextField, kEventTextShouldChangeInRange>& event)
{
    String  replacement;
    bool    handled = TextChanging(event.mSelection, event.mText, replacement, event.mCancel);
    
    if (handled)
        event.mText = replacement;
    
    return (handled);
}


// ==========================================================================================
//  BeepOnValidationFailed

#pragma mark -

// ------------------------------------------------------------------------------------------
void
BeepOnValidationFailed::operator () (
    TextFieldPart&                  /* inPart */,
    bool                            inFiltering,
    const TextValidator::Exception& /* ex */) const
{
    if (inFiltering)
    {
        SysBeep(30);
    }
}


// ==========================================================================================
//  TextField

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
TextField::Instantiate(HIViewRef inViewRef)
{
    return (new TextField(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
TextField*
TextField::Create(
    const HIViewID&             inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef                   inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*                 inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                        inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*          inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    const String&               inText,
    bool                        inIsPassword,
    const ControlFontStyleRec&  inFontStyle)
{
    ::Rect      qdRect  = { 0, 0, 100, 100 };
    HIViewRef   viewRef;
    OSStatus    err;
    
    if (inFrame != NULL)
        qdRect = *inFrame;
    
    err = CreateEditUnicodeTextControl(NULL, &qdRect, 
                                       inText.cf_ref(), inIsPassword, &inFontStyle, 
                                       &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<TextField>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
TextField::TextField(HIViewRef inViewRef)
    : PredefinedView(inViewRef), TextFieldPart(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
String
TextField::GetText() const
{
    return (TextFieldPart::GetText());
}

// ------------------------------------------------------------------------------------------
void
TextField::SetText(const String& inText)
{
    TextFieldPart::SetText(inText);
}

// ------------------------------------------------------------------------------------------
String
TextField::GetPasswordText() const
{
    return (Data<kControlEditTextPasswordCFStringTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
TextField::SetPasswordText(const String& inPasswordText)
{
    Data<kControlEditTextPasswordCFStringTag>::Set(*this, inPasswordText);
}

// ------------------------------------------------------------------------------------------
bool
TextField::GetLocked() const
{
    return (Data<kControlEditTextLockedTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
TextField::SetLocked(bool inLocked)
{
    Data<kControlEditTextLockedTag>::Set(*this, inLocked);
}

// ------------------------------------------------------------------------------------------
bool
TextField::GetSingleLine() const
{
    return (Data<kControlEditTextSingleLineTag>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
TextField::SetSingleLine(bool inSingleLine)
{
    Data<kControlEditTextSingleLineTag>::Set(*this, inSingleLine);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<TextField>    gRegisterTextField(kHITextFieldClassID);
#endif


// ==========================================================================================
//  ScrollView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ScrollView::Instantiate(HIViewRef inViewRef)
{
    return (new ScrollView(inViewRef));
}

// ------------------------------------------------------------------------------------------
ScrollView*
ScrollView::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    OptionBits          inOptions)
{
    HIViewRef   viewRef;
    OSStatus    err;
    
    err = HIScrollViewCreate(inOptions, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ScrollView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ScrollView::ScrollView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
bool
ScrollView::GetAutoHide() const
{
    return (HIScrollViewGetScrollBarAutoHide(*this));
}

// ------------------------------------------------------------------------------------------
void
ScrollView::SetAutoHide(bool autoHide)
{
    OSStatus    err;
    
    err = HIScrollViewSetScrollBarAutoHide(*this, autoHide);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
float
ScrollView::GetLeftInset() const
{
    return (Data<kControlScrollViewHorizontalInset>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ScrollView::SetLeftInset(float inset)
{
    Data<kControlScrollViewHorizontalInset>::Set(*this, inset);
}

// ------------------------------------------------------------------------------------------
float
ScrollView::GetTopInset() const
{
    return (Data<kControlScrollViewVerticalInset>::Get(*this));
}

// ------------------------------------------------------------------------------------------
void
ScrollView::SetTopInset(float inset)
{
    Data<kControlScrollViewVerticalInset>::Set(*this, inset);
}

// ------------------------------------------------------------------------------------------
bool
ScrollView::CanNavigate(HIScrollViewAction inAction) const
{
    return (HIScrollViewCanNavigate(*this, inAction));
}

// ------------------------------------------------------------------------------------------
void
ScrollView::Navigate(HIScrollViewAction inAction)
{
    OSStatus    err;
    
    err = HIScrollViewNavigate(*this, inAction);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ScrollView::ReplaceContent(HIViewRef inNewView)
{
    HIViewRef   subView;
    OSStatus    err;
    
    while ((subView = HIViewGetFirstSubview(*this)) != NULL)
    {
        err = HIViewRemoveFromSuperview(subView);
        B_THROW_IF_STATUS(err);
    }
    
    err = HIViewAddSubview(*this, inNewView);
    B_THROW_IF_STATUS(err);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ScrollView>   gRegisterScrollView(kHIScrollViewClassID);
#endif


// ==========================================================================================
//  ImageView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
ImageView::Instantiate(HIViewRef inViewRef)
{
    return (new ImageView(inViewRef));
}

// ------------------------------------------------------------------------------------------
ImageView*
ImageView::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    OSPtr<CGImageRef>   inImage)
{
    HIViewRef   viewRef;
    OSStatus    err;
    
    err = HIImageViewCreate(inImage, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<ImageView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
ImageView::ImageView(HIViewRef inViewRef)
    : PredefinedView(inViewRef)
{
}

// ------------------------------------------------------------------------------------------
bool
ImageView::GetOpaque() const
{
    return (HIImageViewIsOpaque(*this));
}

// ------------------------------------------------------------------------------------------
void
ImageView::SetOpaque(bool inOpaque)
{
    OSStatus    err;
    
    err = HIImageViewSetOpaque(*this, inOpaque);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
float
ImageView::GetAlpha() const
{
    return (HIImageViewGetAlpha(*this));
}

// ------------------------------------------------------------------------------------------
void
ImageView::SetAlpha(float inAlpha)
{
    OSStatus    err;
    
    err = HIImageViewSetAlpha(*this, inAlpha);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
ImageView::GetScaleToFit() const
{
    return (HIImageViewGetScaleToFit(*this));
}

// ------------------------------------------------------------------------------------------
void
ImageView::SetScaleToFit(bool inScaleToFit)
{
    OSStatus    err;
    
    err = HIImageViewSetScaleToFit(*this, inScaleToFit);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>
ImageView::GetImage() const
{
    CGImageRef          imageRef    = HIImageViewCopyImage(*this);
    OSPtr<CGImageRef>   imagePtr;
    
    if (imageRef != NULL)
        imagePtr.reset(imageRef, from_copy);
    
    return (imagePtr);
}

// ------------------------------------------------------------------------------------------
void
ImageView::SetImage(OSPtr<CGImageRef> inImage)
{
    OSStatus    err;
    
    err = HIImageViewSetImage(*this, inImage);
    B_THROW_IF_STATUS(err);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<ImageView>    gRegisterImageView(kHIImageViewClassID);
#endif


// ==========================================================================================
//  TextView

#pragma mark -

// ------------------------------------------------------------------------------------------
View*
TextView::Instantiate(HIViewRef inViewRef)
{
    return (new TextView(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @note   The view is initially invisible.
*/
TextView*
TextView::Create(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const ViewFactory*  inFactory,      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
    OptionBits          inOptions,
    TXNFrameOptions     inFrameOptions)
{
    HIViewRef   viewRef;
    OSStatus    err;
    
    err = HITextViewCreate(inFrame, inOptions, inFrameOptions, &viewRef);
    B_THROW_IF_STATUS(err);
    
    return (SetupTypedView<TextView>(
                inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory));
}

// ------------------------------------------------------------------------------------------
TextView::TextView(HIViewRef inViewRef)
    : PredefinedView(inViewRef), TextFieldPart(inViewRef), 
      mTextObj(HITextViewGetTXNObject(inViewRef))
{
}

// ------------------------------------------------------------------------------------------
String
TextView::GetText() const
{
    return (TextFieldPart::GetText());
}

// ------------------------------------------------------------------------------------------
void
TextView::SetText(const String& inText)
{
    TextFieldPart::SetText(inText);
}

// ------------------------------------------------------------------------------------------
Graphics::Color
TextView::GetBackgroundColor() const
{
    CGColorRef  colorRef;
    OSStatus    err;
    
    err = HITextViewCopyBackgroundColor(*this, &colorRef);
    B_THROW_IF_STATUS(err);
    
    if (colorRef != NULL)
        return Graphics::Color(OSPtr<CGColorRef>(colorRef, from_copy));
    else
        return Graphics::Color::GetDeviceRGBTransparent();
}

// ------------------------------------------------------------------------------------------
void
TextView::GetSelection(size_t& outSelStart, size_t& outSelEnd) const
{
    TXNOffset   startOffset, endOffset;
    
    TXNGetSelection(mTextObj, &startOffset, &endOffset);
    
    outSelStart = startOffset;
    outSelEnd   = endOffset;
}

// ------------------------------------------------------------------------------------------
void
TextView::SetBackgroundColor(const Graphics::Color& inColor)
{
    CGColorRef  colorRef;
    OSStatus    err;
    
    if (inColor.alpha() < 1.0f)
        colorRef = inColor;
    else
        colorRef = NULL;
        
    err = HITextViewSetBackgroundColor(*this, colorRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
TextView::SetSelection(size_t inSelStart, size_t inSelEnd)
{
    OSStatus    err;
    
    err = TXNSetSelection(mTextObj, inSelStart, inSelEnd);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
TextView::SelectAll()
{
    TXNSelectAll(mTextObj);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<TextView> gRegisterTextView(kHITextViewClassID);
#endif

}   // namespace B
