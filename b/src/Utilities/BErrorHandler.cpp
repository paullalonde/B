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
#include "BErrorHandler.h"

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BBundle.h"
#include "BException.h"
#include "BMutableArray.h"
#include "BStringFormatter.h"
#include "BStringUtilities.h"


namespace B {

/*! @brief  True global state for error handling.
    
    ErrorHandlerGlobals is a true application-wide singleton used internally by 
    ErrorHandler.  It contains the per-thread ErrorHandler singletons, among other things.
*/
// ==========================================================================================
//	ErrorHandlerGlobals

class ErrorHandlerGlobals : public boost::noncopyable
{
private:
	
    //! Constructor.
	ErrorHandlerGlobals();
	
    //! Returns the per-thread ErrorHandler.
	boost::intrusive_ptr<ErrorHandler>*
			GetPerThreadHandler() const;
    //! Sets the per-thread ErrorHandler.
	void	SetPerThreadHandler(boost::intrusive_ptr<ErrorHandler> inHandlerPtr);
	
    //! Adds a message table (i.e. a ".strings" file) to the global list.
	void	AddMessageTable(CFStringRef inTable);
    //! Removes a message table (i.e. a ".strings" file) from the global list.
	void	RemoveMessageTable(CFStringRef inTable);
	
    //! Looks up a localised message with the given @a key in all message tables.
	CFStringRef	CopyLocalisedMessageForKey(const char* key) const;
	
    //! Default "out of memory" message.
	CFStringRef	CopyOutOfMemoryMessage() const	{ CFRetain(mOutOfMemoryMessage.get()); return (mOutOfMemoryMessage); }
    //! Default error message.
	CFStringRef	CopyDefaultMessage() const		{ CFRetain(mDefaultMessage.get()); return (mDefaultMessage); }
	
	// member variables
	SInt32						mRefCount;
	boost::thread_specific_ptr< boost::intrusive_ptr<ErrorHandler> >
								mThreadPtr;
	MutableArray<CFStringRef>	mMessageTableArray;
	OSPtr<CFStringRef>			mOutOfMemoryMessage;
	OSPtr<CFStringRef>			mDefaultMessage;
	
	// friends
    friend class    ErrorHandler;
    friend void     intrusive_ptr_add_ref(ErrorHandlerGlobals* g);
    friend void     intrusive_ptr_release(ErrorHandlerGlobals* g);
};


// ------------------------------------------------------------------------------------------
ErrorHandlerGlobals::ErrorHandlerGlobals()
	: mRefCount(0), 
	  mOutOfMemoryMessage(CFCopyLocalizedStringFromTableInBundle(
			CFSTR("Out of memory"), CFSTR("B"), Bundle::Main().cf_ref(), 
			"An error message describing an out-of-memory situation."), from_copy),
	  mDefaultMessage(CFCopyLocalizedStringFromTableInBundle(
			CFSTR("Unknown exception"), CFSTR("B"), Bundle::Main().cf_ref(), 
			"An error message displayed when something went wrong but we don't know what."), from_copy)
{
	AddMessageTable(CFSTR("B"));
	AddMessageTable(CFSTR("Localizable"));
}

// ------------------------------------------------------------------------------------------
inline boost::intrusive_ptr<ErrorHandler>*
ErrorHandlerGlobals::GetPerThreadHandler() const
{
    return (mThreadPtr.get());
}

// ------------------------------------------------------------------------------------------
void
ErrorHandlerGlobals::SetPerThreadHandler(boost::intrusive_ptr<ErrorHandler> inHandlerPtr)
{
	boost::intrusive_ptr<ErrorHandler>*	handler_ptr	= NULL;
	
	try
	{
		handler_ptr	= new boost::intrusive_ptr<ErrorHandler>(inHandlerPtr);
		
		mThreadPtr.reset(handler_ptr);
	}
	catch (...)
	{
		delete handler_ptr;
		
		throw;
	}
}

// ------------------------------------------------------------------------------------------
void
ErrorHandlerGlobals::AddMessageTable(CFStringRef inTable)
{
	mMessageTableArray.push_back(inTable);
}

// ------------------------------------------------------------------------------------------
void
ErrorHandlerGlobals::RemoveMessageTable(CFStringRef inTable)
{
	mMessageTableArray.erase(
		std::remove(mMessageTableArray.begin(), mMessageTableArray.end(), inTable), 
		mMessageTableArray.end());
}

// ------------------------------------------------------------------------------------------
CFStringRef
ErrorHandlerGlobals::CopyLocalisedMessageForKey(const char* key) const
{
	OSPtr<CFStringRef>	keyStr(make_cfstring(key, kCFStringEncodingASCII));
	CFBundleRef			bundleRef	= Bundle::Main().cf_ref();
	
	for (MutableArray<CFStringRef>::const_iterator it = mMessageTableArray.begin(); 
		 it != mMessageTableArray.end(); 
		 ++it)
	{
		CFStringRef			table	= *it;
		OSPtr<CFStringRef>	value(CFBundleCopyLocalizedString(
									bundleRef, keyStr, keyStr, table), from_copy);
		
		if (CFStringCompare(value, keyStr, 0) != 0)
			return (value.release());
	}
	
	return (NULL);
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_add_ref(ErrorHandlerGlobals* g)
{
	B_ASSERT(g != NULL);
	
	IncrementAtomic(&g->mRefCount);
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_release(ErrorHandlerGlobals* g)
{
	B_ASSERT(g != NULL);
	B_ASSERT(g->mRefCount > 0);
	
	if (DecrementAtomic(&g->mRefCount) == 1)
	{
		// The ref count is now zero
		delete g;
	}
}


// ==========================================================================================
//	ErrorHandler

#pragma mark -

boost::once_flag	ErrorHandler::sGlobalsInit			= BOOST_ONCE_INIT;
bool				ErrorHandler::sGlobalsInitFailure	= false;
boost::intrusive_ptr<ErrorHandlerGlobals>*
                    ErrorHandler::sGlobalsPtr			= NULL;

// ------------------------------------------------------------------------------------------
void
ErrorHandler::InitGlobalsPtr() throw()
{
	B_ASSERT(sGlobalsPtr == NULL);
	
	ErrorHandlerGlobals*	globals_ptr	= NULL;
	
	sGlobalsInitFailure = false;
	
	try
	{
		globals_ptr	= new ErrorHandlerGlobals;
		sGlobalsPtr	= new boost::intrusive_ptr<ErrorHandlerGlobals>(globals_ptr);
		
		atexit(CleanupGlobalsPtr);
	}
	catch (...)
	{
		// Prevent exceptions from propagating.
		
		delete sGlobalsPtr;
		sGlobalsPtr = NULL;
		
		delete globals_ptr;
		
		sGlobalsInitFailure = true;
	}
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::CleanupGlobalsPtr() throw()
{
	delete sGlobalsPtr;
	sGlobalsPtr = NULL;
}

// ------------------------------------------------------------------------------------------
boost::intrusive_ptr<ErrorHandlerGlobals>&
ErrorHandler::GetGlobalsPtr()
{
	// call_once ensures that InitThreadPtr is only called once, even if multiple 
	// threads enter here concurrently.  Furthermore, any threads entering call_once 
	// will be blocked until the first one exits.
	
	boost::call_once(InitGlobalsPtr, sGlobalsInit);
    B_THROW_IF(sGlobalsInitFailure, std::runtime_error("can't init ErrorHandler"));
    
    // If this assertion fails, it's probably because we are being called during 
    // application shutdown, and CleanupGlobalsPtr() has already been called.
    B_ASSERT(sGlobalsPtr != NULL);
    
    return (*sGlobalsPtr);
}

// ------------------------------------------------------------------------------------------
/*!	If there isn't an error handler for the current thread, one will be created.
*/
boost::intrusive_ptr<ErrorHandler>
ErrorHandler::Get()
{
	boost::intrusive_ptr<ErrorHandlerGlobals>	globals	= GetGlobalsPtr();
	boost::intrusive_ptr<ErrorHandler>*			handler	= globals->GetPerThreadHandler();
	
	if (handler == NULL)
	{
		// There's no error handler for this thread -- allocate one.
		
		boost::intrusive_ptr<ErrorHandler>	handlerPtr(new ErrorHandler);
		
		globals->SetPerThreadHandler(handlerPtr);
		
		handler	= globals->GetPerThreadHandler();
		
		B_ASSERT(handler != NULL);
	}
	
	return (*handler);
}

// ------------------------------------------------------------------------------------------
/*!	@note	Ownership of @a inHandler is transferred to the ErrorHandler class.  If an 
			exception is thrown, @a inHandler is deleted.
*/
void
ErrorHandler::Set(ErrorHandler* inHandler)
{
	boost::intrusive_ptr<ErrorHandler>	handlerPtr(inHandler);
	
	GetGlobalsPtr()->SetPerThreadHandler(handlerPtr);
}

// ------------------------------------------------------------------------------------------
ErrorHandler::ErrorHandler()
	: mRefCount(0), mQuietThrow(false), mQuietAssert(false), mGlobals(GetGlobalsPtr())
{
}

// ------------------------------------------------------------------------------------------
ErrorHandler::~ErrorHandler() throw()
{
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::AddMessageTable(CFStringRef inTable)
{
	mGlobals->AddMessageTable(inTable);
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::RemoveMessageTable(CFStringRef inTable)
{
	mGlobals->RemoveMessageTable(inTable);
}

// ------------------------------------------------------------------------------------------
OSStatus
ErrorHandler::GetStatus(const std::exception& ex, OSStatus default_status) throw()
{
	OSStatus	status;
	
	try
	{
		if (dynamic_cast<const std::bad_alloc*>(&ex) != NULL)
		{
			// bad_alloc needs special handling because we want to minimize 
			// the memory allocations when we're in a low-mem situation (!)
			
			status = memFullErr;
		}
		else if (const OSStatusException* except = dynamic_cast<const OSStatusException*>(&ex))
		{
			// OSStatusException already contains an OSStatus, so retrieve it.
			
			status = except->GetStatus();
		}
		else if (dynamic_cast<const std::invalid_argument*>(&ex) != NULL)
		{
			status = paramErr;
		}
		else if (dynamic_cast<const std::ios_base::failure*>(&ex) != NULL)
		{
			status = ioErr;
		}
		else if (dynamic_cast<const std::length_error*>(&ex) != NULL)
		{
			status = errAEIndexTooLarge;
		}
		else if (dynamic_cast<const std::out_of_range*>(&ex) != NULL)
		{
			status = errAEIllegalIndex;
		}
		else
		{
			status = default_status;
		}
	}
	catch (const std::bad_alloc&)
	{
		status = memFullErr;
	}
	catch (...)
	{
		status = default_status;
	}
	
	return (status);
}

// ------------------------------------------------------------------------------------------
/*!	The returned string is meant to be used in a quasi-debugging context, such as an 
	Apple %Event reply.
*/
CFStringRef
ErrorHandler::CopyExceptionMessage(const std::exception& ex) const throw()
{
	OSPtr<CFStringRef>	msg;
	
	try
	{
		if (dynamic_cast<const std::bad_alloc*>(&ex) != NULL)
		{
			// bad_alloc needs special handling because we want to minimize 
			// the memory allocations when we're in a low-mem situation (!)
			
			msg.reset(mGlobals->CopyOutOfMemoryMessage(), from_copy);
		}
		else
		{
			msg.reset(mGlobals->CopyLocalisedMessageForKey(ex.what()), from_copy, std::nothrow);
			
			if (msg.get() == NULL)
			{
				msg.reset(mGlobals->CopyLocalisedMessageForKey(typeid(ex).name()), from_copy, std::nothrow);
				
				if (msg.get() == NULL)
				{
					msg.reset(mGlobals->CopyDefaultMessage(), from_copy);
				}
			}
		}
	}
	catch (const std::bad_alloc&)
	{
		msg.reset(mGlobals->CopyOutOfMemoryMessage(), from_copy);
	}
	catch (...)
	{
		msg.reset(mGlobals->CopyDefaultMessage(), from_copy);
	}
	
	return (msg.release());
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowStatusException(
	OSStatus		inStatus,	//!< The OS error code to translate into an exception object.
	const String&	inMessage)	//!< The string to put into the exception;  may be @c NULL.
{
	std::string	utf8;
	
	switch (inStatus)
	{
	case memFullErr:
		B_THROW(std::bad_alloc());
		break;
		
	case paramErr:
		if (!inMessage.empty())
		{
			B_THROW(std::invalid_argument(make_utf8_string(inMessage.cf_ref())));
		}
		break;
		
	case ioErr:
		if (!inMessage.empty())
		{
			B_THROW(std::ios_base::failure(make_utf8_string(inMessage.cf_ref())));
		}
		break;
		
	case errAEIndexTooLarge:
		if (!inMessage.empty())
		{
			B_THROW(std::length_error(make_utf8_string(inMessage.cf_ref())));
		}
		break;
		
	case errAEIllegalIndex:
		if (!inMessage.empty())
		{
			B_THROW(std::out_of_range(make_utf8_string(inMessage.cf_ref())));
		}
		break;
	}
	
	B_THROW(RethrownException(inStatus, inMessage.cf_ref()));
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowBadAllocException(const char* file, int line, const char* func)
{
	ThrowException(std::bad_alloc(), file, line, func);
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowBadAllocException()
{
	ThrowException(std::bad_alloc());
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowOSStatusException(OSStatus err, const char* file, int line, const char* func)
{
	ThrowException(B::RuntimeOSStatusException(err), file, line, func);
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowOSStatusException(OSStatus err)
{
	ThrowException(B::RuntimeOSStatusException(err));
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowErrnoException(int errn, const char* file, int line, const char* func)
{
	ThrowException(B::ErrnoException(errn), file, line, func);
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowErrnoException(int errn)
{
	ThrowException(B::ErrnoException(errn));
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowErrnoException(const char* file, int line, const char* func)
{
	ThrowException(B::ErrnoException(), file, line, func);
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::ThrowErrnoException()
{
	ThrowException(B::ErrnoException());
}

// ------------------------------------------------------------------------------------------
void
ErrorHandler::HandleAssert(const char* cond, const char* file, int line, const char* func)
{
	Get()->AssertionFailed(cond, file, line, func);
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function to provide custom functionality when an 
	exception is thrown.  It is up to the derived class to obey the quiet flag (as returned 
	by GetQuietThrows()) if it so chooses.
	
	The default implementation displays an alert.
*/
void
ErrorHandler::AboutToThrow(
	const std::exception&	ex, 
	const char*				file, 
	int						line, 
	const char*				func)
{
	if (GetQuietThrows())
		return;
	
	String		messageStr, errorStr, formatStr, explanationStr, okStr, cancelStr, otherStr;
	OSStatus	status, err;
	
	status		= GetStatus(ex, 0);
	messageStr.assign(CopyExceptionMessage(ex), from_copy);
	
	errorStr	= BLocalizedStringWithDefaultValue(
						CFSTR("Exception Thrown"), 
						CFSTR("B"), B::Bundle::Main(), 
						CFSTR("Exception Thrown"), 
						"The title of the exception-throwing debug alert");
	
	formatStr	= BLocalizedStringWithDefaultValue(
							CFSTR("Throw Message"), 
							CFSTR("B"), B::Bundle::Main(), 
							CFSTR("Message: %1%\nWhat: %2%\nError: %3%\n\nFunction: %4%\nFile: %5%\nLine # %6%"), 
							"The message displayed in the exception-throwing debug alert");
	
	okStr		= BLocalizedStringFromTableInBundle(
						CFSTR("Continue"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The label on the OK button in the assertion-failure & throw alerts");
	
	cancelStr	= BLocalizedStringFromTableInBundle(
						CFSTR("Abort"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The label on the Cancel button in the assertion-failure & throw alerts");
	
	otherStr	= BLocalizedStringFromTableInBundle(
						CFSTR("Quiet"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The label on the Quiet button in the assertion-failure & throw alerts");
	
	explanationStr = Extract(StringFormatter(formatStr) % messageStr % ex.what() % status % func % file % line);
	
	AlertStdCFStringAlertParamRec	params;
	
	err = GetStandardAlertDefaultParams(&params, kStdCFStringAlertVersionOne);
	
	if (err == noErr)
	{
		DialogRef	alert;
		
		params.movable		= true;
		params.defaultText	= okStr.cf_ref();
		params.cancelText	= cancelStr.cf_ref();
		params.otherText	= otherStr.cf_ref();
		
		err = CreateStandardAlert(kAlertNoteAlert, 
								  errorStr.cf_ref(), 
								  explanationStr.cf_ref(), 
								  &params, &alert);
		
		if (err == noErr)
		{
			DialogItemIndex	itemHit;
			
			err = RunStandardAlert(alert, NULL, &itemHit);
			
			if (err == noErr)
			{
				switch (itemHit)
				{
				case kAlertStdAlertOKButton:
					break;
					
				case kAlertStdAlertCancelButton:
					abort();
					break;
					
				case kAlertStdAlertOtherButton:
					SetQuietThrows(true);
					break;
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function to provide custom functionality when an 
	exception is thrown.  It is up to the derived class to obey the quiet flag (as returned 
	by GetQuietThrows()) if it so chooses.
	
	The default implementation displays an alert.
*/
void
ErrorHandler::AboutToThrow(
	const std::exception&	ex)
{
	AboutToThrow(ex, "n/a", 0, "n/a");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes may override this function to provide custom functionality when an 
	assertion fails.  It is up to the derived class to obey the quiet flag (as returned 
	by GetQuietAsserts()) if it so chooses.
	
	The default implementation displays an alert.
*/
void
ErrorHandler::AssertionFailed(
	const char*				cond, 
	const char*				file, 
	int						line, 
	const char*				func)
{
	if (GetQuietAsserts())
		return;
	
	String		errorStr, formatStr, explanationStr, okStr, cancelStr, otherStr;
	OSStatus	err;
	
	errorStr	= BLocalizedStringFromTableInBundle(
						CFSTR("Asssertion Failed"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The title of the assertion-failure alert");
	
	formatStr	= BLocalizedStringWithDefaultValue(
						CFSTR("Assert Message"), 
						CFSTR("B"), B::Bundle::Main(), 
						CFSTR("Condition: %1%\n\nFunction: %2%\nFile: %3%\nLine # %4%"), 
						"The message displayed in the assertion-failure alert");
	
	okStr		= BLocalizedStringFromTableInBundle(
						CFSTR("Continue"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The label on the OK button in the assertion-failure & throw alerts");
	
	cancelStr	= BLocalizedStringFromTableInBundle(
						CFSTR("Abort"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The label on the Cancel button in the assertion-failure & throw alerts");
	
	otherStr	= BLocalizedStringFromTableInBundle(
						CFSTR("Quiet"), 
						CFSTR("B"), B::Bundle::Main(), 
						"The label on the Quiet button in the assertion-failure & throw alerts");
	
	explanationStr = Extract(StringFormatter(formatStr) % cond % func % file % line);
		
	AlertStdCFStringAlertParamRec	params;
	
	err = GetStandardAlertDefaultParams(&params, kStdCFStringAlertVersionOne);
	
	if (err == noErr)
	{
		DialogRef	alert;
		
		params.movable		= true;
		params.defaultText	= okStr.cf_ref();
		params.cancelText	= cancelStr.cf_ref();
		params.otherText	= otherStr.cf_ref();
		
		err = CreateStandardAlert(kAlertNoteAlert, 
								  errorStr.cf_ref(), 
								  explanationStr.cf_ref(), 
								  &params, &alert);
		
		if (err == noErr)
		{
			DialogItemIndex	itemHit;
			
			err = RunStandardAlert(alert, NULL, &itemHit);
			
			if (err == noErr)
			{
				switch (itemHit)
				{
				case kAlertStdAlertOKButton:
					break;
					
				case kAlertStdAlertCancelButton:
					abort();
					break;
					
				case kAlertStdAlertOtherButton:
					SetQuietAsserts(true);
					break;
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_add_ref(ErrorHandler* eh)
{
	B_ASSERT(eh != NULL);
	
	IncrementAtomic(&eh->mRefCount);
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_release(ErrorHandler* eh)
{
	B_ASSERT(eh != NULL);
	B_ASSERT(eh->mRefCount > 0);
	
	if (DecrementAtomic(&eh->mRefCount) == 1)
	{
		// The ref count is now zero
		delete eh;
	}
}


// ==========================================================================================
//	AutoThrowHandler

#pragma mark -

// ------------------------------------------------------------------------------------------
AutoThrowHandler::AutoThrowHandler(bool quiet /* = true */)
{
	boost::intrusive_ptr<ErrorHandler>	handler(ErrorHandler::Get());
	
	mSavedQuiet = handler->GetQuietThrows();
	handler->SetQuietThrows(quiet);
}

// ------------------------------------------------------------------------------------------
AutoThrowHandler::~AutoThrowHandler()
{
	ErrorHandler::Get()->SetQuietThrows(mSavedQuiet);
}


}	// namespace B




// Messages for B exception classes

//	CFCopyLocalizedStringFromTable("B::PropertyListCreateException",	"B", "Error message when a property list file can't be read.")

// Messages for standard library exception classes

//	CFCopyLocalizedStringFromTable("std::exception",			"B", "Generic message when an exception occurs.")
//	CFCopyLocalizedStringFromTable("std::ios_base::failure",	"B", "Generic message when an I/O exception occurs.")
//	CFCopyLocalizedStringFromTable("std::logic_error",			"B", "Generic message when a logic exception occurs.")
//	CFCopyLocalizedStringFromTable("std::domain_error",			"B", "Generic message when a domain exception occurs.")
//	CFCopyLocalizedStringFromTable("std::invalid_argument",		"B", "Generic message when an invalid-argument exception occurs.")
//	CFCopyLocalizedStringFromTable("std::length_error",			"B", "Generic message when a length exception occurs.")
//	CFCopyLocalizedStringFromTable("std::out_of_range",			"B", "Generic message when an out-of-range exception occurs.")
//	CFCopyLocalizedStringFromTable("std::runtime_error",		"B", "Generic message when a runtime exception occurs.")
//	CFCopyLocalizedStringFromTable("std::overflow_error",		"B", "Generic message when an overflow exception occurs.")
//	CFCopyLocalizedStringFromTable("std::range_error",			"B", "Generic message when a range exception occurs.")
//	CFCopyLocalizedStringFromTable("std::underflow_error",		"B", "Generic message when an underflow exception occurs.")
//	CFCopyLocalizedStringFromTable("boost::format_error",		"B", "Generic message when an exception occurs in the  boost format library.")
//	CFCopyLocalizedStringFromTable("boost::bad_format_string",	"B", "Generic message when a bad-format-string exception occurs in the  boost format library.")
//	CFCopyLocalizedStringFromTable("boost::too_few_args",		"B", "Generic message when a too-few-arguments exception occurs in the  boost format library.")
//	CFCopyLocalizedStringFromTable("boost::too_many_args",		"B", "Generic message when a too-many-arguments exception occurs in the  boost format library.")
//	CFCopyLocalizedStringFromTable("boost::out_of_range",		"B", "Generic message when a out-of-range exception occurs in the  boost format library.")


//// Status codes for well-known exception classes.  B uses numbers in the range 3300-3500
//
//enum {
//	// std::
//	kOSStatus_std_exception				= 3300,
//	kOSStatus_std_bad_alloc				= 3301,
//	kOSStatus_std_bad_cast				= 3302,
//	kOSStatus_std_bad_exception			= 3303,
//	kOSStatus_std_bad_typeid			= 3304,
//	kOSStatus_std_ios_base_failure		= 3305,
//	kOSStatus_std_logic_error			= 3306,
//	kOSStatus_std_domain_error			= 3307,
//	kOSStatus_std_invalid_argument		= 3308,
//	kOSStatus_std_length_error			= 3309,
//	kOSStatus_std_out_of_range			= 3310,
//	kOSStatus_std_runtime_error			= 3311,
//	kOSStatus_std_overflow_error		= 3312,
//	kOSStatus_std_range_error			= 3313,
//	kOSStatus_std_underflow_error		= 3314,
//	// boost::
//	kOSStatus_boost_format_error		= 3315,
//	kOSStatus_boost_bad_format_string	= 3316,
//	kOSStatus_boost_too_few_args		= 3317,
//	kOSStatus_boost_too_many_args		= 3318,
//	kOSStatus_boost_out_of_range		= 3319
//};
