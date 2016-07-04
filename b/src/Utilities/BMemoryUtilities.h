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

#ifndef BMemoryUtilities_H_
#define BMemoryUtilities_H_

#pragma	once

// standard headers
#include <algorithm>

// system headers
#include <ApplicationServices/ApplicationServices.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BErrorHandler.h"


namespace B {


// ==========================================================================================
//	AutoMacHandle

/*!
    @brief  Wrapper around Carbon @c Handle type.
    
    This class provides "resource acquisition is initialization" (RAII) semantics for 
    Carbon @a Handles.  It features a destructive copy constructor and assignment operator 
    similar in spirit to @c std::auto_ptr.
    
    @note   This class deals with the raw @c Handle type, so it isn't type-safe.
*/
class AutoMacHandle
{
public:
	
    //! Synonym for classic Mac OS Handle.
    typedef ::Handle    MacHandle;
    
    //! @name Constructors & Destructor
    //@{
    //! Default constructor.  Creates a zero-sized handle.
				AutoMacHandle();
    //! AutoMacHandle constructor.  Acquires ownership of @a ioHandle's handle.
				AutoMacHandle(AutoMacHandle& ioHandle);
    //! @c Handle constructor.  Acquires ownership of @a inHandle.
	explicit	AutoMacHandle(MacHandle inHandle);
    //! @c Size constructor.  Creates a handle of size @a inSize.
	explicit	AutoMacHandle(size_t inSize);
    //! Destructor.  Releases the handle, if any.
				~AutoMacHandle();
	//@}
    
    //! Assignment.  Acquires ownership of @a ioHandle's handle.
	AutoMacHandle&	operator = (AutoMacHandle& ioHandle);
	
    //! @name Inquiries
    //@{
	//! Returns the amount of memory allocated to the underlying handle.
	size_t		size() const;
    //! Returns the underlying handle.
	MacHandle	get_handle() const		{ return mHandle; }
    //@}
    
    //! @name Modifiers
    //@{
    //! Relenquishes ownership of the underlying handle, and returns it.
	MacHandle	release();
    //! Relenquishes ownership of the underlying handle, and acquires ownership of @a inHandle.
	void		reset(MacHandle inHandle = NULL);
	//! Changes the amount of memory allocated to the underlying handle.
	void		resize(size_t inNewSize);
    //@}
    
private:
	
	MacHandle	mHandle;
};

// ------------------------------------------------------------------------------------------
inline size_t
AutoMacHandle::size() const
{
	return GetHandleSize(mHandle);
}


// ==========================================================================================
//	AutoTypedHandle

/*!
    @brief  Type-safe wrapper around Carbon @c Handle type.
    
    This class provides "resource acquisition is initialization" (RAII) semantics for 
    Carbon @a Handles.  It features a destructive copy constructor and assignment operator 
    similar in spirit to @c std::auto_ptr.
    
    It's assumed that the handles given to objects of this type match the template 
    parameter @a T.  @a T can be any POD (aka Plain Old Data) type.
*/
template <typename T>
class AutoTypedHandle
{
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef anything	T;	//!< The language type.
	//@}
#endif
    
    //! Synonym for classic Mac OS Handle.
    typedef ::Handle    MacHandle;
    
    //! @name Constructors & Destructor
    //@{
    //! Default constructor.  Creates a handle the size of a @a T.
				AutoTypedHandle();
    //! AutoTypedHandle constructor.  Acquires ownership of @a ioHandle's resource handle.
				AutoTypedHandle(AutoTypedHandle& ioHandle);
    //! @a T constructor.  Acquires ownership of @a inHandle.
	explicit	AutoTypedHandle(T** inHandle);
    //! Destructor.  Releases the handle, if any.
				~AutoTypedHandle();
	//@}
    
    //! Assignment.  Acquires ownership of @a ioHandle's handle.
	AutoTypedHandle&	operator = (AutoTypedHandle& ioHandle);
	
    //! @name Inquiries
    //@{
	//! Returns the amount of memory allocated to the underlying handle.
	size_t		size() const;
    //! Returns properly typed underlying handle.
	T**			get() const			{ return mHandle; }
    //! Returns the underlying handle as a @c ::Handle.
	MacHandle	get_handle() const	{ return reinterpret_cast<MacHandle>(mHandle); }
    //@}
    
    //! @name Modifiers
    //@{
    //! Relenquishes ownership of the underlying handle, and returns it.
	T**			release();
    //! Relenquishes ownership of the underlying handle, and acquires ownership of @a inHandle.
	void		reset(T** inHandle = NULL);
	//! Changes the amount of memory allocated to the underlying handle.
	void		resize(size_t inNewSize);
    //@}
	
private:
	
	T**			mHandle;
};

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedHandle<T>::AutoTypedHandle()
	: mHandle(reinterpret_cast<T**>(NewHandle(sizeof(T))))
{
	B_THROW_IF_NULL(mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedHandle<T>::AutoTypedHandle(AutoTypedHandle& ioHandle)
	: mHandle(NULL)
{
	std::swap(mHandle, ioHandle.mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedHandle<T>::AutoTypedHandle(T** inHandle)
	: mHandle(NULL)
{
    reset(inHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedHandle<T>::~AutoTypedHandle()
{
	reset();
}

// ------------------------------------------------------------------------------------------
template <typename T> inline size_t
AutoTypedHandle<T>::size() const
{
	return GetHandleSize(reinterpret_cast<MacHandle>(mHandle));
}

// ------------------------------------------------------------------------------------------
template <typename T> AutoTypedHandle<T>&
AutoTypedHandle<T>::operator = (AutoTypedHandle& ioHandle)
{
	reset();
	std::swap(mHandle, ioHandle.mHandle);
	
	return *this;
}

// ------------------------------------------------------------------------------------------
template <typename T> T**
AutoTypedHandle<T>::release()
{
	T**	oldHandle	= mHandle;
	
	mHandle = NULL;
	
	return oldHandle;
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is a resource handle (and not an ordinary handle), the result 
            is undefined.
*/
template <typename T> void
AutoTypedHandle<T>::reset(T** inHandle /* = NULL */)
{
    MacHandle   oldH    = reinterpret_cast<MacHandle>(mHandle);
    MacHandle   newH    = reinterpret_cast<MacHandle>(inHandle);
    
    // If you need to wrap a resource handle, use AutoTypedResourceHandle instead.
    B_ASSERT((newH == NULL) || !(HGetState(newH) & kHandleIsResourceMask));
	
	if (oldH != NULL)
		DisposeHandle(oldH);
	
	mHandle = inHandle;
}

// ------------------------------------------------------------------------------------------
template <typename T> void
AutoTypedHandle<T>::resize(size_t inNewSize)
{
	SetHandleSize(reinterpret_cast<MacHandle>(mHandle), inNewSize);
	B_THROW_IF_STATUS(MemError());
}


// ==========================================================================================
//	AutoMacPtr

/*!
    @brief  Wrapper around Carbon @c Ptr type.
    
    This class provides "resource acquisition is initialization" (RAII) semantics for 
    Carbon @a Ptrs.  It features a destructive copy constructor and assignment operator 
    similar in spirit to @c std::auto_ptr.
    
    @note   This class deals with the raw @c Ptr type, so it isn't type-safe.
*/
class AutoMacPtr
{
public:
    
    //! Synonym for classic Mac OS pointer.
    typedef ::Ptr   MacPtr;
    
    //! @name Constructors & Destructor
    //@{
    //! Default constructor.  Creates a zero-sized ptr.
				AutoMacPtr();
    //! AutoMacPtr constructor.  Acquires ownership of @a ioPtr's ptr.
				AutoMacPtr(AutoMacPtr& ioPtr);
    //! @c Ptr constructor.  Acquires ownership of @a inPtr.
	explicit	AutoMacPtr(MacPtr inPtr);
    //! @c Size constructor.  Creates a ptr of size @a inSize.
	explicit	AutoMacPtr(size_t inSize);
    //! Destructor.  Releases the ptr, if any.
				~AutoMacPtr();
	//@}
    
    //! Assignment.  Acquires ownership of @a ioPtr's ptr.
	AutoMacPtr&	operator = (AutoMacPtr& ioPtr);
	
    //! @name Inquiries
    //@{
	//! Returns the amount of memory allocated to the underlying ptr.
	size_t		size() const;
    //! Returns the underlying ptr.
	MacPtr		get_ptr() const			{ return mPtr; }
    //@}
    
    //! @name Modifiers
    //@{
    //! Relenquishes ownership of the underlying ptr, and returns it.
	MacPtr		release();
    //! Relenquishes ownership of the underlying ptr, and acquires ownership of @a inPtr.
	void		reset(MacPtr inPtr = NULL);
	//! Changes the amount of memory allocated to the underlying ptr.
	void		resize(size_t inNewSize);
    //@}
    
private:
	
	MacPtr	mPtr;
};

// ------------------------------------------------------------------------------------------
inline size_t
AutoMacPtr::size() const
{
	return GetPtrSize(mPtr);
}


// ==========================================================================================
//	AutoTypedPtr

/*!
    @brief  Type-safe wrapper around Carbon @c Ptr type.
    
    This class provides "resource acquisition is initialization" (RAII) semantics for 
    Carbon @a Ptrs.  It features a destructive copy constructor and assignment operator 
    similar in spirit to @c std::auto_ptr.
    
    It's assumed that the ptrs given to objects of this type match the template 
    parameter @a T.  @a T can be any POD (aka Plain Old Data) type.
*/
template <typename T>
class AutoTypedPtr
{
public:
	
    //! Synonym for classic Mac OS pointer.
    typedef ::Ptr   MacPtr;
    
    //! @name Constructors & Destructor
    //@{
    //! Default constructor.  Creates a ptr the size of a @a T.
				AutoTypedPtr();
    //! AutoTypedPtr constructor.  Acquires ownership of @a ioPtr's ptr.
				AutoTypedPtr(AutoTypedPtr& ioPtr);
    //! @a T constructor.  Acquires ownership of @a inPtr.
	explicit	AutoTypedPtr(T* inPtr);
    //! Destructor.  Releases the ptr, if any.
				~AutoTypedPtr();
	//@}
    
    //! Assignment.  Acquires ownership of @a ioPtr's ptr.
	AutoTypedPtr&	operator = (AutoTypedPtr& ioPtr);
	
    //! @name Inquiries
    //@{
	//! Returns the amount of memory allocated to the underlying ptr.
	size_t		size() const;
    //! Returns properly typed underlying ptr.
	T*			get() const			{ return mPtr; }
    //! Returns the underlying ptr as a @c Ptr.
	MacPtr		get_ptr() const		{ return reinterpret_cast<MacPtr>(mPtr); }
    //@}
    
    //! @name Modifiers
    //@{
    //! Relenquishes ownership of the underlying ptr, and returns it.
	T*			release();
    //! Relenquishes ownership of the underlying ptr, and acquires ownership of @a inPtr.
	void		reset(T* inPtr = NULL);
	//! Changes the amount of memory allocated to the underlying ptr.
	void		resize(size_t inNewSize);
	//@}
    
private:
	
	T*			mPtr;
};

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedPtr<T>::AutoTypedPtr()
	: mPtr(reinterpret_cast<T*>(NewPtr(sizeof(T))))
{
	B_THROW_IF_NULL(mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedPtr<T>::AutoTypedPtr(AutoTypedPtr& ioPtr)
	: mPtr(NULL)
{
	std::swap(mPtr, ioPtr.mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedPtr<T>::AutoTypedPtr(T* inPtr)
	: mPtr(inPtr)
{
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedPtr<T>::~AutoTypedPtr()
{
	reset();
}

// ------------------------------------------------------------------------------------------
template <typename T> AutoTypedPtr<T>&
AutoTypedPtr<T>::operator = (AutoTypedPtr& ioPtr)
{
	reset();
	std::swap(mPtr, ioPtr.mPtr);
	
	return *this;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline size_t
AutoTypedPtr<T>::size() const
{
	return GetPtrSize(reinterpret_cast<MacPtr>(mPtr));
}

// ------------------------------------------------------------------------------------------
template <typename T> void
AutoTypedPtr<T>::reset(T* inPtr /* = NULL */)
{
	if (mPtr != NULL)
		DisposePtr(reinterpret_cast<MacPtr>(mPtr));
	
	mPtr = inPtr;
}

// ------------------------------------------------------------------------------------------
template <typename T> T*
AutoTypedPtr<T>::release()
{
	T*	oldHandle	= mPtr;
	
	mPtr = NULL;
	
	return oldHandle;
}

// ------------------------------------------------------------------------------------------
template <typename T> void
AutoTypedPtr<T>::resize(size_t inNewSize)
{
	SetPtrSize(reinterpret_cast<MacPtr>(mPtr), inNewSize);
	B_THROW_IF_STATUS(MemError());
}


// ==========================================================================================
//	AutoMacResourceHandle

/*!
    @brief  Wrapper around Carbon resource handles.
    
    This class provides "resource acquisition is initialization" (RAII) semantics for 
    Carbon resource handles.  It features a destructive copy constructor and assignment 
    operator similar in spirit to @c std::auto_ptr.
    
    @note   This class deals with the raw @c Handle type, so it isn't type-safe.
*/
class AutoMacResourceHandle
{
public:
	
    //! Synonym for classic Mac OS Handle.
    typedef ::Handle    MacHandle;
	
    //! @name Constructors & Destructor
    //@{
    //! Constructor.  Reads the resource of type @a inType and ID @a inID.
				AutoMacResourceHandle(OSType inType, SInt16 inID, bool in1Deep = true);
    //! AutoMacResourceHandle constructor.  Acquires ownership of @a ioHandle's handle.
				AutoMacResourceHandle(AutoMacResourceHandle& ioHandle);
    //! @c Handle constructor.  Acquires ownership of @a inHandle.
	explicit	AutoMacResourceHandle(MacHandle inHandle);
    //! Destructor.  Releases the resource handle, if any.
				~AutoMacResourceHandle();
	//@}
	
    //! Assignment.  Acquires ownership of @a ioHandle's handle.
	AutoMacResourceHandle&	operator = (AutoMacResourceHandle& ioHandle);
	
    //! @name Inquiries
    //@{
	//! Returns the amount of memory allocated to the underlying resource handle.
	size_t		size() const;
    //! Returns the underlying resource handle.
	MacHandle	get_handle() const	{ return mHandle; }
    //@}
    
    //! @name Modifiers
    //@{
    //! Relenquishes ownership of the underlying resource handle, and returns it.
	MacHandle	release();
    //! Relenquishes ownership of the underlying resource handle, and acquires ownership of @a inHandle.
	void		reset(MacHandle inHandle = NULL);
	//! Changes the amount of memory allocated to the underlying resource handle.
	void		resize(size_t inNewSize);
    //! Loads the resource handle into memory so it's immediately accessible.  It may still need to be locked and/or protected against purging, though.
	void		load();
	//@}
    
private:
	
	MacHandle	mHandle;
};

// ------------------------------------------------------------------------------------------
inline size_t
AutoMacResourceHandle::size() const
{
	return GetHandleSize(mHandle);
}


// ==========================================================================================
//	AutoTypedResourceHandle

/*!
    @brief  Type-safe wrapper around Carbon resource handles.
    
    This class provides "resource acquisition is initialization" (RAII) semantics for 
    Carbon resource handles.  It features a destructive copy constructor and assignment 
    operator similar in spirit to @c std::auto_ptr.
    
    It's assumed that the resource handles given to objects of this type match the template 
    parameter @a T.  @a T can be any POD (aka Plain Old Data) type.
*/
template <typename T>
class AutoTypedResourceHandle
{
public:
    
    //! Synonym for classic Mac OS Handle.
    typedef ::Handle    MacHandle;
    
    //! @name Constructors & Destructor
    //@{
    //! Constructor.  Reads the resource of type @a inType and ID @a inID.
				AutoTypedResourceHandle(OSType inType, SInt16 inID, bool in1Deep = true);
    //! AutoTypedResourceHandle constructor.  Acquires ownership of @a ioHandle's resource handle.
				AutoTypedResourceHandle(AutoTypedResourceHandle& ioHandle);
    //! @a T constructor.  Acquires ownership of @a inHandle.
	explicit	AutoTypedResourceHandle(T** inHandle);
    //! Destructor.  Releases the resource handle, if any.
				~AutoTypedResourceHandle();
	//@}
    
    //! Assignment.  Acquires ownership of @a ioHandle's handle.
	AutoTypedResourceHandle&	operator = (AutoTypedResourceHandle& ioHandle);
	
    //! @name Inquiries
    //@{
	//! Returns the amount of memory allocated to the underlying resource handle.
	size_t		size() const;
    //! Returns properly typed underlying resource handle.
	T**			get() const			{ return mHandle; }
    //! Returns the underlying resource handle as a @c ::Handle.
	MacHandle	get_handle() const	{ return reinterpret_cast<MacHandle>(mHandle); }
    //@}
    
    //! @name Modifiers
    //@{
    //! Relenquishes ownership of the underlying resource handle, and returns it.
	T**			release();
    //! Relenquishes ownership of the underlying resource handle, and acquires ownership of @a inHandle.
	void		reset(T** inHandle = NULL);
	//! Changes the amount of memory allocated to the underlying resource handle.
	void		resize(size_t inNewSize);
    //! Loads the resource handle into memory so it's immediately accessible.  It may still need to be locked and/or protected against purging, though.
	void		load();
	//@}
    
private:
	
	T**			mHandle;
};

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedResourceHandle<T>::AutoTypedResourceHandle(OSType inType, SInt16 inID, bool in1Deep /* = true */)
{
	if (in1Deep)
		mHandle = reinterpret_cast<T**>(Get1Resource(inType, inID));
	else
		mHandle = reinterpret_cast<T**>(GetResource(inType, inID));
	
	B_THROW_IF_NULL_RSRC(mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedResourceHandle<T>::AutoTypedResourceHandle(AutoTypedResourceHandle& ioHandle)
	: mHandle(NULL)
{
	std::swap(mHandle, ioHandle.mHandle);
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is an ordinary handle (and not a resource handle), the result 
            is undefined.
*/
template <typename T>
AutoTypedResourceHandle<T>::AutoTypedResourceHandle(T** inHandle)
	: mHandle(inHandle)
{
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoTypedResourceHandle<T>::~AutoTypedResourceHandle()
{
	reset();
}

// ------------------------------------------------------------------------------------------
template <typename T> AutoTypedResourceHandle<T>&
AutoTypedResourceHandle<T>::operator = (AutoTypedResourceHandle& ioHandle)
{
	reset();
	std::swap(mHandle, ioHandle.mHandle);
	
	return *this;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline size_t
AutoTypedResourceHandle<T>::size() const
{
	return GetHandleSize(reinterpret_cast<MacHandle>(mHandle));
}

// ------------------------------------------------------------------------------------------
template <typename T> T**
AutoTypedResourceHandle<T>::release()
{
	T**	oldHandle	= mHandle;
	
	mHandle = NULL;
	
	return oldHandle;
}

// ------------------------------------------------------------------------------------------
template <typename T> void
AutoTypedResourceHandle<T>::load()
{
    if (mHandle != NULL)
    {
        LoadResource(reinterpret_cast<MacHandle>(mHandle));
        B_THROW_IF_STATUS(ResError());
    }
}

// ------------------------------------------------------------------------------------------
/*! @note   If @a inHandle is an ordinary handle (and not a resource handle), the result 
            is undefined.
*/
template <typename T> void
AutoTypedResourceHandle<T>::reset(T** inHandle /* = NULL */)
{
    MacHandle   oldH    = reinterpret_cast<MacHandle>(mHandle);
    MacHandle   newH    = reinterpret_cast<MacHandle>(inHandle);
    
    // If you need to wrap an ordinary handle, use AutoTypedHandle instead.
    B_ASSERT((newH == NULL) || (HGetState(newH) & kHandleIsResourceMask));
	
	if (oldH != NULL)
		ReleaseResource(oldH);
	
	mHandle = inHandle;
}

// ------------------------------------------------------------------------------------------
template <typename T> void
AutoTypedResourceHandle<T>::resize(size_t inNewSize)
{
	SetHandleSize(reinterpret_cast<MacHandle>(mHandle), inNewSize);
	B_THROW_IF_STATUS(MemError());
}


// ==========================================================================================
//	AutoHandleLock

/*!
    @brief  Helper class for locking & unlocking Carbon @c Handles.
    
    The constructor locks saves the given handle's lock state and locks the handle, and 
    the destructor restores the lock state.  Can be used with both ordinary and resource 
    handles.
*/
class AutoHandleLock : public boost::noncopyable
{
public:
	
    //! Synonym for classic Mac OS Handle.
    typedef ::Handle    MacHandle;
    
    //! Constructor.  Saves @a inHandle's lock state, then locks it.
	explicit	AutoHandleLock(MacHandle inHandle);
    //! Constructor.  Saves @a inHandle's lock state, then locks it.
	template <typename T>
	explicit	AutoHandleLock(T** inHandle);
    //! Constructor.  Saves @a inHandle's lock state, then locks it.
	explicit	AutoHandleLock(const AutoMacHandle& inHandle);
    //! Constructor.  Saves @a inHandle's lock state, then locks it.
	template <typename T>
	explicit	AutoHandleLock(const AutoTypedHandle<T>& inHandle);
    //! Constructor.  Saves @a inHandle's lock state, then locks it.
	explicit	AutoHandleLock(const AutoMacResourceHandle& inHandle);
    //! Constructor.  Saves @a inHandle's lock state, then locks it.
	template <typename T>
	explicit	AutoHandleLock(const AutoTypedResourceHandle<T>& inHandle);
    //! Destructor.  Restores @a inHandle's lock state.
				~AutoHandleLock();

private:
	
	MacHandle	mHandle;
	SInt8		mState;
};

// ------------------------------------------------------------------------------------------
template <typename T>
AutoHandleLock::AutoHandleLock(T** inHandle)
	: mHandle(reinterpret_cast<MacHandle>(inHandle))
{
	B_ASSERT(mHandle != NULL);
	
	mState = HGetState(mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoHandleLock::AutoHandleLock(const AutoTypedHandle<T>& inHandle)
	: mHandle(inHandle.get_handle())
{
	B_ASSERT(mHandle != NULL);
	
	mState = HGetState(mHandle);
}

// ------------------------------------------------------------------------------------------
template <typename T>
AutoHandleLock::AutoHandleLock(const AutoTypedResourceHandle<T>& inHandle)
	: mHandle(inHandle.get_handle())
{
	B_ASSERT(mHandle != NULL);
	
	mState = HGetState(mHandle);
}


}	// namespace B

#endif	// BMemoryUtilities_H_
