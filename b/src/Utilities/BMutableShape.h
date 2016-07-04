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

#ifndef BMutableShape_H_
#define BMutableShape_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BAutoRegion.h"
#include "BOSPtr.h"
#include "BShape.h"


namespace B {

// forward declarations
class   Shape;
class   Point;
class   Rect;
class   Size;


/*! @brief  Wrapper class for <tt>HIMutableShapeRef</tt>.
    
    @todo   %Document this class!
*/
class MutableShape
{
public:
    
    // constructors / destructor
                MutableShape();
                MutableShape(const MutableShape& inShape);
    explicit    MutableShape(const Shape& inShape);
    explicit    MutableShape(RgnHandle inRegion);
    explicit    MutableShape(const Rect& inRect);
    explicit    MutableShape(const ::Rect& inRect);
    explicit    MutableShape(HIShapeRef inShape);
    explicit    MutableShape(const OSPtr<HIShapeRef>& inShape);
    explicit    MutableShape(const OSPtr<HIMutableShapeRef>& inShape);
                ~MutableShape();
    
    // inquiries
    bool        empty() const;
    bool        operator ! () const;
    bool        rectangular() const;
    Rect        bounds() const;
    
    // modifiers
    void        clear();
    void        swap(MutableShape& shape);
    
    // assignment
    MutableShape&   operator = (const MutableShape &inShape);
    MutableShape&   operator = (const Shape& inShape);
    MutableShape&   operator = (RgnHandle inRegion);
    MutableShape&   operator = (const Rect& inRect);
    MutableShape&   operator = (const OSPtr<HIShapeRef>& inShapeRef);
    MutableShape&   operator = (const OSPtr<HIMutableShapeRef>& inShapeRef);
    
    // conversions
    HIShapeRef          cf_ref() const          { return (mShapeRef); }
    OSPtr<HIShapeRef>   cf_ptr() const          { return (OSPtr<HIShapeRef>(mShapeRef)); }
    AutoRegion          GetRegion() const;
    
    // union
    MutableShape&   operator |= (const MutableShape& inShape);
    MutableShape&   operator |= (const Shape& inShape);
    
    // intersection
    MutableShape&   operator &= (const MutableShape& inShape);
    MutableShape&   operator &= (const Shape& inShape);
    
    // difference
    MutableShape&   operator -= (const MutableShape& inShape);
    MutableShape&   operator -= (const Shape& inShape);
    
    // exclusive-or
    MutableShape&   operator ^= (const MutableShape& inShape);
    MutableShape&   operator ^= (const Shape& inShape);
    
    // translation
    void            offset(short deltaX, short deltaY);
    MutableShape&   operator += (const Point& inPoint);
    MutableShape&   operator -= (const Point& inPoint);
    
    // hit-testing
    bool    contains(const Point& inPoint) const;
    bool    contains(::Point inPoint) const;
    bool    intersects(const Rect& inRect) const;

private:
    
    // member variables
    HIMutableShapeRef   mShapeRef;
};

// ------------------------------------------------------------------------------------------
inline
MutableShape::~MutableShape()
{
    CFRelease(mShapeRef);
}

// ------------------------------------------------------------------------------------------
inline bool
MutableShape::empty() const
{
    return (HIShapeIsEmpty(mShapeRef));
}

// ------------------------------------------------------------------------------------------
inline bool
MutableShape::operator ! () const
{
    return (!HIShapeIsEmpty(mShapeRef));
}

// ------------------------------------------------------------------------------------------
inline bool
MutableShape::rectangular() const
{
    return (HIShapeIsRectangular(mShapeRef));
}

// ------------------------------------------------------------------------------------------
inline void
MutableShape::swap(MutableShape& shape)
{
    std::swap(mShapeRef, shape.mShapeRef);
}

// ------------------------------------------------------------------------------------------
inline void
swap(MutableShape& shape1, MutableShape& shape2)
{
    shape1.swap(shape2);
}


}   // namespace B

#endif  // BMutableShape_H_
