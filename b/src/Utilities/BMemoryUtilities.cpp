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
#include "BMemoryUtilities.h"

// B headers
#include "BErrorHandler.h"


namespace B {

// ==========================================================================================
//  AutoMacHandle

// ------------------------------------------------------------------------------------------
AutoMacHandle::AutoMacHandle()
    : mHandle(NewHandle(0))
{
    B_THROW_IF_NULL(mHandle);
}

// ------------------------------------------------------------------------------------------
AutoMacHandle::AutoMacHandle(AutoMacHandle& ioHandle)
    : mHandle(NULL)
{
    std::swap(mHandle, ioHandle.mHandle);
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is a resource handle (and not an ordinary handle), the result 
            is undefined.
*/
AutoMacHandle::AutoMacHandle(MacHandle inHandle)
    : mHandle(NULL)
{
    reset(inHandle);
}

// ------------------------------------------------------------------------------------------
AutoMacHandle::AutoMacHandle(size_t inSize)
    : mHandle(NewHandle(inSize))
{
    B_THROW_IF_NULL(mHandle);
}

// ------------------------------------------------------------------------------------------
AutoMacHandle::~AutoMacHandle()
{
    reset();
}

// ------------------------------------------------------------------------------------------
AutoMacHandle&
AutoMacHandle::operator = (AutoMacHandle& ioHandle)
{
    reset();
    std::swap(mHandle, ioHandle.mHandle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
AutoMacHandle::MacHandle
AutoMacHandle::release()
{
    MacHandle   oldHandle   = mHandle;
    
    mHandle = NULL;
    
    return (oldHandle);
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is a resource handle (and not an ordinary handle), the result 
            is undefined.
*/
void
AutoMacHandle::reset(MacHandle inHandle /* = NULL */)
{
    // If you need to wrap a resource handle, use AutoMacResourceHandle instead.
    B_ASSERT((inHandle == NULL) || !(HGetState(inHandle) & kHandleIsResourceMask));
    
    if (mHandle != NULL)
        DisposeHandle(mHandle);
    
    mHandle = inHandle;
}

// ------------------------------------------------------------------------------------------
void
AutoMacHandle::resize(size_t inNewSize)
{
    SetHandleSize(mHandle, inNewSize);
    B_THROW_IF_STATUS(MemError());
}


// ==========================================================================================
//  AutoMacPtr

#pragma mark -

// ------------------------------------------------------------------------------------------
AutoMacPtr::AutoMacPtr()
    : mPtr(NewPtr(0))
{
    B_THROW_IF_NULL(mPtr);
}

// ------------------------------------------------------------------------------------------
AutoMacPtr::AutoMacPtr(AutoMacPtr& ioPtr)
    : mPtr(NULL)
{
    std::swap(mPtr, ioPtr.mPtr);
}

// ------------------------------------------------------------------------------------------
AutoMacPtr::AutoMacPtr(MacPtr inPtr)
    : mPtr(inPtr)
{
}

// ------------------------------------------------------------------------------------------
AutoMacPtr::AutoMacPtr(size_t inSize)
    : mPtr(NewPtr(inSize))
{
    B_THROW_IF_NULL(mPtr);
}

// ------------------------------------------------------------------------------------------
AutoMacPtr::~AutoMacPtr()
{
    reset();
}

// ------------------------------------------------------------------------------------------
AutoMacPtr&
AutoMacPtr::operator = (AutoMacPtr& ioPtr)
{
    reset();
    std::swap(mPtr, ioPtr.mPtr);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
AutoMacPtr::MacPtr
AutoMacPtr::release()
{
    MacPtr  oldPtr  = mPtr;
    
    mPtr = NULL;
    
    return (oldPtr);
}

// ------------------------------------------------------------------------------------------
void
AutoMacPtr::reset(MacPtr inPtr /* = NULL */)
{
    if (mPtr != NULL)
        DisposePtr(mPtr);
    
    mPtr = inPtr;
}

// ------------------------------------------------------------------------------------------
void
AutoMacPtr::resize(size_t inNewSize)
{
    SetPtrSize(mPtr, inNewSize);
    B_THROW_IF_STATUS(MemError());
}


// ==========================================================================================
//  AutoMacResourceHandle

#pragma mark -

// ------------------------------------------------------------------------------------------
AutoMacResourceHandle::AutoMacResourceHandle(OSType inType, SInt16 inID, bool in1Deep /* = true */)
{
    if (in1Deep)
        mHandle = Get1Resource(inType, inID);
    else
        mHandle = GetResource(inType, inID);
    
    B_THROW_IF_NULL_RSRC(mHandle);
}

// ------------------------------------------------------------------------------------------
AutoMacResourceHandle::AutoMacResourceHandle(AutoMacResourceHandle& ioHandle)
    : mHandle(NULL)
{
    std::swap(mHandle, ioHandle.mHandle);
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is an ordinary handle (and not a resource handle), the result 
            is undefined.
*/
AutoMacResourceHandle::AutoMacResourceHandle(MacHandle inHandle)
    : mHandle(NULL)
{
    reset(inHandle);
}

// ------------------------------------------------------------------------------------------
AutoMacResourceHandle::~AutoMacResourceHandle()
{
    reset();
}

// ------------------------------------------------------------------------------------------
AutoMacResourceHandle&
AutoMacResourceHandle::operator = (AutoMacResourceHandle& ioHandle)
{
    reset();
    std::swap(mHandle, ioHandle.mHandle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
AutoMacResourceHandle::MacHandle
AutoMacResourceHandle::release()
{
    MacHandle   oldHandle   = mHandle;
    
    mHandle = NULL;
    
    return (oldHandle);
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is an ordinary handle (and not a resource handle), the result 
            is undefined.
*/
void
AutoMacResourceHandle::reset(MacHandle inHandle /* = NULL */)
{
    // If you need to wrap an ordinary handle, use AutoMacHandle instead.
    B_ASSERT((inHandle == NULL) || (HGetState(inHandle) & kHandleIsResourceMask));
    
    if (mHandle != NULL)
        ReleaseResource(mHandle);
    
    mHandle = inHandle;
}

// ------------------------------------------------------------------------------------------
void
AutoMacResourceHandle::resize(size_t inNewSize)
{
    SetHandleSize(mHandle, inNewSize);
    B_THROW_IF_STATUS(MemError());
}

// ------------------------------------------------------------------------------------------
void
AutoMacResourceHandle::load()
{
    if (mHandle != NULL)
    {
        LoadResource(mHandle);
        B_THROW_IF_STATUS(ResError());
    }
}


// ==========================================================================================
//  AutoHandleLock

#pragma mark -

// ------------------------------------------------------------------------------------------
AutoHandleLock::AutoHandleLock(::Handle inHandle)
    : mHandle(inHandle)
{
    B_ASSERT(mHandle != NULL);
    
    mState = HGetState(mHandle);
}

// ------------------------------------------------------------------------------------------
AutoHandleLock::AutoHandleLock(const AutoMacHandle& inHandle)
    : mHandle(inHandle.get_handle())
{
    B_ASSERT(mHandle != NULL);
    
    mState = HGetState(mHandle);
}

// ------------------------------------------------------------------------------------------
AutoHandleLock::AutoHandleLock(const AutoMacResourceHandle& inHandle)
    : mHandle(inHandle.get_handle())
{
    B_ASSERT(mHandle != NULL);
    
    mState = HGetState(mHandle);
}

// ------------------------------------------------------------------------------------------
AutoHandleLock::~AutoHandleLock()
{
    HSetState(mHandle, mState);
}

}   // namespace B
