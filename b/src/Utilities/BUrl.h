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

#ifndef BUrl_H_
#define BUrl_H_

#pragma once

// system headers
#include <CoreFoundation/CFURL.h>

// library headers
#include "BOSPtr.h"


namespace B {


// forward declarations
class	String;


/*!	@brief	Wrapper around a CoreFoundation URL.
	
	@todo	%Document this class!
*/
class Url
{
public:
	
	// constructors / destructor
				Url();
				Url(const Url& url);
	explicit	Url(OSPtr<CFURLRef> cfurl);
	explicit	Url(
					const String&				inString);
	explicit	Url(
					const String&				inString, 
					const Url&					inBase);
	explicit	Url(
					const std::string&			inData, 
					const Url*					inBase = NULL);
	explicit	Url(
					const std::vector<UInt8>&	inData, 
					CFStringEncoding			inEncoding = kCFStringEncodingUTF8, 
					const Url*					inBase = NULL);
				~Url();
	
	// assignment
	Url&	operator = (const Url& url);
	Url&	operator = (OSPtr<CFURLRef> cfurl);
	Url&	operator = (const String& str);
	Url&	Assign(const Url& url);
	Url&	Assign(OSPtr<CFURLRef> cfurl);
	Url&	Assign(
				const String&				inString);
	Url&	Assign(
				const String&				inString, 
				const Url&					inBase);
	Url&	Assign(
				const std::string&			inData, 
				const Url*					inBase = NULL);
	Url&	Assign(
				const std::vector<UInt8>&	inData, 
				CFStringEncoding			inEncoding = kCFStringEncodingUTF8, 
				const Url*					inBase = NULL);
	void	Clear();
	
	bool	Empty() const;
	bool	IsStandard() const;							// conforms to RFC 1808
	String	GetString() const;
	bool	GetBase(Url& outBase) const;
	bool	GetScheme(String& outScheme) const;
	bool	GetSchemeSpecific(String& outSchemeSpecific) const;	// everything after the scheme
	bool	GetAuthority(String& outAuthority) const;	// user:password@host:port
	bool	GetUser(String& outUser) const;
	bool	GetPassword(String& outPassword) const;
	bool	GetHost(String& outHost) const;
	bool	GetPort(int& outPort) const;
	bool	GetPath(String& outPath, bool& outAbsolute) const;
	bool	GetPathExtension(String& outExtension) const;
	bool	GetParameters(String& outParameters) const;
	bool	GetQuery(String& outQuery) const;
	bool	GetFragment(String& outFragment) const;
	
	Url		Absolute() const;
	Url		PushPath(const String& name, bool is_dir) const;
	Url		PopPath() const;
	String	TopPath() const;
	
	// conversions
	CFURLRef	cf_ref() const		{ return (mRef); }
	void		Copy(
					std::string&		outData, 
					bool				inEscapeWhitespace) const;
	void		Copy(
					std::vector<UInt8>&	outData, 
					bool				inEscapeWhitespace, 
					CFStringEncoding	inEncoding = kCFStringEncodingUTF8) const;
	
	// comparisons
	bool	operator == (const Url& url) const;
	bool	operator != (const Url& url) const;
	
	static String	Escape(
						const String&		inString, 
						CFStringRef			inCharactersToLeaveUnescaped = NULL, 
						CFStringRef			inLegalURLCharactersToBeEscaped = NULL, 
						CFStringEncoding	inEncoding = kCFStringEncodingUTF8);
	static String	Unescape(
						const String&		inString, 
						CFStringRef			inCharactersToLeaveEscaped = CFSTR(""));
	
	// ------------------------
	// File URLs
	// ------------------------
	
	explicit	Url(const FSRef& fileRef);
	explicit	Url(
					const B::String&	inPath, 
					bool				inIsDirectory,
					CFURLPathStyle		inPathStyle = kCFURLPOSIXPathStyle);
	
	Url&		operator = (const FSRef& fileRef);
	Url&		Assign(const FSRef& fileRef);
	Url&		AssignPath(
					const B::String&	inPath, 
					bool				inIsDirectory,
					CFURLPathStyle		inPathStyle = kCFURLPOSIXPathStyle);
	
	bool		IsFileUrl() const;
	bool		IsPathDirectory() const;
	
	String		GetFilename() const;
	
	void		Copy(FSRef& fileRef) const;
	bool		Copy(FSRef& fileRef, const std::nothrow_t&) const;
	void		Copy(FSRef& dirRef, HFSUniStr255& fileName) const;
	void		CopyPath(std::string& path) const;
	
	//! Returns the URL corresponding to @a folder in @a domain.
	static Url	Find(short domain, OSType folder, bool create);
	//! Returns the URL corresponding to @a folder in @a domain.
	static bool	Find(short domain, OSType folder, bool create, Url& url);
	
private:
	
	// member variables
	CFURLRef	mRef;
};

// ------------------------------------------------------------------------------------------
inline Url&
Url::operator = (const Url& url)
{
	return (Assign(url));
}

// ------------------------------------------------------------------------------------------
inline Url&
Url::operator = (OSPtr<CFURLRef> cfurl)
{
	return (Assign(cfurl));
}

// ------------------------------------------------------------------------------------------
inline Url&
Url::operator = (const String& str)
{
	return (Assign(str));
}

// ------------------------------------------------------------------------------------------
inline Url&
Url::operator = (const FSRef& fileRef)
{
	return (Assign(fileRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Url::Copy(FSRef& fileRef, const std::nothrow_t&) const
{
	return (CFURLGetFSRef(mRef, &fileRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Url::IsPathDirectory() const
{
	return (CFURLHasDirectoryPath(mRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Url::operator == (const Url& url) const
{
	return (CFEqual(mRef, url.mRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Url::operator != (const Url& url) const
{
	return (!CFEqual(mRef, url.mRef));
}


}	// namespace B


#endif	// BUrl_H_
