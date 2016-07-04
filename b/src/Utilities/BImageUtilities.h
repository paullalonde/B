// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
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

#ifndef BImageUtilities_H_
#define BImageUtilities_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

// forward declarations
class   Url;

//! High-level function creates a @c CGImageRef from the contents of any URL.
OSPtr<CGImageRef>   CreateImageFromURL(const Url& inUrl);

//! Creates a @c CGImageRef from @a inUrl, which is assumed to refer to a JPEG image.
OSPtr<CGImageRef>   CreateImageFromJpegURL(const Url& inUrl);
//! Creates a @c CGImageRef from @a inUrl, which is assumed to refer to a PNG image.
OSPtr<CGImageRef>   CreateImageFromPngURL(const Url& inUrl);
//! Creates a @c CGImageRef from @a inUrl, which is assumed to refer to a 'icns' image.
OSPtr<CGImageRef>   CreateImageFromIcnsURL(const Url& inUrl);
//! Creates a @c CGImageRef from the contents of an icon family.
OSPtr<CGImageRef>   CreateImageFromIconFamily(IconFamilyHandle inFamily, int inSizeHint);

}   // namespace B

#endif  // BImageUtilities_H_
