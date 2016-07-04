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

#ifndef BQuickTimeUtilities_H_
#define BQuickTimeUtilities_H_

#pragma once

// system headers
#include <CoreServices/CoreServices.h>

// library headers
//#include <boost/utility.hpp>

// B headers
#include "BErrorHandler.h"


namespace B {


// ==========================================================================================
//  AutoComponentInstance

class AutoComponentInstance
{
public:
    
                AutoComponentInstance();
                AutoComponentInstance(AutoComponentInstance& ioInstance);
    explicit    AutoComponentInstance(ComponentInstance inInstance);
                ~AutoComponentInstance();
    
    AutoComponentInstance&  operator = (AutoComponentInstance& ioInstance);
    
                        operator ComponentInstance () const { return (mInstance); }
    ComponentInstance   get() const                         { return (mInstance); }
    ComponentInstance   release();
    void                reset(ComponentInstance inInstance = NULL);

private:
    
    ComponentInstance   mInstance;
};

// ------------------------------------------------------------------------------------------
inline
AutoComponentInstance::AutoComponentInstance()
    : mInstance(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoComponentInstance::AutoComponentInstance(AutoComponentInstance& ioInstance)
    : mInstance(NULL)
{
    std::swap(mInstance, ioInstance.mInstance);
}

// ------------------------------------------------------------------------------------------
inline
AutoComponentInstance::AutoComponentInstance(ComponentInstance inInstance)
    : mInstance(inInstance)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoComponentInstance::~AutoComponentInstance()
{
    reset();
}

// ------------------------------------------------------------------------------------------
inline AutoComponentInstance&
AutoComponentInstance::operator = (AutoComponentInstance& ioInstance)
{
    reset();
    std::swap(mInstance, ioInstance.mInstance);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
AutoComponentInstance::reset(ComponentInstance inInstance /* = NULL */)
{
    if (mInstance != NULL)
        CloseComponent(mInstance);
    
    mInstance = inInstance;
}

// ------------------------------------------------------------------------------------------
inline ComponentInstance
AutoComponentInstance::release()
{
    ComponentInstance   oldHandle   = mInstance;
    
    mInstance = NULL;
    
    return (oldHandle);
}


#if 0

// ==========================================================================================
//  AutoMacHandle

class AutoMacHandle
{
public:
    
                AutoMacHandle();
                AutoMacHandle(AutoMacHandle& ioHandle);
    explicit    AutoMacHandle(::Handle inHandle);
    explicit    AutoMacHandle(size_t inSize);
                ~AutoMacHandle();
    
    AutoMacHandle&  operator = (AutoMacHandle& ioHandle);
    
                operator ::Handle () const  { return (mHandle); }
    ::Handle    get() const                 { return (mHandle); }
    ::Handle    release();
    void        reset(::Handle inHandle = NULL);

private:
    
    ::Handle    mHandle;
};

// ------------------------------------------------------------------------------------------
inline
AutoMacHandle::AutoMacHandle()
    : mHandle(NewHandle(0))
{
    B_THROW_IF_NULL(mHandle);
}

// ------------------------------------------------------------------------------------------
inline
AutoMacHandle::AutoMacHandle(AutoMacHandle& ioHandle)
    : mHandle(NULL)
{
    std::swap(mHandle, ioHandle.mHandle);
}

// ------------------------------------------------------------------------------------------
inline
AutoMacHandle::AutoMacHandle(::Handle inHandle)
    : mHandle(inHandle)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoMacHandle::AutoMacHandle(size_t inSize)
    : mHandle(NewHandle(inSize))
{
    B_THROW_IF_NULL(mHandle);
}

// ------------------------------------------------------------------------------------------
inline
AutoMacHandle::~AutoMacHandle()
{
    reset();
}

// ------------------------------------------------------------------------------------------
inline AutoMacHandle&
AutoMacHandle::operator = (AutoMacHandle& ioHandle)
{
    reset();
    std::swap(mHandle, ioHandle.mHandle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
AutoMacHandle::reset(::Handle inHandle /* = NULL */)
{
    if (mHandle != NULL)
        DisposeHandle(mHandle);
    
    mHandle = inHandle;
}

// ------------------------------------------------------------------------------------------
inline ::Handle
AutoMacHandle::release()
{
    ::Handle    oldHandle   = mHandle;
    
    mHandle = NULL;
    
    return (oldHandle);
}


// ==========================================================================================
//  AutoTypedHandle

template <typename T>
class AutoTypedHandle
{
public:
    
                AutoTypedHandle();
                AutoTypedHandle(AutoTypedHandle& ioHandle);
    explicit    AutoTypedHandle(T** inHandle);
                ~AutoTypedHandle();
    
    AutoTypedHandle&    operator = (AutoTypedHandle& ioHandle);
    
                operator T** () const       { return (mHandle); }
                operator Handle () const    { return (reinterpret_cast<Handle>(mHandle)); }
    T**         get() const                 { return (mHandle); }
    T**         release();
    void        reset(T** inHandle = NULL);
    
private:
    
    T**         mHandle;
};

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedHandle<T>::AutoTypedHandle()
    : mHandle(reinterpret_cast<T**>(NewHandle(sizeof(T))))
{
    B_THROW_IF_NULL(mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedHandle<T>::AutoTypedHandle(AutoTypedHandle& ioHandle)
    : mHandle(NULL)
{
    std::swap(mHandle, ioHandle.mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedHandle<T>::AutoTypedHandle(T** inHandle)
    : mHandle(inHandle)
{
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedHandle<T>::~AutoTypedHandle()
{
    reset();
}

// ------------------------------------------------------------------------------------------
template <typename T> inline AutoTypedHandle<T>&
AutoTypedHandle<T>::operator = (AutoTypedHandle& ioHandle)
{
    reset();
    std::swap(mHandle, ioHandle.mHandle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
AutoTypedHandle<T>::reset(T** inHandle /* = NULL */)
{
    if (mHandle != NULL)
        DisposeHandle(mHandle);
    
    mHandle = inHandle;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline T**
AutoTypedHandle<T>::release()
{
    T** oldHandle   = mHandle;
    
    mHandle = NULL;
    
    return (oldHandle);
}


// ==========================================================================================
//  AutoMacPtr

class AutoMacPtr
{
public:
    
                AutoMacPtr();
                AutoMacPtr(AutoMacPtr& ioPtr);
    explicit    AutoMacPtr(::Ptr inPtr);
    explicit    AutoMacPtr(size_t inSize);
                ~AutoMacPtr();
    
    AutoMacPtr& operator = (AutoMacPtr& ioPtr);
    
                operator ::Ptr () const { return (mPtr); }
    ::Ptr       get() const             { return (mPtr); }
    ::Ptr       release();
    void        reset(::Ptr inPtr = NULL);

private:
    
    ::Ptr   mPtr;
};

// ------------------------------------------------------------------------------------------
inline
AutoMacPtr::AutoMacPtr()
    : mPtr(NewPtr(0))
{
    B_THROW_IF_NULL(mPtr);
}

// ------------------------------------------------------------------------------------------
inline
AutoMacPtr::AutoMacPtr(AutoMacPtr& ioPtr)
    : mPtr(NULL)
{
    std::swap(mPtr, ioPtr.mPtr);
}

// ------------------------------------------------------------------------------------------
inline
AutoMacPtr::AutoMacPtr(::Ptr inPtr)
    : mPtr(inPtr)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoMacPtr::AutoMacPtr(size_t inSize)
    : mPtr(NewPtr(inSize))
{
    B_THROW_IF_NULL(mPtr);
}

// ------------------------------------------------------------------------------------------
inline
AutoMacPtr::~AutoMacPtr()
{
    reset();
}

// ------------------------------------------------------------------------------------------
inline AutoMacPtr&
AutoMacPtr::operator = (AutoMacPtr& ioPtr)
{
    reset();
    std::swap(mPtr, ioPtr.mPtr);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
AutoMacPtr::reset(::Ptr inPtr /* = NULL */)
{
    if (mPtr != NULL)
        DisposePtr(mPtr);
    
    mPtr = inPtr;
}

// ------------------------------------------------------------------------------------------
inline ::Ptr
AutoMacPtr::release()
{
    ::Ptr   oldPtr  = mPtr;
    
    mPtr = NULL;
    
    return (oldPtr);
}


// ==========================================================================================
//  AutoTypedPtr

template <typename T>
class AutoTypedPtr
{
public:
    
                AutoTypedPtr();
                AutoTypedPtr(AutoTypedPtr& ioPtr);
    explicit    AutoTypedPtr(T* inPtr);
                ~AutoTypedPtr();
    
    AutoTypedPtr&   operator = (AutoTypedPtr& ioPtr);
    
                operator T* () const    { return (mPtr); }
                operator Ptr () const   { return (reinterpret_cast<Ptr>(mPtr)); }
    T*          get() const             { return (mPtr); }
    T*          release();
    void        reset(T* inPtr = NULL);
    
private:
    
    T*          mPtr;
};

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedPtr<T>::AutoTypedPtr()
    : mPtr(reinterpret_cast<T*>(NewHandle(sizeof(T))))
{
    B_THROW_IF_NULL(mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedPtr<T>::AutoTypedPtr(AutoTypedPtr& ioPtr)
    : mPtr(NULL)
{
    std::swap(mPtr, ioPtr.mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedPtr<T>::AutoTypedPtr(T* inPtr)
    : mPtr(inPtr)
{
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
AutoTypedPtr<T>::~AutoTypedPtr()
{
    reset();
}

// ------------------------------------------------------------------------------------------
template <typename T> inline AutoTypedPtr<T>&
AutoTypedPtr<T>::operator = (AutoTypedPtr& ioPtr)
{
    reset();
    std::swap(mPtr, ioPtr.mPtr);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
AutoTypedPtr<T>::reset(T* inPtr /* = NULL */)
{
    if (mPtr != NULL)
        DisposeHandle(mPtr);
    
    mPtr = inPtr;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline T*
AutoTypedPtr<T>::release()
{
    T*  oldHandle   = mPtr;
    
    mPtr = NULL;
    
    return (oldHandle);
}


// ==========================================================================================
//  AglException

#pragma mark AglException

/*!
    @brief  <tt>AGL</tt> exception class
    
    This class contains an AGL-defined error code.  Typically an exception of 
    this class is thrown after an AGL-defined function returns a non-zero 
    result code.
    
    In all instances of this class, @c what() returns "B::AglException".
*/
class AglException : public Exception
{
public:
    
    static void Throw(const char* file, int line, const char* func);
    static void Throw();
    
    //! @name Constructors / Destructor
    //@{
    //! OSStatus Constructor.
    explicit    AglException(GLenum inError);
    //! Destructor
    virtual     ~AglException();
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns the AGL-defined error code.
    GLenum              GetError() const;
    //! Returns the exception's textual description.
    virtual const char* what() const;
    //@}
    
    //! @name Serialisation
    //@{
    //! Reads in the exception's internal state from @a istr.
    explicit        AglException(std::istream& istr);
    //! Writes out the exception's internal state to @a ostr.
    virtual void    Write(std::ostream& ostr) const;
    //@}
    
private:
    
    // member variables
    GLenum  mError;
};


// ==========================================================================================
//  Exception-throwing macros

#ifndef NDEBUG

#   define ThrowIfAgl(b)    \
        do { if (!(b)) B::AglException::Throw(__FILE__, __LINE__, __func__); } while (0)

#else

#   define ThrowIfAgl(b)    \
        do { if (!(b)) B::AglException::Throw(); } while (0)

#endif

#endif


}   // namespace B

#endif  // BQuickTimeUtilities_H_
