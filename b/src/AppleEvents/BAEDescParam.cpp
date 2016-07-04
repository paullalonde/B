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
#include "BAEDescParam.h"

// library headers
#include "MoreAEDataModel.h"

// B headers
#include "BAEDescriptor.h"
#include "BAEObject.h"
#include "BAEObjectSupport.h"
#include "BAEWriter.h"
#include "BRect.h"
#include "BStringUtilities.h"
#include "CFUtils.h"


namespace {
	const size_t	UNICHAR_BUFFER_SIZE	= 16000;
	const size_t	CHAR_BUFFER_SIZE	= 64000;
}

namespace B {


// ==========================================================================================
//	DescParamHelper

// ------------------------------------------------------------------------------------------
void
DescParamHelper::GetCoercedData(
	const AEDesc&			desc,	//!< The Apple %Event descriptor.
	DescType				type,	//!< The destination type of the coercion.
	void*					data,	//!< The destination address.
	size_t					size)	//!< The destination's size.
{
	AEObjectSupport::CoerceDesc(desc, type, data, size);
}

// ------------------------------------------------------------------------------------------
/*!	This function calls MOSL to perform the coercion.  This means that if @a desc is an 
	object specifier, the specifier is resolved prior to coercion.  For example, if 
	@a desc represents
	
	@code the number of words of paragraph 3 of document "foo" @endcode
	
	and @a type is @c typeIEEE32BitFloatingPoint, then @ desc is resolved (yielding the 
	integer count of words), then coerced into a floating-point value.
	
	@return		An OS status code.
	@exception	none
*/
OSStatus
DescParamHelper::GetCoercedData(
	const AEDesc&			desc,	//!< The Apple %Event descriptor.
	DescType				type,	//!< The destination type of the coercion.
	void*					data,	//!< The destination address.
	size_t					size,	//!< The destination's size.
	const std::nothrow_t&	nt)
{
	return AEObjectSupport::CoerceDesc(desc, type, data, size, nt);
}


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//	DescParam<typeBoolean>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeBoolean>::Get(const AEDesc& desc, bool& value)
{
	Boolean		booleanValue;
	
	AEObjectSupport::CoerceDesc(desc, typeBoolean, &booleanValue, sizeof(booleanValue));
	
	value = booleanValue;
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeBoolean>::Get(const AEDesc& desc, bool& value, const std::nothrow_t&)
{
	Boolean		booleanValue;
	OSStatus	err;
	
	err = AEObjectSupport::CoerceDesc(desc, typeBoolean, &booleanValue, sizeof(booleanValue), 
									  std::nothrow);
	
	if (err == noErr)
		value = booleanValue;
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeBoolean>::Put(AEDesc& desc, bool value)
{
	Boolean		booleanValue	= value;
	OSStatus	err;
	
	err = AECreateDesc(typeBoolean, &booleanValue, sizeof(booleanValue), &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeBoolean>::Write(AEStreamRef stream, bool value)
{
	Boolean		booleanValue	= value;
	OSStatus	err;
	
	err = AEStreamWriteDesc(stream, typeBoolean, &booleanValue, sizeof(booleanValue));
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeQDPoint>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeQDPoint>::Get(const AEDesc& desc, ValueType& value)
{
	::Point		qdPt;
	
	AEObjectSupport::CoerceDesc(desc, typeQDPoint, &qdPt, sizeof(qdPt));
	
	value = qdPt;
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeQDPoint>::Get(const AEDesc& desc, ValueType& value, const std::nothrow_t&)
{
	::Point		qdPt;
	OSStatus	err;
	
	err = AEObjectSupport::CoerceDesc(desc, typeQDPoint, &qdPt, sizeof(qdPt), 
									  std::nothrow);
	
	if (err == noErr)
		value = qdPt;
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeQDPoint>::Put(AEDesc& desc, const ValueType& value)
{
	::Point		qdPt	= value;
	OSStatus	err;
	
	err = AECreateDesc(typeQDPoint, &qdPt, sizeof(qdPt), &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeQDPoint>::Write(AEStreamRef stream, const ValueType& value)
{
	::Point		qdPt	= value;
	OSStatus	err;
	
	err = AEStreamWriteDesc(stream, typeQDPoint, &qdPt, sizeof(qdPt));
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeQDRectangle>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeQDRectangle>::Get(const AEDesc& desc, ValueType& value)
{
	::Rect		qdRect;
	
	AEObjectSupport::CoerceDesc(desc, typeQDRectangle, &qdRect, sizeof(qdRect));
	
	value = qdRect;
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeQDRectangle>::Get(const AEDesc& desc, ValueType& value, const std::nothrow_t&)
{
	::Rect		qdRect;
	OSStatus	err;
	
	err = AEObjectSupport::CoerceDesc(desc, typeQDRectangle, &qdRect, sizeof(qdRect), 
									  std::nothrow);
	
	if (err == noErr)
		value = qdRect;
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeQDRectangle>::Put(AEDesc& desc, const ValueType& value)
{
	::Rect		qdRect	= value;
	OSStatus	err;
	
	err = AECreateDesc(typeQDRectangle, &qdRect, sizeof(qdRect), &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeQDRectangle>::Write(AEStreamRef stream, const ValueType& value)
{
	::Rect		qdRect	= value;
	OSStatus	err;
	
	err = AEStreamWriteDesc(stream, typeQDRectangle, &qdRect, sizeof(qdRect));
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeObjectSpecifier>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeObjectSpecifier>::Get(const AEDesc& desc, AEObjectPtr& value)
{
	value = AEObjectSupport::Get().Resolve(desc);
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeObjectSpecifier>::Get(const AEDesc& desc, AEObjectPtr& value, const std::nothrow_t&)
{
	return AEObjectSupport::Get().Resolve(desc, value);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeObjectSpecifier>::Put(AEDesc& desc, const AEObject& value)
{
	AEWriter	writer;
	
	value.MakeSpecifier(writer);
	writer.Close(desc);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeObjectSpecifier>::Write(AEStreamRef stream, const AEObject& value)
{
	AEWriter	writer(stream);
	
	value.MakeSpecifier(writer);
}


// ==========================================================================================
//	DescParam<typeUTF8Text>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeUTF8Text>::Get(const AEDesc& desc, String& value)
{
	AEDescriptor	textDesc;
	
	AEObjectSupport::CoerceDesc(desc, typeUTF8Text, textDesc);

	size_t		size	= AEGetDescDataSize(textDesc);
	OSStatus	err;
	
	if (size > 0)
	{
		std::vector<UInt8>	buffer(size);
		
		err = AEGetDescData(textDesc, &buffer[0], size);
		B_THROW_IF_STATUS(err);
		
		value = OSPtr<CFStringRef>(CFStringCreateWithBytes(
										NULL, &buffer[0], size, 
										kCFStringEncodingUTF8, true),
								   from_copy);
	}
	else
	{
		value.clear();
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeUTF8Text>::Get(const AEDesc& desc, String& value, const std::nothrow_t&)
{
	AutoThrowHandler	autoThrow(false);
	OSStatus			err	= noErr;
	
	try
	{
		Get(desc, value);
	}
	catch (std::exception& ex)
	{
		err = ErrorHandler::GetStatus(ex, memFullErr);
	}
	catch (...)
	{
		err = memFullErr;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeUTF8Text>::Put(AEDesc& desc, const String& value)
{
	AEStreamRef	stream;
	OSStatus	err;
	
	stream = AEStreamOpen();
	B_THROW_IF_NULL(stream);
	
	Write(stream, value);
	
	err = AEStreamClose(stream, &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeUTF8Text>::Write(AEStreamRef stream, const String& value)
{
	OSStatus	err;
	
	err = AEStreamOpenDesc(stream, typeUTF8Text);
	B_THROW_IF_STATUS(err);
	
	CFStringRef	cfstr	= value.cf_ref();
	CFIndex		size	= CFStringGetLength(cfstr);
	
	if (size > 0)
	{
		CFIndex	done		= 0;
		Boolean	externalRep	= true;
		
		while (done < size)
		{
			UInt8	buffer[CHAR_BUFFER_SIZE];
			CFIndex	used;
			
			done += CFStringGetBytes(cfstr, 
							CFRangeMake(done, size - done), 
							kCFStringEncodingUTF8, 0, externalRep, 
							buffer, sizeof(buffer), &used);
			
			err = AEStreamWriteData(stream, buffer, used);
			B_THROW_IF_STATUS(err);
			
			externalRep = false;
		}
	}
	
	err = AEStreamCloseDesc(stream);
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeUTF16ExternalRepresentation>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeUTF16ExternalRepresentation>::Get(const AEDesc& desc, String& value)
{
	AEDescriptor	textDesc;
	
	AEObjectSupport::CoerceDesc(desc, typeUTF16ExternalRepresentation, textDesc);
	
	size_t	size	= AEGetDescDataSize(textDesc);

	if (size > 0)
	{
		OSStatus	err;
		
#if TARGET_RT_BIG_ENDIAN
		bool		gotData	= false;

		if (size >= sizeof(UniChar))
		{
			UniChar	bom;
			
			err = AEGetDescDataRange(textDesc, &bom, 0, sizeof(bom));
			
			if (err == noErr)
			{
				if ((bom != 0xFEFF) && (bom != 0xFFFE))
				{
					// The string doesn't begin with a BOM, which means it's big-endian.
					
					CFAllocatorRef	allocator	= CFAllocatorGetDefault();
					UniChar*		buffer		= reinterpret_cast<UniChar*>(
													CFAllocatorAllocate(allocator, size, 0));
					
					B_THROW_IF_NULL(buffer);
					
					try
					{
						err = AEGetDescDataRange(textDesc, buffer, 0, size);
						B_THROW_IF_STATUS(err);
						
						value = OSPtr<CFStringRef>(CFStringCreateWithCharactersNoCopy(
														NULL, buffer, 
														size / sizeof(UniChar), allocator),
												   from_copy);
						gotData = true;
					}
					catch (...)
					{
						CFAllocatorDeallocate(allocator, buffer);
						throw;
					}
				}
			}
		}
		
		if (!gotData)
#endif
		{
			std::vector<UInt8>	buffer(size);

			err = AEGetDescData(textDesc, &buffer[0], size);
			B_THROW_IF_STATUS(err);
			
			value = OSPtr<CFStringRef>(CFStringCreateWithBytes(
											NULL, &buffer[0], size, 
											kCFStringEncodingUnicode, true),
									   from_copy);
		}
	}
	else
	{
		value.clear();
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeUTF16ExternalRepresentation>::Get(const AEDesc& desc, String& value, const std::nothrow_t&)
{
	AutoThrowHandler	autoThrow(false);
	OSStatus			err	= noErr;
	
	try
	{
		Get(desc, value);
	}
	catch (std::exception& ex)
	{
		err = ErrorHandler::GetStatus(ex, memFullErr);
	}
	catch (...)
	{
		err = memFullErr;
	}
	
	return (err);

#if 0
	AEDesc		textDesc	= { typeNull, NULL };
	CFStringRef	cfstr		= NULL;
	OSStatus	err			= noErr;
	
	err = AEObjectSupport::CoerceDesc(desc, typeUTF16ExternalRepresentation, textDesc, 
									  std::nothrow);
	
	if (err == noErr)
	{
		size_t	size	= AEGetDescDataSize(&textDesc);
		
		if (size > 0)
		{
#if TARGET_RT_BIG_ENDIAN
			if (static_cast<size_t>(size) >= sizeof(UniChar))
			{
				UniChar	bom;
				
				err = AEGetDescDataRange(&textDesc, &bom, 0, sizeof(bom));
				
				if (err == noErr)
				{
					if ((bom != 0xFEFF) && (bom != 0xFFFE))
					{
						// The string doesn't begin with a BOM, which means it's big-endian.
						
						UniChar*	buffer	= reinterpret_cast<UniChar*>(CFAllocatorAllocate(NULL, size, 0));
						
						if (buffer != NULL)
						{
							err = AEGetDescDataRange(&textDesc, buffer, 0, size);
							
							if (err == noErr)
							{
								cfstr = CFStringCreateWithCharactersNoCopy(
											NULL, buffer, size / sizeof(UniChar), NULL);
								
								if (cfstr == NULL)
									err = memFullErr;
							}
							
							if (err != noErr)
							{
								CFAllocatorDeallocate(NULL, buffer);
							}
						}
						else
						{
							err = memFullErr;
						}
					}
				}
			}
			
			if ((cfstr == NULL) && (err == noErr))
#endif
			{
				UInt8*	buffer	= new (std::nothrow) UInt8[size];
				
				if (buffer != NULL)
				{
					err = AEGetDescData(&textDesc, buffer, size);
					
					if (err == noErr)
					{
						cfstr = CFStringCreateWithBytes(
									NULL, buffer, size, kCFStringEncodingUnicode, true);
						
						if (cfstr == NULL)
							err = memFullErr;
					}
					
					delete [] buffer;
				}
				else
				{
					err = memFullErr;
				}
			}
			
			if (cfstr != NULL)
			{
				value = OSPtr<CFStringRef>(cfstr, from_copy, std::nothrow);
			}
		}
		else
		{
			value.clear();
		}
		
		AEDisposeDesc(&textDesc);
	}
	
	return (err);
#endif
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeUTF16ExternalRepresentation>::Put(AEDesc& desc, const String& value)
{
	AEStreamRef	stream;
	OSStatus	err;
	
	stream = AEStreamOpen();
	B_THROW_IF_NULL(stream);
	
	Write(stream, value);
	
	err = AEStreamClose(stream, &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeUTF16ExternalRepresentation>::Write(AEStreamRef stream, const String& value)
{
	OSStatus	err;
	
	err = AEStreamOpenDesc(stream, typeUTF16ExternalRepresentation);
	B_THROW_IF_STATUS(err);
	
	CFStringRef	cfstr	= value.cf_ref();
	CFIndex		size	= CFStringGetLength(cfstr);
	bool		addBOM	= true;
	
	if (size > 0)
	{
#if TARGET_RT_BIG_ENDIAN
		// typeUTF16ExternalRepresentation allows for the BOM so be absent when the 
		// Unicode characters are big-endian. So in this case, we can bypass the call 
		// to CFStringGetBytes(), which avoids copying.
		
		addBOM = false;
		
		const UniChar*	uniptr	= CFStringGetCharactersPtr(cfstr);
		
		if (uniptr != NULL)
		{
			err = AEStreamWriteData(stream, uniptr, size * sizeof(UniChar));
			B_THROW_IF_STATUS(err);
		}
		else
#endif
		{
			CFIndex	done	= 0;
			
			while (done < size)
			{
				UInt8	buffer[CHAR_BUFFER_SIZE];
				CFIndex	used;
				
				done += CFStringGetBytes(cfstr, 
								CFRangeMake(done, size - done), 
								kCFStringEncodingUnicode, 0, addBOM, 
								buffer, sizeof(buffer), &used);
				
				err = AEStreamWriteData(stream, buffer, used);
				B_THROW_IF_STATUS(err);
				
				addBOM = false;
			}
		}
	}
	
	err = AEStreamCloseDesc(stream);
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeUnicodeText>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeUnicodeText>::Get(const AEDesc& desc, String& value)
{
	AEDescriptor	textDesc;
	
	AEObjectSupport::CoerceDesc(desc, typeUnicodeText, textDesc);
	
	size_t	size	= AEGetDescDataSize(textDesc);
		
	B_ASSERT(size % sizeof(UniChar) == 0);
		
	if (size > 0)
	{
		CFAllocatorRef	allocator	= CFAllocatorGetDefault();
		UniChar*		buffer		= reinterpret_cast<UniChar*>(
											CFAllocatorAllocate(allocator, size, 0));
		
		B_THROW_IF_NULL(buffer);
		
		try
		{
			OSStatus	err;
			
			err = AEGetDescData(&desc, buffer, size);
			B_THROW_IF_STATUS(err);
			
			value = OSPtr<CFStringRef>(CFStringCreateWithCharactersNoCopy(
											NULL, buffer, size / sizeof(UniChar), 
											allocator), 
									   from_copy);
		}
		catch (...)
		{
			CFAllocatorDeallocate(allocator, buffer);
			throw;
		}
	}
	else
	{
		value.clear();
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeUnicodeText>::Get(const AEDesc& desc, String& value, const std::nothrow_t&)
{
	AutoThrowHandler	autoThrow(false);
	OSStatus			err	= noErr;
	
	try
	{
		Get(desc, value);
	}
	catch (std::exception& ex)
	{
		err = ErrorHandler::GetStatus(ex, memFullErr);
	}
	catch (...)
	{
		err = memFullErr;
	}
	
	return (err);

#if 0
	AEDesc		textDesc	= { typeNull, NULL };
	OSStatus	err			= noErr;
	
	err = AEObjectSupport::CoerceDesc(desc, typeUnicodeText, textDesc, std::nothrow);
	
	if (err == noErr)
	{
		size_t	size	= AEGetDescDataSize(&textDesc);
		
		B_ASSERT(size % sizeof(UniChar) == 0);
		
		if (size > 0)
		{
			UniChar*	buffer	= reinterpret_cast<UniChar*>(CFAllocatorAllocate(NULL, size, 0));
			
			if (buffer != NULL)
			{
				err = AEGetDescData(&desc, buffer, size);
				
				if (err == noErr)
				{
					CFStringRef	cfstr	= CFStringCreateWithCharactersNoCopy(
												NULL, buffer, size / sizeof(UniChar), NULL);
					
					if (cfstr != NULL)
					{
						value = OSPtr<CFStringRef>(cfstr, from_copy, std::nothrow);
					}
					else
					{
						err = memFullErr;
					}
				}
				
				if (err != noErr)
				{
					CFAllocatorDeallocate(NULL, buffer);
				}
			}
			else
			{
				err = memFullErr;
			}
		}
		else
		{
			value.clear();
		}
		
		AEDisposeDesc(&textDesc);
	}
	
	return (err);
#endif
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeUnicodeText>::Put(AEDesc& desc, const String& value)
{
	AEStreamRef	stream;
	OSStatus	err;
	
	stream = AEStreamOpen();
	B_THROW_IF_NULL(stream);
	
	Write(stream, value);
	
	err = AEStreamClose(stream, &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeUnicodeText>::Write(AEStreamRef stream, const String& value)
{
	OSStatus	err;
	
	err = AEStreamOpenDesc(stream, typeUnicodeText);
	B_THROW_IF_STATUS(err);
	
	CFStringRef	cfstr	= value.cf_ref();
	CFIndex		size	= CFStringGetLength(cfstr);
	
	if (size > 0)
	{
		const UniChar*	uniptr	= CFStringGetCharactersPtr(cfstr);
		
		if (uniptr != NULL)
		{
			err = AEStreamWriteData(stream, uniptr, size * sizeof(UniChar));
			B_THROW_IF_STATUS(err);
		}
		else
		{
			CFIndex	done	= 0;
			
			while (done < size)
			{
				UniChar	buffer[UNICHAR_BUFFER_SIZE];
				CFIndex	nread	= std::min(size - done, static_cast<CFIndex>(UNICHAR_BUFFER_SIZE));
				
				CFStringGetCharacters(cfstr, CFRangeMake(done, nread), buffer);
				
				err = AEStreamWriteData(stream, buffer, nread * sizeof(UniChar));
				B_THROW_IF_STATUS(err);
				
				done += nread;
			}
		}
	}
	
	err = AEStreamCloseDesc(stream);
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeChar>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeChar>::Get(const AEDesc& desc, std::string& value)
{
	AEDescriptor	charDesc;
	
	AEObjectSupport::CoerceDesc(desc, typeChar, charDesc);
	
	size_t	size	= AEGetDescDataSize(charDesc);
	
	if (size > 0)
	{
		std::vector<char>	buffer(size);
		OSStatus			err;
		
		err = AEGetDescData(charDesc, &buffer[0], size);
		B_THROW_IF_STATUS(err);
		
		value.assign(&buffer[0], size);
	}
	else
	{
		value.clear();
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeChar>::Get(const AEDesc& desc, std::string& value, const std::nothrow_t&)
{
	AutoThrowHandler	autoThrow(false);
	OSStatus			err	= noErr;
	
	try
	{
		Get(desc, value);
	}
	catch (std::exception& ex)
	{
		err = ErrorHandler::GetStatus(ex, memFullErr);
	}
	catch (...)
	{
		err = memFullErr;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeChar>::Put(AEDesc& desc, const std::string& value)
{
	OSStatus	err;
	
	err = AECreateDesc(typeChar, value.c_str(), value.size(), &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeChar>::Write(AEStreamRef stream, const std::string& value)
{
	OSStatus	err;
	
	err = AEStreamWriteDesc(stream, typeChar, value.c_str(), value.size());
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeFileURL>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeFileURL>::Get(const AEDesc& desc, Url& value)
{
	AEDescriptor	urlDesc;

	AEObjectSupport::CoerceDesc(desc, typeFileURL, urlDesc);
	
	size_t	size	= AEGetDescDataSize(urlDesc);
	
	if (size > 0)
	{
		std::vector<UInt8>	buffer(size);
		OSStatus			err;
		
		err = AEGetDescData(urlDesc, &buffer[0], size);
		B_THROW_IF_STATUS(err);
		
		value = OSPtr<CFURLRef>(CFURLCreateWithBytes(NULL, &buffer[0], size, 
													 kCFStringEncodingUTF8, NULL),
								from_copy);
	}
	else
	{
		value.Clear();
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeFileURL>::Get(const AEDesc& desc, Url& value, const std::nothrow_t&)
{
	AutoThrowHandler	autoThrow(false);
	OSStatus			err	= noErr;
	
	try
	{
		Get(desc, value);
	}
	catch (std::exception& ex)
	{
		err = ErrorHandler::GetStatus(ex, memFullErr);
	}
	catch (...)
	{
		err = memFullErr;
	}
	
	return (err);

#if 0
	AEDesc		urlDesc	= { typeNull, NULL };
	OSStatus	err		= noErr;
	
	err = AEObjectSupport::CoerceDesc(desc, typeFileURL, urlDesc, std::nothrow);
	
	if (err == noErr)
	{
		CFAllocatorRef	allocator	= CFAllocatorGetDefault();
		size_t			size;
		UInt8*			buff;
		
		size = AEGetDescDataSize(&urlDesc);
		buff = reinterpret_cast<UInt8*>(CFAllocatorAllocate(allocator, size, 0));
		
		if (buff != NULL)
		{
			err = AEGetDescData(&urlDesc, buff, size);
			
			if (err == noErr)
			{
				CFURLRef	url;
				
				url = CFURLCreateWithBytes(NULL, buff, size, 
										   kCFStringEncodingUTF8, 
										   NULL);
				
				if (url != NULL)
				{
					value = OSPtr<CFURLRef>(url, from_copy);
				}
				else
				{
					err = coreFoundationUnknownErr;
				}
			}
			
			CFAllocatorDeallocate(allocator, buff);
		}
		else
		{
			err = memFullErr;
		}
		
		AEDisposeDesc(&urlDesc);
	}
	
	return (err);
#endif
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeFileURL>::Put(AEDesc& desc, const Url& value)
{
	std::string	buffer;
	OSStatus	err;
	
	value.Copy(buffer, true);
	
	err = ::AECreateDesc(typeFileURL, buffer.data(), buffer.size(), &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeFileURL>::Write(AEStreamRef stream, const Url& value)
{
	std::string	buffer;
	OSStatus	err;
	
	value.Copy(buffer, true);
	
	err = AEStreamWriteDesc(stream, typeFileURL, buffer.data(), buffer.size());
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	DescParam<typeData>

#pragma mark -

// ------------------------------------------------------------------------------------------
void
DescParam<typeData>::Get(const AEDesc& desc, OSPtr<CFDataRef>& value)
{
	AEDescriptor	dataDesc;
	
	AEObjectSupport::CoerceDesc(desc, typeData, dataDesc);
	
	size_t	size	= AEGetDescDataSize(dataDesc);
	
	if (size > 0)
	{
		CFAllocatorRef	allocator	= CFAllocatorGetDefault();
		UInt8*			buffer		= reinterpret_cast<UInt8*>(
											CFAllocatorAllocate(allocator, size, 0));
		
		B_THROW_IF_NULL(buffer);
		
		try
		{
			OSStatus	err;
			
			err = AEGetDescData(dataDesc, buffer, size);
			B_THROW_IF_STATUS(err);
			
			value.reset(CFDataCreateWithBytesNoCopy(NULL, buffer, size, allocator), 
						from_copy);
		}
		catch (...)
		{
			CFAllocatorDeallocate(allocator, buffer);
			throw;
		}
	}
	else
	{
		value.reset(CFDataCreate(NULL, NULL, 0), from_copy);
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
DescParam<typeData>::Get(const AEDesc& desc, OSPtr<CFDataRef>& value, const std::nothrow_t&)
{
	AutoThrowHandler	autoThrow(false);
	OSStatus			err	= noErr;
	
	try
	{
		Get(desc, value);
	}
	catch (std::exception& ex)
	{
		err = ErrorHandler::GetStatus(ex, memFullErr);
	}
	catch (...)
	{
		err = memFullErr;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeData>::Put(AEDesc& desc, const OSPtr<CFDataRef>& value)
{
	size_t			size	= CFDataGetLength(value);
	const UInt8*	ptr		= (size > 0) ? CFDataGetBytePtr(value) : NULL;
	OSStatus		err;
	
	err = ::AECreateDesc(typeData, ptr, size, &desc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DescParam<typeData>::Write(AEStreamRef stream, const OSPtr<CFDataRef>& value)
{
	size_t			size	= CFDataGetLength(value);
	const UInt8*	ptr		= (size > 0) ? CFDataGetBytePtr(value) : NULL;
	OSStatus		err;
	
	err = AEStreamWriteDesc(stream, typeData, ptr, size);
	B_THROW_IF_STATUS(err);
}

#endif	// DOXYGEN_SKIP

}	// namespace B
