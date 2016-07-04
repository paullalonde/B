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

#ifndef BAEReader_H_
#define BAEReader_H_

#pragma once

// standard headers
#include <stack>
#include <vector>

// library headers
#include "BAEDescParam.h"


namespace B {


// ==========================================================================================
//  AEReader

/*! @brief  Helper class for reading from Apple %Event descriptors.
    
    @todo   %Document this class!
*/
class AEReader : boost::noncopyable
{
public:
    
    // constructor / destructor
    AEReader(const AEDesc& inDesc);
    AEReader(const std::vector<UInt8>& inBuffer);
    ~AEReader();
    
    // inquiries
    DescType    GetType() const;
    DescType    GetTypeKey(AEKeyword key) const;
    DescType    GetTypeNth(long index) const;
    bool        IsRecord() const;
    bool        IsList() const;
    bool        Contains(AEKeyword key) const;
    size_t      Count() const;
    size_t      Size() const;
    
    // opening a descriptor
    void    OpenDescKey(AEKeyword key, DescType type = typeWildCard);
    void    OpenDescNth(long index, DescType type = typeWildCard, AEKeyword* key = NULL);
    
    // opening a descriptor, no-throw variant
    bool    OpenDescKey(AEKeyword key, const std::nothrow_t&, DescType type = typeWildCard);
    bool    OpenDescNth(long index, const std::nothrow_t&, DescType type = typeWildCard, AEKeyword* key = NULL);
    
    void    CloseDesc();
    
    // reading a value
    void    Read(void* ptr, size_t size);
    template <DescType TYPE> typename DescParam<TYPE>::ValueType&
            Read(typename DescParam<TYPE>::ValueType& value);
    template <DescType TYPE>
    bool    Read(typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&);
    
    // reading a value from a descriptor record
    template <DescType TYPE> typename DescParam<TYPE>::ValueType&
            ReadKey(AEKeyword key, typename DescParam<TYPE>::ValueType& value);
    template <DescType TYPE>
    bool    ReadKey(AEKeyword key, typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&);
    
    // reading a value from a descriptor list
    template <DescType TYPE> typename DescParam<TYPE>::ValueType&
            ReadNth(long index, typename DescParam<TYPE>::ValueType& value);
    template <DescType TYPE>
    bool    ReadNth(long index, typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&);
    
    // reading a descriptor
    template <DescType TYPE> AEDesc&
            ReadDescKey(AEKeyword key, AEDesc& desc);
    template <DescType TYPE>
    bool    ReadDescKey(AEKeyword key, AEDesc& desc, const std::nothrow_t&);
    
    // reading an attribute (AppleEvents only)
    template <DescType TYPE> typename DescParam<TYPE>::ValueType&
            ReadAttribute(AEKeyword key, typename DescParam<TYPE>::ValueType& value);
    template <DescType TYPE>
    bool    ReadAttribute(AEKeyword key, typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&);
    
private:
    
    OSStatus    PrivateOpen(AEKeyword key, long index, AEKeyword* keyptr, DescType type);
    OSStatus    PrivateReadDesc(AEKeyword key, long index, DescType type, AEDesc& desc);
    OSStatus    PrivateReadAttribute(AEKeyword key, DescType type, void* data, size_t size);
    
    // member variables
    const AEDesc&       mBaseDesc;
    std::stack<AEDesc>  mOpenDescs;
    const AEDesc*       mCurrDesc;
    AEDesc              mBuffDesc;
};

// ------------------------------------------------------------------------------------------
inline DescType
AEReader::GetType() const
{
    return (mCurrDesc->descriptorType);
}

// ------------------------------------------------------------------------------------------
inline bool
AEReader::IsRecord() const
{
    return (AECheckIsRecord(mCurrDesc));
}

// ------------------------------------------------------------------------------------------
inline bool
AEReader::IsList() const
{
    return (mCurrDesc->descriptorType == typeAEList);
}

// ------------------------------------------------------------------------------------------
inline size_t
AEReader::Size() const
{
    return (AEGetDescDataSize(mCurrDesc));
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline
typename DescParam<TYPE>::ValueType&
AEReader::Read(typename DescParam<TYPE>::ValueType& value)
{
    DescParam<TYPE>::Get(*mCurrDesc, value);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline
typename DescParam<TYPE>::ValueType&
AEReader::ReadKey(AEKeyword key, typename DescParam<TYPE>::ValueType& value)
{
    DescParamHelper::ReadKey<TYPE>(*mCurrDesc, key, value);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline
typename DescParam<TYPE>::ValueType&
AEReader::ReadNth(long index, typename DescParam<TYPE>::ValueType& value)
{
    DescParamHelper::ReadParam<TYPE>(*mCurrDesc, index, value);
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline bool
AEReader::Read(typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&)
{
    return (DescParam<TYPE>::Get(*mCurrDesc, value) == noErr);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline bool
AEReader::ReadKey(AEKeyword key, typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&)
{
    return (DescParamHelper::ReadKey<TYPE>(*mCurrDesc, key, value, std::nothrow) == noErr);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline bool
AEReader::ReadNth(long index, typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&)
{
    return (DescParamHelper::ReadParam<TYPE>(*mCurrDesc, index, value, std::nothrow) == noErr);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline AEDesc&
AEReader::ReadDescKey(AEKeyword key, AEDesc& desc)
{
    B_THROW_IF_STATUS(PrivateReadDesc(key, 0, TYPE, desc));
    
    return (desc);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline bool
AEReader::ReadDescKey(AEKeyword key, AEDesc& desc, const std::nothrow_t&)
{
    return (PrivateReadDesc(key, 0, TYPE, desc) == noErr);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline
typename DescParam<TYPE>::ValueType&
AEReader::ReadAttribute(AEKeyword key, typename DescParam<TYPE>::ValueType& value)
{
    B_THROW_IF_STATUS(PrivateReadAttribute(key, TYPE, &value, sizeof(value)));
    
    return (value);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline bool
AEReader::ReadAttribute(AEKeyword key, typename DescParam<TYPE>::ValueType& value, const std::nothrow_t&)
{
    return (PrivateReadAttribute(key, TYPE, &value, sizeof(value)) == noErr);
}


// ==========================================================================================
//  AutoAEReaderDescKey

/*! @brief  Stack-based class that opens a descriptor from a record in an AEReader.
    
    @todo   %Document this class!
*/
class AutoAEReaderDescKey
{
public:
    
    AutoAEReaderDescKey(AEReader& reader, AEKeyword key, DescType type = typeWildCard);
    ~AutoAEReaderDescKey();
    
private:
    
    AEReader&   mReader;
};


// ==========================================================================================
//  AutoAEReaderDescNth

/*! @brief  Stack-based class that opens a descriptor from a list in an AEReader.
    
    @todo   %Document this class!
*/
class AutoAEReaderDescNth
{
public:
    
    AutoAEReaderDescNth(AEReader& reader, long index, DescType type = typeWildCard);
    ~AutoAEReaderDescNth();
    
    AEKeyword   GetKeyword() const  { return mKeyword; }
    
private:
    
    AEReader&   mReader;
    AEKeyword   mKeyword;
};


}   // namespace B

#endif  // BAEReader_H_
