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

#ifndef BMenuCommandProperty_H_
#define BMenuCommandProperty_H_

#if defined(__MWERKS__)
#   pragma once
#endif

// B headers
#include "BToolboxObjectProperty.h"


namespace B {


// ==========================================================================================
//  ToolboxObjectPropertyMenuCommandObjectTraits

#pragma mark ToolboxObjectPropertyMenuCommandObjectTraits

/*!
    @brief  Toolbox object traits class for <tt>MenuRef/MenuCommand</tt> pairs.
    
    Supplies the interface to a @c MenuRef's properties as required by 
    ToolboxObjectProperty.
*/
struct ToolboxObjectPropertyMenuCommandObjectTraits
{
    /*!
        @brief  The type representing the toolbox object in function arguments.
        
        Because menu commands need both a @c MenuCommand and an (optional) 
        @c MenuRef to identify them, this will helper class serves to pair them 
        up so we can conveniently pass it around to the various 
        ToolboxObjectProperty member functions.
    */
    struct ObjectType
    {
        //! Constructor.
        ObjectType(MenuRef menu, MenuCommand cmd)
            : mMenu(menu), mCmd(cmd) {}
        
        //! Constructor.
        ObjectType(MenuCommand cmd)
            : mMenu(NULL), mCmd(cmd) {}
        
        //! Returns the @c MenuRef.
        operator MenuRef () const       { return (mMenu); }
        //! Returns the @c MenuCommand.
        operator MenuCommand () const   { return (mCmd); }
        
        MenuRef     mMenu;  //!< The menu.  May be @c NULL.
        MenuCommand mCmd;   //!< The menu command.
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
//  MenuCommandProperty

#pragma mark -
#pragma mark MenuCommandProperty

/*!
    @brief  Gets & sets a menu command property in a type-safe manner.
    
    MenuCommandProperty allows one to access a specific property (identified by the 
    @a SIG and @a TAG template parameters) of a <tt>MenuRef/MenuCommand</tt> pair 
    in a type-safe manner (this safety is provided indirectly by the @a T template 
    parameter).
    
    The idea is to allow for easy definition of properties in a type-safe manner.  For 
    example, if a client of a @c MenuRef needs to associate a URL with a menu command, 
    it can simply declare a typedef for it:
    
    @code
    typedef B::MenuCommandProperty<kMySig, kMyURLTag, CFURLRef> UrlMenuCommandProperty;
    @endcode
    
    The client can then get & set the property:
    
    @code
    B::Url   myUrl;
    
    myUrl = UrlMenuCommandProperty::Get(kMyCmd);
    UrlMenuCommandProperty::Set(kMyCmd, myUrl);
    @endcode
*/
template <OSType SIG, OSType TAG, typename T>
struct MenuCommandProperty
    : private ToolboxObjectProperty<
                SIG, TAG, T, 
                ToolboxObjectPropertyMenuCommandObjectTraits>
{
private:
    
    //! Shorthand for the base class.
    typedef ToolboxObjectProperty<
                SIG, TAG, T, 
                ToolboxObjectPropertyMenuCommandObjectTraits>   inherited;
    
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
                        MenuCommand     cmd);
    /*! @overload */
    static bool     Exists(
                        MenuCommand     cmd);
    //! Returns the size of the property identified by @a SIG and @a TAG within @a obj.
    static size_t   Size(
                        MenuRef         menu, 
                        MenuCommand     cmd);
    /*! @overload */
    static size_t   Size(
                        MenuCommand     cmd);
    //@}
    
    //! @name Getting / Setting / Removing Properties
    //@{
    //! Retrieves the property identified by @a SIG and @a TAG within @a obj.
    static PropertyType Get(
                        MenuRef         menu, 
                        MenuCommand     cmd);
    /*! @overload */
    static PropertyType Get(
                        MenuCommand     cmd);
    /*! @overload */
    static void     Get(
                        MenuRef         menu, 
                        MenuCommand     cmd, 
                        PropertyType&   value);
    /*! @overload */
    static void     Get(
                        MenuCommand     cmd, 
                        PropertyType&   value);
    /*! @overload */
    static bool     Get(
                        MenuRef         menu, 
                        MenuCommand     cmd, 
                        PropertyType&   value, 
                        const std::nothrow_t&);
    /*! @overload */
    static bool     Get(
                        MenuCommand     cmd, 
                        PropertyType&   value, 
                        const std::nothrow_t&);
    //! Sets the property identified by @a SIG and @a TAG within @a obj.
    static void     Set(
                        MenuRef         menu, 
                        MenuCommand     cmd, 
                        const PropertyType& value);
    /*! @overload */
    static void     Set(
                        MenuCommand     cmd, 
                        const PropertyType& value);
    //! Removes the property identified by @a SIG and @a TAG within @a obj.
    static void     Remove(
                        MenuRef         menu, 
                        MenuCommand     cmd);
    /*! @overload */
    static void     Remove(
                        MenuCommand     cmd);
    //@}
    
    //! @name Attributes
    //@{
    //! Returns the attributes of the property identified by @a SIG and @a TAG within @a obj.
    static UInt32   GetAttrs(
                        MenuRef         menu, 
                        MenuCommand     cmd);
    /*! @overload */
    static UInt32   GetAttrs(
                        MenuCommand     cmd);
    //! Changes the attributes of the property identified by @a SIG and @a TAG within @a obj.
    static void     SetAttrs(
                        MenuRef         menu, 
                        MenuCommand     cmd, 
                        UInt32          setAttributes, 
                        UInt32          clearAttributes);
    /*! @overload */
    static void     SetAttrs(
                        MenuCommand     cmd, 
                        UInt32          setAttributes, 
                        UInt32          clearAttributes);
    //@}
};

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline bool
MenuCommandProperty<SIG, TAG, T>::Exists(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::Exists(ObjectType(menu, cmd)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline bool
MenuCommandProperty<SIG, TAG, T>::Exists(
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::Exists(ObjectType(cmd)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline size_t
MenuCommandProperty<SIG, TAG, T>::Size(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::Size(ObjectType(menu, cmd)));
}

// ------------------------------------------------------------------------------------------
/*! @return     PropertyType The property's value.
*/
template <OSType SIG, OSType TAG, typename T> inline size_t
MenuCommandProperty<SIG, TAG, T>::Size(
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::Size(ObjectType(cmd)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline
typename MenuCommandProperty<SIG, TAG, T>::PropertyType
MenuCommandProperty<SIG, TAG, T>::Get(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::Get(ObjectType(menu, cmd)));
}

// ------------------------------------------------------------------------------------------
/*! @return     PropertyType The property's value.
*/
template <OSType SIG, OSType TAG, typename T> inline
typename MenuCommandProperty<SIG, TAG, T>::PropertyType
MenuCommandProperty<SIG, TAG, T>::Get(
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::Get(ObjectType(cmd)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::Get(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd,            //!< The menu command.
    PropertyType&   value)          //!< Holds the output property value.
{
    value = inherited::Get(ObjectType(menu, cmd));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::Get(
    MenuCommand     cmd,            //!< The menu command.
    PropertyType&   value)          //!< Holds the output property value.
{
    value = inherited::Get(ObjectType(cmd));
}

// ------------------------------------------------------------------------------------------
/*! @return     Boolean indication of sucess or failure.
    @exception  none
*/
template <OSType SIG, OSType TAG, typename T> inline bool
MenuCommandProperty<SIG, TAG, T>::Get(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd,            //!< The menu command.
    PropertyType&   value,          //!< Holds the output property value.
    const std::nothrow_t& nt)       //!< An indication that the caller doesn't want to have the function throw an exception.
{
    return (inherited::Get(ObjectType(menu, cmd), value, nt));
}

// ------------------------------------------------------------------------------------------
/*! @return     Boolean indication of sucess or failure.
    @exception  none
*/
template <OSType SIG, OSType TAG, typename T> inline bool
MenuCommandProperty<SIG, TAG, T>::Get(
    MenuCommand     cmd,            //!< The menu command.
    PropertyType&   value,          //!< Holds the output property value.
    const std::nothrow_t& nt)       //!< An indication that the caller doesn't want to have the function throw an exception.
{
    return (inherited::Get(ObjectType(cmd), value, nt));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::Set(
    MenuRef             menu,       //!< The menu.
    MenuCommand         cmd,        //!< The menu command.
    const PropertyType& value)      //!< The new value.
{
    inherited::Set(ObjectType(menu, cmd), value);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::Set(
    MenuCommand         cmd,        //!< The menu command.
    const PropertyType& value)      //!< The new value.
{
    inherited::Set(ObjectType(cmd), value);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::Remove(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd)            //!< The menu command.
{
    inherited::Remove(ObjectType(menu, cmd));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::Remove(
    MenuCommand     cmd)            //!< The menu command.
{
    inherited::Remove(ObjectType(cmd));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline UInt32
MenuCommandProperty<SIG, TAG, T>::GetAttrs(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::GetAttrs(ObjectType(menu, cmd)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline UInt32
MenuCommandProperty<SIG, TAG, T>::GetAttrs(
    MenuCommand     cmd)            //!< The menu command.
{
    return (inherited::GetAttrs(ObjectType(cmd)));
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::SetAttrs(
    MenuRef         menu,           //!< The menu.
    MenuCommand     cmd,            //!< The menu command.
    UInt32          setAttributes,  //!< The attributes to set.
    UInt32          clearAttributes)//!< The attributes to clear.
{
    inherited::SetAttrs(ObjectType(menu, cmd), setAttributes, clearAttributes);
}

// ------------------------------------------------------------------------------------------
template <OSType SIG, OSType TAG, typename T> inline void
MenuCommandProperty<SIG, TAG, T>::SetAttrs(
    MenuCommand     cmd,            //!< The menu command.
    UInt32          setAttributes,  //!< The attributes to set.
    UInt32          clearAttributes)//!< The attributes to clear.
{
    inherited::SetAttrs(ObjectType(cmd), setAttributes, clearAttributes);
}

}   // namespace B


#endif  // BMenuCommandProperty_H_
