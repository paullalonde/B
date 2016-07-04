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
#include "BView.h"

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BCustomView.h"
#include "BEvent.h"
#include "BNib.h"
#include "BViewData.h"
#include "BViewFactory.h"
#include "BViewProperty.h"


namespace B {


// ==========================================================================================
//  View

namespace {

    typedef B::ViewProperty<B::kEventClassB, 'View', B::View*>  ViewObjectProperty;
    
    /*! @brief  Functor that recursively instantiates Views.
    */
    class Instantiate : public std::unary_function<void, HIViewRef>
    {
    public:
                Instantiate(const B::ViewFactory& factory)
                    : mFactory(factory) {}
                Instantiate(const Instantiate& orig)
                    : mFactory(orig.mFactory) {}
        
        void    operator () (HIViewRef viewRef) const;
        
    private:
        const B::ViewFactory&   mFactory;
    };
    
    void
    Instantiate::operator () (HIViewRef viewRef) const
    {
        B::View*    view;
        
        view = mFactory.Instantiate(viewRef);
        
        std::for_each(view->begin(), view->end(), Instantiate(*this));
    }
    
    class Awakener : public std::unary_function<void, HIViewRef>
    {
    public:
                Awakener(B::Nib* nib)
                    : mNib(nib) {}
                Awakener(const Awakener& orig)
                    : mNib(orig.mNib) {}
        
        void    operator () (HIViewRef viewRef) const
                    {
                        B::View::AwakenViewAndChildren(viewRef, mNib);
                    }
                
    private:
        B::Nib* mNib;
    };
    
}   // anonymous namespace


// ------------------------------------------------------------------------------------------
View::View(HIViewRef inViewRef)
    : mAwakened(false), mViewRef(inViewRef)
{
    B_ASSERT(inViewRef != NULL);
    
    // If this asserts, we're are probably trying to instantiate a second View object 
    // for inViewRef.
    B_ASSERT(!ViewObjectProperty::Exists(inViewRef));

    View*   view    = this;
    
    ViewObjectProperty::Set(inViewRef, view);
}

// ------------------------------------------------------------------------------------------
View::~View()
{
}

// ------------------------------------------------------------------------------------------
void
View::InstantiateSubviews(
    HIViewRef           inSuperView,    //!< The super view to iterate over.
    const ViewFactory&  inFactory)      //!< The factory to use for instantiation.
{
    std::for_each(ViewUtils::begin(inSuperView), ViewUtils::end(inSuperView), 
                  Instantiate(inFactory));
}

// ------------------------------------------------------------------------------------------
/*! Performs a depth-first traversal of the view tree rooted in @a inSuperView, telling 
    each one in turn to awaken.
*/
void
View::AwakenViewAndChildren(
    HIViewRef   inView,         //!< The view to awaken.
    Nib*        inFromNib)      //!< The nib from which to load other resources.  May be @c NULL.
{
    // Start by awakening the children.
    
    std::for_each(ViewUtils::begin(inView), ViewUtils::end(inView), Awakener(inFromNib));
    
    // Awaken any custom view implementation.
    
    CustomView::AwakenView(inView, inFromNib);
    
    // Awaken the (client-side) view.
    
    View*   view    = GetViewFromRef(inView, std::nothrow);
    
    if ((view != NULL) && !view->mAwakened)
    {
        view->mAwakened = true;
        view->Awaken(inFromNib);
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
    
    @note   If the view was loaded from a nib, then @a inNib won't be @c NULL .  If the 
            view was created programatically, it may be @c NULL.  Also, @a inNib is only 
            valid for the duration of this function invocation.  Derived classes should 
            @b not save a reference to it.
*/
void
View::Awaken(
    Nib*    /* inFromNib */)    //!< The nib from which to load other resources.  May be @c NULL.
{
}

// ------------------------------------------------------------------------------------------
HIViewKind
View::GetKind() const
{
    HIViewKind  kind;
    OSStatus    err;
    
    err = HIViewGetKind(mViewRef, &kind);
    B_THROW_IF_STATUS(err);
    
    return (kind);
}

// ------------------------------------------------------------------------------------------
HIViewID
View::GetViewID() const
{
    HIViewID    viewID;
    OSStatus    err;
    
    err = HIViewGetID(mViewRef, &viewID);
    B_THROW_IF_STATUS(err);
    
    return (viewID);
}

// ------------------------------------------------------------------------------------------
void
View::SetViewID(const HIViewID& inViewID)
{
    OSStatus    err;
    
    err = HIViewSetID(mViewRef, inViewID);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
UInt32
View::GetCommandID() const
{
    UInt32      commandID;
    OSStatus    err;
    
    err = HIViewGetCommandID(mViewRef, &commandID);
    B_THROW_IF_STATUS(err);
    
    return (commandID);
}

// ------------------------------------------------------------------------------------------
void
View::SetCommandID(UInt32 inCommandID)
{
    OSStatus    err;
    
    err = HIViewSetCommandID(mViewRef, inCommandID);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::SetID(SInt32 inID)
{
    HIViewID    viewID  = GetViewID();
    
    viewID.id = inID;
    
    SetViewID(viewID);
}

// ------------------------------------------------------------------------------------------
Rect
View::GetBounds() const
{
    Rect        r;
    OSStatus    err;
    
    err = HIViewGetBounds(mViewRef, &r);
    B_THROW_IF_STATUS(err);
    
    return r;
}

// ------------------------------------------------------------------------------------------
Rect
View::GetFrame() const
{
    Rect        r;
    OSStatus    err;
    
    err = HIViewGetFrame(mViewRef, &r);
    B_THROW_IF_STATUS(err);
    
    return r;
}

// ------------------------------------------------------------------------------------------
std::pair<Rect, float>
View::GetBestBounds() const
{
    std::pair<Rect, float>  result(CGRectNull, 0.0f);
    OSStatus                err;
    
    err = HIViewGetOptimalBounds(mViewRef, &result.first, &result.second);
    B_THROW_IF_STATUS(err);
    
    return result;
}

// ------------------------------------------------------------------------------------------
int
View::GetValue() const
{
    return (HIViewGetValue(mViewRef));
}

// ------------------------------------------------------------------------------------------
int
View::GetMinimum() const
{
    return (HIViewGetMinimum(mViewRef));
}

// ------------------------------------------------------------------------------------------
int
View::GetMaximum() const
{
    return (HIViewGetMaximum(mViewRef));
}

// ------------------------------------------------------------------------------------------
String
View::GetText() const
{
    CFStringRef strRef  = HIViewCopyText(mViewRef);
    String      text;
    
    if (strRef != NULL)
    {
        text.assign(strRef, from_copy);
    }
    
    return (text);
}

// ------------------------------------------------------------------------------------------
bool
View::IsFocused() const
{
    return (GetFocusPart() != kHIViewFocusNoPart);
}

// ------------------------------------------------------------------------------------------
HIViewPartCode
View::GetFocusPart() const
{
    HIViewPartCode  part;
    OSStatus        err;
    
    err = HIViewGetFocusPart(mViewRef, &part);
    B_THROW_IF_STATUS(err);
    
    return part;
}

// ------------------------------------------------------------------------------------------
HIViewPartCode
View::GetHighlightPart() const
{
    return (GetControlHilite(mViewRef));
}

// ------------------------------------------------------------------------------------------
bool
View::IsHighlighted() const
{
    return (IsControlHilited(mViewRef));
}

// ------------------------------------------------------------------------------------------
SInt32
View::GetSize() const
{
    return (HIViewGetViewSize(mViewRef));
}

// ------------------------------------------------------------------------------------------
void
View::SetSize(SInt32 inSize)
{
    OSStatus    err;
    
    err = HIViewSetViewSize(mViewRef, inSize);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! @note   Not all controls support getting this data tag.
*/
ControlFontStyleRec
View::GetFontStyle() const
{
    ControlFontStyleRec fontStyle;
    
    ViewDataTrait<ControlFontStyleRec>::Get(mViewRef, kHIViewEntireView, 
                                            kControlFontStyleTag, fontStyle);
    
    return fontStyle;
}

// ------------------------------------------------------------------------------------------
/*! @note   Not all controls support setting this data tag.
*/
void
View::SetFontStyle(const ControlFontStyleRec& inFontStyle)
{
    OSStatus    err;
    
    err = SetControlFontStyle(mViewRef, &inFontStyle);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
ControlActionUPP
View::GetActionProc() const
{
    return (GetControlAction(mViewRef));
}

// ------------------------------------------------------------------------------------------
void
View::SetActionProc(ControlActionUPP inActionProc)
{
    SetControlAction(mViewRef, inActionProc);
}

// ------------------------------------------------------------------------------------------
bool
View::IsActive() const
{
    return (HIViewIsActive(mViewRef, NULL));
}

// ------------------------------------------------------------------------------------------
bool
View::IsViewActive() const
{
    Boolean active;
    
    HIViewIsActive(mViewRef, &active);
    
    return active;
}

// ------------------------------------------------------------------------------------------
bool
View::IsEnabled() const
{
    return (HIViewIsEnabled(mViewRef, NULL));
}

// ------------------------------------------------------------------------------------------
bool
View::IsViewEnabled() const
{
    Boolean enabled;
    
    HIViewIsEnabled(mViewRef, &enabled);
    
    return enabled;
}

// ------------------------------------------------------------------------------------------
bool
View::IsVisible() const
{
    return (HIViewIsVisible(mViewRef));
}

// ------------------------------------------------------------------------------------------
bool
View::IsViewVisible() const
{
    return (HIViewIsLatentlyVisible(mViewRef));
}

// ------------------------------------------------------------------------------------------
bool
View::IsCommandSentToUserFocus() const
{
    OptionBits  attributes;
    OSStatus    err;
    
    err = HIViewGetAttributes(mViewRef, &attributes);
    B_THROW_IF_STATUS(err);
    
    return ((attributes & kHIViewSendCommandToUserFocus ) != 0);
}

// ------------------------------------------------------------------------------------------
WindowRef
View::GetOwner() const
{
    return (HIViewGetWindow(mViewRef));
}

// ------------------------------------------------------------------------------------------
void
View::DebugPrint() const
{
    HIObjectPrintDebugInfo(reinterpret_cast<HIObjectRef>(mViewRef));
}

// ------------------------------------------------------------------------------------------
void
View::SetFrame(const Rect& inNewFrame)
{
    OSStatus    err;
    
    err = HIViewSetFrame(mViewRef, &inNewFrame);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::MoveFrameBy(float deltaX, float deltaY)
{
    OSStatus    err;
    
    err = HIViewMoveBy(mViewRef, deltaX, deltaY);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::MoveFrameTo(float x, float y)
{
    OSStatus    err;
    
    err = HIViewPlaceInSuperviewAt(mViewRef, x, y);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::ResizeFrameBy(float deltaWidth, float deltaHeight)
{
    B::Rect frame   = GetFrame();
    
    frame.size.width    += deltaWidth;
    frame.size.height   += deltaHeight;
    
    SetFrame(frame);
}

// ------------------------------------------------------------------------------------------
void
View::ResizeFrameTo(float width, float height)
{
    B::Rect frame   = GetFrame();
    
    frame.size.width    = width;
    frame.size.height   = height;
    
    SetFrame(frame);
}

// ------------------------------------------------------------------------------------------
void
View::SetValue(int inValue)
{
    HIViewSetValue(mViewRef, inValue);
}

// ------------------------------------------------------------------------------------------
void
View::SetMinimum(int inMininum)
{
    HIViewSetMinimum(mViewRef, inMininum);
}

// ------------------------------------------------------------------------------------------
void
View::SetMaximum(int inMaximum)
{
    HIViewSetMaximum(mViewRef, inMaximum);
}

// ------------------------------------------------------------------------------------------
void
View::SetText(const String& inText)
{
    OSStatus    err;
    
    err = HIViewSetText(mViewRef, inText.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::SetFocus(HIViewPartCode part)
{
    OSStatus    err;
    
    err = SetKeyboardFocus(GetOwner(), mViewRef, part);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::SetHighlight(HIViewPartCode part)
{
    HiliteControl(mViewRef, part);
}

// ------------------------------------------------------------------------------------------
void
View::Activate(bool inActivate)
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
View::Enable(bool inEnable)
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
View::Show(bool inShow)
{
    OSStatus    err;
    
    err = HIViewSetVisible(mViewRef, inShow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::SendCommandToUserFocus(bool inSendIt)
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
View::AddSubview(View* subview)
{
    B_ASSERT(subview != NULL);
    
    OSStatus    err;
    
    err = HIViewAddSubview(*this, *subview);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::RemoveFromSuperview()
{
    OSStatus    err;
    
    err = HIViewRemoveFromSuperview(*this);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::SetLeftBinding(HIBindingKind inBinding)
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.binding.left.toView  = NULL;
    layout.binding.left.kind    = inBinding;
    layout.binding.left.offset  = 0.0f;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetTopBinding(HIBindingKind inBinding)
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.binding.top.toView   = NULL;
    layout.binding.top.kind     = inBinding;
    layout.binding.top.offset   = 0.0f;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetRightBinding(HIBindingKind inBinding)
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.binding.right.toView = NULL;
    layout.binding.right.kind   = inBinding;
    layout.binding.right.offset = 0.0f;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetBottomBinding(HIBindingKind inBinding)
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.binding.bottom.toView    = NULL;
    layout.binding.bottom.kind      = inBinding;
    layout.binding.bottom.offset    = 0.0f;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetBindings(
    HIBindingKind   inLeftBinding, 
    HIBindingKind   inTopBinding, 
    HIBindingKind   inRightBinding, 
    HIBindingKind   inBottomBinding)
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.binding.left.toView      = NULL;
    layout.binding.left.kind        = inLeftBinding;
    layout.binding.left.offset      = 0.0f;
    layout.binding.top.toView       = NULL;
    layout.binding.top.kind         = inTopBinding;
    layout.binding.top.offset       = 0.0f;
    layout.binding.right.toView     = NULL;
    layout.binding.right.kind       = inRightBinding;
    layout.binding.right.offset     = 0.0f;
    layout.binding.bottom.toView    = NULL;
    layout.binding.bottom.kind      = inBottomBinding;
    layout.binding.bottom.offset    = 0.0f;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetHorizontalPosition(
    HIPositionKind  inPosition,             //!< The type of positioning.
    View*           inView /* = NULL */,    //!< The view relative to which the positioning will occur.  May be @c NULL, in which case positioning is relative to the superview.
    float           inOffset /* = 0.0f */)  //!< Offset applied to position.
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.position.x.toView    = inView ? inView->GetViewRef() : NULL;
    layout.position.x.kind      = inPosition;
    layout.position.x.offset    = inOffset;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetHorizontalScale(
    float           inRatio,                //!< The scaling factor.  A value of 0 turns off scaling.
    View*           inView /* = NULL */)    //!< The view relative to which the positioning will occur.  May be @c NULL, in which case positioning is relative to the superview.
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.scale.x.toView   = inView ? inView->GetViewRef() : NULL;
    layout.scale.x.kind     = kHILayoutScaleAbsolute;
    layout.scale.x.ratio    = inRatio;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetVerticalScale(
    float           inRatio,                //!< The scaling factor.  A value of 0 turns off scaling.
    View*           inView /* = NULL */)    //!< The view relative to which the positioning will occur.  May be @c NULL, in which case positioning is relative to the superview.
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.scale.y.toView   = inView ? inView->GetViewRef() : NULL;
    layout.scale.y.kind     = kHILayoutScaleAbsolute;
    layout.scale.y.ratio    = inRatio;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetVerticalPosition(
    HIPositionKind  inPosition,             //!< The type of positioning.
    View*           inView /* = NULL */,    //!< The view relative to which the positioning will occur.  May be @c NULL, in which case positioning is relative to the superview.
    float           inOffset /* = 0.0f */)  //!< Offset applied to position.
{
    HILayoutInfo    layout  = GetLayout();
    
    layout.position.y.toView    = inView ? inView->GetViewRef() : NULL;
    layout.position.y.kind      = inPosition;
    layout.position.y.offset    = inOffset;
    
    SetLayout(layout);
}

// ------------------------------------------------------------------------------------------
void
View::SetLayout(const HILayoutInfo& inLayout)
{
    OSStatus    err;
    
    err = HIViewSetLayoutInfo(mViewRef, &inLayout);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
HILayoutInfo
View::GetLayout() const
{
    HILayoutInfo    layout;
    OSStatus        err;
    
    layout.version = kHILayoutInfoVersionZero;
    
    err = HIViewGetLayoutInfo(mViewRef, &layout);
    B_THROW_IF_STATUS(err);
    
    return layout;
}

// ------------------------------------------------------------------------------------------
void
View::ApplyLayout()
{
    OSStatus    err;
    
    err = HIViewApplyLayout(mViewRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::SuspendLayout()
{
    OSStatus    err;
    
    err = HIViewSuspendLayout(mViewRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
View::ResumeLayout()
{
    OSStatus    err;
    
    err = HIViewResumeLayout(mViewRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
View::IsLayoutActive() const
{
    return (HIViewIsLayoutActive(mViewRef));
}

// ------------------------------------------------------------------------------------------
bool
View::IsViewLayoutActive() const
{
    return (HIViewIsLayoutLatentlyActive(mViewRef));
}

// ------------------------------------------------------------------------------------------
View::const_iterator
View::begin() const
{
    return (const_iterator(mViewRef));
}

// ------------------------------------------------------------------------------------------
View::const_reverse_iterator
View::rbegin() const
{
    return (const_reverse_iterator(mViewRef));
}

// ------------------------------------------------------------------------------------------
/*! @return The View object associated with @a inViewRef.  Never returns @c NULL (an 
            exception is thrown if something goes wrong).
*/
View*
View::GetViewFromRef(
    HIViewRef   inViewRef)  //!< The HIView.
{
    View*   view    = ViewObjectProperty::Get(inViewRef);
    
    if (view == NULL)
        B_THROW_STATUS(errUnknownControl);
    
    return (view);
}

// ------------------------------------------------------------------------------------------
/*! @return The View object associated with @a inViewRef, or @c NULL.
*/
View*
View::GetViewFromRef(
    HIViewRef   inViewRef,  //!< The HIView.
    const std::nothrow_t&)  //!< An indication that the caller doesn't want the function to throw.
{
    View*   view;
    
    if (!ViewObjectProperty::Get(inViewRef, view, std::nothrow))
        view = NULL;
    
    return (view);
}


// ==========================================================================================
//  PredefinedView

#pragma mark -

// ------------------------------------------------------------------------------------------
PredefinedView::PredefinedView(HIViewRef inViewRef)
    : View(inViewRef), 
      mEventHandler(inViewRef)
{
    InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
PredefinedView::InitEventHandler()
{
    mEventHandler.Add(this, &PredefinedView::ControlDispose);
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
/*! Hides the view, optionally sets its id, optionally makes it a subview of 
    @a inSuperview, and finally moves the view to the correct location.
*/
View*
PredefinedView::SetupView(
    const HIViewID&     inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef           inViewRef,      //!< The newly created view.
    HIViewRef           inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*         inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*                inFromNib,      //!< The nib from which to load other resources.  May be @c NULL.
    const ViewFactory*  inFactory)      //!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
{
    OSPtr<HIViewRef>    viewPtr(inViewRef, from_copy);
    View*               viewObj;
    OSStatus            err;
    
    err = HIViewSetVisible(inViewRef, false);
    B_THROW_IF_STATUS(err);
    
    if ((inViewID.signature != 0) || (inViewID.id != 0))
    {
        err = HIViewSetID(inViewRef, inViewID);
        B_THROW_IF_STATUS(err);
    }
    
    if (inSuperview != NULL)
    {
        err = HIViewAddSubview(inSuperview, inViewRef);
        B_THROW_IF_STATUS(err);
    }
    
    if (inFrame != NULL)
    {
        Rect    oldFrame;
        
        err = HIViewGetFrame(inViewRef, &oldFrame);
        B_THROW_IF_STATUS(err);
        
        if (oldFrame != *inFrame)
        {
            err = HIViewSetFrame(inViewRef, inFrame);
            B_THROW_IF_STATUS(err);
        }
    }
    
    if (inFactory == NULL)
        inFactory = &ViewFactory::Default();
    
    viewObj = inFactory->Instantiate(inViewRef);
    
    View::AwakenViewAndChildren(inViewRef, inFromNib);
    
    return (viewPtr.release(), viewObj);
}

// ------------------------------------------------------------------------------------------
bool
PredefinedView::ControlDispose(
    Event<kEventClassControl, kEventControlDispose>&    event)
{
    if (event.mViewRef == *this)
    {
        delete this;
    }
    
    return (false);
}


}   // namespace B
