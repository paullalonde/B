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

#ifndef BArray_H_
#define BArray_H_

#pragma once

// standard headers
#include <iterator>
#include <vector>

// system headers
#include <CoreFoundation/CFArray.h>

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BErrorHandler.h"
#include "CFUtils.h"


namespace B {

template <typename T> class	MutableArray;

// ==========================================================================================
//	Array

/*!
	@brief	Adapter class for immutable arrays.
	
	Array provides a wrapper around a CoreFoundation @c CFArrayRef object, giving it an 
	interface similar (nearly identical in fact) to @c std::vector.  This makes it 
	possible to use Array with STL algorithms, with standard streams, etc.
	
	Array tries to share its underlying @c CFArrayRef whenever possible.
	
	Specifically, Array implements the @c const member functions of @c std::vector.  This 
	reflects the immutable nature of the underlying @c CFArrayRef.  The exception to this 
	is the various overloads of operator = () and assign(), whose absence would have 
	made the class much more difficult to use;  they are implemented by replacing the 
	underlying @c CFArrayRef, rather than by changing its contents.
	
	Other notable differences with @c std::vector :
		
		- The template parameter @a T is assumed to be a CoreFoundation type.  Specifically, 
		  it needs to obey the semantics of @c CFRetain() and @c CFRelease(), and there 
		  needs to be a template specialisation of @c CFUGetType<>() for it.
		  
		- There are more conversions.  This reflects Array's role as an 
		  adapter between CoreFoundation and the Standard C++ Library.  For example, one 
		  may construct instances of Array with any of the following types:
			- Array
			- MutableArray
			- <tt>CFArrayRef</tt>
			- <tt>OSPtr<CFArrayRef></tt>
			- <tt>OSPtr<CFMutableArrayRef></tt>
			- <tt>std::vector</tt>
		
		- Likewise, one may copy() an Array back into an @c std::vector.
		  
		- The allocator type is fixed as @c CFAllocatorRef.  The allocator type is not a 
		  template parameter, however allocators @e are passed as arguments to 
		  constructors and can be retrieved via get_allocator().
		  
		- The underlying @c CFArrayRef's storage is not accessible, so it's not 
		  possible to take the address of an element of an Array.  Likewise, at(), 
		  operator [] (), front() and back() return objects which can extract the 
		  element's value.  Array is incompatible with the STL in this respect.
		
		- Array doesn't support the following global comparison operators: 
		  @c < @c <= @c > @c >= .  This is because ordering of CoreFoundation types isn't 
		  well-defined.  The operators @c == and @c != @e are supported and are 
		  implemented in terms of @c CFEqual().
*/
template <typename T>
class Array
{
public:
	
	//! @name Types
	//@{
	typedef OSPtr<T>			value_type;			//!< The type of the array's elements.
	typedef size_t				size_type;			//!< The unsigned integral type for size values and indices.
	typedef ptrdiff_t			difference_type;	//!< The signed integral type for difference values.
	typedef value_type			const_reference;	//!< The type of constant element references.
	typedef value_type*			pointer;			//!< The type of element pointers.
	typedef const value_type*	const_pointer;		//!< The type of constant element pointers.
	class						iterator;			//!< The type of iterators.
	class						const_iterator;		//!< The type of constant iterators.
	typedef CFAllocatorRef		allocator_type;		//!< The type of the allocator.
	
	/*!
		@todo	%Document this class!
	*/
	class reference
	{
	public:
		
		typedef Array						container_type;
		typedef typename Array::value_type	value_type;
		
		operator value_type () const					{ return (make_value()); }
		reference&	operator = (const value_type value)	{ CFArraySetValueAtIndex(mRef, mIndex, value.get()); return (*this); }
		reference&	operator = (const T value)			{ CFArraySetValueAtIndex(mRef, mIndex, value); return (*this); }
		
		// friends
		
		friend bool	operator == (const reference& ref1, const reference& ref2)	{ return (ref1.make_value() == ref2.make_value()); }
		friend bool	operator != (const reference& ref1, const reference& ref2)	{ return (ref1.make_value() != ref2.make_value()); }
		friend bool	operator >  (const reference& ref1, const reference& ref2)	{ return (ref1.make_value() >  ref2.make_value()); }
		friend bool	operator >= (const reference& ref1, const reference& ref2)	{ return (ref1.make_value() >= ref2.make_value()); }
		friend bool	operator <  (const reference& ref1, const reference& ref2)	{ return (ref1.make_value() <  ref2.make_value()); }
		friend bool	operator <= (const reference& ref1, const reference& ref2)	{ return (ref1.make_value() <= ref2.make_value()); }
		
		friend bool	operator == (const reference& ref, const value_type value)	{ return (ref.make_value() == value); }
		friend bool	operator != (const reference& ref, const value_type value)	{ return (ref.make_value() != value); }
		friend bool	operator >  (const reference& ref, const value_type value)	{ return (ref.make_value() >  value); }
		friend bool	operator >= (const reference& ref, const value_type value)	{ return (ref.make_value() >= value); }
		friend bool	operator <  (const reference& ref, const value_type value)	{ return (ref.make_value() <  value); }
		friend bool	operator <= (const reference& ref, const value_type value)	{ return (ref.make_value() <= value); }
		
		friend bool	operator == (const value_type value, const reference& ref)	{ return (value == ref.make_value()); }
		friend bool	operator != (const value_type value, const reference& ref)	{ return (value != ref.make_value()); }
		friend bool	operator >  (const value_type value, const reference& ref)	{ return (value >  ref.make_value()); }
		friend bool	operator >= (const value_type value, const reference& ref)	{ return (value >= ref.make_value()); }
		friend bool	operator <  (const value_type value, const reference& ref)	{ return (value <  ref.make_value()); }
		friend bool	operator <= (const value_type value, const reference& ref)	{ return (value <= ref.make_value()); }
		
		friend bool	operator == (const reference& ref, const T value)			{ return (ref.make_value().get() == value); }
		friend bool	operator != (const reference& ref, const T value)			{ return (ref.make_value().get() != value); }
		friend bool	operator >  (const reference& ref, const T value)			{ return (ref.make_value().get() >  value); }
		friend bool	operator >= (const reference& ref, const T value)			{ return (ref.make_value().get() >= value); }
		friend bool	operator <  (const reference& ref, const T value)			{ return (ref.make_value().get() <  value); }
		friend bool	operator <= (const reference& ref, const T value)			{ return (ref.make_value().get() <= value); }
		
		friend bool	operator == (const T value, const reference& ref)			{ return (value == ref.make_value().get()); }
		friend bool	operator != (const T value, const reference& ref)			{ return (value != ref.make_value().get()); }
		friend bool	operator >  (const T value, const reference& ref)			{ return (value >  ref.make_value().get()); }
		friend bool	operator >= (const T value, const reference& ref)			{ return (value >= ref.make_value().get()); }
		friend bool	operator <  (const T value, const reference& ref)			{ return (value <  ref.make_value().get()); }
		friend bool	operator <= (const T value, const reference& ref)			{ return (value <= ref.make_value().get()); }
		
	private:
		
		// constructor
		reference(CFMutableArrayRef inRef, CFIndex inIndex)
			: mRef(inRef), mIndex(inIndex) {}
		
		value_type	make_value() const	{ return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, mIndex))); }
		
		// member variables
		CFMutableArrayRef	mRef;
		CFIndex				mIndex;
		
		// friends
		friend class	Array;
		friend class	MutableArray<T>;
		friend class	Array::iterator;
	};
	
	/*!
		@todo	%Document this class!
	*/
	class iterator : public std::iterator<std::random_access_iterator_tag, 
										  typename Array::value_type, 
										  typename Array::difference_type, 
										  typename Array::pointer, 
										  typename Array::reference>
	{
	private:
		
		typedef std::iterator<std::random_access_iterator_tag, 
							  typename Array::value_type, 
							  typename Array::difference_type, 
							  typename Array::pointer, 
							  typename Array::reference> base;

	public:
		
		// types
		typedef typename base::value_type		value_type;
		typedef typename base::difference_type	difference_type;
		typedef typename base::pointer			pointer;
		typedef typename base::reference		reference;
		
		// constructor
		iterator()
			: mRef(NULL), mIndex(0) {}
		iterator(const iterator& it)
			: mRef(it.mRef), mIndex(it.mIndex)	{}
		
		iterator&	operator = (const iterator& it)
			{ mRef = it.mRef; mIndex = it.mIndex; return (*this); }
		
		value_type	operator * () const						{ return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, mIndex))); }
		reference	operator * ()							{ return (reference(mRef, mIndex)); }
		value_type	operator [] (difference_type i) const	{ return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, mIndex + i))); }
		reference	operator [] (difference_type i)			{ return (reference(mRef, mIndex + i)); }
		
		iterator&	operator ++ ()							{ ++mIndex; return (*this); }
		iterator&	operator -- ()							{ --mIndex; return (*this); }
		iterator&	operator += (difference_type n)			{ mIndex += n; return (*this); }
		iterator&	operator -= (difference_type n)			{ mIndex -= n; return (*this); }
		iterator	operator ++ (int)						{ return (iterator(mRef, mIndex++)); }
		iterator	operator -- (int)						{ return (iterator(mRef, mIndex--)); }
		iterator	operator +  (difference_type n) const	{ return (iterator(mRef, mIndex + n)); }
		iterator	operator -  (difference_type n) const	{ return (iterator(mRef, mIndex - n)); }
		
		// friends
		friend iterator			operator +  (difference_type n, const iterator& it)		{ return (iterator(it.mRef, n + it.mIndex)); }
		friend iterator			operator -  (difference_type n, const iterator& it)		{ return (iterator(it.mRef, n - it.mIndex)); }
		friend difference_type	operator -  (const iterator& it1, const iterator& it2)	{ return (it1.mIndex -  it2.mIndex); }
		friend bool				operator == (const iterator& it1, const iterator& it2)	{ return (it1.mIndex == it2.mIndex); }
		friend bool				operator != (const iterator& it1, const iterator& it2)	{ return (it1.mIndex != it2.mIndex); }
		friend bool				operator >  (const iterator& it1, const iterator& it2)	{ return (it1.mIndex >  it2.mIndex); }
		friend bool				operator >= (const iterator& it1, const iterator& it2)	{ return (it1.mIndex >= it2.mIndex); }
		friend bool				operator <  (const iterator& it1, const iterator& it2)	{ return (it1.mIndex <  it2.mIndex); }
		friend bool				operator <= (const iterator& it1, const iterator& it2)	{ return (it1.mIndex <= it2.mIndex); }
		
	private:
		
		iterator(CFMutableArrayRef inRef, CFIndex inIndex)
			: mRef(inRef), mIndex(inIndex)	{}
		
		// member variables
		CFMutableArrayRef	mRef;
		CFIndex				mIndex;
		
		// friends
		friend class	Array;
		friend class	MutableArray<T>;
		friend class	Array::const_iterator;
	};
	
	/*!
		@todo	%Document this class!
	*/
	class const_iterator : public std::iterator<std::random_access_iterator_tag, 
												typename Array::value_type, 
												typename Array::difference_type, 
												typename Array::const_pointer, 
												typename Array::const_reference>
	{
	private:
		
		typedef std::iterator<std::random_access_iterator_tag, 
							  typename Array::value_type, 
							  typename Array::difference_type, 
							  typename Array::const_pointer, 
							  typename Array::const_reference> base;

	public:
		
		// types
		typedef typename base::value_type		value_type;
		typedef typename base::difference_type	difference_type;
		typedef typename base::pointer			pointer;
		typedef typename base::reference		reference;
		
		// constructor
		const_iterator()
			: mRef(NULL), mIndex(0) {}
		const_iterator(const const_iterator& it)
			: mRef(it.mRef), mIndex(it.mIndex)	{}
		const_iterator(const Array::iterator& it)
			: mRef(it.mRef), mIndex(it.mIndex)	{}
		
		const_iterator&	operator = (const const_iterator& it)
			{ mRef = it.mRef; mIndex = it.mIndex; return (*this); }
		const_iterator&	operator = (const Array::iterator& it)
			{ mRef = it.mRef; mIndex = it.mIndex; return (*this); }
		
		value_type	operator * () const							{ return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, mIndex))); }
		value_type	operator [] (difference_type i) const		{ return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, mIndex + i))); }
		
		const_iterator&	operator ++ ()							{ ++mIndex; return (*this); }
		const_iterator&	operator -- ()							{ --mIndex; return (*this); }
		const_iterator&	operator += (difference_type n)			{ mIndex += n; return (*this); }
		const_iterator&	operator -= (difference_type n)			{ mIndex -= n; return (*this); }
		const_iterator	operator ++ (int)						{ return (const_iterator(mRef, mIndex++)); }
		const_iterator	operator -- (int)						{ return (const_iterator(mRef, mIndex--)); }
		const_iterator	operator +  (difference_type n) const	{ return (const_iterator(mRef, mIndex + n)); }
		const_iterator	operator -  (difference_type n) const	{ return (const_iterator(mRef, mIndex - n)); }
		
		// friends
		friend const_iterator	operator +  (difference_type n, const const_iterator& it)			{ return (const_iterator(it.mRef, n + it.mIndex)); }
		friend const_iterator	operator -  (difference_type n, const const_iterator& it)			{ return (const_iterator(it.mRef, n - it.mIndex)); }
		friend difference_type	operator -  (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex -  it2.mIndex); }
		friend bool				operator == (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex == it2.mIndex); }
		friend bool				operator != (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex != it2.mIndex); }
		friend bool				operator >  (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex >  it2.mIndex); }
		friend bool				operator >= (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex >= it2.mIndex); }
		friend bool				operator <  (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex <  it2.mIndex); }
		friend bool				operator <= (const const_iterator& it1, const const_iterator& it2)	{ return (it1.mIndex <= it2.mIndex); }
		
	private:
		
		const_iterator(CFArrayRef inRef, CFIndex inIndex)
			: mRef(inRef), mIndex(inIndex)	{}
		
		// member variables
		CFArrayRef	mRef;
		CFIndex		mIndex;
		
		// friends
		friend class	Array;
		friend class	MutableArray<T>;
	};

	typedef std::reverse_iterator<iterator>			reverse_iterator;		//!< The type of reverse iterators.
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;	//!< The type of constant reverse iterators.

	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
				Array();
	//! Copy constructor.
				Array(const Array& arr);
	//! Allocator constructor.
	explicit	Array(CFAllocatorRef allocator);
	//! Size constructor.
	explicit	Array(size_type num, CFAllocatorRef allocator = NULL);
	//! @a T constructor.
	explicit	Array(size_type num, T value, CFAllocatorRef allocator = NULL);
	//! MutableArray constructor.
	explicit	Array(const MutableArray<T>& arr);
	//! @c CFArrayRef constructor.
	explicit	Array(CFArrayRef cfarr);
	//! @c OSPtr<CFArrayRef> constructor.
	explicit	Array(OSPtr<CFArrayRef> cfarr);
	//! @c OSPtr<CFMutableArrayRef> constructor.
	explicit	Array(OSPtr<CFMutableArrayRef> cfarr);
	//! @c vector constructor.
	explicit	Array(const std::vector<value_type>& vec);
	//! Range constructor.
	template <class InputIterator>
	explicit	Array(InputIterator first, InputIterator last, CFAllocatorRef allocator = NULL);
	//! Destructor.
				~Array();
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
	//@}
	
	/*! @name Assignment
		
		These various overloads of @c operator = and @c assign() replace the array's 
		underlying @c CFArrayRef.
	*/
	//@{
	//! Replaces all existing elements with copies of the elements of @a arr.
	Array&	operator = (const Array& arr);
	//! MutableArray assignemnt.
	Array&	operator = (const MutableArray<T>& arr);
	//! @c CFArrayRef assignemnt.
	Array&	operator = (CFArrayRef cfarr);
	//! @c OSPtr<CFArrayRef> assignemnt.
	Array&	operator = (OSPtr<CFArrayRef> cfarr);
	//! @c OSPtr<CFMutableArrayRef> assignemnt.
	Array&	operator = (OSPtr<CFMutableArrayRef> cfarr);
	//! @c vector assignemnt.
	Array&	operator = (const std::vector<value_type>& vec);
	//! Replaces all existing elements with @a num copies of @a value.
	Array&	assign(size_type num, T value);
	//! Replaces all existing elements with copies of the elements in the range [@a first, @a last).
	template <class InputIterator>
	Array&	assign(InputIterator first, InputIterator last);
	//! Exchanges the contents with @a arr.
	void			swap(Array& arr);
	//@}
	
	//! @name Direct Element Access
	//@{
	//! Returns the element with index @a index.
	const_reference	at(size_type index) const;
	//! Returns the element with index @a index.
	const_reference	operator [] (size_type index) const;
	//! Returns the first element.
	const_reference	front() const;
	//! Returns the last element.
	const_reference	back() const;
	//! Converts the array into a @a vector.
	void			copy(std::vector<value_type>& vec) const;
	//@}
	
	//! @name Operations to Generate Iterators
	//@{
	//! Returns an iterator for the beginning of the array.
	const_iterator			begin() const;
	//! Returns an iterator for the end of the array.
	const_iterator			end() const;
	//! Returns an iterator for the beginning of a reverse iteration of the array.
	const_reverse_iterator	rbegin() const;
	//! Returns an iterator for the end of a reverse iteration of the array.
	const_reverse_iterator	rend() const;
	//@}
	
	//! @name Inserting and Removing Elements
	//@{
	//! Removes all of the elements.
	void		clear();
	//@}
	
	//! @name Generating CFArrayRefs.
	//@{
	//! Returns the underlying @c CFArrayRef.
	CFArrayRef			cf_ref() const;
	//! Returns a "smart pointer" to the underlying @c CFArrayRef.
	OSPtr<CFArrayRef>	cf_ptr() const;
	//@}
	
	//! @name Allocator Support
	//@{
	//! Returns the memory model of the array.
	allocator_type	get_allocator() const;
	//@}

private:
	
	void	init_array(CFArrayRef cfarr, const T* values, size_t num, CFAllocatorRef allocator);
	
	// member variables
	CFArrayRef	mRef;
};

// ------------------------------------------------------------------------------------------
/*!	Creates an empty array using the default allocator.
*/
template <typename T>
Array<T>::Array()
{
	init_array(NULL, NULL, 0, NULL);
}

// ------------------------------------------------------------------------------------------
/*!	Creates a shallow copy of @a arr.
*/
template <typename T> inline
Array<T>::Array(const Array& arr)
{
	init_array(arr.cf_ref(), NULL, 0, arr.get_allocator());
}

// ------------------------------------------------------------------------------------------
/*!	Creates an empty array using @a allocator.
*/
template <typename T>
Array<T>::Array(CFAllocatorRef allocator)
{
	init_array(NULL, NULL, 0, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array containing @a num elements of type @a T whose value is @c NULL.
*/
template <typename T>
Array<T>::Array(size_type num, CFAllocatorRef allocator /* = NULL */)
{
	std::vector<T>	values(num, NULL);
	
	init_array(NULL, &values[0], num, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array that is initialised by @a n occurrences of @a value.
*/
template <typename T>
Array<T>::Array(size_type num, T value, CFAllocatorRef allocator /* = NULL */)
{
	std::vector<T>	values(num, value);
	
	init_array(NULL, &values[0], num, allocator);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a arr, using @a arr's allocator.
*/
template <typename T>
Array<T>::Array(
	const MutableArray<T>&	arr)	//!< The source array.
{
	init_array(arr.cf_ref(), NULL, 0, arr.get_allocator());
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a cfarr, using @a cfarr's allocator.
*/
template <typename T> inline
Array<T>::Array(
	CFArrayRef	cfarr)	//!< The source array.
{
	B_ASSERT(cfarr != NULL);
	
	mRef = CFArrayCreateCopy(CFGetAllocator(cfarr), cfarr);
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a cfarr, using @a cfarr's allocator.
*/
template <typename T> inline
Array<T>::Array(
	OSPtr<CFArrayRef>	cfarr)	//!< The source array.
{
	B_ASSERT(cfarr != NULL);
	
	init_array(cfarr.get(), NULL, 0, CFGetAllocator(cfarr.get()));
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array as a copy of @a cfarr, using @a cfarr's allocator.
*/
template <typename T> inline
Array<T>::Array(
	OSPtr<CFMutableArrayRef>	cfarr)	//!< The source array.
{
	B_ASSERT(cfarr != NULL);
	
	init_array(cfarr.get(), NULL, 0, CFGetAllocator(cfarr.get()));
}

// ------------------------------------------------------------------------------------------
template <typename T>
Array<T>::Array(
	const std::vector<value_type>&	vec)	//!< The source vector.
{
	std::vector<T>	values(vec.begin(), vec.end());

	init_array(NULL, &values[0], values.size(), NULL);
}

// ------------------------------------------------------------------------------------------
/*!	Creates an array that is initialised by all elements of the range [@a first, @a last).
*/
template <typename T> template <class InputIterator>
Array<T>::Array(InputIterator first, InputIterator last, CFAllocatorRef allocator /* = NULL */)
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	std::vector<T>	values(first, last);
	
	init_array(NULL, &values[0], values.size(), allocator);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
Array<T>::~Array()
{
	CFRelease(mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> void
Array<T>::init_array(CFArrayRef cfarr, const T* values, size_t num, CFAllocatorRef allocator)
{
	if (cfarr != NULL)
	{
		mRef = CFArrayCreateCopy(allocator, cfarr);
	}
	else
	{
		mRef = CFArrayCreate(allocator, (const void **) values, num, &kCFTypeArrayCallBacks);
	}
	
	B_THROW_IF_NULL(mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::size_type
Array<T>::size() const
{
	return (CFArrayGetCount(mRef));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline bool
Array<T>::empty() const
{
	return (size() == 0);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::size_type
Array<T>::max_size() const
{
	return (INT_MAX);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::size_type
Array<T>::capacity() const
{
	return (size());
}

// ------------------------------------------------------------------------------------------
template <typename T> inline Array<T>&
Array<T>::operator = (const Array& arr)
{
	CFRetain(arr.cf_ref());
	CFRelease(mRef);
	mRef = arr.cf_ref();
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> Array<T>&
Array<T>::operator = (const MutableArray<T>& arr)	//!< The input array.
{
	Array	temp(arr);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> Array<T>&
Array<T>::operator = (CFArrayRef cfarr)	//!< The input array.
{
	Array	temp(cfarr);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> Array<T>&
Array<T>::operator = (OSPtr<CFArrayRef> cfarr)	//!< The input array.
{
	Array	temp(cfarr);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> Array<T>&
Array<T>::operator = (OSPtr<CFMutableArrayRef> cfarr)	//!< The input array.
{
	Array	temp(cfarr);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> Array<T>&
Array<T>::operator = (const std::vector<value_type>& vec)	//!< The input vector.
{
	Array	temp(vec);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> Array<T>&
Array<T>::assign(size_type num, T value)
{
	Array	temp(num, value);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> template <class InputIterator> Array<T>&
Array<T>::assign(InputIterator first, InputIterator last)
{
	boost::function_requires< boost::InputIteratorConcept<InputIterator> >();
	
	Array	temp(first, last);
	
	swap(temp);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
Array<T>::swap(Array& arr)
{
	std::swap(mRef, arr.mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> typename Array<T>::const_reference
Array<T>::at(size_type index) const
{
	if (index >= size())
		B_THROW(std::out_of_range("index out of range"));
	
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, index)));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::const_reference
Array<T>::operator [] (size_type index) const
{
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, index)));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::const_reference
Array<T>::front() const
{
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, 0)));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::const_reference
Array<T>::back() const
{
	return (CFUMakeTypedValue<T>(CFArrayGetValueAtIndex(mRef, size()-1)));
}

// ------------------------------------------------------------------------------------------
template <typename T> void
Array<T>::copy(std::vector<value_type>& vec) const
{
	vec.clear();
	vec.reserve(size());
	vec.insert(vec.begin(), begin(), end());
}

//// ------------------------------------------------------------------------------------------
//template <> inline typename Array<CFTypeRef>::const_reference
//Array<CFTypeRef>::at(size_type index) const
//{
//	if ((index < 0) || (index >= size()))
//		B_THROW(std::out_of_range("index out of range"));
//	
//	return (const_reference(CFArrayGetValueAtIndex(mRef, index), false));
//}
//
//// ------------------------------------------------------------------------------------------
//template <> inline typename Array<CFTypeRef>::const_reference
//Array<CFTypeRef>::operator [] (size_type index) const
//{
//	return (const_reference(CFArrayGetValueAtIndex(mRef, index), false));
//}
//
//// ------------------------------------------------------------------------------------------
//template <> inline typename Array<CFTypeRef>::const_reference
//Array<CFTypeRef>::front() const
//{
//	return (const_reference(CFArrayGetValueAtIndex(mRef, 0), false));
//}
//
//// ------------------------------------------------------------------------------------------
//template <> inline typename Array<CFTypeRef>::const_reference
//Array<CFTypeRef>::back() const
//{
//	return (const_reference(CFArrayGetValueAtIndex(mRef, size()-1), false));
//}

// ------------------------------------------------------------------------------------------
template <typename T> typename Array<T>::const_iterator
Array<T>::begin() const
{
	return (const_iterator(mRef, 0));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename Array<T>::const_iterator
Array<T>::end() const
{
	return (const_iterator(mRef, size()-1));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename Array<T>::const_reverse_iterator
Array<T>::rbegin() const
{
	return (const_reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
template <typename T> typename Array<T>::const_reverse_iterator
Array<T>::rend() const
{
	return (const_reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
template <typename T> void
Array<T>::clear()
{
	Array().swap(*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline CFArrayRef
Array<T>::cf_ref() const
{
	return (mRef);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline OSPtr<CFArrayRef>
Array<T>::cf_ptr() const
{
	return (OSPtr<CFArrayRef>(mRef, false));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline typename Array<T>::allocator_type
Array<T>::get_allocator() const
{
	return (CFGetAllocator(mRef));
}


// ==========================================================================================
//	Array Global Functions

/*!	@defgroup	ArrayFunctions	Array Global Functions
*/
//@{

//! @name Array Comparisons
//@{

/*! Compares two arrays for equality.
	
	@return		@c true if @a a1 is equal to @a a2
	@relates	Array
*/
template <typename T> inline bool	operator == (const Array<T>& a1, const Array<T>& a2)	{ return (CFEqual(a1.cf_ref(), a2.cf_ref())); }

// ------------------------------------------------------------------------------------------
/*! Compares two arrays for inequality.
	
	@return		@c true if @a a1 is not equal to @a a2
	@relates	Array
*/
template <typename T> inline bool	operator != (const Array<T>& a1, const Array<T>& a2)	{ return (!CFEqual(a1.cf_ref(), a2.cf_ref())); }

//@}

//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a s1 and @a s2.

	@relates	Array
*/
template <typename T> inline void	swap(Array<T>& s1, Array<T>& s2)	{ s1.swap(s2); }

//@}

//@}


}	// namespace B

#endif	// BArray_H_
