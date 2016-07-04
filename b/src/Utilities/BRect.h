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

#ifndef BRect_H_
#define BRect_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>


namespace B {


// ==========================================================================================
//  Point

#pragma mark Point

/*! @brief  Wrapper class around an <tt>CGPoint</tt>
    
    @todo   %Document this class!
*/
class Point : public CGPoint
{
public:
    
    // constructors / destructor
                Point() {}
                Point(const CGPoint& inPoint);
    explicit    Point(::Point inPoint);
    explicit    Point(float inX, float inY);
    
    // conversions
    operator        ::Point () const;
    static ::Point  QDPoint(const CGPoint inPoint);
    
    // assignment
    Point&      operator = (const CGPoint& inPoint);
    Point&      operator = (::Point inPoint);
    
    // translation
    void        moveby(float deltaX, float deltaY);
    void        moveto(float x, float y);
    Point&      operator += (CGPoint inPoint);
    Point&      operator -= (CGPoint inPoint);
    Point&      operator += (float delta);
    Point&      operator -= (float delta);
    
    // debugging
#ifndef NDEBUG
    void        DebugPrint(const char* prefix = NULL) const;
#endif
};

// ------------------------------------------------------------------------------------------
inline
Point::Point(const CGPoint& inPoint)
{
    static_cast<CGPoint&>(*this) = inPoint;
}

// ------------------------------------------------------------------------------------------
inline
Point::Point(const ::Point inPoint)
{
    this->x = inPoint.h;
    this->y = inPoint.v;
}

// ------------------------------------------------------------------------------------------
inline
Point::Point(float inX, float inY)
{
    this->x = inX;
    this->y = inY;
}

// ------------------------------------------------------------------------------------------
inline
Point::operator ::Point () const
{
    ::Point pt  = { static_cast<short>(y), static_cast<short>(x) };
    return (pt);
}

// ------------------------------------------------------------------------------------------
inline Point&
Point::operator = (const CGPoint& inPoint)
{
    static_cast<CGPoint&>(*this) = inPoint;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Point&
Point::operator = (const ::Point inPoint)
{
    this->x = inPoint.h;
    this->y = inPoint.v;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
Point::moveby(float deltaX, float deltaY)
{
    this->x += deltaX;
    this->y += deltaY;
}

// ------------------------------------------------------------------------------------------
inline void
Point::moveto(float newX, float newY)
{
    this->x = newX;
    this->y = newY;
}

// ------------------------------------------------------------------------------------------
inline Point&
Point::operator += (const CGPoint inPoint)
{
    this->x += inPoint.x;
    this->y += inPoint.y;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Point&
Point::operator -= (const CGPoint inPoint)
{
    this->x -= inPoint.x;
    this->y -= inPoint.y;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Point&
Point::operator += (float delta)
{
    this->x += delta;
    this->y += delta;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Point&
Point::operator -= (float delta)
{
    this->x -= delta;
    this->y -= delta;
    return (*this);
}

}   // namespace B


// ------------------------------------------------------------------------------------------
/*! Compares two Point objects for equality.
    
    @return     @c true if @a inLhs is equal to @a inRhs
    @relates    Point
*/
inline bool operator == (const CGPoint& inLhs, const CGPoint& inRhs){ return (CGPointEqualToPoint(inLhs, inRhs)); }

// ------------------------------------------------------------------------------------------
/*! Compares two Point objects for inequality.
    
    @return     @c true if @a inLhs is not equal to @a inRhs
    @relates    Point
*/
inline bool operator != (const CGPoint& inLhs, const CGPoint& inRhs){ return (!CGPointEqualToPoint(inLhs, inRhs)); }

// ------------------------------------------------------------------------------------------
inline CGPoint  operator + (const CGPoint inLhs, const CGPoint inRhs)
{
    return CGPointMake(inLhs.x + inRhs.x, inLhs.y + inRhs.y);
}

// ------------------------------------------------------------------------------------------
inline CGPoint  operator - (const CGPoint inLhs, const CGPoint inRhs)
{
    return CGPointMake(inLhs.x - inRhs.x, inLhs.y - inRhs.y);
}

// ------------------------------------------------------------------------------------------
inline CGPoint  operator + (const CGPoint inLhs, float delta)
{
    return CGPointMake(inLhs.x + delta, inLhs.y + delta);
}

// ------------------------------------------------------------------------------------------
inline CGPoint  operator - (const CGPoint inLhs, float delta)
{
    return CGPointMake(inLhs.x - delta, inLhs.y - delta);
}

// ------------------------------------------------------------------------------------------
/*! Negates the coordinates of a Point object.
    
    @relates    Point
*/
inline CGPoint  operator - (const CGPoint inPt)                     { return CGPointMake(-inPt.x, -inPt.y); }


// ==========================================================================================
//  Size

#pragma mark -
#pragma mark Size

namespace B {

/*! @brief  Wrapper class around an <tt>CGSize</tt>
    
    @todo   %Document this class!
*/
class Size : public CGSize
{
public:
    
    // constructors / destructor
                Size() {}
                Size(const CGSize& inSize);
    explicit    Size(::Point inPoint);
    explicit    Size(float inWidth, float inHeight);
    
    // conversions
    operator        ::Point () const;
    static ::Point  QDPoint(const CGSize inSize);
    
    // assignment
    Size&       operator = (const CGSize& inSize);
    Size&       operator = (::Point inPoint);
    
    // scaling
    Size&       operator += (CGSize inSize);
    Size&       operator -= (CGSize inSize);
    Size&       operator *= (CGSize inSize);
    Size&       operator /= (CGSize inSize);
    Size&       operator += (float f);
    Size&       operator -= (float f);
    Size&       operator *= (float f);
    Size&       operator /= (float f);
    
    // debugging
#ifndef NDEBUG
    void        DebugPrint(const char* prefix = NULL) const;
#endif
};

// ------------------------------------------------------------------------------------------
inline
Size::Size(const CGSize& inSize)
{
    static_cast<CGSize&>(*this) = inSize;
}

// ------------------------------------------------------------------------------------------
inline
Size::Size(const ::Point inPoint)
{
    this->width = inPoint.h;
    this->height = inPoint.v;
}

// ------------------------------------------------------------------------------------------
inline
Size::Size(float inWidth, float inHeight)
{
    this->width = inWidth;
    this->height = inHeight;
}

// ------------------------------------------------------------------------------------------
inline
Size::operator ::Point () const
{
    ::Point pt  = { static_cast<short>(this->height), static_cast<short>(this->width) };
    return (pt);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator = (const CGSize& inSize)
{
    static_cast<CGSize&>(*this) = inSize;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator = (::Point inPoint)
{
    this->width = inPoint.h;
    this->height = inPoint.v;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator += (const CGSize inSize)
{
    this->width  += inSize.width;
    this->height += inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator -= (const CGSize inSize)
{
    this->width  -= inSize.width;
    this->height -= inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator *= (const CGSize inSize)
{
    this->width  *= inSize.width;
    this->height *= inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator /= (const CGSize inSize)
{
    this->width  /= inSize.width;
    this->height /= inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator += (float f)
{
    this->width  += f;
    this->height += f;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator -= (float f)
{
    this->width  -= f;
    this->height -= f;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator *= (float f)
{
    this->width  *= f;
    this->height *= f;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Size&
Size::operator /= (float f)
{
    this->width  /= f;
    this->height /= f;
    return (*this);
}

}   // namespace B

// ------------------------------------------------------------------------------------------
/*! Compares two Size objects for equality.
    
    @return     @c true if @a inLhs is equal to @a inRhs
    @relates    Size
*/
inline bool operator == (const CGSize& inLhs, const CGSize& inRhs)  { return (CGSizeEqualToSize(inLhs, inRhs)); }

// ------------------------------------------------------------------------------------------
/*! Compares two Size objects for inequality.
    
    @return     @c true if @a inLhs is not equal to @a inRhs
    @relates    Size
*/
inline bool operator != (const CGSize& inLhs, const CGSize& inRhs)  { return (!CGSizeEqualToSize(inLhs, inRhs)); }

// ------------------------------------------------------------------------------------------
inline CGSize   operator + (const CGSize size, float delta)
{
    return CGSizeMake(size.width + delta, size.height + delta);
}

// ------------------------------------------------------------------------------------------
inline CGSize   operator - (const CGSize size, float delta)
{
    return CGSizeMake(size.width - delta, size.height - delta);
}

// ------------------------------------------------------------------------------------------
inline CGSize   operator * (const CGSize size, float delta)
{
    return CGSizeMake(size.width * delta, size.height * delta);
}

// ------------------------------------------------------------------------------------------
inline CGSize   operator / (const CGSize size, float delta)
{
    return CGSizeMake(size.width / delta, size.height / delta);
}


// ==========================================================================================
//  Rect

#pragma mark -
#pragma mark Rect

namespace B {

/*! @brief  Wrapper class around an <tt>CGRect</tt>
    
    @todo   %Document this class!
*/
class Rect : public CGRect
{
public:
    
    // constructors / destructor
                Rect() {}
                Rect(
                    const CGRect&   inRect);
    explicit    Rect(
                    const CGPoint&  inOrigin,
                    const CGSize&   inSize);
    explicit    Rect(
                    const ::Rect&   inRect);
    explicit    Rect(
                    float           inX,
                    float           inY,
                    float           inWidth,
                    float           inHeight);
    
    // inquiries
    float       minx() const;
    float       maxx() const;
    float       miny() const;
    float       maxy() const;
    float       width() const;
    float       height() const;
    float       centerx() const;
    float       centery() const;
    CGPoint     center() const;
    bool        null() const;
    bool        empty() const;
    bool        operator ! () const;
    
    // modifiers
    void        width(float w);
    void        height(float h);
    void        center(CGPoint c);
    void        center(CGPoint c, CGSize s);
    void        frame(CGPoint p1, CGPoint p2);
    void        standardize();
    void        integral();
    
    // conversions
    operator        ::Rect () const;
    static ::Rect   QDRect(CGRect inRect);
    
    // assignment
    Rect&       operator = (const CGRect& inRect);
    Rect&       operator = (::Rect inRect);
    
    // hit-testing
    bool        contains(const CGPoint& inPoint) const;
    bool        contains(const CGRect& inRect) const;
    bool        intersects(const CGRect& inRect) const;
    
    // translation
    void        moveby(float deltaX, float deltaY);
    void        moveto(float x, float y);
    Rect&       operator  = (const CGPoint& inPoint);
    Rect&       operator += (CGPoint inPoint);
    Rect&       operator -= (CGPoint inPoint);
    
    // scaling
    void        inset(float delta);
    void        inset(float deltaX, float deltaY);
    void        outset(float delta);
    void        outset(float deltaX, float deltaY);
    Rect&       operator  = (const CGSize& inSize);
    Rect&       operator += (CGSize inSize);
    Rect&       operator -= (CGSize inSize);
    Rect&       operator *= (CGSize inSize);
    Rect&       operator /= (CGSize inSize);
    
    // union
    Rect&       operator |= (const CGRect& inRect);
    
    // intersection
    Rect&       operator &= (const CGRect& inRect);
    
    // debugging
#ifndef NDEBUG
    void        DebugPrint(const char* prefix = NULL) const;
#endif
};

// ------------------------------------------------------------------------------------------
inline
Rect::Rect(
    const CGRect&   inRect)
{
    static_cast<CGRect&>(*this) = inRect;
}

// ------------------------------------------------------------------------------------------
inline
Rect::Rect(
    const CGPoint&  inOrigin,
    const CGSize&   inSize)
{
    this->origin    = inOrigin;
    this->size      = inSize;
}

// ------------------------------------------------------------------------------------------
inline float    Rect::minx() const      { return (CGRectGetMinX(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::maxx() const      { return (CGRectGetMaxX(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::miny() const      { return (CGRectGetMinY(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::maxy() const      { return (CGRectGetMaxY(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::width() const     { return (CGRectGetWidth(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::height() const    { return (CGRectGetHeight(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::centerx() const   { return (CGRectGetMidX(*this)); }

// ------------------------------------------------------------------------------------------
inline float    Rect::centery() const   { return (CGRectGetMidY(*this)); }

// ------------------------------------------------------------------------------------------
inline CGPoint  Rect::center() const    { return (CGPointMake(CGRectGetMidX(*this), CGRectGetMidY(*this))); }

// ------------------------------------------------------------------------------------------
inline bool     Rect::null() const      { return (CGRectIsNull(*this)); }

// ------------------------------------------------------------------------------------------
inline bool     Rect::empty() const     { return (CGRectIsEmpty(*this)); }

// ------------------------------------------------------------------------------------------
inline bool     Rect::operator!() const { return (!CGRectIsEmpty(*this)); }

// ------------------------------------------------------------------------------------------
inline void     Rect::width(float w)    { this->size.width  = w; }

// ------------------------------------------------------------------------------------------
inline void     Rect::height(float h)   { this->size.height = h; }

// ------------------------------------------------------------------------------------------
inline void
Rect::standardize()
{
    static_cast<CGRect&>(*this) = CGRectStandardize(*this);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::integral()
{
    static_cast<CGRect&>(*this) = CGRectIntegral(*this);
}

// ------------------------------------------------------------------------------------------
inline bool
Rect::contains(const CGPoint& inPoint) const
{
    return (CGRectContainsPoint(*this, inPoint));
}

// ------------------------------------------------------------------------------------------
inline bool
Rect::contains(const CGRect& inRect) const
{
    return (CGRectContainsRect(*this, inRect));
}

// ------------------------------------------------------------------------------------------
inline bool
Rect::intersects(const CGRect& inRect) const
{
    return (CGRectIntersectsRect(*this, inRect));
}

// ------------------------------------------------------------------------------------------
inline
Rect::operator ::Rect () const
{
    return (QDRect(*this));
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator = (const CGRect& inRect)
{
    static_cast<CGRect&>(*this) = inRect;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::moveby(float deltaX, float deltaY)
{
    this->origin = CGPointMake(minx() + deltaX, miny() + deltaY);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::moveto(float x, float y)
{
    this->origin = CGPointMake(x, y);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator = (const CGPoint& inPoint)
{
    this->origin = inPoint;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator += (const CGPoint inPoint)
{
    this->origin = CGPointMake(minx() + inPoint.x, miny() + inPoint.y);
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator -= (const CGPoint inPoint)
{
    this->origin = CGPointMake(minx() - inPoint.x, miny() - inPoint.y);
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::inset(float delta)
{
    static_cast<CGRect&>(*this) = CGRectInset(*this, delta, delta);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::inset(float deltaX, float deltaY)
{
    static_cast<CGRect&>(*this) = CGRectInset(*this, deltaX, deltaY);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::outset(float delta)
{
    static_cast<CGRect&>(*this) = CGRectInset(*this, -delta, -delta);
}

// ------------------------------------------------------------------------------------------
inline void
Rect::outset(float deltaX, float deltaY)
{
    static_cast<CGRect&>(*this) = CGRectInset(*this, -deltaX, -deltaY);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator = (const CGSize& inSize)
{
    this->size = inSize;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator += (const CGSize inSize)
{
    this->size.width  += inSize.width;
    this->size.height += inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator -= (const CGSize inSize)
{
    this->size.width  -= inSize.width;
    this->size.height -= inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator *= (const CGSize inSize)
{
    this->size.width  *= inSize.width;
    this->size.height *= inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator /= (const CGSize inSize)
{
    this->size.width  /= inSize.width;
    this->size.height /= inSize.height;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator |= (const CGRect& inRect)
{
    static_cast<CGRect&>(*this) = CGRectUnion(*this, inRect);
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Rect&
Rect::operator &= (const CGRect& inRect)
{
    static_cast<CGRect&>(*this) = CGRectIntersection(*this, inRect);
    return (*this);
}

}   // namespace B

// ------------------------------------------------------------------------------------------
/*! Compares two Rect objects for equality.
    
    @return     @c true if @a inLhs is equal to @a inRhs
    @relates    Rect
*/
inline bool operator == (const CGRect& inLhs, const CGRect& inRhs)  { return (CGRectEqualToRect(inLhs, inRhs)); }

// ------------------------------------------------------------------------------------------
/*! Compares two Rect objects for inequality.
    
    @return     @c true if @a inLhs is not equal to @a inRhs
    @relates    Rect
*/
inline bool operator != (const CGRect& inLhs, const CGRect& inRhs)  { return (!CGRectEqualToRect(inLhs, inRhs)); }

// ------------------------------------------------------------------------------------------
inline CGRect   operator + (const CGRect inLhs, const CGPoint inRhs)
{
    return CGRectMake(inLhs.origin.x + inRhs.x, inLhs.origin.y + inRhs.y, 
                      inLhs.size.width, inLhs.size.height);
}

// ------------------------------------------------------------------------------------------
inline CGRect   operator - (const CGRect inLhs, const CGPoint inRhs)
{
    return CGRectMake(inLhs.origin.x - inRhs.x, inLhs.origin.y - inRhs.y, 
                      inLhs.size.width, inLhs.size.height);
}

// ------------------------------------------------------------------------------------------
inline CGRect   operator + (const CGRect inLhs, const CGSize inRhs)
{
    return CGRectMake(inLhs.origin.x, inLhs.origin.y, 
                      inLhs.size.width + inRhs.width, inLhs.size.height + inRhs.height);
}

// ------------------------------------------------------------------------------------------
inline CGRect   operator - (const CGRect inLhs, const CGSize inRhs)
{
    return CGRectMake(inLhs.origin.x, inLhs.origin.y, 
                      inLhs.size.width - inRhs.width, inLhs.size.height - inRhs.height);
}

// ------------------------------------------------------------------------------------------
inline CGRect   operator * (const CGRect inLhs, const CGSize inRhs)
{
    return CGRectMake(inLhs.origin.x, inLhs.origin.y, 
                      inLhs.size.width * inRhs.width, inLhs.size.height * inRhs.height);
}

// ------------------------------------------------------------------------------------------
inline CGRect   operator / (const CGRect inLhs, const CGSize inRhs)
{
    return CGRectMake(inLhs.origin.x, inLhs.origin.y, 
                      inLhs.size.width / inRhs.width, inLhs.size.height / inRhs.height);
}


#endif  // BRect_H_
