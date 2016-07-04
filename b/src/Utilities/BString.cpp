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
#include "BString.h"

// standard headers
#include <istream>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <wchar.h>

// system headers
#include <CoreFoundation/CFBundle.h>

// B headers
#include "BBundle.h"
#include "BException.h"
#include "BMutableString.h"


namespace B {


// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::make_str(CFStringRef cfstr, size_type pos, size_type n, CFAllocatorRef allocator, bool is_temp)
{
	B_ASSERT(cfstr != NULL);
	
	size_type		len	= CFStringGetLength(cfstr);
	CFStringRef		str;
	const UniChar*	ucp;
	
	B_THROW_IF(pos > len, std::out_of_range("B::String::make_str pos out of range"));
	
	if (n == npos)
		n = len - pos;
	
	if ((pos == 0) && (n == len))
	{
		str = CFStringCreateCopy(allocator, cfstr);
	}
	else if (is_temp && ((ucp = CFStringGetCharactersPtr(cfstr)) != NULL))
	{
		str = CFStringCreateWithCharactersNoCopy(allocator, ucp+pos, n, kCFAllocatorNull);
	}
	else
	{
		str = CFStringCreateWithSubstring(allocator, cfstr, CFRangeMake(pos, n));
	}
	
	return (OSPtr<CFStringRef>(str, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::make_str(const UniChar* ustr, size_type pos, size_type n, CFAllocatorRef allocator, bool is_temp)
{
	CFStringRef	str;
	
	if (n == npos)
		n = uni_strlen(ustr) - pos;
	
	if (is_temp)
	{
		str = CFStringCreateWithCharactersNoCopy(allocator, ustr+pos, n, 
												 kCFAllocatorNull);
	}
	else
	{
		str = CFStringCreateWithCharacters(allocator, ustr+pos, n);
	}
	
	return (OSPtr<CFStringRef>(str, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::make_str(size_type n, UniChar c, CFAllocatorRef allocator)
{
	std::vector<UniChar>	buff(n, c);
	
	return (OSPtr<CFStringRef>(CFStringCreateWithCharacters(allocator, &buff[0], n), from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::make_str(const wchar_t* wcstr, size_type pos, size_type n, CFAllocatorRef allocator, bool /* is_temp */)
{
	if (n == npos)
		n = wcslen(wcstr) - pos;
	
	CFStringRef	cfstr = CFStringCreateWithBytes(allocator, 
												reinterpret_cast<const UInt8*>(wcstr + pos), 
												n * sizeof(wchar_t), 
												kCFStringEncodingUTF32, false);
	
	return (OSPtr<CFStringRef>(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::make_str(const char* cstr, size_type pos, size_type n, CFStringEncoding encoding, CFAllocatorRef allocator, bool is_temp)
{
	CFStringRef	str;
	
	if (n == npos)
	{
		if (is_temp)
		{
			str = CFStringCreateWithCStringNoCopy(allocator, cstr+pos, 
												  encoding, 
												  kCFAllocatorNull);
		}
		else
		{
			str = CFStringCreateWithCString(allocator, cstr+pos, encoding);
		}
	}
	else
	{
		str = CFStringCreateWithBytes(allocator, 
									  reinterpret_cast<const UInt8 *>(cstr+pos), 
									  n, encoding, false);
	}
	
	B_THROW_IF(str == NULL, CharacterEncodingException());
	
	return (OSPtr<CFStringRef>(str, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::make_str(ConstStringPtr pstr, CFStringEncoding encoding, CFAllocatorRef allocator, bool is_temp)
{
	CFStringRef	str;
	
	if (is_temp)
	{
		str = CFStringCreateWithPascalStringNoCopy(allocator, pstr, encoding, 
												   kCFAllocatorNull);
	}
	else
	{
		str = CFStringCreateWithPascalString(allocator, pstr, encoding);
	}
	
	B_THROW_IF(str == NULL, CharacterEncodingException());
	
	return (OSPtr<CFStringRef>(str, from_copy));
}

// ------------------------------------------------------------------------------------------
size_t
String::uni_strlen(const UniChar* ustr)
{
	size_t	len	= static_cast<size_t>(-1);
	
	--ustr;
	
	do
		len++;
	while (*++ustr);
	
	return (len);
}


// ------------------------------------------------------------------------------------------
/*	@note	Since instances of String are immutable, this constructor isn't very useful.
			It's here for completeness.
*/
String::String(
	CFAllocatorRef	allocator)	//!< The new object's allocator.
{
	UniChar	junk;
	
	mRef = CFStringCreateWithCharacters(allocator, &junk, 0);
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a str starting with index @a pos.
*/
String::String(
	const String&			str,					//!< The source string.
	size_type				pos,					//!< The starting position for the copy within the source string.
	size_type				n /* = npos */,			//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(str.cf_ref(), pos, n, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a str, using @a str's allocator.
*/
String::String(
	const MutableString&	str)	//!< The source string.
{
	mRef = CFStringCreateCopy(str.get_allocator(), str.cf_ref());
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a str starting with index @a pos.
*/
String::String(
	const MutableString&	str,					//!< The source string.
	size_type				pos,					//!< The starting position for the copy within the source string.
	size_type				n /* = npos */,			//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(str.cf_ref(), pos, n, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator.
*/
String::String(
	CFStringRef	cfstr)	//!< The source string.
{
	B_ASSERT(cfstr != NULL);
	
	mRef = CFStringCreateCopy(CFGetAllocator(cfstr), cfstr);
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator. The input string's 
 *	retain count is not incremented.
*/
String::String(
	CFStringRef	cfstr, 	//!< The source string.
	const from_copy_t&)	//!< An indication that @a cfstr comes from a Copy or Create function, and therefore that its retain count should not be increased.
{
	B_ASSERT(cfstr != NULL);
	
	mRef = CFStringCreateCopy(CFGetAllocator(cfstr), cfstr);
	B_THROW_IF_NULL(mRef);
	
	OSPtrOwnership::CFObjectOwnershipTrait::release(cfstr);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator. If @a cfstr is 
 *	@c NULL, use an empty string instead.
*/
String::String(
	CFStringRef	cfstr,		//!< The source string.
	const std::nothrow_t&)	//!< An indication that the function should not throw an exception if @cfstr is @c NULL.
{
	if (cfstr != NULL)
	{
		mRef = CFStringCreateCopy(CFGetAllocator(cfstr), cfstr);
		B_THROW_IF_NULL(mRef);
	}
	else
	{
		mRef = CFSTR("");
		OSPtrOwnership::CFObjectOwnershipTrait::retain(mRef);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator. If @a cfstr is 
 *	@c NULL, use an empty string instead.  The input string's retain count is not 
 *	incremented.
*/
String::String(
	CFStringRef	cfstr,		//!< The source string.
	const from_copy_t&,		//!< An indication that @a cfstr comes from a Copy or Create function, and therefore that its retain count should not be increased.
	const std::nothrow_t&)	//!< An indication that the function should not throw an exception if @cfstr is @c NULL.
{
	if (cfstr != NULL)
	{
		mRef = CFStringCreateCopy(CFGetAllocator(cfstr), cfstr);
		B_THROW_IF_NULL(mRef);
		
		OSPtrOwnership::CFObjectOwnershipTrait::release(cfstr);
	}
	else
	{
		mRef = CFSTR("");
		OSPtrOwnership::CFObjectOwnershipTrait::retain(mRef);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Creates a new string that is initialised by, at most, the first @a n characters of 
	@a cfstr starting with index @a pos.
*/
String::String(
	CFStringRef				cfstr,					//!< The source string.
	size_type				pos,					//!< The starting position for the copy within the source string.
	size_type				n /* = npos */,			//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(cfstr, pos, n, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a ustr.  If @a n is @c npos, then @a ustr is assumed to be null-terminated;  else, 
	@c NULL (@c '\\0' ) characters have no special meaning.
*/
String::String(
	const UniChar*			ustr,					//!< The source string.
	size_type				n /* = npos */,			//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(ustr, 0, n, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by @a n occurrences of character @a c.
*/
String::String(
	size_type				n,						//!< The number of occurrences of @a c.
	UniChar					c,						//!< The character to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(n, c, allocator).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a wstr starting with index @a pos.
*/
String::String(
	const std::wstring&		wstr,					//!< The source string.
	size_type				pos /* = 0 */,			//!< The starting position for the copy within the source string.
	size_type				n /* = npos */,			//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(wstr.c_str(), pos, (n == npos) ? wstr.size()-pos : n, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a wcstr.  If @a n is @c npos, then @a wcstr is assumed to be null-terminated;  else, 
	@c NULL (@c L'\\0' ) characters have no special meaning.
*/
String::String(
	const wchar_t*			wcstr,					//!< The source string.
	size_type				n /* = npos */,			//!< The number of characters to copy.
	CFAllocatorRef			allocator /* = NULL */)	//!< The new object's allocator.
		: mRef(make_str(wcstr, 0, n, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a sstr starting with index @a pos.  The source string is assumed to be encoded 
	using the character encoding given in @a encoding.
	
	@note	The default value for @a encoding is @c kCFStringEncodingASCII.  This allows the 
			caller to pass in a string literal without having to worry about specifying an 
			encoding.  It also will be more likely to catch errors (erroneous encodings).
*/
String::String(
	const std::string&	sstr,										//!< The source string.
	CFStringEncoding	encoding /* = kCFStringEncodingASCII */,	//!< The character encoding of the source string.
	size_type			pos /* = 0 */,								//!< The starting position for the copy within the source string.
	size_type			n /* = npos */,								//!< The number of characters to copy.
	CFAllocatorRef		allocator /* = NULL */)						//!< The new object's allocator.
		: mRef(make_str(sstr.c_str(), pos, (n == npos) ? sstr.size()-pos : n, encoding, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by, at most, the first @a n characters of 
	@a cstr.  If @a n is @c npos, then @a cstr is assumed to be null-terminated;  else, 
	@c NULL (@c '\\0' ) characters have no special meaning.  The source string is assumed 
	to be encoded using the character encoding given in @a encoding.
	
	@note	The default value for @a encoding is @c kCFStringEncodingASCII.  This allows the 
			caller to pass in a string literal without having to worry about specifying an 
			encoding.  It also will be more likely to catch errors (erroneous encodings).
*/
String::String(
	const char*			cstr,										//!< The source string.
	CFStringEncoding	encoding /* = kCFStringEncodingASCII */,	//!< The character encoding of the source string.
	size_type			n /* = npos */,								//!< The number of characters to copy.
	CFAllocatorRef		allocator /* = NULL */)						//!< The new object's allocator.
		: mRef(make_str(cstr, 0, n, encoding, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by @a pstr.  The source string is assumed 
	to be encoded using the character encoding given in @a encoding.
	
	@note	The default value for @a encoding is @c kCFStringEncodingASCII.  This allows the 
			caller to pass in a string literal without having to worry about specifying an 
			encoding.  It also will be more likely to catch errors (erroneous encodings).
*/
String::String(
	ConstStringPtr		pstr,										//!< The source string.
	CFStringEncoding	encoding /* = kCFStringEncodingASCII */,	//!< The character encoding of the source string.
	CFAllocatorRef		allocator /* = NULL */)						//!< The new object's allocator.
		: mRef(make_str(pstr, encoding, allocator, false).release())
{
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string that is initialised by @a blob.  The source string is assumed 
	to be encoded using the character encoding given in @a encoding.
*/
String::String(
	const std::vector<UInt8>&	blob,					//!< The source blob.
	CFStringEncoding			encoding,				//!< The character encoding of @a blob.
	bool						external,				//!< If true, checks for BOM and performs byte-swapping as necessary.
	CFAllocatorRef				allocator /* = NULL */)	//!< The new object's allocator.
{
	mRef = CFStringCreateWithBytes(allocator, &blob[0], blob.size(), 
								   encoding, external);
	
	B_THROW_IF(mRef == NULL, CharacterEncodingException());
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator.
*/
String::String(
	const OSPtr<CFStringRef>&	cfstr)	//!< The source string.
{
	B_ASSERT(cfstr != NULL);
	
	mRef = CFStringCreateCopy(CFGetAllocator(cfstr.get()), cfstr);
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a string as a copy of @a cfstr, using @a cfstr's allocator.
*/
String::String(
	const OSPtr<CFMutableStringRef>&	cfstr)	//!< The source string.
{
	B_ASSERT(cfstr != NULL);
	
	mRef = CFStringCreateCopy(CFGetAllocator(cfstr.get()), cfstr);
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
OSPtr<CFStringRef>
String::cf_ptr() const
{
	return (OSPtr<CFStringRef>(mRef));
}

// ------------------------------------------------------------------------------------------
String::size_type
String::max_size() const
{
	return (UINT_MAX);
}

// ------------------------------------------------------------------------------------------
String::size_type
String::capacity() const
{
	return (size());
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, size_type pos1, size_type n1, CFStringRef cfstr)
{
	size_type	len	= CFStringGetLength(ref);
	
	B_THROW_IF(pos1 > len, std::out_of_range("B::String::compare pos1 out of range"));
	
	if (n1 == npos)
		n1 = len - pos1;
	
	return (CFStringCompareWithOptions(ref, cfstr, CFRangeMake(pos1, n1), 0));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, size_type pos1, size_type n1, CFStringRef cfstr, size_type pos2, size_type n2)
{
	String	temp(cfstr, pos2, n2);
	
	return (private_compare(ref, pos1, n1, temp.cf_ref()));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, const UniChar* ustr)
{
	OSPtr<CFStringRef>	temp(make_str(ustr, 0, uni_strlen(ustr), NULL, true));
	
	return (private_compare(ref, temp));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, size_type pos1, size_type n1, const UniChar* ustr)
{
	OSPtr<CFStringRef>	temp(make_str(ustr, 0, uni_strlen(ustr), NULL, true));
	
	return (private_compare(ref, pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, size_type pos1, size_type n1, const UniChar* ustr, size_type n2)
{
	OSPtr<CFStringRef>	temp(make_str(ustr, 0, n2, NULL, true));
	
	return (private_compare(ref, pos1, n1, temp));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, const std::string& sstr, CFStringEncoding encoding)
{
	String	temp(sstr, encoding);
	
	return (private_compare(ref, temp.cf_ref()));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, size_type pos1, size_type n1, const std::string& sstr, CFStringEncoding encoding)
{
	String	temp(sstr, encoding);
	
	return (private_compare(ref, pos1, n1, temp.cf_ref()));
}

// ------------------------------------------------------------------------------------------
int
String::private_compare(CFStringRef ref, size_type pos1, size_type n1, const std::string& sstr, CFStringEncoding encoding, size_type pos2, size_type n2)
{
	String	temp(sstr, encoding, pos2, n2);
	
	return (private_compare(ref, pos1, n1, temp.cf_ref()));
}

// ------------------------------------------------------------------------------------------
/*!	If @a pos is smaller than zero or greater than or equal to @c length(), the function 
	throws an @c std::out_of_range exception.
	
	@exception	std::out_of_range
*/
String::const_reference
String::at(size_type pos) const
{
	size_type	len	= CFStringGetLength(mRef);
	
	B_THROW_IF(pos >= len, std::out_of_range("B::String::at pos out of range"));
	
	return (CFStringGetCharacterAtIndex(mRef, pos));
}

// ------------------------------------------------------------------------------------------
/*!	@return	The number of copied characters.
*/
String::size_type
String::private_copy(
	CFStringRef	ref,	//!< The input string.
	UniChar*	ustr,	//!< The output buffer.
	size_type	n,		//!< The maximum number of characters to copy.
	size_type	pos)	//!< The starting index (in @c *this) for conversion.
{
	size_type	len	= CFStringGetLength(ref);
	
	B_THROW_IF(pos > len, std::out_of_range("B::String::private_copy(UniChar*) pos out of range"));
	
	size_type	rlen	= std::min(n, len-pos);
	
	CFStringGetCharacters(ref, CFRangeMake(pos, rlen), ustr);
	
	return (rlen);
}

// ------------------------------------------------------------------------------------------
/*!	@return	The number of copied characters.
*/
String::size_type
String::private_copy(
	CFStringRef	ref,	//!< The input string.
	wchar_t*	wcstr,	//!< The output buffer.
	size_type	n,		//!< The maximum number of characters to copy.
	size_type	pos)	//!< The starting index (in @c *this) for conversion.
{
	size_type	len	= CFStringGetLength(ref);
	
	B_THROW_IF(pos > len, std::out_of_range("B::String::private_copy(wchar_t*) pos out of range"));
	
	size_type	rlen	= std::min(n, len-pos);
	CFIndex		done, used;
	
	done = CFStringGetBytes(ref, 
				CFRangeMake(pos, rlen), 
				kCFStringEncodingUTF32, 0, false, 
				reinterpret_cast<UInt8*>(wcstr), rlen * sizeof(wchar_t), &used);
	
	B_ASSERT(used % sizeof(wchar_t) == 0);

	return (used / sizeof(wchar_t));
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_copy(
	CFStringRef			ref,		//!< The input string.
	char*				cstr,		//!< The output buffer.
	size_type			n,			//!< The maximum number of characters to copy.
	size_type&			pos,		//!< The starting index (in @c *this) for conversion.  On output, holds the index of the next character to convert, or @c npos if conversion is complete.
	CFStringEncoding	encoding)	//!< The output buffer's encoding.
{
	size_type	len	= CFStringGetLength(ref);
	
	B_THROW_IF(pos > len, std::out_of_range("B::String::private_copy(char*) pos out of range"));

	CFIndex	rlen	= std::min(n, len-pos);
	CFRange	range	= { pos, rlen };
	CFIndex	nconv, nbuff;
	
	// First check for convertability.
	
	nconv = CFStringGetBytes(ref, range, encoding, 0, false, 
							 NULL, 0, &nbuff);
	
	B_THROW_IF(nconv != rlen, CharacterEncodingException());
	
	// Now perform the conversion.
	
	CFStringGetBytes(ref, range, encoding, 0, false, 
					 reinterpret_cast<UInt8*>(cstr), rlen, 
					 &nbuff);
	
	if ((pos += nconv) == len)
		pos = npos;
	
	return (nbuff);
}

// ------------------------------------------------------------------------------------------
void
String::private_copy(
	CFStringRef		ref,	//!< The input string.
	std::wstring&	wstr)	//!< The output string.
{
	const size_t	MAKE_WSTRING_BUFF_SIZE	= 10000;
	
	wstr.clear();
	
	CFIndex	size	= CFStringGetLength(ref);
	
	if (size > 0)
	{
		CFIndex	done	= 0;
		
		while (done < size)
		{
			wchar_t	buffer[MAKE_WSTRING_BUFF_SIZE / sizeof(wchar_t)];
			CFIndex	used;
			
			done += CFStringGetBytes(ref, 
							CFRangeMake(done, size - done), 
							kCFStringEncodingUTF32, 0, false, 
							reinterpret_cast<UInt8*>(buffer), sizeof(buffer), &used);
			
			B_ASSERT(used % sizeof(wchar_t) == 0);
	
			wstr.append(buffer, used / sizeof(wchar_t));
		}
	}
}

// ------------------------------------------------------------------------------------------
void
String::private_copy(
	CFStringRef			ref,		//!< The input string.
	std::string&		sstr,		//!< The output string.
	CFStringEncoding	encoding)	//!< The output string's encoding.
{
	CFIndex	len		= CFStringGetLength(ref);
	CFRange	range	= { 0, len };
	CFIndex	nconv, nbuff;
	
	// First check for convertability.
	
	nconv = CFStringGetBytes(ref, range, encoding, 0, false, 
							 NULL, 0, &nbuff);
	
	B_THROW_IF(nconv != len, CharacterEncodingException());
	
	std::vector<char>	buff(CFStringGetMaximumSizeForEncoding(len, encoding));
	
	sstr.clear();
	
	while (range.length > 0)
	{
		nconv = CFStringGetBytes(ref, range, encoding, 0, false, 
								 reinterpret_cast<UInt8*>(&buff[0]), buff.size(), 
								 &nbuff);
		
		sstr.append(&buff[0], nbuff);
		
		range.location	+= nconv;
		range.length	-= nconv;
	}
}

// ------------------------------------------------------------------------------------------
void
String::private_copy(
	CFStringRef			ref,		//!< The input string.
	StringPtr			pstr,		//!< The output string.
	size_type			n,			//!< The maximum size of the output string, including length byte.
	CFStringEncoding	encoding)	//!< The output string's encoding.
{
	if (!CFStringGetPascalString(ref, pstr, n, encoding))
	{
		// Couldn't retrieve the contents as a Pascal string.  This could be due to 
		// encoding problems, or a buffer-too-small condition.  Determine which one it is.
		
		CFIndex	len		= CFStringGetLength(ref);
		CFRange	range	= { 0, len };
		CFIndex	nconv, nbuff;
		
		nconv = CFStringGetBytes(ref, range, encoding, 0, false, NULL, 0, &nbuff);
		
		if (nconv != len)
			B_THROW(CharacterEncodingException());
		else
			B_THROW(std::out_of_range("B::String::private_copy(StringPtr) buffer too small"));
	}
}

// ------------------------------------------------------------------------------------------
void
String::private_copy(
	CFStringRef			ref,		//!< The input string.
	std::vector<UInt8>&	blob,		//!< The output buffer.
	CFStringEncoding	encoding,	//!< The output string's encoding.
	bool				external,	//!< If @c true, blob get a leading BOM.
	char				loss_byte)	//!< The character to substitute for unconvertible characters.
{
	CFIndex	len	= CFStringGetLength(ref);
	CFIndex	nconv, nbuff;
	
	blob.resize(CFStringGetMaximumSizeForEncoding(len, encoding));
	
	nconv = CFStringGetBytes(ref, CFRangeMake(0, len), encoding, 
							 external, loss_byte, 
							 reinterpret_cast<UInt8*>(&blob[0]), blob.size(), 
							 &nbuff);
	
	B_THROW_IF(nconv != len, CharacterEncodingException());
	
	blob.resize(nbuff);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	const MutableString&	str)	//!< The input string.
{
	String	temp(str);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	const UniChar*	ustr)	//!< The input string.
{
	String	temp(ustr, npos, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	UniChar	c)	//!< The input character.
{
	String	temp(1, c, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	const std::wstring&	wstr)	//!< The input string.
{
	String	temp(wstr, 0, wstr.size(), get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	const wchar_t*	wcstr)	//!< The input string.
{
	String	temp(wcstr, npos, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	@note	The input string is assumed to be encoded as @c kCFStringEncodingASCII.
*/
String&
String::operator = (
	const std::string&	sstr)	//!< The input string.
{
	String	temp(sstr, kCFStringEncodingASCII, 0, sstr.size(), get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	@note	The input string is assumed to be encoded as @c kCFStringEncodingASCII.
*/
String&
String::operator = (
	const char*	cstr)	//!< The input string.
{
	String	temp(cstr, kCFStringEncodingASCII, npos, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	@note	The input string is assumed to be encoded as @c kCFStringEncodingASCII.
*/
String&
String::operator = (
	ConstStringPtr	pstr)	//!< The input string.
{
	String	temp(pstr, kCFStringEncodingASCII, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	const OSPtr<CFStringRef>&	cfstr)	//!< The input string.
{
	B_ASSERT(cfstr != NULL);
	
	CFStringRef	newRef;
	
	newRef = CFStringCreateCopy(CFGetAllocator(cfstr.get()), cfstr);
	B_THROW_IF_NULL(newRef);
	
	OSPtrOwnership::CFObjectOwnershipTrait::release(mRef);
	mRef = newRef;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String&
String::operator = (
	const OSPtr<CFMutableStringRef>&	cfstr)	//!< The input string.
{
	B_ASSERT(cfstr != NULL);
	
	CFStringRef	newRef;
	
	newRef = CFStringCreateCopy(CFGetAllocator(cfstr.get()), cfstr);
	B_THROW_IF_NULL(newRef);
	
	OSPtrOwnership::CFObjectOwnershipTrait::release(mRef);
	mRef = newRef;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a str starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a str.size().
*/
String&
String::assign(
	const String&	str,				//!< The input string.
	size_type		pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(str, pos, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a str starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a str.size().
*/
String&
String::assign(
	const MutableString&	str,				//!< The input string.
	size_type				pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type				n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(str, pos, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a cfstr to this string.  The input string's retain count is not incremented.
*/
String&
String::assign(
	CFStringRef				cfstr,	//!< The source string.
	const from_copy_t&		fc)		//!< An indication that @a cfstr comes from a Copy or Create function, and therefore that its retain count should not be increased.
{
	String	temp(cfstr, fc);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a cfstr to this string.  If @a cfstr is @c NULL, use an empty string instead.
*/
String&
String::assign(
	CFStringRef				cfstr,	//!< The source string.
	const std::nothrow_t&	nt)		//!< An indication that the function should not throw an exception if @cfstr is @c NULL.
{
	String	temp(cfstr, nt);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a cfstr to this string.  If @a cfstr is @c NULL, use an empty string instead.
 *	The input string's retain count is not incremented.
*/
String&
String::assign(
	CFStringRef				cfstr,	//!< The source string.
	const from_copy_t&		fc,		//!< An indication that @a cfstr comes from a Copy or Create function, and therefore that its retain count should not be increased.
	const std::nothrow_t&	nt)		//!< An indication that the function should not throw an exception if @cfstr is @c NULL.
{
	String	temp(cfstr, fc,  nt);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a cfstr starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @c CFStringGetLength(cfstr).
*/
String&
String::assign(
	CFStringRef		cfstr,				//!< The input string.
	size_type		pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(cfstr, pos, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a n characters of @a ustr.  If @a n is @c npos, then @a ustr is assumed to be 
	null-terminated.
*/
String&
String::assign(
	const UniChar*	ustr,				//!< The input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(ustr, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns @a num occurrences of character @a c.
*/
String&
String::assign(
	size_type	n,		//!< The number of occurrences of @a c.
	UniChar		c)		//!< The character to assign.
{
	String	temp(n, c, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a wstr starting with index @a pos.
	
	@exception	@c std::out_of_range	If @a pos > @a wstr.size().
*/
String&
String::assign(
	const std::wstring&	wstr,				//!< The input string.
	size_type			pos /* = 0 */,		//!< The index of the first character to assign from the input string.
	size_type			n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(wstr, pos, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a wcstr starting with index @a pos.  If @a n is 
	@c npos, then @a wcstr is assumed to be null-terminated.
*/
String&
String::assign(
	const wchar_t*	wcstr,				//!< The input string.
	size_type		n /* = npos */)		//!< The number of characters to assign.
{
	String	temp(wcstr, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a sstr starting with index @a pos.  The input string 
	is assumed to be encoded using the character encoding given in @a encoding.
	
	@exception	@c std::out_of_range	If @a pos > @a sstr.size().
*/
String&
String::assign(
	const std::string&	sstr,			//!< The input string.
	CFStringEncoding	encoding,		//!< The input string's encoding.
	size_type			pos /* = 0 */,	//!< The index of the first character to assign from the input string.
	size_type			n /* = npos */)	//!< The number of characters to assign.
{
	String	temp(sstr, encoding, pos, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns at most @a n characters of @a cstr starting with index @a pos.  If @a n is 
	@c npos, then @a cstr is assumed to be null-terminated.  The input string is assumed 
	to be encoded using the character encoding given in @a encoding.
*/
String&
String::assign(
	const char*			cstr,			//!< The input string.
	CFStringEncoding	encoding,		//!< The input string's encoding.
	size_type			n /* = npos */)	//!< The number of characters to assign.
{
	String	temp(cstr, encoding, n, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	The input string is assumed to be encoded using the character encoding given in 
	@a encoding.
*/
String&
String::assign(
	ConstStringPtr		pstr,		//!< The input string.
	CFStringEncoding	encoding)	//!< The input string's encoding.
{
	String	temp(pstr, encoding, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Assigns the string from @a blob.  The source string is assumed to be encoded using the 
	character encoding given in @a encoding.
*/
String&
String::assign(
	const std::vector<UInt8>&	blob,		//!< The source blob.
	CFStringEncoding			encoding,	//!< The character encoding of @a blob.
	bool						external)	//!< If true, checks for BOM and performs byte-swapping as necessary.
{
	String	temp(blob, encoding, external, get_allocator());
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find(CFStringRef ref, UniChar c, size_type pos, unsigned opts)
{
	String	temp(1, c);
	
	return (private_find(ref, temp.cf_ref(), pos, opts));
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find(CFStringRef ref, CFStringRef cfstr, size_type pos, unsigned opts)
{
	size_type	len	= CFStringGetLength(ref);
	CFRange		foundRange;
	size_type	foundPos;
	
	if (CFStringFindWithOptions(ref, cfstr, 
								CFRangeMake(pos, len-pos), 
								opts, 
								&foundRange))
	{
		foundPos = foundRange.location;
	}
	else
	{
		foundPos = npos;
	}
	
	return (foundPos);
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find(CFStringRef ref, const UniChar* ustr, size_type pos, size_type n, unsigned opts)
{
	String	temp(ustr, n);
	
	return (private_find(ref, temp.cf_ref(), pos, opts));
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find(CFStringRef ref, const std::string& sstr, CFStringEncoding encoding, size_type pos, unsigned opts)
{
	String	temp(sstr, encoding);
	
	return (private_find(ref, temp.cf_ref(), pos, opts));
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find_of(CFStringRef ref, CFStringRef cfstr, size_type pos, unsigned opts, bool negate)
{
	size_type					len	= CFStringGetLength(ref);
	OSPtr<CFCharacterSetRef>	charSet;
	CFRange						foundRange;
	size_type					foundPos;
	
	if (!negate)
	{
		charSet.reset(CFCharacterSetCreateWithCharactersInString(NULL, cfstr), from_copy);
	}
	else
	{
		OSPtr<CFMutableCharacterSetRef>	negCharSet;
		
		negCharSet.reset(CFCharacterSetCreateMutable(NULL), from_copy);
		CFCharacterSetAddCharactersInString(negCharSet, cfstr);
		CFCharacterSetInvert(negCharSet);
		
		charSet.reset(negCharSet.release(), from_copy);
	}
	
	if (CFStringFindCharacterFromSet(ref, charSet, CFRangeMake(pos, len-pos), 
									 opts, &foundRange))
	{
		foundPos = foundRange.location;
	}
	else
	{
		foundPos = npos;
	}
	
	return (foundPos);
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find_of(CFStringRef ref, const UniChar* ustr, size_type pos, size_type n, unsigned opts, bool negate)
{
	String	temp(ustr, n);
	
	return (private_find_of(ref, temp.cf_ref(), pos, opts, negate));
}

// ------------------------------------------------------------------------------------------
String::size_type
String::private_find_of(CFStringRef ref, const std::string& sstr, CFStringEncoding encoding, size_type pos, unsigned opts, bool negate)
{
	String	temp(sstr, encoding);
	
	return (private_find_of(ref, temp.cf_ref(), pos, opts, negate));
}

// ------------------------------------------------------------------------------------------
String::const_iterator
String::begin() const
{
	return (const_iterator(mRef, 0));
}

// ------------------------------------------------------------------------------------------
String::const_iterator
String::end() const
{
	return (const_iterator(mRef, CFStringGetLength(mRef)));
}

// ------------------------------------------------------------------------------------------
String::const_reverse_iterator
String::rbegin() const
{
	return (const_reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
String::const_reverse_iterator
String::rend() const
{
	return (const_reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
std::ostream&
String::Write(std::ostream& ostr, CFStringEncoding encoding) const
{
	const char*	cp	= CFStringGetCStringPtr(cf_ref(), encoding);
	
	if (cp != NULL)
	{
		ostr << cp;
	}
	else
	{
		std::string	sstr;
		
		copy(sstr, encoding);
		
		ostr << sstr;
	}
	
	return (ostr);
}

// ------------------------------------------------------------------------------------------
std::istream&
String::Read(std::istream& istr, CFStringEncoding encoding)
{
	std::string	sstr;
	
	istr >> sstr;
	
	assign(sstr, encoding);
	
	return (istr);
}

// ------------------------------------------------------------------------------------------
std::istream&
String::GetLine(std::istream& istr, char delim, CFStringEncoding encoding)
{
	std::string	sstr;
	
	getline(istr, sstr, delim);
	
	assign(sstr, encoding);
	
	return (istr);
}


// ------------------------------------------------------------------------------------------
String
operator + (
	const String&	s1, 
	const String&	s2)
{
	return (String(CFStringCreateWithFormat(s1.get_allocator(), NULL, CFSTR("%@%@"), 
										    s1.cf_ref(), s2.cf_ref()), 
				   from_copy));
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const String&	s1, 
	CFStringRef		s2)
{
	return (String(CFStringCreateWithFormat(s1.get_allocator(), NULL, CFSTR("%@%@"), 
										    s1.cf_ref(), s2), 
				   from_copy));
}

// ------------------------------------------------------------------------------------------
String
operator + (
	CFStringRef		s1, 
	const String&	s2)
{
	return (String(CFStringCreateWithFormat(s2.get_allocator(), NULL, CFSTR("%@%@"), 
										    s1, s2.cf_ref()), 
				   from_copy));
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const String&	s1, 
	const UniChar*	s2)
{
	String	temp(s2, String::npos);
	
	return (s1 + temp);
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const UniChar*	s1, 
	const String&	s2)
{
	String	temp(s1, String::npos, s2.get_allocator());
	
	return (temp + s2);
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const String&	s1, 
	const UniChar	c)
{
	String	temp(1, c);
	
	return (s1 + temp);
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const UniChar	c, 
	const String&	s2)
{
	String	temp(1, c, s2.get_allocator());
	
	return (temp + s2);
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const String&		s1, 
	const std::string&	s2)
{
	String	temp(s2, kCFStringEncodingASCII);
	
	return (s1 + temp);
}

// ------------------------------------------------------------------------------------------
String
operator + (
	const std::string&	s1, 
	const String&		s2)
{
	String	temp(s1, kCFStringEncodingASCII, 0, String::npos, s2.get_allocator());
	
	return (temp + s2);
}


}	// namespace B
