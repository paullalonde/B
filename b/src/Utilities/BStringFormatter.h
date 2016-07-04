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

#ifndef BStringFormatter_H_
#define BStringFormatter_H_

#pragma once

// standard headers
#include <string>

// library headers
#include <boost/format.hpp>

// B headers
#include "BMutableString.h"


namespace B {


/*!	@brief	Type-safe localisable strings.
	
	@todo	%Document this class!
	@todo	Investigate using something lighter-weight than boost::format.
*/
class StringFormatter
{
public:
	
	// constructor / destructor
				StringFormatter(const StringFormatter& inFormatter);
	explicit	StringFormatter(const String& inFormatString);
	explicit	StringFormatter(const std::string& inFormatString);
	explicit	StringFormatter(const char* inFormatString);
	
	StringFormatter&	operator = (const StringFormatter& inFormatter);
	
	String	extract() const;

	StringFormatter& clear(); // empty the string buffers (except bound arguments, see clear_binds() )

	// pass arguments through those operators :
	template <class T>
	StringFormatter&	operator % (const T& x);

#ifndef BOOST_NO_OVERLOAD_FOR_NON_CONST
	template <class T>
	StringFormatter&	operator % (T& x);
#endif

	// system for binding arguments :
	template<class T>  
	StringFormatter&	bind_arg(int argN, const T& val);
	
	StringFormatter&	clear_bind(int argN);
	StringFormatter&	clear_binds();

	// modify the params of a directive, by applying a manipulator :
	template<class T> 
	StringFormatter&	modify_item(int itemN, const T& manipulator);

	// Choosing which errors will throw exceptions :
	unsigned char		exceptions() const;
	unsigned char		exceptions(unsigned char newexcept);
	
private:
	
	static std::string	MakeFormatString(const String& inFormatString);
	
	// member variables
	boost::basic_format<char>	mFormatter;
};

// ------------------------------------------------------------------------------------------
inline StringFormatter&
StringFormatter::clear()
{
	mFormatter.clear();
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <class T> inline StringFormatter&
StringFormatter::operator % (const T& x) 
{ 
	mFormatter % x;
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <> inline StringFormatter&
StringFormatter::operator % (const String& x) 
{
	std::string	sstr;
	
	x.copy(sstr, kCFStringEncodingUTF8);
	
	mFormatter % sstr;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <> inline StringFormatter&
StringFormatter::operator % (const MutableString& x) 
{
	std::string	sstr;
	
	x.copy(sstr, kCFStringEncodingUTF8);
	
	mFormatter % sstr;
	
	return (*this);
}

#ifndef BOOST_NO_OVERLOAD_FOR_NON_CONST
// ------------------------------------------------------------------------------------------
template <class T> inline StringFormatter&
StringFormatter::operator % (T& x) 
{
	mFormatter % x;
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <> inline StringFormatter&
StringFormatter::operator % (String& x) 
{
	std::string	sstr;
	
	x.copy(sstr, kCFStringEncodingUTF8);
	
	mFormatter % sstr;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <> inline StringFormatter&
StringFormatter::operator % (MutableString& x) 
{
	std::string	sstr;
	
	x.copy(sstr, kCFStringEncodingUTF8);
	
	mFormatter % sstr;
	
	return (*this);
}
#endif

// ------------------------------------------------------------------------------------------
template <class T> inline StringFormatter&
StringFormatter::bind_arg(int argN, const T& val) 
{
	mFormatter.bind_arg(argN, val);
	return (*this); 
}

// ------------------------------------------------------------------------------------------
inline StringFormatter&
StringFormatter::clear_bind(int argN)
{
	mFormatter.clear_bind(argN);
	return (*this); 
}

// ------------------------------------------------------------------------------------------
inline StringFormatter&
StringFormatter::clear_binds()
{
	mFormatter.clear_binds();
	return (*this); 
}

// ------------------------------------------------------------------------------------------
template <class T> inline StringFormatter&
StringFormatter::modify_item(int itemN, const T& manipulator) 
{
	mFormatter.modify_item(itemN, manipulator);
	return (*this); 
}

// ------------------------------------------------------------------------------------------
inline unsigned char
StringFormatter::exceptions() const
{
	return (mFormatter.exceptions());
}

// ------------------------------------------------------------------------------------------
inline unsigned char
StringFormatter::exceptions(unsigned char newexcept)
{
	return (mFormatter.exceptions(newexcept));
}

// ------------------------------------------------------------------------------------------
inline String
Extract(const StringFormatter& formatter)
{
	return (formatter.extract());
}


}	// namespace B


#endif	// BStringFormatter_H_
