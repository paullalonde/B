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

#ifndef BToolboxObjectProperty_H_
#define BToolboxObjectProperty_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// standard headers
#include <new>

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BErrorHandler.h"
#include "BRect.h"
#include "BString.h"
#include "BUrl.h"


namespace B {

typedef ::Rect	QDRect;


// ==========================================================================================
//	ToolboxObjectPropertyObjectTraitsArchetype

#pragma mark ToolboxObjectPropertyObjectTraitsArchetype

#if DOXYGEN_SCAN

/*!
	@brief	Traits class for toolbox objects that have properties.
	
	A certain number of toolbox objects have similar APIs for accessing those objects' 
	properties, where a @e property is defined as a client-controlled chunk of arbitrary 
	data, identified by a creator and tag. 
	
	Currently, the following objects have a API for properties:
	
	- Windows (@c WindowRef)
	- Views (@c HIViewRef / @c ControlRef)
	- Menus (@c MenuRef)
	- Collections (@c Collection)
	
	ToolboxObjectPropertyObjectTraitsArchetype is, as the name implies, an archetype 
	class that merely demonstrates the API required to instantiate ToolboxObjectProperty.
*/
struct ToolboxObjectPropertyObjectTraitsArchetype
{
	//! @name Types
	//@{
	typedef typename	ObjectType;	//!< The type representing the toolbox object in function arguments.
	//@}
	
	//! @name Inquiries
	//@{
	
	//! Returns @c true if the property identified by @a creator and @a tag exists within @a obj.
	/*!	@param		obj		The toolbox object.
		@param		creator	The property's owner.
		@param		tag		The property's name.
		@return				@c true if the property exists;  else @c false.
		@exception	none
	*/
	static bool		Exists(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	
	//! Returns the size of the property identified by @a creator and @a tag within @a obj.
	/*!	@param		obj		The toolbox object.
		@param		creator	The property's owner.
		@param		tag		The property's name.
		@return		size_t	The property's size in bytes.
	*/
	static size_t	Size(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	//@}
	
	//! @name Getting / Setting / Removing Properties
	//@{
	
	//! Retrieves the property identified by @a creator and @a tag within @a obj.
	/*!	@param		obj		The toolbox object.
		@param		creator	The property's owner.
		@param		tag		The property's name.
		@param		size	The property's data size in bytes.
		@param		data	A pointer to the property's data.
		@return				System result code.
		@exception	none
	*/
	static OSStatus	Get(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag, 
						size_t		size, 
						void*		data);
	
	//! Sets the property identified by @a creator and @a tag within @a obj.
	/*!	@param		obj		The toolbox object.
		@param		creator	The property's owner.
		@param		tag		The property's name.
		@param		size	The property's data size in bytes.
		@param		data	A pointer to the property's data.
	*/
	static void		Set(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag, 
						size_t		size, 
						const void*	data);
	
	//! Removes the property identified by @a creator and @a tag within @a obj.
	/*!	@param		obj		The toolbox object.
		@param		creator	The property's owner.
		@param		tag		The property's name.
	*/
	static void		Remove(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	//@}
	
	//! @name Attributes
	//@{
	
	//! Returns the attributes of the property identified by @a creator and @a tag within @a obj.
	/*!	@param		obj		The toolbox object.
		@param		creator	The property's owner.
		@param		tag		The property's name.
		@return		UInt32	The property's attributes.
	*/
	static UInt32	GetAttrs(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	
	//! Changes the attributes of the property identified by @a creator and @a tag within @a obj.
	/*!	@param		obj				The toolbox object.
		@param		creator			The property's owner.
		@param		tag				The property's name.
		@param		setAttributes	The property attributes to set.
		@param		clearAttributes	The property attributes to clear.
	*/
	static void		SetAttrs(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag, 
						UInt32		setAttributes, 
						UInt32		clearAttributes);
	//@}
};
#endif


// ==========================================================================================
//	ToolboxObjectPropertyTypeTrait

#pragma mark -
#pragma mark ToolboxObjectPropertyTypeTrait

/*!
	@brief	Traits class for toolbox object properties.
	
	This class allows clients to get & set the properties of toolbox objects in a 
	type-safe manner.  The default implementation simply performs a bitwise copy 
	into and out of the toolbox objet.  However, this behaviour may be overriden by 
	specialising this class.
	
	The template parameter, @a T, is a C/C++ language type.  It's the type of the data to 
	be stored into or retrieved from the toolbox object.  The property data is passed to 
	and returned from as the PropertyType typedef, which by default is a synonym for 
	@a T.  Again this may be changed by specialising the template.
*/
template <typename T, class OBJTR>
struct ToolboxObjectPropertyTypeTrait
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	T;		//!< Template parameter.  Any C/C++ language type.
	typedef typename	OBJTR;	//!< Template parameter.  The traits class for the toolbox object.  See ToolboxObjectPropertyObjectTraitsArchetype.
	//@}
#endif
	
	//! @name Types
	//@{
	typedef T									PropertyType;	//!< The type representing the property data in function arguments.
	typedef OBJTR								ObjectTraits;	//!< The traits for the toolbox object type.
	typedef typename ObjectTraits::ObjectType	ObjectType;		//!< The type representing the toolbox object in function arguments.
	//@}
	
	//! @name Getting & Setting Properties
	//@{
	//!	Retrieves the property with the given creator and tag.
	static PropertyType	Get(
							ObjectType			obj,
							OSType				creator, 
							OSType				tag);
	
	//!	Retrieves the property with the given creator and tag, without throwing an exception.
	static bool			Get(
							ObjectType			obj,
							OSType				creator, 
							OSType				tag, 
							PropertyType&		value);
	
	//! Changes the property with the given creator and tag.
	static void			Set(
							ObjectType			obj, 
							OSType				creator, 
							OSType				tag, 
							const PropertyType&	value);
	//@}
};


// ------------------------------------------------------------------------------------------
/*!	@return		The property's value.
*/
template <typename T, class OBJTR> inline
typename ToolboxObjectPropertyTypeTrait<T, OBJTR>::PropertyType
ToolboxObjectPropertyTypeTrait<T, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	PropertyType	value;
	OSStatus		err;
	
	err = ObjectTraits::Get(obj, creator, tag, sizeof(value), &value);
	B_THROW_IF_STATUS(err);
	
	return (value);
}

// ------------------------------------------------------------------------------------------
/*!	@return		@c true if the property exists;  else @c false.
	@exception	none
	@note		The function assumes that @a PropertyType's copy-assignment operator doesn't 
				throw.
*/
template <typename T, class OBJTR> inline bool
ToolboxObjectPropertyTypeTrait<T, OBJTR>::Get(
	ObjectType		obj,		//!< The toolbox object.
	OSType			creator,	//!< The property's owner.
	OSType			tag,		//!< The property's name.
	PropertyType&	value)		//!< The property's value.
{
	OSStatus	err;
	
	err = ObjectTraits::Get(obj, creator, tag, sizeof(value), &value);
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
template <typename T, class OBJTR> inline void
ToolboxObjectPropertyTypeTrait<T, OBJTR>::Set(
	ObjectType			obj, 		//!< The toolbox object.
	OSType				creator,	//!< The property's owner.
	OSType				tag, 		//!< The property's name.
	const PropertyType&	value)		//!< The new value.
{
	ObjectTraits::Set(obj, creator, tag, sizeof(value), &value);
}


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//	ToolboxObjectPropertyTypeTrait<QDRect, OBJTR>

#pragma mark -
#pragma mark ToolboxObjectPropertyTypeTrait<QDRect, OBJTR>

/*!
	@brief	Template specialisation of ToolboxObjectPropertyTypeTrait for @c ::Rect.
	
	Converts to and from @c ::Rect (the type actually read/written to the toolbox object) 
	and Rect, the C/C++ language type passed to the member functions.
*/
template <class OBJTR>
struct ToolboxObjectPropertyTypeTrait<QDRect, OBJTR>
{
	typedef Rect								PropertyType;
	typedef OBJTR								ObjectTraits;
	typedef typename ObjectTraits::ObjectType	ObjectType;
	
	static Rect		Get(ObjectType obj, OSType creator, OSType tag);
	static bool		Get(ObjectType obj, OSType creator, OSType tag, Rect& value);
	static void		Set(ObjectType obj, OSType creator, OSType tag, const Rect& value);
};

// ------------------------------------------------------------------------------------------
template <class OBJTR> Rect
ToolboxObjectPropertyTypeTrait<QDRect, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	::Rect		qdRect;
	OSStatus	err;
	
	err = ObjectTraits::Get(obj, creator, tag, sizeof(qdRect), &qdRect);
	B_THROW_IF_STATUS(err);
	
	return (Rect(qdRect));
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> bool
ToolboxObjectPropertyTypeTrait<QDRect, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	Rect&		value)		//!< The property's value.
{
	::Rect	qdRect;
	bool	good;
	
	if ((good = (ObjectTraits::Get(obj, creator, tag, sizeof(qdRect), &qdRect) == noErr)))
		value = qdRect;
	
	return (good);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> void
ToolboxObjectPropertyTypeTrait<QDRect, OBJTR>::Set(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	const Rect&	value)		//!< The new value.
{
	::Rect	qdRect	= value;
	
	ObjectTraits::Set(obj, creator, tag, sizeof(qdRect), &qdRect);
}


// ==========================================================================================
//	ToolboxObjectPropertyTypeTrait<Boolean, OBJTR>

#pragma mark -
#pragma mark ToolboxObjectPropertyTypeTrait<Boolean, OBJTR>

/*!
	@brief	Template specialisation of ToolboxObjectPropertyTypeTrait for @c Boolean.
	
	Converts to and from @c Boolean (the type actually read/written to the toolbox object) 
	and @c bool, the C/C++ language type passed to the member functions.
*/
template <class OBJTR>
struct ToolboxObjectPropertyTypeTrait<Boolean, OBJTR>
{
	typedef bool								PropertyType;
	typedef OBJTR								ObjectTraits;
	typedef typename ObjectTraits::ObjectType	ObjectType;
	
	static bool		Get(ObjectType obj, OSType creator, OSType tag);
	static bool		Get(ObjectType obj, OSType creator, OSType tag, bool& value);
	static void		Set(ObjectType obj, OSType creator, OSType tag, bool value);
};

// ------------------------------------------------------------------------------------------
template <class OBJTR> bool
ToolboxObjectPropertyTypeTrait<Boolean, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	Boolean		boolVal;
	OSStatus	err;
	
	err = ObjectTraits::Get(obj, creator, tag, sizeof(boolVal), &boolVal);
	B_THROW_IF_STATUS(err);
	
	return (boolVal);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> bool
ToolboxObjectPropertyTypeTrait<Boolean, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	bool&		value)		//!< The property's value.
{
	Boolean	boolVal;
	bool	good	= (ObjectTraits::Get(obj, creator, tag, sizeof(boolVal), &boolVal) == noErr);
	
	if (good)
		value = boolVal;
	
	return (good);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> void
ToolboxObjectPropertyTypeTrait<Boolean, OBJTR>::Set(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	bool		value)		//!< The new value.
{
	Boolean	boolVal	= value;
	
	ObjectTraits::Set(obj, creator, tag, sizeof(boolVal), &boolVal);
}


// ==========================================================================================
//	ToolboxObjectPropertyTypeTrait<CFStringRef, OBJTR>

#pragma mark -
#pragma mark ToolboxObjectPropertyTypeTrait<CFStringRef, OBJTR>

/*!
	@brief	Template specialisation of ToolboxObjectPropertyTypeTrait for @c CFStringRef.
	
	Converts to and from @c CFStringRef (the type actually read/written to the toolbox 
	object) and String, the C/C++ language type passed to the member functions.
	
	@internal	The Get() functions copy the property's contents into a temporary 
				std::vector, then assign the vector to the result String.  This implies 
				that the code has to special-case a property size of zero, because you 
				can't access the underlying storage of a zero-length vector.
*/
template <class OBJTR>
struct ToolboxObjectPropertyTypeTrait<CFStringRef, OBJTR>
{
	typedef String								PropertyType;
	typedef OBJTR								ObjectTraits;
	typedef typename ObjectTraits::ObjectType	ObjectType;
	
	static String	Get(ObjectType obj, OSType creator, OSType tag);
	static bool		Get(ObjectType obj, OSType creator, OSType tag, String& value);
	static void		Set(ObjectType obj, OSType creator, OSType tag, const String& value);
};

// ------------------------------------------------------------------------------------------
template <class OBJTR> String
ToolboxObjectPropertyTypeTrait<CFStringRef, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	size_t	size	= ObjectTraits::Size(obj, creator, tag);
	String	value;
	
	if (size > 0)
	{
		std::vector<UInt8>	buff(size);
		OSStatus			err;
		
		err = ObjectTraits::Get(obj, creator, tag, size, &buff[0]);
		B_THROW_IF_STATUS(err);
		
		value.assign(buff, kCFStringEncodingUnicode, true);
	}
	
	return (value);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> bool
ToolboxObjectPropertyTypeTrait<CFStringRef, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	String&		value)		//!< The property's value.
{
	bool	good	= false;
	
	// We go out of our way to call member functions of ObjectTraits in such a way 
	// as to avoid throwing.  However, because we call other functions that may 
	// throw (eg std::vector(), String::assign()) we need to catch everything.
	
	try
	{
		// Exists() doesn't throw, so use it to check whether we should go any further.
		
		if (ObjectTraits::Exists(obj, creator, tag))
		{
			size_t	size	= ObjectTraits::Size(obj, creator, tag);
			
			if (size > 0)
			{
				std::vector<UInt8>	buff(size);
				
				if (ObjectTraits::Get(obj, creator, tag, size, &buff[0]) == noErr)
				{
					value.assign(buff, kCFStringEncodingUnicode, true);
					good = true;
				}
			}
			else
			{
				value.clear();
				good = true;
			}
		}
	}
	catch (...)
	{
		good = false;
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> void
ToolboxObjectPropertyTypeTrait<CFStringRef, OBJTR>::Set(
	ObjectType		obj, 		//!< The toolbox object.
	OSType			creator, 	//!< The property's owner.
	OSType			tag,		//!< The property's name.
	const String&	value)		//!< The new value.
{
	std::vector<UInt8>	buff;
	
	value.copy(buff, kCFStringEncodingUnicode, true);
	
	ObjectTraits::Set(obj, creator, tag, buff.size(), &buff[0]);
}


// ==========================================================================================
//	ToolboxObjectPropertyTypeTrait<CFURLRef, OBJTR>

#pragma mark -
#pragma mark ToolboxObjectPropertyTypeTrait<CFURLRef, OBJTR>

/*!
	@brief	Template specialisation of ToolboxObjectPropertyTypeTrait for @c CFURLRef.
	
	Converts to and from @c CFURLRef (the type actually read/written to the toolbox 
	object) and Url, the C/C++ language type passed to the member functions.
	
	@internal	The Get() functions copy the property's contents into a temporary 
				std::vector, then assign the vector to the result Url.  This implies 
				that the code has to special-case a property size of zero, because you 
				can't access the underlying storage of a zero-length vector.
*/
template <class OBJTR>
struct ToolboxObjectPropertyTypeTrait<CFURLRef, OBJTR>
{
	typedef Url									PropertyType;
	typedef OBJTR								ObjectTraits;
	typedef typename ObjectTraits::ObjectType	ObjectType;
	
	static Url		Get(ObjectType obj, OSType creator, OSType tag);
	static bool		Get(ObjectType obj, OSType creator, OSType tag, Url& value);
	static void		Set(ObjectType obj, OSType creator, OSType tag, const Url& value);
};

// ------------------------------------------------------------------------------------------
template <class OBJTR> Url
ToolboxObjectPropertyTypeTrait<CFURLRef, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	size_t	size	= ObjectTraits::Size(obj, creator, tag);
	Url		value;
	
	if (size > 0)
	{
		std::vector<UInt8>	buff(size);
		OSStatus			err;
		
		err = ObjectTraits::Get(obj, creator, tag, size, &buff[0]);
		B_THROW_IF_STATUS(err);
		
		value.Assign(buff);
	}
	
	return (value);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> bool
ToolboxObjectPropertyTypeTrait<CFURLRef, OBJTR>::Get(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	Url&		value)		//!< The property's value.
{
	bool	good	= false;
	
	// We go out of our way to call member functions of ObjectTraits in such a way 
	// as to avoid throwing.  However, because we call other functions that may 
	// throw (eg std::vector(), Url::Assign()) we need to catch everything.
	
	try
	{
		// Exists() doesn't throw, so use it to check whether we should go any further.
		
		if (ObjectTraits::Exists(obj, creator, tag))
		{
			size_t	size	= ObjectTraits::Size(obj, creator, tag);
			
			if (size > 0)
			{
				std::vector<UInt8>	buff(size);
				
				if (ObjectTraits::Get(obj, creator, tag, size, &buff[0]) == noErr)
				{
					value.Assign(buff);
					good = true;
				}
			}
			else
			{
				value.Clear();
				good = true;
			}
		}
	}
	catch (...)
	{
		good = false;
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
template <class OBJTR> void
ToolboxObjectPropertyTypeTrait<CFURLRef, OBJTR>::Set(
	ObjectType	obj, 		//!< The toolbox object.
	OSType		creator, 	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	const Url&	value)		//!< The new value.
{
	std::vector<UInt8>	buff;
	
	if (!value.Empty())
		value.Copy(buff, true);
	
	ObjectTraits::Set(obj, creator, tag, buff.size(), &buff[0]);
}

#endif	// DOXYGEN_SKIP


// ==========================================================================================
//	ToolboxObjectPropertyBase

#pragma mark -
#pragma mark ToolboxObjectPropertyBase

/*!
	@brief	Helper class that serves as the common ancestor of instantiations of 
			ToolboxObjectProperty.
	
	ToolboxObjectProperty needs to call some functions in the toolbox object 
	which are independent of any property data type, so in the interest of saving a 
	bit of space (as well as reducing the work required to specialise 
	ToolboxObjectProperty), those functions have been factored out into this 
	little helper class.
*/
template <OSType SIG, OSType TAG, class OBJTR>
struct ToolboxObjectPropertyBase
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef OSType		SIG;	//!< Template parameter.  See ToolboxObjectPropertyBaseDef.
	typedef OSType		TAG;	//!< Template parameter.  See ToolboxObjectPropertyBaseDef.
	typedef typename	OBJTR;	//!< Template parameter.  The traits class for the toolbox object.  See ToolboxObjectPropertyObjectTraitsArchetype.
	//@}
#endif

	//! @name Types
	//@{
	typedef OBJTR								ObjectTraits;	//!< The traits for the toolbox object type.
	typedef typename ObjectTraits::ObjectType	ObjectType;		//!< The type representing the toolbox object in function arguments.
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns @c true if the property identified by @a SIG and @a TAG exists within @a obj.
	static bool		Exists(
						ObjectType	obj);
	//! Returns the size of the property identified by @a SIG and @a TAG within @a obj.
	static size_t	Size(
						ObjectType	obj);
	//@}
	
	//! @name Getting / Setting / Removing Properties
	//@{
	//! Removes the property identified by @a SIG and @a TAG within @a obj.
	static void		Remove(
						ObjectType	obj);
	//@}
	
	//! @name Attributes
	//@{
	//! Returns the attributes of the property identified by @a SIG and @a TAG within @a obj.
	static UInt32	GetAttrs(
						ObjectType	obj);
	//! Changes the attributes of the property identified by @a SIG and @a TAG within @a obj.
	static void		SetAttrs(
						ObjectType	obj, 
						UInt32		setAttributes, 
						UInt32		clearAttributes);
	//@}
};

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, class OBJTR> inline bool
ToolboxObjectPropertyBase<SIG, TAG, OBJTR>::Exists(
	ObjectType	obj)			//!< The toolbox object.
{
	return (ObjectTraits::Exists(obj, SIG, TAG));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, class OBJTR> inline size_t
ToolboxObjectPropertyBase<SIG, TAG, OBJTR>::Size(
	ObjectType	obj)			//!< The toolbox object.
{
	return (ObjectTraits::Size(obj, SIG, TAG));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, class OBJTR> inline void
ToolboxObjectPropertyBase<SIG, TAG, OBJTR>::Remove(
	ObjectType	obj)			//!< The toolbox object.
{
	ObjectTraits::Remove(obj, SIG, TAG);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, class OBJTR> inline UInt32
ToolboxObjectPropertyBase<SIG, TAG, OBJTR>::GetAttrs(
	ObjectType	obj)			//!< The toolbox object.
{
	return (ObjectTraits::GetAttrs(obj, SIG, TAG));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, class OBJTR> inline void
ToolboxObjectPropertyBase<SIG, TAG, OBJTR>::SetAttrs(
	ObjectType	obj,				//!< The toolbox object.
	UInt32		setAttributes,		//!< The attributes to set.
	UInt32		clearAttributes)	//!< The attributes to clear.
{
	ObjectTraits::SetAttrs(obj, SIG, TAG, setAttributes, clearAttributes);
}


// ==========================================================================================
//	ToolboxObjectProperty

#pragma mark -
#pragma mark ToolboxObjectProperty

/*!
	@brief	Accesses a toolbox object property in a type-safe manner.
	
	ToolboxObjectProperty allows one to access a specific property (identified by the 
	@a SIG and @a TAG template parameters) of a specific toolbox object (whose type is 
	identified indirectly by the @a OBJTR template parameter) in a type-safe 
	manner (this safety is provided indirectly by the @a T template parameter).
	
	Although the class can be used directly, it's really meant to be subclassed and/or 
	specialised for the various concrete toolbox objects (@c WindowRef, etc).
	
	The individual property within the toolbox object is identified the @a SIG and @a TAG 
	template parameters, as well as the @c ObjectType argument given at runtime.
	
	The type-safety of the property data is provided by the ToolboxObjectPropertyTypeTrait 
	class template, which is accessed via the @a T template parameter.
	
	The type-safety of the toolbox object is provided by the OBJTR class, 
	ToolboxObjectProperty constructs a quadruple <SIG, TAG, T, OBJTR> as described 
	in ToolboxObjectPropertyObjectTraitsArchetype.
	
	
	ToolboxObjectProperty utilises ToolboxObjectPropertyDef to actually retrieve & store the data.  The 
	particular instantiation of ToolboxObjectPropertyDef is determined by the two template 
	parameters.  See ToolboxObjectPropertyDef for more information.
*/
template <OSType SIG, OSType TAG, typename T, class OBJTR>
struct ToolboxObjectProperty : public ToolboxObjectPropertyBase<SIG, TAG, OBJTR>
{
private:
	
	typedef ToolboxObjectPropertyBase<SIG, TAG, OBJTR>	inherited;
	
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef OSType		SIG;	//!< Template parameter.  The creator to use when accessing the property.
	typedef OSType		TAG;	//!< Template parameter.  The tag to use when accessing the property.
	typedef typename	T;		//!< Template parameter.  Any C/C++ language type.
	typedef typename	OBJTR;	//!< Template parameter.  The traits class for the toolbox object.  See ToolboxObjectPropertyObjectTraitsArchetype.
	//@}
#endif

	//! @name Types
	//@{
	typedef typename inherited::ObjectTraits			ObjectTraits;	//!< The traits for the toolbox object type.
	typedef typename inherited::ObjectType				ObjectType;		//!< The type representing the toolbox object in function arguments.
	typedef ToolboxObjectPropertyTypeTrait<T, OBJTR>	TypeTraits;		//!< The traits for C/C++ language type of the property.
	typedef typename TypeTraits::PropertyType			PropertyType;	//!< The type representing the property data in function arguments.  See ToolboxObjectPropertyTypeTrait.
	//@}
	
	//! @name Getting / Setting / Removing Properties
	//@{
	//! Retrieves the property identified by @a SIG and @a TAG within @a obj.
	static PropertyType	Get(
						ObjectType		obj);
	/*! @overload */
	static void		Get(
						ObjectType		obj, 
						PropertyType&	value);
	/*! @overload */
	static bool		Get(
						ObjectType		obj, 
						PropertyType&	value, 
						const std::nothrow_t&);
	//! Sets the property identified by @a SIG and @a TAG within @a obj.
	static void		Set(
						ObjectType		obj, 
						const PropertyType&	value);
	//@}
};

// ------------------------------------------------------------------------------------------
/*!	@return		The property's value.
*/
template <OSType SIG, OSType TAG, typename T, class OBJTR> inline
typename ToolboxObjectProperty<SIG, TAG, T, OBJTR>::PropertyType
ToolboxObjectProperty<SIG, TAG, T, OBJTR>::Get(
	ObjectType	obj)			//!< The toolbox object.
{
	return (TypeTraits::Get(obj, SIG, TAG));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T, class OBJTR> inline void
ToolboxObjectProperty<SIG, TAG, T, OBJTR>::Get(
	ObjectType		obj,			//!< The toolbox object.
	PropertyType&	value)			//!< Holds the output property value.
{
	value = TypeTraits::Get(obj, SIG, TAG);
}

// ------------------------------------------------------------------------------------------
/*!	@return		@c true if the property was retrieved successfully;  else @c false.
	@exception	none
*/
template <OSType SIG, OSType TAG, typename T, class OBJTR> bool
ToolboxObjectProperty<SIG, TAG, T, OBJTR>::Get(
	ObjectType		obj,		//!< The toolbox object.
	PropertyType&	value,		//!< Holds the output property value.
	const std::nothrow_t&)		//!< An indication that the caller doesn't want to have the function throw an exception.
{
	return (TypeTraits::Get(obj, SIG, TAG, value));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T, class OBJTR> inline void
ToolboxObjectProperty<SIG, TAG, T, OBJTR>::Set(
	ObjectType			obj,		//!< The toolbox object.
	const PropertyType&	value)		//!< The new value.
{
	TypeTraits::Set(obj, SIG, TAG, value);
}

}	// namespace B


#endif	// BToolboxObjectProperty_H_
