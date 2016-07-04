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

/*!	@file	BString.h
	
	@brief	String class and related global functions.
*/

#ifndef BString_H_
#define BString_H_

#pragma once

// standard headers
#include <iosfwd>
#include <istream>
#include <iterator>
#include <cstdarg>
#include <string>
#include <vector>

// system headers
#include <CoreFoundation/CFString.h>

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BErrorHandler.h"
#include "BFwd.h"
#include "BOSPtr.h"


namespace B {

// forward declarations
class	MutableString;

// ==========================================================================================
//	String

/*!
	@brief	Adapter class for immutable strings.
	
	String provides a wrapper around a CoreFoundation @c CFStringRef object, giving it an 
	interface similar (nearly identical in fact) to <tt>std::string</tt>.  This makes it 
	possible to use String with STL algorithms, with standard streams, etc.
	
	String tries to share its underlying @c CFStringRef whenever possible.
	
	Specifically, String implements the @c const member functions of @c std::string.  This 
	reflects the immutable nature of the underlying @c CFStringRef.  The exception to this 
	is the various overloads of operator = () and assign(), whose absence would have 
	made the class much more difficult to use;  they are implemented by replacing the 
	underlying @c CFStringRef, rather than by changing its contents.
	
	Other notable differences with <tt>std::basic_string<...></tt>:
		
		- There are no template parameters, because CFStrings only have one underlying 
		  character type, namely @c UniChar.  One can think of String as 
		  analogous to <tt>std::basic_string<UniChar,char_traits<UniChar>,CFAllocatorRef></tt>.
		  
		- There are many more conversions.  This reflects String's role as an adapter 
		  between CoreFoundation and the Standard C++ Library.  For example, one may 
		  construct instances of String with any of the following types:
			- String
			- MutableString
			- <tt>CFStringRef</tt>
			- <tt>const UniChar*</tt>
			- <tt>std::string</tt>
			- <tt>std::wstring</tt>
			- <tt>const char*</tt>
			- <tt>const wchar_t*</tt>
			- <tt>UniChar</tt>
			- <tt>ConstStringPtr</tt>
			- <tt>std::vector<UInt8></tt>
		
		- Likewise, one may copy() a String into one of the following data structures:
			- <tt>CFMutableStringRef</tt>
			- <tt>std::string</tt>
			- <tt>std::wstring</tt>
			- <tt>char*</tt>
			- <tt>wchar_t*</tt>
			- <tt>UniChar*</tt>
			- <tt>StringPtr</tt>
			- <tt>std::vector<UInt8></tt>
		  
		- The allocator type is fixed as @c CFAllocatorRef.  The allocator type is not a 
		  template parameter, however allocators @e are passed as arguments to 
		  constructors and can be retrieved via get_allocator().
		  
		- The underlying @c CFStringRef's storage is not accessible, so it's not 
		  possible to take the address of an element of a String.  String 
		  is incompatible with the STL in this respect.
	
	A number of related @c #defines provide support for localised strings, 
	and are designed to integrate with Apple's localisation tools such 
	as @c genstrings.  They are:
	
		- #BLocalizedString
		- #BLocalizedStringFromTable
		- #BLocalizedStringFromTableInBundle
		- #BLocalizedStringWithDefaultValue
*/
class String
{
public:
	
	//! @name Types
	//@{
#if 0
	//! The type of the character traits.	### UNIMPLEMENTED ###
	//typedef char_traits<XXX>		traits_type;
#endif
	typedef UniChar					value_type;			//!< The type of the characters.
	typedef size_t					size_type;			//!< The unsigned integral type for size values and indices.
	typedef ptrdiff_t				difference_type;	//!< The signed integral type for difference values.
	typedef value_type				const_reference;	//!< The type of constant character references.
	typedef value_type*				pointer;			//!< The type of constant character pointers.
	typedef const value_type*		const_pointer;		//!< The type of constant character pointers.
	class							iterator;			//!< The type of iterators.
	class							const_iterator;		//!< The type of constant iterators.
	typedef CFAllocatorRef			allocator_type;		//!< The type of the allocator.
	
	/*!	@brief	Modifiable reference to a character in a string.
		
		This class is used internally by String and MutableString.
	*/
	class reference
	{
	public:
		
		//! Extracts the character's value.
		operator String::value_type () const					{ return (CFStringGetCharacterAtIndex(mRef, mIndex)); }
		
		//! Changes the character's value.
		reference&	operator = (String::value_type c)			{ String temp(1, c); CFStringReplace(mRef, CFRangeMake(mIndex, 1), temp.cf_ref()); return (*this); }
		
	private:
		
		reference(CFMutableStringRef inRef, CFIndex inIndex)	: mRef(inRef), mIndex(inIndex) {}
		
		// member variables
		CFMutableStringRef	mRef;
		CFIndex				mIndex;
		
		// friends
		friend class	String;
		friend class	MutableString;
		friend class	String::iterator;
	};
	
	/*!	@brief	Modifiable iterator on a string.
		
		This class is used internally by String and MutableString.
	*/
	class iterator : public std::iterator<std::random_access_iterator_tag, 
										  String::value_type, 
										  String::difference_type, 
										  String::pointer, 
										  String::reference>
	{
	public:
		
		// constructor
		iterator()											: mRef(NULL), mIndex(0) {}
		iterator(const iterator& it)						: mRef(it.mRef), mIndex(it.mIndex)	{}
		
		iterator&	operator = (const iterator& it)			{ mRef = it.mRef; mIndex = it.mIndex; return (*this); }
		
		value_type	operator * () const						{ return (CFStringGetCharacterAtIndex(mRef, mIndex)); }
		reference	operator * ()							{ return (reference(mRef, mIndex)); }
		value_type	operator [] (difference_type i) const	{ return (CFStringGetCharacterAtIndex(mRef, mIndex + i)); }
		reference	operator [] (difference_type i)			{ return (reference(mRef, mIndex + i)); }
		
		iterator&	operator ++ ()							{ ++mIndex; return (*this); }
		iterator&	operator -- ()							{ --mIndex; return (*this); }
		iterator&	operator += (difference_type n)			{ mIndex += n; return (*this); }
		iterator&	operator -= (difference_type n)			{ mIndex -= n; return (*this); }
		iterator	operator ++ (int)						{ return (iterator(mRef, mIndex++)); }
		iterator	operator -- (int)						{ return (iterator(mRef, mIndex--)); }
		iterator	operator +  (difference_type n) const	{ return (iterator(mRef, mIndex + n)); }
		iterator	operator -  (difference_type n) const	{ return (iterator(mRef, mIndex - n)); }
		
		// internal
		int				compare(const iterator& it) const	{ return ((mIndex > it.mIndex) ? 1 : ((mIndex < it.mIndex) ? -1 : 0)); }
		difference_type	distance(const iterator& it) const	{ return (mIndex - it.mIndex); }
		
	private:
		
		iterator(CFMutableStringRef inRef, CFIndex inIndex)	: mRef(inRef), mIndex(inIndex)	{}
		
		// member variables
		CFMutableStringRef	mRef;
		CFIndex				mIndex;
		
		// friends
		friend class		String;
		friend class		MutableString;
		friend class		String::const_iterator;
	};
	
	/*!	@brief	Non-modifiable iterator on a string.
		
		This class is used internally by String and MutableString.
	*/
	class const_iterator : public std::iterator<std::random_access_iterator_tag, 
												String::value_type, 
												String::difference_type, 
												String::const_pointer, 
												String::const_reference>
	{
	public:
		
		// constructor
		const_iterator()										: mRef(NULL), mIndex(0) {}
		const_iterator(const const_iterator& it)				: mRef(it.mRef), mIndex(it.mIndex)	{}
		
		const_iterator&	operator = (const const_iterator& it)	{ mRef = it.mRef; mIndex = it.mIndex; return (*this); }
		const_iterator&	operator = (const String::iterator& it)	{ mRef = it.mRef; mIndex = it.mIndex; return (*this); }
		
		value_type	operator * () const							{ return (CFStringGetCharacterAtIndex(mRef, mIndex)); }
		value_type	operator [] (difference_type i) const		{ return (CFStringGetCharacterAtIndex(mRef, mIndex + i)); }
		
		const_iterator&	operator ++ ()							{ ++mIndex; return (*this); }
		const_iterator&	operator -- ()							{ --mIndex; return (*this); }
		const_iterator&	operator += (difference_type n)			{ mIndex += n; return (*this); }
		const_iterator&	operator -= (difference_type n)			{ mIndex -= n; return (*this); }
		const_iterator	operator ++ (int)						{ return (const_iterator(mRef, mIndex++)); }
		const_iterator	operator -- (int)						{ return (const_iterator(mRef, mIndex--)); }
		const_iterator	operator +  (difference_type n) const	{ return (const_iterator(mRef, mIndex + n)); }
		const_iterator	operator -  (difference_type n) const	{ return (const_iterator(mRef, mIndex - n)); }
		
		// internal
		int				compare(const const_iterator& it) const	{ return ((mIndex > it.mIndex) ? 1 : ((mIndex < it.mIndex) ? -1 : 0)); }
		difference_type	distance(const const_iterator& it) const{ return (mIndex - it.mIndex); }
		
	private:
		
		const_iterator(CFStringRef inRef, CFIndex inIndex)		: mRef(inRef), mIndex(inIndex)	{}
		
		// member variables
		CFStringRef	mRef;
		CFIndex		mIndex;
		
		// friends
		friend class	String;
		friend class	MutableString;
	};

	typedef std::reverse_iterator<iterator>			reverse_iterator;		//!< The type of reverse iterators.
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;	//!< The type of constant reverse iterators.
	
	//@}
	
	//! @name Constants
	//@{
	//! Sentinel value meaning "not found" or "all remaining characters"
	static const size_type	npos = size_type(-1);
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
				String();
	//! Copy constructor.
				String(const String& str);
	//! Allocator constructor.
	explicit	String(CFAllocatorRef allocator);
	//! String constructor.
	explicit	String(const String& str, size_type pos, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! MutableString constructor.
	explicit	String(const MutableString& str);
	//! MutableString constructor.
	explicit	String(const MutableString& str, size_type pos, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! @c CFStringRef constructor.
	explicit	String(CFStringRef cfstr);
	//! @c CFStringRef constructor.
	explicit	String(CFStringRef cfstr, const from_copy_t&);
	//! @c CFStringRef constructor.
	explicit	String(CFStringRef cfstr, const std::nothrow_t&);
	//! @c CFStringRef constructor.
	explicit	String(CFStringRef cfstr, const from_copy_t&, const std::nothrow_t&);
	//! @c CFStringRef constructor.
	explicit	String(CFStringRef cfstr, size_type pos, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! @c UniChar array constructor.
	explicit	String(const UniChar* ustr, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! @c UniChar constructor.
	explicit	String(size_type n, UniChar c, CFAllocatorRef allocator = NULL);
	//! @c std::wstring constructor.
	explicit	String(const std::wstring& wstr, size_type pos = 0, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! @c wchar_t array constructor.
	explicit	String(const wchar_t* wcstr, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! @c std::string constructor.
	explicit	String(const std::string& sstr, CFStringEncoding encoding = kCFStringEncodingASCII, size_type pos = 0, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! @c char* array constructor.
	explicit	String(const char* cstr, CFStringEncoding encoding = kCFStringEncodingASCII, size_type n = npos, CFAllocatorRef allocator = NULL);
	//! Pascal string constructor.
	explicit	String(ConstStringPtr pstr, CFStringEncoding encoding = kCFStringEncodingASCII, CFAllocatorRef allocator = NULL);
	//! Blob constructor.
	explicit	String(const std::vector<UInt8>& blob, CFStringEncoding encoding, bool external, CFAllocatorRef allocator = NULL);
	//! @c OSPtr<CFStringRef> constructor.
	explicit	String(const OSPtr<CFStringRef>& cfstr);
	//! @c OSPtr<CFMutableStringRef> constructor.
	explicit	String(const OSPtr<CFMutableStringRef>& cfstr);
	//! Range constructor.
	template <class InputIterator>
	explicit	String(InputIterator first, InputIterator last, CFAllocatorRef allocator = NULL);
	//! Destructor.
				~String();
	//@}
	
	//! @name Operations for Size and Capacity
	//@{
	//! Returns the number of characters in the string.
	size_type	size() const;
	//! Returns the number of characters in the string.
	size_type	length() const;
	//! Returns whether the string is empty.
	bool		empty() const;
	//! Returns the maximum number of characters a string could contain.
	size_type	max_size() const;
	//! Returns the number of characters the string could contain without reallocation.
	size_type	capacity() const;
	//@}
	
	//! @name Comparisons
	//@{
	//! Compares the string to @a str.
	int	compare(const String& str) const;
	//! Compares the string to @a str.
	int	compare(size_type pos1, size_type n1, const String& str) const;
	//! Compares the string to @a str.
	int	compare(size_type pos1, size_type n1, const String& str, size_type pos2, size_type n2) const;
	//! Compares the string to @a cfstr.
	int	compare(CFStringRef cfstr) const;
	//! Compares the string to @a cfstr.
	int	compare(size_type pos1, size_type n1, CFStringRef cfstr) const;
	//! Compares the string to @a cfstr.
	int	compare(size_type pos1, size_type n1, CFStringRef cfstr, size_type pos2, size_type n2) const;
	//! Compares the string to @a ustr.
	int	compare(const UniChar* ustr) const;
	//! Compares the string to @a ustr.
	int	compare(size_type pos1, size_type n1, const UniChar* ustr) const;
	//! Compares the string to @a ustr.
	int	compare(size_type pos1, size_type n1, const UniChar* ustr, size_type n2) const;
	//! Compares the string to @a sstr.
	int	compare(const std::string& sstr, CFStringEncoding encoding) const;
	//! Compares the string to @a sstr.
	int	compare(size_type pos1, size_type n1, const std::string& sstr, CFStringEncoding encoding) const;
	//! Compares the string to @a sstr.
	int	compare(size_type pos1, size_type n1, const std::string& sstr, CFStringEncoding encoding, size_type pos2, size_type n2) const;
	//@}
	
	//! @name Character Access
	//@{
	//! Returns the character at index @a pos.
	const_reference	operator [] (size_type pos) const;
	//! Returns the character at index @a pos.
	const_reference	at(size_type pos) const;
	//@}
	
	/*! @name Generating CFStringRefs and Character Arrays.
		
		Because of the fundamental differences in underlying storage between String 
		and <tt>std::basic_string<></tt>, it's impossible to provide direct counterparts to 
		@a <tt>std::basic_string<>c_str()</tt> and <tt>std::basic_string<>data()</tt>.  
		@c cf_ref() is analogous to @c c_str().
		
		The various overloads of copy() allow conversions from String to a number 
		of output types.
	*/
	//@{
	//! Returns the underlying @c CFStringRef.
	CFStringRef	cf_ref() const;
	//! Returns a "smart pointer" to the underlying @c CFStringRef.
	OSPtr<CFStringRef>	cf_ptr() const;
	//! Copies the string to a @c UniChar array.
	size_type	copy(UniChar* ustr, size_type n, size_type pos = 0) const;
	//! Copies the string to a @c wchar_t array.
	size_type	copy(wchar_t* wcstr, size_type n, size_type pos = 0) const;
	//! Copies at most @a n bytes, starting at index @a pos in @c *this, into the buffer pointed to by @a cstr.
	size_type	copy(char* cstr, size_type n, size_type& pos, CFStringEncoding encoding) const;
	//! Copies the string to a @c CFMutableStringRef.
	void		copy(CFMutableStringRef cfstr) const;
	//! Copies the string to a @c std::wstring.
	void		copy(std::wstring& wstr) const;
	//! Copies the string to a @c std::string.
	void		copy(std::string& sstr, CFStringEncoding encoding) const;
	//! Copies the string to a Pascal String.
	void		copy(StringPtr pstr, size_type n, CFStringEncoding encoding) const;
	//! Copies the string to a blob.
	void		copy(std::vector<UInt8>& blob, CFStringEncoding encoding, bool external, char loss_byte = 0) const;
	//@}
	
	/*! @name Assignment
		
		These various overloads of @c operator = and @c assign() replace the string's 
		underlying @c CFStringRef.
	*/
	//@{
	//! String assignemnt.
	String&	operator = (const String& str);
	//! MutableString assignemnt.
	String&	operator = (const MutableString& str);
	//! @c CFStringRef assignemnt.
	String&	operator = (CFStringRef cfstr);
	//! @c CFMutableStringRef assignemnt.
	String&	operator = (CFMutableStringRef cfstr);
	//! @c OSPtr<CFStringRef> assignemnt.
	String&	operator = (const OSPtr<CFStringRef>& cfstr);
	//! @c OSPtr<CFMutableStringRef> assignemnt.
	String&	operator = (const OSPtr<CFMutableStringRef>& cfstr);
	//! @c UniChar array assignemnt.
	String&	operator = (const UniChar* ustr);
	//! @c UniChar assignemnt.
	String&	operator = (UniChar c);
	//! @c std::wstring assignemnt.
	String&	operator = (const std::wstring& wstr);
	//! Null-terminated @c wchar_t array assignemnt.
	String&	operator = (const wchar_t* ustr);
	//! @c std::string assignemnt.
	String&	operator = (const std::string& sstr);
	//! Null-terminated @c char array assignemnt.
	String&	operator = (const char* cstr);
	//! Pascal string assignemnt.
	String&	operator = (ConstStringPtr pstr);
	//! String assignemnt.
	String&	assign(const String& str, size_type pos = 0, size_type n = npos);
	//! MutableString assignemnt.
	String&	assign(const MutableString& str, size_type pos = 0, size_type n = npos);
	//! @c CFStringRef assignemnt.
	String&	assign(CFStringRef cfstr);
	//! @c CFStringRef assignemnt.
	String&	assign(CFStringRef cfstr, const from_copy_t&);
	//! @c CFStringRef assignemnt.
	String&	assign(CFStringRef cfstr, const std::nothrow_t&);
	//! @c CFStringRef assignemnt.
	String&	assign(CFStringRef cfstr, const from_copy_t&, const std::nothrow_t&);
	//! @c CFStringRef assignemnt.
	String&	assign(CFStringRef cfstr, size_type pos, size_type n = npos);
	//! @c UniChar array assignemnt.
	String&	assign(const UniChar* ustr, size_type n = npos);
	//! @c UniChar assignemnt.
	String&	assign(size_type n, UniChar c);
	//! @c std::wstring assignemnt.
	String&	assign(const std::wstring& wstr, size_type pos = 0, size_type n = npos);
	//! @c wchar_t array assignemnt.
	String&	assign(const wchar_t* wcstr, size_type n = npos);
	//! @c std::string assignemnt.
	String&	assign(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0, size_type n = npos);
	//! @c char array assignemnt.
	String&	assign(const char* cstr, CFStringEncoding encoding, size_type n = npos);
	//! Pascal string assignemnt.
	String&	assign(ConstStringPtr pstr, CFStringEncoding encoding);
	//! Blob assignemnt.
	String&	assign(const std::vector<UInt8>& blob, CFStringEncoding encoding, bool external);
	//! Range assignment.
	template <class InputIterator>
	String&	assign(InputIterator first, InputIterator last);
	//! Exchanges the contents of the string with @a str.
	void	swap(String& str);
	//@}
	
	//! @name Erasing Characters
	//@{
	//! Removes all characters.
	void	clear();
	//@}

	//! @name Finding a Character
	//@{
	//! Finds the first occurrence of given character.
	size_type	find(UniChar c, size_type pos = 0) const;
	//! Finds the last occurrence of given character.
	size_type	rfind(UniChar c, size_type pos = 0) const;
	//@}
	
	//! @name Finding a Substring
	//@{
	//! Finds the first String.
	size_type	find(const String& str, size_type pos = 0) const;
	//! Finds the last String.
	size_type	rfind(const String& str, size_type pos = 0) const;
	//! Finds the first @c CFStringRef.
	size_type	find(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the last @c CFStringRef.
	size_type	rfind(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the first @c UniChar array.
	size_type	find(const UniChar* ustr, size_type pos = 0, size_type n = npos) const;
	//! Finds the last @c UniChar array.
	size_type	rfind(const UniChar* ustr, size_type pos = 0, size_type n = npos) const;
	//! Finds the first @c std::string.
	size_type	find(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//! Finds the last @c std::string.
	size_type	rfind(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//@}
	
	//! @name Finding the First of a Set of Characters
	//@{
	//! Finds the first character in a set given as a String.
	size_type	find_first_of(const String& str, size_type pos = 0) const;
	//! Finds the first character not in a set given as a String.
	size_type	find_first_not_of(const String& str, size_type pos = 0) const;
	//! Finds the first character in a set given as a @c CFStringRef.
	size_type	find_first_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the first character not in a set given as a @c CFStringRef.
	size_type	find_first_not_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the first character in a set given as a @c UniChar array.
	size_type	find_first_of(const UniChar* ustr, size_type pos = 0, size_type n = npos) const;
	//! Finds the first character not in a set given as a @c UniChar array.
	size_type	find_first_not_of(const UniChar* ustr, size_type pos = 0, size_type n = npos) const;
	//! Finds the first character in a set given as a @c std::string.
	size_type	find_first_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//! Finds the first character not in a set given as a @c std::string.
	size_type	find_first_not_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//@}
	
	//! @name Finding the Last of a Set of Characters
	//@{
	//! Finds the last character in a set given as a String.
	size_type	find_last_of(const String& str, size_type pos = 0) const;
	//! Finds the last character not in a set given as a String.
	size_type	find_last_not_of(const String& str, size_type pos = 0) const;
	//! Finds the last character in a set given as a @c CFStringRef.
	size_type	find_last_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the last character not in a set given as a @c CFStringRef.
	size_type	find_last_not_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the last character in a set given as a @c UniChar array.
	size_type	find_last_of(const UniChar* ustr, size_type pos = 0, size_type n = npos) const;
	//! Finds the last character not in a set given as a @c UniChar array.
	size_type	find_last_not_of(const UniChar* ustr, size_type pos = 0, size_type n = npos) const;
	//! Finds the last character in a set given as a @c std::string.
	size_type	find_last_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//! Finds the last character not in a set given as a @c std::string.
	size_type	find_last_not_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//@}
	
	//! @name Substrings
	//@{
	//! Returns a substring.
	String		substr(size_type pos = 0, size_type n = npos) const;
	//@}
	
	//! @name Generating Iterators
	//@{
	//! Returns a constant random access iterator for the beginning of the string.
	const_iterator			begin() const;
	//! Returns a constant random access iterator for the end of the string.
	const_iterator			end() const;
	//! Returns a constant random access iterator for the beginning of a reverse iteration of the string.
	const_reverse_iterator	rbegin() const;
	//! Returns a constant random access iterator for the end of a reverse iteration of the string.
	const_reverse_iterator	rend() const;
	//@}
	
	//! @name Allocator Support
	//@{
	//! Returns the string's allocator.
	allocator_type	get_allocator() const;
	//@}
	
	//! @name I/O Support
	//@{
	//! Write the characters of @a str to @a ostr.
	std::ostream&	Write(std::ostream& ostr, CFStringEncoding encoding) const;
	//! Reads the characters of the next word from @a istr into the string.
	std::istream&	Read(std::istream& istr, CFStringEncoding encoding);
	//! Reads the characters of the next line from @a istr into the string.
	std::istream&	GetLine(std::istream& istr, char delim, CFStringEncoding encoding);
	//@}
	
private:
	
	static OSPtr<CFStringRef>	make_str(CFStringRef cfstr, size_type pos, size_type n, CFAllocatorRef allocator, bool is_temp);
	static OSPtr<CFStringRef>	make_str(const UniChar* ustr, size_type pos, size_type n, CFAllocatorRef allocator, bool is_temp);
	static OSPtr<CFStringRef>	make_str(size_type n, UniChar c, CFAllocatorRef allocator);
	static OSPtr<CFStringRef>	make_str(const wchar_t* wcstr, size_type pos, size_type n, CFAllocatorRef allocator, bool is_temp);
	static OSPtr<CFStringRef>	make_str(const char* cstr, size_type pos, size_type n, CFStringEncoding encoding, CFAllocatorRef allocator, bool is_temp);
	static OSPtr<CFStringRef>	make_str(ConstStringPtr pstr, CFStringEncoding encoding, CFAllocatorRef allocator, bool is_temp);
	static size_t				uni_strlen(const UniChar* ustr);
	
	// comparisons
	static int	private_compare(CFStringRef ref, CFStringRef cfstr);
	static int	private_compare(CFStringRef ref, size_type pos1, size_type n1, CFStringRef cfstr);
	static int	private_compare(CFStringRef ref, size_type pos1, size_type n1, CFStringRef cfstr, size_type pos2, size_type n2);
	static int	private_compare(CFStringRef ref, const UniChar* ustr);
	static int	private_compare(CFStringRef ref, size_type pos1, size_type n1, const UniChar* ustr);
	static int	private_compare(CFStringRef ref, size_type pos1, size_type n1, const UniChar* ustr, size_type n2);
	static int	private_compare(CFStringRef ref, const std::string& sstr, CFStringEncoding encoding);
	static int	private_compare(CFStringRef ref, size_type pos1, size_type n1, const std::string& sstr, CFStringEncoding encoding);
	static int	private_compare(CFStringRef ref, size_type pos1, size_type n1, const std::string& sstr, CFStringEncoding encoding, size_type pos2, size_type n2);
	
	// copying
	static size_type	private_copy(CFStringRef ref, UniChar* ustr, size_type n, size_type pos);
	static size_type	private_copy(CFStringRef ref, wchar_t* wcstr, size_type n, size_type pos);
	static size_type	private_copy(CFStringRef ref, char* cstr, size_type n, size_type& pos, CFStringEncoding encoding);
	static void			private_copy(CFStringRef ref, std::wstring& wstr);
	static void			private_copy(CFStringRef ref, std::string& sstr, CFStringEncoding encoding);
	static void			private_copy(CFStringRef ref, StringPtr pstr, size_type n, CFStringEncoding encoding);
	static void			private_copy(CFStringRef ref, std::vector<UInt8>& blob, CFStringEncoding encoding, bool external, char loss_byte);
	
	// finding a character
	static size_type	private_find(CFStringRef ref, UniChar c, size_type pos, unsigned opts);
	
	// finding a substring
	static size_type	private_find(CFStringRef ref, CFStringRef cfstr, size_type pos, unsigned opts);
	static size_type	private_find(CFStringRef ref, const UniChar* ustr, size_type pos, size_type n, unsigned opts);
	static size_type	private_find(CFStringRef ref, const std::string& sstr, CFStringEncoding encoding, size_type pos, unsigned opts);
	
	// finding a character from a set
	static size_type	private_find_of(CFStringRef ref, CFStringRef cfstr, size_type pos, unsigned opts, bool negate);
	static size_type	private_find_of(CFStringRef ref, const UniChar* ustr, size_type pos, size_type n, unsigned opts, bool negate);
	static size_type	private_find_of(CFStringRef ref, const std::string& sstr, CFStringEncoding encoding, size_type pos, unsigned opts, bool negate);
	
	// member variables
	CFStringRef	mRef;
	
	// friends
	friend class	MutableString;
};

// ------------------------------------------------------------------------------------------
/*!	Creates an empty string using the default allocator.
*/
inline
String::String()
	: mRef(CFSTR(""))
{
	OSPtrOwnership::CFObjectOwnershipTrait::retain(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a str, using @a str's allocator.
*/
inline
String::String(
	const String&	str)	//!< The source string.
		: mRef(str.cf_ref())
{
	OSPtrOwnership::CFObjectOwnershipTrait::retain(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by all character of the range [@a first, @a last).
*/
template <class InputIterator>
String::String(
	InputIterator	first,					//!< The start of the range of characters for the new string.
	InputIterator	last,					//!< The end of the range of characters for the new string.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	std::vector<UniChar>	temp(first, last);
	
	mRef = make_str(&temp[0], 0, temp.size(), allocator, false).release();
}

// ------------------------------------------------------------------------------------------
inline
String::~String()
{
	OSPtrOwnership::CFObjectOwnershipTrait::release(mRef);
}

// ------------------------------------------------------------------------------------------
inline String::size_type
String::size() const
{
	return (CFStringGetLength(mRef));
}

// ------------------------------------------------------------------------------------------
inline String::size_type
String::length() const
{
	return (CFStringGetLength(mRef));
}

// ------------------------------------------------------------------------------------------
inline bool
String::empty() const
{
	return (CFStringGetLength(mRef) == 0);
}

// ------------------------------------------------------------------------------------------
inline int
String::private_compare(CFStringRef ref, CFStringRef cfstr)
{
	return (CFStringCompare(ref, cfstr, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the string @a cfstr.
	
	@return		< 0 if @c *this < @a cfstr;  0 if @c *this == @a cfstr;  > 0 if @c *this > @a cfstr.
*/
inline int
String::compare(
	CFStringRef	cfstr)	//!< The string to compare against.
	const
{
	return (private_compare(mRef, cfstr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with string @a cfstr.
	
	@return		< 0 if @c *this < @a cfstr;  0 if @c *this == @a cfstr;  > 0 if @c *this > @a cfstr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
String::compare(
	size_type	pos1,	//!< The index of the first character to compare in @c *this.
	size_type	n1,		//!< The number of characters to compare in @c *this.
	CFStringRef	cfstr)	//!< The string to compare against.
	const
{
	return (private_compare(mRef, pos1, n1, cfstr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with, at most, @a n2 characters of string @a cfstr starting with index @a pos2.
	
	@return		< 0 if @c *this < @a cfstr;  0 if @c *this == @a cfstr;  > 0 if @c *this > @a cfstr.
	@exception	std::out_of_range	If @a pos1 > @c size() or @a pos2 > @c CFStringGetLength( @a cfstr ).
*/
inline int
String::compare(
	size_type	pos1,	//!< The index of the first character to compare in @c *this.
	size_type	n1,		//!< The number of characters to compare in @c *this.
	CFStringRef	cfstr,	//!< The string to compare against.
	size_type	pos2,	//!< The index of the first character to compare in @a cfstr.
	size_type	n2)		//!< The number of characters to compare in @a cfstr.
	const
{
	return (private_compare(mRef, pos1, n1, cfstr, pos2, n2));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the string @a str.

	@return		< 0 if @c *this < @a str;  0 if @c *this == @a str;  > 0 if @c *this > @a str.
*/
inline int
String::compare(
	const String&	str)	//!< The string to compare against.
	const
{
	return (private_compare(mRef, str.cf_ref()));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with string @a str.

	@return		< 0 if @c *this < @a str;  0 if @c *this == @a str;  > 0 if @c *this > @a str.
*/
inline int
String::compare(
	size_type		pos1,	//!< The index of the first character to compare in @c *this.
	size_type		n1,		//!< The number of characters to compare in @c *this.
	const String&	str)	//!< The string to compare against.
	const
{
	return (private_compare(mRef, pos1, n1, str.cf_ref()));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with, at most, @a n2 characters of string @a str starting with index @a pos2.
	
	@return		< 0 if @c *this < @a str;  0 if @c *this == @a str;  > 0 if @c *this > @a str.
	@exception	std::out_of_range	If @a pos1 > @c size() or @a pos2 > @a str.size().
*/
inline int
String::compare(
	size_type		pos1,	//!< The index of the first character to compare in @c *this.
	size_type		n1,		//!< The number of characters to compare in @c *this.
	const String&	str,	//!< The string to compare against.
	size_type		pos2,	//!< The index of the first character to compare in @a str.
	size_type		n2)		//!< The number of characters to compare in @a str.
	const
{
	return (private_compare(mRef, pos1, n1, str.cf_ref(), pos2, n2));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the null-terminated @c UniChar array @a ustr.
	
	@return		< 0 if @c *this < @a ustr;  0 if @c *this == @a ustr;  > 0 if @c *this > @a ustr.
*/
inline int
String::compare(
	const UniChar*	ustr)	//!< The string to compare against.
	const
{
	return (private_compare(mRef, ustr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with the null-terminated @c UniChar array @a ustr.
	
	@return		< 0 if @c *this < @a ustr;  0 if @c *this == @a ustr;  > 0 if @c *this > @a ustr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
String::compare(
	size_type		pos1,	//!< The index of the first character to compare in @c *this.
	size_type		n1,		//!< The number of characters to compare in @c *this.
	const UniChar*	ustr)	//!< The string to compare against.
	const
{
	return (private_compare(mRef, pos1, n1, ustr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with @a n2 characters of @c UniChar array @a ustr.
	
	@return		< 0 if @c *this < @a ustr;  0 if @c *this == @a ustr;  > 0 if @c *this > @a ustr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
String::compare(
	size_type		pos1,	//!< The index of the first character to compare in @c *this.
	size_type		n1,		//!< The number of characters to compare in @c *this.
	const UniChar*	ustr,	//!< The string to compare against.
	size_type		n2)		//!< The number of characters to compare in @a ustr.
	const
{
	return (private_compare(mRef, pos1, n1, ustr, n2));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the string @a sstr, which is assumed 
	to be encoded using the character encoding given in @a encoding.
	
	@return		< 0 if @c *this < @a sstr;  0 if @c *this == @a sstr;  > 0 if @c *this > @a sstr.
*/
inline int
String::compare(
	const std::string&	sstr,		//!< The string to compare against.
	CFStringEncoding	encoding)	//!< The character encoding of @a sstr.
	const
{
	return (private_compare(mRef, sstr, encoding));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with the string @a sstr, which is assumed to be encoded using the character encoding 
	given in @a encoding.
	
	@return		< 0 if @c *this < @a sstr;  0 if @c *this == @a sstr;  > 0 if @c *this > @a sstr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
String::compare(
	size_type			pos1,		//!< The index of the first character to compare in @c *this.
	size_type			n1,			//!< The number of characters to compare in @c *this.
	const std::string&	sstr,		//!< The string to compare against.
	CFStringEncoding	encoding)	//!< The character encoding of @a sstr.
	const
{
	return (private_compare(mRef, pos1, n1, sstr, encoding));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with, at most, @a n2 characters of string @a sstr starting with index @a pos2.  @a Sstr 
	is assumed to be encoded using the character encoding given in @a encoding.
	
	@return		< 0 if @c *this < @a sstr;  0 if @c *this == @a sstr;  > 0 if @c *this > @a sstr.
	@exception	std::out_of_range	If @a pos1 > @c size() or @a pos2 > @a sstr.size().
*/
inline int
String::compare(
	size_type			pos1,		//!< The index of the first character to compare in @c *this.
	size_type			n1,			//!< The number of characters to compare in @c *this.
	const std::string&	sstr,		//!< The string to compare against.
	CFStringEncoding	encoding,	//!< The character encoding of @a sstr.
	size_type			pos2,		//!< The index of the first character to compare in @a str.
	size_type			n2)			//!< The number of characters to compare in @a sstr.
	const
{
	return (private_compare(mRef, pos1, n1, sstr, encoding, pos2, n2));
}

// ------------------------------------------------------------------------------------------
/*!	If @a pos equals @c length(), the function returns '\\0'.  If @a pos is smaller than 
	zero or greater than @c length(), the results are undefined.
*/
inline String::const_reference
String::operator [] (
	size_type	pos)	//!< The index of the character to return.
	const
{
	if (pos == length())
		return ('\0');
	else
		return (CFStringGetCharacterAtIndex(mRef, pos));
}

// ------------------------------------------------------------------------------------------
inline CFStringRef
String::cf_ref() const
{
	return (mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Copies at most @a n characters, starting at index @a pos in @c *this, into the buffer 
	pointed to by @a ustr.  No null character is appended.
	
	@return	The number of copied characters.
*/
inline String::size_type
String::copy(
	UniChar*	ustr,			//!< The output buffer.
	size_type	n,				//!< The maximum number of characters to copy.
	size_type	pos /* = 0 */)	//!< The starting index (in @c *this) for conversion.
	const
{
	return (private_copy(mRef, ustr, n, pos));
}

// ------------------------------------------------------------------------------------------
/*!	Copies at most @a n characters, starting at index @a pos in @c *this, into the buffer 
	pointed to by @a wcstr.  The string's characters are converted to @c wchar_t upon 
	copying.  No null character is appended.
	
	@return	The number of copied characters.
*/
inline String::size_type
String::copy(
	wchar_t*	wcstr,			//!< The output buffer.
	size_type	n,				//!< The maximum number of characters to copy.
	size_type	pos /* = 0 */)	//!< The starting index (in @c *this) for conversion.
	const
{
	return (private_copy(mRef, wcstr, n, pos));
}

// ------------------------------------------------------------------------------------------
/*!	Copies at most @a n characters, starting at index @a pos in @c *this, into the buffer 
	pointed to by @a wcstr.  The string's characters are converted to @a encoding upon 
	copying.  No null character is appended.
	
	@return	The number of copied characters.
*/
inline String::size_type
String::copy(
	char*				cstr,		//!< The output buffer.
	size_type			n,			//!< The maximum number of characters to copy.
	size_type&			pos,		//!< The starting index (in @c *this) for conversion.  On output, holds the index of the next character to convert, or @c npos if conversion is complete.
	CFStringEncoding	encoding)	//!< The output buffer's encoding.
	const
{
	return (private_copy(mRef, cstr, n, pos, encoding));
}

// ------------------------------------------------------------------------------------------
/*!	The current contents of @a cfstr are completely replaced.  If @c cfstr doesn't have 
	enough capacity to hold the contents, the results are undefined.
*/
inline void
String::copy(
	CFMutableStringRef	cfstr)	//!< The output string.
	const
{
	CFStringReplaceAll(cfstr, cf_ref());
}

// ------------------------------------------------------------------------------------------
/*!	The string's characters are converted to @c wchar_t upon copying.
*/
inline void
String::copy(
	std::wstring&	wstr)	//!< The output string.
	const
{
	private_copy(mRef, wstr);
}

// ------------------------------------------------------------------------------------------
/*!	The string's characters are converted to @a encoding upon copying.
*/
inline void
String::copy(
	std::string&		sstr,		//!< The output string.
	CFStringEncoding	encoding)	//!< The output string's encoding.
	const
{
	private_copy(mRef, sstr, encoding);
}

// ------------------------------------------------------------------------------------------
/*!	The string's characters are converted to @a encoding upon copying.
*/
inline void
String::copy(
	StringPtr			pstr,		//!< The output string.
	size_type			n,			//!< The maximum size of the output string, including length byte.
	CFStringEncoding	encoding)	//!< The output string's encoding.
	const
{
	private_copy(mRef, pstr, n, encoding);
}

// ------------------------------------------------------------------------------------------
/*!	If @a external is @c true, then the string is converted to an "external form", which 
	usually means it is made suitable for writing to a file.  In Unicode-speak, this implies 
	prepending a BOM (Byte-Order Mark) character.  The string's characters are converted to 
	@a encoding upon copying.  If @a loss_byte is non-zero, then any characters 
	unconvertible to @a encoding will be replaced by @a loss_byte;  else, conversion errors 
	will cause an exception.
*/
inline void
String::copy(
	std::vector<UInt8>&	blob,					//!< The output buffer.
	CFStringEncoding	encoding,				//!< The output string's encoding.
	bool				external,				//!< If @c true, blob get a leading BOM.
	char				loss_byte /* = 0 */)	//!< The character to substitute for unconvertible characters.
	const
{
	private_copy(mRef, blob, encoding, external, loss_byte);
}

// ------------------------------------------------------------------------------------------
inline void
String::swap(String& str)
{
	std::swap(mRef, str.mRef);
}

// ------------------------------------------------------------------------------------------
inline String&
String::operator = (
	const String&	str)	//!< The input string.
{
	OSPtrOwnership::CFObjectOwnershipTrait::retain(str.cf_ref());
	OSPtrOwnership::CFObjectOwnershipTrait::release(mRef);
	mRef = str.cf_ref();
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline String&
String::operator = (
	CFStringRef	cfstr)	//!< The input string.
{
	B_ASSERT(cfstr != NULL);
	
	CFStringRef	newRef;
	
	newRef = CFStringCreateCopy(CFGetAllocator(cfstr), cfstr);
	B_THROW_IF_NULL(newRef);
	
	OSPtrOwnership::CFObjectOwnershipTrait::release(mRef);
	mRef = newRef;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns the string with all character of the range [@a first, @a last).
*/
template <class InputIterator> String&
String::assign(
	InputIterator	first,	//!< The start of the range of characters to assign.
	InputIterator	last)	//!< The end of the range of characters to assign.
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	String	temp(first, last, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
String::clear()
{
	OSPtrOwnership::CFObjectOwnershipTrait::release(mRef);
	mRef = CFSTR("");
	OSPtrOwnership::CFObjectOwnershipTrait::retain(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the character @a c, starting at index @a pos.
	
	@return	The index of @a c, or @c npos if not found.
*/
inline String::size_type
String::find(
	UniChar		c,				//!< The character to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, c, pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the character @a c, starting at index @a pos.
	
	@return	The index of @a c, or @c npos if not found.
*/
inline String::size_type
String::rfind(
	UniChar		c,				//!< The character to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, c, pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the string @a str, starting at index @a pos.
	
	@return	The index of @a str, or @c npos if not found.
*/
inline String::size_type
String::find(
	const String&	str,			//!< The string to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, str.cf_ref(), pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the string @a str, starting at index @a pos.
	
	@return	The index of @a str, or @c npos if not found.
*/
inline String::size_type
String::rfind(
	const String&	str,			//!< The string to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, str.cf_ref(), pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the string @a cfstr, starting at index @a pos.
	
	@return	The index of @a cfstr, or @c npos if not found.
*/
inline String::size_type
String::find(
	CFStringRef	cfstr,			//!< The string to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, cfstr, pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the string @a cfstr, starting at index @a pos.
	
	@return	The index of @a cfstr, or @c npos if not found.
*/
inline String::size_type
String::rfind(
	CFStringRef	cfstr,			//!< The string to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, cfstr, pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the @c UniChar array @a ustr of length @a n, 
	starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline String::size_type
String::find(
	const UniChar*	ustr,			//!< The string to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (private_find(mRef, ustr, pos, n, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the @c UniChar array @a ustr of length @a n, 
	starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline String::size_type
String::rfind(
	const UniChar*	ustr,			//!< The string to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (private_find(mRef, ustr, pos, n, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the string @a sstr, starting at index @a pos.  
	The input string is assumed to be encoded using the character encoding given in 
	@a encoding.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline String::size_type
String::find(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, sstr, encoding, pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the string @a sstr, starting at index @a pos.
	The input string is assumed to be encoded using the character encoding given in 
	@a encoding.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline String::size_type
String::rfind(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find(mRef, sstr, encoding, pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_of(
	const String&	str,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, str.cf_ref(), pos, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_not_of(
	const String&	str,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, str.cf_ref(), pos, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, cfstr, pos, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_not_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, cfstr, pos, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (private_find_of(mRef, ustr, pos, n, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_not_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (private_find_of(mRef, ustr, pos, n, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a sstr, starting at 
	index @a pos.  The input string is assumed to be encoded using the character encoding 
	given in @a encoding.

	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, sstr, encoding, pos, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a sstr, 
	starting at index @a pos.  The input string is assumed to be encoded using the 
	character encoding given in @a encoding.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_first_not_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, sstr, encoding, pos, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_of(
	const String&	str,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, str.cf_ref(), pos, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_not_of(
	const String&	str,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, str.cf_ref(), pos, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, cfstr, pos, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_not_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, cfstr, pos, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (private_find_of(mRef, ustr, pos, n, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_not_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (private_find_of(mRef, ustr, pos, n, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a sstr, starting at 
	index @a pos.  The input string is assumed to be encoded using the character encoding 
	given in @a encoding.

	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, sstr, encoding, pos, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a sstr, 
	starting at index @a pos.  The input string is assumed to be encoded using the 
	character encoding given in @a encoding.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline String::size_type
String::find_last_not_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (private_find_of(mRef, sstr, encoding, pos, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Returns a substring of, at most, @a n characters of the string @c *this starting 
	with index @a pos.
*/
inline String
String::substr(
	size_type	pos /* = 0 */,	//!< The starting position for the substring.
	size_type	n /* = npos */)	//!< The length of the substring.
	const
{
	return (String(cf_ref(), pos, n, get_allocator()));
}

// ------------------------------------------------------------------------------------------
inline String::allocator_type
String::get_allocator() const
{
	return (CFGetAllocator(mRef));
}


// ==========================================================================================
//	String Global Functions

/*!	@defgroup	StringFunctions	String Global Functions
*/
//@{

//! @name String Concatenation
//@{

/*!	Returns a new string consisting of the concatenation of @a s1 and @a s2.
	
	@relates	String
*/
String	operator + (const String&       s1, const String&       s2);
/*! @overload
*/
String	operator + (const String&       s1, CFStringRef         s2);
/*! @overload
*/
String	operator + (CFStringRef         s1, const String&       s2);
/*! @overload
*/
String	operator + (const String&       s1, const UniChar*      s2);
/*! @overload
*/
String	operator + (const UniChar*      s1, const String&       s2);
/*! @overload
*/
String	operator + (const String&       s1, UniChar	            c);
/*! @overload
*/
String	operator + (UniChar              c, const String&       s2);
/*! @overload
*/
String	operator + (const String&       s1, const std::string&  s2);
/*! @overload
*/
String	operator + (const std::string&  s1, const String&       s2);
//@}


/*! @name Input/Output Functions
	
	String only supports I/O to @c char streams (i.e., 
	<tt>std::basic_ostream<char,char_traits<char>></tt>).
*/
//@{

// ------------------------------------------------------------------------------------------
/*! Writes the characters of @a str to @a os.  @a str is converted to ASCII prior to being 
	written.  If the conversion fails (eg there are non-7-bit-ASCII characters in @a str), 
	an exception may be thrown.
	
	If you need to write characters to a @c std::ostream that are in an encoding other 
	than 7-bit ASCII, use String::Write() instead.
	
	@relates	String
*/
inline std::ostream&
operator << (std::ostream& os, const String& str)
{
	return (str.Write(os, kCFStringEncodingASCII));
}

// ------------------------------------------------------------------------------------------
/*! Reads the characters of the next word from @a is into the string @a str.  The 
	characters read from @a is are assumed to be 7-bit ASCII.  If they aren't, an 
	exception may be thrown.
	
	If you need to read characters from a @c std::istream that are in an encoding other 
	than 7-bit ASCII, use String::Read() instead.
	
	@relates	String
*/
inline std::istream&
operator >> (std::istream& is, String& str)
{
	return (str.Read(is, kCFStringEncodingASCII));
}

// ------------------------------------------------------------------------------------------
/*! Reads the characters of the next line from @a is into the string @a str.  The 
	characters read from @a is are assumed to be 7-bit ASCII.  If they aren't, an 
	exception may be thrown.
	
	If you need to read characters from a @c std::istream that are in an encoding other 
	than 7-bit ASCII, use String::Read() instead.
	
	@relates	String
*/
inline std::istream&
getline(std::istream& is, String& str, char delim)
{
	return (str.GetLine(is, delim, kCFStringEncodingASCII));
}

// ------------------------------------------------------------------------------------------
/*! Reads the characters of the next line from @a is into the string @a str.  The 
	characters read from @a is are assumed to be 7-bit ASCII.  If they aren't, an 
	exception may be thrown.
	
	If you need to read characters from a @c std::istream that are in an encoding other 
	than 7-bit ASCII, use String::Read() instead.
	
	@relates	String
*/
inline std::istream&
getline(std::istream& is, String& str)
{
	return (str.GetLine(is, is.widen('\n'), kCFStringEncodingASCII));
}
//@}


//! @name String Comparisons
//@{

// ------------------------------------------------------------------------------------------
/*! Compares two strings for equality.
	
	@return		@c true if @a s1 is equal to @a s2
	@relates	String
*/
inline bool	operator == (const String&  s1, const String&  s2)	{ return (s1.compare(s2) == 0); }
/*! @overload
*/
inline bool	operator == (const String&  s1, CFStringRef    s2)	{ return (s1.compare(s2) == 0); }
/*! @overload
*/
inline bool	operator == (CFStringRef    s1, const String&  s2)	{ return (s2.compare(s1) == 0); }
/*! @overload
*/
inline bool	operator == (const String&  s1, const UniChar* s2)	{ return (s1.compare(s2) == 0); }
/*! @overload
*/
inline bool	operator == (const UniChar* s1, const String&  s2)	{ return (s2.compare(s1) == 0); }

// ------------------------------------------------------------------------------------------
/*! Compares two strings for inequality.
	
	@return		@c true if @a s1 is not equal to @a s2
	@relates	String
*/
inline bool	operator != (const String&  s1, const String&  s2)	{ return (s1.compare(s2) != 0); }
/*! @overload
*/
inline bool	operator != (const String&  s1, CFStringRef    s2)	{ return (s1.compare(s2) != 0); }
/*! @overload
*/
inline bool	operator != (CFStringRef    s1, const String&  s2)	{ return (s2.compare(s1) != 0); }
/*! @overload
*/
inline bool	operator != (const String&  s1, const UniChar* s2)	{ return (s1.compare(s2) != 0); }
/*! @overload
*/
inline bool	operator != (const UniChar* s1, const String&  s2)	{ return (s2.compare(s1) != 0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \<.
	
	@return		@c true if @a s1 is less than @a s2
	@relates	String
*/
inline bool	operator <  (const String&  s1, const String&  s2)	{ return (s1.compare(s2) <  0); }
/*! @overload
*/
inline bool	operator <  (const String&  s1, CFStringRef    s2)	{ return (s1.compare(s2) <  0); }
/*! @overload
*/
inline bool	operator <  (CFStringRef    s1, const String&  s2)	{ return (s2.compare(s1) >= 0); }
/*! @overload
*/
inline bool	operator <  (const String&  s1, const UniChar* s2)	{ return (s1.compare(s2) <  0); }
/*! @overload
*/
inline bool	operator <  (const UniChar* s1, const String&  s2)	{ return (s2.compare(s1) >= 0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \<=.
	
	@return		@c true if @a s1 is less than or equal to @a s2
	@relates	String
*/
inline bool	operator <= (const String&  s1, const String&  s2)	{ return (s1.compare(s2) <= 0); }
/*! @overload
*/
inline bool	operator <= (const String&  s1, CFStringRef    s2)	{ return (s1.compare(s2) <= 0); }
/*! @overload
*/
inline bool	operator <= (CFStringRef    s1, const String&  s2)	{ return (s2.compare(s1) >  0); }
/*! @overload
*/
inline bool	operator <= (const String&  s1, const UniChar* s2)	{ return (s1.compare(s2) <= 0); }
/*! @overload
*/
inline bool	operator <= (const UniChar* s1, const String&  s2)	{ return (s2.compare(s1) >  0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \>.
	
	@return		@c true if @a s1 is greater than @a s2
	@relates	String
*/
inline bool	operator >  (const String&  s1, const String&  s2)	{ return (s1.compare(s2) >  0); }
/*! @overload
*/
inline bool	operator >  (const String&  s1, CFStringRef    s2)	{ return (s1.compare(s2) >  0); }
/*! @overload
*/
inline bool	operator >  (CFStringRef    s1, const String&  s2)	{ return (s2.compare(s1) <= 0); }
/*! @overload
*/
inline bool	operator >  (const String&  s1, const UniChar* s2)	{ return (s1.compare(s2) >  0); }
/*! @overload
*/
inline bool	operator >  (const UniChar* s1, const String&  s2)	{ return (s2.compare(s1) <= 0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \>=.
	
	@return		@c true if @a s1 is greater than or equal to @a s2
	@relates	String
*/
inline bool	operator >= (const String&  s1, const String&  s2)	{ return (s1.compare(s2) >= 0); }
/*! @overload
*/
inline bool	operator >= (const String&  s1, CFStringRef    s2)	{ return (s1.compare(s2) >= 0); }
/*! @overload
*/
inline bool	operator >= (CFStringRef    s1, const String&  s2)	{ return (s2.compare(s1) <  0); }
/*! @overload
*/
inline bool	operator >= (const String&  s1, const UniChar* s2)	{ return (s1.compare(s2) >= 0); }
/*! @overload
*/
inline bool	operator >= (const UniChar* s1, const String&  s2)	{ return (s2.compare(s1) <  0); }

//@}

//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a s1 and @a s2.

	@relates	String
*/
inline void	swap(String& s1, String& s2)	{ s1.swap(s2); }

//@}

//@}


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//	String::iterator Global Functions

inline String::iterator	operator +  (ptrdiff_t n, const String::iterator& it)				{ return (it + n); }
inline String::iterator	operator -  (ptrdiff_t n, const String::iterator& it)				{ return (it - n); }

inline ptrdiff_t	operator -  (const String::iterator& it1, const String::iterator& it2)	{ return (it1.distance(it2)); }
inline bool			operator == (const String::iterator& it1, const String::iterator& it2)	{ return (it1.compare(it2) == 0); }
inline bool			operator != (const String::iterator& it1, const String::iterator& it2)	{ return (it1.compare(it2) != 0); }
inline bool			operator <  (const String::iterator& it1, const String::iterator& it2)	{ return (it1.compare(it2) <  0); }
inline bool			operator <= (const String::iterator& it1, const String::iterator& it2)	{ return (it1.compare(it2) <= 0); }
inline bool			operator >  (const String::iterator& it1, const String::iterator& it2)	{ return (it1.compare(it2) >  0); }
inline bool			operator >= (const String::iterator& it1, const String::iterator& it2)	{ return (it1.compare(it2) >= 0); }

// ==========================================================================================
//	String::const_iterator Global Functions

inline String::const_iterator	operator +  (ptrdiff_t n, const String::const_iterator& it)				{ return (it + n); }
inline String::const_iterator	operator -  (ptrdiff_t n, const String::const_iterator& it)				{ return (it - n); }

inline ptrdiff_t	operator -  (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.distance(it2)); }
inline bool			operator == (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.compare(it2) == 0); }
inline bool			operator != (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.compare(it2) != 0); }
inline bool			operator <  (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.compare(it2) <  0); }
inline bool			operator <= (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.compare(it2) <= 0); }
inline bool			operator >  (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.compare(it2) >  0); }
inline bool			operator >= (const String::const_iterator& it1, const String::const_iterator& it2)	{ return (it1.compare(it2) >= 0); }

#endif	// DOXYGEN_SKIP

}	// namespace B

#endif	// BString_H_
