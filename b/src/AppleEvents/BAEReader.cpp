// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
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

// file header
#include "BAEReader.h"

// B headers
#include "BAEDescriptor.h"
#include "BAEObjectSupport.h"


namespace B {

// ==========================================================================================
//	AEReader

// ------------------------------------------------------------------------------------------
AEReader::AEReader(
	const AEDesc& inDesc)
		: mBaseDesc(inDesc), mCurrDesc(&mBaseDesc)
{
	AEInitializeDescInline(&mBuffDesc);
}

// ------------------------------------------------------------------------------------------
AEReader::AEReader(
	const std::vector<UInt8>&	inBuffer)
		: mBaseDesc(mBuffDesc), mCurrDesc(&mBaseDesc)
{
	OSStatus	err;
	
	err = AEUnflattenDesc(const_cast<char*>(reinterpret_cast<const char*>(&inBuffer[0])), 
						  &mBuffDesc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AEReader::~AEReader()
{
	while (!mOpenDescs.empty())
	{
		AEDisposeDesc(&mOpenDescs.top());
		mOpenDescs.pop();
	}
	
	AEDisposeDesc(&mBuffDesc);
}

// ------------------------------------------------------------------------------------------
DescType
AEReader::GetTypeKey(AEKeyword key) const
{
	DescType	descType;
	::Size		junkSize;
	OSStatus	err;
	
	err = AESizeOfKeyDesc(mCurrDesc, key, &descType, &junkSize);
	B_THROW_IF_STATUS(err);
	
	return (descType);
}

// ------------------------------------------------------------------------------------------
DescType
AEReader::GetTypeNth(long index) const
{
	DescType	descType;
	::Size		junkSize;
	OSStatus	err;
	
	err = AESizeOfNthItem(mCurrDesc, index, &descType, &junkSize);
	B_THROW_IF_STATUS(err);
	
	return (descType);
}

// ------------------------------------------------------------------------------------------
bool
AEReader::Contains(AEKeyword key) const
{
	DescType	junkType;
	::Size		junkSize;
	
	return (AESizeOfKeyDesc(mCurrDesc, key, &junkType, &junkSize) == noErr);
}

// ------------------------------------------------------------------------------------------
size_t
AEReader::Count() const
{
	long		count;
	OSStatus	err;
	
	err = AECountItems(mCurrDesc, &count);
	B_THROW_IF_STATUS(err);
	
	return (count);
}

// ------------------------------------------------------------------------------------------
void
AEReader::OpenDescKey(AEKeyword key, DescType type /* = typeWildCard */)
{
	B_THROW_IF_STATUS(PrivateOpen(key, 0, NULL, type));
}

// ------------------------------------------------------------------------------------------
void
AEReader::OpenDescNth(long index, DescType type /* = typeWildCard */, AEKeyword* key /* = NULL */)
{
	B_THROW_IF_STATUS(PrivateOpen(0, index, key, type));
}

// ------------------------------------------------------------------------------------------
bool
AEReader::OpenDescKey(AEKeyword key, const std::nothrow_t&, DescType type /* = typeWildCard */)
{
	return (PrivateOpen(key, 0, NULL, type) == noErr);
}

// ------------------------------------------------------------------------------------------
bool
AEReader::OpenDescNth(long index, const std::nothrow_t&, DescType type /* = typeWildCard */, AEKeyword* key /* = NULL */)
{
	return (PrivateOpen(0, index, key, type) == noErr);
}

// ------------------------------------------------------------------------------------------
void
AEReader::CloseDesc()
{
	B_ASSERT(!mOpenDescs.empty());
	
	AEDisposeDesc(&mOpenDescs.top());
	mOpenDescs.pop();
	
	if (mOpenDescs.empty())
		mCurrDesc = &mBaseDesc;
	else
		mCurrDesc = &mOpenDescs.top();
}

// ------------------------------------------------------------------------------------------
void
AEReader::Read(void* ptr, size_t size)
{
	OSStatus	err;
	
	err = AEGetDescData(mCurrDesc, ptr, size);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
AEReader::PrivateOpen(AEKeyword key, long index, AEKeyword* keyptr, DescType type)
{
	AEDesc		desc, coercedDesc;
	OSStatus	err;
	
	AEInitializeDescInline(&desc);
	AEInitializeDescInline(&coercedDesc);
	
	if (key != 0)
	{
		err = AEGetKeyDesc(mCurrDesc, key, typeWildCard, &desc);
		
		if ((err == noErr) && (keyptr != NULL))
			*keyptr = typeWildCard;
	}
	else
	{
		err = AEGetNthDesc(mCurrDesc, index, typeWildCard, keyptr, &desc);
	}
	
	if (err == noErr)
	{
		err = AEObjectSupport::CoerceDesc(desc, type, coercedDesc, std::nothrow);
		AEDisposeDesc(&desc);
	}
	
	if (err == noErr)
	{
		try
		{
			mOpenDescs.push(coercedDesc);
			
			mCurrDesc = &mOpenDescs.top();
		}
		catch (std::exception& ex)
		{
			err = ErrorHandler::GetStatus(ex, errAECoercionFail);
			AEDisposeDesc(&coercedDesc);
		}
		catch (...)
		{
			err = paramErr;
			AEDisposeDesc(&coercedDesc);
		}
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
AEReader::PrivateReadDesc(AEKeyword key, long index, DescType type, AEDesc& desc)
{
	AEDescriptor	itemDesc;
	OSStatus		err;
	
	if (key != 0)
		err = AEGetKeyDesc(mCurrDesc, key, typeWildCard, itemDesc);
	else
		err = AEGetNthDesc(mCurrDesc, index, typeWildCard, NULL, itemDesc);
		
	if (err == noErr)
		err = AEObjectSupport::CoerceDesc(itemDesc, type, desc, std::nothrow);
	
	return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
AEReader::PrivateReadAttribute(AEKeyword key, DescType type, void* data, size_t size)
{
	DescType	junkType;
	::Size		junkSize;
	OSStatus	err;
	
	err = AEGetAttributePtr(mCurrDesc, key, type, &junkType, data, size, &junkSize);
	
	return (err);
}


// ==========================================================================================
//	AutoAEReaderDescKey

#pragma mark -

// ------------------------------------------------------------------------------------------
AutoAEReaderDescKey::AutoAEReaderDescKey(AEReader& reader, AEKeyword key, DescType type /* = typeWildCard */)
	: mReader(reader)
{
	mReader.OpenDescKey(key, type);
}

// ------------------------------------------------------------------------------------------
AutoAEReaderDescKey::~AutoAEReaderDescKey()
{
	mReader.CloseDesc();
}


// ==========================================================================================
//	AutoAEReaderDescNth

#pragma mark -

// ------------------------------------------------------------------------------------------
AutoAEReaderDescNth::AutoAEReaderDescNth(AEReader& reader, long index, DescType type /* = typeWildCard */)
	: mReader(reader)
{
	mReader.OpenDescNth(index, type, &mKeyword);
}

// ------------------------------------------------------------------------------------------
AutoAEReaderDescNth::~AutoAEReaderDescNth()
{
	mReader.CloseDesc();
}


}	// namespace B
