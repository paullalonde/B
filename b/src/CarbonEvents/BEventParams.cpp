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
#include "BEventParams.h"

// B headers
#include "BRect.h"
#include "BShape.h"
#include "BString.h"


namespace B {


// ==========================================================================================
//  EventParamTypeTraitBase

// ------------------------------------------------------------------------------------------
/*! @return A boolean indication of success or failure.
*/
bool
EventParamTypeTraitBase::Exists(
    EventRef        inEvent,    //!< The Carbon %Event.
    EventParamName  inName,     //!< The parameter's name.
    EventParamType  inType)     //!< The parameter's type.
{
    UInt32  junk;
    
    return (GetEventParameter(inEvent, inName, inType, NULL, 0, &junk, NULL) == noErr);
}


// ==========================================================================================
//  EventParamTypeTrait<typeBoolean>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeBoolean>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    Boolean     booleanValue;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeBoolean, NULL, 
                            sizeof(booleanValue), NULL, &booleanValue);
    
    if (err == noErr)
        outValue = booleanValue;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeBoolean>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    Boolean booleanValue    = inValue;
    
    return (SetEventParameter(inEvent, inName, typeBoolean, 
                              sizeof(booleanValue), &booleanValue));
}


// ==========================================================================================
//  EventParamTypeTrait<typeQDPoint>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeQDPoint>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    ::Point     qdPt;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeQDPoint, NULL, sizeof(qdPt), NULL, &qdPt);
    
    if (err == noErr)
        outValue = qdPt;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeQDPoint>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    ::Point     qdPt    = inValue;
    
    return (SetEventParameter(inEvent, inName, typeQDPoint, sizeof(qdPt), &qdPt));
}


// ==========================================================================================
//  EventParamTypeTrait<typeQDRectangle>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeQDRectangle>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    ::Rect      qdRect;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeQDRectangle, NULL, 
                            sizeof(qdRect), NULL, &qdRect);
    
    if (err == noErr)
        outValue = qdRect;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeQDRectangle>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    ::Rect      qdRect  = inValue;
    
    return (SetEventParameter(inEvent, inName, typeQDRectangle, sizeof(qdRect), &qdRect));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFArrayRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFArrayRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFArrayRef  ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeCFArrayRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFArrayRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFArrayRef  ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeCFArrayRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFDictionaryRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFDictionaryRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFDictionaryRef ref;
    OSStatus        err;
    
    err = GetEventParameter(inEvent, inName, typeCFDictionaryRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFDictionaryRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFDictionaryRef ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeCFDictionaryRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFMutableArrayRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFMutableArrayRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFMutableArrayRef   ref;
    OSStatus            err;
    
    err = GetEventParameter(inEvent, inName, typeCFMutableArrayRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFMutableArrayRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFMutableArrayRef   ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeCFMutableArrayRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFMutableDictionaryRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFMutableDictionaryRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFMutableDictionaryRef  ref;
    OSStatus                err;
    
    err = GetEventParameter(inEvent, inName, typeCFMutableDictionaryRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFMutableDictionaryRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFMutableDictionaryRef  ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeCFMutableDictionaryRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFMutableStringRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFMutableStringRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFMutableStringRef  ref;
    OSStatus            err;
    
    err = GetEventParameter(inEvent, inName, typeCFMutableStringRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFMutableStringRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFMutableStringRef  ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeCFMutableStringRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFStringRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFStringRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFStringRef ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeCFStringRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue = ref;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFStringRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFStringRef ref = inValue.cf_ref();
    
    return (SetEventParameter(inEvent, inName, typeCFStringRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeCFTypeRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFTypeRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    CFTypeRef   ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeCFTypeRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeCFTypeRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    CFTypeRef   ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeCFTypeRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeHIObjectRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIObjectRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    HIObjectRef ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeHIObjectRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIObjectRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    HIObjectRef ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeHIObjectRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeHIToolbarRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIToolbarRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    HIToolbarRef    ref;
    OSStatus        err;
    
    err = GetEventParameter(inEvent, inName, typeHIToolbarRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIToolbarRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    HIToolbarRef    ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeHIToolbarRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeHIToolbarItemRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIToolbarItemRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    HIToolbarItemRef    ref;
    OSStatus        err;
    
    err = GetEventParameter(inEvent, inName, typeHIToolbarItemRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIToolbarItemRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    HIToolbarItemRef    ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeHIToolbarItemRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeEventRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeEventRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    EventRef    ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeEventRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeEventRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    EventRef    ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeEventRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeHIShapeRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIShapeRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    HIShapeRef  ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeHIShapeRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.assign(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeHIShapeRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    HIShapeRef  ref = inValue.cf_ref();
    
    return (SetEventParameter(inEvent, inName, typeHIShapeRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeMenuRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeMenuRef>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    MenuRef     ref;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeMenuRef, NULL, 
                            sizeof(ref), NULL, &ref);
    
    if (err == noErr)
        outValue.reset(ref);
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeMenuRef>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    MenuRef ref = inValue;
    
    return (SetEventParameter(inEvent, inName, typeMenuRef, 
                              sizeof(ref), &ref));
}


// ==========================================================================================
//  EventParamTypeTrait<typeChar>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeChar>::Get(
    EventRef        inEvent, 
    EventParamName  inName, 
    Type&           outValue)
{
    UInt32      size;
    OSStatus    err;
    
    err = GetEventParameter(inEvent, inName, typeChar, NULL, 0, &size, NULL);
    
    if (err == noErr)
    {
        try
        {
            if (size > 0)
            {
                std::vector<char>   buffer(size);
                
                err = GetEventParameter(inEvent, inName, typeChar, NULL, size, 
                                        NULL, &buffer[0]);
                
                if (err == noErr)
                {
                    // Remove any trailing null characters. Interface Builder appends 
                    // a null to C String parameters to HIViews.
                    
                    while ((size > 0) && (buffer[size-1] == '\0'))
                        --size;
                    
                    outValue.assign(&buffer[0], size);
                }
            }
            else
            {
                outValue.clear();
            }
        }
        catch (std::bad_alloc&)
        {
            err = memFullErr;
        }
        catch (...)
        {
            err = paramErr;
        }
    }
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
EventParamTypeTrait<typeChar>::Set(
    EventRef            inEvent, 
    EventParamName      inName, 
    const Type&         inValue)
{
    return (SetEventParameter(inEvent, inName, typeChar, inValue.size(), inValue.c_str()));
}


}   // namespace B
