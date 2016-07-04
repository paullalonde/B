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

#ifndef BColor_H_
#define BColor_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {
namespace Graphics {

class Color
{
public:
	
	// popular color spaces
	static OSPtr<CGColorSpaceRef>	GetGenericGrayColorSpace();
	static OSPtr<CGColorSpaceRef>	GetGenericRGBColorSpace();
	static OSPtr<CGColorSpaceRef>	GetGenericCMYKColorSpace();
	
	// popular colors
	static Color	GetDeviceRGBBlack();
	static Color	GetDeviceRGBWhite();
	static Color	GetDeviceRGBTransparent();
	
	// constructors / destructor
				Color(const Color& inColor);
	explicit	Color(const OSPtr<CGColorRef>& inColor);
	explicit	Color(CGColorSpaceRef inColorSpace, const float inComponents[]);
	explicit	Color(CGColorSpaceRef inColorSpace, float f0);
	explicit	Color(CGColorSpaceRef inColorSpace, float f0, float f1);
	explicit	Color(CGColorSpaceRef inColorSpace, float f0, float f1, float f2);
	explicit	Color(CGColorSpaceRef inColorSpace, float f0, float f1, float f2, float f3);
	explicit	Color(CGColorSpaceRef inColorSpace, float f0, float f1, float f2, float f3, float f4);
				
	// assignment
	Color&	operator = (const Color& inColor);
	Color&	operator = (const OSPtr<CGColorRef>& inColor);
	//! Exchanges the contents of the color with @a ioColor.
	void	swap(Color& ioColor);
	
	// copying
	Color	clone() const;
	Color	clone(float alpha) const;
	
	// inquiries
	CGColorSpaceRef	colorSpace() const	{ return (CGColorGetColorSpace(mColor)); }
	size_t			count() const		{ return (CGColorGetNumberOfComponents(mColor)); }
	const float*	components() const	{ return (CGColorGetComponents(mColor)); }
	float			alpha() const		{ return (CGColorGetAlpha(mColor)); }
	
	// conversions
				operator CGColorRef () const	{ return (mColor); }
	CGColorRef	get() const						{ return (mColor); }
	
private:
	
	explicit	Color(CGColorRef inColorRef, const from_copy_t&);
	
	static CGColorSpaceRef	GetDeviceRGBColorSpace();
	
	// member variables
	B::OSPtr<CGColorRef>	mColor;
};

// ------------------------------------------------------------------------------------------
inline
Color::Color(const Color& inColor)
	: mColor(inColor.mColor)
{
}

// ------------------------------------------------------------------------------------------
inline
Color::Color(const OSPtr<CGColorRef>& inColor)
	: mColor(inColor)
{
}

// ------------------------------------------------------------------------------------------
inline
Color::Color(CGColorRef inColorRef, const from_copy_t& fc)
	: mColor(inColorRef, fc)
{
}

// ------------------------------------------------------------------------------------------
inline void
Color::swap(Color& ioColor)
{
	mColor.swap(ioColor.mColor);
}

// ------------------------------------------------------------------------------------------
inline Color&
Color::operator = (const Color& inColor)
{
	mColor = inColor.mColor;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Color&
Color::operator = (const OSPtr<CGColorRef>& inColor)
{
	mColor = inColor;
	return (*this);
}


/*!	@defgroup	ColorFunctions	B::Color Global Functions
*/
//@{

// ------------------------------------------------------------------------------------------
inline bool	operator == (const Color& c1, const Color& c2)
{
	return (CGColorEqualToColor(c1, c2));
}

// ------------------------------------------------------------------------------------------
inline bool	operator != (const Color& c1, const Color& c2)
{
	return (!CGColorEqualToColor(c1, c2));
}

//@}


//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a c1 and @a c2.

	@relates	Color
*/
template <typename T> inline void
swap(Color& c1, Color& c2)	{ c1.swap(c2); }

//@}


}	// namespace Graphics
}	// namespace B


#endif	// BColor_H_
