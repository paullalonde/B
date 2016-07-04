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
#include "BMutableString.h"

// standard headers
#include <istream>
#include <ostream>
#include <stdexcept>
#include <vector>

// B headers
#include "BOSPtr.h"


namespace B {

// ------------------------------------------------------------------------------------------
/*!	Creates an empty string using @a allocator.
*/
MutableString::MutableString(
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
{
	init_str(NULL, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a str starting with index @a pos.
*/
MutableString::MutableString(
	const MutableString&	str,					//!< The source string.
	size_type				pos,					//!< The starting position for the copy within the source string.
	size_type				n /* = String::npos */,	//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(str.cf_ref(), pos, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a str starting with index @a pos.
*/
MutableString::MutableString(
	const String&	str,					//!< The source string.
	size_type		pos,					//!< The starting position for the copy within the source string.
	size_type		n /* = String::npos */,	//!< The number of characters to copy.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(str.cf_ref(), pos, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a new string that is initialised by, at most, the first @a n characters of 
	@a cfstr starting with index @a pos.
*/
MutableString::MutableString(
	CFStringRef		cfstr,					//!< The source string.
	size_type		pos,					//!< The starting position for the copy within the source string.
	size_type		n /* = String::npos */,	//!< The number of characters to copy.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(cfstr, pos, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a ustr.  If @a n is @c String::npos, then @a ustr is assumed to be null-terminated;  else, 
	@c NULL (@c '\\0' ) characters have no special meaning.
*/
MutableString::MutableString(
	const UniChar*	ustr,					//!< The source string.
	size_type		n /* = String::npos */,	//!< The number of characters to copy.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(ustr, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by @a n occurrences of character @a c.
*/
MutableString::MutableString(
	size_type		n,						//!< The number of occurrences of @a c.
	UniChar			c,						//!< The character to copy.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(n, c);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a wstr starting with index @a pos.
*/
MutableString::MutableString(
	const std::wstring&	wstr,					//!< The source string.
	size_type			pos /* = 0 */,			//!< The starting position for the copy within the source string.
	size_type			n /* = String::npos */,	//!< The number of characters to copy.
	CFAllocatorRef		allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(wstr, pos, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a wcstr.  If @a n is @c String::npos, then @a wcstr is assumed to be null-terminated;  else, 
	@c NULL (@c L'\\0' ) characters have no special meaning.
*/
MutableString::MutableString(
	const wchar_t*	wcstr,					//!< The source string.
	size_type		n /* = String::npos */,	//!< The number of characters to copy.
	CFAllocatorRef	allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(wcstr, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a sstr starting with index @a pos.  The source string is assumed to be encoded 
	using the character encoding given in @a encoding.
	
	@note	The default value for @a encoding is @c kCFStringEncodingASCII.  This allows the 
			caller to pass in a string literal without having to worry about specifying an 
			encoding.  It also will be more likely to catch errors (erroneous encodings).
*/
MutableString::MutableString(
	const std::string&	sstr,										//!< The source string.
	CFStringEncoding	encoding /* = kCFStringEncodingASCII */,	//!< The character encoding of the source string.
	size_type			pos /* = 0 */,								//!< The starting position for the copy within the source string.
	size_type			n /* = npos */,								//!< The number of characters to copy.
	CFAllocatorRef		allocator /* = NULL */)						//!< The new object's allocator.
{
	String	temp(sstr, encoding, pos, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a cstr.  If @a n is @c String::npos, then @a cstr is assumed to be null-terminated;  else, 
	@c NULL (@c '\\0' ) characters have no special meaning.  The source string is assumed 
	to be encoded using the character encoding given in @a encoding.
*/
MutableString::MutableString(
	const char*			cstr,										//!< The source string.
	CFStringEncoding	encoding /* = kCFStringEncodingASCII */,	//!< The character encoding of the source string.
	size_type			n /* = npos */,								//!< The number of characters to copy.
	CFAllocatorRef		allocator /* = NULL */)						//!< The new object's allocator.
{
	String	temp(cstr, encoding, n);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by @a pstr.  The source string is assumed 
	to be encoded using the character encoding given in @a encoding.
*/
MutableString::MutableString(
	ConstStringPtr		pstr,										//!< The source string.
	CFStringEncoding	encoding /* = kCFStringEncodingASCII */,	//!< The character encoding of the source string.
	CFAllocatorRef		allocator /* = NULL */)						//!< The new object's allocator.
{
	String	temp(pstr, encoding);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by @a blob.  The source string is assumed 
	to be encoded using the character encoding given in @a encoding.
*/
MutableString::MutableString(
	const std::vector<UInt8>&	blob,					//!< The source blob.
	CFStringEncoding			encoding,				//!< The character encoding of @a blob.
	bool						external,				//!< If true, checks for BOM and performs byte-swapping as necessary.
	CFAllocatorRef				allocator /* = NULL */)	//!< The new object's allocator.
{
	String	temp(blob, encoding, external);
	
	init_str(temp.cf_ref(), allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator.
*/
MutableString::MutableString(
	const OSPtr<CFStringRef>&	cfstr)	//!< The source string.
{
	B_ASSERT(cfstr != NULL);
	
	init_str(cfstr, CFGetAllocator(cfstr.get()));
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string object with @a cfstr. Does NOT copy @a cfstr.
*/
MutableString::MutableString(
	const OSPtr<CFMutableStringRef>&	cfstr)	//!< The source string.
{
	B_ASSERT(cfstr != NULL);
	
	mRef = cfstr;
	CFRetain(mRef);
}

// ------------------------------------------------------------------------------------------
void
MutableString::init_str(CFStringRef srcRef, CFAllocatorRef allocator)
{
	CFMutableStringRef	mutRef;
	
	if (srcRef != NULL)
	{
		mutRef = CFStringCreateMutableCopy(allocator, 0, srcRef);
	}
	else
	{
		mutRef = CFStringCreateMutable(allocator, 0);
	}
	
	B_THROW_IF_NULL(mutRef);
	
	mRef = mutRef;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFMutableStringRef>
MutableString::cf_ptr() const
{
	return (OSPtr<CFMutableStringRef>(mRef));
}

// ------------------------------------------------------------------------------------------
MutableString::size_type
MutableString::max_size() const
{
	return (UINT_MAX);
}

// ------------------------------------------------------------------------------------------
MutableString::size_type
MutableString::capacity() const
{
	return (CFStringGetLength(mRef));
}

// ------------------------------------------------------------------------------------------
/*!	Stub implementation.  CFMutableStringRef doesn't have a notion of capacity that 
	parallels the one in std::basic_string.
*/
void
MutableString::reserve(size_type /* n */ /* = 0 */)
{
}

// ------------------------------------------------------------------------------------------
/*!	If @a pos is smaller than zero or greater than or equal to @c length(), the function 
	throws an @c std::out_of_range exception.
	
	@exception	std::out_of_range
*/
MutableString::const_reference
MutableString::at(size_type pos) const
{
	size_type	len	= CFStringGetLength(mRef);
	
	B_THROW_IF(pos >= len, std::out_of_range("B::String::MutableString::at pos out of range"));
	
	return (CFStringGetCharacterAtIndex(mRef, pos));
}

// ------------------------------------------------------------------------------------------
/*!	If @a pos is smaller than zero or greater than or equal to @c length(), the function 
	throws an @c std::out_of_range exception.
	
	@exception	std::out_of_range
*/
MutableString::reference
MutableString::at(size_type pos)
{
	size_type	len	= CFStringGetLength(mRef);
	
	B_THROW_IF(pos >= len, std::out_of_range("B::String::MutableString::at pos out of range"));
	
	return (reference(mRef, pos));
}

// ------------------------------------------------------------------------------------------
MutableString&
MutableString::operator = (
	const UniChar*	ustr)	//!< The input string.
{
	String	temp(ustr, String::npos);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableString&
MutableString::operator = (
	UniChar	c)	//!< The input character.
{
	String	temp(1, c);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableString&
MutableString::operator = (
	const std::wstring&	wstr)	//!< The input string.
{
	String	temp(wstr, 0, wstr.size());
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableString&
MutableString::operator = (
	const wchar_t*	wcstr)	//!< The input string.
{
	String	temp(wcstr, String::npos);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	@note	The input string is assumed to be encoded as @c kCFStringEncodingASCII.
*/
MutableString&
MutableString::operator = (
	const std::string&	sstr)	//!< The input string.
{
	String	temp(sstr, kCFStringEncodingASCII, 0, sstr.size());
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	@note	The input string is assumed to be encoded as @c kCFStringEncodingASCII.
*/
MutableString&
MutableString::operator = (
	const char*	cstr)	//!< The input string.
{
	String	temp(cstr, kCFStringEncodingASCII, String::npos);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableString&
MutableString::operator = (
	const OSPtr<CFStringRef>&	cfstr)	//!< The input string.
{
	B_ASSERT(cfstr != NULL);
	
	CFStringReplaceAll(mRef, cfstr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
MutableString&
MutableString::operator = (
	const OSPtr<CFMutableStringRef>&	cfstr)	//!< The input string.
{
	B_ASSERT(cfstr != NULL);
	
	CFStringReplaceAll(mRef, cfstr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a str starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a str.size().
*/
MutableString&
MutableString::assign(
	const String&	str,				//!< The input string.
	size_type		pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(str, pos, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a str starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a str.size().
*/
MutableString&
MutableString::assign(
	const MutableString&	str,				//!< The input string.
	size_type				pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type				n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(str, pos, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a cfstr starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @c CFStringGetLength(cfstr).
*/
MutableString&
MutableString::assign(
	CFStringRef		cfstr,				//!< The input string.
	size_type		pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(cfstr, pos, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a n characters of @a ustr.  If @a n is @c npos, then @a ustr is assumed to be 
	null-terminated.
*/
MutableString&
MutableString::assign(
	const UniChar*	ustr,				//!< The input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(ustr, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a num occurrences of character @a c.
*/
MutableString&
MutableString::assign(
	size_type	n,		//!< The number of occurrences of @a c.
	UniChar		c)		//!< The character to assign.
{
	String	temp(n, c);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a wstr starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a wstr.size().
*/
MutableString&
MutableString::assign(
	const std::wstring&	wstr,				//!< The input string.
	size_type			pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type			n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(wstr, pos, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a wcstr starting with index @a pos.  If @a n is 
	@c npos, then @a wcstr is assumed to be null-terminated.
*/
MutableString&
MutableString::assign(
	const wchar_t*	wcstr,				//!< The input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(wcstr, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a sstr starting with index @a pos.  The input string 
	is assumed to be encoded using the character encoding given in @a encoding.
	
	@exception	@c std::out_of_range	If @a pos > @a sstr.size().
*/
MutableString&
MutableString::assign(
	const std::string&	sstr,			//!< The input string.
	CFStringEncoding	encoding,		//!< The input string's encoding.
	size_type			pos /* = 0 */,	//!< The index of the first character to assign from the input string.
	size_type			n /* = npos */)	//!< The number of characters to assign.
{
	String	temp(sstr, encoding, pos, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a cstr starting with index @a pos.  If @a n is 
	@c npos, then @a cstr is assumed to be null-terminated.  The input string is assumed 
	to be encoded using the character encoding given in @a encoding.
*/
MutableString&
MutableString::assign(
	const char*			cstr,			//!< The input string.
	CFStringEncoding	encoding,		//!< The input string's encoding.
	size_type			n /* = npos */)	//!< The number of characters to assign.
{
	String	temp(cstr, encoding, n);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	The input string is assumed to be encoded using the character encoding given in 
	@a encoding.
*/
MutableString&
MutableString::assign(
	ConstStringPtr		pstr,		//!< The input string.
	CFStringEncoding	encoding)	//!< The input string's encoding.
{
	String	temp(pstr, encoding);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns the string from @a blob.  The source string is assumed to be encoded using the 
	character encoding given in @a encoding.
*/
MutableString&
MutableString::assign(
	const std::vector<UInt8>&	blob,		//!< The source blob.
	CFStringEncoding			encoding,	//!< The character encoding of @a blob.
	bool						external)	//!< If true, checks for BOM and performs byte-swapping as necessary.
{
	String	temp(blob, encoding, external);
	
	CFStringReplaceAll(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	@note	The input string is assumed to be encoded as @c kCFStringEncodingASCII.
*/
MutableString&
MutableString::operator += (
	const std::string&	sstr)	//!< The input string.
{
	String	temp(sstr, kCFStringEncodingASCII);
	
	CFStringAppend(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Appends, at most, @a n characters for @a str, starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a str.size().
*/
MutableString&
MutableString::append(
	const MutableString&	str,			//!< The input string.
	size_type				pos /* = 0 */,	//!< The index of the first character to append from the input string.
	size_type				n /* = npos */)	//!< The number of characters to append.
{
	String	temp(str.cf_ref(), pos, n);
	
	CFStringAppend(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Appends, at most, @a n characters for @a str, starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a str.size().
*/
MutableString&
MutableString::append(
	const String&	str,			//!< The input string.
	size_type		pos /* = 0 */,	//!< The index of the first character to append from the input string.
	size_type		n /* = npos */)	//!< The number of characters to append.
{
	String	temp(str.cf_ref(), pos, n);
	
	CFStringAppend(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Appends, at most, @a n characters for @a cfstr, starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @c CFStringGetLength(cfstr).
*/
MutableString&
MutableString::append(
	CFStringRef	cfstr,			//!< The input string.
	size_type	pos /* = 0 */,	//!< The index of the first character to append from the input string.
	size_type	n /* = npos */)	//!< The number of characters to append.
{
	String	temp(cfstr, pos, n);
	
	CFStringAppend(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Appends @a num occurrences of character @a c.
*/
MutableString&
MutableString::append(
	size_type	n,		//!< The number of occurrences of @a c.
	UniChar		c)		//!< The character to append.
{
	String	temp(n, c);
	
	CFStringAppend(mRef, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts, at most, @a n characters of @a str, starting with index @a pos2, so that the 
	new characters start with index @a pos1.
	
	@exception	@c std::out_of_range	If @a pos2 > @a str.size() or @a pos1 > @c size().
*/
MutableString&
MutableString::insert(
	size_type				pos1,			//!< The index at which to start inserting characters.
	const MutableString&	str,			//!< The input string.
	size_type				pos2 /* = 0 */,	//!< The index of the first character to insert from the input string.
	size_type				n /* = npos */)	//!< The number of characters to insert.
{
	String	temp(str.cf_ref(), pos2, n);
	
	CFStringInsert(mRef, pos1, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts, at most, @a n characters of @a str, starting with index @a pos2, so that the 
	new characters start with index @a pos1.
	
	@exception	@c std::out_of_range	If @a pos2 > @a str.size() or @a pos1 > @c size().
*/
MutableString&
MutableString::insert(
	size_type		pos1,			//!< The index at which to start inserting characters.
	const String&	str,			//!< The input string.
	size_type		pos2 /* = 0 */,	//!< The index of the first character to insert from the input string.
	size_type		n /* = npos */)	//!< The number of characters to insert.
{
	String	temp(str.cf_ref(), pos2, n);
	
	CFStringInsert(mRef, pos1, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts, at most, @a n characters of @a cfstr, starting with index @a pos2, so that the 
	new characters start with index @a pos1.
	
	@exception	@c std::out_of_range	If @a pos2 > @c CFStringGetLength(cfstr) or @a pos1 > @c size().
*/
MutableString&
MutableString::insert(
	size_type	pos1,			//!< The index at which to start inserting characters.
	CFStringRef	cfstr,			//!< The input string.
	size_type	pos2 /* = 0 */,	//!< The index of the first character to insert from the input string.
	size_type	n /* = npos */)	//!< The number of characters to insert.
{
	String	temp(cfstr, pos2, n);
	
	CFStringInsert(mRef, pos1, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts @a n characters of @a ustr so that the new characters start with index @a pos.  
	If @a n is @c npos, then @a ustr is assumed to be null-terminated.
	
	@exception	@c std::out_of_range	If @a pos > @c size().
*/
MutableString&
MutableString::insert(
	size_type		pos,			//!< The index at which to start inserting characters.
	const UniChar*	ustr,			//!< The input string.
	size_type		n /* = npos */)	//!< The number of characters to insert.
{
	String	temp(ustr, n);
	
	CFStringInsert(mRef, pos, temp.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts a copy of character @a c before the character to which iterator @a it refers.
*/
MutableString::iterator
MutableString::insert(
	iterator	it,	//!< The location at which to insert @a c.
	UniChar		c)	//!< The character to insert.
{
	String	temp(1, c);
	
	CFStringInsert(mRef, it.mIndex, temp.cf_ref());
	
	return (it);
}

// ------------------------------------------------------------------------------------------
/*!	Inserts @a n occurences of @a c at the location in the string given in @a it.
*/
void
MutableString::insert(
	iterator	it,	//!< The location at which to start inserting characters.
	size_type	n,	//!< The number of occurrences of @a c to insert.
	UniChar		c)	//!< The character to insert.
{
	String	temp(n, c);
	
	CFStringInsert(mRef, it.mIndex, temp.cf_ref());
}

// ------------------------------------------------------------------------------------------
/*!	Removes, at most, @a n characters from @c *this, starting at index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @c size().
*/
MutableString&
MutableString::erase(
	size_type		pos,			//!< The index at which to start inserting characters.
	size_type		n /* = npos */)	//!< The number of characters to insert.
{
	if (n == String::npos)
		n = size() - pos;
	
	CFStringDelete(mRef, CFRangeMake(pos, n));
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Removes the single character to which iterator @a it refers.

	@return		An iterator referring to the first character remaining, or end().
	@exception	@c std::out_of_range	If @a pos > @c size().
*/
MutableString::iterator
MutableString::erase(iterator it)
{
	CFStringDelete(mRef, CFRangeMake(it.mIndex, 1));
	
	return (it);
}

// ------------------------------------------------------------------------------------------
/*!	Removes the characters in the range [@a first, @a last).

	@return		An iterator referring to the first character remaining, or end().
	@exception	@c std::out_of_range	If @a pos > @c size().
*/
MutableString::iterator
MutableString::erase(iterator first, iterator last)
{
	CFStringDelete(mRef, 
				   CFRangeMake(first.mIndex, last.mIndex - first.mIndex));
	
	return (first);
}

// ------------------------------------------------------------------------------------------
/*!	Changes the number of characters in @c *this to @a n.  Thus, if @a n is not equal to 
	@a size(), the function appends or removes characters at the end according to the 
	new size.
*/
void
MutableString::resize(
	size_type	n,	//!< The string's new size.
	UniChar		c)	//!< The character to append if increasing the string's size.
{
	size_t	len	= size();
	
	if (len > n)
	{
		erase(n, String::npos);
	}
	else if (n > len)
	{
		append(n-len, c);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	at most @a n2 characters of @a str staring with index @a pos2.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size() or @a pos2 > @a str.size().
*/
MutableString&
MutableString::replace(
	size_type				pos1,	//!< The index of the first character to replace.
	size_type				n1,		//!< The number of characters to replace.
	const MutableString&	str,	//!< The input string.
	size_type				pos2,	//!< The index of the first character from the input string.
	size_type				n2)		//!< The number of characters from the input string.
{
	String	temp(str, pos2, n2);
	
	return (replace(pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	at most @a n2 characters of @a str staring with index @a pos2.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size() or @a pos2 > @a str.size().
*/
MutableString&
MutableString::replace(
	size_type		pos1,	//!< The index of the first character to replace.
	size_type		n1,		//!< The number of characters to replace.
	const String&	str,	//!< The input string.
	size_type		pos2,	//!< The index of the first character from the input string.
	size_type		n2)		//!< The number of characters from the input string.
{
	String	temp(str, pos2, n2);
	
	return (replace(pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	at most @a n2 characters of @a cfstr staring with index @a pos2.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size() or @a pos2 > @c CFStringGetLength(cfstr).
*/
MutableString&
MutableString::replace(
	size_type		pos1,	//!< The index of the first character to replace.
	size_type		n1,		//!< The number of characters to replace.
	CFStringRef		cfstr,	//!< The input string.
	size_type		pos2,	//!< The index of the first character from the input string.
	size_type		n2)		//!< The number of characters from the input string.
{
	String	temp(cfstr, pos2, n2);
	
	return (replace(pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	@a n2 characters of @a ustr.  If @a n is @c npos, then @a ustr is assumed to be 
	null-terminated.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size()
*/
MutableString&
MutableString::replace(
	size_type		pos1,				//!< The index of the first character to replace.
	size_type		n1,					//!< The number of characters to replace.
	const UniChar*	ustr,				//!< The input string.
	size_type		n2 /* = npos */)	//!< The number of characters from the input string.
{
	String	temp(ustr, n2);
	
	return (replace(pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Replaces, at most, @a n1 characters of @c *this, starting with index @a pos1, with 
	@a n occurrences of character @a c.
	
	@exception	@c std::out_of_range	If @a pos1 > @c size()
*/
MutableString&
MutableString::replace(
	size_type		pos1,	//!< The index of the first character to replace.
	size_type		n1,		//!< The number of characters to replace.
	size_type		n,		//!< The number of occurrences of @a c to insert.
	UniChar			c)		//!< The character to insert.
{
	String	temp(n, c);
	
	return (replace(pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Replaces all characters of the range [@a first, @a last) with @a n2 characters 
	of @a ustr.  If @a n is @c npos, then @a ustr is assumed to be null-terminated.
*/
MutableString&
MutableString::replace(
	iterator		it1,				//!< The start of the range of characters to replace.
	iterator		it2,				//!< The end of the range of characters to replace.
	const UniChar*	ustr,				//!< The input string.
	size_type		n2 /* = npos */)	//!< The number of characters from the input string.
{
	String	temp(ustr, n2);
	
	return (replace(it1, it2, temp));
}

// ------------------------------------------------------------------------------------------
/*!	Replaces all characters of the range [@a first, @a last) with @a n occurrences 
	of character @a c.
*/
MutableString&
MutableString::replace(
	iterator		it1,	//!< The start of the range of characters to replace.
	iterator		it2,	//!< The end of the range of characters to replace.
	size_type		n,		//!< The number of occurrences of @a c to insert.
	UniChar			c)		//!< The character to insert.
{
	String	temp(n, c);
	
	return (replace(it1, it2, temp));
}

// ------------------------------------------------------------------------------------------
MutableString::const_iterator
MutableString::begin() const
{
	return (const_iterator(mRef, 0));
}

// ------------------------------------------------------------------------------------------
MutableString::iterator
MutableString::begin()
{
	return (iterator(mRef, 0));
}

// ------------------------------------------------------------------------------------------
MutableString::const_iterator
MutableString::end() const
{
	return (const_iterator(mRef, CFStringGetLength(mRef)));
}

// ------------------------------------------------------------------------------------------
MutableString::iterator
MutableString::end()
{
	return (iterator(mRef, CFStringGetLength(mRef)));
}

// ------------------------------------------------------------------------------------------
MutableString::const_reverse_iterator
MutableString::rbegin() const
{
	return (const_reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
MutableString::reverse_iterator
MutableString::rbegin()
{
	return (reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
MutableString::const_reverse_iterator
MutableString::rend() const
{
	return (const_reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
MutableString::reverse_iterator
MutableString::rend()
{
	return (reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
std::ostream&
MutableString::Write(std::ostream& ostr, CFStringEncoding encoding) const
{
	std::string	sstr;
	
	copy(sstr, encoding);
	
	return (ostr << sstr);
}

// ------------------------------------------------------------------------------------------
std::istream&
MutableString::Read(std::istream& istr, CFStringEncoding encoding)
{
	std::string	sstr;
	
	istr >> sstr;
	
	assign(sstr, encoding);
	
	return (istr);
}

// ------------------------------------------------------------------------------------------
std::istream&
MutableString::GetLine(std::istream& istr, char delim, CFStringEncoding encoding)
{
	std::string	sstr;
	
	getline(istr, sstr, delim);
	
	assign(sstr, encoding);
	
	return (istr);
}


}	// namespace B
