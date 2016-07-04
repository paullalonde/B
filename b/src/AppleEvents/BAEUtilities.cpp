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

// file header
#include "BAEUtilities.h"

// B headers
#include "BException.h"


namespace B {

// ==========================================================================================
//	AutoAECoerceDescHandler

const AutoAECoerceDescUPP	AutoAECoerceDescHandler::sUPP(CoercionProc);

// ------------------------------------------------------------------------------------------
AutoAECoerceDescHandler::AutoAECoerceDescHandler(
	DescType			inFromType,
	DescType			inToType,
	CoercionFunction	inFunction)
		: mFromType(inFromType), mToType(inToType), mFunction(inFunction)
{
	OSStatus	err;
	
	err = AEGetCoercionHandler(mFromType, mToType, &mOldUPP, &mOldRefCon, 
							   &mOldFromTypeIsDesc, false);
	if (err != noErr)
		mOldUPP = NULL;
	
	err = AEInstallCoercionHandler(mFromType, mToType, sUPP, 
								   reinterpret_cast<long>(this), true, false);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AutoAECoerceDescHandler::~AutoAECoerceDescHandler()
{
	OSStatus	err;
	
	err = AERemoveCoercionHandler(mFromType, mToType, sUPP, false);
	// don't throw in a destructor!
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AutoAECoerceDescHandler::CoercionProc(
	const AEDesc*	inDesc, 
	DescType		toType, 
	long			refcon, 
	AEDesc*			result)
{
	OSStatus	err	= noErr;
	
	try
	{
		AutoAECoerceDescHandler*	handler	= reinterpret_cast<AutoAECoerceDescHandler*>(refcon);
		
		AEInitializeDescInline(result);
		
		handler->mFunction(*inDesc, toType, *result);
	}
	catch (OSStatusException& ex)
	{
		err = ex.GetStatus();
		AEDisposeDesc(result);
	}
	catch (...)
	{
		err = paramErr;
		AEDisposeDesc(result);
	}
	
	return err;
}


// ==========================================================================================
//	AutoAECoercePtrHandler

const AutoAECoercePtrUPP	AutoAECoercePtrHandler::sUPP(CoercionProc);

// ------------------------------------------------------------------------------------------
AutoAECoercePtrHandler::AutoAECoercePtrHandler(
	DescType			inFromType,
	DescType			inToType,
	CoercionFunction	inFunction)
		: mFromType(inFromType), mToType(inToType), mFunction(inFunction)
{
	OSStatus	err;
	
	err = AEGetCoercionHandler(mFromType, mToType, &mOldUPP, &mOldRefCon, 
							   &mOldFromTypeIsDesc, false);
	if (err != noErr)
		mOldUPP = NULL;
	
	err = AEInstallCoercionHandler(mFromType, mToType, 
								   reinterpret_cast<AECoerceDescUPP>(static_cast<AECoercePtrUPP>(sUPP)), 
								   reinterpret_cast<long>(this), 
								   false, false);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AutoAECoercePtrHandler::~AutoAECoercePtrHandler()
{
	OSStatus	err;
	
	err = AERemoveCoercionHandler(mFromType, mToType, 
								  reinterpret_cast<AECoerceDescUPP>(static_cast<AECoercePtrUPP>(sUPP)), 
								  false);
	// don't throw in a destructor!
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AutoAECoercePtrHandler::CoercionProc(
	DescType		typeCode, 
	const void*		inData, 
	::Size			inSize, 
	DescType		toType, 
	long			refcon, 
	AEDesc*			result)
{
	OSStatus	err	= noErr;
	
	try
	{
		AutoAECoercePtrHandler*	handler	= reinterpret_cast<AutoAECoercePtrHandler*>(refcon);
		
		AEInitializeDescInline(result);
		
		handler->mFunction(typeCode, inData, inSize, toType, *result);
	}
	catch (OSStatusException& ex)
	{
		err = ex.GetStatus();
		AEDisposeDesc(result);
	}
	catch (...)
	{
		err = paramErr;
		AEDisposeDesc(result);
	}
	
	return err;
}


}	// namespace B
