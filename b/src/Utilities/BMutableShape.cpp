// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
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
#include "BMutableShape.h"

// B headers
#include "BRect.h"
#include "BShape.h"


namespace B {

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape()
{
	mShapeRef = HIShapeCreateMutable();
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(const MutableShape& inShape)
{
	mShapeRef = HIShapeCreateMutableCopy(inShape.cf_ref());
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(const Shape& inShape)
{
	mShapeRef = HIShapeCreateMutableCopy(inShape.cf_ref());
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(RgnHandle inRegion)
{
	B_ASSERT(inRegion != NULL);
	
	Shape	shape(inRegion);
	
	mShapeRef = HIShapeCreateMutableCopy(shape.cf_ref());
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(const Rect& inRect)
{
	Shape	shape(inRect);
	
	mShapeRef = HIShapeCreateMutableCopy(shape.cf_ref());
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(const ::Rect& inRect)
{
	Shape	shape(inRect);
	
	mShapeRef = HIShapeCreateMutableCopy(shape.cf_ref());
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(HIShapeRef inShapeRef)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = HIShapeCreateMutableCopy(inShapeRef);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(const OSPtr<HIShapeRef>& inShapeRef)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = HIShapeCreateMutableCopy(inShapeRef);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
MutableShape::MutableShape(const OSPtr<HIMutableShapeRef>& inShapeRef)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = inShapeRef;
	CFRetain(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Rect
MutableShape::bounds() const
{
	Rect	r;
	
	HIShapeGetBounds(mShapeRef, &r);
	
	return r;
}

// ------------------------------------------------------------------------------------------
void
MutableShape::clear()
{
	OSStatus	err;
	
	err = HIShapeSetEmpty(mShapeRef);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator = (const MutableShape& inShape)
{
	OSStatus	err;
	
	err = HIShapeUnion(inShape.cf_ref(), inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator = (const Shape& inShape)
{
	OSStatus	err;
	
	err = HIShapeUnion(inShape.cf_ref(), inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator = (const Rect& inRect)
{
	MutableShape(inRect).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator = (RgnHandle inRegion)
{
	MutableShape(inRegion).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator = (const OSPtr<HIShapeRef>& inShapeRef)
{
	MutableShape(inShapeRef).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator = (const OSPtr<HIMutableShapeRef>& inShapeRef)
{
	MutableShape(inShapeRef).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator |= (const MutableShape& inShape)
{
	OSStatus	err;
	
	err = HIShapeUnion(mShapeRef, inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator |= (const Shape& inShape)
{
	OSStatus	err;
	
	err = HIShapeUnion(mShapeRef, inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator &= (const MutableShape& inShape)
{
	OSStatus	err;
	
	err = HIShapeIntersect(mShapeRef, inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator &= (const Shape& inShape)
{
	OSStatus	err;
	
	err = HIShapeIntersect(mShapeRef, inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator -= (const MutableShape& inShape)
{
	OSStatus	err;
	
	err = HIShapeDifference(mShapeRef, inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator -= (const Shape& inShape)
{
	OSStatus	err;
	
	err = HIShapeDifference(mShapeRef, inShape.cf_ref(), mShapeRef);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator ^= (const MutableShape& inShape)
{
	MutableShape	intersection(*this);
	
	intersection	&= inShape;
	*this			|= inShape;
	*this			-= intersection;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator ^= (const Shape& inShape)
{
	MutableShape	intersection(*this);
	
	intersection	&= inShape;
	*this			|= inShape;
	*this			-= intersection;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
AutoRegion
MutableShape::GetRegion() const
{
	AutoRegion	autoRgn;
	OSStatus	err;
	
	err = HIShapeGetAsQDRgn(mShapeRef, autoRgn);
	B_THROW_IF_STATUS(err);
	
	return (autoRgn);
}

// ------------------------------------------------------------------------------------------
void
MutableShape::offset(short deltaX, short deltaY)
{
	OSStatus	err;
	
	err = HIShapeOffset(mShapeRef, deltaX, deltaY);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator += (const Point& inPoint)
{
	OSStatus	err;
	
	err = HIShapeOffset(mShapeRef, inPoint.x, inPoint.y);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableShape&
MutableShape::operator -= (const Point& inPoint)
{
	OSStatus	err;
	
	err = HIShapeOffset(mShapeRef, -inPoint.x, -inPoint.y);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
bool
MutableShape::contains(const Point& inPoint) const
{
	return (HIShapeContainsPoint(mShapeRef, &inPoint));
}

// ------------------------------------------------------------------------------------------
bool
MutableShape::contains(::Point inPoint) const
{
	Point	p(inPoint);
	
	return (HIShapeContainsPoint(mShapeRef, &p));
}

// ------------------------------------------------------------------------------------------
bool
MutableShape::intersects(const Rect& inRect) const
{
	return (HIShapeIntersectsRect(mShapeRef, &inRect));
}


}	// namespace B
