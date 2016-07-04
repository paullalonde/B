// ==========================================================================================
//	
//	Copyright (C) 2005-2006 Paul Lalonde enrg.
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

#ifndef BAutoTrackingArea_H_
#define BAutoTrackingArea_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B {

class   Shape;
class   View;

/*!
	@brief	Lifecycle management for @c HIViewTrackingAreaRefs
	
	AutoTrackingArea is class that helps to manage the lifecycle of @c HIViewTrackingAreaRefs.  It's 
	similar in spirit to @c std::auto_ptr.  Like that class, this one as ownership-transfer 
	semantics.
	
	@note	Because of ambiguities caused by the various conversion operators, it's not 
			possible to copy-construct an AutoTrackingArea using another AutoTrackingArea returned 
			as a function result.  For example, the following code won't work:
			
			@code
				AutoTrackingArea newRegion(MyFunctionReturningTrackingArea());
			@endcode
			
			You may change your code to this, which does work:
			
			@code
				AutoTrackingArea newArea(NULL);
				
				newArea = MyFunctionReturningTrackingArea();
			@endcode
*/
class AutoTrackingArea
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
				AutoTrackingArea();
	//! Constructor from HIToolbox objects.
				AutoTrackingArea(
                    HIViewRef               inViewRef,
                    HIShapeRef              inShapeRef,
                    HIViewTrackingAreaID    inID = 0);
	//! Copy constructor.  Transfers ownership of @a ioTrackingArea's @c HIViewTrackingAreaRef.
				AutoTrackingArea(AutoTrackingArea& ioTrackingArea);
	//! @c HIViewTrackingAreaRef constructor.  Takes ownership of @a inTrackingArea.
	explicit	AutoTrackingArea(HIViewTrackingAreaRef inTrackingArea);
	//! Destructor.
				~AutoTrackingArea();
	//@}
	
	//! @name Assignment
	//@{
	//! Assignment operator.  Transfers ownership of @a ioTrackingArea's @c HIViewTrackingAreaRef.
	AutoTrackingArea&	operator = (AutoTrackingArea& ioTrackingArea);
    //! Assignment from HIToolbox objects.
    AutoTrackingArea&   assign(
                            HIViewRef               inViewRef,
                            HIShapeRef              inShapeRef,
                            HIViewTrackingAreaID    inID = 0);
	//@}
	
    //! @name Inquiries
	//@{
    HIViewTrackingAreaID    GetID() const;
	//@}
    
	//! @name Modifiers
	//@{
    void    SetShape(HIShapeRef inShapeRef);
	//@}
    
	//! @name Conversions
	//@{
	//! @c HIViewTrackingAreaRef conversion operator.
    operator HIViewTrackingAreaRef () const	{ return (mTrackingArea); }
	//! Returns the underlying @c HIViewTrackingAreaRef.
	HIViewTrackingAreaRef	get() const { return (mTrackingArea); }
	//! Relinquishes ownership of underlying @c HIViewTrackingAreaRef.
	HIViewTrackingAreaRef	release();
	//! Relinquishes ownership of underlying @c HIViewTrackingAreaRef, then takes ownership of @a inTrackingArea.
	void                    reset(HIViewTrackingAreaRef inTrackingArea = NULL);
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
		explicit                Ref(HIViewTrackingAreaRef inArea);
		//! The @c RgnHandle.
		HIViewTrackingAreaRef   mArea;
	};
	
	//! Helper class constructor.
                        AutoTrackingArea(Ref ref);
	//! Helper class assignment.
	AutoTrackingArea&   operator=(Ref ref);
	//! Helper class conversion.
                        operator Ref ();
	//@}
	
private:
	
	HIViewTrackingAreaRef	mTrackingArea;
};

// ------------------------------------------------------------------------------------------
inline
AutoTrackingArea::AutoTrackingArea()
	: mTrackingArea(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoTrackingArea::AutoTrackingArea(AutoTrackingArea& ioTrackingArea)
	: mTrackingArea(ioTrackingArea.release())
{
}

// ------------------------------------------------------------------------------------------
inline
AutoTrackingArea::AutoTrackingArea(HIViewTrackingAreaRef inTrackingArea)
	: mTrackingArea(inTrackingArea)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoTrackingArea::AutoTrackingArea(Ref ref)
	: mTrackingArea(ref.mArea)
{
}

// ------------------------------------------------------------------------------------------
inline
AutoTrackingArea::~AutoTrackingArea()
{
	reset();
}

// ------------------------------------------------------------------------------------------
inline AutoTrackingArea&
AutoTrackingArea::operator = (AutoTrackingArea& ioTrackingArea)
{
	reset(ioTrackingArea.release());
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline AutoTrackingArea&
AutoTrackingArea::operator = (Ref ref)
{
	reset(ref.mArea);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline AutoTrackingArea&
AutoTrackingArea::assign(
    HIViewRef               inViewRef,
    HIShapeRef              inShapeRef,
    HIViewTrackingAreaID    inID /* = 0 */)
{
    AutoTrackingArea    area(inViewRef, inShapeRef, inID);
    
    return operator = (area);
}

// ------------------------------------------------------------------------------------------
inline void
AutoTrackingArea::reset(HIViewTrackingAreaRef inTrackingArea /* = NULL */)
{
	if (mTrackingArea != inTrackingArea)
	{
		if (mTrackingArea != NULL)
			HIViewDisposeTrackingArea(mTrackingArea);
		
		mTrackingArea = inTrackingArea;
	}
}

// ------------------------------------------------------------------------------------------
/*!	@return	The previously-owned @c HIViewTrackingAreaRef, or @c NULL.
*/
inline HIViewTrackingAreaRef
AutoTrackingArea::release()
{
	HIViewTrackingAreaRef	oldArea	= mTrackingArea;
	
	mTrackingArea = NULL;
	
	return (oldArea);
}

// ------------------------------------------------------------------------------------------
inline
AutoTrackingArea::operator Ref ()
{
	return (Ref(release()));
}


}	// namespace B


#endif	// BAutoTrackingArea_H_
