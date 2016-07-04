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

#ifndef BLaunchUtilities_H_
#define BLaunchUtilities_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B { namespace LS {

OSPtr<CFStringRef>  DisplayNameForRef(const FSRef& inRef);
OSPtr<CFStringRef>  DisplayNameForURL(CFURLRef inURL);
OSPtr<CFStringRef>  KindStringForRef(const FSRef& inRef);
OSPtr<CFStringRef>  KindStringForURL(CFURLRef inURL);
OSPtr<CFStringRef>  KindStringForTypeInfo(OSType inType, OSType inCreator, CFStringRef inExtension);
OSPtr<CFStringRef>  KindStringForMIMEType(CFStringRef inMIMEType);


} } // namespace LS / namespace B


#endif  // LSUtilities_H_
