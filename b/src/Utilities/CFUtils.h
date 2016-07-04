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

#ifndef CFUtils_H_
#define CFUtils_H_

#pragma once

// system headers
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFString.h>

// B headers
#include "BErrorHandler.h"
#include "BOSPtr.h"


// CFUGetType returns the CFTypeID of the argument's type.
template <typename T> CFTypeID  CFUGetType();


// CFUGet accesses a member of a collection in a type-safe way.  It's assumed that the 
// collections do not contain any NULL elements.  This is true for the "usual" collections 
// used by system software, property lists for example.

template <typename T> bool
CFUGet(CFDictionaryRef coll, CFStringRef key, B::OSPtr<T>& valT)
{
    bool        good    = false;
    CFTypeRef   value;
    
    value = CFDictionaryGetValue(coll, key);
    
    if ((value != NULL) && (CFGetTypeID(value) == CFUGetType<T>()))
    {
        valT.reset(reinterpret_cast<T>(const_cast<void*>(value)));
        good = true;
    }
    
    return (good);
}

template <typename T> B::OSPtr<T>
CFUGet(CFDictionaryRef coll, CFStringRef key)
{
    CFTypeRef   value;
    
    value = CFDictionaryGetValue(coll, key);
    B_THROW_IF_NULL(value);
    
    if (CFGetTypeID(value) != CFUGetType<T>())
        throw std::bad_cast();
    
    return (B::OSPtr<T>(reinterpret_cast<T>(const_cast<void*>(value))));
}

template <typename T> B::OSPtr<T>
CFUGet(CFDictionaryRef coll, CFStringRef key, const std::nothrow_t&)
{
    CFTypeRef   value;
    B::OSPtr<T> valT;
    
    value = CFDictionaryGetValue(coll, key);
    
    if ((value != NULL) && (CFGetTypeID(value) == CFUGetType<T>()))
    {
        valT.reset(reinterpret_cast<T>(const_cast<void*>(value)));
    }
    
    return (valT);
}

template <typename T> bool
CFUGet(CFArrayRef coll, CFIndex key, B::OSPtr<T>& valT)
{
    bool        good    = false;
    CFTypeRef   value;
    
    value = CFArrayGetValueAtIndex(coll, key);
    
    if ((value != NULL) && (CFGetTypeID(value) == CFUGetType<T>()))
    {
        valT.reset(reinterpret_cast<T>(const_cast<void*>(value)));
        good = true;
    }
    
    return (good);
}

template <typename T> B::OSPtr<T>
CFUGet(CFArrayRef coll, CFIndex key)
{
    CFTypeRef   value;
    
    value = CFArrayGetValueAtIndex(coll, key);
    B_THROW_IF_NULL(value);
    
    if (CFGetTypeID(value) != CFUGetType<T>())
        throw std::bad_cast();
    
    return (B::OSPtr<T>(reinterpret_cast<T>(const_cast<void*>(value))));
}

template <typename T> B::OSPtr<T>
CFUGet(CFArrayRef coll, CFIndex key, const std::nothrow_t&)
{
    CFTypeRef   value;
    B::OSPtr<T> valT;
    
    value = CFArrayGetValueAtIndex(coll, key);
    
    if ((value != NULL) && (CFGetTypeID(value) == CFUGetType<T>()))
    {
        valT.reset(reinterpret_cast<T>(const_cast<void*>(value)));
    }
    
    return (valT);
}

// Specialisations of CFUGet
template <> bool    CFUGet(CFDictionaryRef coll, CFStringRef key, B::OSPtr<CFTypeRef>& valT);
template <> bool    CFUGet(CFArrayRef coll, CFIndex index, B::OSPtr<CFTypeRef>& valT);

template <typename T> T     CFUGetNumber(CFDictionaryRef coll, CFStringRef key);
template <typename T> bool  CFUGetNumber(CFDictionaryRef coll, CFStringRef key, T& value);
template <typename T> T     CFUGetNumber(CFArrayRef coll, CFIndex index);
template <typename T> bool  CFUGetNumber(CFArrayRef coll, CFIndex index, T& value);

// type conversions

template <typename T> B::OSPtr<T>
CFUMakeTypedValue(CFTypeRef inValue)
{
    B_ASSERT((inValue == NULL) || (CFGetTypeID(inValue) == CFUGetType<T>()));
    
    return (B::OSPtr<T>(reinterpret_cast<T>(inValue), std::nothrow));
}

template <> inline B::OSPtr<CFTypeRef>
CFUMakeTypedValue(CFTypeRef inValue)
{
    return (B::OSPtr<CFTypeRef>(inValue, std::nothrow));
}

/*!
    @brief  Maps a C/C++ numeric type to a @c CFNumberType.
*/
template <typename VALUE> struct NumberRefTraits
{
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef anything            VALUE;          //!< The template parameter.  Should be an numeric type.
    static const CFNumberType   kNumberType;    //!< A @c CFNumberType.
    //@}
#endif
};

template <> struct NumberRefTraits<char>        { static const CFNumberType kNumberType = kCFNumberCharType; };
template <> struct NumberRefTraits<short>       { static const CFNumberType kNumberType = kCFNumberShortType; };
template <> struct NumberRefTraits<int>         { static const CFNumberType kNumberType = kCFNumberIntType; };
template <> struct NumberRefTraits<long>        { static const CFNumberType kNumberType = kCFNumberLongType; };
template <> struct NumberRefTraits<long long>   { static const CFNumberType kNumberType = kCFNumberLongLongType; };
template <> struct NumberRefTraits<float>       { static const CFNumberType kNumberType = kCFNumberFloatType; };
template <> struct NumberRefTraits<double>      { static const CFNumberType kNumberType = kCFNumberDoubleType; };
template <> struct NumberRefTraits<SInt8>       { static const CFNumberType kNumberType = kCFNumberSInt8Type; };
#if 0   // Some CFNumberType values map to the same language type. 
template <> struct NumberRefTraits<SInt16>      { static const CFNumberType kNumberType = kCFNumberSInt16Type; };
template <> struct NumberRefTraits<SInt32>      { static const CFNumberType kNumberType = kCFNumberSInt32Type; };
template <> struct NumberRefTraits<SInt64>      { static const CFNumberType kNumberType = kCFNumberSInt64Type; };
template <> struct NumberRefTraits<Float32>     { static const CFNumberType kNumberType = kCFNumberFloat32Type; };
template <> struct NumberRefTraits<Float64>     { static const CFNumberType kNumberType = kCFNumberFloat64Type; };
template <> struct NumberRefTraits<CFIndex>     { static const CFNumberType kNumberType = kCFNumberCFIndexType; };
#endif

B::OSPtr<CFDataRef>         BlobToData(const std::vector<UInt8>& blob);
void                        DataToBlob(B::OSPtr<CFDataRef> data, std::vector<UInt8>& blob);
template <typename T> bool  TypeRefToNumber(CFTypeRef typeRef, T& value);
template <> bool            TypeRefToNumber(CFTypeRef typeRef, bool& value);

template <typename T> B::OSPtr<CFNumberRef> NumberToNumberRef(T value);
B::OSPtr<CFBooleanRef>                      NumberToNumberRef(bool value);

// ------------------------------------------------------------------------------------------
template <typename T> bool
TypeRefToNumber(CFTypeRef typeRef, T& value)
{
    bool    good    = false;
    
    value = 0;
    
    if ((typeRef != NULL) && (CFGetTypeID(typeRef) == CFUGetType<CFNumberRef>()))
    {
        CFNumberRef numberRef   = reinterpret_cast<CFNumberRef>(typeRef);
        
        good = CFNumberGetValue(numberRef, NumberRefTraits<T>::kNumberType, &value);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
template <typename T> B::OSPtr<CFNumberRef>
NumberToNumberRef(T value)
{
    return B::OSPtr<CFNumberRef>(CFNumberCreate(
                                    NULL, NumberRefTraits<T>::kNumberType, &value),
                                 B::from_copy);
}

// ------------------------------------------------------------------------------------------
inline B::OSPtr<CFBooleanRef>
NumberToNumberRef(bool value)
{
    return B::OSPtr<CFBooleanRef>(value ? kCFBooleanTrue : kCFBooleanFalse);
}

// ------------------------------------------------------------------------------------------
template <typename T> T
CFUGetNumber(CFDictionaryRef coll, CFStringRef key)
{
    T   value;
    
    if (!TypeRefToNumber(CFDictionaryGetValue(coll, key), value))
        B_THROW_STATUS(paramErr);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <typename T> bool
CFUGetNumber(CFDictionaryRef coll, CFStringRef key, T& value)
{
    return (TypeRefToNumber(CFDictionaryGetValue(coll, key), value));
}

// ------------------------------------------------------------------------------------------
template <typename T> T
CFUGetNumber(CFArrayRef coll, CFIndex index)
{
    T   value;
    
    if (!TypeRefToNumber(CFArrayGetValueAtIndex(coll, index), value))
        B_THROW_STATUS(paramErr);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <typename T> bool
CFUGetNumber(CFArrayRef coll, CFIndex index, T& value)
{
    return (TypeRefToNumber(CFArrayGetValueAtIndex(coll, index), value));
}

// localisation

#define CFUSTR(s)   (B::OSPtr<CFStringRef>(CFSTR(s)))


#endif  // CFUtils_H_
