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

// file header
#include "BPreferences.h"

// system headers
#include <CoreFoundation/CFBundle.h>

// B headers
#include "BBundle.h"
#include "BString.h"


namespace B {

// ------------------------------------------------------------------------------------------
Preferences::Preferences()
    : mApplicationID(Bundle::Main().Identifier().cf_ptr())
{
}

// ------------------------------------------------------------------------------------------
Preferences::Preferences(
    const char* inApplicationID)
        : mApplicationID(String(inApplicationID, kCFStringEncodingASCII).cf_ptr())
{
}

// ------------------------------------------------------------------------------------------
Preferences::Preferences(
    const Bundle&   inBundle)
        : mApplicationID(inBundle.Identifier().cf_ptr())
{
}

// ------------------------------------------------------------------------------------------
OSPtr<CFTypeRef>
Preferences::GetCFType(
    const char*     inKey) const
{
    String  key(inKey, kCFStringEncodingASCII);
    
    return (OSPtr<CFTypeRef>(CFPreferencesCopyAppValue(key.cf_ref(), mApplicationID), 
                             from_copy, std::nothrow));
}

// ------------------------------------------------------------------------------------------
bool
Preferences::GetBool(
    const char*     inKey,
    bool            inDefaultValue) const
{
    return GetNumber<bool>(inKey, inDefaultValue);
}

// ------------------------------------------------------------------------------------------
void
Preferences::SetBool(
    const char* inKey,
    bool        inValue)
{
    SetCFType(inKey, inValue ? kCFBooleanTrue : kCFBooleanFalse);
}

// ------------------------------------------------------------------------------------------
B::String
Preferences::GetString(
    const char*     inKey,
    CFStringRef     inDefaultValue) const
{
    B_ASSERT(inDefaultValue != NULL);
    
    B::OSPtr<CFStringRef>   value   = GetPtr<CFStringRef>(inKey);
    
    if (value.get() == NULL)
        value.reset(inDefaultValue);
    
    return B::String(value);
}

// ------------------------------------------------------------------------------------------
void
Preferences::SetString(
    const char*         inKey,
    const B::String&    inValue)
{
    SetPtr(inKey, inValue.cf_ptr());
}

// ------------------------------------------------------------------------------------------
void
Preferences::SetString(
    const char*         inKey,
    const std::string&  inUtf8Value)
{
    SetString(inKey, B::String(inUtf8Value, kCFStringEncodingUTF8));
}

// ------------------------------------------------------------------------------------------
std::auto_ptr<std::istream>
Preferences::GetStream(
    const char*     inKey) const
{
    B::String   unicodeStr  = GetString(inKey, CFSTR(""));
    std::string utf8Str;
    
    unicodeStr.copy(utf8Str, kCFStringEncodingUTF8);
    
    return std::auto_ptr<std::istream>(new std::istringstream(utf8Str));
}

#if 0
// ------------------------------------------------------------------------------------------
template <> bool
Preferences::GetValue<bool>(
    const char*     inKey,
    bool            inDefaultValue) const
{
    bool    value   = inDefaultValue;
    
    if (!TypeRefToBool(GetCFType(inKey), value))
        value = inDefaultValue;
    
    return value;
}

// ------------------------------------------------------------------------------------------
template <> void
Preferences::SetValue<bool>(
    const char*     inKey,
    bool            inValue)
{
    SetCFType(inKey, inValue ? kCFBooleanTrue : kCFBooleanFalse);
}

// ------------------------------------------------------------------------------------------
template <> int
Preferences::GetValue<int>(
    const char* inKey,
    int         inDefaultValue) const
{
    int value   = inDefaultValue;
    
    if (!TypeRefToInt(GetCFType(inKey), value))
        value = inDefaultValue;
    
    return value;
}

// ------------------------------------------------------------------------------------------
template <> void
Preferences::SetValue<int>(
    const char* inKey,
    int         inValue)
{
    OSPtr<CFNumberRef>  numRef(CFNumberCreate(NULL, kCFNumberIntType, &inValue), from_copy);
    
    SetCFType(inKey, numRef.get());
}
#endif

// ------------------------------------------------------------------------------------------
OSPtr<CFTypeRef>
Preferences::GetCFTypeFromSource(
    const char*     inKey, 
    CFStringRef     inUser /* = kCFPreferencesCurrentUser */, 
    CFStringRef     inHost /* = kCFPreferencesCurrentHost */) const
{
    String  key(inKey, kCFStringEncodingASCII);
    
    return (OSPtr<CFTypeRef>(CFPreferencesCopyValue(key.cf_ref(), mApplicationID, inUser, inHost), from_copy));
}

// ------------------------------------------------------------------------------------------
void
Preferences::SetCFType(
    const char*     inKey, 
    CFTypeRef       inValue)
{
    String  key(inKey, kCFStringEncodingASCII);
    
    CFPreferencesSetAppValue(key.cf_ref(), inValue, mApplicationID);
}

// ------------------------------------------------------------------------------------------
void
Preferences::SetCFTypeInSource(
    const char*         inKey, 
    OSPtr<CFTypeRef>    inValue, 
    CFStringRef         inUser /* = kCFPreferencesCurrentUser */, 
    CFStringRef         inHost /* = kCFPreferencesCurrentHost */)
{
    String  key(inKey, kCFStringEncodingASCII);
    
    CFPreferencesSetValue(key.cf_ref(), inValue, mApplicationID, inUser, inHost);
}

// ------------------------------------------------------------------------------------------
bool
Preferences::IsKeyForced(
    const char*     inKey) const
{
    String  key(inKey, kCFStringEncodingASCII);
    
    return (CFPreferencesAppValueIsForced(key.cf_ref(), mApplicationID));
}

// ------------------------------------------------------------------------------------------
void
Preferences::Flush()
{
    CFPreferencesAppSynchronize(mApplicationID);
}

// ------------------------------------------------------------------------------------------
void
Preferences::FlushSource(
    CFStringRef     inUser /* = kCFPreferencesCurrentUser */, 
    CFStringRef     inHost /* = kCFPreferencesCurrentHost */)
{
    CFPreferencesSynchronize(mApplicationID, inUser, inHost);
}

}   // namespace B
