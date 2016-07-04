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

#ifndef BViewData_H_
#define BViewData_H_

#if defined(__MWERKS__)
#   pragma once
#endif

// standard headers
#include <new>

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BFwd.h"
#include "BString.h"


#if !B_BUILDING_ON_10_3_OR_LATER
// Missing declarations from pre-Panther headers.
enum {
    kControlStaticTextIsMultilineTag    = 'stim'    // 10.1 and up, but apparently undocumented until 10.3
};
#endif

// Missing declarations from all versions of headers (up to Tiger anyway)
enum {
    // These two were documented by Eric Schlegel on Carbon-Dev mailing list.
    kControlScrollViewVerticalInset     = 'vsin',   // Offset of the vertical scroll bar from the left edge of the scroll view.
    kControlScrollViewHorizontalInset   = 'hsin'    // Offset of the horizontal scroll bar from the left edge of the scroll view.
};

namespace B {

// forward declarations
class   Point;
class   Rect;
class   Url;

typedef ::Point QDPoint;
typedef ::Rect  QDRect;

// ==========================================================================================
//  ViewDataTrait

/*!
    @brief  Gets & sets HIVieRef data in a type-safe manner.
    
    ViewDataTrait provides a type-safe wrapper around the Control 
    Manager calls GetControlData() and SetControlData().
    
    The template parameter, @a T, is a C/C++ language type.  It's the 
    type of the data to be stored into or retrieved from the view.
    
    @ingroup    ViewGroup
*/
template <typename T> struct ViewDataTrait
{
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    T;      //!< The template parameter.  Any C/C++ language type.
    //@}
#endif
    
    //! @name Types
    //@{
    typedef T           Type;   //!< An easy-to-access synonym for @a T.
    //@}
    
    /*! @name View Access
        Allows access to the data for a given view, part, and tag.  Clients will 
        use these functions to interact with the view.
    */
    //@{
    //! Retrieves the view data for the given view, part, and tag.
    static void     Get(
                        HIViewRef       view,
                        ControlPartCode part, 
                        ResType         tag, 
                        T&              value);
    //! Changes the view data for the given view, part, and tag.
    static void     Set(
                        HIViewRef       view, 
                        ControlPartCode part, 
                        ResType         tag, 
                        const T&        value);
    
    //! Retrieves the view data for the given view, part, and tag (no-throw variant).
    static OSStatus Get(
                        HIViewRef       view,
                        ControlPartCode part, 
                        ResType         tag, 
                        T&              value,
                        const std::nothrow_t&);
    //! Changes the view data for the given view, part, and tag (no-throw variant).
    static OSStatus Set(
                        HIViewRef       view, 
                        ControlPartCode part, 
                        ResType         tag, 
                        const T&        value,
                        const std::nothrow_t&);
    //@}
    
    /*! @name Memory Access
        Allows access to the data at a given memory address.  Implementations 
        of custom controls will use these functions to read and write the data 
        to/from Carbon %Events.
    */
    //@{
    //! Retrieves the view data from an arbitrary address in memory.
    static OSStatus Get(
                        const void*     data, 
                        size_t          size,
                        T&              value);
    
    //! Copies the view data to an arbitrary address in memory.
    static OSStatus Set(
                        void*           data, 
                        size_t          size,
                        const T&        value);
    //@}
};


// ------------------------------------------------------------------------------------------
/*! @return     An OS status code.
    @exception  none
*/
template <typename T> inline OSStatus
ViewDataTrait<T>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    T&              value,  //!< Holds the output.
    const std::nothrow_t&)
{
    ::Size      size;
    OSStatus    err;
    
    err = GetControlData(view, part, tag, sizeof(value), &value, &size);
    if ((err == noErr) && (size != sizeof(value)))
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
template <typename T> void
ViewDataTrait<T>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    T&              value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! @return     An OS status code.
    @exception  none
*/
template <typename T> inline OSStatus
ViewDataTrait<T>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    const T&        value,  //!< The new value.
    const std::nothrow_t&)
{
    return (SetControlData(view, part, tag, sizeof(value), &value));
}

// ------------------------------------------------------------------------------------------
template <typename T> void
ViewDataTrait<T>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    const T&        value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! Note that the size is checked to ensure it is equal to @c sizeof(value).
    
    @return     An OS status code.
    @exception  none
*/
template <typename T> OSStatus
ViewDataTrait<T>::Get(
    const void*     data,   //!< The source address of the view data.
    size_t          size,   //!< The size of the view data.
    T&              value)  //!< Holds the output.
{
    OSStatus    err = noErr;
    
    if (size == sizeof(value))
        value = * reinterpret_cast<const T*>(data);
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
/*! Note that the size is checked to ensure it is equal to @c sizeof(value).
    
    @return     An OS status code.
    @exception  none
*/
template <typename T> OSStatus
ViewDataTrait<T>::Set(
    void*           data,   //!< The destination address of the view data.
    size_t          size,   //!< The size of the view data.
    const T&        value)  //!< The new value.
{
    OSStatus    err = noErr;
    
    if (size == sizeof(value))
        * reinterpret_cast<T*>(data) = value;
    else
        err = errDataSizeMismatch;
    
    return (err);
}


// ==========================================================================================
//  Template instantiations of ViewDataTrait<T>

#ifndef DOXYGEN_SKIP

/*! @brief  ViewDataTrait template specialisation for @c Boolean.
    
    Converts to and from @c Boolean (the type actually read/written to the view) and 
    @c bool, the C/C++ language type passed to the member functions.
*/
template <> struct ViewDataTrait<Boolean>
{
    typedef bool    Type;
    
    static OSStatus Get(HIViewRef view, ControlPartCode part, ResType tag, bool& value, const std::nothrow_t&);
    static OSStatus Set(HIViewRef view, ControlPartCode part, ResType tag, bool value, const std::nothrow_t&);
    static void     Get(HIViewRef view, ControlPartCode part, ResType tag, bool& value);
    static void     Set(HIViewRef view, ControlPartCode part, ResType tag, bool value);

    static OSStatus Get(const void* data, size_t size, bool& value);
    static OSStatus Set(void* data, size_t size, bool value);
};

/*! @brief  ViewDataTrait template specialisation for @c CFStringRef.
    
    Converts to and from @c CFStringRef (the type actually read/written to the view) and 
    @c String, the C/C++ language type passed to the member functions.
*/
template <> struct ViewDataTrait<CFStringRef>
{
    typedef String  Type;
    
    static OSStatus Get(HIViewRef view, ControlPartCode part, ResType tag, String& value, const std::nothrow_t&);
    static OSStatus Set(HIViewRef view, ControlPartCode part, ResType tag, const String& value, const std::nothrow_t&);
    static void     Get(HIViewRef view, ControlPartCode part, ResType tag, String& value);
    static void     Set(HIViewRef view, ControlPartCode part, ResType tag, const String& value);

    static OSStatus Get(const void* data, size_t size, String& value);
    static OSStatus Set(void* data, size_t size, const String& value);
};

/*! @brief  ViewDataTrait template specialisation for @c CFURLRef.
    
    Converts to and from @c CFURLRef (the type actually read/written to the view) and 
    @c Url, the C/C++ language type passed to the member functions.
*/
template <> struct ViewDataTrait<CFURLRef>
{
    typedef Url Type;
    
    static OSStatus Get(HIViewRef view, ControlPartCode part, ResType tag, Url& value, const std::nothrow_t&);
    static OSStatus Set(HIViewRef view, ControlPartCode part, ResType tag, const Url& value, const std::nothrow_t&);
    static void     Get(HIViewRef view, ControlPartCode part, ResType tag, Url& value);
    static void     Set(HIViewRef view, ControlPartCode part, ResType tag, const Url& value);

    static OSStatus Get(const void* data, size_t size, Url& value);
    static OSStatus Set(void* data, size_t size, const Url& value);
};

/*! @brief  ViewDataTrait template specialisation for @c ControlTabInfoRecV1.
    
    The tab control defines two version of the @c ControlTabInfoRec.  This specialisation converts 
    @c ControlTabInfoRec (the original version) into @c ControlTabInfoRecV1 as necessary.  This 
    allows clients of the class to deal with just one language type.
*/
template <> struct ViewDataTrait<ControlTabInfoRecV1>
{
    typedef ControlTabInfoRecV1 Type;
    
    static OSStatus Get(HIViewRef view, ControlPartCode part, ResType tag, ControlTabInfoRecV1& value, const std::nothrow_t&);
    static OSStatus Set(HIViewRef view, ControlPartCode part, ResType tag, const ControlTabInfoRecV1& value, const std::nothrow_t&);
    static void     Get(HIViewRef view, ControlPartCode part, ResType tag, ControlTabInfoRecV1& value);
    static void     Set(HIViewRef view, ControlPartCode part, ResType tag, const ControlTabInfoRecV1& value);

    static OSStatus Get(const void* data, size_t size, ControlTabInfoRecV1& value);
    static OSStatus Set(void* data, size_t size, const ControlTabInfoRecV1& value);
};

/*! @brief  ViewDataTrait template specialisation for @c ::Rect.
    
    Converts to and from @c ::Rect (the type actually read/written to the view) and 
    @c Rect, the C/C++ language type passed to the member functions.
    
    @bug    I need to ensure this won't cause problems if a Toolbox control decides one day 
            to have data of type @c HIRect.
*/
template <> struct ViewDataTrait<QDRect>
{
    typedef Rect    Type;
    
    static OSStatus Get(HIViewRef view, ControlPartCode part, ResType tag, Rect& value, const std::nothrow_t&);
    static OSStatus Set(HIViewRef view, ControlPartCode part, ResType tag, const Rect& value, const std::nothrow_t&);
    static void     Get(HIViewRef view, ControlPartCode part, ResType tag, Rect& value);
    static void     Set(HIViewRef view, ControlPartCode part, ResType tag, const Rect& value);

    static OSStatus Get(const void* data, size_t size, Rect& value);
    static OSStatus Set(void* data, size_t size, const Rect& value);
};

/*! @brief  ViewDataTrait template specialisation for @c Point.
    
    Converts to and from @c ::Point (the type actually read/written to the view) and 
    @c Point, the C/C++ language type passed to the member functions.
    
    @bug    I need to ensure this won't cause problems if a Toolbox control decides one day 
            to have data of type @c HIPoint.
*/
template <> struct ViewDataTrait<QDPoint>
{
    typedef Point   Type;
    
    static OSStatus Get(HIViewRef view, ControlPartCode part, ResType tag, Point& value, const std::nothrow_t&);
    static OSStatus Set(HIViewRef view, ControlPartCode part, ResType tag, const Point& value, const std::nothrow_t&);
    static void     Get(HIViewRef view, ControlPartCode part, ResType tag, Point& value);
    static void     Set(HIViewRef view, ControlPartCode part, ResType tag, const Point& value);

    static OSStatus Get(const void* data, size_t size, Point& value);
    static OSStatus Set(void* data, size_t size, const Point& value);
};

#endif  // DOXYGEN_SKIP


// ==========================================================================================
//  ViewDataDef

/*!
    @brief  Maps a data tag for a specific view class onto a C/C++ language type.
    
    ViewDataDef maps a specific piece of view data, for a specific class of control, 
    onto a C/C++ language type.
    
    Here is now the identification is accomplished:
    
        -# The view class is identified via the @a SIG and @a KIND template parameters.  
           These correspond to the signature and kind fields of the Control Manager's 
           @c ControlKind struct.
        -# The individual piece of data within the view class is identified by the 
           @a TAG template parameter, which corresponds to (you guessed it) the data tag.
        -# The language type is identified by an instantiation of the ViewDataTrait class 
           template.
    
    Thus the code to get or set a piece of data can be accessed via the struct's 
    Traits type:
    
        @code ViewDataDef<SIG, KIND, TAG>::Trait::Get(...) @endcode
    
    And the language type itself can be accessed via:
    
        @code ViewDataDef<SIG, KIND, TAG>::Trait::Type; @endcode
    
    For example, in the definition:
    
        @code
        ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabEnabledFlagTag>::Trait my_traits;
        ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabEnabledFlagTag>::Trait::Type my_var;
        @endcode
    
    @p my_traits is of type @p ViewDataTrait<Boolean> whereas @p my_var is of type @p bool.
    
    Also, the view data can the get & set via the trait itself:
    
        @code ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabEnabledFlagTag>::Trait::Set(...); @endcode
    
    Note that the generic definition of ViewDataDef is deliberately useless, 
    because ViewDataDef is meant to be specialised for each <@a SIG, @a KIND, @a TAG> triple.

    @ingroup    ViewGroup
*/

template <OSType SIG, OSType KIND, ResType TAG> struct ViewDataDef
{
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef OSType  SIG;    //!< Template parameter.  An integral four-char constant containing the control class' signature.
    typedef OSType  KIND;   //!< Template parameter.  An integral four-char constant containing the control class' kind.
    typedef OSType  TAG;    //!< Template parameter.  An integral four-char constant containing the data tag.
    //@}
    
    //! @name Types
    //@{
    typedef ViewDataTrait<...>  Trait;  //!< The traits type.  An instantiation of ViewDataTrait.
    //@}
#endif
};


// ==========================================================================================
//  Template instantiations of ViewDataDef<>

#ifndef DOXYGEN_SKIP

// Common data tags (from Controls.h)

//template <> struct ViewDataDef<kControlFontStyleTag>  { typedef ControlFontStyleRec   ValueType;  };
//template <> struct ViewDataDef<kControlKeyFilterTag>  { typedef ControlKeyFilterUPP   ValueType;  };
//template <> struct ViewDataDef<kControlKindTag>       { typedef ControlKind           ValueType;  };
//template <> struct ViewDataDef<kControlSizeTag>       { typedef ControlSize           ValueType;  };

// Data tags of the system-supplied controls

// Bevel buttons

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonContentTag>          { typedef ViewDataTrait<ControlButtonContentInfo>       Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonTransformTag>        { typedef ViewDataTrait<IconTransformType>              Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonTextAlignTag>        { typedef ViewDataTrait<ControlButtonTextAlignment>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonTextOffsetTag>       { typedef ViewDataTrait<SInt16>                         Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonGraphicAlignTag>     { typedef ViewDataTrait<ControlButtonGraphicAlignment>  Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonGraphicOffsetTag>    { typedef ViewDataTrait<QDPoint>                            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonTextPlaceTag>        { typedef ViewDataTrait<ControlButtonTextAlignment>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonMenuValueTag>        { typedef ViewDataTrait<SInt16>                         Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonMenuRefTag>          { typedef ViewDataTrait<MenuRef>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonCenterPopupGlyphTag> { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonLastMenuTag>         { typedef ViewDataTrait<SInt16>                         Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonMenuDelayTag>        { typedef ViewDataTrait<SInt32>                         Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonScaleIconTag>        { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonOwnedMenuRefTag>     { typedef ViewDataTrait<MenuRef>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonKindTag>             { typedef ViewDataTrait<ThemeButtonKind>                Trait;  };
#if B_BUILDING_CAN_USE_10_3_APIS
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindBevelButton, kControlBevelButtonIsMultiValueMenuTag> { typedef ViewDataTrait<Boolean>                        Trait;  };
#endif

// slider

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindSlider, kControlSizeTag>                               { typedef ViewDataTrait<ControlSize>    Trait;  };

// disclosure triangle

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindDisclosureTriangle, kControlTriangleLastValueTag>    { typedef ViewDataTrait<SInt16>         Trait;  };

// progress bar

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindProgressBar, kControlProgressBarIndeterminateTag>    { typedef ViewDataTrait<Boolean>        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindProgressBar, kControlProgressBarAnimatingTag>        { typedef ViewDataTrait<Boolean>        Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindProgressBar, kControlSizeTag>                      { typedef ViewDataTrait<ControlSize>    Trait;  };

// relevance bar

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindRelevanceBar, kControlProgressBarIndeterminateTag> { typedef ViewDataTrait<Boolean>        Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindRelevanceBar, kControlProgressBarAnimatingTag>     { typedef ViewDataTrait<Boolean>        Trait;  };

// little arrows

#if B_BUILDING_CAN_USE_10_3_APIS
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindLittleArrows, kControlLittleArrowsIncrementValueTag> { typedef ViewDataTrait<SInt32>         Trait;  };
#endif

// chasing arrows

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindChasingArrows, kControlChasingArrowsAnimatingTag>    { typedef ViewDataTrait<Boolean>        Trait;  };

// tabs

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabContentRectTag>     { typedef ViewDataTrait<QDRect>                     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabEnabledFlagTag>     { typedef ViewDataTrait<Boolean>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabFontStyleTag>       { typedef ViewDataTrait<ControlFontStyleRec>        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabInfoTag>            { typedef ViewDataTrait<ControlTabInfoRecV1>        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlTabImageContentTag>    { typedef ViewDataTrait<ControlButtonContentInfo>   Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindTabs, kControlSizeTag>             { typedef ViewDataTrait<ControlSize>                Trait;  };

// group box

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindGroupBox, kControlGroupBoxMenuRefTag>      { typedef ViewDataTrait<MenuRef>                Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindGroupBox, kControlGroupBoxFontStyleTag>        { typedef ViewDataTrait<ControlFontStyleRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindGroupBox, kControlGroupBoxTitleRectTag>      { typedef ViewDataTrait<QDRect>                 Trait;  };
#if B_BUILDING_CAN_USE_10_3_APIS
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindGroupBox, kControlGroupBoxFrameRectTag>      { typedef ViewDataTrait<QDRect>                 Trait;  };
#endif

// check box group box

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindCheckGroupBox, kControlGroupBoxMenuRefTag> { typedef ViewDataTrait<MenuRef>                Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindCheckGroupBox, kControlGroupBoxFontStyleTag>   { typedef ViewDataTrait<ControlFontStyleRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindCheckGroupBox, kControlGroupBoxTitleRectTag> { typedef ViewDataTrait<QDRect>                 Trait;  };
#if B_BUILDING_CAN_USE_10_3_APIS
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindCheckGroupBox, kControlGroupBoxFrameRectTag> { typedef ViewDataTrait<QDRect>                 Trait;  };
#endif

// popup group box

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupGroupBox, kControlGroupBoxMenuRefTag>   { typedef ViewDataTrait<MenuRef>                Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupGroupBox, kControlGroupBoxFontStyleTag>   { typedef ViewDataTrait<ControlFontStyleRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupGroupBox, kControlGroupBoxTitleRectTag> { typedef ViewDataTrait<QDRect>                 Trait;  };
#if B_BUILDING_CAN_USE_10_3_APIS
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupGroupBox, kControlGroupBoxFrameRectTag> { typedef ViewDataTrait<QDRect>                 Trait;  };
#endif

// image well

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindImageWell, kControlImageWellContentTag>              { typedef ViewDataTrait<ControlButtonContentInfo>   Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindImageWell, kControlImageWellTransformTag>            { typedef ViewDataTrait<IconTransformType>          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindImageWell, kControlImageWellIsDragDestinationTag>    { typedef ViewDataTrait<Boolean>                    Trait;  };

// clock

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindClock, kControlClockLongDateTag>     { typedef ViewDataTrait<LongDateRec>            Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindClock, kControlClockFontStyleTag>  { typedef ViewDataTrait<ControlFontStyleRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindClock, kControlClockAnimatingTag>    { typedef ViewDataTrait<Boolean>                Trait;  };

// user pane

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserItemDrawProcTag>       { typedef ViewDataTrait<UserItemUPP>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneDrawProcTag>       { typedef ViewDataTrait<ControlUserPaneDrawUPP>         Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneHitTestProcTag>    { typedef ViewDataTrait<ControlUserPaneHitTestUPP>      Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneTrackingProcTag>   { typedef ViewDataTrait<ControlUserPaneTrackingUPP>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneIdleProcTag>       { typedef ViewDataTrait<ControlUserPaneIdleUPP>         Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneKeyDownProcTag>    { typedef ViewDataTrait<ControlUserPaneKeyDownUPP>      Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneActivateProcTag>   { typedef ViewDataTrait<ControlUserPaneActivateUPP>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneFocusProcTag>      { typedef ViewDataTrait<ControlUserPaneFocusUPP>        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindUserPane, kControlUserPaneBackgroundProcTag> { typedef ViewDataTrait<ControlUserPaneBackgroundUPP>   Trait;  };

// edit text

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextStyleTag>                    { typedef ViewDataTrait<ControlFontStyleRec>            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextTextTag>                   { typedef ViewDataTrait<char*>                          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextTEHandleTag>               { typedef ViewDataTrait<TEHandle>                       Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextKeyFilterTag>                { typedef ViewDataTrait<ControlKeyFilterUPP>            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextSelectionTag>              { typedef ViewDataTrait<ControlEditTextSelectionRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextPasswordTag>               { typedef ViewDataTrait<char*>                          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextKeyScriptBehaviorTag>      { typedef ViewDataTrait<ControlKeyScriptBehavior>       Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextLockedTag>                 { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextFixedTextTag>              { typedef ViewDataTrait<char*>                          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextValidationProcTag>         { typedef ViewDataTrait<ControlEditTextValidationUPP>   Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextInlinePreUpdateProcTag>    { typedef ViewDataTrait<TSMTEPreUpdateUPP>              Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextInlinePostUpdateProcTag>   { typedef ViewDataTrait<TSMTEPostUpdateUPP>             Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextCFStringTag>               { typedef ViewDataTrait<CFStringRef>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditText, kControlEditTextPasswordCFStringTag>       { typedef ViewDataTrait<CFStringRef>                    Trait;  };

// static text

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlStaticTextStyleTag>        { typedef ViewDataTrait<ControlFontStyleRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlStaticTextTextTag>       { typedef ViewDataTrait<char*>                  Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlStaticTextTextHeightTag> { typedef ViewDataTrait<SInt16>                 Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlStaticTextTruncTag>      { typedef ViewDataTrait<TruncCode>              Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlStaticTextCFStringTag>   { typedef ViewDataTrait<CFStringRef>            Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlSizeTag>                   { typedef ViewDataTrait<ControlSize>            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindStaticText, kControlStaticTextIsMultilineTag>    { typedef ViewDataTrait<Boolean>            Trait;  };

// picture

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPicture, kControlPictureHandleTag>   { typedef ViewDataTrait<PicHandle>      Trait;  };

// icon

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindIcon, kControlIconTransformTag>  { typedef ViewDataTrait<IconTransformType>          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindIcon, kControlIconAlignmentTag>  { typedef ViewDataTrait<IconAlignmentType>          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindIcon, kControlIconResourceIDTag> { typedef ViewDataTrait<SInt16>                     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindIcon, kControlIconContentTag>    { typedef ViewDataTrait<ControlButtonContentInfo>   Trait;  };

// window header

#if B_BUILDING_CAN_USE_10_3_APIS
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindWindowHeader, kControlWindowHeaderIsListHeaderTag>   { typedef ViewDataTrait<Boolean>        Trait;  };
#endif

// list box

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindListBox, kControlListBoxListHandleTag>   { typedef ViewDataTrait<ListHandle>             Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindListBox, kControlListBoxKeyFilterTag>  { typedef ViewDataTrait<ControlKeyFilterUPP>    Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindListBox, kControlListBoxFontStyleTag>  { typedef ViewDataTrait<ControlFontStyleRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindListBox, kControlListBoxDoubleClickTag>  { typedef ViewDataTrait<Boolean>                Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindListBox, kControlListBoxLDEFTag>         { typedef ViewDataTrait<SInt16>                 Trait;  };

// push button

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPushButton, kControlPushButtonDefaultTag>        { typedef ViewDataTrait<Boolean>        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPushButton, kControlPushButtonCancelTag>         { typedef ViewDataTrait<Boolean>        Trait;  };

// push icon button

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPushIconButton, kControlPushButtonDefaultTag>    { typedef ViewDataTrait<Boolean>        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPushIconButton, kControlPushButtonCancelTag>     { typedef ViewDataTrait<Boolean>        Trait;  };

// check box

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindCheckBox, kControlSizeTag>     { typedef ViewDataTrait<ControlSize>        Trait;  };

// radio button

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindRadioButton, kControlSizeTag>  { typedef ViewDataTrait<ControlSize>        Trait;  };

// scroll bars

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollBar, kControlScrollBarShowsArrowsTag>      { typedef ViewDataTrait<Boolean>        Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollBar, kControlSizeTag>                        { typedef ViewDataTrait<ControlSize>    Trait;  };

// popup button

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupButton, kControlPopupButtonMenuRefTag>      { typedef ViewDataTrait<MenuRef>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupButton, kControlPopupButtonMenuIDTag>       { typedef ViewDataTrait<SInt16>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupButton, kControlPopupButtonExtraHeightTag>  { typedef ViewDataTrait<SInt16>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupButton, kControlPopupButtonOwnedMenuRefTag> { typedef ViewDataTrait<MenuRef>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindPopupButton, kControlPopupButtonCheckCurrentTag> { typedef ViewDataTrait<Boolean>    Trait;  };

// scroll text box

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollingTextBox, kControlScrollTextBoxDelayBeforeAutoScrollTag>     { typedef ViewDataTrait<UInt32>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollingTextBox, kControlScrollTextBoxDelayBetweenAutoScrollTag>    { typedef ViewDataTrait<UInt32>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollingTextBox, kControlScrollTextBoxAutoScrollAmountTag>          { typedef ViewDataTrait<UInt16>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollingTextBox, kControlScrollTextBoxContentsTag>                  { typedef ViewDataTrait<SInt16>     Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindScrollingTextBox, kControlScrollTextBoxAnimatingTag>                 { typedef ViewDataTrait<Boolean>    Trait;  };

// round button

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindRoundButton, kControlRoundButtonContentTag>  { typedef ViewDataTrait<ControlButtonContentInfo>   Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindRoundButton, kControlRoundButtonSizeTag>       { typedef ViewDataTrait<ControlRoundButtonSize>     Trait;  };

// data browser

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindDataBrowser, kControlDataBrowserIncludesFrameAndFocusTag>    { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindDataBrowser, kControlDataBrowserKeyFilterTag>                { typedef ViewDataTrait<ControlKeyFilterUPP>            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindDataBrowser, kControlDataBrowserEditTextValidationProcTag>   { typedef ViewDataTrait<ControlEditTextValidationUPP>   Trait;  };

// edit unicode text

//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextStyleTag>                 { typedef ViewDataTrait<ControlFontStyleRec>            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextTextTag>                    { typedef ViewDataTrait<char*>                          Trait;  };
//template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextKeyFilterTag>             { typedef ViewDataTrait<ControlKeyFilterUPP>            Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextSelectionTag>               { typedef ViewDataTrait<ControlEditTextSelectionRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextPasswordTag>                { typedef ViewDataTrait<char*>                          Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextKeyScriptBehaviorTag>       { typedef ViewDataTrait<ControlKeyScriptBehavior>       Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextLockedTag>                  { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextFixedTextTag>               { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextValidationProcTag>          { typedef ViewDataTrait<ControlEditTextValidationUPP>   Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextCFStringTag>                { typedef ViewDataTrait<CFStringRef>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextPasswordCFStringTag>        { typedef ViewDataTrait<CFStringRef>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditUnicodeTextPostUpdateProcTag>   { typedef ViewDataTrait<EditUnicodePostUpdateUPP>       Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindEditUnicodeText, kControlEditTextSingleLineTag>              { typedef ViewDataTrait<Boolean>                        Trait;  };

// scroll view

template <> struct ViewDataDef<kControlKindSignatureApple, kEventClassScrollable, kControlScrollViewVerticalInset>                  { typedef ViewDataTrait<float>  Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kEventClassScrollable, kControlScrollViewHorizontalInset>                { typedef ViewDataTrait<float>  Trait;  };

// combo box

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHIComboBox, kControlEditTextSelectionTag>                    { typedef ViewDataTrait<ControlEditTextSelectionRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHIComboBox, kControlEditTextKeyScriptBehaviorTag>            { typedef ViewDataTrait<ControlKeyScriptBehavior>       Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHIComboBox, kControlEditTextFixedTextTag>                    { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHIComboBox, kControlEditTextValidationProcTag>               { typedef ViewDataTrait<ControlEditTextValidationUPP>   Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHIComboBox, kControlEditTextCFStringTag>                     { typedef ViewDataTrait<CFStringRef>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHIComboBox, kControlEditUnicodeTextPostUpdateProcTag>        { typedef ViewDataTrait<EditUnicodePostUpdateUPP>       Trait;  };

// text view

template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHITextView, kControlEditTextSelectionTag>                    { typedef ViewDataTrait<ControlEditTextSelectionRec>    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHITextView, kControlEditTextKeyScriptBehaviorTag>            { typedef ViewDataTrait<ControlKeyScriptBehavior>       Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHITextView, kControlEditTextFixedTextTag>                    { typedef ViewDataTrait<Boolean>                        Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHITextView, kControlEditTextValidationProcTag>               { typedef ViewDataTrait<ControlEditTextValidationUPP>   Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHITextView, kControlEditTextCFStringTag>                     { typedef ViewDataTrait<CFStringRef>                    Trait;  };
template <> struct ViewDataDef<kControlKindSignatureApple, kControlKindHITextView, kControlEditUnicodeTextPostUpdateProcTag>        { typedef ViewDataTrait<EditUnicodePostUpdateUPP>       Trait;  };

#endif  // DOXYGEN_SKIP


// ==========================================================================================
//  ViewData

/*!
    @brief  Gets & sets a data tag for a specific control class in a type-safe manner.
    
    ViewData utilises ViewDataDef to actually retrieve & store the data.  The 
    particular instantiation of ViewDataDef is determined by the three template 
    parameters.  See ViewDataDef for more information.
    
    @ingroup    ViewGroup
*/
template <OSType SIG, OSType KIND, ResType TAG> struct ViewData
{
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef OSType  SIG;    //!< Template parameter.  See ViewDataDef.
    typedef OSType  KIND;   //!< Template parameter.  See ViewDataDef.
    typedef ResType TAG;    //!< Template parameter.  See ViewDataDef.
    //@}
#endif

    //! @name Types
    //@{
    typedef typename ViewDataDef<SIG, KIND, TAG>::Trait Trait;  //!< The view data traits.  See ViewDataDef.
    typedef typename Trait::Type                        Type;   //!< The language type.  See ViewDataTrait.
    //@}
    
    /*! @name View Access
        Allows access to the data for a given view, part, and tag.  Clients will 
        use these functions to interact with the view.
    */
    //@{
    //! Retrieves the view data for the given view and part, returning it as the function result.
    static Type Get(
                    HIViewRef       view, 
                    ControlPartCode part = kHIViewEntireView);
    //! Retrieves the view data for the given view and part, returning it in @a value.
    static void Get(
                    HIViewRef       view, 
                    Type&           value, 
                    ControlPartCode part = kHIViewEntireView);
    //! Retrieves the view data for the given view and part, returning it in @a value and without throwing an exception.
    static bool Get(
                    HIViewRef       view, 
                    Type&           value, 
                    const std::nothrow_t&, 
                    ControlPartCode part = kHIViewEntireView);
    //! Changes the view data for the given view and part.
    static void Set(
                    HIViewRef       view, 
                    const Type&     value, 
                    ControlPartCode part = kHIViewEntireView);
    //@}
    
    /*! @name Memory Access
        Allows access to the data at a given memory address.  Implementations 
        of custom controls will use these functions to read and write the data 
        to/from Carbon %Events.
    */
    //@{
    //! Retrieves the view data from an arbitrary address in memory, returning it as the function result.
    static Type Get(
                    const void*     data,
                    size_t          size);
    //! Retrieves the view data from an arbitrary address in memory, returning it in @a value.
    static void Get(
                    const void*     data,
                    size_t          size,
                    Type&           value);
    //! Retrieves the view data from an arbitrary address in memory, returning it in @a value and without throwing an exception.
    static bool Get(
                    const void*     data,
                    size_t          size,
                    Type&           value,
                    const std::nothrow_t&);
    //! Copies the view data to an arbitrary address in memory.
    static void Set(
                    void*           data,
                    size_t          size,
                    const Type&     value);
    //@}
};

// ------------------------------------------------------------------------------------------
/*! @return     The tag's value.
*/
template <OSType SIG, OSType KIND, ResType TAG> typename ViewData<SIG, KIND, TAG>::Type
ViewData<SIG, KIND, TAG>::Get(
    HIViewRef       view,                           //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part /* = kHIViewEntireView */) //!< The part code.
{
    Type    value;
    
    Trait::Get(view, part, TAG, value);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType KIND, ResType TAG> inline void
ViewData<SIG, KIND, TAG>::Get(
    HIViewRef       view,                           //!< The view (HIViewRef a.k.a. ControlRef).
    Type&           value,                          //!< Holds the output.
    ControlPartCode part /* = kHIViewEntireView */) //!< The part code.
{
    Trait::Get(view, part, TAG, value);
}

// ------------------------------------------------------------------------------------------
/*! @return     Boolean indication of sucess or failure.
    @exception  none
*/
template <OSType SIG, OSType KIND, ResType TAG> bool
ViewData<SIG, KIND, TAG>::Get(
    HIViewRef               view,                           //!< The view (HIViewRef a.k.a. ControlRef).
    Type&                   value,                          //!< Holds the output.
    const std::nothrow_t&   nt,                             //!< An indication that the caller doesn't want to have the function throw an exception.
    ControlPartCode         part /* = kHIViewEntireView */) //!< The part code.
{
    OSStatus    err;
    
    err = Trait::Get(view, part, TAG, value, nt);
    
    return (err == noErr);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType KIND, ResType TAG> inline void
ViewData<SIG, KIND, TAG>::Set(
    HIViewRef       view,                           //!< The view (HIViewRef a.k.a. ControlRef).
    const Type&     value,                          //!< The new value.
    ControlPartCode part /* = kHIViewEntireView */) //!< The part code.
{
    Trait::Set(view, part, TAG, value);
}

// ------------------------------------------------------------------------------------------
/*! Note that the size is checked to ensure it is equal to sizeof(Type).
    
    @return     The tag's value.
*/
template <OSType SIG, OSType KIND, ResType TAG> typename ViewData<SIG, KIND, TAG>::Type
ViewData<SIG, KIND, TAG>::Get(
    const void*     data,   //!< The source address of the view data.
    size_t          size)   //!< The size of the view data.
{
    Type        value;
    OSStatus    err;
    
    err = Trait::Get(data, size, value);
    B_THROW_IF_STATUS(err);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
/*! Note that the size is checked to ensure it is equal to sizeof(Type).
*/
template <OSType SIG, OSType KIND, ResType TAG> void
ViewData<SIG, KIND, TAG>::Get(
    const void* data,   //!< The source address of the view data.
    size_t      size,   //!< The size of the view data.
    Type&       value)  //!< Holds the output value.
{
    OSStatus    err;
    
    err = Trait::Get(data, size, value);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! Note that the size is checked to ensure it is equal to sizeof(Type).
    
    @return     Boolean indication of sucess or failure.
    @exception  none
*/
template <OSType SIG, OSType KIND, ResType TAG> bool
ViewData<SIG, KIND, TAG>::Get(
    const void* data,       //!< The source address of the view data.
    size_t      size,       //!< The size of the view data.
    Type&       value,      //!< Holds the output value.
    const std::nothrow_t&)  //!< An indication that the caller doesn't want to have the function throw an exception.
{
    OSStatus    err;
    
    err = Trait::Get(data, size, value);
    
    return (err == noErr);
}

// ------------------------------------------------------------------------------------------
/*! Note that the size is checked to ensure it is equal to sizeof(Type).
*/
template <OSType SIG, OSType KIND, ResType TAG> void
ViewData<SIG, KIND, TAG>::Set(
    void*       data,   //!< The destination address of the view data
    size_t      size,   //!< The size of the view data
    const Type& value)  //!< The new value.
{
    OSStatus    err;
    
    err = Trait::Set(data, size, value);
    B_THROW_IF_STATUS(err);
}

}   // namespace B


#endif  // BViewData_H_
