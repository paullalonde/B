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

#ifndef BAEDescParam_H_
#define BAEDescParam_H_

#pragma once

// B headers
#include "BTaggedTypeTraits.h"
#include "BOSPtr.h"


namespace B {

// forward declarations
class	AEObject;
template <DescType TYPE> struct	DescParam;


// ==========================================================================================
//	DescParamHelper

#pragma mark DescParamHelper

/*!
	@brief	Common utility functions for the use of DescParam.
	
	@ingroup	AppleEvents
*/
struct DescParamHelper
{
	//!	Coerces the given Apple %Event descriptor to the given type.
	static void		GetCoercedData(
						const AEDesc&	desc, 
						DescType		type, 
						void*			data, 
						size_t			size);
    /*! @overload
    */
	static OSStatus	GetCoercedData(
						const AEDesc&	desc, 
						DescType		type, 
						void*			data, 
						size_t			size,
						const std::nothrow_t&);
	
	//! Converts an element of an Apple %Event descriptor record into a C/C++ language type.
	template <DescType TYPE>
	static void		ReadKey(
						const AERecord&								record, 
						AEKeyword									key, 
						typename TaggedTypeTrait<TYPE>::Type&		value);
    /*! @overload
    */
	template <DescType TYPE>
	static OSStatus	ReadKey(
						const AERecord&								record, 
						AEKeyword									key, 
						typename TaggedTypeTrait<TYPE>::Type&		value,
						const std::nothrow_t&);
	
	//! Converts an element of an Apple %Event descriptor list into a C/C++ language type.
	template <DescType TYPE>
	static void		ReadParam(
						const AEDescList&							list, 
						long										index, 
						typename TaggedTypeTrait<TYPE>::Type&		value);
    /*! @overload
    */
	template <DescType TYPE>
	static OSStatus	ReadParam(
						const AEDescList&							list, 
						long										index, 
						typename TaggedTypeTrait<TYPE>::Type&		value,
						const std::nothrow_t&);
	
	//! Adds a value of the C/C++ language type matching @a TYPE to an Apple %Event record.
	template <DescType TYPE>
	static void		WriteKey(
						AERecord&									record, 
						AEKeyword									key, 
						const typename TaggedTypeTrait<TYPE>::Type&	value);
};

// ------------------------------------------------------------------------------------------
/*!	@param		TYPE	An integral four-char constant in the DescType namespace.
*/
template <DescType TYPE> void
DescParamHelper::ReadKey(
	const AERecord&							record,	//!< The Apple %Event descriptor record.
	AEKeyword								key,	//!< The key of the requested descriptor.
	typename TaggedTypeTrait<TYPE>::Type&	value)	//!< Holds the output value.
{
	AEDesc		desc;
	
	AEInitializeDescInline(&desc);
	
	try
	{
		OSStatus	err;
		
		err = AEGetParamDesc(&record, key, typeWildCard, &desc);
		B_THROW_IF_STATUS(err);
		
		DescParam<TYPE>::Get(desc, value);
		AEDisposeDesc(&desc);
	}
	catch (...)
	{
		AEDisposeDesc(&desc);
		throw;
	}
}

// ------------------------------------------------------------------------------------------
/*!	@param		TYPE	An integral four-char constant in the DescType namespace.
	@return		An OS status code.
	@exception	none
*/
template <DescType TYPE> OSStatus
DescParamHelper::ReadKey(
	const AERecord&							record,	//!< The Apple %Event descriptor record.
	AEKeyword								key,	//!< The key of the requested descriptor.
	typename TaggedTypeTrait<TYPE>::Type&	value,	//!< Holds the output value.
	const std::nothrow_t&					nt)
{
	AEDesc		desc;
	OSStatus	err;
	
	err = AEGetParamDesc(&record, key, typeWildCard, &desc);
	
	if (err == noErr)
	{
		err = DescParam<TYPE>::Get(desc, value, nt);
		
		AEDisposeDesc(&desc);
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
/*!	@param		TYPE	An integral four-char constant in the DescType namespace.
*/
template <DescType TYPE> void
DescParamHelper::ReadParam(
	const AEDescList&						list,	//!< The Apple %Event descriptor list.
	long									index,	//!< The index of the requested descriptor.
	typename TaggedTypeTrait<TYPE>::Type&	value)	//!< Holds the output value.
{
	AEDesc		desc;
	
	AEInitializeDescInline(&desc);
	
	try
	{
		OSStatus	err;
		
		err = AEGetNthDesc(&list, index, typeWildCard, NULL, &desc);
		B_THROW_IF_STATUS(err);
		
		DescParam<TYPE>::Get(desc, value);
		AEDisposeDesc(&desc);
	}
	catch (...)
	{
		AEDisposeDesc(&desc);
		throw;
	}
}

// ------------------------------------------------------------------------------------------
/*!	@param		TYPE	An integral four-char constant in the DescType namespace.
	@return		An OS status code.
	@exception	none
*/
template <DescType TYPE> OSStatus
DescParamHelper::ReadParam(
	const AEDescList&						list,	//!< The Apple %Event descriptor list.
	long									index,	//!< The index of the requested descriptor.
	typename TaggedTypeTrait<TYPE>::Type&	value,	//!< Holds the output value.
	const std::nothrow_t&					nt)
{
	AEDesc		desc;
	OSStatus	err;
	
	err = AEGetNthDesc(&list, index, typeWildCard, NULL, &desc);
	
	if (err == noErr)
	{
		err = DescParam<TYPE>::Get(desc, value, nt);
		
		AEDisposeDesc(&desc);
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
/*!	@param		TYPE	An integral four-char constant in the DescType namespace.
*/
template <DescType TYPE> void
DescParamHelper::WriteKey(
	AERecord&									record,	//!< The Apple %Event descriptor record.
	AEKeyword									key,	//!< The key of the input value.
	const typename TaggedTypeTrait<TYPE>::Type&	value)	//!< The input value.
{
	AEDesc	desc	= { typeNull, NULL };
	
	try
	{
		OSStatus	err;
		
		DescParam<TYPE>::Put(desc, value);
		
		err = AEPutParamDesc(&record, key, &desc);
		B_THROW_IF_STATUS(err);
		
		AEDisposeDesc(&desc);
	}
	catch (...)
	{
		AEDisposeDesc(&desc);
		
		throw;
	}
}


// ==========================================================================================
//	DescParam

#pragma mark -
#pragma mark DescParam

/*!
	@brief	Reads & writes an Apple %Event descriptor in a type-safe manner.
	
	DescParam provides a type-safe wrapper around the Apple %Event Manager 
	calls that read and write Apple %Event descriptors.
	
	The class's type-safety is provided by the related type TaggedTypeTrait, 
	which maps an @c DescType onto a C/C++ language type.  Thus DescParam 
	augments TaggedTypeTrait with Apple %Event accessors.
	
	@ingroup	AppleEvents
*/
template <DescType TYPE> struct DescParam
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef DescType	TYPE;		//!< The template parameter.  Should be an integral four-char constant (a @c DescType).
	//@}
#endif
	
	//! @name Types
	//@{
	typedef typename TaggedTypeTrait<TYPE>::Type	ValueType;	//!< The language type.  See TaggedTypeTrait.
	//@}
	
	//! @name Reading Apple %Event Descriptors
	//@{
	//! Converts the contents of an Apple %Event descriptor into a C/C++ language type.
	static void		Get(const AEDesc& desc, ValueType& value);
    /*! @overload
    */
	static OSStatus	Get(const AEDesc& desc, ValueType& value, const std::nothrow_t&);
	//@}
	
	//! @name Writing Apple %Event Descriptors
	//@{
	//! Converts a C/C++ language type value into an Apple %Event descriptor.
	static void		Put(AEDesc& desc, const ValueType& value);
	//! Appends a C/C++ language type value to an Apple %Event descriptor stream.
	static void		Write(AEStreamRef stream, const ValueType& value);
	//@}
};


// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline void
DescParam<TYPE>::Get(
	const AEDesc&			desc,	//!< The Apple %Event descriptor.
	ValueType&				value)	//!< Holds the output value.
{
	DescParamHelper::GetCoercedData(desc, TYPE, &value, sizeof(value));
}

// ------------------------------------------------------------------------------------------
/*!	@return		An OS status code.
	@exception	none
*/
template <DescType TYPE> inline OSStatus
DescParam<TYPE>::Get(
	const AEDesc&			desc,	//!< The Apple %Event descriptor.
	ValueType&				value,	//!< Holds the output value.
	const std::nothrow_t&	nt)
{
	return (DescParamHelper::GetCoercedData(desc, TYPE, &value, sizeof(value), nt));
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> void
DescParam<TYPE>::Put(
	AEDesc&				desc,	//!< The Apple %Event descriptor record.
	const ValueType&	value)	//!< The new value.
{
	OSStatus	err;
	
	err = AECreateDesc(TYPE, &value, sizeof(value), &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function if you are appending to an open descriptor or to an open 
	descriptor list.
*/
template <DescType TYPE> void
DescParam<TYPE>::Write(
	AEStreamRef			stream,	//!< The Apple %Event descriptor stream.
	const ValueType&	value)	//!< The new value.
{
	OSStatus	err;
	
	err = AEStreamWriteDesc(stream, TYPE, &value, sizeof(value));
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	Template specialisations of DescParam<TYPE>

#ifndef DOXYGEN_SKIP

/*!	@brief	DescParam template specialisation for @c typeBoolean.
	
	Converts to and from @c Boolean (the type actually read/written) and 
	@c bool, the C/C++ language type passed to the member functions.
*/
template <> struct DescParam<typeBoolean>
{
	typedef TaggedTypeTrait<typeBoolean>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, bool& value);
	static OSStatus	Get(const AEDesc& desc, bool& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, bool value);
	static void		Write(AEStreamRef stream, bool value);
};

/*!	@brief	DescParam template specialisation for @c typeQDPoint.
	
	Converts to and from @c ::Point (the type actually read/written) and 
	Point, the C/C++ language type passed to the member functions.
*/
template <> struct DescParam<typeQDPoint>
{
	typedef TaggedTypeTrait<typeQDPoint>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, ValueType& value);
	static OSStatus	Get(const AEDesc& desc, ValueType& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const ValueType& value);
	static void		Write(AEStreamRef stream, const ValueType& value);
};

/*!	@brief	DescParam template specialisation for @c typeQDRectangle.
	
	Converts to and from @c ::Rect (the type actually read/written) and 
	Rect, the C/C++ language type passed to the member functions.
*/
template <> struct DescParam<typeQDRectangle>
{
	typedef TaggedTypeTrait<typeQDRectangle>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, ValueType& value);
	static OSStatus	Get(const AEDesc& desc, ValueType& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const ValueType& value);
	static void		Write(AEStreamRef stream, const ValueType& value);
};


/*!	@brief	DescParam template specialisation for @c typeObjectSpecifier.
	
	Converts to and from object specifier descriptors (the type actually read/written) 
	and AEObject*, the C/C++ language type passed to the member functions.
*/
template <> struct DescParam<typeObjectSpecifier>
{
	typedef TaggedTypeTrait<typeObjectSpecifier>::Type	ValueType;

	static void		Get(const AEDesc& desc, AEObjectPtr& value);
	static OSStatus	Get(const AEDesc& desc, AEObjectPtr& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const AEObject& value);
	static void		Write(AEStreamRef stream, const AEObject& value);
};


/*!	@brief	DescParam template specialisation for @c typeUTF8Text.
	
	Converts to and from the externalised representation of a @c CFStringRef (the 
	type actually read/written) and String, the C/C++ language type passed to the 
	member functions.
*/
template <> struct DescParam<typeUTF8Text>
{
	typedef TaggedTypeTrait<typeUTF8Text>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, String& value);
	static OSStatus	Get(const AEDesc& desc, String& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const String& value);
	static void		Write(AEStreamRef stream, const String& value);
};


/*!	@brief	DescParam template specialisation for @c typeUTF16ExternalRepresentation.
	
	Converts to and from the externalised representation of a @c CFStringRef (the 
	type actually read/written) and String, the C/C++ language type passed to the 
	member functions.
*/
template <> struct DescParam<typeUTF16ExternalRepresentation>
{
	typedef TaggedTypeTrait<typeUTF16ExternalRepresentation>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, String& value);
	static OSStatus	Get(const AEDesc& desc, String& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const String& value);
	static void		Write(AEStreamRef stream, const String& value);
};


/*!	@brief	DescParam template specialisation for @c typeUnicodeText.
	
	Converts to and from the externalised representation of a @c CFStringRef (the 
	type actually read/written) and String, the C/C++ language type passed to the 
	member functions.
*/
template <> struct DescParam<typeUnicodeText>
{
	typedef TaggedTypeTrait<typeUnicodeText>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, String& value);
	static OSStatus	Get(const AEDesc& desc, String& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const String& value);
	static void		Write(AEStreamRef stream, const String& value);
};


/*!	@brief	DescParam template specialisation for @c typeUnicodeText.
	
	Converts to and from an array of @c char (the type actually read/written) 
	and @c std::string, the C/C++ language type passed to the member functions.
*/
template <> struct DescParam<typeChar>
{
	typedef TaggedTypeTrait<typeChar>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, std::string& value);
	static OSStatus	Get(const AEDesc& desc, std::string& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const std::string& value);
	static void		Write(AEStreamRef stream, const std::string& value);
};


/*!	@brief	DescParam template specialisation for @c typeFileURL.
	
	Converts to and from the externalised representation of a @c CFURLRef (the 
	type actually read/written) and Url, the C/C++ language type passed to the 
	member functions.
*/
template <> struct DescParam<typeFileURL>
{
	typedef TaggedTypeTrait<typeFileURL>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, Url& value);
	static OSStatus	Get(const AEDesc& desc, Url& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const Url& value);
	static void		Write(AEStreamRef stream, const Url& value);
};


/*!	@brief	DescParam template specialisation for @c typeData.
	
	Converts to and from CoreFoundation DataRefs.
*/
template <> struct DescParam<typeData>
{
	typedef TaggedTypeTrait<typeData>::Type	ValueType;
	
	static void		Get(const AEDesc& desc, OSPtr<CFDataRef>& value);
	static OSStatus	Get(const AEDesc& desc, OSPtr<CFDataRef>& value, const std::nothrow_t&);
	
	static void		Put(AEDesc& desc, const OSPtr<CFDataRef>& value);
	static void		Write(AEStreamRef stream, const OSPtr<CFDataRef>& value);
};

#endif	// DOXYGEN_SKIP


}	// namespace B

#endif	// BAEDescParam_H_
