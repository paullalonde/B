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

// B headers
#include "BErrorHandler.h"
#include "BRect.h"
#include "BRegion.h"


namespace B {

// ------------------------------------------------------------------------------------------
Region::Region()
	: mOwner(true)
{
	mRegion = NewRgn();
	B_THROW_IF_NULL(mRegion);
}

// ------------------------------------------------------------------------------------------
Region::Region(const Region& inRegion)
	: mRegion(NULL), mOwner(true)
{
	try
	{
		mRegion = NewRgn();
		B_THROW_IF_NULL(mRegion);
		
		CopyRgn(inRegion.mRegion, mRegion);
		B_THROW_IF_STATUS(QDError());
	}
	catch (...)
	{
		if (mRegion != NULL)
			DisposeRgn(mRegion);
		
		throw;
	}
}

// ------------------------------------------------------------------------------------------
Region::Region(const HIRect& inRect)
	: mOwner(true)
{
	mRegion = NewRgn();
	B_THROW_IF_NULL(mRegion);
	
	RectRgn(mRegion, Rect::GetQDRect(inRect));
}

// ------------------------------------------------------------------------------------------
Region::Region(const ::Rect& inRect)
	: mOwner(true)
{
	mRegion = NewRgn();
	B_THROW_IF_NULL(mRegion);
	
	RectRgn(mRegion, &inRect);
}

// ------------------------------------------------------------------------------------------
Region::Region(
	RgnHandle	inRegion,
	bool		inOwner /* = true */)
		: mRegion(inRegion), mOwner(inOwner)
{
	B_ASSERT(inRegion != NULL);
}

// ------------------------------------------------------------------------------------------
Region::~Region() throw()
{
	B_ASSERT(mRegion != NULL);
	
	if (mOwner)
		DisposeRgn(mRegion);
}

// ------------------------------------------------------------------------------------------
bool
Region::empty() const
{
	return (EmptyRgn(mRegion));
}

// ------------------------------------------------------------------------------------------
bool
Region::operator ! () const
{
	return (!EmptyRgn(mRegion));
}

// ------------------------------------------------------------------------------------------
bool
Region::contains(const HIPoint& inPoint) const
{
	return (PtInRgn(Point::QDPoint(inPoint), mRegion));
}

// ------------------------------------------------------------------------------------------
void
Region::clear()
{
	SetEmptyRgn(mRegion);
}

// ------------------------------------------------------------------------------------------
RgnHandle
Region::release()
{
	mOwner = false;
	
	return (mRegion);
}

// ------------------------------------------------------------------------------------------
void
Region::adopt(
	RgnHandle	inRegion,
	bool		inOwner /* = true */)
{
	B_ASSERT(inRegion != NULL);
	
	if (mOwner)
		DisposeRgn(mRegion);
	
	mOwner	= inOwner;
	mRegion	= inRegion;
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator = (const Region& inRegion)
{
	CopyRgn(inRegion.mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator = (RgnHandle inRegion)
{
	CopyRgn(inRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator = (const HIRect& inRect)
{
	RectRgn(mRegion, Rect::GetQDRect(inRect));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator |= (const Region& inRegion)
{
	UnionRgn(inRegion.mRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator |= (RgnHandle inRegion)
{
	UnionRgn(inRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator |= (const HIRect& inRect)
{
	Region	rhs(inRect);
	
	UnionRgn(rhs, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator |= (const ::Rect &inRect)
{
	Region	rhs(inRect);
	
	UnionRgn(rhs, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator &= (const Region &inRegion)
{
	SectRgn(inRegion.mRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator &= (RgnHandle inRegion)
{
	SectRgn(inRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator &= (const HIRect& inRect)
{
	Region	rhs(inRect);
	
	SectRgn(rhs, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator -= (const Region& inRegion)
{
	DiffRgn(inRegion.mRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator -= (RgnHandle inRegion)
{
	DiffRgn(inRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator -= (const HIRect& inRect)
{
	Region	rhs(inRect);
	
	DiffRgn(rhs, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator ^= (const Region& inRegion)
{
	XorRgn(inRegion.mRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator ^= (RgnHandle inRegion)
{
	XorRgn(inRegion, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator ^= (const HIRect& inRect)
{
	Region	rhs(inRect);
	
	XorRgn(rhs, mRegion, mRegion);
	B_THROW_IF_STATUS(QDError());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Region::offset(short deltaX, short deltaY)
{
	OffsetRgn(mRegion, deltaX, deltaY);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator += (const HIPoint& inPoint)
{
	OffsetRgn(mRegion, static_cast<short>(inPoint.x), static_cast<short>(inPoint.y));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator -= (const HIPoint& inPoint)
{
	OffsetRgn(mRegion, static_cast<short>(-inPoint.x), static_cast<short>(-inPoint.y));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Region::inset(short deltaX, short deltaY)
{
	InsetRgn(mRegion, deltaX, deltaY);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator += (const HISize& inSize)
{
	InsetRgn(mRegion, static_cast<short>(-inSize.width), static_cast<short>(-inSize.height));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Region&
Region::operator -= (const HISize& inSize)
{
	InsetRgn(mRegion, static_cast<short>(inSize.width), static_cast<short>(inSize.height));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
bool
operator == (const Region& inLhs, const Region& inRhs)
{
	return (EqualRgn(inLhs.GetNonModifiableRegion(), inRhs.GetNonModifiableRegion()));
}

// ------------------------------------------------------------------------------------------
bool
operator != (const Region& inLhs, const Region& inRhs)
{
	return (!EqualRgn(inLhs.GetNonModifiableRegion(), inRhs.GetNonModifiableRegion()));
}

// ------------------------------------------------------------------------------------------
Region
operator | (const Region& inLhs, const Region& inRhs)
{
	Region	result(inLhs);
	
	result |= inRhs;
	
	return (result);
}

// ------------------------------------------------------------------------------------------
Region
operator & (const Region& inLhs, const Region& inRhs)
{
	Region	result(inLhs);
	
	result &= inRhs;
	
	return (result);
}

// ------------------------------------------------------------------------------------------
Region
operator - (const Region& inLhs, const Region& inRhs)
{
	Region	result(inLhs);
	
	result -= inRhs;
	
	return (result);
}

// ------------------------------------------------------------------------------------------
Region
operator ^ (const Region& inLhs, const Region& inRhs)
{
	Region	result(inLhs);
	
	result ^= inRhs;
	
	return (result);
}


}	// namespace B
