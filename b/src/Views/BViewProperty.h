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

#ifndef BViewProperty_H_
#define BViewProperty_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// B headers
#include "BToolboxObjectProperty.h"


namespace B {


// ==========================================================================================
//	ToolboxObjectPropertyViewObjectTraits

#pragma mark ToolboxObjectPropertyViewObjectTraits

/*!
	@brief	Toolbox object traits class for @c HIViewRef.
	
	Supplies the interface to an @c HIViewRef's properties as required by 
	ToolboxObjectProperty.
*/
struct ToolboxObjectPropertyViewObjectTraits
{
	//! @name Types
	//@{
	typedef HIViewRef	ObjectType;	//!< The type representing the toolbox object in function arguments.
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
//	ViewProperty

#pragma mark -
#pragma mark ViewProperty

/*!
	@brief	Gets & sets a view property in a type-safe manner.
	
	ViewProperty allows one to access a specific property (identified by the 
	@a SIG and @a TAG template parameters) of an @c HIViewRef in a type-safe 
	manner (this safety is provided indirectly by the @a T template parameter).
	
	The idea is to allow for easy definition of properties in a type-safe manner.  For 
	example, if a client of an @c HIViewRef needs to associate a URL with a view, it can 
	simply declare a typedef for it:
	
	@code
	typedef B::ViewProperty<kMySig, kMyURLTag, CFURLRef>	UrlViewProperty;
	@endcode
	
	The client can then get & set the property:
	
	@code
	HIViewRef  myView1, myView2;
	B::Url     myUrl;
	
	myUrl = UrlViewProperty::Get(myView1);
	UrlViewProperty::Set(myView2, myUrl);
	@endcode

	@ingroup	ViewGroup
*/
template <OSType SIG, OSType TAG, typename T>
struct ViewProperty
	: public ToolboxObjectProperty<SIG, TAG, T, ToolboxObjectPropertyViewObjectTraits>
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

}	// namespace B


#endif	// BViewProperty_H_
