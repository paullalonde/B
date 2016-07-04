// ==========================================================================================
//  
//  Copyright (C) 2006 Paul Lalonde enrg.
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

#ifndef BTransform_H_
#define BTransform_H_

#pragma once

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class   Point;
class   Rect;
class   Size;

namespace Graphics {

class Transform : public CGAffineTransform
{
public:
    
    // constants
    static const Transform  Identity;
    
    // factory functions
    static Transform    MakeTranslate(float tx, float ty);
    static Transform    MakeTranslate(const CGPoint& inPt);
    static Transform    MakeScale(float sx, float sy);
    static Transform    MakeScale(const CGSize& inSize);
    static Transform    MakeRotate(float radians);
    
    // constructors
                Transform();
    explicit    Transform(const CGAffineTransform& inTransform);
    
    // assignment
    Transform&  operator = (const CGAffineTransform& inTransform);
    
    // inquiries
    bool        identity() const;
    Transform   inverse() const;
    
    // concatenation
    Transform&  operator += (const CGAffineTransform& inTransform);
    
    // translation
    void        translate(float tx, float ty);
    void        translate(const CGPoint& inPt);
    Transform&  operator += (const CGPoint& inPt);
    
    // scaling
    void        scale(float sx, float sy);
    void        scale(const CGSize& inSize);
    Transform&  operator *= (const CGSize& inSize);
    
    // rotation
    void        rotate(float radians);
    Transform&  operator ^= (float radians);
    
    CGRect      apply(const CGRect& ioRect) const;
    CGPoint     apply(const CGPoint& ioPoint) const;
    CGSize      apply(const CGSize& ioSize) const;
    
    // debugging
#ifndef NDEBUG
    void        DebugPrint(const char* prefix = NULL) const;
#endif
    
private:
    
    CGAffineTransform&  AT()    { return (static_cast<CGAffineTransform&>(*this)); }
};

// ------------------------------------------------------------------------------------------
inline
Transform::Transform()
{
    AT() = Identity;
}

// ------------------------------------------------------------------------------------------
inline
Transform::Transform(const CGAffineTransform& inTransform)
{
    AT() = inTransform;
}

// ------------------------------------------------------------------------------------------
inline Transform&
Transform::operator = (const CGAffineTransform& inTransform)
{
    AT() = inTransform;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline bool
Transform::identity() const
{
    return (CGAffineTransformIsIdentity(*this));
}

#ifdef NDEBUG
// ------------------------------------------------------------------------------------------
void
Transform::DebugPrint() const
{
}
#endif


// ------------------------------------------------------------------------------------------
Transform   operator + (const CGAffineTransform& t1, const CGAffineTransform& t2);


/*! @defgroup   TransformFunctions  B::Transform Global Functions
*/
//@{

// ------------------------------------------------------------------------------------------
inline bool operator == (const CGAffineTransform& t1, const CGAffineTransform& t2)
{
    return (CGAffineTransformEqualToTransform(t1, t2));
}

// ------------------------------------------------------------------------------------------
inline bool operator != (const CGAffineTransform& t1, const CGAffineTransform& t2)
{
    return (!CGAffineTransformEqualToTransform(t1, t2));
}

//@}

}   // namespace Graphics
}   // namespace B


#endif  // BTransform_H_
