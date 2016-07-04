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
#include "BMenuItemProperty.h"


namespace B {


// ==========================================================================================
//	ToolboxObjectPropertyMenuItemObjectTraits

// ------------------------------------------------------------------------------------------
/*!	@return		Boolean	@c true if the property exists;  else @c false.
	@exception	none
*/
bool
ToolboxObjectPropertyMenuItemObjectTraits::Exists(
	ObjectType	obj,		//!< The menu/item pair.
	OSType		creator, 	//!< The property's owner.
	OSType		tag) 		//!< The property's name.
{
	UInt32		size;
	OSStatus	err;
	
	err = GetMenuItemPropertySize(obj.mMenu, obj.mIndex, creator, tag, &size);
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
/*!	@return		size_t	The property's size in bytes.
*/
size_t
ToolboxObjectPropertyMenuItemObjectTraits::Size(
	ObjectType	obj,		//!< The menu/item pair.
	OSType		creator, 	//!< The property's owner.
	OSType		tag) 		//!< The property's name.
{
	UInt32		size;
	OSStatus	err;
	
	err = GetMenuItemPropertySize(obj.mMenu, obj.mIndex, creator, tag, &size);
	B_THROW_IF_STATUS(err);
	
	return (size);
}

// ------------------------------------------------------------------------------------------
OSStatus
ToolboxObjectPropertyMenuItemObjectTraits::Get(
	ObjectType	obj,		//!< The menu/item pair.
	OSType		creator,	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	size_t		size,		//!< The property's data size in bytes.
	void*		data)		//!< A pointer to the property's data.
{
	UInt32		actualSize	= size;
	OSStatus	err;
	
	err = GetMenuItemProperty(obj.mMenu, obj.mIndex, creator, tag, size, &actualSize, data);
	if ((err == noErr) && (size != actualSize))
		err = errDataSizeMismatch;
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyMenuItemObjectTraits::Set(
	ObjectType	obj,		//!< The menu/item pair.
	OSType		creator,	//!< The property's owner.
	OSType		tag,		//!< The property's name.
	size_t		size,		//!< The property's data size in bytes.
	const void*	data)		//!< A pointer to the property's data.
{
	OSStatus	err;
	
	err = SetMenuItemProperty(obj.mMenu, obj.mIndex, creator, tag, size, data);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyMenuItemObjectTraits::Remove(
	ObjectType	obj,		//!< The menu/item pair.
	OSType		creator,	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	OSStatus	err;
	
	err = RemoveMenuItemProperty(obj.mMenu, obj.mIndex, creator, tag);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	@return		UInt32	The property's attributes.
*/
UInt32
ToolboxObjectPropertyMenuItemObjectTraits::GetAttrs(
	ObjectType	obj,		//!< The menu/item pair.
	OSType		creator,	//!< The property's owner.
	OSType		tag)		//!< The property's name.
{
	UInt32		attributes;
	OSStatus	err;
	
	err = GetMenuItemPropertyAttributes(obj.mMenu, obj.mIndex, creator, tag, &attributes);
	B_THROW_IF_STATUS(err);
	
	return (attributes);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyMenuItemObjectTraits::SetAttrs(
	ObjectType	obj,				//!< The menu/item pair.
	OSType		creator,		 	//!< The property's owner.
	OSType		tag, 				//!< The property's name.
	UInt32		setAttributes,		//!< The attributes to set.
	UInt32		clearAttributes)	//!< The attributes to clear.
{
	OSStatus	err;
	
	err = ChangeMenuItemPropertyAttributes(obj.mMenu, obj.mIndex, creator, tag, 
										   setAttributes | clearAttributes, 
										   setAttributes);
	B_THROW_IF_STATUS(err);
}

}	// namespace B
