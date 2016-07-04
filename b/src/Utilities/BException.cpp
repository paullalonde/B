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
#include "BException.h"

// standard headers
#include <errno.h>
#include <stdio.h>

// library headers
#include "MoreOSL.h"

// B headers
#include "BErrorHandler.h"
#include "BExceptionStreamer.h"
#include "BFwd.h"
#include "BMutableString.h"
#include "BUrl.h"


namespace B {


// ==========================================================================================
//	ExceptionStringHolder

#pragma mark -

// ------------------------------------------------------------------------------------------
ExceptionStringHolder::ExceptionStringHolder() throw()
	: mString(CFSTR("")), mCachedBuff(NULL)
{
	CFRetain(mString);
}

// ------------------------------------------------------------------------------------------
ExceptionStringHolder::ExceptionStringHolder(
	const ExceptionStringHolder&	inHolder)	//!< The original exception.
	throw()
		: mString(inHolder.mString), mCachedBuff(NULL)
{
	CFRetain(mString);
}

// ------------------------------------------------------------------------------------------
ExceptionStringHolder::ExceptionStringHolder(
	CFStringRef	inString)	//!< The error message.
	throw()
		: mString(inString), mCachedBuff(NULL)
{
	B_ASSERT(inString != NULL);
	
	CFRetain(mString);
}

// ------------------------------------------------------------------------------------------
ExceptionStringHolder::ExceptionStringHolder(
	CFStringRef	inString,	//!< The error message;  may be @c NULL.
	CFStringRef inFallback)	//!< The error message to use if @a inString is @c NULL.
	throw()
		: mString(inString), mCachedBuff(NULL)
{
	if (mString == NULL)
	{
		B_ASSERT(inFallback != NULL);
		
		mString = inFallback;
	}
	
	CFRetain(mString);
}

// ------------------------------------------------------------------------------------------
/*!	The input string is assumed to be encoded as UTF-8.
*/
ExceptionStringHolder::ExceptionStringHolder(
	const std::string&	str)	//!< The error message.
		: mCachedBuff(NULL)
{
	String	temp(str, kCFStringEncodingUTF8);
	
	mString = temp.cf_ref();
	CFRetain(mString);
}

// ------------------------------------------------------------------------------------------
/*!	The entire contents of @a istr are read into the object's string.
	
	The input stream is assumed to be encoded as UTF-8.
*/
ExceptionStringHolder::ExceptionStringHolder(
	std::istream&	istr)	//!< Input stream containing the error message.
		: mString(NULL), mCachedBuff(NULL)
{
	Read(istr);
}

// ------------------------------------------------------------------------------------------
ExceptionStringHolder::~ExceptionStringHolder() throw()
{
	// Note:	We tolerate a non-NULL mString, and set the member variables to NULL, in 
	//			in order to make ourselves immune to double-destruction.
	
	if (mCachedBuff != NULL)
	{
		delete [] mCachedBuff;
		mCachedBuff = NULL;
	}
	
	if (mString != NULL)
	{
		CFRelease(mString);
		mString = NULL;
	}
}

// ------------------------------------------------------------------------------------------
ExceptionStringHolder&
ExceptionStringHolder::operator = (
	const ExceptionStringHolder&	ex)	//!< The original exception.
	throw()
{
	if (&ex != this)
	{
		B_ASSERT(ex.mString != NULL);
		
		CFRetain(ex.mString);
		CFRelease(mString);
		mString = ex.mString;
		
		delete [] mCachedBuff;
		mCachedBuff = NULL;
	}
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	The returned string is encoded as UTF-8.
*/
const char*
ExceptionStringHolder::GetString(const char* inFallback) const throw()
{
    B_ASSERT(inFallback != NULL);
    
    const char* str;
    
	if (mCachedBuff == NULL)
	{
        // We haven't got a cached buffer.  Allocate one and copy mString into it, 
        // and transcode it into UTF-8 while we're at it.  We can't throw in this 
        // function, so catch everything and if something goes wrong we return the 
        // fallback string.
        
        try
        {
            String		errorStr(mString, false);
            std::string	tempStr;
            
            errorStr.copy(tempStr, kCFStringEncodingUTF8);
            
            mCachedBuff = new char [tempStr.size()+1];
            strcpy(mCachedBuff, tempStr.c_str());
            
            str = mCachedBuff;
        }
        catch (...)
        {
            str = inFallback;
        }
	}
	else
	{
		str = mCachedBuff;
	}
	
	return (str);
}

// ------------------------------------------------------------------------------------------
/*!	The entire contents of @a istr are read into the object's string.
	
	The input stream is assumed to be encoded as UTF-8.
*/
void
ExceptionStringHolder::Read(
	std::istream&	istr)	//!< Input stream containing the error message.
{
	char		buffer[256];
	std::string	str;
	size_t		nread;
	
	do
	{
		nread = istr.readsome(buffer, sizeof(buffer));
		
		str.append(buffer, nread);
		
	} while (nread > 0);
	
	String	temp(str, kCFStringEncodingUTF8);
	
	if (mString != NULL)
		CFRelease(mString);
	
	mString = temp.cf_ref();
	CFRetain(mString);
	
	delete [] mCachedBuff;
	mCachedBuff = NULL;
}

// ------------------------------------------------------------------------------------------
/*!	The output stream is assumed to be encoded as UTF-8.
*/
void
ExceptionStringHolder::Write(std::ostream& ostr) const
{
    String	temp(mString);
	
	temp.Write(ostr, kCFStringEncodingUTF8);
}


// ==========================================================================================
//	IOException

#pragma mark -

// ------------------------------------------------------------------------------------------
IOException::~IOException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
IOException::what() const throw()
{
	return ("B::IOException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredIOException = ExceptionStreamer::Register<IOException>();
#endif


// ==========================================================================================
//	FileNotFoundException

#pragma mark -

// ------------------------------------------------------------------------------------------
FileNotFoundException::~FileNotFoundException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
FileNotFoundException::what() const throw()
{
	return ("B::FileNotFoundException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredFileNotFoundException = ExceptionStreamer::Register<FileNotFoundException>();
#endif


// ==========================================================================================
//	OpenException

#pragma mark -

// ------------------------------------------------------------------------------------------
OpenException::~OpenException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
OpenException::what() const throw()
{
	return ("B::OpenException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredOpenException = ExceptionStreamer::Register<OpenException>();
#endif


// ==========================================================================================
//	EOFException

#pragma mark -

// ------------------------------------------------------------------------------------------
EOFException::~EOFException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
EOFException::what() const throw()
{
	return ("B::EOFException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredEOFException = ExceptionStreamer::Register<EOFException>();
#endif


// ==========================================================================================
//	ReadException

#pragma mark -

// ------------------------------------------------------------------------------------------
ReadException::~ReadException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
ReadException::what() const throw()
{
	return ("B::ReadException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredReadException = ExceptionStreamer::Register<ReadException>();
#endif


// ==========================================================================================
//	WriteException

#pragma mark -

// ------------------------------------------------------------------------------------------
WriteException::~WriteException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
WriteException::what() const throw()
{
	return ("B::WriteException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredWriteException = ExceptionStreamer::Register<WriteException>();
#endif


// ==========================================================================================
//	OSStatusException

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatusException::~OSStatusException() throw()
{
}


// ==========================================================================================
//	RuntimeOSStatusException

#pragma mark -

// ------------------------------------------------------------------------------------------
RuntimeOSStatusException::RuntimeOSStatusException(
	const RuntimeOSStatusException&	ex)	//!< The original exception.
	throw()
		: mStatus(ex.mStatus)
{
}

// ------------------------------------------------------------------------------------------
RuntimeOSStatusException::RuntimeOSStatusException(
	OSStatus	inStatus)	//!< The error code.
	throw()
		: mStatus(inStatus)
{
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to reading in their own state.
*/
RuntimeOSStatusException::RuntimeOSStatusException(
	std::istream&	istr)	//!< The input stream.
{
	istr >> mStatus;
}

// ------------------------------------------------------------------------------------------
RuntimeOSStatusException::~RuntimeOSStatusException() throw()
{
}

// ------------------------------------------------------------------------------------------
RuntimeOSStatusException&
RuntimeOSStatusException::operator = (
	const RuntimeOSStatusException&	ex)	//!< The original exception.
	throw()
{
	mStatus = ex.mStatus;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
const char*
RuntimeOSStatusException::what() const throw()
{
	return ("B::RuntimeOSStatusException");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
RuntimeOSStatusException::Write(std::ostream& ostr) const
{
	ostr << mStatus << "\n";
}

#ifndef DOXYGEN_SKIP
bool gRegisteredOSStatusException = ExceptionStreamer::Register<RuntimeOSStatusException>();
#endif


// ==========================================================================================
//	RethrownException

#pragma mark -

const CFStringRef	RethrownException::sDefaultMsg	= CFSTR("B::RethrownException");

// ------------------------------------------------------------------------------------------
RethrownException::RethrownException(
	const RethrownException&	ex)	//!< The original exception.
	throw()
		: mStatus(ex.mStatus), mHolder(ex.mHolder)
{
}

// ------------------------------------------------------------------------------------------
RethrownException::RethrownException(
	OSStatus	inStatus,	//!< The error code.
	CFStringRef	inErrorMsg)	//!< The error message, which should already be localised;  may be @c NULL.
	throw()
		: mStatus(inStatus), mHolder(inErrorMsg, sDefaultMsg)
{
}

// ------------------------------------------------------------------------------------------
RethrownException::RethrownException(
	std::istream&	istr)	//!< The input stream.
{
	istr >> mStatus;
	mHolder.Read(istr);
}

// ------------------------------------------------------------------------------------------
RethrownException::~RethrownException() throw()
{
}

// ------------------------------------------------------------------------------------------
RethrownException&
RethrownException::operator = (
	const RethrownException&	ex)	//!< The original exception.
	throw()
{
	if (&ex != this)
	{
		mStatus	= ex.mStatus;
		mHolder	= ex.mHolder;
	}
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
OSStatus
RethrownException::GetStatus() const throw()
{
	return (mStatus);
}

// ------------------------------------------------------------------------------------------
const char*
RethrownException::what() const throw()
{
	return (mHolder.GetString("B::RethrownException"));
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
RethrownException::Write(std::ostream& ostr) const
{
	ostr << mStatus << " ";
	mHolder.Write(ostr);
}

#ifndef DOXYGEN_SKIP
bool gRegisteredRethrownException = ExceptionStreamer::Register<RethrownException>();
#endif


// ==========================================================================================
//	PropertyListCreateException

#pragma mark -

// ------------------------------------------------------------------------------------------
PropertyListCreateException::PropertyListCreateException(
	const PropertyListCreateException&	ex)	//!< The original exception.
	throw()
		: std::exception(ex), 
		  mHolder(ex.mHolder)
{
}

// ------------------------------------------------------------------------------------------
PropertyListCreateException::PropertyListCreateException(
	CFStringRef inErrorMsg)	//!< The error message returned by the function that failed.
	throw()
		: mHolder(inErrorMsg)
{
}

// ------------------------------------------------------------------------------------------
PropertyListCreateException::PropertyListCreateException(
	std::istream&	istr)	//!< The input stream.
		: mHolder(istr)
{
}

// ------------------------------------------------------------------------------------------
PropertyListCreateException::~PropertyListCreateException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
PropertyListCreateException::what() const throw()
{
	return ("B::PropertyListCreateException");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
PropertyListCreateException::Write(std::ostream& ostr) const
{
	mHolder.Write(ostr);
}

#ifndef DOXYGEN_SKIP
bool gRegisteredPropertyListCreateException = ExceptionStreamer::Register<PropertyListCreateException>();
#endif


// ==========================================================================================
//	MalformedUrlException

#pragma mark -

// ------------------------------------------------------------------------------------------
MalformedUrlException::~MalformedUrlException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
MalformedUrlException::what() const throw()
{
	return ("B::MalformedUrlException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredMalformedUrlException = ExceptionStreamer::Register<MalformedUrlException>();
#endif


// ==========================================================================================
//	UnsupportedUrlSchemeException

#pragma mark -

// ------------------------------------------------------------------------------------------
UnsupportedUrlSchemeException::~UnsupportedUrlSchemeException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
UnsupportedUrlSchemeException::what() const throw()
{
	return ("B::UnsupportedUrlSchemeException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredUnsupportedUrlSchemeException = ExceptionStreamer::Register<UnsupportedUrlSchemeException>();
#endif


// ==========================================================================================
//	CharacterEncodingException

#pragma mark -

// ------------------------------------------------------------------------------------------
CharacterEncodingException::~CharacterEncodingException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
CharacterEncodingException::what() const throw()
{
	return ("B::CharacterEncodingException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredCharacterEncodingException = ExceptionStreamer::Register<CharacterEncodingException>();
#endif


// ==========================================================================================
//	InteractionTimeoutException

#pragma mark -

// ------------------------------------------------------------------------------------------
InteractionTimeoutException::~InteractionTimeoutException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
InteractionTimeoutException::what() const throw()
{
	return ("B::InteractionTimeoutException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredInteractionTimeoutException = ExceptionStreamer::Register<InteractionTimeoutException>();
#endif


// ==========================================================================================
//	FSExchangeObjectsException

#pragma mark -

// ------------------------------------------------------------------------------------------
FSExchangeObjectsException::FSExchangeObjectsException(
	OSStatus		inError,	//!< The error code result from @c FSExchangeObjectsCompat().
	const FSRef&	inSrcRef,	//!< The identity of the source file at the time of failure.
	const FSRef&	inDstRef)	//!< The identity of the destination file at the time of failure.
	throw()
		: RuntimeOSStatusException(inError), 
		  mSrcRef(inSrcRef), mDstRef(inDstRef)
{
}

// ------------------------------------------------------------------------------------------
FSExchangeObjectsException::FSExchangeObjectsException(
	std::istream&	istr)	//!< The input stream.
		: RuntimeOSStatusException(istr)
{
	Url			tempUrl;
	std::string	tempBuffer;
	
	// Read in the source file ref.
	
	std::getline(istr, tempBuffer);
	tempUrl.Assign(tempBuffer);
	tempUrl.Copy(mSrcRef);
	
	// Read in the destination file ref.
	
	std::getline(istr, tempBuffer);
	tempUrl.Assign(tempBuffer);
	tempUrl.Copy(mDstRef);
}

// ------------------------------------------------------------------------------------------
FSExchangeObjectsException::~FSExchangeObjectsException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
FSExchangeObjectsException::what() const throw()
{
	return ("B::FSExchangeObjectsException");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
FSExchangeObjectsException::Write(std::ostream& ostr) const
{
	RuntimeOSStatusException::Write(ostr);
	
	Url			tempUrl;
	std::string	tempBuffer;
	
	// Write out the source file ref.
	
	tempUrl = mSrcRef;
	tempUrl.Copy(tempBuffer, true);
	
	ostr << tempBuffer << "\n";
	
	// Write out the destination file ref.
	
	tempUrl = mDstRef;
	tempUrl.Copy(tempBuffer, true);
	
	ostr << tempBuffer << "\n";
}

#ifndef DOXYGEN_SKIP
bool gRegisteredFSExchangeObjectsException = ExceptionStreamer::Register<FSExchangeObjectsException>();
#endif


// ==========================================================================================
//	UserCanceledException

#ifndef DOXYGEN_SKIP
bool gRegisteredUserCanceledException = ExceptionStreamer::Register<UserCanceledException>();
#endif


// ==========================================================================================
//	AENoSuchObjectException

#ifndef DOXYGEN_SKIP
bool gRegisteredAENoSuchObjectException = ExceptionStreamer::Register<AENoSuchObjectException>();
#endif


// ==========================================================================================
//	AECantHandleClassException

#ifndef DOXYGEN_SKIP
bool gRegisteredAECantHandleClassException = ExceptionStreamer::Register<AECantHandleClassException>();
#endif


// ==========================================================================================
//	AENotModifiableException

#ifndef DOXYGEN_SKIP
bool gRegisteredAENotModifiableException = ExceptionStreamer::Register<AENotModifiableException>();
#endif


// ==========================================================================================
//	AEBadKeyFormException

#ifndef DOXYGEN_SKIP
bool gRegisteredAEBadKeyFormException = ExceptionStreamer::Register<AEBadKeyFormException>();
#endif


// ==========================================================================================
//	AECantPutThatThereException

#ifndef DOXYGEN_SKIP
bool gRegisteredAECantPutThatThereException = ExceptionStreamer::Register<AECantPutThatThereException>();
#endif


// ==========================================================================================
//	AEWrongDataTypeException

#ifndef DOXYGEN_SKIP
bool gRegisteredAEWrongDataTypeException = ExceptionStreamer::Register<AEWrongDataTypeException>();
#endif


// ==========================================================================================
//	AECoercionFailException

#ifndef DOXYGEN_SKIP
bool gRegisteredAECoercionFailException = ExceptionStreamer::Register<AECoercionFailException>();
#endif


// ==========================================================================================
//	AEEventNotHandledException

#ifndef DOXYGEN_SKIP
bool gRegisteredAEEventNotHandledException = ExceptionStreamer::Register<AEEventNotHandledException>();
#endif


// ==========================================================================================
//	AEEventFailedException

#ifndef DOXYGEN_SKIP
bool gRegisteredAEEventFailedException = ExceptionStreamer::Register<AEEventFailedException>();
#endif


// ==========================================================================================
//	AENoUserInteractionException

#ifndef DOXYGEN_SKIP
bool gRegisteredAENoUserInteractionException = ExceptionStreamer::Register<AENoUserInteractionException>();
#endif


// ==========================================================================================
//	AEClassHasNoElementsOfThisTypeException

#pragma mark -

// ------------------------------------------------------------------------------------------
AEClassHasNoElementsOfThisTypeException::~AEClassHasNoElementsOfThisTypeException() throw()
{
}

// ------------------------------------------------------------------------------------------
OSStatus
AEClassHasNoElementsOfThisTypeException::GetStatus() const throw()
{
	return (kMOSLClassHasNoElementsOfThisTypeErr);
}

// ------------------------------------------------------------------------------------------
const char*
AEClassHasNoElementsOfThisTypeException::what() const throw()
{
	return ("B::AEClassHasNoElementsOfThisTypeException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAEClassHasNoElementsOfThisTypeException = ExceptionStreamer::Register<AEClassHasNoElementsOfThisTypeException>();
#endif


// ==========================================================================================
//	AEUnrecognisedOperatorException

#pragma mark -

// ------------------------------------------------------------------------------------------
AEUnrecognisedOperatorException::~AEUnrecognisedOperatorException() throw()
{
}

// ------------------------------------------------------------------------------------------
OSStatus
AEUnrecognisedOperatorException::GetStatus() const throw()
{
	return (kMOSLUnrecognisedOperatorErr);
}

// ------------------------------------------------------------------------------------------
const char*
AEUnrecognisedOperatorException::what() const throw()
{
	return ("B::AEUnrecognisedOperatorException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAEUnrecognisedOperatorException = ExceptionStreamer::Register<AEUnrecognisedOperatorException>();
#endif


// ==========================================================================================
//	AEDirectObjectRequiredException

#pragma mark -

// ------------------------------------------------------------------------------------------
AEDirectObjectRequiredException::~AEDirectObjectRequiredException() throw()
{
}

// ------------------------------------------------------------------------------------------
OSStatus
AEDirectObjectRequiredException::GetStatus() const throw()
{
	return (kMOSLDirectObjectRequiredErr);
}

// ------------------------------------------------------------------------------------------
const char*
AEDirectObjectRequiredException::what() const throw()
{
	return ("B::AEDirectObjectRequiredException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAEDirectObjectRequiredException = ExceptionStreamer::Register<AEDirectObjectRequiredException>();
#endif


// ==========================================================================================
//	AEDirectObjectNotAllowedException

#pragma mark -

// ------------------------------------------------------------------------------------------
AEDirectObjectNotAllowedException::~AEDirectObjectNotAllowedException() throw()
{
}

// ------------------------------------------------------------------------------------------
OSStatus
AEDirectObjectNotAllowedException::GetStatus() const throw()
{
	return (kMOSLDirectObjectNotAllowedErr);
}

// ------------------------------------------------------------------------------------------
const char*
AEDirectObjectNotAllowedException::what() const throw()
{
	return ("B::AEDirectObjectNotAllowedException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAEDirectObjectNotAllowedException = ExceptionStreamer::Register<AEDirectObjectNotAllowedException>();
#endif


// ==========================================================================================
//	AECantRelateObjectsException

#pragma mark -

// ------------------------------------------------------------------------------------------
AECantRelateObjectsException::~AECantRelateObjectsException() throw()
{
}

// ------------------------------------------------------------------------------------------
OSStatus
AECantRelateObjectsException::GetStatus() const throw()
{
	return (kMOSLCantRelateObjectsErr);
}

// ------------------------------------------------------------------------------------------
const char*
AECantRelateObjectsException::what() const throw()
{
	return ("B::AECantRelateObjectsException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAECantRelateObjectsException = ExceptionStreamer::Register<AECantRelateObjectsException>();
#endif


// ==========================================================================================
//	AEBoundaryMustBeObjectException

#pragma mark -

// ------------------------------------------------------------------------------------------
AEBoundaryMustBeObjectException::~AEBoundaryMustBeObjectException() throw()
{
}

// ------------------------------------------------------------------------------------------
OSStatus
AEBoundaryMustBeObjectException::GetStatus() const throw()
{
	return (kMOSLBoundaryMustBeObjectErr);
}

// ------------------------------------------------------------------------------------------
const char*
AEBoundaryMustBeObjectException::what() const throw()
{
	return ("B::AEBoundaryMustBeObjectException");
}

#ifndef DOXYGEN_SKIP
bool gRegisteredAEBoundaryMustBeObjectException = ExceptionStreamer::Register<AEBoundaryMustBeObjectException>();
#endif


// ==========================================================================================
//	ErrnoException

#pragma mark -

// ------------------------------------------------------------------------------------------
ErrnoException::ErrnoException() throw()
	: mErrno(errno)
{
}

// ------------------------------------------------------------------------------------------
ErrnoException::ErrnoException(
	int	inErrno)	//!< The error code.
	throw()
		: mErrno(inErrno)
{
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to reading in their own state.
*/
ErrnoException::ErrnoException(
	std::istream&	istr)	//!< The input stream.
{
	istr >> mErrno;
}

// ------------------------------------------------------------------------------------------
ErrnoException::~ErrnoException() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
ErrnoException::what() const throw()
{
	return ("B::ErrnoException");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
ErrnoException::Write(std::ostream& ostr) const
{
	ostr << mErrno << "\n";
}

#ifndef DOXYGEN_SKIP
bool gRegisteredErrnoException = ExceptionStreamer::Register<ErrnoException>();
#endif


// ==========================================================================================

#define TEST 0	// Define non-zero to compile test functions below.

#if TEST

#include <iostream>

template <class EX> static void	test_ex(const EX& ex0)
{
	EX	ex1(ex0);
//	EX	ex2(std::cin);
	
	ex1 = ex0;
	
	std::cout << ex0;
}

static void	test()
{
	FSRef	srcRef, dstRef;
	
	test_ex(Exception());
	test_ex(RuntimeOSStatusException(paramErr));
	test_ex(PropertyListCreateException(String("error")));
	test_ex(UrlResolutionException());
	test_ex(UnknownElementException());
	test_ex(UnsupportedUrlSchemeException());
	test_ex(FileOpenException());
	test_ex(ReadException());
	test_ex(WriteException());
	test_ex(FSExchangeObjectsException(paramErr, srcRef, dstRef));
}

#endif

}	// namespace B
