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

#ifndef BUtility_H_
#define BUtility_H_

#pragma once

// standard headers
#include <stack>

// B headers
#include "BFwd.h"


namespace B {


// ==========================================================================================
//	AutoValue

/*!
	@brief	Temporarily changes the value of a variable.
	
	The constructor saves (and optionally changes) the variable, and the destructor 
	restores the old value.
*/
template <typename T>
class AutoValue
{
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	T;		//!< The template parameter.  Any assignable type will do.
	//@}
#endif
	
	//! @name Constructor / Destructor
	//@{
	
	//! Constructor.  Saves the value of @a inVar, then sets @a inVar to @a inValue.
	AutoValue(T& inVar, const T& inValue)
		: mVar(inVar), mOldValue(mVar)
	{
		mVar = inValue;
	}
	
	//! Constructor.  Saves the value of @a inVar.
	AutoValue(T& inVar)
		: mVar(inVar), mOldValue(inVar)
	{
	}
	
	//! Constructor.  Restores the saved value of @a inVar.
	~AutoValue()
	{
		mVar = mOldValue;
	}
	
	//@}
	
private:
	
	T&	mVar;
	T	mOldValue;
};


// ==========================================================================================
//	DeleteObject

/*!
	@brief	Functor that deletes the object it's called with.
	
	This is a little helper class that can be used to delete the elements of a container.
	
	This implementation comes from the book "Effective STL" by Scott Meyers (item 7).
*/
template <typename T>
struct DeleteObject : public std::unary_function<const T*, void>
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	T;		//!< The template parameter.  Any deletable type will do.
	//@}
#endif
	
	//! Functor invocation.  Deletes @a ptr.
	void	operator () (const T* ptr) const
	{
		delete ptr;
	}
};


// ==========================================================================================
//	Dereference

/*!
	@brief	Functor that dereferences the object it's called with.
	
	This is a little helper class that can be used to dereference the elements of a container.
	
	This implementation comes from the book "Effective STL" by Scott Meyers (item 20).
*/
template <typename T>
struct Dereference : public std::unary_function<const T*, T>
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	T;		//!< The template parameter.  Any dereferenceable type will do.
	//@}
#endif
	
	//! Functor invocation.  Dereferences @a ptr.
	const T&	operator () (const T* ptr) const
	{
		return (*ptr);
	}
};


// ==========================================================================================
//	MapKey

/*!
	@brief	Functor that returns the first member of a <tt>std::pair<></tt>.
	
	This is a little helper class that can be used to retrieve the key of a map element.
*/
template <typename KEY, typename VALUE>
struct MapKey : public std::unary_function< std::pair<const KEY, VALUE>, KEY>
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	KEY;	//!< The template parameter.  Any type will do.
	typedef typename	VALUE;	//!< The template parameter.  Any type will do.
	//@}
#endif
	
	//! Functor invocation.  Returns <i>pair.first</i>.
	const KEY&	operator () (const std::pair<const KEY, VALUE>& elem) const
	{
		return (elem.first);
	}
};


// ==========================================================================================
//	MapValue

/*!
	@brief	Functor that returns the second member of a <tt>std::pair<></tt>.
	
	This is a little helper class that can be used to retrieve the value of a map element.
*/
template <typename KEY, typename VALUE>
struct MapValue : public std::unary_function< std::pair<const KEY, VALUE>, VALUE>
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	KEY;	//!< The template parameter.  Any type will do.
	typedef typename	VALUE;	//!< The template parameter.  Any type will do.
	//@}
#endif
	
	//! Functor invocation.  Returns <i>pair.second</i>.
	const VALUE&	operator () (const std::pair<const KEY, VALUE>& elem) const
	{
		return (elem.second);
	}
};


}	// namespace B


#endif	// BUtility_H_
