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

#ifndef BMenuItemProperty_H_
#define BMenuItemProperty_H_

#if defined(__MWERKS__)
#   pragma once
#endif

// B headers
#include "BToolboxObjectProperty.h"


namespace B {


// ==========================================================================================
//  ToolboxObjectPropertyMenuItemObjectTraits

#pragma mark ToolboxObjectPropertyMenuItemObjectTraits

/*!
    @brief  Toolbox object traits class for <tt>MenuRef/MenuItemIndex</tt> pairs.
    
    Supplies the interface to a @c MenuRef's properties as required by 
    ToolboxObjectProperty.
*/
struct ToolboxObjectPropertyMenuItemObjectTraits
{
    /*!
        @brief  The type representing the toolbox object in function arguments.
        
        Because menu commands need both a @c MenuRef and a @c MenuItemIndex to 
        identify them, this will helper class serves to pair them up so we can 
        conveniently pass it around to the various ToolboxObjectProperty member 
        functions.
    */
    struct ObjectType
    {
        //! Constructor.
        ObjectType(MenuRef menu, MenuItemIndex index)
            : mMenu(menu), mIndex(index) {}
        
        //! Returns the @c MenuRef.
        operator MenuRef () const       { return (mMenu); }
        //! Returns the @c MenuItemIndex.
        operator MenuItemIndex () const { return (mIndex); }
        
        MenuRef         mMenu;  //!< The menu.
        MenuItemIndex   mIndex; //!< The menu item.
    };
    
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
//  MenuItemProperty

#pragma mark -
#pragma mark MenuItemProperty

/*!
    @brief  Gets & sets a menu item property in a type-safe manner.
    
    MenuItemProperty allows one to access a specific property (identified by the 
    @a SIG and @a TAG template parameters) of a <tt>MenuRef/MenuItemIndex</tt> pair 
    in a type-safe manner (this safety is provided indirectly by the @a T template 
    parameter).
    
    The idea is to allow for easy definition of properties in a type-safe manner.  For 
    example, if a client of a @c MenuRef needs to associate a URL with a menu item, it can 
    simply declare a typedef for it:
    
    @code
    typedef B::MenuItemProperty<kMySig, kMyURLTag, CFURLRef>    UrlMenuItemProperty;
    @endcode
    
    The client can then get & set the property:
    
    @code
    MenuRef  myMenu;
    B::Url   myUrl;
    
    myUrl = UrlMenuItemProperty::Get(myMenu, 1);
    UrlMenuItemProperty::Set(myMenu, 2, myUrl);
    @endcode
*/
template <OSType SIG, OSType TAG, typename T>
struct MenuItemProperty
    : private ToolboxObjectProperty<
                SIG, TAG, T, 
                ToolboxObjectPropertyMenuItemObjectTraits>
{
private:
    
    typedef ToolboxObjectProperty<
                SIG, TAG, T, 
                ToolboxObjectPropertyMenuItemObjectTraits>  inherited;
    
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef OSType      SIG;    //!< Template parameter.  The creator to use when accessing the property.
    typedef OSType      TAG;    //!< Template parameter.  The tag to use when accessing the property.
    typedef typename    T;      //!< Template parameter.  Any C/C++ language type.
    //@}
#endif

    //! @name Types
    //@{
    typedef typename inherited::ObjectTraits    ObjectTraits;   //!< The traits for the toolbox object type.
    typedef typename inherited::ObjectType      ObjectType;     //!< The type representing the toolbox object in function arguments.
    typedef typename inherited::TypeTraits      TypeTraits;     //!< The traits for C/C++ language type of the property.
    typedef typename inherited::PropertyType    PropertyType;   //!< The type representing the property data in function arguments.  See ToolboxObjectPropertyTypeTrait.
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns @c true if the property identified by @a SIG and @a TAG exists within @a obj.
    static bool     Exists(
                        MenuRef         menu, 
                        MenuItemIndex   index);
    //! Returns the size of the property identified by @a SIG and @a TAG within @a obj.
    static size_t   Size(
                        MenuRef         menu, 
                        MenuItemIndex   index);
    //@}
    
    //! @name Getting / Setting / Removing Properties
    //@{
    //! Retrieves the property identified by @a SIG and @a TAG within @a obj.
    static PropertyType Get(
                        MenuRef         menu, 
                        MenuItemIndex   index);
    /*! @overload */
    static void     Get(
                        MenuRef         menu, 
                        MenuItemIndex   index, 
                        PropertyType&   value);
    /*! @overload */
    static bool     Get(
                        MenuRef         menu, 
                        MenuItemIndex   index, 
                        PropertyType&   value, 
                        const std::nothrow_t&);
    //! Sets the property identified by @a SIG and @a TAG within @a obj.
    static void     Set(
                        MenuRef         menu, 
                        MenuItemIndex   index, 
                        const PropertyType& value);
    //! Removes the property identified by @a SIG and @a TAG within @a obj.
    static void     Remove(
                        MenuRef         menu, 
                        MenuItemIndex   index);
    //@}
    
    //! @name Attributes
    //@{
    //! Returns the attributes of the property identified by @a SIG and @a TAG within @a obj.
    static UInt32   GetAttrs(
                        MenuRef         menu, 
                        MenuItemIndex   index);
    //! Changes the attributes of the property identified by @a SIG and @a TAG within @a obj.
    static void     SetAttrs(
                        MenuRef         menu, 
                        MenuItemIndex   index, 
                        UInt32          setAttributes, 
                        UInt32          clearAttributes);
    //@}
};

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline bool
MenuItemProperty<SIG, TAG, T>::Exists(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index)          //!< The menu item.
{
    return (inherited::Exists(ObjectType(menu, index)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline size_t
MenuItemProperty<SIG, TAG, T>::Size(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index)          //!< The menu item.
{
    return (inherited::Size(ObjectType(menu, index)));
}

// ------------------------------------------------------------------------------------------
/*! @return     PropertyType The property's value.
*/
template <OSType SIG, OSType TAG, typename T> inline
typename MenuItemProperty<SIG, TAG, T>::PropertyType
MenuItemProperty<SIG, TAG, T>::Get(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index)          //!< The menu item.
{
    return (inherited::Get(ObjectType(menu, index)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuItemProperty<SIG, TAG, T>::Get(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index,          //!< The menu item.
    PropertyType&   value)          //!< Holds the output property value.
{
    value = inherited::Get(ObjectType(menu, index));
}

// ------------------------------------------------------------------------------------------
/*! @return     Boolean indication of sucess or failure.
    @exception  none
*/
template <OSType SIG, OSType TAG, typename T> inline bool
MenuItemProperty<SIG, TAG, T>::Get(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index,          //!< The menu item.
    PropertyType&   value,          //!< Holds the output property value.
    const std::nothrow_t& nt)       //!< An indication that the caller doesn't want to have the function throw an exception.
{
    return (inherited::Get(ObjectType(menu, index), value, nt));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuItemProperty<SIG, TAG, T>::Set(
    MenuRef             menu,       //!< The menu.
    MenuItemIndex       index,      //!< The menu item.
    const PropertyType& value)      //!< The new value.
{
    inherited::Set(ObjectType(menu, index), value);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuItemProperty<SIG, TAG, T>::Remove(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index)          //!< The menu item.
{
    inherited::Remove(ObjectType(menu, index));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline UInt32
MenuItemProperty<SIG, TAG, T>::GetAttrs(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index)          //!< The menu item.
{
    return (inherited::GetAttrs(ObjectType(menu, index)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuItemProperty<SIG, TAG, T>::SetAttrs(
    MenuRef         menu,           //!< The menu.
    MenuItemIndex   index,          //!< The menu item.
    UInt32          setAttributes,  //!< The attributes to set.
    UInt32          clearAttributes)//!< The attributes to clear.
{
    inherited::SetAttrs(ObjectType(menu, index), setAttributes, clearAttributes);
}

}   // namespace B


#endif  // BMenuItemProperty_H_
