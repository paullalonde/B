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

#ifndef BMutableString_H_
#define BMutableString_H_

#pragma once

// standard headers
#include <iosfwd>
#include <cstdarg>
#include <string>
#include <vector>

// system headers
#include <CoreFoundation/CFString.h>

// library headers
#include <boost/concept_check.hpp>
#include <boost/utility.hpp>

// B headers
#include "BString.h"


namespace B {


// ==========================================================================================
//	MutableString

/*!
	@brief	Adapter class for changeable strings.
	
	MutableString provides a wrapper around a CoreFoundation @c CFMutableStringRef object, 
	giving it an interface similar (nearly identical in fact) to <tt>std::string</tt>.  This 
	makes it possible to use MutableString with STL algorithms, with standard streams, etc.
	
	Unlike String, MutableString does @b not share @c CFMutableStringRefs between its
	instances.  Each MutableString gets its own independent @c CFMutableStringRef.  The one 
	exception to this is the @c OSPtr<CFMutableStringRef> constructor, which can optionally 
	share its argument instead of copying it.
	
	Other notable differences with <tt>std::basic_string<...></tt>:
		
		- There are no template parameters, because CFStrings only have one underlying 
		  character type, namely @c UniChar.  One can think of MutableString as 
		  analogous to <tt>std::basic_string<UniChar,char_traits<UniChar>,CFAllocatorRef></tt>.
		  
		- There are many more conversions.  This reflects MutableString's role as an 
		  adapter between CoreFoundation and the Standard C++ Library.  For example, one 
		  may construct instances of MutableString with any of the following types:
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
		
		- Likewise, one may copy() a MutableString into one of the following data 
		  structures:
			- <tt>CFMutableStringRef</tt>
			- <tt>std::string</tt>
			- <tt>std::wstring</tt>
			- <tt>char*</tt>
			- <tt>wchar_t*</tt>
			- <tt>UniChar*</tt>
			- <tt>StringPtr</tt>
			- <tt>std::vector<UInt8></tt>
	
		- The functions that manipulate the string's underlying storage (capacity() and 
		  reserve()) do nothing, because there's no equivalent concept in CoreFoundation.
		  
		- The allocator type is fixed as @c CFAllocatorRef.  The allocator type is not a 
		  template parameter, however allocators @e are passed as arguments to 
		  constructors and can be retrieved via get_allocator().
		  
		- The underlying @c CFMutableStringRef's storage is not accessible, so it's not 
		  possible to take the address of an element of a MutableString.  MutableString 
		  is incompatible with the STL in this respect.
*/
class MutableString
{
public:
	
	//! @name Types
	//@{
#if 0
	//! The type of the character traits.	### UNIMPLEMENTED ###
	//typedef char_traits<XXX>		traits_type;
#endif
	typedef String::value_type				value_type;				//!< The type of the characters.
	typedef String::size_type				size_type;				//!< The unsigned integral type for size values and indices.
	typedef String::difference_type			difference_type;		//!< The signed integral type for difference values.
	typedef String::reference				reference;				//!< The signed integral type for difference values.
	typedef String::const_reference			const_reference;		//!< The type of constant character references.
	typedef String::pointer					pointer;				//!< The type of character pointers.
	typedef String::const_pointer			const_pointer;			//!< The type of character pointers.
	typedef String::iterator				iterator;				//!< The type of iterators.
	typedef String::const_iterator			const_iterator;			//!< The type of constant iterators.
	typedef String::reverse_iterator		reverse_iterator;		//!< The type of reverse iterators.
	typedef String::const_reverse_iterator	const_reverse_iterator;	//!< The type of constant reverse iterators.
	typedef String::allocator_type			allocator_type;			//!< The type of the allocator.
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
				MutableString();
	//! Copy constructor.
				MutableString(const MutableString& str);
	//! Allocator constructor.
	explicit	MutableString(CFAllocatorRef allocator);
	//! MutableString constructor.
	explicit	MutableString(const MutableString& str, size_type pos, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! String constructor.
	explicit	MutableString(const String& str);
	//! String constructor.
	explicit	MutableString(const String& str, size_type pos, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! @c CFStringRef constructor.
	explicit	MutableString(CFStringRef cfstr);
	//! @c CFStringRef constructor.
	explicit	MutableString(CFStringRef cfstr, size_type pos, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! @c UniChar array constructor.
	explicit	MutableString(const UniChar* ustr, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! @c UniChar constructor.
	explicit	MutableString(size_type n, UniChar c, CFAllocatorRef allocator = NULL);
	//! @c std::wstring constructor.
	explicit	MutableString(const std::wstring& wstr, size_type pos = 0, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! @c wchar_t array constructor.
	explicit	MutableString(const wchar_t* wcstr, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! @c std::string constructor.
	explicit	MutableString(const std::string& sstr, CFStringEncoding encoding = kCFStringEncodingASCII, size_type pos = 0, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! @c char* array constructor.
	explicit	MutableString(const char* cstr, CFStringEncoding encoding = kCFStringEncodingASCII, size_type n = String::npos, CFAllocatorRef allocator = NULL);
	//! Pascal string constructor.
	explicit	MutableString(ConstStringPtr pstr, CFStringEncoding encoding = kCFStringEncodingASCII, CFAllocatorRef allocator = NULL);
	//! Blob constructor.
	explicit	MutableString(const std::vector<UInt8>& blob, CFStringEncoding encoding, bool external, CFAllocatorRef allocator = NULL);
	//! @c OSPtr<CFStringRef> constructor.
	explicit	MutableString(const OSPtr<CFStringRef>& cfstr);
	//! @c OSPtr<CFMutableStringRef> constructor.
	explicit	MutableString(const OSPtr<CFMutableStringRef>& cfstr);
	//! Range constructor.
	template <class InputIterator>
	explicit	MutableString(InputIterator first, InputIterator last, CFAllocatorRef allocator = NULL);
	//! Destructor.
				~MutableString();
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
	//! Non-binding request to resize the internal storage to max(n, size()).
	void		reserve(size_type n = 0);
	//@}
	
	//! @name Comparisons
	//@{
	//! Compares the string to @a str.
	int	compare(const MutableString& str) const;
	//! Compares the string to @a str.
	int	compare(size_type pos1, size_type n1, const MutableString& str) const;
	//! Compares the string to @a str.
	int	compare(size_type pos1, size_type n1, const MutableString& str, size_type pos2, size_type n2) const;
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
	//! Returns a modifiable reference to the character at index @a pos.
	reference		operator [] (size_type pos);
	//! Returns the character at index @a pos.
	const_reference	at(size_type pos) const;
	//! Returns a modifiable reference to the character at index @a pos.
	reference		at(size_type pos);
	//@}
	
	/*! @name Generating CFMutableStringRefs and Character Arrays.
		
		Because of the fundamental differences in underlying storage between MutableString 
		and <tt>std::basic_string<></tt>, it's impossible to provide direct counterparts to 
		@a <tt>std::basic_string<>c_str()</tt> and <tt>std::basic_string<>data()</tt>.  
		@c cf_ref() is analogous to @c c_str().
		
		The various overloads of copy() allow conversions from MutableString to a number 
		of output types.
	*/
	//@{
	//! Returns the underlying @c CFStringRef.
	CFMutableStringRef	cf_ref() const;
	//! Returns a "smart pointer" to the underlying @c CFStringRef.
	OSPtr<CFMutableStringRef>	cf_ptr() const;
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
		
		The various overloads of @c operator = and @c assign() replace the 
		string's contents with the given input string.  However, the string's 
		@c CFMutableStringRef does @b not change.
	*/
	//@{
	//! MutableString assignemnt.
	MutableString&	operator = (const MutableString& str);
	//! String assignemnt.
	MutableString&	operator = (const String& str);
	//! @c CFStringRef assignemnt.
	MutableString&	operator = (CFStringRef cfstr);
	//! @c OSPtr<CFStringRef> assignemnt.
	MutableString&	operator = (const OSPtr<CFStringRef>& cfstr);
	//! @c OSPtr<CFMutableStringRef> assignemnt.
	MutableString&	operator = (const OSPtr<CFMutableStringRef>& cfstr);
	//! Null-terminated @c UniChar array assignemnt.
	MutableString&	operator = (const UniChar* ustr);
	//! @c UniChar assignemnt.
	MutableString&	operator = (UniChar c);
	//! @c std::wstring assignemnt.
	MutableString&	operator = (const std::wstring& wstr);
	//! Null-terminated @c wchar_t array assignemnt.
	MutableString&	operator = (const wchar_t* ustr);
	//! @c std::string assignemnt.
	MutableString&	operator = (const std::string& sstr);
	//! Null-terminated @c char array assignemnt.
	MutableString&	operator = (const char* cstr);
	//! Pascal string assignemnt.
	MutableString&	operator = (ConstStringPtr pstr);
	//! MutableString assignemnt.
	MutableString&	assign(const MutableString& str, size_type pos = 0, size_type n = String::npos);
	//! String assignemnt.
	MutableString&	assign(const String& str, size_type pos = 0, size_type n = String::npos);
	//! @c CFStringRef assignemnt.
	MutableString&	assign(CFStringRef cfstr, size_type pos = 0, size_type n = String::npos);
	//! @c UniChar array assignemnt.
	MutableString&	assign(const UniChar* ustr, size_type n = String::npos);
	//! @c UniChar assignemnt.
	MutableString&	assign(size_type n, UniChar c);
	//! @c std::wstring assignemnt.
	MutableString&	assign(const std::wstring& wstr, size_type pos = 0, size_type n = String::npos);
	//! @c wchar_t array assignemnt.
	MutableString&	assign(const wchar_t* wcstr, size_type n = String::npos);
	//! @c std::string assignemnt.
	MutableString&	assign(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0, size_type n = String::npos);
	//! @c char array assignemnt.
	MutableString&	assign(const char* cstr, CFStringEncoding encoding, size_type n = String::npos);
	//! Pascal string assignemnt.
	MutableString&	assign(ConstStringPtr pstr, CFStringEncoding encoding);
	//! Blob assignemnt.
	MutableString&	assign(const std::vector<UInt8>& blob, CFStringEncoding encoding, bool external);
	//! Range assignment.
	template <class InputIterator>
	MutableString&	assign(InputIterator first, InputIterator last);
	//! Exchanges the contents of the string with @a str.
	void			swap(MutableString& str);
	//@}
	
	//! @name Appending Characters
	//@{
	//! Appends a MutableString.
	MutableString&	operator += (const MutableString& str);
	//! Appends a String.
	MutableString&	operator += (const String& str);
	//! Appends a @c CFStringRef.
	MutableString&	operator += (CFStringRef cfstr);
	//! Appends a null-terminated @c UniChar array.
	MutableString&	operator += (const UniChar* ustr);
	//! Appends a @c UniChar.
	MutableString&	operator += (UniChar c);
	//! Appends a @c std::string.
	MutableString&	operator += (const std::string& sstr);
	//! Appends a MutableString.
	MutableString&	append(const MutableString& str, size_type pos = 0, size_type n = String::npos);
	//! Appends a String.
	MutableString&	append(const String& str, size_type pos = 0, size_type n = String::npos);
	//! Appends a @c CFStringRef.
	MutableString&	append(CFStringRef cfstr, size_type pos = 0, size_type n = String::npos);
	//! Appends a @c UniChar array.
	MutableString&	append(const UniChar* ustr, size_type n = String::npos);
	//! Appends @c UniChars.
	MutableString&	append(size_type n, UniChar c);
	//! Appends a range.
	template <class InputIterator>
	MutableString&	append(InputIterator first, InputIterator last);
	//! Appends a single character.
	void			push_back(UniChar c);
	//@}
	
	//! @name Inserting Characters
	//@{
	//! Inserts a MutableString.
	MutableString&	insert(size_type pos1, const MutableString& str, size_type pos2 = 0, size_type n = String::npos);
	//! Inserts a String.
	MutableString&	insert(size_type pos1, const String& str, size_type pos2 = 0, size_type n = String::npos);
	//! Inserts a @c CFStringRef.
	MutableString&	insert(size_type pos1, CFStringRef cfstr, size_type pos2 = 0, size_type n = String::npos);
	//! Inserts a @c UniChar array.
	MutableString&	insert(size_type pos, const UniChar* ustr, size_type n = String::npos);
	//! Inserts @c UniChars.
	MutableString&	insert(size_type pos, size_type n, UniChar c);
	//! Inserts a @c UniChar.
	iterator		insert(iterator it, UniChar c);
	//! Inserts @c UniChars.
	void			insert(iterator it, size_t n, UniChar c);
	//! Inserts a range.
	template<class InputIterator>
	void			insert(iterator it, InputIterator first, InputIterator last);
	//@}
	
	//! @name Erasing Characters
	//@{
	//! Removes all characters.
	MutableString&	erase();
	//! Removes a range of characters.
	MutableString&	erase(size_type pos, size_type n = String::npos);
	//! Removes one character.
	iterator		erase(iterator it);
	//! Removes a range of characters.
	iterator		erase(iterator first, iterator last);
	//! Removes all characters.
	void			clear();
	//@}
	
	//! @name Changing the Size
	//@{
	//! Resizes the string, inserting '\\0' characters as necessary.
	void	resize(size_type n);
	//! Resizes the string, inserting @a c characters as necessary.
	void	resize(size_type n, UniChar c);
	//@}
	
	//! @name Replacing Characters
	//@{
	//! Replaces characters with a MutableString.
	MutableString&	replace(size_type pos1, size_type n1, const MutableString& str);
	//! Replaces characters with a MutableString.
	MutableString&	replace(size_type pos1, size_type n1, const MutableString& str, size_type pos2, size_type n2);
	//! Replaces characters with a String.
	MutableString&	replace(size_type pos1, size_type n1, const String& str);
	//! Replaces characters with a String.
	MutableString&	replace(size_type pos1, size_type n1, const String& str, size_type pos2, size_type n2);
	//! Replaces characters with a @c CFStringRef.
	MutableString&	replace(size_type pos1, size_type n1, CFStringRef cfstr);
	//! Replaces characters with a @c CFStringRef.
	MutableString&	replace(size_type pos1, size_type n1, CFStringRef cfstr, size_type pos2, size_type n2);
	//! Replaces characters with a @c UniChar array.
	MutableString&	replace(size_type pos1, size_type n1, const UniChar* ustr, size_type n2 = String::npos);
	//! Replaces characters with @c UniChars.
	MutableString&	replace(size_type pos1, size_type n1, size_type n, UniChar c);
	//! Replaces characters with a MutableString.
	MutableString&	replace(iterator it1, iterator it2, const MutableString& str);
	//! Replaces characters with a String.
	MutableString&	replace(iterator it1, iterator it2, const String& str);
	//! Replaces characters with a @c CFStringRef.
	MutableString&	replace(iterator it1, iterator it2, CFStringRef cfstr);
	//! Replaces characters with a @c UniChar array.
	MutableString&	replace(iterator it1, iterator it2, const UniChar* ustr, size_type n2 = String::npos);
	//! Replaces characters with @c UniChars.
	MutableString&	replace(iterator it1, iterator it2, size_type n, UniChar c);
	//! Replaces characters with a range.
	template <class InputIterator>
	MutableString&	replace(iterator it1, iterator it2, InputIterator first, InputIterator last);
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
	//! Finds the first MutableString.
	size_type	find(const MutableString& str, size_type pos = 0) const;
	//! Finds the last MutableString.
	size_type	rfind(const MutableString& str, size_type pos = 0) const;
	//! Finds the first @c CFStringRef.
	size_type	find(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the last @c CFStringRef.
	size_type	rfind(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the first @c UniChar array.
	size_type	find(const UniChar* ustr, size_type pos = 0, size_type n = String::npos) const;
	//! Finds the last @c UniChar array.
	size_type	rfind(const UniChar* ustr, size_type pos = 0, size_type n = String::npos) const;
	//! Finds the first @c std::string.
	size_type	find(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//! Finds the last @c std::string.
	size_type	rfind(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//@}
	
	//! @name Finding the First of a Set of Characters
	//@{
	//! Finds the first character in a set given as a MutableString.
	size_type	find_first_of(const MutableString& str, size_type pos = 0) const;
	//! Finds the first character not in a set given as a MutableString.
	size_type	find_first_not_of(const MutableString& str, size_type pos = 0) const;
	//! Finds the first character in a set given as a @c CFStringRef.
	size_type	find_first_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the first character not in a set given as a @c CFStringRef.
	size_type	find_first_not_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the first character in a set given as a @c UniChar array.
	size_type	find_first_of(const UniChar* ustr, size_type pos = 0, size_type n = String::npos) const;
	//! Finds the first character not in a set given as a @c UniChar array.
	size_type	find_first_not_of(const UniChar* ustr, size_type pos = 0, size_type n = String::npos) const;
	//! Finds the first character in a set given as a @c std::string.
	size_type	find_first_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//! Finds the first character not in a set given as a @c std::string.
	size_type	find_first_not_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//@}
	
	//! @name Finding the Last of a Set of Characters
	//@{
	//! Finds the last character in a set given as a MutableString.
	size_type	find_last_of(const MutableString& str, size_type pos = 0) const;
	//! Finds the last character not in a set given as a MutableString.
	size_type	find_last_not_of(const MutableString& str, size_type pos = 0) const;
	//! Finds the last character in a set given as a @c CFStringRef.
	size_type	find_last_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the last character not in a set given as a @c CFStringRef.
	size_type	find_last_not_of(CFStringRef cfstr, size_type pos = 0) const;
	//! Finds the last character in a set given as a @c UniChar array.
	size_type	find_last_of(const UniChar* ustr, size_type pos = 0, size_type n = String::npos) const;
	//! Finds the last character not in a set given as a @c UniChar array.
	size_type	find_last_not_of(const UniChar* ustr, size_type pos = 0, size_type n = String::npos) const;
	//! Finds the last character in a set given as a @c std::string.
	size_type	find_last_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//! Finds the last character not in a set given as a @c std::string.
	size_type	find_last_not_of(const std::string& sstr, CFStringEncoding encoding, size_type pos = 0) const;
	//@}
	
	//! @name Substrings
	//@{
	//! Returns a substring.
	MutableString	substr(size_type pos = 0, size_type n = String::npos) const;
	//@}
	
	//! @name Generating Iterators
	//@{
	//! Returns a constant random access iterator for the beginning of the string.
	const_iterator			begin() const;
	//! Returns a random access iterator for the beginning of the string.
	iterator				begin();
	//! Returns a constant random access iterator for the end of the string.
	const_iterator			end() const;
	//! Returns a random access iterator for the end of the string.
	iterator				end();
	//! Returns a constant random access iterator for the beginning of a reverse iteration of the string.
	const_reverse_iterator	rbegin() const;
	//! Returns a random access iterator for the beginning of a reverse iteration of the string.
	reverse_iterator		rbegin();
	//! Returns a constant random access iterator for the end of a reverse iteration of the string.
	const_reverse_iterator	rend() const;
	//! Returns a random access iterator for the end of a reverse iteration of the string.
	reverse_iterator		rend();
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
	
	void	init_str(CFStringRef srcRef, CFAllocatorRef allocator);
	
	// member variables
	CFMutableStringRef	mRef;
};

// ------------------------------------------------------------------------------------------
/*!	Creates an empty string using the default allocator.
*/
inline
MutableString::MutableString()
{
	init_str(NULL, NULL);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a str, using @a str's allocator.
*/
inline
MutableString::MutableString(
	const MutableString&	str)	//!< The source string.
{
	init_str(str.cf_ref(), str.get_allocator());
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a str, using @a str's allocator.
*/
inline
MutableString::MutableString(
	const String&	str)	//!< The source string.
{
	init_str(str.cf_ref(), str.get_allocator());
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator.
*/
inline
MutableString::MutableString(
	CFStringRef	cfstr)	//!< The source string.
{
	B_ASSERT(cfstr != NULL);
	
	init_str(cfstr, CFGetAllocator(cfstr));
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by all character of the range [@a first, @a last).
*/
template <class InputIterator>
MutableString::MutableString(
	InputIterator	first,					//!< The start of the range of characters for the new string.
	InputIterator	last,					//!< The end of the range of characters for the new string.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(CFSTR(""))
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	String	temp(first, last);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
inline
MutableString::~MutableString()
{
	CFRelease(mRef);
}

// ------------------------------------------------------------------------------------------
inline MutableString::size_type
MutableString::size() const
{
	return (CFStringGetLength(mRef));
}

// ------------------------------------------------------------------------------------------
inline MutableString::size_type
MutableString::length() const
{
	return (CFStringGetLength(mRef));
}

// ------------------------------------------------------------------------------------------
inline bool
MutableString::empty() const
{
	return (CFStringGetLength(mRef) == 0);
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the string @a cfstr.
	
	@return		< 0 if @c *this < @a cfstr;  0 if @c *this == @a cfstr;  > 0 if @c *this > @a cfstr.
*/
inline int
MutableString::compare(
	CFStringRef	cfstr)	//!< The string to compare against.
	const
{
	return (String::private_compare(mRef, cfstr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with string @a cfstr.
	
	@return		< 0 if @c *this < @a cfstr;  0 if @c *this == @a cfstr;  > 0 if @c *this > @a cfstr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
MutableString::compare(
	size_type	pos1,	//!< The index of the first character to compare in @c *this.
	size_type	n1,		//!< The number of characters to compare in @c *this.
	CFStringRef	cfstr)	//!< The string to compare against.
	const
{
	return (String::private_compare(mRef, pos1, n1, cfstr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with, at most, @a n2 characters of string @a cfstr starting with index @a pos2.
	
	@return		< 0 if @c *this < @a cfstr;  0 if @c *this == @a cfstr;  > 0 if @c *this > @a cfstr.
	@exception	std::out_of_range	If @a pos1 > @c size() or @a pos2 > @c CFStringGetLength( @a cfstr ).
*/
inline int
MutableString::compare(
	size_type	pos1,	//!< The index of the first character to compare in @c *this.
	size_type	n1,		//!< The number of characters to compare in @c *this.
	CFStringRef	cfstr,	//!< The string to compare against.
	size_type	pos2,	//!< The index of the first character to compare in @a cfstr.
	size_type	n2)		//!< The number of characters to compare in @a cfstr.
	const
{
	return (String::private_compare(mRef, pos1, n1, cfstr, pos2, n2));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the string @a str.

	@return		< 0 if @c *this < @a str;  0 if @c *this == @a str;  > 0 if @c *this > @a str.
*/
inline int
MutableString::compare(
	const MutableString&	str)	//!< The string to compare against.
	const
{
	return (String::private_compare(mRef, str.cf_ref()));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with string @a str.

	@return		< 0 if @c *this < @a str;  0 if @c *this == @a str;  > 0 if @c *this > @a str.
*/
inline int
MutableString::compare(
	size_type				pos1,	//!< The index of the first character to compare in @c *this.
	size_type				n1,		//!< The number of characters to compare in @c *this.
	const MutableString&	str)	//!< The string to compare against.
	const
{
	return (String::private_compare(mRef, pos1, n1, str.cf_ref()));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with, at most, @a n2 characters of string @a str starting with index @a pos2.
	
	@return		< 0 if @c *this < @a str;  0 if @c *this == @a str;  > 0 if @c *this > @a str.
	@exception	std::out_of_range	If @a pos1 > @c size() or @a pos2 > @a str.size().
*/
inline int
MutableString::compare(
	size_type				pos1,	//!< The index of the first character to compare in @c *this.
	size_type				n1,		//!< The number of characters to compare in @c *this.
	const MutableString&	str,	//!< The string to compare against.
	size_type				pos2,	//!< The index of the first character to compare in @a str.
	size_type				n2)		//!< The number of characters to compare in @a str.
	const
{
	return (String::private_compare(mRef, pos1, n1, str.cf_ref(), pos2, n2));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the null-terminated @c UniChar array @a ustr.
	
	@return		< 0 if @c *this < @a ustr;  0 if @c *this == @a ustr;  > 0 if @c *this > @a ustr.
*/
inline int
MutableString::compare(
	const UniChar*	ustr)	//!< The string to compare against.
	const
{
	return (String::private_compare(mRef, ustr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with the null-terminated @c UniChar array @a ustr.
	
	@return		< 0 if @c *this < @a ustr;  0 if @c *this == @a ustr;  > 0 if @c *this > @a ustr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
MutableString::compare(
	size_type		pos1,	//!< The index of the first character to compare in @c *this.
	size_type		n1,		//!< The number of characters to compare in @c *this.
	const UniChar*	ustr)	//!< The string to compare against.
	const
{
	return (String::private_compare(mRef, pos1, n1, ustr));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with @a n2 characters of @c UniChar array @a ustr.
	
	@return		< 0 if @c *this < @a ustr;  0 if @c *this == @a ustr;  > 0 if @c *this > @a ustr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
MutableString::compare(
	size_type		pos1,	//!< The index of the first character to compare in @c *this.
	size_type		n1,		//!< The number of characters to compare in @c *this.
	const UniChar*	ustr,	//!< The string to compare against.
	size_type		n2)		//!< The number of characters to compare in @a ustr.
	const
{
	return (String::private_compare(mRef, pos1, n1, ustr, n2));
}

// ------------------------------------------------------------------------------------------
/*!	Compares the string @c *this with the string @a sstr, which is assumed 
	to be encoded using the character encoding given in @a encoding.
	
	@return		< 0 if @c *this < @a sstr;  0 if @c *this == @a sstr;  > 0 if @c *this > @a sstr.
*/
inline int
MutableString::compare(
	const std::string&	sstr,		//!< The string to compare against.
	CFStringEncoding	encoding)	//!< The character encoding of @a sstr.
	const
{
	return (String::private_compare(mRef, sstr, encoding));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with the string @a sstr, which is assumed to be encoded using the character encoding 
	given in @a encoding.
	
	@return		< 0 if @c *this < @a sstr;  0 if @c *this == @a sstr;  > 0 if @c *this > @a sstr.
	@exception	std::out_of_range	If @a pos1 > @c size().
*/
inline int
MutableString::compare(
	size_type			pos1,		//!< The index of the first character to compare in @c *this.
	size_type			n1,			//!< The number of characters to compare in @c *this.
	const std::string&	sstr,		//!< The string to compare against.
	CFStringEncoding	encoding)	//!< The character encoding of @a sstr.
	const
{
	return (String::private_compare(mRef, pos1, n1, sstr, encoding));
}

// ------------------------------------------------------------------------------------------
/*!	Compares, at most, @a n1 characters of string @c *this, starting with index @a pos1 
	with, at most, @a n2 characters of string @a sstr starting with index @a pos2.  @a Sstr 
	is assumed to be encoded using the character encoding given in @a encoding.
	
	@return		< 0 if @c *this < @a sstr;  0 if @c *this == @a sstr;  > 0 if @c *this > @a sstr.
	@exception	std::out_of_range	If @a pos1 > @c size() or @a pos2 > @a sstr.size().
*/
inline int
MutableString::compare(
	size_type			pos1,		//!< The index of the first character to compare in @c *this.
	size_type			n1,			//!< The number of characters to compare in @c *this.
	const std::string&	sstr,		//!< The string to compare against.
	CFStringEncoding	encoding,	//!< The character encoding of @a sstr.
	size_type			pos2,		//!< The index of the first character to compare in @a str.
	size_type			n2)			//!< The number of characters to compare in @a sstr.
	const
{
	return (String::private_compare(mRef, pos1, n1, sstr, encoding, pos2, n2));
}

// ------------------------------------------------------------------------------------------
/*!	If @a pos equals @c length(), the function returns '\\0'.  If @a pos is smaller than 
	zero or greater than @c length(), the results are undefined.
*/
inline MutableString::const_reference
MutableString::operator [] (
	size_type	pos)	//!< The index of the character to return.
	const
{
	if (pos == length())
		return ('\0');
	else
		return (CFStringGetCharacterAtIndex(mRef, pos));
}

// ------------------------------------------------------------------------------------------
/*!	If @a pos is smaller than zero or greater than or equal to @c length(), the results are 
	undefined.
*/
inline MutableString::reference
MutableString::operator [] (size_type pos)
{
	return (reference(mRef, pos));
}

// ------------------------------------------------------------------------------------------
inline CFMutableStringRef
MutableString::cf_ref() const
{
	return (mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Copies at most @a n characters, starting at index @a pos in @c *this, into the buffer 
	pointed to by @a ustr.  No null character is appended.
	
	@return	The number of copied characters.
*/
inline MutableString::size_type
MutableString::copy(
	UniChar*	ustr,			//!< The output buffer.
	size_type	n,				//!< The maximum number of characters to copy.
	size_type	pos /* = 0 */)	//!< The starting index (in @c *this) for conversion.
	const
{
	return (String::private_copy(mRef, ustr, n, pos));
}

// ------------------------------------------------------------------------------------------
/*!	Copies at most @a n characters, starting at index @a pos in @c *this, into the buffer 
	pointed to by @a wcstr.  The string's characters are converted to @c wchar_t upon 
	copying.  No null character is appended.
	
	@return	The number of copied characters.
*/
inline MutableString::size_type
MutableString::copy(
	wchar_t*	wcstr,			//!< The output buffer.
	size_type	n,				//!< The maximum number of characters to copy.
	size_type	pos /* = 0 */)	//!< The starting index (in @c *this) for conversion.
	const
{
	return (String::private_copy(mRef, wcstr, n, pos));
}

// ------------------------------------------------------------------------------------------
/*!	Copies at most @a n characters, starting at index @a pos in @c *this, into the buffer 
	pointed to by @a wcstr.  The string's characters are converted to @a encoding upon 
	copying.  No null character is appended.
	
	@return	The number of copied characters.
*/
inline MutableString::size_type
MutableString::copy(
	char*				cstr,		//!< The output buffer.
	size_type			n,			//!< The maximum number of characters to copy.
	size_type&			pos,		//!< The starting index (in @c *this) for conversion.  On output, holds the index of the next character to convert, or @c npos if conversion is complete.
	CFStringEncoding	encoding)	//!< The output buffer's encoding.
	const
{
	return (String::private_copy(mRef, cstr, n, pos, encoding));
}

// ------------------------------------------------------------------------------------------
/*!	The current contents of @a cfstr are completely replaced.  If @c cfstr doesn't have 
	enough capacity to hold the contents, the results are undefined.
*/
inline void
MutableString::copy(
	CFMutableStringRef	cfstr)	//!< The output string.
	const
{
	CFStringReplaceAll(cfstr, cf_ref());
}

// ------------------------------------------------------------------------------------------
/*!	The string's characters are converted to @c wchar_t upon copying.
*/
inline void
MutableString::copy(
	std::wstring&	wstr)	//!< The output string.
	const
{
	String::private_copy(mRef, wstr);
}

// ------------------------------------------------------------------------------------------
/*!	The string's characters are converted to @a encoding upon copying.
*/
inline void
MutableString::copy(
	std::string&		sstr,		//!< The output string.
	CFStringEncoding	encoding)	//!< The output string's encoding.
	const
{
	String::private_copy(mRef, sstr, encoding);
}

// ------------------------------------------------------------------------------------------
/*!	The string's characters are converted to @a encoding upon copying.
*/
inline void
MutableString::copy(
	StringPtr			pstr,		//!< The output string.
	size_type			n,			//!< The maximum size of the output string, including length byte.
	CFStringEncoding	encoding)	//!< The output string's encoding.
	const
{
	String::private_copy(mRef, pstr, n, encoding);
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
MutableString::copy(
	std::vector<UInt8>&	blob,					//!< The output buffer.
	CFStringEncoding	encoding,				//!< The output string's encoding.
	bool				external,				//!< If @c true, blob get a leading BOM.
	char				loss_byte /* = 0 */)	//!< The character to substitute for unconvertible characters.
	const
{
	String::private_copy(mRef, blob, encoding, external, loss_byte);
}

// ------------------------------------------------------------------------------------------
inline void
MutableString::swap(MutableString& str)
{
	std::swap(mRef, str.mRef);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator = (
	const MutableString&	str)	//!< The input string.
{
	CFStringReplaceAll(mRef, str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator = (
	const String&	str)	//!< The input string.
{
	CFStringReplaceAll(mRef, str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator = (
	CFStringRef	cfstr)	//!< The input string.
{
	B_ASSERT(cfstr != NULL);
	
	CFStringReplaceAll(mRef, cfstr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns the string with all characters of the range [@a first, @a last).
*/
template <class InputIterator> MutableString&
MutableString::assign(
	InputIterator	first,	//!< The start of the range of characters to assign.
	InputIterator	last)	//!< The end of the range of characters to assign.
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	String	temp(first, last);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator += (
	const MutableString&	str)	//!< The input string.
{
	CFStringAppend(mRef, str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator += (
	const String&	str)	//!< The input string.
{
	CFStringAppend(mRef, str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator += (
	CFStringRef	cfstr)	//!< The input string.
{
	CFStringAppend(mRef, cfstr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::operator += (
	const UniChar* ustr)	//!< The input string.
{
	CFStringAppendCharacters(mRef, ustr, String::uni_strlen(ustr));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Appends, at most, @a n characters of @a ustr starting with index @a pos.  If @a n is 
	@c npos, then @a ustr is assumed to be null-terminated.
*/
inline MutableString&
MutableString::append(
	const UniChar*	ustr,			//!< The input string.
	size_type		n /* = npos */)	//!< The number of characters to assign.
{
	CFStringAppendCharacters(mRef, ustr, (n == String::npos) ? String::uni_strlen(ustr) : n);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Appends all characters of the range [@a first, @a last).
*/
template <class InputIterator> MutableString&
MutableString::append(
	InputIterator	first,	//!< The start of the range of characters to append.
	InputIterator	last)	//!< The end of the range of characters to append.
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	String	temp(first, last);
	
	CFStringAppend(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
MutableString::push_back(UniChar c)
{
	CFStringAppendCharacters(mRef, &c, 1);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts all characters of the range [@a first, @a last) before the character to which 
	iterator @a it refers.
*/
template <class InputIterator> void
MutableString::insert(
	iterator		it,		//!< The location at which to start inserting characters.
	InputIterator	first,	//!< The start of the range of characters to insert.
	InputIterator	last)	//!< The end of the range of characters to insert.
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	String	temp(first, last);
	
	insert(it, temp.cf_ref());
}

// ------------------------------------------------------------------------------------------
inline MutableString&
MutableString::erase()
{
	CFStringReplaceAll(mRef, CFSTR(""));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
MutableString::clear()
{
	CFStringReplaceAll(mRef, CFSTR(""));
}

// ------------------------------------------------------------------------------------------
/*!	Changes the number of characters in @c *this to @a n.  Thus, if @a n is not equal to 
	@a size(), the function appends or removes characters at the end according to the 
	new size.
*/
inline void
MutableString::resize(
	size_type	n)	//!< The string's new size.
{
	resize(n, 0);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	all characters of @a str.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size().
*/
inline MutableString&
MutableString::replace(
	size_type				pos1,	//!< The index of the first character to replace.
	size_type				n1,		//!< The number of characters to replace.
	const MutableString&	str)	//!< The input string.
{
	CFStringReplace(mRef, 
					CFRangeMake(pos1, (n1 == String::npos) ? size()-pos1 : n1), 
					str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	all characters of @a str.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size().
*/
inline MutableString&
MutableString::replace(
	size_type		pos1,	//!< The index of the first character to replace.
	size_type		n1,		//!< The number of characters to replace.
	const String&	str)	//!< The input string.
{
	CFStringReplace(mRef, 
					CFRangeMake(pos1, (n1 == String::npos) ? size()-pos1 : n1), 
					str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	all characters of @a cfstr.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size().
*/
inline MutableString&
MutableString::replace(
	size_type		pos1,	//!< The index of the first character to replace.
	size_type		n1,		//!< The number of characters to replace.
	CFStringRef		cfstr)	//!< The input string.
{
	CFStringReplace(mRef, 
					CFRangeMake(pos1, (n1 == String::npos) ? size()-pos1 : n1), 
					cfstr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces all characters of the range [@a first, @a last) with all characters of 
	@a cfstr.
*/
inline MutableString&
MutableString::replace(
	iterator				it1,	//!< The start of the range of characters to replace.
	iterator				it2,	//!< The end of the range of characters to replace.
	const MutableString&	str)	//!< The input string.
{
	CFStringReplace(mRef, CFRangeMake(it1.mIndex, it2.mIndex - it1.mIndex), str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces all characters of the range [@a first, @a last) with all characters of 
	@a cfstr.
*/
inline MutableString&
MutableString::replace(
	iterator		it1,	//!< The start of the range of characters to replace.
	iterator		it2,	//!< The end of the range of characters to replace.
	const String&	str)	//!< The input string.
{
	CFStringReplace(mRef, CFRangeMake(it1.mIndex, it2.mIndex - it1.mIndex), str.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces all characters of the range [@a first, @a last) with all characters of 
	@a cfstr.
*/
inline MutableString&
MutableString::replace(
	iterator		it1,	//!< The start of the range of characters to replace.
	iterator		it2,	//!< The end of the range of characters to replace.
	CFStringRef		cfstr)	//!< The input string.
{
	CFStringReplace(mRef, CFRangeMake(it1.mIndex, it2.mIndex - it1.mIndex), cfstr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Replaces all characters of the range [@a first, @a last) with all characters of 
	the range [@a it1, @a it2).
*/
template <class InputIterator> MutableString&
MutableString::replace(
	iterator		it1,	//!< The start of the range of characters to erase.
	iterator		it2,	//!< The end of the range of characters to erase.
	InputIterator	first,	//!< The start of the range of characters to insert.
	InputIterator	last)	//!< The end of the range of characters to insert.
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	String	temp(first, last);
	
	return (replace(it1, it2, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the character @a c, starting at index @a pos.
	
	@return	The index of @a c, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find(
	UniChar		c,				//!< The character to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, c, pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the character @a c, starting at index @a pos.
	
	@return	The index of @a c, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::rfind(
	UniChar		c,				//!< The character to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, c, pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the string @a str, starting at index @a pos.
	
	@return	The index of @a str, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find(
	const MutableString&	str,			//!< The string to look for.
	size_type				pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, str.cf_ref(), pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the string @a str, starting at index @a pos.
	
	@return	The index of @a str, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::rfind(
	const MutableString&	str,			//!< The string to look for.
	size_type				pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, str.cf_ref(), pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the string @a cfstr, starting at index @a pos.
	
	@return	The index of @a cfstr, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find(
	CFStringRef	cfstr,			//!< The string to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, cfstr, pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the string @a cfstr, starting at index @a pos.
	
	@return	The index of @a cfstr, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::rfind(
	CFStringRef	cfstr,			//!< The string to look for.
	size_type	pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, cfstr, pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the @c UniChar array @a ustr of length @a n, 
	starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find(
	const UniChar*	ustr,			//!< The string to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (String::private_find(mRef, ustr, pos, n, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the @c UniChar array @a ustr of length @a n, 
	starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::rfind(
	const UniChar*	ustr,			//!< The string to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (String::private_find(mRef, ustr, pos, n, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of the string @a sstr, starting at index @a pos.  
	The input string is assumed to be encoded using the character encoding given in 
	@a encoding.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, sstr, encoding, pos, 0));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of the string @a sstr, starting at index @a pos.
	The input string is assumed to be encoded using the character encoding given in 
	@a encoding.
	
	@return	The index of @a sstr, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::rfind(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find(mRef, sstr, encoding, pos, kCFCompareBackwards));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_of(
	const MutableString&	str,			//!< The set of characters to look for.
	size_type				pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, str.cf_ref(), pos, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_not_of(
	const MutableString&	str,			//!< The set of characters to look for.
	size_type				pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, str.cf_ref(), pos, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, cfstr, pos, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_not_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, cfstr, pos, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (String::private_find_of(mRef, ustr, pos, n, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_not_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (String::private_find_of(mRef, ustr, pos, n, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character contained in @a sstr, starting at 
	index @a pos.  The input string is assumed to be encoded using the character encoding 
	given in @a encoding.

	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, sstr, encoding, pos, 0, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the first occurrence of a character @b not contained in @a sstr, 
	starting at index @a pos.  The input string is assumed to be encoded using the 
	character encoding given in @a encoding.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_first_not_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, sstr, encoding, pos, 0, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_of(
	const MutableString&	str,			//!< The set of characters to look for.
	size_type				pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, str.cf_ref(), pos, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a str, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_not_of(
	const MutableString&	str,			//!< The set of characters to look for.
	size_type				pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, str.cf_ref(), pos, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, cfstr, pos, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a cfstr, 
	starting at index @a pos.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_not_of(
	CFStringRef		cfstr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, cfstr, pos, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (String::private_find_of(mRef, ustr, pos, n, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a ustr of length 
	@a n, starting at index @a pos.  If @a n equals @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_not_of(
	const UniChar*	ustr,			//!< The set of characters to look for.
	size_type		pos /* = 0 */,	//!< The starting position for the search.
	size_type		n /* = npos */)	//!< The number of characters in @a ustr.
	const
{
	return (String::private_find_of(mRef, ustr, pos, n, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character contained in @a sstr, starting at 
	index @a pos.  The input string is assumed to be encoded using the character encoding 
	given in @a encoding.

	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, sstr, encoding, pos, kCFCompareBackwards, false));
}

// ------------------------------------------------------------------------------------------
/*!	Searches for the last occurrence of a character @b not contained in @a sstr, 
	starting at index @a pos.  The input string is assumed to be encoded using the 
	character encoding given in @a encoding.
	
	@return	The index of the found character, or @c npos if not found.
*/
inline MutableString::size_type
MutableString::find_last_not_of(
	const std::string&	sstr,			//!< The string to look for.
	CFStringEncoding	encoding,		//!< The character encoding of @a blob.
	size_type			pos /* = 0 */)	//!< The starting position for the search.
	const
{
	return (String::private_find_of(mRef, sstr, encoding, pos, kCFCompareBackwards, true));
}

// ------------------------------------------------------------------------------------------
/*!	Returns a substring of, at most, @a n characters of the string @c *this starting 
	with index @a pos.
*/
inline MutableString
MutableString::substr(
	size_type	pos /* = 0 */,	//!< The starting position for the substring.
	size_type	n /* = npos */)	//!< The length of the substring.
	const
{
	return (MutableString(cf_ref(), pos, n, get_allocator()));
}

// ------------------------------------------------------------------------------------------
inline MutableString::allocator_type
MutableString::get_allocator() const
{
	return (CFGetAllocator(mRef));
}


// ==========================================================================================
//	MutableString Global Functions

/*!	@defgroup	MutableStringFunctions	B::MutableString Global Functions
*/
//@{

//! @name String Concatenation
//@{

// ------------------------------------------------------------------------------------------
/*!	Returns a new string consisting of the concatenation of @a s1 and @a s2.
	
	@relates	MutableString
*/
inline MutableString	operator + (const MutableString& s1, const MutableString&  s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const MutableString& s1, const String&         s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const String&        s1, const MutableString&  s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const MutableString& s1, CFStringRef           s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (CFStringRef          s1, const MutableString&  s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const MutableString& s1, const UniChar*        s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const UniChar*       s1, const MutableString&  s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const MutableString& s1, UniChar                c)	{ return (MutableString(s1) += c); }
/*! @overload
*/
inline MutableString	operator + (UniChar               c, const MutableString&  s2)	{ return (MutableString(1, c) += s2); }
/*! @overload
*/
inline MutableString	operator + (const MutableString&  s1, const std::string&   s2)	{ return (MutableString(s1) += s2); }
/*! @overload
*/
inline MutableString	operator + (const std::string&    s1, const MutableString& s2)	{ return (MutableString(s1) += s2); }
//@}


/*! @name Input/Output Functions
	
	B::MutableString only supports I/O to @c char streams (i.e., 
	<tt>std::basic_ostream<char,char_traits<char>></tt>).
*/
//@{

// ------------------------------------------------------------------------------------------
/*! Writes the characters of @a str to @a os.  @a str is converted to ASCII prior to being 
	written.  If the conversion fails (eg there are non-7-bit-ASCII characters in @a str), 
	an exception may be thrown.
	
	If you need to write characters to a @c std::ostream that are in an encoding other 
	than 7-bit ASCII, use MutableString::Write() instead.
	
	@relates	MutableString
*/
inline std::ostream&
operator << (std::ostream& os, const MutableString& str)
{
	return (str.Write(os, kCFStringEncodingASCII));
}

// ------------------------------------------------------------------------------------------
/*! Reads the characters of the next word from @a is into the string @a str.  The 
	characters read from @a is are assumed to be 7-bit ASCII.  If they aren't, an 
	exception may be thrown.
	
	If you need to read characters from a @c std::istream that are in an encoding other 
	than 7-bit ASCII, use MutableString::Read() instead.
	
	@relates	MutableString
*/
inline std::istream&
operator >> (std::istream& is, MutableString& str)
{
	return (str.Read(is, kCFStringEncodingASCII));
}

// ------------------------------------------------------------------------------------------
/*! Reads the characters of the next line from @a is into the string @a str.  The 
	characters read from @a is are assumed to be 7-bit ASCII.  If they aren't, an 
	exception may be thrown.
	
	If you need to read characters from a @c std::istream that are in an encoding other 
	than 7-bit ASCII, use MutableString::Read() instead.
	
	@relates	MutableString
*/
inline std::istream&
getline(std::istream& is, MutableString& str, char delim)
{
	return (str.GetLine(is, delim, kCFStringEncodingASCII));
}

// ------------------------------------------------------------------------------------------
/*! Reads the characters of the next line from @a is into the string @a str.  The 
	characters read from @a is are assumed to be 7-bit ASCII.  If they aren't, an 
	exception may be thrown.
	
	If you need to read characters from a @c std::istream that are in an encoding other 
	than 7-bit ASCII, use MutableString::Read() instead.
	
	@relates	MutableString
*/
inline std::istream&
getline(std::istream& is, MutableString& str)
{
	return (str.GetLine(is, is.widen('\n'), kCFStringEncodingASCII));
}
//@}


//! @name String Comparisons
//@{

// ------------------------------------------------------------------------------------------
/*! Compares two strings for equality.
	
	@return		@c true if @a s1 is equal to @a s2
	@relates	MutableString
*/
inline bool	operator == (const MutableString& s1, const MutableString& s2)	{ return (s1.compare(s2) == 0); }
/*! @overload
*/
inline bool	operator == (const MutableString& s1, CFStringRef          s2)	{ return (s1.compare(s2) == 0); }
/*! @overload
*/
inline bool	operator == (CFStringRef          s1, const MutableString& s2)	{ return (s2.compare(s1) == 0); }

// ------------------------------------------------------------------------------------------
/*! Compares two strings for inequality.
	
	@return		@c true if @a s1 is not equal to @a s2
	@relates	MutableString
*/
inline bool	operator != (const MutableString& s1, const MutableString& s2)	{ return (s1.compare(s2) != 0); }
/*! @overload
*/
inline bool	operator != (const MutableString& s1, CFStringRef          s2)	{ return (s1.compare(s2) != 0); }
/*! @overload
*/
inline bool	operator != (CFStringRef          s1, const MutableString& s2)	{ return (s2.compare(s1) != 0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \<.
	
	@return		@c true if @a s1 is less than @a s2
	@relates	MutableString
*/
inline bool	operator <  (const MutableString& s1, const MutableString& s2)	{ return (s1.compare(s2) <  0); }
/*! @overload
*/
inline bool	operator <  (const MutableString& s1, CFStringRef          s2)	{ return (s1.compare(s2) <  0); }
/*! @overload
*/
inline bool	operator <  (CFStringRef          s1, const MutableString& s2)	{ return (s2.compare(s1) >= 0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \<=.
	
	@return		@c true if @a s1 is less than or equal to @a s2
	@relates	MutableString
*/
inline bool	operator <= (const MutableString& s1, const MutableString& s2)	{ return (s1.compare(s2) <= 0); }
/*! @overload
*/
inline bool	operator <= (const MutableString& s1, CFStringRef          s2)	{ return (s1.compare(s2) <= 0); }
/*! @overload
*/
inline bool	operator <= (CFStringRef          s1, const MutableString& s2)	{ return (s2.compare(s1) >  0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \>.
	
	@return		@c true if @a s1 is greater than @a s2
	@relates	MutableString
*/
inline bool	operator >  (const MutableString& s1, const MutableString& s2)	{ return (s1.compare(s2) >  0); }
/*! @overload
*/
inline bool	operator >  (const MutableString& s1, CFStringRef          s2)	{ return (s1.compare(s2) >  0); }
/*! @overload
*/
inline bool	operator >  (CFStringRef          s1, const MutableString& s2)	{ return (s2.compare(s1) <= 0); }

// ------------------------------------------------------------------------------------------
/*! Orders two strings according to \>=.
	
	@return		@c true if @a s1 is greater than or equal to @a s2
	@relates	MutableString
*/
inline bool	operator >= (const MutableString& s1, const MutableString& s2)	{ return (s1.compare(s2) >= 0); }
/*! @overload
*/
inline bool	operator >= (const MutableString& s1, CFStringRef          s2)	{ return (s1.compare(s2) >= 0); }
/*! @overload
*/
inline bool	operator >= (CFStringRef          s1, const MutableString& s2)	{ return (s2.compare(s1) <  0); }

//@}

//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a s1 and @a s2.

	@relates	MutableString
*/
inline void
swap(MutableString& s1, MutableString& s2)	{ s1.swap(s2); }
//@}

//@}

}	// namespace B

#endif	// BMutableString_H_
