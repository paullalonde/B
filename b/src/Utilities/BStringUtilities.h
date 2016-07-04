// ==========================================================================================
//	
//	Copyright (C) 2005-2006 Paul Lalonde enrg.
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

#ifndef BStringUtilities_H_
#define BStringUtilities_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B {

std::wstring	make_wstring(
					CFStringRef			cfstr);
OSStatus		make_wstring(
					CFStringRef			cfstr, 
					std::wstring&		wstr, 
					const std::nothrow_t&);
std::wstring	make_wstring(
					const std::string&	str, 
					CFStringEncoding	encoding);

OSPtr<CFStringRef>
				make_cfstring(
					const std::wstring&	wstr);
OSStatus		make_cfstring(
					const std::wstring&	wstr, 
					OSPtr<CFStringRef>&	cfstr, 
					const std::nothrow_t&);
OSPtr<CFStringRef>
				make_cfstring(
					const std::string&	str, 
					CFStringEncoding	encoding);
OSPtr<CFStringRef>
				make_cfstring(
					OSType				type);

OSPtr<CFMutableStringRef>
				make_mutablecfstring(
					const std::wstring&	wstr);

std::string		make_string(
					const std::wstring&	wstr, 
					CFStringEncoding	encoding);
std::string		make_string(
					CFStringRef			cfstr, 
					CFStringEncoding	encoding);
std::string		make_string(
					OSType				type);

std::string		make_utf8_string(
					const std::wstring&	wstr);
std::string		make_utf8_string(
					CFStringRef			cfstr);

OSType			make_ostype(
					const std::wstring&	wstr);
OSType			make_ostype(
					CFStringRef			cfstr);

void			make_pstring(
					const std::wstring&	wstr, 
					StringPtr			pstr,
					size_t				psize,
					CFStringEncoding	encoding);
void			make_pstring(
					CFStringRef			cfstr, 
					StringPtr			pstr,
					size_t				psize,
					CFStringEncoding	encoding);

void			make_unichars(
					const std::wstring&		wstr, 
					std::vector<UniChar>&	vec);
void			make_unichars(
					CFStringRef				cfstr, 
					std::vector<UniChar>&	vec);

void			binary_to_base64(
					const void*			data,
					size_t				size,
					std::string&		str);
void			base64_to_binary(
					const char*			data, 
					size_t				size,
					std::vector<UInt8>&	vec);

}	// namespace B


#endif	// BStringUtilities_H_
