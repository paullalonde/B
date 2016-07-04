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

#ifndef BErrorHandler_H_
#define BErrorHandler_H_

#pragma once

// standard headers
#include <exception>
#include <iosfwd>
#include <typeinfo>
#if !__MWERKS__
#	include <string>
#endif

// compiler headers
#if __MWERKS__
#	include <stringfwd>
#endif

// library headers
#include <boost/intrusive_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/tss.hpp>
#include <boost/utility.hpp>

// system headers
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CFBase.h>


/*! @name Detailed Exception & Assert Information
	
	The macros in this group determine if detailed information (i.e., file name, line 
	number, function name) about the throw/assert site is gathered.
	
	@ingroup	ExceptionGroup
*/
//@{

#ifndef B_DETAILED_EXCEPTION_INFO
#	ifndef NDEBUG
#		define B_DETAILED_EXCEPTION_INFO	1
#	else
#		define B_DETAILED_EXCEPTION_INFO	0
#	endif
#elif DOXYGEN_SCAN
	/*!	@def	B_DETAILED_EXCEPTION_INFO
		@brief	Determines whether file/line/function info is generated when an exception is 
				thrown.
		
		Normally, the detailed info is only generated in debug builds.  However, by defining 
		B_DETAILED_EXCEPTION_INFO to a non-zero value, it can be generated in release builds 
		also.  This is convenient if your error handler needs to perform some kind of logging.
		
		@relates	ErrorHandler
	*/
#	define B_DETAILED_EXCEPTION_INFO	1
#endif

#ifndef B_DETAILED_ASSERT_INFO
#	ifndef NDEBUG
#		define B_DETAILED_ASSERT_INFO	1
#	else
#		define B_DETAILED_ASSERT_INFO	0
#	endif
#elif DOXYGEN_SCAN
	/*!	@def	B_DETAILED_ASSERT_INFO
		@brief	Determines whether file/line/function info is generated when an assertion 
				fails.
		
		Normally, the detailed info is only generated in debug builds.  However, by defining 
		B_DETAILED_ASSERT_INFO to a non-zero value, it can be generated in release builds 
		also.  This is convenient if your error handler needs to perform some kind of logging.
		
		@relates	ErrorHandler
	*/
#	define B_DETAILED_ASSERT_INFO	1
#endif

//@}


namespace B {

// forward declarations
class	ErrorHandlerGlobals;
class	String;


/*!	@brief	Centralised error handling.
	
	ErrorHandler is a per-thread singleton object to which all throws and asserts 
	(at least those that go through the macros B_THROW(), B_ASSERT(), etc.) are funnelled.
	To retrieve the current thread's error handler, call Get().  To change it, call Set().
	Note that Set() allows the developer to write a custom error-handling object.  This 
	is useful for doing things such as logging exceptions to a file or system log.
	
	Get() actually returns a smart pointer.  The singleton's lifetime is controlled by 
	the number of clients of the smart pointer.  If you have code that depends upon the 
	availability of the error handler, event during thread termination or application 
	shutdown, it should hold on to this smart pointer.
	
	When an exception is thrown or an assertion fails, ErrorHandler will display an alert 
	by default.  This can be changed by calling SetQuietThrows() and SetQuietAsserts().
	
	@sa			@ref using_exceptions
	@ingroup	ExceptionGroup
*/
class ErrorHandler : public boost::noncopyable
{
public:
	
	/*! @name Singleton Access
		
		This is a per-thread singleton.
	*/
	//@{
	//! Returns the ErrorHandler instance for the current thread.
	static boost::intrusive_ptr<ErrorHandler>
				Get();
	//! Changes the ErrorHandler instance for the current thread.
	static void	Set(ErrorHandler* inHandler);
	//@}
	
	/*! @name Throwing Exceptions
		
		Clients should normally use one of the exception-throwing macros instead 
		of calling these functions directly.
	*/
	//@{
	//! Calls the throw hook (passing it @a file, @a line and @a func), then throws @a ex.
	template <class EXCEPTION>
	static void	ThrowException(const EXCEPTION& ex, const char* file, int line, const char* func);
	//! Calls the throw hook, then throws @a ex.
	template <class EXCEPTION>
	static void	ThrowException(const EXCEPTION& ex);
	//! Calls the throw hook (passing it @a file, @a line and @a func), then throws @c std::bad_alloc.
	static void	ThrowBadAllocException(const char* file, int line, const char* func);
	//! Calls the throw hook, then throws @c std::bad_alloc.
	static void	ThrowBadAllocException();
	//! Calls the throw hook (passing it @a file, @a line and @a func), then throws @c RuntimeOSStatusException.
	static void	ThrowOSStatusException(OSStatus err, const char* file, int line, const char* func);
	//! Calls the throw hook, then throws @c RuntimeOSStatusException.
	static void	ThrowOSStatusException(OSStatus err);
	//! Calls the throw hook (passing it @a file, @a line and @a func), then throws @c ErrnoException.
	static void	ThrowErrnoException(int errn, const char* file, int line, const char* func);
	/*! @overload
	*/
	static void	ThrowErrnoException(const char* file, int line, const char* func);
	//! Calls the throw hook, then @c ErrnoException.
	static void	ThrowErrnoException(int errn);
	/*! @overload
	*/
	static void	ThrowErrnoException();
	//! Throws an exception object matching @a inStatus and @a inMessage.
	virtual void	ThrowStatusException(
						OSStatus		inStatus, 
						const String&	inMessage);
	//@}
	
	//! @name Asserts
	//@{
	//! Calls the assert hook, passing it @a cond, @a file, @a line and @a func.
	static void	HandleAssert(const char* cond, const char* file, int line, const char* func);
	//@}
	
	//! @name Querying Exception Objects
	//@{
	//! Returns an OS error code describing @a ex.
	static OSStatus	GetStatus(const std::exception& ex, OSStatus default_status) throw();
	//! Returns a string describing @a ex.
	CFStringRef		CopyExceptionMessage(const std::exception& ex) const throw();
	//@}
	
	//! @name Message Tables
	//@{
	//! Adds @a inTable to the collection of tables used to generate error messages.
	void	AddMessageTable(CFStringRef inTable);
	//! Removes @a inTable from the collection of tables used to generate error messages.
	void	RemoveMessageTable(CFStringRef inTable);
	//@}
	
	/*! @name Quiet Exceptions & Asserts
		
		The "quiet flags" are a hint to AboutToThrow() & AssertionFailed() that they 
		should not display a user interface.
	*/
	//@{
	//! Returns the quiet flag for throws.
	bool	GetQuietThrows() const		{ return (mQuietThrow); }
	//! Changes the quiet flag for throws.
	void	SetQuietThrows(bool quiet)	{ mQuietThrow = quiet; }
	//! Returns the quiet flag for asserts.
	bool	GetQuietAsserts() const		{ return (mQuietAssert); }
	//! Changes the quiet flag for asserts.
	void	SetQuietAsserts(bool quiet)	{ mQuietAssert = quiet; }
	//@}
	
protected:
	
	//! @name Constructor / Destructor
	//@{
	//! Constructor.
			ErrorHandler();
	//! Destructor.
	virtual	~ErrorHandler() throw();
	//@}
	
	//! @name Hooks
	//@{
	//! This is called before throwing exception @a ex in debug builds.
	virtual void	AboutToThrow(
						const std::exception&	ex, 
						const char*				file, 
						int						line, 
						const char*				func);
	//! This is called before throwing exception @a ex in release builds.
	virtual void	AboutToThrow(
						const std::exception&	ex);
	//! This is called when an assertion fails in debug builds.
	virtual void	AssertionFailed(
						const char*				cond, 
						const char*				file, 
						int						line, 
						const char*				func);
	//@}
	
private:
	
	static boost::intrusive_ptr<ErrorHandlerGlobals>&
				GetGlobalsPtr();
	static void	InitGlobalsPtr() throw();
	static void	CleanupGlobalsPtr() throw();
	
	// member variables
	SInt32			mRefCount;
	bool			mQuietThrow;
	bool			mQuietAssert;
	boost::intrusive_ptr<ErrorHandlerGlobals>
                    mGlobals;
	
	// static member variables
	static boost::once_flag	sGlobalsInit;
	static bool				sGlobalsInitFailure;
	static boost::intrusive_ptr<ErrorHandlerGlobals>*
                            sGlobalsPtr;
	
	// friends
	friend class	ErrorHandlerGlobals;
    friend void     intrusive_ptr_add_ref(ErrorHandler* eh);
    friend void     intrusive_ptr_release(ErrorHandler* eh);
};

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ErrorHandler::ThrowException(const EXCEPTION& ex, const char* file, int line, const char* func)
{
	Get()->AboutToThrow(ex, file, line, func);
	
	throw ex;
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ErrorHandler::ThrowException(const EXCEPTION& ex)
{
	Get()->AboutToThrow(ex);
	
	throw ex;
}


// ==========================================================================================
//	Exception-throwing macros

#if B_DETAILED_EXCEPTION_INFO

	/*! @name Exception-Throwing Macros
		
		@ingroup	ExceptionGroup
	*/
	//@{
	
	/*!	@brief	Throws the exception object @a ex.
		
		@param		ex	The exception object.  Must derive from @c std::exception.
		@relates	ErrorHandler
	*/
#	define B_THROW(ex)	\
		B::ErrorHandler::ThrowException(ex, __FILE__, __LINE__, __func__)

	/*!	@brief	Throws @c std::bad_alloc if the pointer @a p is @c NULL.
		
		@param		p	Any pointer.
		@relates	ErrorHandler
	*/
#	define B_THROW_IF_NULL(p)	\
		do { if ((p) == 0) B::ErrorHandler::ThrowBadAllocException(__FILE__, __LINE__, __func__); } while (0)

	/*!	@brief	Throws an exception determined by the Resource Manager if the handle @a h 
				is @c NULL.
		
		@param		h	Any MacOS-style handle.
		@relates	ErrorHandler
	*/
#	define B_THROW_IF_NULL_RSRC(h)	\
		do { if ((h) == 0) { OSStatus __status = ResError(); if (__status == noErr) __status = resNotFound; B::ErrorHandler::ThrowOSStatusException(__status, __FILE__, __LINE__, __func__); } } while (0)

	/*!	@brief	Throws the exception object @a ex if condition @a c is non-zero.
		
		@param		c	The condition.  Any integer-valued expression.
		@param		ex	The exception object.  Must derive from @c std::exception.
		@relates	ErrorHandler
	*/
#	define B_THROW_IF(c, ex)	\
		do { if (c) B::ErrorHandler::ThrowException(ex, __FILE__, __LINE__, __func__); } while (0)

	/*!	@brief	Throws RuntimeOSStatusException(@a stat).
		
		@param		stat	An @c OSStatus value.
		@relates	ErrorHandler
	*/
#	define B_THROW_STATUS(stat)	\
		B::ErrorHandler::ThrowOSStatusException(stat, __FILE__, __LINE__, __func__)

	/*!	@brief	Throws RuntimeOSStatusException(@a stat) if @a stat is non-zero.
		
		This is @b very convenient for testing the return value of "Classic" Mac OS functions.
		
		@param		stat	An @c OSStatus value.
		@relates	ErrorHandler
	*/
#	define B_THROW_IF_STATUS(stat)	\
		do { OSStatus __status = stat; if ((__status) != noErr) B::ErrorHandler::ThrowOSStatusException(__status, __FILE__, __LINE__, __func__); } while (0)

	/*!	@brief	Throws RuntimeOSStatusException(@a stat) if condition @a c is non-zero.
		
		@param		c		The condition.  Any integer-valued expression.
		@param		stat	An @c OSStatus value.
		@relates	ErrorHandler
	*/
#	define B_THROW_STATUS_IF(c, stat)	\
		do { if (c) B::ErrorHandler::ThrowOSStatusException(stat, __FILE__, __LINE__, __func__); } while (0)

	/*! @brief	Throws an ErrnoException with the current value of @c errno if condition 
				@a c is non-zero.
		
		This is convenient for testing the return value of BSD functions that set @c errno 
		internally but only return a success-failure indication.
		
		@param		c	The condition.  Any integer-valued expression.
		@relates	ErrorHandler
	*/
#	define B_THROW_ERRNO_IF(c)	\
		do { if (c) B::ErrorHandler::ThrowErrnoException(__FILE__, __LINE__, __func__); } while (0)

	/*!	@brief	Throws an ErrnoException(@a errn) if @a errn is non-zero.
		
		This is convenient for testing the return value of BSD functions that directly return 
		an errno-style value, such as pthreads functions.
		
		@param		errn	An @c errno value.
		@relates	ErrorHandler
	*/
#	define B_THROW_IF_ERRNO(errn)	\
		do { int __errn = errn; if (__errn) B::ErrorHandler::ThrowOSStatusException(__errn, __FILE__, __LINE__, __func__); } while (0)
	
	//@}
	
#else

#	define B_THROW(ex)	\
		B::ErrorHandler::ThrowException(ex)

#	define B_THROW_IF_NULL(p)	\
		do { if ((p) == 0) B::ErrorHandler::ThrowBadAllocException(); } while (0)

#	define B_THROW_IF_NULL_RSRC(h)	\
		do { if ((h) == 0) { OSStatus __status = ResError(); if (__status == noErr) __status = resNotFound; B::ErrorHandler::ThrowOSStatusException(__status); } } while (0)

#	define B_THROW_IF(c, ex)	\
		do { if (c) B::ErrorHandler::ThrowException(ex); } while (0)

#	define B_THROW_STATUS(stat)	\
		B::ErrorHandler::ThrowOSStatusException(stat)

#	define B_THROW_IF_STATUS(stat)	\
		do { OSStatus __status = stat; if ((__status) != noErr) B::ErrorHandler::ThrowOSStatusException(__status); } while (0)

#	define B_THROW_STATUS_IF(c, stat)	\
		do { if (c) B::ErrorHandler::ThrowOSStatusException(stat); } while (0)

#	define B_THROW_ERRNO_IF(c)	\
		do { if (c) B::ErrorHandler::ThrowErrnoException(); } while (0)

#	define B_THROW_IF_ERRNO(errn)	\
		do { int __errn = errn; if (__errn) B::ErrorHandler::ThrowOSStatusException(__errn); } while (0)

#endif


#if B_DETAILED_ASSERT_INFO

	/*! @name Assertion Macros
		
		@ingroup	ExceptionGroup
	*/
	//@{
	
	/*!	@brief	Invokes ErrorHandler if condition @a c is zero.
		
		@param		c	The condition.  Any integer-valued expression.
		@relates	ErrorHandler
	*/
#	define B_ASSERT(c)	\
		do { if (!(c)) 	B::ErrorHandler::HandleAssert(#c, __FILE__, __LINE__, __func__); } while (0)

	/*!	@brief	Invokes ErrorHandler unconditionally.
		
		@param		s	A string.
		@relates	ErrorHandler
	*/
#	define B_SIGNAL(s)	\
		B::ErrorHandler::HandleAssert(s, __FILE__, __LINE__, __func__)
	
	//@}
	
#else

#	define B_ASSERT(c)
#	define B_SIGNAL(s)

#endif


// ==========================================================================================
//	AutoThrowHandler

#pragma mark -
#pragma mark AutoThrowHandler

/*!	@brief	Convenience class for temporarily altering the quiet-ness of throws.
	
	@todo	%Document this class!
*/
class AutoThrowHandler : public boost::noncopyable
{
public:
	
	AutoThrowHandler(bool quiet = true);
	~AutoThrowHandler();
	
private:
	
	bool	mSavedQuiet;
};


}	// namespace B


#endif	// BErrorHandler_H_
