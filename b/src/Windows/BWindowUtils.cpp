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
#include "BWindowUtils.h"


namespace B {

namespace WindowUtils {

// ------------------------------------------------------------------------------------------
bool
IsComposited(
    WindowRef   inWindowRef)    //!< The window.
{
    return ((GetAttributes(inWindowRef) & kWindowCompositingAttribute) != 0);
}

// ------------------------------------------------------------------------------------------
bool
IsMetal(
    WindowRef   inWindowRef)    //!< The window.
{
    return ((GetAttributes(inWindowRef) & kWindowMetalAttribute) != 0);
}

// ------------------------------------------------------------------------------------------
WindowClass
GetClass(
    WindowRef   inWindowRef)    //!< The window.
{
    B_ASSERT(inWindowRef != NULL);
    
    WindowClass windowClass;
    OSStatus    err;
    
    err = GetWindowClass(inWindowRef, &windowClass);
    B_THROW_IF_STATUS(err);
    
    return (windowClass);
}

// ------------------------------------------------------------------------------------------
WindowAttributes
GetAttributes(
    WindowRef   inWindowRef)    //!< The window.
{
    B_ASSERT(inWindowRef != NULL);
    
    WindowAttributes    attributes;
    OSStatus            err;
    
    err = GetWindowAttributes(inWindowRef, &attributes);
    B_THROW_IF_STATUS(err);
    
    return (attributes);
}

// ------------------------------------------------------------------------------------------
WindowModality
GetModality(
    WindowRef   inWindowRef)    //!< The window.
{
    B_ASSERT(inWindowRef != NULL);
    
    WindowModality  modality;
    OSStatus        err;
    
    err = GetWindowModality(inWindowRef, &modality, NULL);
    B_THROW_IF_STATUS(err);
    
    return (modality);
}

// ------------------------------------------------------------------------------------------
/*! In a compositing window, the function returns the <em>bona fide</em> content view.  In 
    a non-compositing window, it returns the root control.
*/
HIViewRef
GetContentView(
    WindowRef   inWindowRef)    //!< The window.
{
    return (GetContentView(inWindowRef, IsComposited(inWindowRef)));
}

// ------------------------------------------------------------------------------------------
/*! Use this variant when you know already whether @a inWindowRef is a compositing window.
*/
HIViewRef   GetContentView(
    WindowRef   inWindowRef,    //!< The window.
    bool        inComposited)   //!< Is this a compositing window?
{
    HIViewRef   content;
    OSStatus    err;
    
    if (inComposited)
    {
        err = HIViewFindByID(HIViewGetRoot(inWindowRef), kHIViewWindowContentID, &content);
        B_THROW_IF_STATUS(err);
    }
    else
    {
        err = GetRootControl(inWindowRef, &content);
        B_THROW_IF_STATUS(err);
    }
    
    return (content);
}

// ------------------------------------------------------------------------------------------
/*! In a compositing window, the function returns the bounds of the <em>bona fide</em> 
    content view.  In a non-compositing window, it returns the bounds of the root control.
*/
Rect
GetContentViewBounds(
    WindowRef   inWindowRef)    //!< The window.
{
    return (GetContentViewBounds(inWindowRef, IsComposited(inWindowRef)));
}

// ------------------------------------------------------------------------------------------
/*! Use this variant when you know already whether @a inWindowRef is a compositing window.
*/
Rect
GetContentViewBounds(
    WindowRef   inWindowRef,    //!< The window.
    bool        inComposited)   //!< Is this a compositing window?
{
    B_ASSERT(inWindowRef != NULL);
    
    Rect    bounds;
    
    if (inComposited)
    {
        OSStatus    err;
        
        err = HIViewGetBounds(GetContentView(inWindowRef, inComposited), &bounds);
        B_THROW_IF_STATUS(err);
    }
    else
    {
        ::Rect  qdRect;
        
        GetWindowPortBounds(inWindowRef, &qdRect);
        
        bounds = qdRect;
    }
    
    return bounds;
}

// ------------------------------------------------------------------------------------------
Rect
GetContentViewBounds(
    WindowRef   inWindowRef,    //!< The window.
    bool        inComposited,   //!< Is this a compositing window?
    HIViewRef   inContentView)
{
    B_ASSERT(inWindowRef != NULL);
    
    Rect    bounds;

    if (inComposited)
    {
        OSStatus    err;
        
        err = HIViewGetBounds(inContentView, &bounds);
        B_THROW_IF_STATUS(err);
    }
    else
    {
        ::Rect  qdRect;
        
        GetWindowPortBounds(inWindowRef, &qdRect);
        
        bounds = qdRect;
    }
    
    return bounds;
}

// ------------------------------------------------------------------------------------------
Rect
GetStructureBounds(
    WindowRef   inWindowRef)    //!< The window.
{
    B_ASSERT(inWindowRef != NULL);
    
    ::Rect      qdRect;
    OSStatus    err;
    
    err = GetWindowBounds(inWindowRef, kWindowStructureRgn, &qdRect);
    B_THROW_IF_STATUS(err);
    
    return Rect(qdRect);
}

// ------------------------------------------------------------------------------------------
Point
GetStructureOrigin(
    WindowRef   inWindowRef)    //!< The window.
{
    return GetStructureBounds(inWindowRef).origin;
}

// ------------------------------------------------------------------------------------------
Size
GetStructureSize(
    WindowRef   inWindowRef)    //!< The window.
{
    return GetStructureBounds(inWindowRef).size;
}

// ------------------------------------------------------------------------------------------
Rect
GetContentBounds(
    WindowRef   inWindowRef)    //!< The window.
{
    B_ASSERT(inWindowRef != NULL);
    
    ::Rect      qdRect;
    OSStatus    err;
    
    err = GetWindowBounds(inWindowRef, kWindowContentRgn, &qdRect);
    B_THROW_IF_STATUS(err);
    
    return Rect(qdRect);
}

// ------------------------------------------------------------------------------------------
Point
GetContentOrigin(
    WindowRef   inWindowRef)    //!< The window.
{
    return GetContentBounds(inWindowRef).origin;
}

// ------------------------------------------------------------------------------------------
Size
GetContentSize(
    WindowRef   inWindowRef)    //!< The window.
{
    return GetContentBounds(inWindowRef).size;
}

// ------------------------------------------------------------------------------------------
CGPoint
ConvertFromWindowToLocal(
    const CGPoint&  inPoint,        //!< The point to convert.
    WindowRef       inWindowRef)    //!< The window in whose coordinates is @ ioPoint.
{
    B_ASSERT(inWindowRef != NULL);
    
    CGPoint     point   = inPoint;
    OSStatus    err;
    
    err = HIViewConvertPoint(&point, NULL, GetContentView(inWindowRef));
    B_THROW_IF_STATUS(err);
    
    return point;
}

// ------------------------------------------------------------------------------------------
CGRect
ConvertFromWindowToLocal(
    const CGRect&   inRect,         //!< The rect to convert.
    WindowRef       inWindowRef)    //!< The window in whose coordinates is @ ioPoint.
{
    B_ASSERT(inWindowRef != NULL);
    
    CGRect      rect    = inRect;
    OSStatus    err;
    
    err = HIViewConvertRect(&rect, NULL, GetContentView(inWindowRef));
    B_THROW_IF_STATUS(err);
    
    return rect;
}

// ------------------------------------------------------------------------------------------
CGPoint
ConvertFromLocalToWindow(
    const CGPoint&  inPoint,        //!< The point to convert.
    WindowRef       inWindowRef)    //!< The window in whose coordinates is @ ioPoint.
{
    CGPoint     point   = inPoint;
    OSStatus    err;
    
    err = HIViewConvertPoint(&point, GetContentView(inWindowRef), NULL);
    B_THROW_IF_STATUS(err);
    
    return point;
}

// ------------------------------------------------------------------------------------------
CGRect
ConvertFromLocalToWindow(
    const CGRect&   inRect,         //!< The rect to convert.
    WindowRef       inWindowRef)    //!< The window in whose coordinates is @ ioPoint.
{
    CGRect      rect    = inRect;
    OSStatus    err;
    
    err = HIViewConvertRect(&rect, GetContentView(inWindowRef), NULL);
    B_THROW_IF_STATUS(err);
    
    return rect;
}

}   // namespace WindowUtils

}   // namespace B
