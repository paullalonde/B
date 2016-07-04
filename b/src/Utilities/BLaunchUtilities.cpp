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

// file header
#include "BLaunchUtilities.h"

// B headers
#include "BOSPtr.h"


namespace B { namespace LS {

OSPtr<CFStringRef>  DisplayNameForRef(const FSRef& inRef)
{
    CFStringRef cfref;
    OSStatus    err;
    
    err = LSCopyDisplayNameForRef(&inRef, &cfref);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CFStringRef>(cfref, from_copy));
}

OSPtr<CFStringRef>  DisplayNameForURL(CFURLRef inURL)
{
    CFStringRef cfref;
    OSStatus    err;
    
    err = LSCopyDisplayNameForURL(inURL, &cfref);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CFStringRef>(cfref, from_copy));
}

OSPtr<CFStringRef>  KindStringForRef(const FSRef& inRef)
{
    CFStringRef cfref;
    OSStatus    err;
    
    err = LSCopyKindStringForRef(&inRef, &cfref);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CFStringRef>(cfref, from_copy));
}

OSPtr<CFStringRef>  KindStringForURL(CFURLRef inURL)
{
    CFStringRef cfref;
    OSStatus    err;
    
    err = LSCopyKindStringForURL(inURL, &cfref);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CFStringRef>(cfref, from_copy));
}

OSPtr<CFStringRef>  KindStringForTypeInfo(OSType inType, OSType inCreator, CFStringRef inExtension)
{
    CFStringRef cfref;
    OSStatus    err;
    
    err = LSCopyKindStringForTypeInfo(inType, inCreator, inExtension, &cfref);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CFStringRef>(cfref, from_copy));
}

OSPtr<CFStringRef>  KindStringForMIMEType(CFStringRef inMIMEType)
{
    CFStringRef cfref;
    OSStatus    err;
    
    err = LSCopyKindStringForMIMEType(inMIMEType, &cfref);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CFStringRef>(cfref, from_copy));
}


} } // namespace LS / namespace B
