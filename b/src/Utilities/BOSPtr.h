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

#ifndef BOSPtr_H_
#define BOSPtr_H_

#pragma once

// standard headers
#include <new>
#include <vector>

// system headers
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFString.h>

// B headers
#include "BErrorHandler.h"


namespace B {

namespace OSPtrOwnership {
	
// ==========================================================================================
//	OwnershipTrait

#if DOXYGEN_SCAN
	/*!
		@brief	Policy class for manipulating the refcount of an OS-managed data structure.
		
		Although data structures managed by Mac OS X are almost always "CF" 
		(i.e., CoreFoundation) objects nowadays, there remain a few data structures that have 
		their own retain/release APIs.
		
		OwnershipTrait is a "policy" class (i.e. it declares an API, but doesn't provide 
		an implementation).  It declares an API for manipulating the reference count for 
		objects managed by the OS.  This policy is implemented in a number of different
		classes (one per API obviously).
	*/
	struct OwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef void	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref);
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref);
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref);
	};
#endif
	
// ==========================================================================================
//	OwnershipTrait implementation classes

#ifndef DOXYGEN_SKIP

	/*!
		@brief	Implementation of the OwnershipTrait policy for CoreFoundation objects.
		
		CF objects include not only objects created by CoreFoundation.framework per se, 
		but also most new ref-countable objects in the OS.  For example:  HIObjectRef, 
		CGContextRef, etc.
	*/
	struct CFObjectOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CFTypeRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
		{
#if B_DEBUG_CF_RETAINCOUNTS
			assert(CFGetRetainCount(ref) > 0);
#endif
			CFRetain(ref);
		}
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
		{
#if B_DEBUG_CF_RETAINCOUNTS
			assert(CFGetRetainCount(ref) > 0);
#endif
			CFRelease(ref);
		}
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)		{ return (CFGetRetainCount(ref)); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for @c EventRef objects.
	*/
	struct EventOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef EventRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ RetainEvent(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ ReleaseEvent(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)		{ return (GetEventRetainCount(ref)); }
	};
	
	/*!
		@brief	Implementation of the OwnershipTrait policy for @c QDPictRef objects.
	*/
	struct PictOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef QDPictRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ QDPictRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ QDPictRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};
	
	/*!
		@brief	Implementation of the OwnershipTrait policy for @c WindowRef objects.
	*/
	struct WindowOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef WindowRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = RetainWindow(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = ReleaseWindow(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)
							{
								return (GetWindowRetainCount(ref));
							}
	};
	
	/*!
		@brief	Implementation of the OwnershipTrait policy for @c WindowGroupRef objects.
	*/
	struct WindowGroupOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef WindowGroupRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = RetainWindowGroup(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = ReleaseWindowGroup(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)
							{
								return (GetWindowGroupRetainCount(ref));
							}
	};
	
	/*!
		@brief	Implementation of the OwnershipTrait policy for @c MenuRef objects.
	*/
	struct MenuOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef MenuRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = RetainMenu(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = ReleaseMenu(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)
							{
								return (GetMenuRetainCount(ref));
							}
	};
	
	/*!
		@brief	Implementation of the OwnershipTrait policy for @c IconRef objects.
	*/
	struct IconOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef IconRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = AcquireIconRef(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = ReleaseIconRef(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)
							{
								UInt16		owners; 
								OSStatus	err;
								
								err = GetIconRefOwners(ref, &owners); 
								B_THROW_IF_STATUS(err);
								
								return (owners);
							}
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for @c Collection objects.
	*/
	struct CollectionOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef Collection	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = RetainCollection(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = ReleaseCollection(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)
							{
								return (GetCollectionRetainCount(ref));
							}
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for @c MouseTrackingRef objects.
	*/
	struct MouseTrackingRegionOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef MouseTrackingRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = RetainMouseTrackingRegion(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = ReleaseMouseTrackingRegion(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)	
							{
								return (1);
							}
	};
	
	/*!
		@brief	Implementation of the OwnershipTrait policy for printing-related objects.
	*/
	struct PMObjectOwnershipTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef PMObject	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)
							{
								OSStatus	err;
								
								err = PMRetain(ref);  
								B_THROW_IF_STATUS(err);
							}
		
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)
							{
								OSStatus	err;
								
								err = PMRelease(ref);
								B_THROW_IF_STATUS(err);
							}
							
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)	
							{
								return (1);
							}
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGColorSpaceRef.
	*/
	struct CGColorSpaceTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGColorSpaceRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGColorSpaceRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGColorSpaceRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGContextRef.
	*/
	struct CGContextTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGContextRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGContextRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGContextRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGDataConsumerRef.
	*/
	struct CGDataConsumerTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGDataConsumerRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGDataConsumerRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGDataConsumerRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGDataProviderRef.
	*/
	struct CGDataProviderTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGDataProviderRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGDataProviderRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGDataProviderRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGFontRef.
	*/
	struct CGFontTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGFontRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGFontRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGFontRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGFunctionRef.
	*/
	struct CGFunctionTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGFunctionRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGFunctionRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGFunctionRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGImageRef.
	*/
	struct CGImageTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGImageRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGImageRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGImageRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGLayerRef.
	*/
	struct CGLayerTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGLayerRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGLayerRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGLayerRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGPathRef.
	*/
	struct CGPathTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGPathRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGPathRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGPathRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGPatternRef.
	*/
	struct CGPatternTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGPatternRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGPatternRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGPatternRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGPDFDocumentRef.
	*/
	struct CGPDFDocumentTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGPDFDocumentRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGPDFDocumentRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGPDFDocumentRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGPDFPageRef.
	*/
	struct CGPDFPageTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGPDFPageRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGPDFPageRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGPDFPageRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

	/*!
		@brief	Implementation of the OwnershipTrait policy for CGShadingRef.
	*/
	struct CGShadingTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef CGShadingRef	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ CGShadingRetain(ref); }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ CGShadingRelease(ref); }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type)			{ return (1); }
	};

#	if defined(__OBJC__)
	/*!
		@brief	Implementation of the OwnershipTrait policy for NSObject.
	*/
	struct NSObjectTrait
	{
		//! Declares the type of the data structure whose refcount is manipulated.
		typedef NSObject*	obj_type;
		
		//! Increments the object's reference count.  Throws exceptions where appropriate.
		static void		retain(obj_type ref)	{ [ref retain]; }
		//! Decrements the object's reference count.  Throws exceptions where appropriate.
		static void		release(obj_type ref)	{ [ref release]; }
		//! Returns the object's reference count.  For APIs that don't support this functionality, a positive integer should be returned.
		static unsigned	count(obj_type ref)		{ return [ref retainCount]; }
	};
#	endif	// __OBJC__

#endif	// DOXYGEN_SKIP


// ==========================================================================================
//	TypedOwnershipTrait

	/*!
		@brief	Maps a C/C++ language type onto an OwnershipTrait policy class.
		
		The default OwnershipTrait class is CFObjectOwnershipTrait, which means that by 
		default this class will use @c CFRetain & @c CFRelease to manipulate the reference 
		count of objects of type @a T.  This is by far the most common case.
		
		For OS-defined data structures (i.e. values of @a T) that @e aren't CF objects, 
		this class template should be specialised in order to map @a T onto its particular 
		OwnershipTrait implementation class.
	*/
	template <typename T> struct TypedOwnershipTrait
	{
#if DOXYGEN_SCAN
		//! @name Template Parameters
		//@{
		typedef typename	T;	//!< The template parameter.  Should be C/C++ type defined by the OS.
		//@}
#endif
		
		//! The policy class.
		typedef CFObjectOwnershipTrait	trait_type;
	};

// ==========================================================================================
//	TypedOwnershipTrait template specialisations

#ifndef DOXYGEN_SKIP

	template <> struct TypedOwnershipTrait<EventRef>			{ typedef EventOwnershipTrait				trait_type; };
	template <> struct TypedOwnershipTrait<QDPictRef>			{ typedef PictOwnershipTrait				trait_type; };
	template <> struct TypedOwnershipTrait<WindowRef>			{ typedef WindowOwnershipTrait				trait_type; };
	template <> struct TypedOwnershipTrait<WindowGroupRef>		{ typedef WindowGroupOwnershipTrait			trait_type; };
	template <> struct TypedOwnershipTrait<MenuRef>				{ typedef MenuOwnershipTrait				trait_type; };
	template <> struct TypedOwnershipTrait<IconRef>				{ typedef IconOwnershipTrait				trait_type; };
	template <> struct TypedOwnershipTrait<Collection>			{ typedef CollectionOwnershipTrait			trait_type; };
	template <> struct TypedOwnershipTrait<MouseTrackingRef>	{ typedef MouseTrackingRegionOwnershipTrait	trait_type; };
	template <> struct TypedOwnershipTrait<PMPrintSettings>		{ typedef PMObjectOwnershipTrait			trait_type; };
	template <> struct TypedOwnershipTrait<PMPageFormat>		{ typedef PMObjectOwnershipTrait			trait_type; };
	template <> struct TypedOwnershipTrait<PMPrintSession>		{ typedef PMObjectOwnershipTrait			trait_type; };
	template <> struct TypedOwnershipTrait<PMPrinter>			{ typedef PMObjectOwnershipTrait			trait_type; };
	template <> struct TypedOwnershipTrait<CGColorSpaceRef>		{ typedef CGColorSpaceTrait					trait_type; };
	template <> struct TypedOwnershipTrait<CGContextRef>		{ typedef CGContextTrait					trait_type; };
	template <> struct TypedOwnershipTrait<CGDataConsumerRef>	{ typedef CGDataConsumerTrait				trait_type; };
	template <> struct TypedOwnershipTrait<CGDataProviderRef>	{ typedef CGDataProviderTrait				trait_type; };
	template <> struct TypedOwnershipTrait<CGFontRef>			{ typedef CGFontTrait						trait_type; };
	template <> struct TypedOwnershipTrait<CGFunctionRef>		{ typedef CGFunctionTrait					trait_type; };
	template <> struct TypedOwnershipTrait<CGLayerRef>			{ typedef CGLayerTrait						trait_type; };
	template <> struct TypedOwnershipTrait<CGPathRef>			{ typedef CGPathTrait						trait_type; };
	template <> struct TypedOwnershipTrait<CGPatternRef>		{ typedef CGPatternTrait					trait_type; };
	template <> struct TypedOwnershipTrait<CGPDFDocumentRef>	{ typedef CGPDFDocumentTrait				trait_type; };
	template <> struct TypedOwnershipTrait<CGPDFPageRef>		{ typedef CGPDFPageTrait					trait_type; };
	template <> struct TypedOwnershipTrait<CGShadingRef>		{ typedef CGShadingTrait					trait_type; };

#	if defined(__OBJC__)
	template <> struct TypedOwnershipTrait<NSObject*>			{ typedef NSObjectTrait						trait_type; };
#	endif	// __OBJC__

#endif	// DOXYGEN_SKIP

}	// namespace OSPtrOwnership

struct from_copy_t { };
extern const from_copy_t	from_copy;


/*!
	@brief	A simple "smart pointer" for reference-counted OS objects.
	
	This class basically manages the object's reference count.  It is used liberally 
	in the B framework.
	
	The class may be instantiated with any type for which an instantiation of the 
	TypedOwnershipTrait<T> class template is defined.  In practice, this means any 
	CoreFoundation object, as well as printing objects and 
	
		- CoreFoundation objects
		- Other objects that are CF-compatible (meaning they can be passed to @c CFRetain 
		  and @c CFRelease).  These days, this includes almost all new ref-counted objects 
		  in the Mac OS X Carbon/CoreServices/ApplicationServices API sets.
		- @c WindowRef and @c WindowGroupRef
		- @c MenuRef
		- @c IconRef
		- @c MouseTrackingRef
		- @c Collection
		- Common printing objects ( @c PMPrintSettings, @c PMPageFormat, @c PMPrinter, 
		  @c PMPrintSession)
	
	Although this isn't really a smart pointer in the sense that one can dereference 
	it, a number of member functions are named after similar functions in 
	@c std::auto_ptr<>.
*/
template <typename T> class OSPtr
{
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	T;	//!< The template parameter.  Should be an OS-defined type.
	//@}
#endif
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  Initialises the object to @c NULL.
				OSPtr();
	//! Copy constructor.
				OSPtr(const OSPtr& ptr);
	//! Constructor for a @a T.
	explicit	OSPtr(T ptr);
	//! Constructor for a @a T.
	explicit	OSPtr(T ptr, const from_copy_t&);
	//! Constructor for a @a T (non-throwing version).
	explicit	OSPtr(T ptr, const std::nothrow_t&);
	//! Constructor for a @a T (non-throwing version).
	explicit	OSPtr(T ptr, const from_copy_t&, const std::nothrow_t&);
	//! Destructor.
				~OSPtr();
	//@}
	
	//! @name Assignment
	//@{
	//! Assignment operator.
	OSPtr&		operator = (const OSPtr& ptr);
	//@}
	
	//! @name Access
	//@{
	//! Returns the underlying OS object.
	T			get() const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Changes the underlying OS object.
	void		reset(const OSPtr& ptr);
	//! Changes the underlying OS object.
	void		reset(T ptr);
	//! Changes the underlying OS object.
	void		reset(T ptr, const from_copy_t&);
	//! Changes the underlying OS object (non-throwing variant).
	void		reset(T ptr, const std::nothrow_t&);
	//! Changes the underlying OS object (non-throwing variant).
	void		reset(T ptr, const from_copy_t&, const std::nothrow_t&);
	//! Changes the underlying OS object to @c NULL.
	void		reset();
	//! Relenquishes ownership of the OS object.
	T			release();
	//! Exchanges the contents of the object with @a ptr.
	void		swap(OSPtr& ptr);
	//@}
	
	//! @name Conversions
	//@{
	//! Returns the underlying OS object.
	operator	T () const;
	//! Returns @c true if the underlying OS object is @b not @c NULL.
	int			operator ! () const;
	//@}
	
private:
	
	typedef typename OSPtrOwnership::TypedOwnershipTrait<T>::trait_type	trait_type;
	
	// illegal operations
	explicit	OSPtr(T ptr, bool);
	explicit	OSPtr(T ptr, bool, const std::nothrow_t&);
	explicit	OSPtr(const OSPtr& ptr, const from_copy_t&);
	explicit	OSPtr(const OSPtr& ptr, const from_copy_t&, const std::nothrow_t&);
	void		reset(T ptr, bool);
	void		reset(T ptr, bool, const std::nothrow_t&);
	void		reset(const OSPtr& ptr, bool);
	void		reset(const OSPtr& ptr, bool, const std::nothrow_t&);
	
	// member variables
	T			mPtr;	//!< The OS object.
};

// ------------------------------------------------------------------------------------------
template <typename T> inline 
OSPtr<T>::OSPtr()
	: mPtr(NULL)
{
}

// ------------------------------------------------------------------------------------------
/*!	Increments the retain count on the underlying object (if it isn't @c NULL).
*/
template <typename T> 
OSPtr<T>::OSPtr(
	const OSPtr&	ptr)	//!< The source object.
	: mPtr(ptr.mPtr)
{
	if (mPtr != NULL)	trait_type::retain(mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
OSPtr<T>::OSPtr(
	T		ptr)				//!< The OS object to wrap.
		: mPtr(NULL)
{
	reset(ptr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
OSPtr<T>::OSPtr(
	T					ptr,	//!< The OS object to wrap.
	const from_copy_t&	fc)		//!< Indicates that @a ptr came from a "Create" or "Copy" function.
		: mPtr(NULL)
{
	reset(ptr, fc);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
OSPtr<T>::OSPtr(
	T						ptr,	//!< The OS object to wrap.
	const std::nothrow_t&	nt)		//!< indicates that the caller doesn't want the function to throw an exception.
		: mPtr(NULL)
{
	reset(ptr, nt);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
OSPtr<T>::OSPtr(
	T						ptr,	//!< The OS object to wrap.
	const from_copy_t&		fc,		//!< Indicates that @a ptr came from a "Create" or "Copy" function.
	const std::nothrow_t&	nt)		//!< indicates that the caller doesn't want the function to throw an exception.
		: mPtr(NULL)
{
	reset(ptr, fc, nt);
}

// ------------------------------------------------------------------------------------------
/*!	Decrements the retain count on the underlying object (if it isn't @c NULL).
*/
template <typename T>  
OSPtr<T>::~OSPtr()
{
	if (mPtr != NULL)
	{
		B_ASSERT(trait_type::count(mPtr) > 0);
		trait_type::release(mPtr);
	}
}

// ------------------------------------------------------------------------------------------
template <typename T> inline T
OSPtr<T>::get() const
{
	return (mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> void
OSPtr<T>::reset(
	const OSPtr<T>&	ptr)				//!< The OS object to wrap.
{
	T	temp	= mPtr;
	
	mPtr = ptr;
	
	if (mPtr != NULL)
	{
		trait_type::retain(mPtr);
	}

	if (temp != NULL)
	{
		B_ASSERT(trait_type::count(temp) > 0);
		trait_type::release(temp);
	}
}

// ------------------------------------------------------------------------------------------
template <typename T> void
OSPtr<T>::reset(
	T		ptr)				//!< The OS object to wrap.
{
	B_THROW_IF_NULL(ptr);
	trait_type::retain(ptr);
	
	if (mPtr != NULL)
	{
		B_ASSERT(trait_type::count(mPtr) > 0);
		trait_type::release(mPtr);
	}
	
	mPtr = ptr;
}

// ------------------------------------------------------------------------------------------
/*!	Call this function if you have a @a T that you wish to assign to a @c OSPtr<T>, for 
	example if you obtained the @a T from a call to the OS.
	
	The function "consumes" a refcount, meaning that the object's refcount isn't 
	incremented.  This is appropriate when an OS object is created (or its refcount 
	otherwise increased) prior to being passed to this function.  In CoreFoundation terms,  
	@a ptr is assumed to have been obtained from a "Create" or "Copy" function.
	
	This function throws an exception if @a ptr is @c NULL;  this often is a result of a 
	"Create" or "Copy" function that failed.
	
	The previous underlying OS object gets its retain count decremented (if it wasn't 
	@c NULL).
	
	@exception	std::bad_alloc	If @a ptr is @c NULL.
*/
template <typename T> void
OSPtr<T>::reset(
	T		ptr,		//!< The OS object to wrap.
	const from_copy_t&)	//!< Indicates that @a ptr came from a "Create" or "Copy" function.
{
	B_THROW_IF_NULL(ptr);
	
	if (mPtr != NULL)
	{
		B_ASSERT(trait_type::count(mPtr) > 0);
		trait_type::release(mPtr);
	}
	
	mPtr = ptr;
}

// ------------------------------------------------------------------------------------------
template <typename T> void
OSPtr<T>::reset(
	T		ptr,				//!< The OS object to wrap.
	const std::nothrow_t&)		//!< indicates that the caller doesn't want the function to throw an exception.
{
	if (ptr != NULL)
		trait_type::retain(ptr);
	
	if (mPtr != NULL)
	{
		B_ASSERT(trait_type::count(mPtr) > 0);
		trait_type::release(mPtr);
	}
	
	mPtr = ptr;
}

// ------------------------------------------------------------------------------------------
/*!	Call this function if you have a @a T that you wish to assign to a @c OSPtr<T>, for 
	example if you obtained the @a T from a call to the OS.
	
	This function "consumes" a refcount, meaning that the object's refcount isn't 
	incremented.  This is appropriate when an OS object is created (or its refcount 
	otherwise increased) prior to being passed to this function.  In CoreFoundation terms,  
	@a ptr is assumed to have been obtained from a "Create" or "Copy" function.
	
	This function, unlike one of the other overloads of reset(), will quietly accept a 
	@c NULL @a ptr.
	
	The previous underlying OS object gets its retain count decremented (if it wasn't 
	@c NULL).
*/
template <typename T> void
OSPtr<T>::reset(
	T		ptr,			//!< The OS object to wrap.
	const from_copy_t&,		//!< Indicates that @a ptr came from a "Create" or "Copy" function.
	const std::nothrow_t&)	//!< indicates that the caller doesn't want the function to throw an exception.
{
	if (mPtr != NULL)
	{
		B_ASSERT(trait_type::count(mPtr) > 0);
		trait_type::release(mPtr);
	}
	
	mPtr = ptr;
}

// ------------------------------------------------------------------------------------------
/*!	The underlying OS object gets its retain count decremented (if it isn't @c NULL).
*/
template <typename T> void
OSPtr<T>::reset()
{
	if (mPtr != NULL)
		trait_type::release(mPtr);
	
	mPtr = NULL;
}

// ------------------------------------------------------------------------------------------
/*!	The OS object's retain count is @b not modified.
	
	@return	The underlying OS object.
*/
template <typename T> inline T
OSPtr<T>::release()
{
	T	tempPtr	= mPtr;
	
	mPtr = NULL;
	
	return (tempPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
OSPtr<T>::swap(OSPtr<T>& ptr)
{
	std::swap(mPtr, ptr.mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline OSPtr<T>&
OSPtr<T>::operator = (
	const OSPtr&	ptr)	//!< The source object.
{
	reset(ptr);
	return (*this);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline
OSPtr<T>::operator T () const
{
	return (mPtr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline int
OSPtr<T>::operator ! () const
{
	return (mPtr == NULL);
}


/*!	@defgroup	OSPtrFunctions	B::OSPtr<T> Global Functions
*/
//@{

// ------------------------------------------------------------------------------------------
/*!	Returns @c true if @a t1 equals @a t2.
	
	@relates	OSPtr
	@param		T	Template parameter.  Should be an OS-defined type.
*/
template <typename T> inline bool
operator == (
	const OSPtr<T>&	t1,	//!< A "smart pointer" wrapping an object of type @a T.
	const OSPtr<T>&	t2)	//!< A "smart pointer" wrapping an object of type @a T.
{
	return (t1.get() == t2.get());
}

// ------------------------------------------------------------------------------------------
/*!	Returns @c true if @a t1 equals @a t2.
	
	@relates	OSPtr
	@param		T	Template parameter.  Should be an OS-defined type.
*/
template <typename T> inline bool
operator == (
	const OSPtr<T>&	t1,	//!< A "smart pointer" wrapping an object of type @a T.
	T				t2)	//!< An OS object of type @a T.
{
	return (t1.get() == t2);
}

// ------------------------------------------------------------------------------------------
/*!	Returns @c true if @a t1 equals @a t2.
	
	@relates	OSPtr
	@param		T	Template parameter.  Should be an OS-defined type.
*/
template <typename T> inline bool
operator == (
	T				t1,	//!< An OS object of type @a T.
	const OSPtr<T>&	t2)	//!< A "smart pointer" wrapping an object of type @a T.
{
	return (t1 == t2.get());
}

// ------------------------------------------------------------------------------------------
/*!	Returns @c true if @a t1 does not equal @a t2.
	
	@relates	OSPtr
	@param		T	Template parameter.  Should be an OS-defined type.
*/
template <typename T> inline bool
operator != (
	const OSPtr<T>&	t1,	//!< A "smart pointer" wrapping an object of type @a T.
	const OSPtr<T>&	t2)	//!< A "smart pointer" wrapping an object of type @a T.
{
	return (t1.get() != t2.get());
}

// ------------------------------------------------------------------------------------------
/*!	Returns @c true if @a t1 does not equal @a t2.
	
	@relates	OSPtr
	@param		T	Template parameter.  Should be an OS-defined type.
*/
template <typename T> inline bool
operator != (
	const OSPtr<T>&	t1,	//!< A "smart pointer" wrapping an object of type @a T.
	T				t2)	//!< An OS object of type @a T.
{
	return (t1.get() != t2);
}

// ------------------------------------------------------------------------------------------
/*!	Returns @c true if @a t1 does not equal @a t2.
	
	@relates	OSPtr
	@param		T	Template parameter.  Should be an OS-defined type.
*/
template <typename T> inline bool
operator != (
	T				t1,	//!< An OS object of type @a T.
	const OSPtr<T>&	t2)	//!< A "smart pointer" wrapping an object of type @a T.
{
	return (t1 != t2.get());
}

//@}

//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a s1 and @a s2.

	@relates	OSPtr
*/
template <typename T> inline void
swap(OSPtr<T>& p1, OSPtr<T>& p2)	{ p1.swap(p2); }

//@}

}	// namespace B

#endif	// BOSPtr_H_
