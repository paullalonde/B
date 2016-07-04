// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

#ifndef BWindowProperty_H_
#define BWindowProperty_H_

#if defined(__MWERKS__)
#   pragma once
#endif

// B headers
#include "BToolboxObjectProperty.h"


namespace B {


// ==========================================================================================
//  ToolboxObjectPropertyWindowObjectTraits

#pragma mark ToolboxObjectPropertyWindowObjectTraits

/*!
    @brief  Toolbox object traits class for @c WindowRef.
    
    Supplies the interface to an @c WindowRef's properties as required by 
    ToolboxObjectProperty.
*/
struct ToolboxObjectPropertyWindowObjectTraits
{
    //! @name Types
    //@{
    typedef WindowRef   ObjectType; //!< The type representing the toolbox object in function arguments.
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns @c true if the property identified by @a creator and @a tag exists within @a obj.
    static bool     Exists(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag);
    //! Returns the size of the property identified by @a creator and @a tag within @a obj.
    static size_t   Size(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag);
    //@}
    
    //! @name Getting / Setting / Removing Properties
    //@{
    //! Retrieves the property identified by @a creator and @a tag within @a obj.
    static OSStatus Get(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag, 
                        size_t      size, 
                        void*       data);
    //! Sets the property identified by @a creator and @a tag within @a obj.
    static void     Set(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag, 
                        size_t      size, 
                        const void* data);
    //! Removes the property identified by @a creator and @a tag within @a obj.
    static void     Remove(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag);
    //@}
    
    //! @name Attributes
    //@{
    //! Returns the attributes of the property identified by @a creator and @a tag within @a obj.
    static UInt32   GetAttrs(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag);
    //! Changes the attributes of the property identified by @a creator and @a tag within @a obj.
    static void     SetAttrs(
                        ObjectType  obj, 
                        OSType      creator, 
                        OSType      tag, 
                        UInt32      setAttributes, 
                        UInt32      clearAttributes);
    //@}
};


// ==========================================================================================
//  WindowProperty

#pragma mark -
#pragma mark WindowProperty

/*!
    @brief  Gets & sets a window property in a type-safe manner.
    
    WindowProperty allows one to access a specific property (identified by the 
    @a SIG and @a TAG template parameters) of an @c WindowRef in a type-safe 
    manner (this safety is provided indirectly by the @a T template parameter).
    
    The idea is to allow for easy definition of properties in a type-safe manner.  For 
    example, if a client of a @c WindowRef needs to associate a URL with a window, it can 
    simply declare a typedef for it:
    
    @code
    typedef B::WindowProperty<kMySig, kMyURLTag, CFURLRef>  UrlWindowProperty;
    @endcode
    
    The client can then get & set the property:
    
    @code
    WindowRef  myWindow1, myWindow2;
    B::Url     myUrl;
    
    myUrl = UrlWindowProperty::Get(myWindow1);
    UrlWindowProperty::Set(myWindow2, myUrl);
    @endcode
*/
template <OSType SIG, OSType TAG, typename T>
struct WindowProperty
    : public ToolboxObjectProperty<SIG, TAG, T, ToolboxObjectPropertyWindowObjectTraits>
{
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef OSType      SIG;    //!< Template parameter.  The creator to use when accessing the property.
    typedef OSType      TAG;    //!< Template parameter.  The tag to use when accessing the property.
    typedef typename    T;      //!< Template parameter.  Any C/C++ language type.
    //@}
#endif
};

}   // namespace B


#endif  // BWindowProperty_H_
