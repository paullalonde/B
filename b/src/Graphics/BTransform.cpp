// ==========================================================================================
//	
//	Copyright (C) 2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

// file header
#include "BTransform.h"

// B headers
#include "BRect.h"


namespace B {
namespace Graphics {


const Transform	Transform::Identity(CGAffineTransformMake(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f));

// ------------------------------------------------------------------------------------------
Transform
Transform::MakeTranslate(float tx, float ty)
{
	return (Transform(CGAffineTransformMakeTranslation(tx, ty)));
}

// ------------------------------------------------------------------------------------------
Transform
Transform::MakeTranslate(const CGPoint& inPt)
{
	return (Transform(CGAffineTransformMakeTranslation(inPt.x, inPt.y)));
}

// ------------------------------------------------------------------------------------------
Transform
Transform::MakeScale(float sx, float sy)
{
	return (Transform(CGAffineTransformMakeScale(sx, sy)));
}

// ------------------------------------------------------------------------------------------
Transform
Transform::MakeScale(const CGSize& inSize)
{
	return (Transform(CGAffineTransformMakeScale(inSize.width, inSize.height)));
}

// ------------------------------------------------------------------------------------------
Transform
Transform::MakeRotate(float radians)
{
	return (Transform(CGAffineTransformMakeRotation(radians)));
}

// ------------------------------------------------------------------------------------------
Transform
Transform::inverse() const
{
	return (Transform(CGAffineTransformInvert(*this)));
}

// ------------------------------------------------------------------------------------------
Transform&
Transform::operator += (const CGAffineTransform& inTransform)
{
	AT() = CGAffineTransformConcat(*this, inTransform);
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Transform::translate(float dx, float dy)
{
	AT() = CGAffineTransformTranslate(*this, dx, dy);
}

// ------------------------------------------------------------------------------------------
void
Transform::translate(const CGPoint& inPt)
{
	AT() = CGAffineTransformTranslate(*this, inPt.x, inPt.y);
}

// ------------------------------------------------------------------------------------------
Transform&
Transform::operator += (const CGPoint& inPt)
{
	AT() = CGAffineTransformTranslate(*this, inPt.x, inPt.y);
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Transform::scale(float sx, float sy)
{
	AT() = CGAffineTransformScale(*this, sx, sy);
}

// ------------------------------------------------------------------------------------------
void
Transform::scale(const CGSize& inSize)
{
	AT() = CGAffineTransformScale(*this, inSize.width, inSize.height);
}

// ------------------------------------------------------------------------------------------
Transform&
Transform::operator *= (const CGSize& inSize)
{
	AT() = CGAffineTransformScale(*this, inSize.width, inSize.height);
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Transform::rotate(float radians)
{
	AT() = CGAffineTransformRotate(*this, radians);
}

// ------------------------------------------------------------------------------------------
Transform&
Transform::operator ^= (float radians)
{
	AT() = CGAffineTransformRotate(*this, radians);
	return (*this);
}

// ------------------------------------------------------------------------------------------
CGRect
Transform::apply(const CGRect& ioRect) const
{
	return CGRectApplyAffineTransform(ioRect, *this);
}

// ------------------------------------------------------------------------------------------
CGPoint
Transform::apply(const CGPoint& ioPoint) const
{
	return CGPointApplyAffineTransform(ioPoint, *this);
}

// ------------------------------------------------------------------------------------------
CGSize
Transform::apply(const CGSize& ioSize) const
{
	return CGSizeApplyAffineTransform(ioSize, *this);
}

#ifndef NDEBUG
// ------------------------------------------------------------------------------------------
void
Transform::DebugPrint(const char* prefix /* = NULL */) const
{
	printf("%s%s[%g %g %g %g %g %g]\n", 
		   (prefix != NULL) ? prefix : "",
		   (prefix != NULL) ? " " : "",
		   a, b, c, d, tx, ty);
}
#endif

// ------------------------------------------------------------------------------------------
Transform	operator + (const CGAffineTransform& t1, const CGAffineTransform& t2)
{
	Transform	t(t1);
	
	t += t2;
	
	return t;
}


}	// namespace Graphics
}	// namespace B
