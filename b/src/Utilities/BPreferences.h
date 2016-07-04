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

#ifndef BPreferences_H_
#define BPreferences_H_

#pragma once

// standard headers
#include <vector>

// system headers
#include <CoreFoundation/CFPreferences.h>

// library headers
#include "CFUtils.h"


namespace B {


// forward declarations
class   Bundle;


/*! @brief  Interface to CF preferences
    
    @todo   %Document this class!
*/
class Preferences
{
public:
    
    // types
    typedef std::vector<UInt8>  DataBlob;
    
    // constructors / destructor
            Preferences();
            Preferences(
                const char*         inApplicationID);
            Preferences(
                const Bundle&       inBundle);
    
    // Search for a pref value in the various sources of defaults.
    OSPtr<CFTypeRef>
                GetCFType(
                    const char*     inKey) const;
    template <typename T> OSPtr<T>
                GetPtr(
                    const char*     inKey) const;
    template <typename T> T
                GetNumber(
                    const char*     inKey, 
                    T               inDefaultValue) const;
    bool        GetData(
                    const char*     inKey, 
                    DataBlob&       outValue) const;
    bool        GetBool(
                    const char*     inKey, 
                    bool            inDefaultValue) const;
    B::String   GetString(
                    const char*     inKey, 
                    CFStringRef     inDefaultValue) const;
    std::auto_ptr<std::istream>
                GetStream(
                    const char*     inKey) const;
                    
    // Set a pref value in the default location.
    void        SetCFType(
                    const char*     inKey, 
                    CFTypeRef       inValue);
    template <typename T> void
                SetPtr(
                    const char*     inKey, 
                    OSPtr<T>        inValue);
    template <typename T> void
                SetNumber(
                    const char*     inKey, 
                    T               inValue);
    void        SetNumber(
                    const char*     inKey, 
                    bool            inValue);
    void        SetData(
                    const char*     inKey, 
                    const DataBlob& inValue);
    void        SetBool(
                    const char*     inKey, 
                    bool            inValue);
    void        SetString(
                    const char*     inKey, 
                    const B::String&    inValue);
    void        SetString(
                    const char*     inKey, 
                    const std::string&  inUtf8Value);
    
    // Is user authorized to change this pref?
    bool        IsKeyForced(
                    const char*     inKey) const;
    
    // Writing to disk.
    void        Flush();
    
    // Single-source variants.
    OSPtr<CFTypeRef>
                GetCFTypeFromSource(
                    const char*     inKey, 
                    CFStringRef     inUser = kCFPreferencesCurrentUser, 
                    CFStringRef     inHost = kCFPreferencesCurrentHost) const;
    template <typename T> OSPtr<T>
                GetPtrFromSource(
                    const char*     inKey, 
                    CFStringRef     inUser = kCFPreferencesCurrentUser, 
                    CFStringRef     inHost = kCFPreferencesCurrentHost) const;
    void        SetCFTypeInSource(
                    const char*     inKey, 
                    OSPtr<CFTypeRef> inValue, 
                    CFStringRef     inUser = kCFPreferencesCurrentUser, 
                    CFStringRef     inHost = kCFPreferencesCurrentHost);
    template <typename T> void
                SetPtrInSource(
                    const char*     inKey, 
                    OSPtr<T>        inValue, 
                    CFStringRef     inUser = kCFPreferencesCurrentUser, 
                    CFStringRef     inHost = kCFPreferencesCurrentHost);
    void        FlushSource(
                    CFStringRef     inUser = kCFPreferencesCurrentUser, 
                    CFStringRef     inHost = kCFPreferencesCurrentHost);
    
private:
    
    OSPtr<CFStringRef>  mApplicationID;
};


// ------------------------------------------------------------------------------------------
template <typename T> inline OSPtr<T>
Preferences::GetPtr(
    const char*     inKey) const
{
    return (CFUMakeTypedValue<T>(GetCFType(inKey)));
}

// ------------------------------------------------------------------------------------------
template <typename T> T
Preferences::GetNumber(
    const char* inKey,
    T           inDefaultValue) const
{
    T   value   = inDefaultValue;
    
    if (!TypeRefToNumber(GetCFType(inKey), value))
        value = inDefaultValue;
    
    return value;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
Preferences::SetPtr(
    const char*     inKey, 
    OSPtr<T>        inValue)
{
    return (SetCFType(inKey, inValue.get()));
}

// ------------------------------------------------------------------------------------------
template <typename T> void
Preferences::SetNumber(
    const char* inKey,
    T           inValue)
{
    SetCFType(inKey, NumberToNumberRef(inValue).get());
}

// ------------------------------------------------------------------------------------------
inline void
Preferences::SetNumber(
    const char* inKey,
    bool        inValue)
{
    SetBool(inKey, inValue);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline OSPtr<T>
Preferences::GetPtrFromSource(
    const char*     inKey, 
    CFStringRef     inUser /* = kCFPreferencesCurrentUser */, 
    CFStringRef     inHost /* = kCFPreferencesCurrentHost */) const
{
    return (CFUMakeTypedValue<T>(GetCFTypeFromSource(inKey, inUser, inHost)));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
Preferences::SetPtrInSource(
    const char*     inKey, 
    OSPtr<T>        inValue, 
    CFStringRef     inUser /* = kCFPreferencesCurrentUser */, 
    CFStringRef     inHost /* = kCFPreferencesCurrentHost */)
{
    return (SetCFTypeInSource(inKey, OSPtr<CFTypeRef>(inValue.Get(), false), inUser, inHost));
}


}   // namespace B


#endif  // BPreferences_H_
