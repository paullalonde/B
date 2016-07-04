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

#ifndef BCollectionItem_H_
#define BCollectionItem_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// B headers
#include "BToolboxObjectProperty.h"


namespace B {


// ==========================================================================================
//	ToolboxObjectPropertyCollectionObjectTraits

#pragma mark ToolboxObjectPropertyCollectionObjectTraits

/*!	@brief	ToolboxObjectPropertyObjectTraits template specialisation for @c WindowRef.
	
	Converts to and from @c Boolean (the type actually read/written to the window) and 
	@c bool, the C/C++ language type passed to the member functions.
*/
struct ToolboxObjectPropertyCollectionObjectTraits
{
	//! @name Types
	//@{
	typedef Collection	ObjectType;	//!< The type representing the toolbox object in function arguments.
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns @c true if the property identified by @a creator and @a tag exists within @a obj.
	static bool		Exists(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	//! Returns the size of the property identified by @a creator and @a tag within @a obj.
	static size_t	Size(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	//@}
	
	//! @name Getting / Setting / Removing Properties
	//@{
	//! Retrieves the property identified by @a creator and @a tag within @a obj.
	static OSStatus	Get(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag, 
						size_t		size, 
						void*		data);
	//! Sets the property identified by @a creator and @a tag within @a obj.
	static void		Set(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag, 
						size_t		size, 
						const void*	data);
	//! Removes the property identified by @a creator and @a tag within @a obj.
	static void		Remove(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	//@}
	
	//! @name Attributes
	//@{
	//! Returns the attributes of the property identified by @a creator and @a tag within @a obj.
	static UInt32	GetAttrs(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag);
	//! Changes the attributes of the property identified by @a creator and @a tag within @a obj.
	static void		SetAttrs(
						ObjectType	obj, 
						OSType		creator, 
						OSType		tag, 
						UInt32		setAttributes, 
						UInt32		clearAttributes);
	//@}
};


// ==========================================================================================
//	CollectionItem

#pragma mark -
#pragma mark CollectionItem

/*!
	@brief	Gets & sets a collection item in a type-safe manner.
	
	CollectionItem allows one to access a specific collection item (identified by the 
	@a SIG and @a TAG template parameters) of a @c Collection in a type-safe 
	manner (this safety is provided indirectly by the @a T template parameter).
	
	The idea is to allow for easy definition of properties in a type-safe manner.  For 
	example, if a client of a @c Collection needs to associate a URL with one of its 
	items, it can simply declare a typedef for it:
	
	@code
	typedef B::CollectionItem<kMySig, kMyURLTag, CFURLRef>	UrlCollectionItem;
	@endcode
	
	The client can then get & set the property:
	
	@code
	Collection myCollection1, myCollection2;
	B::Url     myUrl;
	
	myUrl = UrlCollectionItem::Get(myCollection1);
	UrlCollectionItem::Set(myCollection2, myUrl);
	@endcode
*/
template <OSType SIG, OSType TAG, typename T>
struct CollectionItem
	: public ToolboxObjectProperty<SIG, TAG, T, ToolboxObjectPropertyCollectionObjectTraits>
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef OSType		SIG;	//!< Template parameter.  The creator to use when accessing the property.
	typedef OSType		TAG;	//!< Template parameter.  The tag to use when accessing the property.
	typedef typename	T;		//!< Template parameter.  Any C/C++ language type.
	//@}
#endif
};


// ==========================================================================================
//	StandardViewCollectionItemDef

/*!
	@brief	Maps a standard collection tag onto a C/C++ language type.
	
	The Control Manager defines a number of collection tags that are used to communicate 
	configuration information to a control as it is being initialised.  
	StandardViewCollectionItemDef maps these standard tags onto a C/C++ language type.
	
	Note that the generic definition of StandardViewCollectionItemDef is deliberately 
	useless, because StandardViewCollectionItemDef is meant to be specialised for each 
	pre-defined value of @a TAG.
*/
template <CollectionTag TAG>
struct StandardViewCollectionItemDef
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef CollectionTag	TAG;	//!< Template parameter.  An integral four-char constant containing the collection item's tag.
	//@}
	
	//! @name Types
	//@{
	typedef typename		Type;	//!< Any C/C++ language type.
	//@}
#endif
};


// ==========================================================================================
//	Template specialisations of StandardViewCollectionItemDef<>

#ifndef DOXYGEN_SKIP

template <> struct StandardViewCollectionItemDef<kControlCollectionTagBounds>		{ typedef QDRect		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagValue>		{ typedef SInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagMinimum>		{ typedef SInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagMaximum>		{ typedef SInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagViewSize>		{ typedef SInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagVisibility>	{ typedef Boolean		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagRefCon>		{ typedef SInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagUnicodeTitle>	{ typedef CFStringRef	Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagIDSignature>	{ typedef OSType		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagIDID>			{ typedef SInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagCommand>		{ typedef UInt32		Type;	};
template <> struct StandardViewCollectionItemDef<kControlCollectionTagVarCode>		{ typedef SInt16		Type;	};

#endif	// DOXYGEN_SKIP


// ==========================================================================================
//	StandardViewCollectionItem

#pragma mark -
#pragma mark StandardViewCollectionItem

/*!
	@brief	Gets & sets a standard view-related collection item in a type-safe manner.
	
	StandardViewCollectionItem allows one to access collection items defined by the 
	Control Manager in a type-safe manner (this safety is provided indirectly by the 
	@a T template parameter).  These collection items are typically used to communicate 
	configuration information to a control as it is being initialised.
	
	The idea is to allow for easy use of these standard collection items.  For example, 
	if a client of a control needs to initialise it with a specific value, it can simply 
	declare a typedef for it:
	
	@code
	typedef B::StandardViewCollectionItem<kControlCollectionTagValue>	ValueCollectionItem;
	@endcode
	
	The client can then simply add the value to the @c Collection:
	
	@code
	Collection myInitColl;
	
	ValueCollectionItem::Set(myInitColl, 50);
	@endcode
*/
template <CollectionTag TAG>
struct StandardViewCollectionItem
	: public ToolboxObjectProperty<
				0, TAG, 
				typename StandardViewCollectionItemDef<TAG>::Type, 
				ToolboxObjectPropertyCollectionObjectTraits>
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef CollectionTag	TAG;	//!< Template parameter.  An integral four-char constant containing the collection item's tag.
	//@}
#endif
};

}	// namespace B


#endif	// BCollectionItem_H_
