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

#ifndef BMutableArray_H_
#define BMutableArray_H_

#pragma once

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BArray.h"


namespace B {

// ==========================================================================================
//	MutableArray

/*!
	@brief	Adapter class for changeable arrays.

	MutableArray provides a wrapper around a CoreFoundation @c CFMutableArrayRef object, 
	giving it an interface similar (nearly identical in fact) to @c std::vector.  This 
	makes it possible to use MutableArray with STL algorithms, with standard streams, etc.
	
	Unlike Array, MutableArray does @b not share @c CFMutableArrayRefs between its
	instances.  Each MutableArray gets its own independent @c CFMutableArrayRefs.  The one 
	exception to this is the @c OSPtr<CFMutableArrayRef> constructor, which can optionally 
	share its argument instead of copying it.
	
	Other notable differences with @c std::vector :
		
		- The template parameter @a T is assumed to be a CoreFoundation type.  Specifically, 
		  it needs to obey the semantics of @c CFRetain() and @c CFRelease(), and there 
		  needs to be a template specialisation of @c CFUGetType<>() for it.
		  
		- There are more conversions.  This reflects MutableArray's role as an 
		  adapter between CoreFoundation and the Standard C++ Library.  For example, one 
		  may construct instances of MutableArray with any of the following types:
			- Array
			- MutableArray
			- <tt>CFArrayRef</tt>
			- <tt>OSPtr<CFArrayRef></tt>
			- <tt>OSPtr<CFMutableArrayRef></tt>
			- <tt>std::vector</tt>
		
		- Likewise, one may copy() a MutableArray back into an @c std::vector.
		  
		- The allocator type is fixed as @c CFAllocatorRef.  The allocator type is not a 
		  template parameter, however allocators @e are passed as arguments to 
		  constructors and can be retrieved via get_allocator().
		  
		- The underlying @c CFMutableArrayRef's storage is not accessible, so it's not 
		  possible to take the address of an element of an MutableArray.  Likewise, at(), 
		  operator [] (), front() and back() return objects which can extract the 
		  element's value.  MutableArray is incompatible with the STL in this respect.
		
		- MutableArray doesn't support the following global comparison operators: 
		  @c < @c <= @c > @c >= .  This is because ordering of CoreFoundation types isn't 
		  well-defined.  The operators @c == and @c != @e are supported and are 
		  implemented in terms of @c CFEqual().
*/
template <typename T>
class MutableArray
{
public:
	
	//! @name Types
	//@{
	typedef typename Array<T>::value_type				value_type;				//!< The type of the characters.
	typedef typename Array<T>::size_type				size_type;				//!< The unsigned integral type for size values and indices.
	typedef typename Array<T>::difference_type			difference_type;		//!< The signed integral type for difference values.
	typedef typename Array<T>::reference				reference;				//!< The signed integral type for difference values.
	typedef typename Array<T>::const_reference			const_reference;		//!< The type of constant character references.
	typedef typename Array<T>::pointer					pointer;				//!< The type of character pointers.
	typedef typename Array<T>::const_pointer			const_pointer;			//!< The type of character pointers.
	typedef typename Array<T>::iterator					iterator;				//!< The type of iterators.
	typedef typename Array<T>::const_iterator			const_iterator;			//!< The type of constant iterators.
	typedef typename Array<T>::reverse_iterator			reverse_iterator;		//!< The type of reverse iterators.
	typedef typename Array<T>::const_reverse_iterator	const_reverse_iterator;	//!< The type of constant reverse iterators.
	typedef typename Array<T>::allocator_type			allocator_type;			//!< The type of the allocator.
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
				MutableArray();
	//! Copy constructor.
				MutableArray(const MutableArray& arr);
	//! Array constructor.
				MutableArray(const Array<T>& arr);
	//! Allocator constructor.
	explicit	MutableArray(CFAllocatorRef allocator);
	//! Size constructor.
	explicit	MutableArray(size_type num, CFAllocatorRef allocator = NULL);
	//! @a T constructor.
	explicit	MutableArray(size_type num, T value, CFAllocatorRef allocator = NULL);
	//! @c CFArrayRef constructor.
	explicit	MutableArray(CFArrayRef cfarr);
	//! @c OSPtr<CFArrayRef> constructor.
	explicit	MutableArray(OSPtr<CFArrayRef> cfarr);
	//! @c OSPtr<CFMutableArrayRef> constructor.
	explicit	MutableArray(OSPtr<CFMutableArrayRef> cfarr, bool acquire /* = false */);
	//! @c vector constructor.
	explicit	MutableArray(const std::vector<value_type>& vec);
	//! Range constructor.
	template <class InputIterator>
	explicit	MutableArray(InputIterator first, InputIterator last, CFAllocatorRef allocator = NULL);
	//! Destructor.
				~MutableArray();
	//@}
	
	//! @name Operations for Size and Capacity
	//@{
	//! Returns the number of elements in the array.
	size_type	size() const;
	//! Returns whether the array is empty.
	bool		empty() const;
	//! Returns the maximum number of characters an array could contain.
	size_type	max_size() const;
	//! Returns the number of elements the array could contain without reallocation.
	size_type	capacity() const;
	//! Non-binding request to resize the internal storage to max(n, size()).
	void		reserve(size_type n = 0);
	//@}
	
	//! @name Assignments
	//@{
	//! Replaces all existing elements with copies of the elements of @a arr.
	MutableArray&	operator = (const MutableArray& arr);
	//! Array assignemnt.
	MutableArray&	operator = (const Array<T>& arr);
	//! @c CFArrayRef assignemnt.
	MutableArray&	operator = (CFArrayRef cfarr);
	//! @c OSPtr<CFArrayRef> assignemnt.
	MutableArray&	operator = (OSPtr<CFArrayRef> cfarr);
	//! @c OSPtr<CFMutableArrayRef> assignemnt.
	MutableArray&	operator = (OSPtr<CFMutableArrayRef> cfarr);
	//! @c vector assignemnt.
	MutableArray&	operator = (const std::vector<value_type>& vec);
	//! Replaces all existing elements with @a num copies of @a value.
	MutableArray&	assign(size_type num, T value);
	//! Replaces all existing elements with copies of the elements in the range [@a first, @a last).
	template <class InputIterator>
	MutableArray&	assign(InputIterator first, InputIterator last);
	//! Exchanges the contents with @a arr.
	void			swap(MutableArray& arr);
	//@}
	
	//! @name Direct Element Access
	//@{
	//! Returns the element with index @a index.
	reference		at(size_type index);
	//! Returns the element with index @a index.
	const_reference	at(size_type index) const;
	//! Returns the element with index @a index.
	reference		operator [] (size_type index);
	//! Returns the element with index @a index.
	const_reference	operator [] (size_type index) const;
	//! Returns the first element.
	reference		front();
	//! Returns the first element.
	const_reference	front() const;
	//! Returns the last element.
	reference		back();
	//! Returns the last element.
	const_reference	back() const;
	//! Converts the array into a @a vector.
	void			copy(std::vector<value_type>& vec) const;
	//@}
	
	//! @name Operations to Generate Iterators
	//@{
	//! Returns an iterator for the beginning of the array.
	iterator				begin();
	//! Returns an iterator for the beginning of the array.
	const_iterator			begin() const;
	//! Returns an iterator for the end of the array.
	iterator				end();
	//! Returns an iterator for the end of the array.
	const_iterator			end() const;
	//! Returns an iterator for the beginning of a reverse iteration of the array.
	reverse_iterator		rbegin();
	//! Returns an iterator for the beginning of a reverse iteration of the array.
	const_reverse_iterator	rbegin() const;
	//! Returns an iterator for the end of a reverse iteration of the array.
	reverse_iterator		rend();
	//! Returns an iterator for the end of a reverse iteration of the array.
	const_reverse_iterator	rend() const;
	//@}
	
	//! @name Inserting and Removing Elements
	//@{
	//! Inserts a copy of @a value at the position of iterator @a pos.
	iterator	insert(iterator pos, T value);
	//! Inserts @a num copies of @a value at the position of iterator @a pos.
	void		insert(iterator pos, size_type num, T value);
	//! Inserts copies of the elements in the range [@a first, @a last) at the position of iterator @a pos.
	template <class InputIterator>
	void		insert(iterator pos, InputIterator first, InputIterator last);
	//! Appends a copy of @a value at the end of the array.
	void		push_back(T value);
	//! Removes the element at the position of iterator @a pos.
	iterator	erase(iterator pos);
	//! Removes the elements in the range [@a first, @a last).
	iterator	erase(iterator first, iterator last);
	//! Removes the last element of the array.
	void		pop_back();
	//! Changes the number of elements to @a num.
	void		resize(size_type num, T value = NULL);
	//! Removes all of the elements.
	void		clear();
	//@}
	
	//! @name Generating CFMutableArrayRefs.
	//@{
	//! Returns the underlying @c CFMutableArrayRef.
	CFMutableArrayRef			cf_ref() const;
	//! Returns a "smart pointer" to the underlying @c CFMutableArrayRef.
	OSPtr<CFMutableArrayRef>	cf_ptr() const;
	//@}
	
	//! @name Allocator Support
	//@{
	//! Returns the memory model of the array.
	allocator_type	get_allocator() const;
	//@}

private:
	
	void	init_array(CFArrayRef cfarr, const T* values, size_t num, CFAllocatorRef allocator);
	void	replace_array(CFArrayRef cfarr);
	
	// member variables
	CFMutableArrayRef	mRef;
};

// ------------------------------------------------------------------------------------------
/*!	Creates an empty array using the default allocator.
*/
template <typename T> inline
MutableArray<T>::MutableArray()
{
	init_array(NULL, NULL, 0, NULL);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a shallow copy of @a arr.
*/
template <typename T> inline
MutableArray<T>::MutableArray(const MutableArray& arr)
{
	init_array(arr.mRef, NULL, 0, CFGetAllocator(arr.mRef));
}

// ------------------------------------------------------------------------------------------
/*!	Creates a shallow copy of @a arr.
*/
template <typename T> inline
MutableArray<T>::MutableArray(const Array<T>& arr)
{
	init_array(arr.cf_ref(), NULL, 0, CFGetAllocator(arr.mRef));
}

// ------------------------------------------------------------------------------------------
/*!	Creates an empty array using @a allocator.
*/
template <typename T> inline
MutableArray<T>::MutableArray(CFAllocatorRef allocator)
{
	init_array(NULL, NULL, 0, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array containing @a num elements of type @a T whose value is @c NULL.
*/
template <typename T>
MutableArray<T>::MutableArray(size_type num, CFAllocatorRef allocator /* = NULL */)
{
	std::vector<T>	values(num, NULL);
	
	init_array(NULL, &values[0], num, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array that is initialised by @a n occurrences of @a value.
*/
template <typename T>
MutableArray<T>::MutableArray(size_type num, T value, CFAllocatorRef allocator /* = NULL */)
{
	std::vector<T>	values(num, value);
	
	init_array(NULL, &values[0], num, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a cfarr, using @a cfarr's allocator.
*/
template <typename T> inline
MutableArray<T>::MutableArray(
	CFArrayRef	cfarr)	//!< The source array.
{
	B_ASSERT(cfarr != NULL);
	
	init_array(cfarr, NULL, 0, CFGetAllocator(cfarr));
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a cfarr, using @a cfarr's allocator.
*/
template <typename T> inline
MutableArray<T>::MutableArray(
	OSPtr<CFArrayRef>	cfarr)	//!< The source array.
{
	B_ASSERT(cfarr != NULL);
	
	init_array(cfarr, NULL, 0, CFGetAllocator(cfarr));
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a cfarr, using @a cfarr's allocator.
*/
template <typename T> inline
MutableArray<T>::MutableArray(
	OSPtr<CFMutableArrayRef>	cfarr,					//!< The source array.
	bool						acquire /* = false*/)	//!< If @c true, we take @a cfarr as our internal storage, rather than making a copy.
{
	B_ASSERT(cfarr != NULL);
	
	if (acquire)
	{
		mRef = cfarr;
		CFRetain(mRef);
	}
	else
	{
		init_array(cfarr, NULL, 0, CFGetAllocator(cfarr.get()));
	}
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
MutableArray<T>::MutableArray(
	const std::vector<value_type>&	vec)	//!< The source vector.
{
	std::vector<T>	values(vec.begin(), vec.end());
	
	init_array(NULL, &values[0], values.size(), NULL);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array that is initialised by all elements of the range [@a first, @a last).
*/
template <typename T> template <class InputIterator>
MutableArray<T>::MutableArray(InputIterator first, InputIterator last, CFAllocatorRef allocator /* = NULL */)
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	std::vector<T>	values(first, last);
	
	init_array(NULL, &values[0], values.size(), allocator);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
MutableArray<T>::~MutableArray()
{
	CFRelease(mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> void
MutableArray<T>::init_array(CFArrayRef cfarr, const T* values, size_t num, CFAllocatorRef allocator)
{
	CFMutableArrayRef	mutRef;
	
	if (cfarr != NULL)
	{
		mutRef = CFArrayCreateMutableCopy(allocator, 0, cfarr);
	}
	else
	{
		mutRef = CFArrayCreateMutable(allocator, 0, &kCFTypeArrayCallBacks);
		
		if ((mutRef != NULL) && (num > 0))
		{
			CFArrayReplaceValues(mutRef, ::CFRangeMake(0, 0), (const void **) values, num);
		}
	}
	
	B_THROW_IF_NULL(mutRef);
	
	mRef = mutRef;
}

// ------------------------------------------------------------------------------------------
template <typename T> void
MutableArray<T>::replace_array(CFArrayRef cfarr)
{
	size_t			nelems	= CFArrayGetCount(cfarr);
	std::vector<T>	elems(nelems);
	
	CFArrayGetValues(cfarr, CFRangeMake(0, nelems), 
					 reinterpret_cast<const void **>(&elems[0]));
	
	CFArrayReplaceValues(mRef, ::CFRangeMake(0, size()), 
						 reinterpret_cast<const void **>(&elems[0]), nelems);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::size_type
MutableArray<T>::size() const
{
	return (CFArrayGetCount(mRef));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline bool
MutableArray<T>::empty() const
{
	return (size() == 0);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::size_type
MutableArray<T>::max_size() const
{
	return (INT_MAX);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::size_type
MutableArray<T>::capacity() const
{
	return (size());
}

// ------------------------------------------------------------------------------------------
/*!	Stub implementation.  CFMutableArrayRef doesn't have a notion of capacity that 
	parallels the one in std::vector.
*/
template <typename T> inline void
MutableArray<T>::reserve(size_type /* n */ /* = 0 */)
{
}

// ------------------------------------------------------------------------------------------
template <typename T> inline MutableArray<T>&
MutableArray<T>::operator = (const MutableArray& arr)
{
	replace_array(arr.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline MutableArray<T>&
MutableArray<T>::operator = (const Array<T>& arr)
{
	replace_array(arr.cf_ref());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline MutableArray<T>&
MutableArray<T>::operator = (
	CFArrayRef	cfarr)	//!< The input array.
{
	replace_array(cfarr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> MutableArray<T>&
MutableArray<T>::operator = (
	OSPtr<CFArrayRef>	cfarr)	//!< The input array.
{
	replace_array(cfarr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> MutableArray<T>&
MutableArray<T>::operator = (
	OSPtr<CFMutableArrayRef>	cfarr)	//!< The input array.
{
	replace_array(cfarr);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> MutableArray<T>&
MutableArray<T>::operator = (
	const std::vector<value_type>&	vec)	//!< The input vector.
{
	CFArrayReplaceValues(mRef, ::CFRangeMake(0, size()), 
						 reinterpret_cast<const void **>(&vec[0]), vec.size());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> MutableArray<T>&
MutableArray<T>::assign(size_type num, T value)
{
	std::vector<T>	elems(num, value);
	
	CFArrayReplaceValues(mRef, ::CFRangeMake(0, size()), 
						 reinterpret_cast<const void **>(&elems[0]), elems.size());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> template <class InputIterator> MutableArray<T>&
MutableArray<T>::assign(InputIterator first, InputIterator last)
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	std::vector<T>	elems(first, last);
	
	CFArrayReplaceValues(mRef, ::CFRangeMake(0, size()), 
						 reinterpret_cast<const void **>(&elems[0]), elems.size());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
MutableArray<T>::swap(MutableArray& arr)
{
	std::swap(mRef, arr.mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::reference
MutableArray<T>::at(size_type index)
{
	if (index >= size())
		B_THROW(std::out_of_range("index out of range"));
	
	return (reference(mRef, index));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::const_reference
MutableArray<T>::at(size_type index) const
{
	if (index >= size())
		B_THROW(std::out_of_range("index out of range"));
	
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, index)));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::reference
MutableArray<T>::operator [] (size_type index)
{
	return (reference(mRef, index));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::const_reference
MutableArray<T>::operator [] (size_type index) const
{
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, index)));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::reference
MutableArray<T>::front()
{
	return (reference(mRef, 0));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::const_reference
MutableArray<T>::front() const
{
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, 0)));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::reference
MutableArray<T>::back()
{
	return (reference(mRef, size()-1));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::const_reference
MutableArray<T>::back() const
{
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, size()-1)));
}

// ------------------------------------------------------------------------------------------
template <typename T> void
MutableArray<T>::copy(std::vector<value_type>& vec) const
{
	size_t	nelems	= size();
	
	vec.resize(nelems);
	
	CFArrayGetValues(mRef, CFRangeMake(0, nelems), reinterpret_cast<const void **>(&vec[0]));
}

//// ------------------------------------------------------------------------------------------
//template <> inline typename MutableArray<CFTypeRef>::const_reference
//MutableArray<CFTypeRef>::at(size_type index) const
//{
//	if ((index < 0) || (index >= size()))
//		B_THROW(std::out_of_range("index out of range"));
//	
//	return (const_reference(CFArrayGetValueAtIndex(mRef, index), false));
//}
//
//// ------------------------------------------------------------------------------------------
//template <> inline typename MutableArray<CFTypeRef>::const_reference
//MutableArray<CFTypeRef>::operator [] (size_type index) const
//{
//	return (const_reference(CFArrayGetValueAtIndex(mRef, index), false));
//}
//
//// ------------------------------------------------------------------------------------------
//template <> inline typename MutableArray<CFTypeRef>::const_reference
//MutableArray<CFTypeRef>::front() const
//{
//	return (const_reference(CFArrayGetValueAtIndex(mRef, 0), false));
//}
//
//// ------------------------------------------------------------------------------------------
//template <> inline typename MutableArray<CFTypeRef>::const_reference
//MutableArray<CFTypeRef>::back() const
//{
//	return (const_reference(CFArrayGetValueAtIndex(mRef, size()-1), false));
//}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::iterator
MutableArray<T>::begin()
{
	return (iterator(mRef, 0));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::const_iterator
MutableArray<T>::begin() const
{
	return (const_iterator(mRef, 0));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::iterator
MutableArray<T>::end()
{
	return (iterator(mRef, size()-1));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::const_iterator
MutableArray<T>::end() const
{
	return (const_iterator(mRef, size()-1));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::reverse_iterator
MutableArray<T>::rbegin()
{
	return (reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::const_reverse_iterator
MutableArray<T>::rbegin() const
{
	return (const_reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::reverse_iterator
MutableArray<T>::rend()
{
	return (reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::const_reverse_iterator
MutableArray<T>::rend() const
{
	return (const_reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::iterator
MutableArray<T>::insert(iterator pos, T value)
{
	CFArrayInsertValueAtIndex(mRef, pos.mIndex, value);
	
	return (pos);
}

// ------------------------------------------------------------------------------------------
template <typename T> void
MutableArray<T>::insert(iterator pos, size_type num, T value)
{
	std::vector<T>	elems(num, value);
	
	CFArrayReplaceValues(mRef, ::CFRangeMake(pos.mIndex, 0), 
						 (const void **) &elems[0], elems.size());
}

// ------------------------------------------------------------------------------------------
template <typename T> template <class InputIterator> inline void
MutableArray<T>::insert(iterator pos, InputIterator first, InputIterator last)
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	std::vector<T>	elems(first, last);
	
	CFArrayReplaceValues(mRef, ::CFRangeMake(pos.mIndex, 0), &elems[0], elems.size());
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
MutableArray<T>::push_back(T value)
{
	CFArrayAppendValue(mRef, value);
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::iterator
MutableArray<T>::erase(iterator pos)
{
	CFArrayReplaceValues(mRef, ::CFRangeMake(pos.mIndex, 1), NULL, 0);
	
	return (pos);
}

// ------------------------------------------------------------------------------------------
template <typename T> typename MutableArray<T>::iterator
MutableArray<T>::erase(iterator first, iterator last)
{
	CFArrayReplaceValues(mRef, ::CFRangeMake(first.mIndex, last.mIndex - first.mIndex), 
						 NULL, 0);
	
	return (first);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
MutableArray<T>::pop_back()
{
	CFArrayRemoveValueAtIndex(mRef, size()-1);
}

// ------------------------------------------------------------------------------------------
template <typename T> void
MutableArray<T>::resize(size_type num, T value /* = NULL */)
{
	size_type	old_size	= size();
	
	if (num < old_size)
	{
		CFArrayReplaceValues(mRef, ::CFRangeMake(num, old_size - num), NULL, 0);
	}
	else if (num > old_size)
	{
		std::vector<T>	elems(num - old_size, value);
		
		CFArrayReplaceValues(mRef, ::CFRangeMake(old_size, 0), 
							 (const void **) &elems[0], elems.size());
	}
}

// ------------------------------------------------------------------------------------------
template <typename T> void
MutableArray<T>::clear()
{
	CFArrayRemoveAllValues(mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline CFMutableArrayRef
MutableArray<T>::cf_ref() const
{
	return (mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline OSPtr<CFMutableArrayRef>
MutableArray<T>::cf_ptr() const
{
	return (OSPtr<CFMutableArrayRef>(mRef));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename MutableArray<T>::allocator_type
MutableArray<T>::get_allocator() const
{
	return (CFGetAllocator(mRef));
}


// ==========================================================================================
//	MutableArray Global Functions

/*!	@defgroup	MutableArrayFunctions	MutableArray Global Functions
*/
//@{

//! @name MutableArray Comparisons
//@{

/*! Compares two arrays for equality.
	
	@return		@c true if @a a1 is equal to @a a2
	@relates	MutableArray
*/
template <typename T> inline bool	operator == (const MutableArray<T>& a1, const MutableArray<T>& a2)	{ return (CFEqual(a1.cf_ref(), a2.cf_ref())); }

// ------------------------------------------------------------------------------------------
/*! Compares two arrays for inequality.
	
	@return		@c true if @a a1 is not equal to @a a2
	@relates	MutableArray
*/
template <typename T> inline bool	operator != (const MutableArray<T>& a1, const MutableArray<T>& a2)	{ return (!CFEqual(a1.cf_ref(), a2.cf_ref())); }

//@}

//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a a1 and @a a2.

	@relates	MutableArray
*/
template <typename T> inline void	swap(MutableArray<T>& a1, MutableArray<T>& a2)	{ a1.swap(a2); }

//@}

//@}

}	// namespace B

#endif	// BMutableArray_H_
