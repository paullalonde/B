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

#ifndef BAutoRegion_H_
#define BAutoRegion_H_

#pragma	once

// system headers
#include <Carbon/Carbon.h>


namespace B {

/*!
	@brief	Lifecycle management for @c RgnHandles
	
	AutoRegion is class that helps to manage the lifecycle of @c RgnHandles.  It's 
	similar in spirit to @c std::auto_ptr.  Like that class, this one has ownership-transfer 
	semantics.
	
	@note	Because of ambiguities caused by the various conversion operators, it's not 
			possible to copy-construct an AutoRegion using another AutoRegion returned 
			as a function result.  For example, the following code won't work:
			
			@code
				AutoRegion newRegion(MyFunctionReturningAutoRegion());
			@endcode
			
			You may change your code to this, which does work:
			
			@code
				AutoRegion newRegion(NULL);
				
				newRegion = MyFunctionReturningAutoRegion();
			@endcode
*/
class AutoRegion
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  Allocates an empty region.
				AutoRegion();
	//! Copy constructor.  Transfers ownership of @a ioRegion's @c RgnHandle.
				AutoRegion(AutoRegion& ioRegion);
	//! @c RgnHandle constructor.  Takes ownership of @a inRegion.
	explicit	AutoRegion(RgnHandle inRegion);
	//! Destructor.
				~AutoRegion();
	//@}
	
	//! @name Assignment
	//@{
	//! Assignment operator.  Transfers ownership of @a ioRegion's @c RgnHandle.
	AutoRegion&	operator = (AutoRegion& ioRegion);
	//@}
	
	//! @name Conversions
	//@{
	//! @c RgnHandle conversion operator.
				operator RgnHandle () const	{ return (mRgn); }
	//! Returns the underlying @c RgnHandle.
	RgnHandle	get() const					{ return (mRgn); }
	//! Relinquishes ownership of underlying @c RgnHandle.
	RgnHandle	release();
	//! Relinquishes ownership of underlying @c RgnHandle, then takes ownership of @a inRegion.
	void		reset(RgnHandle inRegion = NULL);
	//@}
	
	/*! @name Special Conversions
		
		These declarations are here to enable copies and assignment.  They are 
		similar to std::auto_ptr_ref.  See "The C++ Standard Library" by Nicolai 
		Josuttis for an explanation of why they are necessary.
	*/
	//@{
	
	//! Helper class
	struct Ref
	{
		//! Constructor.
		explicit	Ref(RgnHandle inRegion);
		//! The @c RgnHandle.
		RgnHandle   mRgn;
	};
	
	//! Helper class constructor.
				AutoRegion(Ref ref);
	//! Helper class assignment.
	AutoRegion& operator=(Ref ref);
	//! Helper class conversion.
				operator Ref ();
	//@}
	
private:
	
	RgnHandle	mRgn;
};

// ------------------------------------------------------------------------------------------
inline
AutoRegion::Ref::Ref(RgnHandle inRegion)
	: mRgn(inRegion)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoRegion::AutoRegion(AutoRegion& ioRegion)
	: mRgn(ioRegion.release())
{
}

// ------------------------------------------------------------------------------------------
inline
AutoRegion::AutoRegion(RgnHandle inRegion)
	: mRgn(inRegion)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoRegion::AutoRegion(Ref ref)
	: mRgn(ref.mRgn)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoRegion::~AutoRegion()
{
	reset();
}

// ------------------------------------------------------------------------------------------
inline AutoRegion&
AutoRegion::operator = (AutoRegion& ioRegion)
{
	reset(ioRegion.release());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline AutoRegion&
AutoRegion::operator = (Ref ref)
{
	reset(ref.mRgn);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline void
AutoRegion::reset(RgnHandle inRegion /* = NULL */)
{
	if (mRgn != inRegion)
	{
		if (mRgn != NULL)
			DisposeRgn(mRgn);
		
		mRgn = inRegion;
	}
}

// ------------------------------------------------------------------------------------------
/*!	@return	The previously-owned @c RgnHandle, or @c NULL.
*/
inline RgnHandle
AutoRegion::release()
{
	RgnHandle	oldRgn	= mRgn;
	
	mRgn = NULL;
	
	return (oldRgn);
}

// ------------------------------------------------------------------------------------------
inline
AutoRegion::operator Ref ()
{
	return (Ref(release()));
}


}	// namespace B

#endif	// BAutoRegion_H_
