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

#ifndef BContext_H_
#define BContext_H_

#pragma once

// B headers
#include "BOSPtr.h"
#include "BRect.h"
#include "BTransform.h"


namespace B {

// forward declarations
class	MutableShape;
class	Shape;

namespace Graphics {

// forward declarations
class	Color;
class	Path;


class Context
{
public:
	
	// constructors / destructor
				Context(const Context& inContext);
	explicit	Context(CGContextRef inContext);
	explicit	Context(CGContextRef inContext, const from_copy_t&);
	explicit	Context(const OSPtr<CGContextRef>& inContext);
				
	// assignment
	Context&	operator = (const Context& inContext);
	Context&	operator = (const OSPtr<CGContextRef>& inContext);
	Context&	assign(const Context& inContext);
	Context&	assign(CGContextRef inContext);
	Context&	assign(CGContextRef inContext, const from_copy_t&);
	Context&	assign(const OSPtr<CGContextRef>& inContext);
	//! Exchanges the contents of the path with @a ioContext.
	void		swap(Context& ioContext);
	
	// CTM
	Transform	GetTransform() const;
	void		Concat(const Transform& inTransform);
	void		Translate(float tx, float ty);
	void		Translate(const CGPoint& inPoint);
	void		Scale(float sx, float sy);
	void		Scale(const CGSize& inSize);
	void		Rotate(float angle);
	
	// Clipping
	Rect		GetClipBox() const;
	void		ClipPath();
	void		ClipRect(const CGRect& inRect);
	
	// Stroking
	void		SetStrokeColorSpace(CGColorSpaceRef colorSpace);
	void		SetStrokeColor(const Color& inColor);
	void		SetRGBStrokeColor(float r, float g, float b, float a);
	void		SetGrayStrokeColor(float g, float a);
	void		SetLineWidth(float width);
	void		SetLineCap(CGLineCap cap);
	void		SetLineJoin(CGLineJoin join);
	
	// Filling
	void		SetFillColorSpace(CGColorSpaceRef colorSpace);
	void		SetFillColor(const Color& inColor);
	void		SetRGBFillColor(float r, float g, float b, float a);
	void		SetGrayFillColor(float g, float a);
	
	// Paths
	void		BeginPath();
	void		ClosePath();
	void		AddPath(const Path& inPath);
	void		MoveTo(float x, float y);
	void		MoveTo(const CGPoint& inPoint);
	void		AddLineTo(float x, float y);
	void		AddLineTo(const CGPoint& inPoint);
	void		AddRect(const CGRect& inRect);
	void		AddRects(const CGRect inRects[], size_t inCount);
	void		AddEllipse(const CGRect& inRect);
	void		SetPath(HIShapeRef inShape);
	void		SetPath(const Shape& inShape);
	void		SetPath(const MutableShape& inShape);
	void		DrawPath(CGPathDrawingMode inMode);
	void		StrokePath();
	void		StrokeRect(const CGRect& inRect);
	void		StrokeRects(const CGRect inRects[], size_t inCount);
	void		StrokeEllipse(const CGRect& inRect);
	void		FillPath();
	void		FillRect(const CGRect& inRect);
	void		FillRects(const CGRect inRects[], size_t inCount);
	void		FillEllipse(const CGRect& inRect);
	void		FillStrokePath();
	void		FillStrokeRect(const CGRect& inRect);
	void		FillStrokeRects(const CGRect inRects[], size_t inCount);
	void		FillStrokeEllipse(const CGRect& inRect);
	
	void		Clear(const CGRect& inRect);
	void		SetAlpha(float alpha);
	void		SetShadow(const CGSize& inOffset, float blur);
	
//	bool		empty() const	{ return (CGContextIsEmpty(mContext)); }
//	Point		current() const	{ return (Point(CGContextGetCurrentPoint(mContext))); }
	
	// conversions
	CGContextRef	get() const	{ return (mContext.get()); }
					operator CGContextRef () const	{ return (mContext); }
	
	// Coordinate conversions
	Transform	GetUserSpaceTransform() const;
	CGRect		ConvertToDeviceSpace(const CGRect& inRect) const;
	CGPoint		ConvertToDeviceSpace(const CGPoint& inPoint) const;
	CGSize		ConvertToDeviceSpace(const CGSize& inSize) const;
	CGRect		ConvertToUserSpace(const CGRect& inRect) const;
	CGPoint		ConvertToUserSpace(const CGPoint& inPoint) const;
	CGSize		ConvertToUserSpace(const CGSize& inSize) const;

private:
	
	//explicit	Context(CGContextRef inContextRef, const from_copy_t&);
	
	// member variables
	OSPtr<CGContextRef>	mContext;
};

// ------------------------------------------------------------------------------------------
inline
Context::Context(const Context& inContext)
	: mContext(inContext.mContext)
{
}

// ------------------------------------------------------------------------------------------
inline
Context::Context(CGContextRef inContext)
	: mContext(inContext)
{
}

// ------------------------------------------------------------------------------------------
inline
Context::Context(CGContextRef inContext, const from_copy_t& fc)
	: mContext(inContext, fc)
{
}

// ------------------------------------------------------------------------------------------
inline
Context::Context(const OSPtr<CGContextRef>& inContext)
	: mContext(inContext)
{
}

// ------------------------------------------------------------------------------------------
inline void
Context::swap(Context& ioContext)
{
	mContext.swap(ioContext.mContext);
}

// ------------------------------------------------------------------------------------------
inline Context&
Context::operator = (const Context& inContext)
{
	mContext = inContext.mContext;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Context&
Context::operator = (const OSPtr<CGContextRef>& inContext)
{
	mContext = inContext;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Context&
Context::assign(const Context& inContext)
{
	mContext = inContext.mContext;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Context&
Context::assign(CGContextRef inContext)
{
	mContext.reset(inContext);
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Context&
Context::assign(CGContextRef inContext, const from_copy_t& fc)
{
	mContext.reset(inContext, fc);
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Context&
Context::assign(const OSPtr<CGContextRef>& inContext)
{
	mContext = inContext;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline Transform
Context::GetTransform() const
{
	return Transform(CGContextGetCTM(mContext));
}

// ------------------------------------------------------------------------------------------
inline void
Context::Concat(const Transform& inTransform)
{
	CGContextConcatCTM(mContext, inTransform);
}

// ------------------------------------------------------------------------------------------
inline void
Context::Translate(float tx, float ty)
{
	CGContextTranslateCTM(mContext, tx, ty);
}

// ------------------------------------------------------------------------------------------
inline void
Context::Translate(const CGPoint& inPoint)
{
	CGContextTranslateCTM(mContext, inPoint.x, inPoint.y);
}

// ------------------------------------------------------------------------------------------
inline void
Context::Scale(float sx, float sy)
{
	CGContextScaleCTM(mContext, sx, sy);
}

// ------------------------------------------------------------------------------------------
inline void
Context::Scale(const CGSize& inSize)
{
	CGContextScaleCTM(mContext, inSize.width, inSize.height);
}

// ------------------------------------------------------------------------------------------
inline void
Context::Rotate(float angle)
{
	CGContextRotateCTM(mContext, angle);
}

// ------------------------------------------------------------------------------------------
inline Rect
Context::GetClipBox() const
{
	return Rect(CGContextGetClipBoundingBox(mContext));
}

// ------------------------------------------------------------------------------------------
inline void
Context::ClipPath()
{
	CGContextClip(mContext);
}

// ------------------------------------------------------------------------------------------
inline void
Context::ClipRect(const CGRect& inRect)
{
	CGContextClipToRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetStrokeColorSpace(CGColorSpaceRef colorSpace)
{
	CGContextSetStrokeColorSpace(mContext, colorSpace);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetRGBStrokeColor(float r, float g, float b, float a)
{
	CGContextSetRGBStrokeColor(mContext, r, g, b, a);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetGrayStrokeColor(float g, float a)
{
	CGContextSetGrayStrokeColor(mContext, g, a);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetLineWidth(float width)
{
	CGContextSetLineWidth(mContext, width);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetLineCap(CGLineCap cap)
{
	CGContextSetLineCap(mContext, cap);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetLineJoin(CGLineJoin join)
{
	CGContextSetLineJoin(mContext, join);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetFillColorSpace(CGColorSpaceRef colorSpace)
{
	CGContextSetFillColorSpace(mContext, colorSpace);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetRGBFillColor(float r, float g, float b, float a)
{
	CGContextSetRGBFillColor(mContext, r, g, b, a);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetGrayFillColor(float g, float a)
{
	CGContextSetGrayFillColor(mContext, g, a);
}

// ------------------------------------------------------------------------------------------
inline void
Context::BeginPath()
{
	CGContextBeginPath(mContext);
}

// ------------------------------------------------------------------------------------------
inline void
Context::ClosePath()
{
	CGContextClosePath(mContext);
}

// ------------------------------------------------------------------------------------------
inline void
Context::MoveTo(float x, float y)
{
	CGContextMoveToPoint(mContext, x, y);
}

// ------------------------------------------------------------------------------------------
inline void
Context::MoveTo(const CGPoint& inPoint)
{
	CGContextMoveToPoint(mContext, inPoint.x, inPoint.y);
}

// ------------------------------------------------------------------------------------------
inline void
Context::AddLineTo(float x, float y)
{
	CGContextAddLineToPoint(mContext, x, y);
}

// ------------------------------------------------------------------------------------------
inline void
Context::AddLineTo(const CGPoint& inPoint)
{
	CGContextAddLineToPoint(mContext, inPoint.x, inPoint.y);
}

// ------------------------------------------------------------------------------------------
inline void
Context::AddRect(const CGRect& inRect)
{
	CGContextAddRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::AddRects(const CGRect inRects[], size_t inCount)
{
	CGContextAddRects(mContext, inRects, inCount);
}

// ------------------------------------------------------------------------------------------
inline void
Context::AddEllipse(const CGRect& inRect)
{
	CGContextAddEllipseInRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::DrawPath(CGPathDrawingMode inMode)
{
	CGContextDrawPath(mContext, inMode);
}

// ------------------------------------------------------------------------------------------
inline void
Context::StrokePath()
{
	CGContextStrokePath(mContext);
}

// ------------------------------------------------------------------------------------------
inline void
Context::StrokeRect(const CGRect& inRect)
{
	CGContextStrokeRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::StrokeEllipse(const CGRect& inRect)
{
	CGContextStrokeEllipseInRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::FillPath()
{
	CGContextFillPath(mContext);
}

// ------------------------------------------------------------------------------------------
inline void
Context::FillRect(const CGRect& inRect)
{
	CGContextFillRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::FillRects(const CGRect inRects[], size_t inCount)
{
	CGContextFillRects(mContext, inRects, inCount);
}

// ------------------------------------------------------------------------------------------
inline void
Context::FillEllipse(const CGRect& inRect)
{
	CGContextFillEllipseInRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::FillStrokePath()
{
	CGContextDrawPath(mContext, kCGPathFillStroke);
}

// ------------------------------------------------------------------------------------------
inline void
Context::Clear(const CGRect& inRect)
{
	CGContextClearRect(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetAlpha(float alpha)
{
	CGContextSetAlpha(mContext, alpha);
}

// ------------------------------------------------------------------------------------------
inline void
Context::SetShadow(const CGSize& inOffset, float blur)
{
	CGContextSetShadow(mContext, inOffset, blur);
}

// ------------------------------------------------------------------------------------------
inline CGRect
Context::ConvertToDeviceSpace(const CGRect& inRect) const
{
	return CGContextConvertRectToDeviceSpace(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline CGPoint
Context::ConvertToDeviceSpace(const CGPoint& inPoint) const
{
	return CGContextConvertPointToDeviceSpace(mContext, inPoint);
}

// ------------------------------------------------------------------------------------------
inline CGSize
Context::ConvertToDeviceSpace(const CGSize& inSize) const
{
	return CGContextConvertSizeToDeviceSpace(mContext, inSize);
}

// ------------------------------------------------------------------------------------------
inline CGRect
Context::ConvertToUserSpace(const CGRect& inRect) const
{
	return CGContextConvertRectToUserSpace(mContext, inRect);
}

// ------------------------------------------------------------------------------------------
inline CGPoint
Context::ConvertToUserSpace(const CGPoint& inPoint) const
{
	return CGContextConvertPointToUserSpace(mContext, inPoint);
}

// ------------------------------------------------------------------------------------------
inline CGSize
Context::ConvertToUserSpace(const CGSize& inSize) const
{
	return CGContextConvertSizeToUserSpace(mContext, inSize);
}


//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a c1 and @a c2.

	@relates	Path
*/
template <typename T> inline void
swap(Context& c1, Context& c2)	{ c1.swap(c2); }

//@}

}	// namespace Graphics
}	// namespace B


#endif	// BContext_H_
