// ==========================================================================================
//  
//  Copyright (C) 2006 Paul Lalonde enrg.
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
#include "BAEToken.h"

// B headers
#include "BAEDescriptor.h"
#include "BAEObject.h"
#include "BException.h"


namespace B {

int AEToken::sObjectCount   = 0;

// ------------------------------------------------------------------------------------------
AEToken::AEToken()
{
    AEDesc  nullDesc    = { typeNull, NULL };
    
    InitToken(nullDesc);
}

// ------------------------------------------------------------------------------------------
AEToken::AEToken(const AEAutoTokenDescriptor& inDescriptor)
{
    InitToken(inDescriptor);
}

// ------------------------------------------------------------------------------------------
AEToken::AEToken(const AEDesc& inDescriptor)
{
    InitToken(inDescriptor);
}

// ------------------------------------------------------------------------------------------
AEToken::AEToken(AEObjectPtr inObject)
{
    AEDesc  nullDesc    = { typeNull, NULL };
    
    InitToken(nullDesc);
    SetObject(inObject);
}

// ------------------------------------------------------------------------------------------
AEToken::AEToken(AEObjectPtr inObject, DescType inPropertyName)
{
    AEDesc  nullDesc    = { typeNull, NULL };
    
    InitToken(nullDesc);
    SetProperty(inObject, inPropertyName);
}

// ------------------------------------------------------------------------------------------
AEToken::~AEToken()
{
    DeleteTokenData(mOwned, mData);
}

// ------------------------------------------------------------------------------------------
void
AEToken::InitToken(const AEDesc& inDescriptor)
{
    OSStatus    err;
    
    switch (inDescriptor.descriptorType)
    {
    case kBObjectToken:
        err = ReadTokenData(inDescriptor, mBuffer, mData);
        B_THROW_IF_STATUS(err);
        mOwned = false;
        break;
        
    case typeNull:
        mData = NULL;
        mOwned = true;
        break;
    
    default:
        B_THROW_STATUS(paramErr);
        break;
    }
}

// ------------------------------------------------------------------------------------------
bool
AEToken::IsTokenDescriptor(const AEDesc& inDescriptor)
{
    switch (inDescriptor.descriptorType)
    {
    case kBObjectToken:
    case typeNull:
        return true;
    
    default:
        return false;
    }
}

// ------------------------------------------------------------------------------------------
DescType
AEToken::GetContainerClass(const AEDesc& inDescriptor)
{
    DescType    containerClassID;
    
    switch (inDescriptor.descriptorType)
    {
    case typeNull:
        containerClassID = AEObject::GetDefaultObject()->GetClassID();
        break;
    
    default:
        containerClassID = inDescriptor.descriptorType;
        break;
    }
    
    return containerClassID;
}

// ------------------------------------------------------------------------------------------
DescType
AEToken::GetObjectClassID(const AEDesc& inDescriptor)
{
    DescType    objectClassID   = 0;
    TokenData*  tokenData;
    char        buffer[sizeof(TokenData)];
    OSStatus    err;
    
    switch (inDescriptor.descriptorType)
    {
    case kBObjectToken:
        err = ReadTokenData(inDescriptor, buffer, tokenData);
        B_THROW_IF_STATUS(err);
        objectClassID = tokenData->obj->GetClassID();
        break;
        
    case typeNull:
        objectClassID = AEObject::GetDefaultObject()->GetClassID();
        break;
    
    default:
        B_THROW_STATUS(paramErr);
        break;
    }
    
    return objectClassID;
}

// ------------------------------------------------------------------------------------------
bool
AEToken::IsPropertyToken() const
{
    if (mData != NULL)
        return (mData->pname != 0);
    else
        return false;
}

// ------------------------------------------------------------------------------------------
AEObjectPtr
AEToken::GetObject() const
{
    if (mData != NULL)
        return mData->obj;
    else
        return AEObject::GetDefaultObject();
}

// ------------------------------------------------------------------------------------------
DescType
AEToken::GetPropertyName() const
{
    if (mData != NULL)
        return mData->pname;
    else
        return 0;
}

// ------------------------------------------------------------------------------------------
void
AEToken::SetToken(AEObjectPtr inObject, DescType inPropertyName)
{
    B_THROW_IF(!mOwned, std::logic_error("attempting to modify non-owned AEToken"));
    B_THROW_IF(inObject.get() == NULL, std::runtime_error("attempting to set null object in token"));
    
    if (mData == NULL)
        mData = new (mBuffer) TokenData;
    
    mData->obj      = inObject;
    mData->pname    = inPropertyName;
}

// ------------------------------------------------------------------------------------------
void
AEToken::SetObject(AEObjectPtr inObject)
{
    SetToken(inObject, 0);
}

// ------------------------------------------------------------------------------------------
void
AEToken::SetProperty(AEObjectPtr inObject, DescType inPropertyName)
{
    B_ASSERT(inPropertyName != 0);
    
    SetToken(inObject, inPropertyName);
}

// ------------------------------------------------------------------------------------------
void
AEToken::SetProperty(const AEToken& inSourceToken, DescType inPropertyName)
{
    B_ASSERT(!inSourceToken.IsPropertyToken());
    B_ASSERT(inPropertyName != 0);
    
    SetToken(inSourceToken.GetObject(), inPropertyName);
}

// ------------------------------------------------------------------------------------------
void
AEToken::Clear()
{
    B_THROW_IF(!mOwned, std::logic_error("attempting to modify non-owned AEToken"));
    
    AEDesc  nullDesc    = { typeNull, NULL };
    
    DeleteTokenData(mOwned, mData);
    InitToken(nullDesc);
}

#if 0
// ------------------------------------------------------------------------------------------
void
AEToken::Resolve(const AEDesc& inObject, short flags)
{
    B_THROW_IF_STATUS(Resolve(inObject, flags, std::nothrow));
}

// ------------------------------------------------------------------------------------------
OSStatus
AEToken::Resolve(const AEDesc& inObject, short flags, const std::nothrow_t&)
{
    B_THROW_IF(!mOwned, std::logic_error("attempting to modify non-owned AEToken"));
    
    AEDescriptor    tokenDesc;
    OSStatus        err;
    
    err = AEResolve(&inObject, flags, tokenDesc);
    
    if (err == noErr)
    {
        err = ReadTokenData(tokenDesc, mData);
    }
    
    return err;
}
#endif

// ------------------------------------------------------------------------------------------
void
AEToken::Commit(AEDesc& outDescriptor)
{
    B_THROW_IF(!mOwned, std::logic_error("attempting to modify non-owned AEToken"));
    
    OSStatus    err;
    
    err = AEDisposeDesc(&outDescriptor);
    B_THROW_IF_STATUS(err);
    
    TokenData*  tokenData;
    char        buffer[sizeof(TokenData)];
    
    if (mData != NULL)
    {
        tokenData = mData;
    }
    else
    {
        tokenData = new (buffer) TokenData(GetObject(), 0);
    }
    
    err = AECreateDesc(kBObjectToken, tokenData, sizeof(TokenData), &outDescriptor);
    B_THROW_IF_STATUS(err);
    
    mOwned  = false;
    mData   = NULL;
    
    ++sObjectCount;

#if 0
printf("AEToken::Commit %d\n", sObjectCount);
#endif
}

// ------------------------------------------------------------------------------------------
OSStatus
AEToken::Dispose(AEDesc& ioTokenDesc)
{
    TokenData*  tokenData;
    char        buffer[sizeof(TokenData)];
    OSStatus    err;
    
    switch (ioTokenDesc.descriptorType)
    {
    case typeAEList:
        {
            long    count;
            
            err = AECountItems(&ioTokenDesc, &count);
            
            if (err == noErr)
            {
                for (long i = 1; i <= count; i++)
                {
                    AEDesc  itemDesc;
                    
                    err = AEGetNthDesc(&ioTokenDesc, i, typeWildCard, NULL, &itemDesc);
                    if (err != noErr)
                        break;
                    
                    err = Dispose(itemDesc);
                    if (err != noErr)
                        break;
                }
            }
            
        }
        break;
        
    case kBObjectToken:
        err = ReadTokenData(ioTokenDesc, buffer, tokenData);
        if (err == noErr)
        {
            DeleteTokenData(true, tokenData);
            --sObjectCount;

#if 0
printf("AEToken::Dispose %d\n", sObjectCount);
#endif
        }
        break;
        
    default:
        break;
    }
    
    err = AEDisposeDesc(&ioTokenDesc);
    
    return err;
}

// ------------------------------------------------------------------------------------------
OSStatus
AEToken::ReadTokenData(
    const AEDesc&   inDescriptor, 
    char*           ioBuffer,
    TokenData*&     ioTokenData)
{
    OSStatus    err = errAECoercionFail;
    
    ioTokenData = NULL;
    
    if (inDescriptor.descriptorType == kBObjectToken)
    {
        size_t  size    = AEGetDescDataSize(&inDescriptor);
        
        if (size == sizeof(TokenData))
        {
            err = AEGetDescData(&inDescriptor, ioBuffer, sizeof(TokenData));
            
            if (err == noErr)
            {
                ioTokenData = reinterpret_cast<TokenData*>(ioBuffer);
            }
        }
    }
    
    return err;
}

// ------------------------------------------------------------------------------------------
void
AEToken::DeleteTokenData(
    bool            inOwned,
    TokenData*&     ioTokenData)
{
    if (inOwned && (ioTokenData != NULL))
    {
        ioTokenData->~TokenData();
        ioTokenData = NULL;
    }
}


}   // namespace B
