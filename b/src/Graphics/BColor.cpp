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
#include "BColor.h"


namespace B {
namespace Graphics {

// ------------------------------------------------------------------------------------------
OSPtr<CGColorSpaceRef>
Color::GetGenericGrayColorSpace()
{
	static OSPtr<CGColorSpaceRef>	sGrayColorSpace(
										CGColorSpaceCreateWithName(kCGColorSpaceGenericGray), 
										from_copy);
	
	return sGrayColorSpace;
}

// ------------------------------------------------------------------------------------------
OSPtr<CGColorSpaceRef>
Color::GetGenericRGBColorSpace()
{
	static OSPtr<CGColorSpaceRef>	sRGBColorSpace(
										CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB), 
										from_copy);
	
	return sRGBColorSpace;
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CGColorSpaceRef>
Color::GetGenericCMYKColorSpace()
{
	static OSPtr<CGColorSpaceRef>	sCMYKColorSpace(
										CGColorSpaceCreateWithName(kCGColorSpaceGenericCMYK), 
										from_copy);
	
	return sCMYKColorSpace;
}

// ------------------------------------------------------------------------------------------
Color
Color::GetDeviceRGBBlack()
{
	static Color	black(GetDeviceRGBColorSpace(), 0.0f, 0.0f, 0.0f, 1.0f);
	
	return (black);
}

// ------------------------------------------------------------------------------------------
Color
Color::GetDeviceRGBWhite()
{
	static Color	white(GetDeviceRGBColorSpace(), 1.0f, 1.0f, 1.0f, 1.0f);
	
	return (white);
}

// ------------------------------------------------------------------------------------------
Color
Color::GetDeviceRGBTransparent()
{
	static Color	transparent(GetDeviceRGBColorSpace(), 0.0f, 0.0f, 0.0f, 0.0f);
	
	return (transparent);
}

// ------------------------------------------------------------------------------------------
CGColorSpaceRef
Color::GetDeviceRGBColorSpace()
{
	static OSPtr<CGColorSpaceRef>	colorSpace(CGColorSpaceCreateDeviceRGB(), from_copy);
	
	return (colorSpace);
}

// ------------------------------------------------------------------------------------------
Color::Color(CGColorSpaceRef inColorSpace, const float inComponents[])
	: mColor(CGColorCreate(inColorSpace, inComponents), from_copy)
{
}

// ------------------------------------------------------------------------------------------
Color::Color(CGColorSpaceRef inColorSpace, float f0)
{
	float	f[]	= { f0 };
	
	mColor.reset(CGColorCreate(inColorSpace, f), from_copy);
}

// ------------------------------------------------------------------------------------------
Color::Color(CGColorSpaceRef inColorSpace, float f0, float f1)
{
	float	f[]	= { f0, f1 };
	
	mColor.reset(CGColorCreate(inColorSpace, f), from_copy);
}

// ------------------------------------------------------------------------------------------
Color::Color(CGColorSpaceRef inColorSpace, float f0, float f1, float f2)
{
	float	f[]	= { f0, f1, f2 };
	
	mColor.reset(CGColorCreate(inColorSpace, f), from_copy);
}

// ------------------------------------------------------------------------------------------
Color::Color(CGColorSpaceRef inColorSpace, float f0, float f1, float f2, float f3)
{
	float	f[]	= { f0, f1, f2, f3 };
	
	mColor.reset(CGColorCreate(inColorSpace, f), from_copy);
}

// ------------------------------------------------------------------------------------------
Color::Color(CGColorSpaceRef inColorSpace, float f0, float f1, float f2, float f3, float f4)
{
	float	f[]	= { f0, f1, f2, f3, f4 };
	
	mColor.reset(CGColorCreate(inColorSpace, f), from_copy);
}

// ------------------------------------------------------------------------------------------
Color
Color::clone() const
{
	return Color(CGColorCreateCopy(mColor), from_copy);
}

// ------------------------------------------------------------------------------------------
Color
Color::clone(float a) const
{
	return Color(CGColorCreateCopyWithAlpha(mColor, a), from_copy);
}

}	// namespace Graphics
}	// namespace B
