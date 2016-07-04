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
#include "BOpenGLUtilities.h"

// standard headers
#include <istream>

// B headers
#include "BErrorHandler.h"
#include "BExceptionStreamer.h"
#include "BFwd.h"


namespace B {

// ==========================================================================================
//	AutoSetAglContext

// ------------------------------------------------------------------------------------------
AutoSetAglContext::AutoSetAglContext(AGLContext inContext)
	: mSavedContext(aglGetCurrentContext())
{
	B_ASSERT(inContext != NULL);
	
	ThrowIfAgl(aglSetCurrentContext(inContext));
}

// ------------------------------------------------------------------------------------------
AutoSetAglContext::~AutoSetAglContext()
{
	ThrowIfAgl(aglSetCurrentContext(mSavedContext));
}


// ==========================================================================================
//	AutoAglContext

// ------------------------------------------------------------------------------------------
AutoAglContext::AutoAglContext(AGLContext inContext)
	: mContext(inContext)
{
	B_THROW_IF_NULL(inContext);
}


// ==========================================================================================
//	AutoAglPixelFormat

// ------------------------------------------------------------------------------------------
AutoAglPixelFormat::AutoAglPixelFormat(AGLPixelFormat inFormat)
	: mPixelFormat(inFormat)
{
	B_THROW_IF_NULL(inFormat);
}


// ==========================================================================================
//	AglException

#pragma mark -

// ------------------------------------------------------------------------------------------
void
AglException::Throw(const char* file, int line, const char* func)
{
	GLenum	error	= aglGetError();
	
	if (error == AGL_BAD_ALLOC)
	{
		ErrorHandler::ThrowBadAllocException(file, line, func);
	}
	else
	{
		ErrorHandler::ThrowException(AglException(error), file, line, func);
	}
}

// ------------------------------------------------------------------------------------------
void
AglException::Throw()
{
	GLenum	error	= aglGetError();
	
	if (error == AGL_BAD_ALLOC)
	{
		ErrorHandler::ThrowBadAllocException();
	}
	else
	{
		ErrorHandler::ThrowException(AglException(error));
	}
}

// ------------------------------------------------------------------------------------------
AglException::AglException(
	GLenum inError)	//!< The error code.
		: mError(inError)
{
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to reading in their own state.
*/
AglException::AglException(
	std::istream&	istr)	//!< The input stream.
{
	istr >> mError;
}

// ------------------------------------------------------------------------------------------
AglException::~AglException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
AglException::what() const throw()
{
	return ("B::AglException");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
AglException::Write(std::ostream& ostr) const
{
	ostr << mError << "\n";
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAglException = ExceptionStreamer::Register<AglException>();
#endif

}	// namespace B
