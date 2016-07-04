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

// file header
#include "BViewProperty.h"


namespace B {


// ==========================================================================================
//  ToolboxObjectPropertyViewObjectTraits

// ------------------------------------------------------------------------------------------
/*! @return     Boolean @c true if the property exists;  else @c false.
    @exception  none
*/
bool
ToolboxObjectPropertyViewObjectTraits::Exists(
    ObjectType  obj,        //!< The view.
    OSType      creator,    //!< The property's owner.
    OSType      tag)        //!< The property's name.
{
    UInt32      size;
    OSStatus    err;
    
    err = GetControlPropertySize(obj, creator, tag, &size);
    
    return (err == noErr);
}

// ------------------------------------------------------------------------------------------
/*! @return     size_t  The property's size in bytes.
*/
size_t
ToolboxObjectPropertyViewObjectTraits::Size(
    ObjectType  obj,        //!< The view.
    OSType      creator,    //!< The property's owner.
    OSType      tag)        //!< The property's name.
{
    UInt32      size;
    OSStatus    err;
    
    err = GetControlPropertySize(obj, creator, tag, &size);
    B_THROW_IF_STATUS(err);
    
    return (size);
}

// ------------------------------------------------------------------------------------------
OSStatus
ToolboxObjectPropertyViewObjectTraits::Get(
    ObjectType  obj,        //!< The view.
    OSType      creator,    //!< The property's owner.
    OSType      tag,        //!< The property's name.
    size_t      size,       //!< The property's data size in bytes.
    void*       data)       //!< A pointer to the property's data.
{
    UInt32      actualSize  = size;
    OSStatus    err;
    
    err = GetControlProperty(obj, creator, tag, size, &actualSize, data);
    if ((err == noErr) && (size != actualSize))
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyViewObjectTraits::Set(
    ObjectType  obj,        //!< The view.
    OSType      creator,    //!< The property's owner.
    OSType      tag,        //!< The property's name.
    size_t      size,       //!< The property's data size in bytes.
    const void* data)       //!< A pointer to the property's data.
{
    OSStatus    err;
    
    err = SetControlProperty(obj, creator, tag, size, data);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyViewObjectTraits::Remove(
    ObjectType  obj,        //!< The view.
    OSType      creator,    //!< The property's owner.
    OSType      tag)        //!< The property's name.
{
    OSStatus    err;
    
    err = RemoveControlProperty(obj, creator, tag);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*! @return     UInt32  The property's attributes.
*/
UInt32
ToolboxObjectPropertyViewObjectTraits::GetAttrs(
    ObjectType  obj,        //!< The view.
    OSType      creator,    //!< The property's owner.
    OSType      tag)        //!< The property's name.
{
    UInt32      attributes;
    OSStatus    err;
    
    err = GetControlPropertyAttributes(obj, creator, tag, &attributes);
    B_THROW_IF_STATUS(err);
    
    return (attributes);
}

// ------------------------------------------------------------------------------------------
void
ToolboxObjectPropertyViewObjectTraits::SetAttrs(
    ObjectType  obj,                //!< The view.
    OSType      creator,            //!< The property's owner.
    OSType      tag,                //!< The property's name.
    UInt32      setAttributes,      //!< The attributes to set.
    UInt32      clearAttributes)    //!< The attributes to clear.
{
    OSStatus    err;
    
    err = ChangeControlPropertyAttributes(obj, creator, tag, 
                                          setAttributes | clearAttributes, 
                                          setAttributes);
    B_THROW_IF_STATUS(err);
}

}   // namespace B
