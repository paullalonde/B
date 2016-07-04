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
#include "BContext.h"

// B headers
#include "BColor.h"
#include "BMutableShape.h"
#include "BPath.h"
#include "BShape.h"


namespace B {
namespace Graphics {


// ------------------------------------------------------------------------------------------
void
Context::SetStrokeColor(const Color& inColor)
{
	CGContextSetStrokeColorWithColor(mContext, inColor);
}

// ------------------------------------------------------------------------------------------
void
Context::SetFillColor(const Color& inColor)
{
	CGContextSetFillColorWithColor(mContext, inColor);
}

// ------------------------------------------------------------------------------------------
void
Context::AddPath(const Path& inPath)
{
	CGContextAddPath(mContext, inPath);
}

// ------------------------------------------------------------------------------------------
void
Context::SetPath(HIShapeRef inShape)
{
	OSStatus	err;
	
	err = HIShapeReplacePathInCGContext(inShape, mContext);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Context::SetPath(const Shape& inShape)
{
	OSStatus	err;
	
	err = HIShapeReplacePathInCGContext(inShape.cf_ref(), mContext);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Context::SetPath(const MutableShape& inShape)
{
	OSStatus	err;
	
	err = HIShapeReplacePathInCGContext(inShape.cf_ref(), mContext);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Context::StrokeRects(const CGRect inRects[], size_t inCount)
{
	CGContextBeginPath(mContext);
	CGContextAddRects(mContext, inRects, inCount);
	CGContextStrokePath(mContext);
}

// ------------------------------------------------------------------------------------------
void
Context::FillStrokeRect(const CGRect& inRect)
{
	CGContextBeginPath(mContext);
	CGContextAddRect(mContext, inRect);
	CGContextDrawPath(mContext, kCGPathFillStroke);
}

// ------------------------------------------------------------------------------------------
void
Context::FillStrokeRects(const CGRect inRects[], size_t inCount)
{
	CGContextBeginPath(mContext);
	CGContextAddRects(mContext, inRects, inCount);
	CGContextDrawPath(mContext, kCGPathFillStroke);
}

// ------------------------------------------------------------------------------------------
void
Context::FillStrokeEllipse(const CGRect& inRect)
{
	CGContextBeginPath(mContext);
	CGContextAddEllipseInRect(mContext, inRect);
	CGContextDrawPath(mContext, kCGPathFillStroke);
}

// ------------------------------------------------------------------------------------------
Transform
Context::GetUserSpaceTransform() const
{
	return Transform(CGContextGetUserSpaceToDeviceSpaceTransform(mContext));
}

}	// namespace Graphics
}	// namespace B
