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
#include "BGraphicsUtilities.h"

// B headers
#include "BContext.h"


namespace B {
namespace Graphics {

// ==========================================================================================
//	AutoSaveContext

// ------------------------------------------------------------------------------------------
AutoSaveContext::AutoSaveContext(Context& context)
	: mContext(context.get())
{
	CGContextSaveGState(mContext);
}

// ------------------------------------------------------------------------------------------
AutoSaveContext::AutoSaveContext(CGContextRef context)
	: mContext(context)
{
	CGContextSaveGState(mContext);
}

// ------------------------------------------------------------------------------------------
AutoSaveContext::~AutoSaveContext()
{
	CGContextRestoreGState(mContext);
}


// ==========================================================================================
//	AutoTransparencyLayer

// ------------------------------------------------------------------------------------------
AutoTransparencyLayer::AutoTransparencyLayer(
	Context&		context, 
	CFDictionaryRef	info /* = NULL */)
	: mContext(context.get())
{
	CGContextBeginTransparencyLayer(mContext, info);
}

// ------------------------------------------------------------------------------------------
AutoTransparencyLayer::AutoTransparencyLayer(
	CGContextRef	context, 
	CFDictionaryRef	info /* = NULL */)
		: mContext(context)
{
	CGContextBeginTransparencyLayer(mContext, info);
}

// ------------------------------------------------------------------------------------------
AutoTransparencyLayer::~AutoTransparencyLayer()
{
	CGContextEndTransparencyLayer(mContext);
}


// ==========================================================================================
//	AutoPage

// ------------------------------------------------------------------------------------------
AutoPage::AutoPage(
	Context&		context, 
	const CGRect*	inMediaBox /* = NULL */)
		: mContext(context.get())
{
	CGContextBeginPage(mContext, inMediaBox);
}

// ------------------------------------------------------------------------------------------
AutoPage::AutoPage(
	CGContextRef	context, 
	const CGRect*	inMediaBox /* = NULL */)
		: mContext(context)
{
	CGContextBeginPage(mContext, inMediaBox);
}

// ------------------------------------------------------------------------------------------
AutoPage::~AutoPage()
{
	CGContextEndPage(mContext);
}


// ==========================================================================================
//	AutoPDFPage

// ------------------------------------------------------------------------------------------
AutoPDFPage::AutoPDFPage(
	Context&		context, 
	CFDictionaryRef	info /* = NULL */)
		: mContext(context.get())
{
	CGPDFContextBeginPage(mContext, info);
}

// ------------------------------------------------------------------------------------------
AutoPDFPage::AutoPDFPage(
	CGContextRef	context, 
	CFDictionaryRef	info /* = NULL */)
		: mContext(context)
{
	CGPDFContextBeginPage(mContext, info);
}

// ------------------------------------------------------------------------------------------
AutoPDFPage::~AutoPDFPage()
{
	CGPDFContextEndPage(mContext);
}

}	// namespace Graphics
}	// namespace B
