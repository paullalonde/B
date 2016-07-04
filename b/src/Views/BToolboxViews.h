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

/*! @file   BToolboxViews.h
    
    @note   We don't do the following controls, since they don't support
            compositing windows:
            - TE-based edit text control
            - list box control
            - scroll text box control
    
    @note   When creating a view programatically (ie via XXX::Create()), the view is 
            initially  made invisible.  This paves over an inconsistency between the 
            older Control-Mgr-style views and the new HIView-style ones.
    
    @todo   HIComboBox
    @todo   HISearchField, HISegmentedView (Panther only)
*/

#ifndef BToolboxViews_H_
#define BToolboxViews_H_

#pragma once

// standard headers
#include <vector>

// boost headers
#include <boost/logic/tribool.hpp>

// B headers
#include "BFwd.h"
#include "BTextSource.h"
#include "BTextValidator.h"
#include "BView.h"
#include "BViewData.h"


namespace B {

// forward declarations
class   Point;
class   Rect;
namespace Graphics {
    class   Color;
}


// ==========================================================================================
//  BevelButton

/*! @brief  Bevel button control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class BevelButton : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static BevelButton*
                    Create(
                        const HIViewID&                 inViewID,
                        HIViewRef                       inSuperview,
                        const Rect*                     inFrame, 
                        Nib*                            inFromNib, 
                        const ViewFactory*              inFactory,
                        const String&                   inTitle, 
                        ControlBevelThickness           inThickness, 
                        ControlBevelButtonBehavior      inBehavior, 
                        const HIViewImageContentInfo&   inContent, 
                        MenuRef                         inMenuRef, 
                        bool                            inViewOwnsMenu, 
                        ControlBevelButtonMenuBehavior  inMenuBehavior,
                        ControlBevelButtonMenuPlacement inMenuPlacement);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    BevelButton(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    ThemeButtonKind
            GetButtonKind() const;
    ControlButtonTextAlignment
            GetTextAlignment() const;
    SInt16  GetTextOffset() const;
    ControlButtonTextPlacement
            GetTextPlacement() const;
    void    GetGraphic(HIViewImageContentInfo& outContent) const;
    IconTransformType
            GetGraphicTransform() const;
    ControlButtonGraphicAlignment
            GetGraphicAlignment() const;
    Point   GetGraphicOffset() const;
    bool    GetScaleIcon() const;
    MenuRef GetMenu() const;
    SInt16  GetMenuValue() const;
    SInt32  GetMenuDelay() const;
    bool    GetCenterPopupGlyph() const;
    bool    GetMenuMultiValued() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetButtonKind(ThemeButtonKind inButtonKind);
    void    SetTextAlignment(ControlButtonTextAlignment inAlignment);
    void    SetTextOffset(SInt16 inOffset);
    void    SetTextPlacement(ControlButtonTextPlacement inPlacement);
    void    SetGraphic(const HIViewImageContentInfo& inContent);
    void    SetGraphicTransform(IconTransformType inTransform);
    void    SetGraphicAlignment(ControlButtonGraphicAlignment inAlignment);
    void    SetGraphicOffset(const Point& inOffset);
    void    SetScaleIcon(bool scaleIt);
    void    SetMenu(MenuRef inMenu, bool inViewOwnsMenu);
    void    SetMenuValue(SInt16 inValue);
    void    SetMenuDelay(SInt32 inDelay);
    void    SetCenterPopupGlyph(bool centerIt);
    void    SetMenuMultiValued(bool isMultivalued);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindBevelButton, TAG> {};
};


// ==========================================================================================
//  Slider

/*! @brief  %Slider control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class Slider : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static Slider*  Create(
                        const HIViewID&             inViewID,
                        HIViewRef                   inSuperview, 
                        const Rect*                 inFrame, 
                        Nib*                        inFromNib, 
                        const ViewFactory*          inFactory,
                        SInt32                      inValue, 
                        SInt32                      inMinimum, 
                        SInt32                      inMaximum, 
                        ControlSliderOrientation    inOrientation, 
                        UInt16                      inNumTickMarks, 
                        bool                        inLiveTracking, 
                        ControlActionUPP            inLiveTrackingUPP);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    Slider(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindSlider, TAG> {};
};


// ==========================================================================================
//  DisclosureTriangle

/*! @brief  Disclosure Triangle control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class DisclosureTriangle : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static DisclosureTriangle*
                    Create(
                        const HIViewID&                         inViewID,
                        HIViewRef                               inSuperview, 
                        const Rect*                             inFrame, 
                        Nib*                                    inFromNib, 
                        const ViewFactory*                      inFactory,
                        ControlDisclosureTriangleOrientation    inOrientation, 
                        const String&                           inTitle, 
                        SInt32                                  inInitialValue, 
                        bool                                    inDrawTitle, 
                        bool                                    inAutoToggle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    DisclosureTriangle(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    SInt16  GetLastValue() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetLastValue(SInt16 inLastValue);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindDisclosureTriangle, TAG> {};
};


// ==========================================================================================
//  ProgressBar

/*! @brief  Progress Bar control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ProgressBar : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ProgressBar*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        SInt32              inValue, 
                        SInt32              inMinimum, 
                        SInt32              inMaximum, 
                        bool                inIndeterminate);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ProgressBar(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    bool    GetIndeterminate() const;
    bool    GetAnimating() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetIndeterminate(bool inIndeterminate);
    void    SetAnimating(bool inAnimating);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindProgressBar, TAG> {};
};


// ==========================================================================================
//  RelevanceBar

/*! @brief  Relevance Bar control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class RelevanceBar : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static RelevanceBar*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        SInt32              inValue, 
                        SInt32              inMinimum, 
                        SInt32              inMaximum);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    RelevanceBar(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindRelevanceBar, TAG> {};
};


// ==========================================================================================
//  LittleArrows

/*! @brief  Little Arrows control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class LittleArrows : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static LittleArrows*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        SInt32              inValue, 
                        SInt32              inMinimum, 
                        SInt32              inMaximum, 
                        SInt32              inIncrement);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    LittleArrows(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    SInt32  GetIncrement() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetIncrement(SInt32 inIncrement);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindLittleArrows, TAG> {};
};


// ==========================================================================================
//  ChasingArrows

/*! @brief  Chasing Arrows control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ChasingArrows : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ChasingArrows*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame,
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ChasingArrows(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    bool    GetAnimating() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetAnimating(bool inAnimating);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindChasingArrows, TAG> {};
};


// ==========================================================================================
//  TabbedView

/*! @brief  Tabbed %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class TabbedView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static TabbedView*
                    Create(
                        const HIViewID&                     inViewID,
                        HIViewRef                           inSuperview, 
                        const Rect*                         inFrame, 
                        Nib*                                inFromNib, 
                        const ViewFactory*                  inFactory,
                        ControlTabSize                      inSize,
                        ControlTabDirection                 inDirection, 
                        const std::vector<ControlTabEntry>& inTabs);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    TabbedView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    Rect        GetContentRect() const;
    //@}
    
    /*! @name Tab Access
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void        SetTabID(int inTabIndex, int inID);
    bool        IsTabEnabled(int inTabIndex) const;
    void        EnableTab(int inTabIndex, bool inEnable);
    void        GetTabFont(int inTabIndex, ControlFontStyleRec& outFont) const;
    void        SetTabFont(int inTabIndex, const ControlFontStyleRec& inFont);
    String      GetTabName(int inTabIndex) const;
    void        SetTabName(int inTabIndex, const String& inName);
    void        GetTabImageContent(int inTabIndex, HIViewImageContentInfo& outContent) const;
    void        SetTabImageContent(int inTabIndex, const HIViewImageContentInfo& inContent);
    //@}
    
protected:
    
    void    ShowTab(int inTabIndex);
    
private:
    
    // types
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindTabs, TAG> {};
    
    void    InitEventHandler();
    
    // Carbon %Event handlers
    bool    ControlValueFieldChanged(
                Event<kEventClassControl, kEventControlValueFieldChanged>&      event);
    
    // member variables
    EventHandler        mEventHandler;
    std::vector<View*>  mUserPanes;
    mutable int         mLastTabIndex;
};


// ==========================================================================================
//  VisualSeparator

/*! @brief  Visual Separator control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class VisualSeparator : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static VisualSeparator*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame,
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    VisualSeparator(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindSeparator, TAG> {};
};


// ==========================================================================================
//  GroupBox

/*! @brief  Group Box control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class GroupBox : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static GroupBox*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        const String&       inTitle, 
                        bool                inPrimary);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    GroupBox(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    Rect    GetTitleRect() const;
    Rect    GetContentFrameRect() const;
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindGroupBox, TAG> {};
};


// ==========================================================================================
//  CheckBoxGroup

#pragma mark -
/*! @brief  Check Box Group control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class CheckBoxGroup : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static CheckBoxGroup*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        const String&       inTitle, 
                        SInt32              inInitialValue,
                        bool                inPrimary, 
                        bool                inAutoToggle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    CheckBoxGroup(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    Rect    GetTitleRect() const;
    Rect    GetContentFrameRect() const;
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindCheckGroupBox, TAG> {};
};


// ==========================================================================================
//  ImageWell

/*! @brief  Image Well control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ImageWell : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ImageWell*
                    Create(
                        const HIViewID&                 inViewID,
                        HIViewRef                       inSuperview, 
                        const Rect*                     inFrame, 
                        Nib*                            inFromNib, 
                        const ViewFactory*              inFactory,
                        const HIViewImageContentInfo&   inContent);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ImageWell(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    GetContent(HIViewImageContentInfo& outContent) const;
    IconTransformType
            GetTransform() const;
    bool    GetIsDragDestination() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetContent(const HIViewImageContentInfo& inContent);
    void    SetTransform(IconTransformType inTransform);
    void    SetIsDragDestination(bool inIsDestination);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindImageWell, TAG> {};
};


// ==========================================================================================
//  PopupArrow

/*! @brief  Popup Arrow control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class PopupArrow : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static PopupArrow*
                    Create(
                        const HIViewID&                 inViewID,
                        HIViewRef                       inSuperview, 
                        const Rect*                     inFrame, 
                        Nib*                            inFromNib, 
                        const ViewFactory*              inFactory,
                        ControlPopupArrowOrientation    inOrientation, 
                        ControlPopupArrowSize           inSize);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    PopupArrow(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindPopupArrow, TAG> {};
};


// ==========================================================================================
//  PlacardView

/*! @brief  Placard %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class PlacardView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static PlacardView*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview,
                        const Rect*         inFrame,
                        Nib*                inFromNib,
                        const ViewFactory*  inFactory);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    PlacardView(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindPlacard, TAG> {};
};


// ==========================================================================================
//  ClockView

/*! @brief  Clock %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ClockView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ClockView*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        ControlClockType    inType, 
                        ControlClockFlags   inFlags);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ClockView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    GetDate(LongDateRec& outDate) const;
    bool    GetAnimating() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetDate(const LongDateRec& inDate);
    void    SetAnimating(bool inAnimating);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindClock, TAG> {};
};


// ==========================================================================================
//  UserPane

/*! @brief  User Pane view.
    
    User panes are only supported as simple embedding views.  If you need your own 
    custom view, derive a class from CustomView or CustomViewBase.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class UserPane : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static UserPane*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        UInt32              inFeatures);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    UserPane(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindUserPane, TAG> {};
};


// ==========================================================================================
//  StaticTextView

/*! @brief  Static Text %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class StaticTextView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static StaticTextView*
                    Create(
                        const HIViewID&             inViewID,
                        HIViewRef                   inSuperview, 
                        const Rect*                 inFrame, 
                        Nib*                        inFromNib, 
                        const ViewFactory*          inFactory,
                        const String&               inText,
                        const ControlFontStyleRec&  inFontStyle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    StaticTextView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    virtual String  GetText() const;
    TruncCode       GetTruncation() const;
    bool            GetIsMultiline() const;
    SInt16          GetTextHeight() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    virtual void    SetText(const String& inTitle);
    void            SetTruncation(TruncCode inTruncation);
    void            SetIsMultiline(bool inIsMultiline);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindStaticText, TAG> {};
};


// ==========================================================================================
//  PictureControl

/*! @brief  Picture control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class PictureControl : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static PictureControl*
                    Create(
                        const HIViewID&                 inViewID,
                        HIViewRef                       inSuperview, 
                        const Rect*                     inFrame, 
                        Nib*                            inFromNib, 
                        const ViewFactory*              inFactory,
                        const HIViewImageContentInfo&   inContent, 
                        bool                            inDontTrack);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    PictureControl(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    PicHandle   GetPicture() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void        SetPicture(PicHandle inPicture);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindPicture, TAG> {};
};


// ==========================================================================================
//  IconView

/*! @brief  %Icon %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class IconView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static IconView*
                    Create(
                        const HIViewID&                 inViewID,
                        HIViewRef                       inSuperview, 
                        const Rect*                     inFrame, 
                        Nib*                            inFromNib, 
                        const ViewFactory*              inFactory,
                        const HIViewImageContentInfo&   inContent, 
                        bool                            inDontTrack);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    IconView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    GetContent(HIViewImageContentInfo& outContent) const;
    IconTransformType
            GetTransform() const;
    IconAlignmentType
            GetAlignment() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetContent(const HIViewImageContentInfo& inContent);
    void    SetTransform(IconTransformType inTransform);
    void    SetAlignment(IconAlignmentType inAlignment);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindIcon, TAG> {};
};


// ==========================================================================================
//  WindowHeaderView

/*! @brief  %Window Header %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class WindowHeaderView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static WindowHeaderView*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        bool                inIsListHeader);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    WindowHeaderView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    bool    IsListHeader() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetListHeader(bool inIsListHeader);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindWindowHeader, TAG> {};
};


// ==========================================================================================
//  PushButton

/*! @brief  Push Button control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class PushButton : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static PushButton*  Create(
                            const HIViewID&                 inViewID,
                            HIViewRef                       inSuperview, 
                            const Rect*                     inFrame, 
                            Nib*                            inFromNib, 
                            const ViewFactory*              inFactory,
                            String&                         inTitle,
                            const HIViewImageContentInfo&   inContent, 
                            ControlPushButtonIconAlignment  inAlignment);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    PushButton(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    bool    IsDefault() const;
    bool    IsCancel() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetDefault(bool inSetIt);
    void    SetCancel(bool inSetIt);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindPushButton, TAG> {};
};


// ==========================================================================================
//  GenericToggle

/*! @brief  Common implementation for radio buttons and check boxes.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
template <OSType KIND> class GenericToggle : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static GenericToggle<KIND>*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        String&             inTitle,
                        SInt32              inInitialValue, 
                        bool                inAutoToggle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    GenericToggle(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    bool    AutoToggles() const;
    bool    IsOn() const            { return (GetValue() == kControlCheckBoxCheckedValue); }
    bool    IsOff() const           { return (GetValue() == kControlCheckBoxUncheckedValue); }
    bool    IsMixed() const         { return (GetValue() == kControlCheckBoxMixedValue); }
    boost::logic::tribool
            GetTriState() const;
    bool    SupportsMixed() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetOn()                 { SetValue(kControlCheckBoxCheckedValue); }
    void    SetOff()                { SetValue(kControlCheckBoxUncheckedValue); }
    void    SetMixed()              { SetValue(kControlCheckBoxMixedValue); }
    void    SetTriState(boost::logic::tribool tb);
    void    SupportsMixed(bool inCanBeMixed);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, KIND, TAG> {};
};

/*! @brief  Check Box control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
typedef GenericToggle<kControlKindCheckBox>     CheckBox;

/*! @brief  Radio Button control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
typedef GenericToggle<kControlKindRadioButton>  RadioButton;


// ==========================================================================================
//  ScrollBar

/*! @brief  Scroll Bar control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ScrollBar : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ScrollBar*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        SInt32              inValue, 
                        SInt32              inMinimum, 
                        SInt32              inMaximum, 
                        SInt32              inViewSize, 
                        bool                inLiveTracking, 
                        ControlActionUPP    inLiveTrackingUPP);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ScrollBar(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    SInt32  GetViewSize() const;
    bool    GetShowsArrows() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetViewSize(SInt32 inViewSize);
    void    SetShowsArrows(bool inShowsArrows);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindScrollBar, TAG> {};
};


// ==========================================================================================
//  PopupButton

/*! @brief  Popup Button control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class PopupButton : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static PopupButton*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        const String&       inTitle, 
                        MenuRef             inMenuRef, 
                        bool                inViewOwnsMenu, 
                        bool                inVariableWidth, 
                        SInt16              inTitleWidth, 
                        SInt16              inJustification, 
                        Style               inStyle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    PopupButton(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    MenuRef GetMenu() const;
    SInt16  GetExtraHeight() const;
    bool    GetCheckCurrent() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetMenu(MenuRef inMenu, bool inViewOwnsMenu);
    void    SetExtraHeight(SInt16 inExtraHeight);
    void    SetCheckCurrent(bool inCheckCurrent);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindPopupButton, TAG> {};
};


// ==========================================================================================
//  RadioGroup

/*! @brief  Radio Group control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class RadioGroup : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static RadioGroup*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview,
                        const Rect*         inFrame,
                        Nib*                inFromNib,
                        const ViewFactory*  inFactory);
    //! Create an object and its HIToolbox view, then embed the given list of views within it.
    static RadioGroup*
                    CreateAndEmbedSubviews(
                        const HIViewID&     inViewID,
                        UInt32              inCommandID,
                        HIViewRef           inSuperview,
                        const std::vector<HIViewID> inSubviewIDs,
                        Nib*                inFromNib,
                        const ViewFactory*  inFactory);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    RadioGroup(HIViewRef inViewRef);
    //@}
    
private:
    
    static HIViewRef    FindEmbeddedView(
                            HIViewRef   inSuperview,
                            HIViewID    inViewID);
    static void         MergeEmbeddedViewFrame(
                            HIViewRef   inSubview,
                            B::Rect&    ioGroupFrame);
    static void         MoveEmbeddedView(
                            HIViewRef   inSubview,
                            CGPoint     inGroupOrigin,
                            RadioGroup* inGroup);
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindRadioGroup, TAG> {};
};


// ==========================================================================================
//  DisclosureButton

/*! @brief  Disclosure Button control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class DisclosureButton : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static DisclosureButton*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        SInt32              inValue, 
                        bool                inAutoToggle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    DisclosureButton(HIViewRef inViewRef);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindDisclosureButton, TAG> {};
};


// ==========================================================================================
//  RoundButton

/*! @brief  Round Button control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class RoundButton : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*        Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static RoundButton* Create(
                            const HIViewID&                 inViewID,
                            HIViewRef                       inSuperview, 
                            const Rect*                     inFrame, 
                            Nib*                            inFromNib, 
                            const ViewFactory*              inFactory,
                            ControlRoundButtonSize          inSize, 
                            const HIViewImageContentInfo&   inContent);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    RoundButton(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    GetContent(HIViewImageContentInfo& outContent) const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    void    SetContent(const HIViewImageContentInfo& inContent);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindRoundButton, TAG> {};
};


// NOTE:    The data browser is in its own file (it's big enough for that I think).


// ==========================================================================================
//  TextFieldPart

class TextFieldPart : public TextSource
{
private:
    
    typedef boost::signal1<void, TextFieldPart&>                                        TextChangedSignal;
    typedef boost::signal3<void, TextFieldPart&, bool, const TextValidator::Exception&> ValidationFailedSignal;

public:
    
    //! @name Types
    //@{
    //! Shorthand for a type of signal used by this class.
    typedef TextValidator::Signal::slot_function_type   FilterFunction;
    //! Shorthand for a type of signal used by this class.
    typedef TextChangedSignal::slot_function_type       TextChangedFunction;
    //! Shorthand for a type of signal used by this class.
    typedef ValidationFailedSignal::slot_function_type  ValidationFailedFunction;
    //@}

    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    virtual String  GetText() const;
    virtual size_t  GetTextSize() const;
    virtual void    GetSelection(size_t& outSelStart, size_t& outSelEnd) const;
    ControlKeyScriptBehavior
                    GetKeyScriptBehavior() const;
    bool            GetFixedText() const;
    ControlEditTextValidationUPP
                    GetValidationProc() const;
    EditUnicodePostUpdateUPP
                    GetPostUpdateProc() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    virtual void    SetText(const String& inText);
    virtual void    SetSelection(size_t inSelStart, size_t inSelEnd);
    virtual void    SelectAll();
    void            SetKeyScriptBehavior(ControlKeyScriptBehavior inKeyScriptBehavior);
    void            SetFixedText(bool inFixedText);
    void            SetValidationProc(ControlEditTextValidationUPP inValidationProc);
    void            SetPostUpdateProc(EditUnicodePostUpdateUPP inPostUpdateProc);
    //@}
    
    bool    Validate();
    
    //! @name Signals
    //@{
    void    ConnectFilter(
                FilterFunction              inFunction);
    void    ConnectFilter(
                int                         inOrder,
                FilterFunction              inFunction);
    void    ConnectTextChanged(
                TextChangedFunction         inFunction);
    void    ConnectValidationFailed(
                ValidationFailedFunction    inFunction);
    //@}
    
protected:
    
    //! @name Constructor / Destructor.
    //@{
    //! Constructor.
    TextFieldPart(HIViewRef inViewRef);
    //! Destructor.
    virtual ~TextFieldPart();
    //@}
    
    virtual bool    TextChanging(
                        const CFRange&  inSelection,
                        const String&   inNewText, 
                        String&         outReplacementText,
                        bool&           outCancel);
    virtual void    TextChanged();
    
private:
    
    void    InitEventHandler();

    // Carbon %Event handlers
    bool    TextAccepted(
                Event<kEventClassTextField, kEventTextAccepted>&            event);
    bool    TextShouldChangeInRange(
                Event<kEventClassTextField, kEventTextShouldChangeInRange>& event);
    
    // member variables
    const HIViewRef         mViewRef;
    EventHandler            mEventHandler;
    TextValidator           mValidator;
    TextChangedSignal       mTextChangedSignal;
    ValidationFailedSignal  mValidationFailedSignal;
};

// ------------------------------------------------------------------------------------------
inline void
TextFieldPart::ConnectFilter(
    int             inOrder,
    FilterFunction  inFunction)
{
    mValidator.Connect(inOrder, inFunction);
}

// ------------------------------------------------------------------------------------------
inline void
TextFieldPart::ConnectFilter(
    FilterFunction  inFunction)
{
    mValidator.Connect(inFunction);
}


// ==========================================================================================
//  BeepOnValidationFailed

/*! @brief  Validation-failed functor that beeps.
    
    This is a simple helper class that is meant to be passed to 
    TextFieldPart::ConnectValidationFailed(). It beeps when invoked. This has the effect 
    of causing a beep whenever the user enters a character that is rejected by one of the 
    filters attached to the text field part.
    
    @ingroup    ViewGroup
*/
struct BeepOnValidationFailed
{
    void    operator () (
                TextFieldPart&                  inPart,
                bool                            inFiltering,
                const TextValidator::Exception& ex) const;
};


// ==========================================================================================
//  TextField

/*! @brief  Text Field control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
    @todo   What about kControlEditTextInsertCFStringRefTag (get & set)?
*/
class TextField : public PredefinedView, public TextFieldPart
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static TextField*
                    Create(
                        const HIViewID&             inViewID,
                        HIViewRef                   inSuperview, 
                        const Rect*                 inFrame, 
                        Nib*                        inFromNib, 
                        const ViewFactory*          inFactory,
                        const String&               inText,
                        bool                        inIsPassword,
                        const ControlFontStyleRec&  inFontStyle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    TextField(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    virtual String  GetText() const;
    String          GetPasswordText() const;
    bool            GetLocked() const;
    bool            GetSingleLine() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/ControlDefinitions.h\> for more information.
    */
    //@{
    virtual void    SetText(const String& inText);
    void            SetPasswordText(const String& inPasswordText);
    void            SetLocked(bool inLocked);
    void            SetSingleLine(bool inSingleLine);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindEditUnicodeText, TAG> {};
};


// ==========================================================================================
//  ScrollView

/*! @brief  Scroll %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ScrollView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ScrollView*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        OptionBits          inOptions);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ScrollView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/HIView.h\> for more information.
    */
    //@{
    bool    GetAutoHide() const;
    float   GetLeftInset() const;
    float   GetTopInset() const;
    bool    CanNavigate(HIScrollViewAction inAction) const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/HIView.h\> for more information.
    */
    //@{
    void    SetAutoHide(bool autoHide);
    void    SetLeftInset(float inset);
    void    SetTopInset(float inset);
    void    Navigate(HIScrollViewAction inAction);
    //@}
    
    void    ReplaceContent(HIViewRef inNewView);
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindHIScrollView, TAG> {};
};


// ==========================================================================================
//  ImageView

/*! @brief  Image %View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ImageView : public PredefinedView
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ImageView*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        OSPtr<CGImageRef>   inImage);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ImageView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/HIView.h\> for more information.
    */
    //@{
    bool    GetOpaque() const;
    float   GetAlpha() const;
    bool    GetScaleToFit() const;
    OSPtr<CGImageRef>
            GetImage() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/HIView.h\> for more information.
    */
    //@{
    void    SetOpaque(bool inOpaque);
    void    SetAlpha(float inAlpha);
    void    SetScaleToFit(bool inScaleToFit);
    void    SetImage(OSPtr<CGImageRef> inImage);
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindHIImageView, TAG> {};
};


// ==========================================================================================
//  ComboBox

/*! @brief  Combo Box control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class ComboBox : public PredefinedView, public TextFieldPart
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static ComboBox*
                    Create(
                        SInt32                      inViewID,
                        HIViewRef                   inSuperview, 
                        const Rect*                 inFrame, 
                        Nib*                      inFromNib, 
                        const ViewFactory*          inFactory,
                        OptionBits                  inOptions,
                        const String&               inText,
                        //const Array&                inList,
                        const ControlFontStyleRec&  inStyle);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    ComboBox(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/HIView.h\> for more information.
    */
    //@{
    virtual String  GetText() const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/HIView.h\> for more information.
    */
    //@{
    virtual void    SetText(const String& inText);
    //@}
    
protected:
    
    //! Destructor.
    virtual ~ComboBox();
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindHIComboBox, TAG> {};
};


// ==========================================================================================
//  SegmentedView


// ==========================================================================================
//  TextView

/*! @brief  Text View control.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
    @todo   What about kControlEditTextInsertCFStringRefTag (get & set)?
*/
class TextView : public PredefinedView, public TextFieldPart
{
public:
    
    //! @name Instantiation
    //@{
    //! Create an object given an existing HIToolbox view.
    static View*    Instantiate(HIViewRef inViewRef);
    //! Create an object and its HIToolbox view, from the given parameters.
    static TextView*
                    Create(
                        const HIViewID&     inViewID,
                        HIViewRef           inSuperview, 
                        const Rect*         inFrame, 
                        Nib*                inFromNib, 
                        const ViewFactory*  inFactory,
                        OptionBits          inOptions,
                        TXNFrameOptions     inFrameOptions);
    //@}
    
    //! @name Constructor
    //@{
    //! Constructor.
    TextView(HIViewRef inViewRef);
    //@}
    
    /*! @name Inquiries
        See \<HIToolbox/MacTextEditor.h\> for more information.
    */
    //@{
    virtual String  GetText() const;
    TXNObject       GetTXNObject() const        { return mTextObj; }
    Graphics::Color GetBackgroundColor() const;
    virtual void    GetSelection(size_t& outSelStart, size_t& outSelEnd) const;
    //@}
    
    /*! @name Modifiers
        See \<HIToolbox/MacTextEditor.h\> for more information.
    */
    //@{
    virtual void    SetText(const String& inText);
    void            SetBackgroundColor(const Graphics::Color& inColor);
    virtual void    SetSelection(size_t inSelStart, size_t inSelEnd);
    virtual void    SelectAll();
    //@}
    
private:
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindHITextView, TAG> {};
    
    // member variables
    const TXNObject mTextObj;
};


}   // namespace B


#endif  // BToolboxViews_H_
