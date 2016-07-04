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

#ifndef BShape_H_
#define BShape_H_

#pragma	once

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BAutoRegion.h"
#include "BOSPtr.h"


namespace B {

// forward declarations
class	MutableShape;
class	Point;
class	Rect;
class	Size;


/*!	@brief	Wrapper class for <tt>HIShapeRef</tt>.
	
	@todo	%Document this class!
*/
class Shape
{
public:
	
	// constructors / destructor
				Shape();
				Shape(const Shape& inShape);
	explicit	Shape(const MutableShape& inShape);
	explicit	Shape(RgnHandle inRegion);
	explicit	Shape(const Rect& inRect);
	explicit	Shape(const ::Rect& inRect);
	explicit	Shape(HIShapeRef inShapeRef);
	explicit	Shape(HIShapeRef inShapeRef, const from_copy_t&);
	explicit	Shape(HIShapeRef inShapeRef, const std::nothrow_t&);
	explicit	Shape(HIShapeRef inShapeRef, const from_copy_t&, const std::nothrow_t&);
	explicit	Shape(const OSPtr<HIShapeRef>& inShapeRef);
	explicit	Shape(const OSPtr<HIMutableShapeRef>& inShapeRef);
				~Shape();
	
	// inquiries
	bool		empty() const;
	bool		operator ! () const;
	bool		rectangular() const;
	Rect		bounds() const;
	
	// modifiers
	void		clear();
	void		swap(Shape& shape);
	
	// assignment
	Shape&	operator = (const Shape &inShape);
	Shape&	operator = (const MutableShape& inShape);
	Shape&	assign(RgnHandle inRegion);
	Shape&	assign(const Rect& inRect);
	Shape&	assign(const OSPtr<HIShapeRef>& inShapeRef);
	Shape&	assign(const OSPtr<HIMutableShapeRef>& inShapeRef);
	Shape&	assign(HIShapeRef inShapeRef);
	Shape&	assign(HIShapeRef inShapeRef, const from_copy_t&);
	
	// conversions
	HIShapeRef			cf_ref() const			{ return (mShapeRef); }
	OSPtr<HIShapeRef>	cf_ptr() const			{ return (OSPtr<HIShapeRef>(mShapeRef)); }
	AutoRegion			GetRegion() const;
	
	// hit-testing
	bool	contains(const Point& inPoint) const;
	bool	contains(::Point inPoint) const;
	bool	intersects(const Rect& inRect) const;

private:
	
	// static member variables
	static const OSPtr<HIShapeRef>	sEmptyShape;
	
	// member variables
	HIShapeRef	mShapeRef;
};

// ------------------------------------------------------------------------------------------
inline
Shape::Shape(const Shape& inShape)
	: mShapeRef(inShape.mShapeRef)
{
	CFRetain(mShapeRef);
}

// ------------------------------------------------------------------------------------------
inline
Shape::~Shape()
{
	CFRelease(mShapeRef);
}

// ------------------------------------------------------------------------------------------
inline bool
Shape::empty() const
{
	return (HIShapeIsEmpty(mShapeRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Shape::operator ! () const
{
	return (!HIShapeIsEmpty(mShapeRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Shape::rectangular() const
{
	return (HIShapeIsRectangular(mShapeRef));
}

// ------------------------------------------------------------------------------------------
inline void
Shape::swap(Shape& shape)
{
	std::swap(mShapeRef, shape.mShapeRef);
}


// ------------------------------------------------------------------------------------------
bool	operator == (const Shape& inLhs, const Shape& inRhs);
bool	operator == (const Shape& inLhs, const MutableShape& inRhs);
bool	operator == (const MutableShape& inLhs, const Shape& inRhs);
bool	operator == (const MutableShape& inLhs, const MutableShape& inRhs);

// ------------------------------------------------------------------------------------------
bool	operator != (const Shape& inLhs, const Shape& inRhs);
bool	operator != (const Shape& inLhs, const MutableShape& inRhs);
bool	operator != (const MutableShape& inLhs, const Shape& inRhs);
bool	operator != (const MutableShape& inLhs, const MutableShape& inRhs);

// ------------------------------------------------------------------------------------------
Shape	operator | (const Shape& inLhs, const Shape& inRhs);
Shape	operator | (const Shape& inLhs, const MutableShape& inRhs);
Shape	operator | (const MutableShape& inLhs, const Shape& inRhs);
Shape	operator | (const MutableShape& inLhs, const MutableShape& inRhs);

// ------------------------------------------------------------------------------------------
Shape	operator & (const Shape& inLhs, const Shape& inRhs);
Shape	operator & (const Shape& inLhs, const MutableShape& inRhs);
Shape	operator & (const MutableShape& inLhs, const Shape& inRhs);
Shape	operator & (const MutableShape& inLhs, const MutableShape& inRhs);

// ------------------------------------------------------------------------------------------
Shape	operator - (const Shape& inLhs, const Shape& inRhs);
Shape	operator - (const Shape& inLhs, const MutableShape& inRhs);
Shape	operator - (const MutableShape& inLhs, const Shape& inRhs);
Shape	operator - (const MutableShape& inLhs, const MutableShape& inRhs);

// ------------------------------------------------------------------------------------------
Shape	operator ^ (const Shape& inLhs, const Shape& inRhs);
Shape	operator ^ (const Shape& inLhs, const MutableShape& inRhs);
Shape	operator ^ (const MutableShape& inLhs, const Shape& inRhs);
Shape	operator ^ (const MutableShape& inLhs, const MutableShape& inRhs);

// ------------------------------------------------------------------------------------------
inline void
swap(Shape& shape1, Shape& shape2)
{
	shape1.swap(shape2);
}


}	// namespace B

#endif	// BShape_H_
