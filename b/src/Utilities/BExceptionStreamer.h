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

#ifndef BExceptionStreamer_H_
#define BExceptionStreamer_H_

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

// B headers
#include "BErrorHandler.h"
#include "BException.h"


namespace B {

// forward declarations
class	String;


/*!	@brief	Exception serialisation support
	
	ExceptionStreamer is a singleton that manages the serialisation and 
	deserialisation of exception objects.
	
	ExceptionStreamer is "told" about exception classes by calling 
	Register<EXCEPTION>().  Once that is done for a given exception class, instances 
	of that class may be safely serialised.
	
	Serialisation is performed by Externalize(), whereas deserialisation is done by 
	Rethrow().  Developers will rarely if ever need to call these functions directly.
	
	Currently, the format of the serialised exception is a very small XML document:
	
	@verbatim
	<?xml version="1.0" encoding="UTF-8"?>
	<exception class="..." value="..."/>
	@endverbatim
	
	The @c class attribute contains the result of 
	<tt>typeid(<i>exception-object</i>).name()</tt>.  The @c value attribute is optional.
	
	@sa			@ref using_exceptions
	@ingroup	ExceptionGroup
	
	@note	The implementation relies on having a reasonable implementation of 
			@c typeinfo::name().  The returned string needs to (1) be different for 
			each distinct exception class, and (2) always be the same for each class 
			(the value doesn't change at runtime).  Current versions of CodeWarrior 
			and gcc satisfy these requirements;  if this class is ever ported to other 
			compilers we'll have to revisit this area.
*/
class ExceptionStreamer : public boost::noncopyable
{
public:
	
	//! @name Singleton Access
	//@{
	//! Returns the ExceptionStreamer instance.
	static boost::intrusive_ptr<ExceptionStreamer>	Get();
	//@}
	
	//! @name Registration
	//@{
	//! Register class @a EXCEPTION for serialisation/deserialisation.
	template <class EXCEPTION> static bool	Register();
	//! Register rethrower @a RETHROWER for serialisation/deserialisation.
	template <class RETHROWER> static bool	RegisterRethrower();
	//! Unregister class @a EXCEPTION.
	template <class EXCEPTION> static bool	Unregister();
	//@}
	
	//! @name Serialisation / Deserialisation
	//@{
	//! Write out the state of @a ex to @a ostr.
	void	Externalize(
				const std::exception&	ex, 
				std::ostream&			ostr) const;
	//! Instantiate an exception object matching the contents of @a istr, then throw it.
	void	Rethrow(
				std::istream&			istr) const;
	//@}
	
private:
	
	// types
	class RethrowerBase;
	typedef std::map<std::string, RethrowerBase*>	RethrowerMap;
	typedef std::map<char, std::string>				EncodingMap;
	typedef std::map<std::string, char>				DecodingMap;
	
	// nested classes
	class RethrowerBase
	{
	public:
		
		virtual void	Externalize(
							const std::exception&	ex, 
							std::ostream&			ostr) const = 0;
		virtual void	Rethrow(
							std::istream&			istr) const = 0;
	
	protected:
		
		std::string	ReadExString(std::istream& istr) const;
	};
	
	template <class EXCEPTION, class TRAITS>
	class Rethrower : public RethrowerBase
	{
    public:
    
        typedef EXCEPTION   ExceptionType;
        
	private:
    
       	virtual void	Externalize(
							const std::exception&	ex, 
							std::ostream&			ostr) const;
		virtual void	Rethrow(
							std::istream&			istr) const;
	};
	
	template <class EXCEPTION>
	class Rethrower<EXCEPTION, DefaultExceptionTag> : public RethrowerBase
	{
    public:
    
        typedef EXCEPTION   ExceptionType;
        
	private:
    
		virtual void	Externalize(
							const std::exception&	ex, 
							std::ostream&			ostr) const;
		virtual void	Rethrow(
							std::istream&			istr) const;
	};
	
	template <class EXCEPTION>
	class Rethrower<EXCEPTION, StringExceptionTag> : public RethrowerBase
	{
    public:
    
        typedef EXCEPTION   ExceptionType;
        
	private:
    
		virtual void	Externalize(
							const std::exception&	ex, 
							std::ostream&			ostr) const;
		virtual void	Rethrow(
							std::istream&			istr) const;
	};
	
	template <class EXCEPTION>
	class Rethrower<EXCEPTION, StreamExceptionTag> : public RethrowerBase
	{
    public:
    
        typedef EXCEPTION   ExceptionType;
        
	private:
    
		virtual void	Externalize(
							const std::exception&	ex, 
							std::ostream&			ostr) const;
		virtual void	Rethrow(
							std::istream&			istr) const;
	};
	
public:
	
	template <class EXCEPTION>
	class CustomRethrower : public RethrowerBase
	{
    public:
    
        typedef EXCEPTION   ExceptionType;
        
	private:
    
		virtual void	Externalize(
							const std::exception&	ex, 
							std::ostream&			ostr) const;
		virtual void	Write(
							const ExceptionType&	ex, 
							std::ostream&			ostr) const = 0;
	};
	
private:

	//! Constructor.
	ExceptionStreamer();
	//! Destructor.
	~ExceptionStreamer();
	
	template <class EXCEPTION>
	bool		PrivateRegisterType();
	template <class RETHROWER>
	bool		PrivateRegisterRethrower();
	template <class EXCEPTION>
	bool		PrivateUnregister();
	bool		PrivateRegister(
            	   	const char*                     exceptionClass, 
                	std::auto_ptr<RethrowerBase>    rethrower);
	bool		PrivateUnregister(
                	const char*                     exceptionClass);
	
	std::string	Encode(const std::string& inString) const;
	std::string	Decode(const std::string& inString) const;
	
	static void	InitSingleton();
	static void	CleanupSingleton();
	
	// member variables
	SInt32			mRefCount;
	RethrowerMap	mRethrowerMap;
	EncodingMap		mEncodingMap;
	DecodingMap		mDecodingMap;
	std::string		mEncodedChars;
	
	// static member variables
	static boost::once_flag							sInitOnce;
	static bool										sInitFailure;
	static boost::intrusive_ptr<ExceptionStreamer>*	sInstancePtr;
	
	// friends
    friend void     intrusive_ptr_add_ref(ExceptionStreamer* s);
    friend void     intrusive_ptr_release(ExceptionStreamer* s);
};

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> bool
ExceptionStreamer::PrivateRegisterType()
{
    std::auto_ptr<RethrowerBase>    ptr(new Rethrower<EXCEPTION, typename ExceptionTraits<EXCEPTION>::Category>());

	return (PrivateRegister(typeid(EXCEPTION).name(), ptr));
}

// ------------------------------------------------------------------------------------------
/*! The RETHROWER template parameter must derive from 
    CustomRethrower<RETHROWER::ExceptionType>.
*/
template <class RETHROWER> bool
ExceptionStreamer::PrivateRegisterRethrower()
{
    typedef typename RETHROWER::ExceptionType   ExceptionType;
    
    std::auto_ptr<RethrowerBase>    ptr(new RETHROWER());
    
	return (PrivateRegister(typeid(ExceptionType).name(), ptr));
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> bool
ExceptionStreamer::PrivateUnregister()
{
	return (PrivateUnregister(typeid(EXCEPTION).name()));
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> inline bool
ExceptionStreamer::Register()
{
	return (Get()->PrivateRegisterType<EXCEPTION>());
}

// ------------------------------------------------------------------------------------------
template <class RETHROWER> inline bool
ExceptionStreamer::RegisterRethrower()
{
	return (Get()->PrivateRegisterRethrower<RETHROWER>());
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> inline bool
ExceptionStreamer::Unregister()
{
	return (Get()->PrivateUnregister<EXCEPTION>());
}

// ==========================================================================================

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::Rethrower<EXCEPTION, DefaultExceptionTag>::Externalize(
	const std::exception&	/* ex */, 
	std::ostream&			/* ostr */) const
{
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::Rethrower<EXCEPTION, DefaultExceptionTag>::Rethrow(
	std::istream&			/* istr */) const
{
	B_THROW(EXCEPTION());
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::Rethrower<EXCEPTION, StringExceptionTag>::Externalize(
	const std::exception&	ex, 
	std::ostream&			ostr) const
{
	ostr << ex.what();
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::Rethrower<EXCEPTION, StringExceptionTag>::Rethrow(
	std::istream&			istr) const
{
	B_THROW(EXCEPTION(ReadExString(istr)));
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::Rethrower<EXCEPTION, StreamExceptionTag>::Externalize(
	const std::exception&	ex, 
	std::ostream&			ostr) const
{
	dynamic_cast<const EXCEPTION&>(ex).Write(ostr);
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::Rethrower<EXCEPTION, StreamExceptionTag>::Rethrow(
	std::istream&			istr) const
{
	B_THROW(EXCEPTION(istr));
}

// ------------------------------------------------------------------------------------------
template <class EXCEPTION> void
ExceptionStreamer::CustomRethrower<EXCEPTION>::Externalize(
	const std::exception&	ex, 
	std::ostream&			ostr) const
{
    Write(dynamic_cast<const ExceptionType&>(ex), ostr);
}


}	// namespace B


#endif	// BExceptionStreamer_H_
