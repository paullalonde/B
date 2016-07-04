// ==========================================================================================
//	
//	Copyright (C) 2006 Paul Lalonde enrg.
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

#ifndef BAEToken_H_
#define BAEToken_H_

#pragma once

// B headers
#include "BAEDescriptor.h"
#include "BAEObject.h"
#include "BAutoUPP.h"


namespace B {

// forward declarations
class	AEAutoTokenDescriptor;

class AEToken
{
public:
	
	enum {
		kBObjectToken	= 'BTko',
	};
	
				AEToken();
	explicit	AEToken(const AEAutoTokenDescriptor& inDescriptor);
	explicit	AEToken(const AEDesc& inDescriptor);
	explicit	AEToken(AEObjectPtr inObject);
	explicit	AEToken(AEObjectPtr inObject, DescType inPropertyName);
				~AEToken();
	
	static bool		IsTokenDescriptor(const AEDesc& inDescriptor);
	static DescType	GetContainerClass(const AEDesc& inDescriptor);
	static DescType	GetObjectClassID(const AEDesc& inDescriptor);
	
	bool		IsPropertyToken() const;
	AEObjectPtr	GetObject() const;
	DescType	GetObjectClassID() const	{ return GetObject()->GetClassID(); }
	DescType	GetPropertyName() const;
	
	void		SetObject(AEObjectPtr inObject);
	void		SetProperty(AEObjectPtr inObject, DescType inPropertyName);
	void		SetProperty(const AEToken& inSourceToken, DescType inPropertyName);
	
	void		Clear();
	void		Commit(AEDesc& outDescriptor);
	
	static OSStatus	Dispose(AEDesc& ioTokenDesc);
	
private:
	
	struct TokenData
	{
		TokenData() {};
		TokenData(AEObjectPtr o, DescType p) : obj(o), pname(p) {}
		
		DescType	pname;
		AEObjectPtr	obj;
	};
	
	void	InitToken(const AEDesc& inDescriptor);
	void	SetToken(AEObjectPtr inObject, DescType inPropertyName);
	
	static OSStatus	ReadTokenData(
						const AEDesc&	inDescriptor, 
						char*			ioBuffer,
						TokenData*&		ioTokenData);
	static void		DeleteTokenData(
						bool			inOwned,
						TokenData*&		ioTokenData);
	
	// member variables
	bool		mOwned;
	TokenData*	mData;
	char		mBuffer[sizeof(TokenData)];
	
	// static member variables
	static int	sObjectCount;
};


// ==========================================================================================
//	AEAutoTokenDescriptor

class AEAutoTokenDescriptor
{
public:

	AEAutoTokenDescriptor();
	~AEAutoTokenDescriptor();
	
	DescType	GetType() const		{ return mDesc.descriptorType; }
	
	operator AEDesc* ()				{ return &mDesc; }
	operator const AEDesc* () const	{ return &mDesc; }
	operator AEDesc& ()				{ return mDesc; }
	operator const AEDesc& () const	{ return mDesc; }
	
private:
	
	// member variables
	AEDesc	mDesc;
};

// ------------------------------------------------------------------------------------------
inline
AEAutoTokenDescriptor::AEAutoTokenDescriptor()
{
	AEInitializeDescInline(&mDesc);
}

// ------------------------------------------------------------------------------------------
inline
AEAutoTokenDescriptor::~AEAutoTokenDescriptor()
{
	AEDisposeToken(&mDesc);
}
	
}	// namespace B


#endif	// BAEToken_H_
