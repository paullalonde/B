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

#ifndef BRegion_H_
#define BRegion_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>


namespace B {


/*! @brief  Wrapper class for <tt>RgnHandle</tt>.
    
    @todo   Document this class!
*/
class Region
{
public:
    
    // constructors / destructor
                Region();
                Region(const Region& inRegion);
    explicit    Region(const HIRect& inRect);
    explicit    Region(const ::Rect& inRect);
    explicit    Region(
                    RgnHandle   inRegion,
                    bool        inOwner = true);
                ~Region() throw();
    
    // inquiries
    bool        empty() const;
    bool        operator ! () const;
    HIRect&     bounds(HIRect& r) const;
    
    // modifiers
    void        clear();
    RgnHandle   release();
    void        adopt(
                    RgnHandle   inRegion,
                    bool        inOwner = true);
    
    // assignment
    Region& operator = (const Region &inRegion);
    Region& operator = (RgnHandle inRegion);
    Region& operator = (const HIRect &inRect);
    
    // conversions
    operator    RgnHandle ()                    { return (mRegion); }
    RgnHandle   GetNonModifiableRegion() const  { return (mRegion); }
    
    // union
    Region& operator |= (const Region& inRegion);
    Region& operator |= (RgnHandle inRegion);
    Region& operator |= (const HIRect& inRect);
    Region& operator |= (const ::Rect& inRect);
    
    // intersection
    Region& operator &= (const Region& inRegion);
    Region& operator &= (RgnHandle inRegion);
    Region& operator &= (const HIRect& inRect);
    
    // difference
    Region& operator -= (const Region& inRegion);
    Region& operator -= (RgnHandle inRegion);
    Region& operator -= (const HIRect& inRect);
    
    // exclusive-or
    Region& operator ^= (const Region& inRegion);
    Region& operator ^= (RgnHandle inRegion);
    Region& operator ^= (const HIRect& inRect);
    
    // translation
    void    offset(short deltaX, short deltaY);
    Region& operator += (const HIPoint& inPoint);
    Region& operator -= (const HIPoint& inPoint);
    
    // expansion/constraction
    void    inset(short deltaX, short deltaY);
    Region& operator += (const HISize& inSize);
    Region& operator -= (const HISize& inSize);
    
    // hit-testing
    bool    contains(const HIPoint& inPoint) const;

private:
    
    // member variables
    RgnHandle   mRegion;
    bool        mOwner;
};


// ------------------------------------------------------------------------------------------
bool    operator == (const Region& inLhs, const Region& inRhs);

// ------------------------------------------------------------------------------------------
template <typename T> bool operator == (const Region& inLhs, const T& inRhs)
{
    Region  rhs(inRhs);
    
    return (inLhs == rhs);
}

// ------------------------------------------------------------------------------------------
template <typename T> bool operator == (const T& inLhs, const Region& inRhs)
{
    Region  lhs(inLhs);
    
    return (lhs == inRhs);
}

// ------------------------------------------------------------------------------------------
bool    operator != (const Region& inLhs, const Region& inRhs);

// ------------------------------------------------------------------------------------------
template <typename T> bool operator != (const Region& inLhs, const T& inRhs)
{
    Region  rhs(inRhs);
    
    return (inLhs != rhs);
}

// ------------------------------------------------------------------------------------------
template <typename T> bool operator != (const T& inLhs, const Region& inRhs)
{
    Region  lhs(inLhs);
    
    return (lhs != inRhs);
}

// ------------------------------------------------------------------------------------------
Region  operator | (const Region& inLhs, const Region& inRhs);

// ------------------------------------------------------------------------------------------
template <typename T> Region operator | (const Region& inLhs, const T& inRhs)
{
    Region  rhs(inRhs);
    
    rhs |= inLhs;
    
    return (rhs);
}

// ------------------------------------------------------------------------------------------
template <typename T> Region operator | (const T& inLhs, const Region& inRhs)
{
    Region  lhs(inLhs);
    
    lhs |= inRhs;
    
    return (lhs);
}

// ------------------------------------------------------------------------------------------
Region  operator & (const Region& inLhs, const Region& inRhs);

// ------------------------------------------------------------------------------------------
template <typename T> Region operator & (const Region& inLhs, const T& inRhs)
{
    Region  rhs(inRhs);
    
    rhs &= inLhs;
    
    return (rhs);
}

// ------------------------------------------------------------------------------------------
template <typename T> Region operator & (const T& inLhs, const Region& inRhs)
{
    Region  lhs(inLhs);
    
    lhs &= inRhs;
    
    return (lhs);
}

// ------------------------------------------------------------------------------------------
Region  operator - (const Region& inLhs, const Region& inRhs);

// ------------------------------------------------------------------------------------------
template <typename T> Region operator - (const Region& inLhs, const T& inRhs)
{
    Region  rhs(inRhs);
    
    rhs -= inLhs;
    
    return (rhs);
}

// ------------------------------------------------------------------------------------------
template <typename T> Region operator - (const T& inLhs, const Region& inRhs)
{
    Region  lhs(inLhs);
    
    lhs -= inRhs;
    
    return (lhs);
}

// ------------------------------------------------------------------------------------------
Region  operator ^ (const Region& inLhs, const Region& inRhs);

// ------------------------------------------------------------------------------------------
template <typename T> Region operator ^ (const Region& inLhs, const T& inRhs)
{
    Region  rhs(inRhs);
    
    rhs ^= inLhs;
    
    return (rhs);
}

// ------------------------------------------------------------------------------------------
template <typename T> Region operator ^ (const T& inLhs, const Region& inRhs)
{
    Region  lhs(inLhs);
    
    lhs ^= inRhs;
    
    return (lhs);
}


}   // namespace B

#endif  // BRegion_H_
