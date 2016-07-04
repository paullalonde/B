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
#include "BViewData.h"

// B headers
#include "BRect.h"
#include "BUrl.h"


namespace B {


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//  ViewDataTrait<Boolean>

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<Boolean>::Get(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    bool&           value,
    const std::nothrow_t&)
{
    Boolean     booleanValue;
    ::Size      size;
    OSStatus    err;
    
    err = GetControlData(control, part, tag, 
                         sizeof(booleanValue), &booleanValue, &size);
    if ((err == noErr) && (size != sizeof(booleanValue)))
        err = errDataSizeMismatch;
    
    if (err == noErr)
        value = booleanValue;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<Boolean>::Set(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    bool            value,
    const std::nothrow_t&)
{
    Boolean booleanValue    = value;
    
    return (SetControlData(control, part, tag, 
                           sizeof(booleanValue), &booleanValue));
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<Boolean>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    bool&           value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<Boolean>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    bool            value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<Boolean>::Get(
    const void*     data, 
    size_t          size,
    bool&           value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(Boolean))
        value = * reinterpret_cast<const Boolean*>(data);
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<Boolean>::Set(
    void*           data, 
    size_t          size,
    bool            value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(Boolean))
        * reinterpret_cast<Boolean*>(data) = value;
    else
        err = errDataSizeMismatch;
    
    return (err);
}


// ==========================================================================================
//  ViewDataTrait<CFStringRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFStringRef>::Get(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    String&         value,
    const std::nothrow_t&)
{
    CFStringRef ref;
    ::Size      size;
    OSStatus    err;
    
    err = GetControlData(control, part, tag, sizeof(ref), &ref, &size);
    if ((err == noErr) && (size != sizeof(ref)))
        err = errDataSizeMismatch;
    
    if (err == noErr)
    {
        // Do not retain the CFStringRef.  We assume that the control retained it prior 
        // to returning it.  This matches the behaviour of the static text and edit unicode 
        // text controls.  Let's just hope that other controls do likewise...
        
        value.assign(ref, from_copy);
    }
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFStringRef>::Set(
    HIViewRef           control, 
    ControlPartCode     part, 
    ResType             tag, 
    const String&       value,
    const std::nothrow_t&)
{
    // Do not fiddle with the CFStringRef's ref count.  We assume that the control will 
    // retain it.
    
    CFStringRef ref = value.cf_ref();
    
    return (SetControlData(control, part, tag, sizeof(ref), &ref));
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<CFStringRef>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    String&         value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<CFStringRef>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    const String&   value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFStringRef>::Get(
    const void*     data, 
    size_t          size,
    String&         value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(CFStringRef))
    {
        CFStringRef ref = * reinterpret_cast<const CFStringRef*>(data);
        
        // Bump the retain count of the CFStringRef.
        
        if (ref != NULL)
            value = ref;
    }
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFStringRef>::Set(
    void*               data, 
    size_t              size,
    const String&       value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(CFStringRef))
    {
        // Retain the CFStringRef prior to returning it.  This matches the behaviour of 
        // existing toolbox controls such as the static text and edit unicode text 
        // controls.
        
        CFStringRef ref = value.cf_ref();
        
        if (ref != NULL)
            CFRetain(ref);
        
        * reinterpret_cast<CFStringRef*>(data) = ref;
    }
    else
        err = errDataSizeMismatch;
    
    return (err);
}


// ==========================================================================================
//  ViewDataTrait<CFURLRef>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFURLRef>::Get(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    Url&            value,
    const std::nothrow_t&)
{
    CFURLRef    ref;
    ::Size      size;
    OSStatus    err;
    
    err = GetControlData(control, part, tag, sizeof(ref), &ref, &size);
    if ((err == noErr) && (size != sizeof(ref)))
        err = errDataSizeMismatch;
    
    if (err == noErr)
    {
        // We use a NULL CFURLRef as a synonym of an empty Url.
        
        if (ref != NULL)
        {
            // Do not retain the CFURLRef.  We assume that the control retained it prior 
            // to returning it.
            
            value = OSPtr<CFURLRef>(ref, from_copy);
        }
        else
        {
            value.Clear();
        }
    }
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFURLRef>::Set(
    HIViewRef           control, 
    ControlPartCode     part, 
    ResType             tag, 
    const Url&          value,
    const std::nothrow_t&)
{
    // Do not fiddle with the CFURLRef's ref count.  We assume that the control will 
    // retain it.  Also, an empty Url is converted into a NULL CFURLRef.
    
    CFURLRef    ref = value.Empty() ? NULL : value.cf_ref();
    
    return (SetControlData(control, part, tag, sizeof(ref), &ref));
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<CFURLRef>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    Url&            value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<CFURLRef>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    const Url&      value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFURLRef>::Get(
    const void*     data, 
    size_t          size,
    Url&            value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(CFURLRef))
    {
        CFURLRef    ref = * reinterpret_cast<const CFURLRef*>(data);
        
        // We use a NULL CFURLRef as a synonym of an empty Url.
        
        if (ref != NULL)
        {
            // Bump the retain count of the CFURLRef.
            value = OSPtr<CFURLRef>(ref);
        }
        else
        {
            value.Clear();
        }
    }
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<CFURLRef>::Set(
    void*               data, 
    size_t              size,
    const Url&          value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(CFURLRef))
    {
        // Retain the CFURLRef prior to returning it.  Also, an empty Url is converted 
        // into a NULL CFURLRef.
        
        CFURLRef    ref = value.Empty() ? NULL : value.cf_ref();
        
        if (ref != NULL)
            CFRetain(ref);
        
        * reinterpret_cast<CFURLRef*>(data) = ref;
    }
    else
        err = errDataSizeMismatch;
    
    return (err);
}


// ==========================================================================================
//  ViewDataTrait<ControlTabInfoRecV1>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<ControlTabInfoRecV1>::Get(
    HIViewRef               control, 
    ControlPartCode         part, 
    ResType                 tag, 
    ControlTabInfoRecV1&    value,
    const std::nothrow_t&)
{
    ::Size      size;
    OSStatus    err;
    
    // First, try version 1 tab info record.
    
    BlockZero(&value, sizeof(value));
    value.version = kControlTabInfoVersionOne;
    
    err = GetControlData(control, part, tag, sizeof(value), &value, &size);
    if ((err == noErr) && (size != sizeof(value)))
        err = errDataSizeMismatch;
    
    if (err != noErr)
    {
        // Couldn't get version 1 record;  try version 0.
        
        ControlTabInfoRec   info0;
        
        BlockZero(&info0, sizeof(info0));
        info0.version = kControlTabInfoVersionZero;
        
        err = GetControlData(control, part, tag, sizeof(info0), &info0, &size);
        if ((err == noErr) && (size != sizeof(value)))
            err = errDataSizeMismatch;
        
        if (err == noErr)
        {
            String  name(info0.name, GetApplicationTextEncoding());
            
            value.version       = kControlTabInfoVersionOne;
            value.iconSuiteID   = info0.iconSuiteID;
            value.name          = name.cf_ref();
            
            CFRetain(value.name);
        }
    }
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<ControlTabInfoRecV1>::Set(
    HIViewRef                   control, 
    ControlPartCode             part, 
    ResType                     tag, 
    const ControlTabInfoRecV1&  value,
    const std::nothrow_t&)
{
    OSStatus    err;
    
    // First, try version 1 tab info record.
    
    err = SetControlData(control, part, tag, sizeof(value), &value);
    
    if (err != noErr)
    {
        // Couldn't set version 1 record;  try version 0.
        
        ControlTabInfoRec   info0;
        
        BlockZero(&info0, sizeof(info0));
        
        info0.version       = kControlTabInfoVersionZero;
        info0.iconSuiteID   = value.iconSuiteID;
        
        if (CFStringGetPascalString(value.name, info0.name, sizeof(info0.name), 
                                    GetApplicationTextEncoding()))
        {
            err = SetControlData(control, part, tag, sizeof(info0), &info0);
        }
        else
            err = coreFoundationUnknownErr;
    }
    
    return (err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<ControlTabInfoRecV1>::Get(
    HIViewRef               view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode         part,   //!< The part code (often kHIViewEntireView).
    ResType                 tag,    //!< The tag.
    ControlTabInfoRecV1&    value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<ControlTabInfoRecV1>::Set(
    HIViewRef                   view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode             part,   //!< The part code (often kHIViewEntireView).
    ResType                     tag,    //!< The tag.
    const ControlTabInfoRecV1&  value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<ControlTabInfoRecV1>::Get(
    const void*             data, 
    size_t                  size,
    ControlTabInfoRecV1&    value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(value))
        BlockMoveData(data, &value, size);
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<ControlTabInfoRecV1>::Set(
    void*                       data, 
    size_t                      size,
    const ControlTabInfoRecV1&  value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(value))
        BlockMoveData(&value, data, size);
    else
        err = errDataSizeMismatch;
    
    return (err);
}


// ==========================================================================================
//  ViewDataTrait<QDRect>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDRect>::Get(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    Rect&           value,
    const std::nothrow_t&)
{
    QDRect      qdRect;
    ::Size      size;
    OSStatus    err;
    
    err = GetControlData(control, part, tag, 
                         sizeof(qdRect), &qdRect, &size);
    if ((err == noErr) && (size != sizeof(qdRect)))
        err = errDataSizeMismatch;
    
    if (err == noErr)
        value = qdRect;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDRect>::Set(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    const Rect&     value,
    const std::nothrow_t&)
{
    ::Rect  qdRect  = value;
    
    return (SetControlData(control, part, tag, 
                           sizeof(QDRect), &qdRect));
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<QDRect>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    Rect&           value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<QDRect>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    const Rect&     value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDRect>::Get(
    const void*     data, 
    size_t          size,
    Rect&           value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(QDRect))
        value = * reinterpret_cast<const QDRect*>(data);
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDRect>::Set(
    void*           data, 
    size_t          size,
    const Rect&     value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(QDRect))
        * reinterpret_cast<QDRect*>(data) = value;
    else
        err = errDataSizeMismatch;
    
    return (err);
}


// ==========================================================================================
//  ViewDataTrait<QDPoint>

#pragma mark -

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDPoint>::Get(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    Point&          value,
    const std::nothrow_t&)
{
    QDPoint     qdPt;
    ::Size      size;
    OSStatus    err;
    
    err = GetControlData(control, part, tag, 
                         sizeof(qdPt), &qdPt, &size);
    if ((err == noErr) && (size != sizeof(qdPt)))
        err = errDataSizeMismatch;
    
    if (err == noErr)
        value = qdPt;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDPoint>::Set(
    HIViewRef       control, 
    ControlPartCode part, 
    ResType         tag, 
    const Point&    value,
    const std::nothrow_t&)
{
    QDPoint qdPt    = value;
    
    return (SetControlData(control, part, tag, 
                           sizeof(QDPoint), &qdPt));
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<QDPoint>::Get(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    Point&          value)  //!< Holds the output.
{
    OSStatus    err;
    
    err = Get(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ViewDataTrait<QDPoint>::Set(
    HIViewRef       view,   //!< The view (HIViewRef a.k.a. ControlRef).
    ControlPartCode part,   //!< The part code (often kHIViewEntireView).
    ResType         tag,    //!< The tag.
    const Point&        value)  //!< The new value.
{
    OSStatus    err;
    
    err = Set(view, part, tag, value, std::nothrow);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDPoint>::Get(
    const void*     data, 
    size_t          size,
    Point&          value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(QDPoint))
        value = * reinterpret_cast<const QDPoint*>(data);
    else
        err = errDataSizeMismatch;
    
    return (err);
}

// ------------------------------------------------------------------------------------------
OSStatus
ViewDataTrait<QDPoint>::Set(
    void*           data, 
    size_t          size,
    const Point&    value)
{
    OSStatus    err = noErr;
    
    if (size == sizeof(QDPoint))
        * reinterpret_cast<QDPoint*>(data) = value;
    else
        err = errDataSizeMismatch;
    
    return (err);
}

#endif  // DOXYGEN_SKIP


}   // namespace B
