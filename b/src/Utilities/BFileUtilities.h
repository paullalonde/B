// ==========================================================================================
//  
//  Copyright (C) 2005-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

#ifndef BFileUtilities_H_
#define BFileUtilities_H_

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

// forward class declarations
class   String;

void            RefToBlob(const FSRef& ref, std::vector<UInt8>& blob, 
                    const FSRef* baseRef = NULL);
void            AliasToBlob(AliasHandle aliasH, std::vector<UInt8>& blob);
bool            BlobToRef(const std::vector<UInt8>& blob, FSRef& ref, bool showUI = true, 
                        const FSRef* baseRef = NULL, bool* wasChanged = NULL);
AliasHandle     BlobToAlias(const std::vector<UInt8>& blob);

bool            GetFileNameExtension(
                    const String&   inName, 
                    String&         outExtension);
//! Returns the given file name's display name.  This does NOT require the file to exist.
String          GetDisplayName(
                    const String&   inName, 
                    bool            inHideExtension);

bool            DoesFinderAlwaysDisplayExtensions();

void            make_hfsstring(
                    CFStringRef         cfstr, 
                    HFSUniStr255&       hfsstr);
void            make_hfsstring(
                    const std::wstring& wstr, 
                    HFSUniStr255&       hfsstr);
void            make_hfsstring(
                    const std::string&  str, 
                    CFStringEncoding    encoding,
                    HFSUniStr255&       hfsstr);

}   // namespace B


#endif  // FileUtilities_H_
