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
#include "BRect.h"


namespace B {


// ==========================================================================================
//  Point

// ------------------------------------------------------------------------------------------
::Point
Point::QDPoint(const CGPoint inPoint)
{
    ::Point pt  = { static_cast<short>(inPoint.y), static_cast<short>(inPoint.x) };
    
    return (pt);
}

#ifndef NDEBUG
// ------------------------------------------------------------------------------------------
void
Point::DebugPrint(const char* prefix /* = NULL */) const
{
    printf("%s%s[%g %g]\n", 
           (prefix != NULL) ? prefix : "",
           (prefix != NULL) ? " " : "",
           x, y);
}
#endif


// ==========================================================================================
//  Size

#pragma mark -

// ------------------------------------------------------------------------------------------
::Point
Size::QDPoint(const CGSize inSize)
{
    ::Point pt  = { static_cast<short>(inSize.height), static_cast<short>(inSize.width) };
    
    return (pt);
}

#ifndef NDEBUG
// ------------------------------------------------------------------------------------------
void
Size::DebugPrint(const char* prefix /* = NULL */) const
{
    printf("%s%s[%g %g]\n", 
           (prefix != NULL) ? prefix : "",
           (prefix != NULL) ? " " : "",
           width, height);
}
#endif


// ==========================================================================================
//  Rect

#pragma mark -

// ------------------------------------------------------------------------------------------
Rect::Rect(
    const ::Rect&   inRect)
{
    static_cast<CGRect&>(*this) = CGRectMake(inRect.left, 
                                             inRect.top, 
                                             inRect.right  - inRect.left, 
                                             inRect.bottom - inRect.top);
}

// ------------------------------------------------------------------------------------------
Rect::Rect(
    float           inX,
    float           inY,
    float           inWidth,
    float           inHeight)
{
    static_cast<CGRect&>(*this) = CGRectMake(inX, inY, inWidth, inHeight);
}

// ------------------------------------------------------------------------------------------
void
Rect::center(const CGPoint c)
{
    static_cast<CGRect&>(*this) = CGRectMake(c.x - this->size.width / 2.0f, 
                                             c.y - this->size.height / 2.0f, 
                                             this->size.width, 
                                             this->size.height);
}

// ------------------------------------------------------------------------------------------
void
Rect::center(const CGPoint c, const CGSize s)
{
    static_cast<CGRect&>(*this) = CGRectMake(c.x - s.width/2.0f, 
                                             c.y - s.height/2.0f, 
                                             s.width, 
                                             s.height);
}

// ------------------------------------------------------------------------------------------
void
Rect::frame(const CGPoint p1, const CGPoint p2)
{
    static_cast<CGRect&>(*this) = CGRectStandardize(CGRectMake(
                                    p1.x, p1.y, p2.x - p1.x, p2.y - p1.y));
}

// ------------------------------------------------------------------------------------------
::Rect
Rect::QDRect(const CGRect inRect)
{
    ::Rect  qdRect;
    
    qdRect.top      = static_cast<short>(CGRectGetMinY(inRect));
    qdRect.left     = static_cast<short>(CGRectGetMinX(inRect));
    qdRect.bottom   = static_cast<short>(CGRectGetMaxY(inRect));
    qdRect.right    = static_cast<short>(CGRectGetMaxX(inRect));
    
    return qdRect;
}

// ------------------------------------------------------------------------------------------
Rect&
Rect::operator = (const ::Rect inRect)
{
    static_cast<CGRect&>(*this) = CGRectMake(inRect.left, 
                                             inRect.top, 
                                             inRect.right  - inRect.left, 
                                             inRect.bottom - inRect.top);
    
    return (*this);
}

#ifndef NDEBUG
// ------------------------------------------------------------------------------------------
void
Rect::DebugPrint(const char* prefix /* = NULL */) const
{
    printf("%s%s[%g %g %g %g]\n", 
           (prefix != NULL) ? prefix : "",
           (prefix != NULL) ? " " : "",
           origin.x, origin.y, size.width, size.height);
}
#endif


}   // namespace B
