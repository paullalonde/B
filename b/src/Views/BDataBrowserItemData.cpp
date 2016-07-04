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
#include "BDataBrowserItemData.h"

// B headers
#include "BString.h"


namespace B {

// ------------------------------------------------------------------------------------------
DataBrowserItemData::DataBrowserItemData(
    DataBrowserItemDataRef  inItemData)
        : mItemData(inItemData)
{
}

// ------------------------------------------------------------------------------------------
String
DataBrowserItemData::GetText()
{
    CFStringRef cfstr;
    OSStatus    err;
    
    err = GetDataBrowserItemDataText(mItemData, &cfstr);
    B_THROW_IF_STATUS(err);
    
    return (String(cfstr));
}

// ------------------------------------------------------------------------------------------
SInt32
DataBrowserItemData::GetValue()
{
    SInt32      result;
    OSStatus    err;
    
    err = GetDataBrowserItemDataValue(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
SInt32
DataBrowserItemData::GetMinimum()
{
    SInt32      result;
    OSStatus    err;
    
    err = GetDataBrowserItemDataMinimum(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
SInt32
DataBrowserItemData::GetMaximum()
{
    SInt32      result;
    OSStatus    err;
    
    err = GetDataBrowserItemDataMaximum(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
Boolean
DataBrowserItemData::GetBoolean()
{
    Boolean     result;
    OSStatus    err;
    
    err = GetDataBrowserItemDataBooleanValue(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
MenuRef
DataBrowserItemData::GetMenu()
{
    MenuRef     result;
    OSStatus    err;
    
    err = GetDataBrowserItemDataMenuRef(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::GetColor(
    RGBColor&       outColor)
{
    OSStatus    err;
    
    err = GetDataBrowserItemDataRGBColor(mItemData, &outColor);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
ThemeDrawState
DataBrowserItemData::GetDrawState()
{
    ThemeDrawState  result;
    OSStatus        err;
    
    err = GetDataBrowserItemDataDrawState(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
ThemeButtonValue
DataBrowserItemData::GetButtonValue()
{
    ThemeButtonValue    result;
    OSStatus            err;
    
    err = GetDataBrowserItemDataButtonValue(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
IconTransformType
DataBrowserItemData::GetIconTransform()
{
    IconTransformType   result;
    OSStatus            err;
    
    err = GetDataBrowserItemDataIconTransform(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
long
DataBrowserItemData::GetDateTime()
{
    long        result;
    OSStatus    err;
    
    err = GetDataBrowserItemDataDateTime(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::GetLongDateTime(
    LongDateTime&   outLongDateTime)
{
    OSStatus    err;
    
    err = GetDataBrowserItemDataLongDateTime(mItemData, &outLongDateTime);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
DataBrowserPropertyID
DataBrowserItemData::GetProperty()
{
    DataBrowserPropertyID   result;
    OSStatus                err;
    
    err = GetDataBrowserItemDataProperty(mItemData, &result);
    B_THROW_IF_STATUS(err);
    
    return (result);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetIcon(
    IconRef             inIconRef)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataIcon(mItemData, inIconRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetText(
    CFStringRef     inText)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataText(mItemData, inText);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetText(
    const String&       inText)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataText(mItemData, inText.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetValue(
    SInt32              inValue)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataValue(mItemData, inValue);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetMinimum(
    SInt32              inMinimum)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataMinimum(mItemData, inMinimum);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetMaximum(
    SInt32              inMaximum)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataMaximum(mItemData, inMaximum);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetBoolean(
    Boolean             inBoolean)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataBooleanValue(mItemData, inBoolean);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetMenu(
    MenuRef             inMenuRef)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataMenuRef(mItemData, inMenuRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetColor(
    const RGBColor&     inColor)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataRGBColor(mItemData, &inColor);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetDrawState(
    ThemeDrawState      inState)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataDrawState(mItemData, inState);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetButtonValue(
    ThemeButtonValue    inValue)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataButtonValue(mItemData, inValue);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetIconTransform(
    IconTransformType   inTransform)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataIconTransform(mItemData, inTransform);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetDateTime(
    long                inDateTime)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataDateTime(mItemData, inDateTime);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetLongDateTime(
    const LongDateTime& inLongDateTime)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataLongDateTime(mItemData, &inLongDateTime);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItemData::SetItemID(
    DataBrowserItemID   inItemID)
{
    OSStatus    err;
    
    err = SetDataBrowserItemDataItemID(mItemData, inItemID);
    B_THROW_IF_STATUS(err);
}

}   // namespace B
