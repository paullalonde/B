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

#ifndef BException_H_
#define BException_H_

#pragma once

// standard headers
#include <iosfwd>
#include <string>

// system headers
#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CFBase.h>


namespace B {

// ==========================================================================================
//	ExceptionTraits

#pragma mark ExceptionTraits

/*!	@brief	Tag describing exception classes that don't contain any state.
	
	Using this tag implies that:
	
		- The exception is constructed with a default constructor.
		- The return value from what() is always the same.
	
	@ingroup	ExceptionGroup
*/
struct DefaultExceptionTag {};

/*!	@brief	Tag describing exception classes whose state is a single string.
	
	Using this tag implies that:
		
		- The exception is constructed with a constructor taking an @c std::string.
		- The return value from what() completely describes the exception's state.
	
	@ingroup	ExceptionGroup
*/
struct StringExceptionTag {};

/*!	@brief	Tag describing exception classes that contain complex state.
	
	Using this tag implies that:
		
		- The exception is constructed with a constructor taking an @c std::istream.
		- The exception contains a member function with the following signature:
		  @code void Write(std::ostream& ostr) const @endcode.  This function serialises 
		  the exception's state to @a ostr.
	
	@ingroup	ExceptionGroup
*/
struct StreamExceptionTag {};

/*!	@brief	Describes the characteristics of an exception class.
	
	Currently, the only characteristic is the exception's ::Category.  The ::Category is 
	used by code such as ExceptionStreamer that needs to know how to extract information 
	from an exception object and re-construct it later.
	
	An exception class' category may be one of DefaultExceptionTag, StringExceptionTag, 
	or StreamExceptionTag.  The default is DefaultExceptionTag, meaning that without 
	programmer intervention ExceptionStreamer will attempt to build exception objects 
	using their default constructor, and that it won't attempt to extract state from 
	them.
	
	Exception classes that need to store state will need to specialise ExceptionTraits 
	so that the Category is either StringExceptionTag or StreamExceptionTag.
	
	@ingroup	ExceptionGroup
*/
template <class EXCEPTION> struct ExceptionTraits
{
	//! Describes how to retrieve and construct the exception's state.
	typedef DefaultExceptionTag	Category;
};


// ==========================================================================================
//	ExceptionStringHolder

#pragma mark ExceptionStringHolder

/*!
	@brief	Exception helper class
	
	This class can be embedded within an exceptions object as a member variable.  It 
	holds a @c CFStringRef, which is converted on-demand into a <tt>const char*</tt> 
	via GetString().
	
	The class' member functions are set up so that they cannot throw at an inopportune 
	time (i.e. during construction).  Its throw-specs are consistent with those of 
	std:exception and its derived classes.
	
	@ingroup	ExceptionGroup
*/
class ExceptionStringHolder
{
public:
	
	//! Default constructor.
				ExceptionStringHolder() throw();
	//! Copy constructor.
				ExceptionStringHolder(const ExceptionStringHolder& inHolder) throw();
	//! @c CFStringRef constructor.
	explicit	ExceptionStringHolder(CFStringRef inString) throw();
	//! @c CFStringRef + fallback constructor.
	explicit	ExceptionStringHolder(CFStringRef inString, CFStringRef inFallback) throw();
	//! @c std::string constructor.
	explicit	ExceptionStringHolder(const std::string& str);
	//! Stream constructor.
	explicit	ExceptionStringHolder(std::istream& istr);
	//! Destructor.
				~ExceptionStringHolder() throw();
	
	//! Copy assignment.
	ExceptionStringHolder&	operator = (const ExceptionStringHolder& ex) throw();
	
	//! Returns the held string.
	CFStringRef	cf_ref() const	{ return (mString); }
	//! Returns the held string.
	const char*	GetString(const char* inFallback) const throw();
	
	//! Reads in the holder's internal state from @a istr.
	void		Read(std::istream& istr);
	//! Writes out the holder's internal state to @a ostr.
	void		Write(std::ostream& ostr) const;
	
protected:
	
	// member variables
	CFStringRef		mString;
	mutable char*	mCachedBuff;
};


// ==========================================================================================
//	IOException

#pragma mark IOException

/*!
	@brief	I/O exception base class
	
	This class is the base class of all I/O-related exceptions.
	
	In all instances of this class, what() returns "B::IOException".
	
	@ingroup	ExceptionGroup
*/
class IOException : public std::exception
{
public:
	
	//! Destructor.
	virtual	~IOException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	FileNotFoundException

#pragma mark FileNotFoundException

/*!
	@brief	File-not-found exception class
	
	Objects of this class are thrown when a file can't be located.
	
	In all instances of this class, what() returns "B::FileNotFoundException".
	
	@ingroup	ExceptionGroup
*/
class FileNotFoundException : public IOException
{
public:

	//! Destructor.
	virtual	~FileNotFoundException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	OpenException

#pragma mark OpenException

/*!
	@brief	File/stream open exception class
	
	Objects of this class are thrown when a file or stream can't be opened.
	
	In all instances of this class, what() returns "B::OpenException".
	
	@ingroup	ExceptionGroup
*/
class OpenException : public IOException
{
public:

	//! Destructor.
	virtual	~OpenException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	EOFException

#pragma mark EOFException

/*!
	@brief	File/stream end-of-stream exception class
	
	Objects of this class are thrown when the end of a file or stream is reached 
	unexpectedly.
	
	In all instances of this class, what() returns "B::EOFException".
	
	@ingroup	ExceptionGroup
*/
class EOFException : public IOException
{
public:

	//! Destructor.
	virtual	~EOFException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	ReadException

#pragma mark ReadException

/*!
	@brief	File/stream read exception class
	
	Objects of this class are thrown when a non-EOF error occurs while reading from a 
	file or stream.
	
	In all instances of this class, what() returns "B::ReadException".
	
	@ingroup	ExceptionGroup
*/
class ReadException : public IOException
{
public:
	
	//! Destructor.
	virtual	~ReadException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	WriteException

#pragma mark WriteException

/*!
	@brief	File/stream write exception class
	
	Objects of this class are thrown when an error occurs while writing to a 
	file or stream.
	
	In all instances of this class, what() returns "B::WriteException".
	
	@ingroup	ExceptionGroup
*/
class WriteException : public IOException
{
public:
	
	//! Destructor.
	virtual	~WriteException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	OSStatusException

#pragma mark OSStatusException

/*!
	@brief	Abtract base class for exceptions containing a MacOS error code
	
	This class serves as a place to hang a pure virtual GetStatus() member function, 
	which the error-handling infrastructure can then query.
	
	@ingroup	ExceptionGroup
*/
class OSStatusException : public std::exception
{
public:
	
	//! Destructor.
	virtual		~OSStatusException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw() = 0;
};


// ==========================================================================================
//	RuntimeOSStatusException

#pragma mark RuntimeOSStatusException

/*!
	@brief	Low-level MacOS-error-code exception class
	
	This class contains an OS-defined error code.  Typically an exception of 
	this class is thrown after an OS-defined function returns a non-zero 
	result code.
	
	In all instances of this class, what() returns "B::RuntimeOSStatusException".
	
	@ingroup	ExceptionGroup
*/
class RuntimeOSStatusException : public OSStatusException
{
public:
	
	//! Copy constructor.
				RuntimeOSStatusException(const RuntimeOSStatusException& ex) throw();
	//! @c OSStatus constructor.
	explicit	RuntimeOSStatusException(OSStatus inStatus) throw();
	//! Stream constructor.
	explicit	RuntimeOSStatusException(std::istream& istr);
	//! Destructor.
	virtual		~RuntimeOSStatusException() throw();
	
	//! Copy assignment.
	RuntimeOSStatusException&	operator = (const RuntimeOSStatusException& ex) throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw()			{ return (mStatus); }
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
	
	//! Writes out the exception's internal state to @a ostr.
	void                Write(std::ostream& ostr) const;
	
private:
	
	// member variables
	OSStatus	mStatus;
};

template <> struct ExceptionTraits<RuntimeOSStatusException>	{ typedef StreamExceptionTag	Category; };


// ==========================================================================================
//	ConstantOSStatusException

#pragma mark ConstantOSStatusException

/*!
	@brief	Hard-wired MacOS-error-code exception class
	
	This class contains a constant OS-defined error code.  It is typically used to 
	generate an error which is guaranteed to be mapped to a given @c OSStatus code.
	
	In all instances of this class, what() returns an template-instantiation-specific 
	string (courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
template <OSStatus STATUS> class ConstantOSStatusException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~ConstantOSStatusException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};

// ------------------------------------------------------------------------------------------
template <OSStatus STATUS>
ConstantOSStatusException<STATUS>::~ConstantOSStatusException() throw()
{
}

// ------------------------------------------------------------------------------------------
template <OSStatus STATUS> OSStatus
ConstantOSStatusException<STATUS>::GetStatus() const throw()
{
	return (STATUS);
}

// ------------------------------------------------------------------------------------------
template <OSStatus STATUS> const char*
ConstantOSStatusException<STATUS>::what() const throw()
{
	return (typeid(*this).name());
}


// ==========================================================================================
//	RethrownException

#pragma mark RethrownException

/*!
	@brief	Internal exception class.
	
	This class is used internally by the framework to re-throw exceptions that have 
	been propagated through the OS (eg Apple Event manager).
	
	what() returns the error message given in the constructor, encoded as UTF-8.
	
	@ingroup	ExceptionGroup
*/
class RethrownException : public OSStatusException
{
public:
	
	//! Copy constructor.
				RethrownException(const RethrownException& ex) throw();
	//! @c OSStatus constructor.
	explicit	RethrownException(OSStatus inStatus, CFStringRef inErrorMsg) throw();
	//! Stream constructor.
	explicit	RethrownException(std::istream& istr);
	//! Destructor.
	virtual		~RethrownException() throw();
	
	//! Copy assignment.
	RethrownException&	operator = (const RethrownException& ex) throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
	
	//! Writes out the exception's internal state to @a ostr.
	void                Write(std::ostream& ostr) const;
	
private:
	
	// member variables
	OSStatus				mStatus;
	ExceptionStringHolder	mHolder;
	
	// static member variables
	static const CFStringRef	sDefaultMsg;
};

template <> struct ExceptionTraits<RethrownException>
{
	typedef StreamExceptionTag	Category;
};


// ==========================================================================================
//	PropertyListCreateException

#pragma mark PropertyListCreateException

/*!
	@brief	Property list creation exception class
	
	Objects of this class are thrown when a property list creation function such as 
	@c CFPropertyListCreateFromXMLData() fails.  The exception holds the error message 
	(a @c CFStringRef) returned by the failing function.
	
	In all instances of this class, what() returns "B::PropertyListCreateException".
	
	@ingroup	ExceptionGroup
*/
class PropertyListCreateException : public std::exception
{
public:
	
	//! Copy constructor.
				PropertyListCreateException(const PropertyListCreateException& ex) throw();
	//! @c CFStringRef constructor.
	explicit	PropertyListCreateException(CFStringRef inErrorMsg) throw();
	//! Stream constructor.
	explicit	PropertyListCreateException(std::istream& istr);
	//! Destructor.
	virtual		~PropertyListCreateException() throw();
	
	//! Copy assignment.
	PropertyListCreateException&	operator = (const PropertyListCreateException& ex) throw();
	
	//! Returns the error message returned from the XML creation function.
	CFStringRef			GetErrorMessage() const		{ return (mHolder.cf_ref()); }
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
	
	//! Writes out the exception's internal state to @a ostr.
	void                Write(std::ostream& ostr) const;
	
private:
	
	// member variables
	ExceptionStringHolder	mHolder;
};

template <> struct ExceptionTraits<PropertyListCreateException>
{
	typedef StreamExceptionTag	Category;
};


// ==========================================================================================
//	MalformedUrlException

#pragma mark MalformedUrlException

/*!
	@brief	Malformed URL exception class
	
	Objects of this class are thrown when an attempt is made to create an Url object 
	with a string that doesn't follow the syntax of URLs.
	
	In all instances of this class, what() returns "B::MalformedUrlException".
	
	@ingroup	ExceptionGroup
*/
class MalformedUrlException : public std::exception
{
public:
	
	//! Destructor.
	virtual	~MalformedUrlException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	UnsupportedUrlSchemeException

#pragma mark UnsupportedUrlSchemeException

/*!
	@brief	Unsupported URL scheme exception class
	
	Objects of this class are thrown when a URL is encountered that has a scheme that the 
	application can't handle.  For example, the document-handling code throws this if 
	it's given a URL that doesn't use the "file:" scheme.
	
	In all instances of this class, what() returns "B::UnsupportedUrlSchemeException".
	
	@ingroup	ExceptionGroup
*/
class UnsupportedUrlSchemeException : public std::exception
{
public:
	
	//! Destructor.
	virtual	~UnsupportedUrlSchemeException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	CharacterEncodingException

#pragma mark CharacterEncodingException

/*!
	@brief	Character encoding exception class
	
	Objects of this class are thrown when an error occurs while attempting to convert a 
	string from one character encoding to another.
	
	In all instances of this class, what() returns "B::CharacterEncodingException".
	
	@ingroup	ExceptionGroup
*/
class CharacterEncodingException : public std::exception
{
public:
	
	//! Destructor.
	virtual	~CharacterEncodingException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	InteractionTimeoutException

#pragma mark InteractionTimeoutException

/*!
	@brief	User interaction timeout exception class
	
	Objects of this class are thrown when a request to get user interaction (i.e., via 
	the InteractWithUserDialog) fails.
	
	In all instances of this class, what() returns "B::InteractionTimeoutException".
	
	@ingroup	ExceptionGroup
*/
class InteractionTimeoutException : public std::exception
{
public:
	
	//! Destructor.
	virtual	~InteractionTimeoutException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	FSExchangeObjectsException

#pragma mark FSExchangeObjectsException

/*!
	@brief	FSExchangeObjects exception class
	
	Objects of this class are thrown when @c FSExchangeObjectsCompat() fails.  It contains 
	the source and destination @c FSRefs, which contain the identities of the two files 
	to be exchanged at the time of failure.
	
	In all instances of this class, what() returns "B::FSExchangeObjectsException".
	
	@ingroup	ExceptionGroup
*/
class FSExchangeObjectsException : public RuntimeOSStatusException
{
public:
	
	//! Constructor.
	explicit	FSExchangeObjectsException(
					OSStatus		inError, 
					const FSRef&	inSrcRef, 
					const FSRef&	inDstRef) throw();
	//! Stream constructor.
	explicit		FSExchangeObjectsException(std::istream& istr);
	//! Destructor.
	virtual		~FSExchangeObjectsException() throw();
	
	//! Returns the source file reference.
	const FSRef&		GetSourceRef() const		{ return (mSrcRef); }
	//! Returns the destination file reference.
	const FSRef&		GetDestinationRef() const	{ return (mDstRef); }
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
	
	//! Writes out the exception's internal state to @a ostr.
	void            Write(std::ostream& ostr) const;
	
private:
	
	FSRef	mSrcRef;
	FSRef	mDstRef;
};

template <> struct ExceptionTraits<FSExchangeObjectsException>
{
	typedef StreamExceptionTag	Category;
};


// ==========================================================================================
//	UserCanceledException

#pragma mark UserCanceledException

/*!
	@brief	"user canceled" exception class
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c userCanceledErr.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<userCanceledErr>	UserCanceledException;


#pragma mark -
#pragma mark -- AppleEvent Exceptions --


// ==========================================================================================
//	AENoSuchObjectException

#pragma mark AENoSuchObjectException

/*!
	@brief	"Can't find AE object" exception class
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAENoSuchObject.
	
	Exceptions of this class are meant to be thrown when attempting to resolve or locate 
	a non-existent object.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAENoSuchObject>	AENoSuchObjectException;


// ==========================================================================================
//	AECantHandleClassException

#pragma mark AECantHandleClassException

/*!
	@brief	"Apple Event not handled" exception class
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAECantHandleClass.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAECantHandleClass>	AECantHandleClassException;


// ==========================================================================================
//	AENotModifiableException

#pragma mark AENotModifiableException

/*!
	@brief	"Element/property not modifiable" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAENotModifiable.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAENotModifiable>	AENotModifiableException;


// ==========================================================================================
//	AEBadKeyFormException

#pragma mark AEBadKeyFormException

/*!
	@brief	"Bad key form" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAEBadKeyForm.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAEBadKeyForm>	AEBadKeyFormException;


// ==========================================================================================
//	AECantPutThatThereException

#pragma mark AECantPutThatThereException

/*!
	@brief	"Class can't be element of container" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAECantPutThatThere.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAECantPutThatThere>	AECantPutThatThereException;


// ==========================================================================================
//	AEWrongDataTypeException

#pragma mark AEWrongDataTypeException

/*!
	@brief	"Wrong data type" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAEWrongDataType.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAEWrongDataType>	AEWrongDataTypeException;


// ==========================================================================================
//	AECoercionFailException

#pragma mark AECoercionFailException

/*!
	@brief	"Coercion failure" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAECoercionFail.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAECoercionFail>	AECoercionFailException;


// ==========================================================================================
//	AEEventNotHandledException

#pragma mark AEEventNotHandledException

/*!
	@brief	"Apple Event not handled" exception class
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAEEventNotHandled.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAEEventNotHandled>	AEEventNotHandledException;


// ==========================================================================================
//	AEEventFailedException

#pragma mark AEEventFailedException

/*!
	@brief	General Apple %Event exception class
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAEEventFailed.
	
	Exceptions of this class are meant to be thrown when a more specific exception class 
	isn't available.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAEEventFailed>	AEEventFailedException;


// ==========================================================================================
//	AENoUserInteractionException

#pragma mark AENoUserInteractionException

/*!
	@brief	Interaction timeout Apple %Event exception class
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c errAENoUserInteraction.
	
	In all instances of this class, what() returns an implementation-defined string 
	(courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
typedef ConstantOSStatusException<errAENoUserInteraction>	AENoUserInteractionException;


// ==========================================================================================
//	AEClassHasNoElementsOfThisTypeException

#pragma mark AEClassHasNoElementsOfThisTypeException

/*!
	@brief	"Unknown element class" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c kMOSLClassHasNoElementsOfThisTypeErr.
	
	In all instances of this class, what() returns 
	"B::AEClassHasNoElementsOfThisTypeException".
	
	@internal	This class could have been an instantiation of ConstantOSStatusException 
				(like AEEventFailedException for example), but that would have required 
				#including "MoreOSL.h" in the header file and would therefore have 
				increased the coupling between B and MoreOSL.
	
	@ingroup	ExceptionGroup
*/
class AEClassHasNoElementsOfThisTypeException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~AEClassHasNoElementsOfThisTypeException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	AEUnrecognisedOperatorException

#pragma mark AEUnrecognisedOperatorException

/*!
	@brief	"Unrecognised comparison operator" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c kMOSLUnrecognisedOperatorErr.
	
	In all instances of this class, what() returns 
	"B::AEUnrecognisedOperatorException".
	
	@internal	This class could have been an instantiation of ConstantOSStatusException 
				(like AEEventFailedException for example), but that would have required 
				#including "MoreOSL.h" in the header file and would therefore have 
				increased the coupling between B and MoreOSL.
	
	@ingroup	ExceptionGroup
*/
class AEUnrecognisedOperatorException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~AEUnrecognisedOperatorException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	AEDirectObjectRequiredException

#pragma mark AEDirectObjectRequiredException

/*!
	@brief	"Direct object required" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c kMOSLDirectObjectRequiredErr.
	
	In all instances of this class, what() returns 
	"B::AEDirectObjectRequiredException".
	
	@internal	This class could have been an instantiation of ConstantOSStatusException 
				(like AEEventFailedException for example), but that would have required 
				#including "MoreOSL.h" in the header file and would therefore have 
				increased the coupling between B and MoreOSL.
	
	@ingroup	ExceptionGroup
*/
class AEDirectObjectRequiredException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~AEDirectObjectRequiredException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	AEDirectObjectNotAllowedException

#pragma mark AEDirectObjectNotAllowedException

/*!
	@brief	"Direct object not allowed" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c kMOSLDirectObjectNotAllowedErr.
	
	In all instances of this class, what() returns 
	"B::AEDirectObjectNotAllowedException".
	
	@internal	This class could have been an instantiation of ConstantOSStatusException 
				(like AEEventFailedException for example), but that would have required 
				#including "MoreOSL.h" in the header file and would therefore have 
				increased the coupling between B and MoreOSL.
	
	@ingroup	ExceptionGroup
*/
class AEDirectObjectNotAllowedException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~AEDirectObjectNotAllowedException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	AECantRelateObjectsException

#pragma mark AECantRelateObjectsException

/*!
	@brief	"Cant relate objects" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c kMOSLCantRelateObjectsErr.
	
	In all instances of this class, what() returns 
	"B::AECantRelateObjectsException".
	
	@internal	This class could have been an instantiation of ConstantOSStatusException 
				(like AEEventFailedException for example), but that would have required 
				#including "MoreOSL.h" in the header file and would therefore have 
				increased the coupling between B and MoreOSL.
	
	@ingroup	ExceptionGroup
*/
class AECantRelateObjectsException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~AECantRelateObjectsException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


// ==========================================================================================
//	AEBoundaryMustBeObjectException

#pragma mark AEBoundaryMustBeObjectException

/*!
	@brief	"Boundary must be object" exception
	
	This is basically an OSStatusException whose status code is hard-wired to 
	@c kMOSLBoundaryMustBeObjectErr.
	
	In all instances of this class, what() returns 
	"B::AEBoundaryMustBeObjectException".
	
	@internal	This class could have been an instantiation of ConstantOSStatusException 
				(like AEEventFailedException for example), but that would have required 
				#including "MoreOSL.h" in the header file and would therefore have 
				increased the coupling between B and MoreOSL.
	
	@ingroup	ExceptionGroup
*/
class AEBoundaryMustBeObjectException : public OSStatusException
{
public:
	
	//! Destructor.
	virtual	~AEBoundaryMustBeObjectException() throw();
	
	//! Returns the OS-defined error code.
	virtual OSStatus	GetStatus() const throw();
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};


#pragma mark -
#pragma mark -- BSD Exceptions --

// ==========================================================================================
//	ErrnoException

#pragma mark ErrnoException

/*!
	@brief	BSD errno exception class
	
	This class contains an OS-defined error code in the namespace used by the @c errno 
	global variable.  Possible values are @c ENOMEM, @c EACCESS, etc.   Typically an 
	exception of this class is thrown after a BSD-level function returns a non-zero 
	result code.
	
	In all instances of this class, what() returns "B::ErrnoException".
	
	@ingroup	ExceptionGroup
*/
class ErrnoException : public std::exception
{
public:
	
	//! Default constructor.  The error value is taken from the @c errno global variable.
				ErrnoException() throw();
	//! Errno value Constructor.
	explicit	ErrnoException(int inErrno) throw();
	//! Stream Constructor.
	explicit	ErrnoException(std::istream& istr);
	//! Destructor.
	virtual		~ErrnoException() throw();
	
	//! Returns the OS-defined error code.
	int					GetErrno() const			{ return (mErrno); }
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
	
	//! Writes out the exception's internal state to @a ostr.
	void                Write(std::ostream& ostr) const;
	
private:
	
	// member variables
	int	mErrno;
};

template <> struct ExceptionTraits<ErrnoException>
{
	typedef StreamExceptionTag	Category;
};


// ==========================================================================================
//	ConstantErrnoException

#pragma mark ConstantErrnoException

/*!
	@brief	Hard-wired @c errno exception class
	
	@todo	Complete this!
	
	In all instances of this class, what() returns an template-instantiation-specific 
	string (courtesy of @c typeinfo).
	
	@ingroup	ExceptionGroup
*/
template <int ERRNO> class ConstantErrnoException : public std::exception
{
public:
	
	//! Destructor.
	virtual	~ConstantErrnoException() throw();
	
	//! Returns the exception's textual description.
	virtual const char*	what() const throw();
};

// ------------------------------------------------------------------------------------------
template <int ERRNO>
ConstantErrnoException<ERRNO>::~ConstantErrnoException() throw()
{
}

// ------------------------------------------------------------------------------------------
template <int ERRNO> const char*
ConstantErrnoException<ERRNO>::what() const throw()
{
	return (typeid(*this).name());
}

template <int ERRNO> struct ExceptionTraits< ConstantErrnoException<ERRNO> >
{
	typedef DefaultExceptionTag	Category;
};


}	// namespace B

#endif	// BException_H_
