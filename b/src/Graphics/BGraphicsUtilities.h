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

#ifndef BGraphicsUtilities_H_
#define BGraphicsUtilities_H_

#pragma once

// B headers
#include "BTransform.h"


namespace B {
namespace Graphics {


// forward declarations
class	Context;


/*!
	@brief	Automatic graphics state management.
	
	The constructor pushes the graphics state, and the destructor pop it.
*/
class AutoSaveContext : public boost::noncopyable
{
public:
	
	AutoSaveContext(Context& context);
	AutoSaveContext(CGContextRef context);
	~AutoSaveContext();
	
private:
	
	CGContextRef	mContext;
};


/*!
	@brief	Transparency layer lifecycle management.
	
	The constructor begins a transparency layer, and the destructor ends it.
*/
class AutoTransparencyLayer : public boost::noncopyable
{
public:
	
	AutoTransparencyLayer(
		Context&		context, 
		CFDictionaryRef	info = NULL);
	AutoTransparencyLayer(
		CGContextRef	context, 
		CFDictionaryRef	info = NULL);
	~AutoTransparencyLayer();
	
private:
	
	CGContextRef	mContext;
};


/*!
	@brief	Page lifecycle management.
	
	The constructor begins a new page, and the destructor ends it.
	
	NOTE: Pages cannot be nested.
*/
class AutoPage : public boost::noncopyable
{
public:
	
	AutoPage(
		Context&		context, 
		const CGRect*	inMediaBox = NULL);
	AutoPage(
		CGContextRef	context, 
		const CGRect*	inMediaBox = NULL);
	~AutoPage();
	
private:
	
	CGContextRef	mContext;
};


/*!
	@brief	PDF Page lifecycle management.
	
	The constructor begins a new PDF page, and the destructor ends it.
	
	NOTE: Pages cannot be nested.
*/
class AutoPDFPage : public boost::noncopyable
{
public:
	
	AutoPDFPage(
		Context&		context, 
		CFDictionaryRef	info = NULL);
	AutoPDFPage(
		CGContextRef	context, 
		CFDictionaryRef	info = NULL);
	~AutoPDFPage();
	
private:
	
	CGContextRef	mContext;
};


}	// namespace Graphics
}	// namespace B


#endif	// BGraphicsUtilities_H_
