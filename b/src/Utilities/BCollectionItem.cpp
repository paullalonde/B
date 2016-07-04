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
#include "BCollectionItem.h"


namespace B {


// ==========================================================================================
//	ToolboxObjectPropertyCollectionObjectTraits

// ------------------------------------------------------------------------------------------
/*!	@return		Boolean	@c true if the property exists;  else @c false.
	@exception	none
*/
bool
ToolboxObjectPropertyCollectionObjectTraits::Exists(
	ObjectType	obj, 		//!< The collection.
	OSType		creator, 	//!< The property's owner.
	OSType		tag) 		//!< The property's name.
{
	OSStatus	err;
	
	err = GetCollectionItemInfo(obj, tag, creator, 
								reinterpret_cast<SInt32*>(kCollectionDontWantIndex), 
								reinterpret_cast<SInt32*>(kCollectionDontWantSize), 
								reinterpret_cast<SInt32*>(kCollectionDontWantAttributes));
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
/*!	@return		size_t	The property's size in bytes.
*/
size_t
ToolboxObjectPropertyCollectionObjectTraits::Size(
	ObjectType	obj, 		//!< The collection.
	OSType		creator, 	//!< The property's owner.
	OSType		tag) 		//!< The property's name.
{
	SInt32		size;
	OSStatus	err;
	
	err = GetCollectionItemInfo(obj, tag, creator, 
								reinterpret_cast<SInt32*>(kCollectionDontWantIndex), 
								&size, 
								reinterpret_cast<SInt32*>(kCollectionDontWantAttributes));
	B_THROW_IF_STATUS(err);
	
	return (size);
}

// ------------------------------------------------------------------------------------------
OSStatus
ToolboxObjectPropertyCollectionObjectTraits::Get(
	ObjectType	obj, 		//!< The collection.
	OSType		creator,	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	size_t		size,		//!< The property's data size in bytes.
	void*		data)		//!< A pointer to the property's data.
{
	SInt32		actualSize	= size;
	OSStatus	err;
	
	err = GetCollectionItem(obj, tag, creator, &actualSize, data);
	if ((err == noErr) && (size != static_cast<size_t>(actualSize)))
		err = errDataSizeMismatch;
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyCollectionObjectTraits::Set(
	ObjectType	obj, 		//!< The collection.
	OSType		creator,	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	size_t		size,		//!< The property's data size in bytes.
	const void*	data)		//!< A pointer to the property's data.
{
	OSStatus	err;
	
	err = AddCollectionItem(obj, tag, creator, size, data);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyCollectionObjectTraits::Remove(
	ObjectType	obj, 		//!< The collection.
	OSType		creator,	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	OSStatus	err;
	
	err = RemoveCollectionItem(obj, tag, creator);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	@return		UInt32	The property's attributes.
*/
UInt32
ToolboxObjectPropertyCollectionObjectTraits::GetAttrs(
	ObjectType	obj,		//!< The collection.
	OSType		creator,	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	SInt32		attributes;
	OSStatus	err;
	
	err = GetCollectionItemInfo(obj, tag, creator, 
								reinterpret_cast<SInt32*>(kCollectionDontWantIndex), 
								reinterpret_cast<SInt32*>(kCollectionDontWantSize), 
								&attributes);
	B_THROW_IF_STATUS(err);
	
	return (attributes);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyCollectionObjectTraits::SetAttrs(
	ObjectType	obj, 			//!< The collection.
	OSType		creator,		 	//!< The property's owner.
	OSType		tag, 				//!< The property's name.
	UInt32		setAttributes,		//!< The attributes to set.
	UInt32		clearAttributes)	//!< The attributes to clear.
{
	OSStatus	err;
	
	err = SetCollectionItemInfo(obj, tag, creator, 
								setAttributes | clearAttributes, 
								setAttributes);
	B_THROW_IF_STATUS(err);
}

}	// namespace B
