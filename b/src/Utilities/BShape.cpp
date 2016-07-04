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
#include "BShape.h"

// B headers
#include "BRect.h"
#include "BMutableShape.h"


namespace B {

const OSPtr<HIShapeRef>	Shape::sEmptyShape(HIShapeCreateEmpty(), from_copy);

// ------------------------------------------------------------------------------------------
Shape::Shape()
{
	mShapeRef = sEmptyShape.get();
	CFRetain(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(const MutableShape& inShape)
{
	mShapeRef = HIShapeCreateCopy(inShape.cf_ref());
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(RgnHandle inRegion)
{
	B_ASSERT(inRegion != NULL);

	mShapeRef = HIShapeCreateWithQDRgn(inRegion);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(const Rect& inRect)
{
	mShapeRef = HIShapeCreateWithRect(&inRect);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(const ::Rect& inRect)
{
	Rect	r(inRect);
	
	mShapeRef = HIShapeCreateWithRect(&r);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(HIShapeRef inShapeRef)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = HIShapeCreateCopy(inShapeRef);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(HIShapeRef inShapeRef, const from_copy_t&)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = HIShapeCreateCopy(inShapeRef);
	B_THROW_IF_NULL(mShapeRef);
	
	CFRelease(inShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(HIShapeRef inShapeRef, const std::nothrow_t&)
{
	if (inShapeRef != NULL)
	{
		mShapeRef = HIShapeCreateCopy(inShapeRef);
		B_THROW_IF_NULL(mShapeRef);
	}
	else
	{
		mShapeRef = sEmptyShape.get();
		CFRetain(mShapeRef);
	}
}

// ------------------------------------------------------------------------------------------
Shape::Shape(HIShapeRef inShapeRef, const from_copy_t&, const std::nothrow_t&)
{
	if (inShapeRef != NULL)
	{
		mShapeRef = HIShapeCreateCopy(inShapeRef);
		B_THROW_IF_NULL(mShapeRef);
		
		CFRelease(inShapeRef);
	}
	else
	{
		mShapeRef = sEmptyShape.get();
		CFRetain(mShapeRef);
	}
}

// ------------------------------------------------------------------------------------------
Shape::Shape(const OSPtr<HIShapeRef>& inShapeRef)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = HIShapeCreateCopy(inShapeRef);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Shape::Shape(const OSPtr<HIMutableShapeRef>& inShapeRef)
{
	B_ASSERT(inShapeRef != NULL);
	
	mShapeRef = HIShapeCreateCopy(inShapeRef);
	B_THROW_IF_NULL(mShapeRef);
}

// ------------------------------------------------------------------------------------------
Rect
Shape::bounds() const
{
	Rect	r;
	
	HIShapeGetBounds(mShapeRef, &r);
	
	return r;
}

// ------------------------------------------------------------------------------------------
void
Shape::clear()
{
	Shape().swap(*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::operator = (const Shape& inShape)
{
	Shape(inShape).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::operator = (const MutableShape& inShape)
{
	Shape(inShape).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::assign(const Rect& inRect)
{
	Shape(inRect).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::assign(RgnHandle inRegion)
{
	Shape(inRegion).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::assign(const OSPtr<HIShapeRef>& inShapeRef)
{
	Shape(inShapeRef).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::assign(const OSPtr<HIMutableShapeRef>& inShapeRef)
{
	Shape(inShapeRef).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::assign(HIShapeRef inShapeRef)
{
	Shape(inShapeRef).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Shape&
Shape::assign(HIShapeRef inShapeRef, const from_copy_t& fc)
{
	Shape(inShapeRef, fc).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
AutoRegion
Shape::GetRegion() const
{
	AutoRegion	autoRgn;
	OSStatus	err;
	
	err = HIShapeGetAsQDRgn(mShapeRef, autoRgn);
	B_THROW_IF_STATUS(err);
	
	return (autoRgn);
}

// ------------------------------------------------------------------------------------------
bool
Shape::contains(const Point& inPoint) const
{
	return (HIShapeContainsPoint(mShapeRef, &inPoint));
}

// ------------------------------------------------------------------------------------------
bool
Shape::contains(::Point inPoint) const
{
	Point	p(inPoint);
	
	return (HIShapeContainsPoint(mShapeRef, &p));
}

// ------------------------------------------------------------------------------------------
bool
Shape::intersects(const Rect& inRect) const
{
	return (HIShapeIntersectsRect(mShapeRef, &inRect));
}

// ------------------------------------------------------------------------------------------
bool
operator == (const Shape& inLhs, const Shape& inRhs)
{
	return (CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator == (const Shape& inLhs, const MutableShape& inRhs)
{
	return (CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator == (const MutableShape& inLhs, const Shape& inRhs)
{
	return (CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator == (const MutableShape& inLhs, const MutableShape& inRhs)
{
	return (CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator != (const Shape& inLhs, const Shape& inRhs)
{
	return (!CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator != (const Shape& inLhs, const MutableShape& inRhs)
{
	return (!CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator != (const MutableShape& inLhs, const Shape& inRhs)
{
	return (!CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
operator != (const MutableShape& inLhs, const MutableShape& inRhs)
{
	return (!CFEqual(inLhs.cf_ref(), inRhs.cf_ref()));
}

// ------------------------------------------------------------------------------------------
Shape
operator | (const Shape& inLhs, const Shape& inRhs)
{
	return (Shape(HIShapeCreateUnion(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator | (const Shape& inLhs, const MutableShape& inRhs)
{
	return (Shape(HIShapeCreateUnion(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator | (const MutableShape& inLhs, const Shape& inRhs)
{
	return (Shape(HIShapeCreateUnion(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator | (const MutableShape& inLhs, const MutableShape& inRhs)
{
	return (Shape(HIShapeCreateUnion(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator & (const Shape& inLhs, const Shape& inRhs)
{
	return (Shape(HIShapeCreateIntersection(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator & (const Shape& inLhs, const MutableShape& inRhs)
{
	return (Shape(HIShapeCreateIntersection(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator & (const MutableShape& inLhs, const Shape& inRhs)
{
	return (Shape(HIShapeCreateIntersection(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator & (const MutableShape& inLhs, const MutableShape& inRhs)
{
	return (Shape(HIShapeCreateIntersection(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator - (const Shape& inLhs, const Shape& inRhs)
{
	return (Shape(HIShapeCreateDifference(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator - (const Shape& inLhs, const MutableShape& inRhs)
{
	return (Shape(HIShapeCreateDifference(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator - (const MutableShape& inLhs, const Shape& inRhs)
{
	return (Shape(HIShapeCreateDifference(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator - (const MutableShape& inLhs, const MutableShape& inRhs)
{
	return (Shape(HIShapeCreateDifference(inLhs.cf_ref(), inRhs.cf_ref()), from_copy));
}

// ------------------------------------------------------------------------------------------
Shape
operator ^ (const Shape& inLhs, const Shape& inRhs)
{
	return ((inLhs | inRhs) - (inLhs & inRhs));
}

// ------------------------------------------------------------------------------------------
Shape
operator ^ (const Shape& inLhs, const MutableShape& inRhs)
{
	return ((inLhs | inRhs) - (inLhs & inRhs));
}

// ------------------------------------------------------------------------------------------
Shape
operator ^ (const MutableShape& inLhs, const Shape& inRhs)
{
	return ((inLhs | inRhs) - (inLhs & inRhs));
}

// ------------------------------------------------------------------------------------------
Shape
operator ^ (const MutableShape& inLhs, const MutableShape& inRhs)
{
	return ((inLhs | inRhs) - (inLhs & inRhs));
}


}	// namespace B
