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

#ifndef BOpenGLUtilities_H_
#define BOpenGLUtilities_H_

#pragma	once

// system headers
#include <AGL/agl.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BException.h"


namespace B {


// ==========================================================================================
//	AutoSetAglContext

class AutoSetAglContext : public boost::noncopyable
{
public:
	
	AutoSetAglContext(AGLContext inContext);
	~AutoSetAglContext();

private:
	
	AGLContext	mSavedContext;
};


// ==========================================================================================
//	AutoAglContext

class AutoAglContext
{
public:
	
				AutoAglContext();
				AutoAglContext(AutoAglContext& ioContext);
	explicit	AutoAglContext(AGLContext inContext);
				~AutoAglContext();
	
	AutoAglContext&	operator = (AutoAglContext& ioContext);
	
				operator AGLContext () const	{ return (mContext); }
	AGLContext	get() const						{ return (mContext); }
	AGLContext	release();
	void		reset(AGLContext inContext = NULL);

private:
	
	AGLContext	mContext;
};

// ------------------------------------------------------------------------------------------
inline
AutoAglContext::AutoAglContext()
	: mContext(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoAglContext::AutoAglContext(AutoAglContext& ioContext)
	: mContext(NULL)
{
	std::swap(mContext, ioContext.mContext);
}

// ------------------------------------------------------------------------------------------
inline
AutoAglContext::~AutoAglContext()
{
	reset();
}

// ------------------------------------------------------------------------------------------
inline AutoAglContext&
AutoAglContext::operator = (AutoAglContext& ioContext)
{
	reset();
	std::swap(mContext, ioContext.mContext);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
AutoAglContext::reset(AGLContext inContext /* = NULL */)
{
	if (mContext != NULL)
		aglDestroyContext(mContext);
	
	mContext = inContext;
}

// ------------------------------------------------------------------------------------------
inline AGLContext
AutoAglContext::release()
{
	AGLContext	oldContext	= mContext;
	
	mContext = NULL;
	
	return (oldContext);
}


// ==========================================================================================
//	AutoAglPixelFormat

class AutoAglPixelFormat
{
public:
	
				AutoAglPixelFormat();
				AutoAglPixelFormat(AutoAglPixelFormat& ioFormat);
	explicit	AutoAglPixelFormat(AGLPixelFormat inFormat);
				~AutoAglPixelFormat();
	
	AutoAglPixelFormat&
				operator = (AutoAglPixelFormat& ioFormat);
	
					operator AGLPixelFormat () const	{ return (mPixelFormat); }
	AGLPixelFormat	get() const							{ return (mPixelFormat); }
	AGLPixelFormat	release();
	void			reset(AGLPixelFormat inFormat = NULL);

private:
	
	AGLPixelFormat	mPixelFormat;
};

// ------------------------------------------------------------------------------------------
inline
AutoAglPixelFormat::AutoAglPixelFormat()
	: mPixelFormat(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoAglPixelFormat::AutoAglPixelFormat(AutoAglPixelFormat& ioFormat)
	: mPixelFormat(NULL)
{
	std::swap(mPixelFormat, ioFormat.mPixelFormat);
}

// ------------------------------------------------------------------------------------------
inline
AutoAglPixelFormat::~AutoAglPixelFormat()
{
	reset();
}

// ------------------------------------------------------------------------------------------
inline AutoAglPixelFormat&
AutoAglPixelFormat::operator = (AutoAglPixelFormat& ioFormat)
{
	reset();
	std::swap(mPixelFormat, ioFormat.mPixelFormat);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
AutoAglPixelFormat::reset(AGLPixelFormat inFormat /* = NULL */)
{
	if (mPixelFormat != NULL)
		aglDestroyPixelFormat(mPixelFormat);
	
	mPixelFormat = inFormat;
}

// ------------------------------------------------------------------------------------------
inline AGLPixelFormat
AutoAglPixelFormat::release()
{
	AGLPixelFormat	oldContext	= mPixelFormat;
	
	mPixelFormat = NULL;
	
	return (oldContext);
}


// ==========================================================================================
//	AglException

#pragma mark AglException

/*!
	@brief	<tt>AGL</tt> exception class
	
	This class contains an AGL-defined error code.  Typically an exception of 
	this class is thrown after an AGL-defined function returns a non-zero 
	result code.
	
	In all instances of this class, @c what() returns "B::AglException".
*/
class AglException : public std::exception
{
public:
	
	static void	Throw(const char* file, int line, const char* func);
	static void	Throw();
	
	//! @c GLenum Constructor.
	explicit	AglException(GLenum inError);
	//! Stream constructor.
	explicit	AglException(std::istream& istr);
	//! Destructor
	virtual		~AglException() throw();
	
	//! Returns the AGL-defined error code.
	GLenum				GetError() const		{ return (mError); }
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
	
	//! Writes out the exception's internal state to @a ostr.
	virtual void	Write(std::ostream& ostr) const;
	
private:
	
	// member variables
	GLenum	mError;
};

template <> struct ExceptionTraits<AglException>	{ typedef StreamExceptionTag	Category; };


// ==========================================================================================
//	Exception-throwing macros

#ifndef NDEBUG

#	define ThrowIfAgl(b)	\
		do { if (!(b)) B::AglException::Throw(__FILE__, __LINE__, __func__); } while (0)

#else

#	define ThrowIfAgl(b)	\
		do { if (!(b)) B::AglException::Throw(); } while (0)

#endif


}	// namespace B

#endif	// BOpenGLUtilities_H_
