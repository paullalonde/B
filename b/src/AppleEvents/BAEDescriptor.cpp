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
#include "BAEDescriptor.h"

// standard headers
#include <istream>
#include <ostream>


namespace B {

// ------------------------------------------------------------------------------------------
AEDescriptor::AEDescriptor(const AEDescriptor &inOriginal)
{
	OSStatus	err;
	
	err = AEDuplicateDesc(&inOriginal.mDesc, &mDesc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AEDescriptor::AEDescriptor(DescType inType, const void* inPtr, size_t inSize)
{
	OSStatus	err;
	
	err = AECreateDesc(inType, inPtr, inSize, &mDesc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AEDescriptor::~AEDescriptor()
{
	AEDisposeDesc(&mDesc);
}

// ------------------------------------------------------------------------------------------
void
AEDescriptor::Clear()
{
	OSStatus	err;
	
	err = AEDisposeDesc(&mDesc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AEDescriptor&
AEDescriptor::operator = (const AEDescriptor &inOriginal)
{
	AEDescriptor	temp(inOriginal);
	
	swap(temp);
	
	return (*this);
}


// ==========================================================================================
//	AEListIterator

// ------------------------------------------------------------------------------------------
AEListIterator::AEListIterator(
	const AEDescList&	inList, 
	DescType			inDescType, 
	size_t				inIndex)
		: mList(inList), mCount(GetCount(inList)), mDescType(inDescType), 
		  mIndex(std::min(mCount+1, inIndex))
{
}

// ------------------------------------------------------------------------------------------
size_t
AEListIterator::GetCount(const AEDescList& inList)
{
	long		count;
	OSStatus	err;
	
	err = AECountItems(&inList, &count);
	B_THROW_IF_STATUS(err);
	
	return count;
}

// ------------------------------------------------------------------------------------------
const AEDesc&	
AEListIterator::operator * () const
{
	if (mDesc.Empty())
	{
		OSStatus	err;
	
		err = AEGetNthDesc(&mList, mIndex, mDescType, NULL, mDesc);
		B_THROW_IF_STATUS(err);
	}
	
	return mDesc;
}

// ------------------------------------------------------------------------------------------
const AEDesc*	
AEListIterator::operator -> () const
{
	if (mDesc.Empty())
	{
		OSStatus	err;
	
		err = AEGetNthDesc(&mList, mIndex, mDescType, NULL, mDesc);
		B_THROW_IF_STATUS(err);
	}
	
	return mDesc;
}

// ------------------------------------------------------------------------------------------
AEListIterator
MakeAEListBeginIterator(
	const AEDescList&	inList, 
	DescType			inDescType /* = typeWildCard */)
{
	return AEListIterator(inList, inDescType, 1);
}

// ------------------------------------------------------------------------------------------
AEListIterator
MakeAEListEndIterator(
	const AEDescList&	inList)
{
	return AEListIterator(inList, typeWildCard, UINT_MAX);
}

}	// namespace B


// ------------------------------------------------------------------------------------------
std::ostream&
operator << (std::ostream& ostr, const AEDesc& desc)
{
	SInt32				size, junk;
	std::vector<char>	buff;
	OSStatus			err;
	
	size = AESizeOfFlattenedDesc(&desc);
	
	buff.resize(size);
	
	err = AEFlattenDesc(&desc, &buff[0], size, &junk);
	B_THROW_IF_STATUS(err);
	
	{
		SInt32 bigEndianSize = EndianS32_NtoB(size);
		ostr.write(reinterpret_cast<const char*>(&bigEndianSize), sizeof(bigEndianSize));
	}
	
	ostr.write(&buff[0], size);
	
	return (ostr);
}

// ------------------------------------------------------------------------------------------
std::istream&
operator >> (std::istream& istr, AEDesc& desc)
{
	AEDesc				tempDesc;
	SInt32				size;
	std::vector<char>	buff;
	OSStatus			err;
	
	{
		SInt32	bigEndianSize;
		
		istr.read(reinterpret_cast<char*>(&bigEndianSize), sizeof(bigEndianSize));
		size = EndianS32_BtoN(bigEndianSize);
	}
	
	buff.resize(size);
	istr.read(&buff[0], size);
	
	err = AEUnflattenDesc(&buff[0], &tempDesc);
	B_THROW_IF_STATUS(err);
	
	AEDisposeDesc(&desc);
	desc = tempDesc;
	
	return (istr);
}
