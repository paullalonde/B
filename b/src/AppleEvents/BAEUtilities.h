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

#ifndef BAEUtilities_H_
#define BAEUtilities_H_

#pragma once

// B headers
#include "BAutoUPP.h"


namespace B {

// ==========================================================================================
//	AutoAECoerceDescHandler

/*!
	@brief	Lifecycle management of descriptor-based Apple %Event coercion handler.
*/
class AutoAECoerceDescHandler
{
public:
	
	// types
	typedef boost::function3<void, 
		const AEDesc&, 
		DescType, 
		AEDesc&>		CoercionFunction;
	
	//! Constructor. Installs the coercion handler.
	AutoAECoerceDescHandler(
		DescType			inFromType,
		DescType			inToType,
		CoercionFunction	inFunction);
	//! Destructor. Removes the coercion handler.
	~AutoAECoerceDescHandler();
	
private:
	
	static pascal OSErr	CoercionProc(
		const AEDesc*	inDesc, 
		DescType		toType, 
		long			refcon, 
		AEDesc*			result);
	
	// member variables
	const DescType			mFromType;
	const DescType			mToType;
	const CoercionFunction	mFunction;
	AECoerceDescUPP			mOldUPP;
	long					mOldRefCon;
	Boolean					mOldFromTypeIsDesc;
	
	// static member variables
	static const AutoAECoerceDescUPP	sUPP;
};


// ==========================================================================================
//	AutoAECoercePtrHandler

/*!
	@brief	Lifecycle management of pointer-based Apple %Event coercion handler.
*/
class AutoAECoercePtrHandler
{
public:
	
	// types
	typedef boost::function5<void, 
		DescType, 
		const void*,
		size_t,
		DescType, 
		AEDesc&>		CoercionFunction;
	
	//! Constructor. Installs the coercion handler.
	AutoAECoercePtrHandler(
		DescType			inFromType,
		DescType			inToType,
		CoercionFunction	inFunction);
	//! Destructor. Removes the coercion handler.
	~AutoAECoercePtrHandler();
	
private:
	
	static pascal OSErr	CoercionProc(
		DescType		typeCode, 
		const void*		inData, 
		::Size			inSize, 
		DescType		toType, 
		long			refcon, 
		AEDesc*			result);
	
	const DescType			mFromType;
	const DescType			mToType;
	const CoercionFunction	mFunction;
	AECoerceDescUPP			mOldUPP;
	long					mOldRefCon;
	Boolean					mOldFromTypeIsDesc;
	
	// static member variables
	static const AutoAECoercePtrUPP	sUPP;
};


}	// namespace B


#endif	// BAEUtilities_H_
