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

#ifndef BViewUtils_H_
#define BViewUtils_H_

#pragma once

// standard headers
#include <iterator>

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class	Rect;
class	Size;


/*!	@brief	A collection of useful utility functions for getting information about 
			@c HIViewRefs.
*/
namespace ViewUtils
{
	//! @name Types
	//@{

	template <typename SPACE> struct CoordinateSpaceTraits
	{
	};
	
	template <> struct CoordinateSpaceTraits<HIViewRef>
	{
		enum { kHISpace = kHICoordSpaceView };
	};
	
	template <> struct CoordinateSpaceTraits<WindowRef>
	{
		enum { kHISpace = kHICoordSpaceWindow };
	};
	
	//@}
	
	//! @name Coordinate Conversions
	//@{

	//! Converts @a inValue from the coordinate system of @a inSrcSpace to that of @a inDstSpace.
	template <typename VALUE, typename SRC_SPACE, typename DST_SPACE>
	inline VALUE
	Convert(
		const VALUE&	inValue,
		SRC_SPACE		inSrcSpace,
		DST_SPACE		inDstSpace)
	{
		return ConvertValue(inValue, 
							CoordinateSpaceTraits<SRC_SPACE>::kHISpace,
							reinterpret_cast<void*>(inSrcSpace), 
							CoordinateSpaceTraits<DST_SPACE>::kHISpace,
							reinterpret_cast<void*>(inDstSpace));
					   
	}

	//! Converts @a inValue from the coordinate system of @a inSrcSpace to global coordinates.
	template <typename VALUE, typename SRC_SPACE>
	inline VALUE
	ConvertToGlobal(
		const VALUE&	inValue,
		SRC_SPACE		inSrcSpace)
	{
		return ConvertValue(inValue, 
							CoordinateSpaceTraits<SRC_SPACE>::kHISpace,
							reinterpret_cast<void*>(inSrcSpace), 
							kHICoordSpace72DPIGlobal, NULL);
	}

	//! Converts @a inValue from global coordinates to the coordinate system of @a inDstSpace.
	template <typename VALUE, typename DST_SPACE>
	inline VALUE
	ConvertFromGlobal(
		const VALUE&	inValue,
		DST_SPACE		inDstSpace)
	{
		return ConvertValue(inValue, 
							kHICoordSpace72DPIGlobal, NULL,
							CoordinateSpaceTraits<DST_SPACE>::kHISpace,
							reinterpret_cast<void*>(inDstSpace));
	}

	//! Converts @a inValue from the coordinate system of @a inSrcSpace to screen coordinates.
	template <typename VALUE, typename SRC_SPACE>
	inline VALUE
	ConvertToScreen(
		const VALUE&	inValue,
		SRC_SPACE		inSrcSpace)
	{
		return ConvertValue(inValue, 
							CoordinateSpaceTraits<SRC_SPACE>::kHISpace,
							reinterpret_cast<void*>(inSrcSpace), 
							kHICoordSpaceScreenPixel, NULL);
	}

	//! Converts @a inValue from screen coordinates to the coordinate system of @a inDstSpace.
	template <typename VALUE, typename DST_SPACE>
	inline VALUE
	ConvertFromScreen(
		const VALUE&	inValue,
		DST_SPACE		inDstSpace)
	{
		return ConvertValue(inValue, 
							kHICoordSpaceScreenPixel, NULL,
							CoordinateSpaceTraits<DST_SPACE>::kHISpace,
							reinterpret_cast<void*>(inDstSpace));
	}
	
	//! Converts @a inValue from the coordinate system of @a inSrcSpace and @a inSrcObject to that of @a inDstSpace and @a inDstObject.
	CGPoint	ConvertValue(
				const CGPoint&		inValue,
				HICoordinateSpace	inSrcSpace,
				void*				inSrcObject,
				HICoordinateSpace	inDstSpace,
				void*				inDstObject);
	/*! @overload
	*/
	CGSize	ConvertValue(
				const CGSize&		inValue,
				HICoordinateSpace	inSrcSpace,
				void*				inSrcObject,
				HICoordinateSpace	inDstSpace,
				void*				inDstObject);
	/*! @overload
	*/
	CGRect	ConvertValue(
				const CGRect&		inValue,
				HICoordinateSpace	inSrcSpace,
				void*				inSrcObject,
				HICoordinateSpace	inDstSpace,
				void*				inDstObject);
	//@}
	
	//! @name Content View
	//@{
	
	//! Returns the content view of @a inViewRef's owning window.
	HIViewRef	GetOwnerContentView(
					HIViewRef	inViewRef);
	/*! @overload
	*/
	HIViewRef	GetOwnerContentView(
					HIViewRef	inViewRef, 
					bool		inComposited);
	
	//! Returns the bounds of the content view of @a inViewRef's owning window.
	Rect		GetOwnerContentViewBounds(
					HIViewRef	inViewRef);
	/*! @overload
	*/
	Rect		GetOwnerContentViewBounds(
					HIViewRef	inViewRef, 
					bool		inComposited);
	//@}
	
	//! @name Finding Views
	//@{
	//! Returns the subview of @a inSuperview that matches @a inID.
	HIViewRef	FindSubview(
					HIViewRef		inSuperview,
					const HIViewID&	inID);
	//! Returns the subview of @a inSuperview that matches the main bundle's signature and @a inID.
	HIViewRef	FindSubview(
					HIViewRef		inSuperview,
					int				inID);
	//! Returns the subview of @a inSuperview that matches @a inID, or @c NULL.
	HIViewRef	FindSubview(
					HIViewRef		inSuperview,
					const HIViewID&	inID,
					const std::nothrow_t&);
	//! Returns the subview of @a inSuperview that matches the main bundle's signature and @a inID, or @c NULL.
	HIViewRef	FindSubview(
					HIViewRef		inSuperview,
					int				inID,
					const std::nothrow_t&);
	//@}
	

// ==========================================================================================
//	SubviewIterator

#pragma mark SubviewIterator

/*!
	@brief	Iterates over the children of an HIView.
	
	This is a forward iterator, but without the ability to modify the referenced 
	sequence.  It's suitable for use in typical "const_iterator" situations.
	
	The iteration starts with the parent view's first child, then proceeds forwards.
*/
class SubviewIterator : public std::iterator<std::forward_iterator_tag, 
											 HIViewRef, 
											 ptrdiff_t, 
											 HIViewRef*, 
											 HIViewRef&>
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  It's equivalent to an end-of-sequence iterator.
	SubviewIterator();
	//! Copy constructor.
	SubviewIterator(const SubviewIterator& it);
	//! HIView constructor.  The object is initialised to point to @a inParentViewRef's first child.
	SubviewIterator(HIViewRef inParentViewRef);
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	SubviewIterator&	operator = (const SubviewIterator& it);
	//@}
	
	//! @name Iterator Specifics
	//@{
	//! Returns the @c HIViewRef that the iterator currently points to.
	value_type			operator * () const;
	//! Advances the iterator, returning the new position.
	SubviewIterator&	operator ++ ();
	//! Advances the iterator, returning the old position.
	SubviewIterator		operator ++ (int);
	//@}
	
private:
	
	// member variables
	HIViewRef	mRef;
};

// ------------------------------------------------------------------------------------------
inline
SubviewIterator::SubviewIterator()
	: mRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
SubviewIterator::SubviewIterator(const SubviewIterator& it)
	: mRef(it.mRef)
{
}

// ------------------------------------------------------------------------------------------
inline
SubviewIterator::SubviewIterator(HIViewRef inParentViewRef)
	: mRef(HIViewGetFirstSubview(inParentViewRef))
{
}

// ------------------------------------------------------------------------------------------
inline SubviewIterator&
SubviewIterator::operator = (const SubviewIterator& it)
{
	mRef = it.mRef;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline SubviewIterator::value_type
SubviewIterator::operator * () const
{
	return (mRef);
}

// ------------------------------------------------------------------------------------------
inline SubviewIterator&
SubviewIterator::operator ++ ()
{
	mRef = HIViewGetNextView(mRef);
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline SubviewIterator
SubviewIterator::operator ++ (int)
{
	return (SubviewIterator(HIViewGetNextView(mRef)));
}


// ==========================================================================================
//	SubviewIterator Global Functions

//! Compares @a it1 and @a it2 for equality.
inline bool	operator == (const SubviewIterator& it1, const SubviewIterator& it2)	{ return (*it1 == *it2); }
//! Compares @a it1 and @a it2 for inequality.
inline bool	operator != (const SubviewIterator& it1, const SubviewIterator& it2)	{ return (*it1 != *it2); }


// ==========================================================================================
//	ReverseSubviewIterator

#pragma mark -
#pragma mark ReverseSubviewIterator

/*!
	@brief	Iterates over the children of an HIView in reverse order.
	
	This is a forward iterator, but without the ability to modify the referenced 
	sequence.  It's suitable for use in typical "const_iterator" situations.
	
	The iteration starts with the parent view's last child, then proceeds backwards.
*/
class ReverseSubviewIterator : public std::iterator<std::forward_iterator_tag, 
													HIViewRef, 
													ptrdiff_t, 
													HIViewRef*, 
													HIViewRef&>
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  It's equivalent to an end-of-sequence iterator.
	ReverseSubviewIterator();
	//! Copy constructor.
	ReverseSubviewIterator(const ReverseSubviewIterator& it);
	//! HIView constructor.  The object is initialised to point to @a inParentViewRef's first child.
	ReverseSubviewIterator(HIViewRef inParentViewRef);
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	ReverseSubviewIterator&	operator = (const ReverseSubviewIterator& it);
	//@}
	
	//! @name Iterator Specifics
	//@{
	//! Returns the @c HIViewRef that the iterator currently points to.
	value_type				operator * () const;
	//! Advances the iterator, returning the new position.
	ReverseSubviewIterator&	operator ++ ();
	//! Advances the iterator, returning the old position.
	ReverseSubviewIterator	operator ++ (int);
	//@}
	
private:
	
	// member variables
	HIViewRef	mRef;
};

// ------------------------------------------------------------------------------------------
inline
ReverseSubviewIterator::ReverseSubviewIterator()
	: mRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
ReverseSubviewIterator::ReverseSubviewIterator(const ReverseSubviewIterator& it)
	: mRef(it.mRef)
{
}

// ------------------------------------------------------------------------------------------
inline
ReverseSubviewIterator::ReverseSubviewIterator(HIViewRef inParentViewRef)
	: mRef(HIViewGetLastSubview(inParentViewRef))
{
}

// ------------------------------------------------------------------------------------------
inline ReverseSubviewIterator&
ReverseSubviewIterator::operator = (const ReverseSubviewIterator& it)
{
	mRef = it.mRef;
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline ReverseSubviewIterator::value_type
ReverseSubviewIterator::operator * () const
{
	return (mRef);
}

// ------------------------------------------------------------------------------------------
inline ReverseSubviewIterator&
ReverseSubviewIterator::operator ++ ()
{
	mRef = HIViewGetPreviousView(mRef);
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline ReverseSubviewIterator
ReverseSubviewIterator::operator ++ (int)
{
	return (ReverseSubviewIterator(HIViewGetPreviousView(mRef)));
}


// ==========================================================================================
//	ReverseSubviewIterator Global Functions

//! Compares @a it1 and @a it2 for equality.
inline bool	operator == (const ReverseSubviewIterator& it1, const ReverseSubviewIterator& it2)	{ return (*it1 == *it2); }
//! Compares @a it1 and @a it2 for inequality.
inline bool	operator != (const ReverseSubviewIterator& it1, const ReverseSubviewIterator& it2)	{ return (*it1 != *it2); }


	//! @name Iterators
	//@{
	
	//! Returns an iterator pointing to the beginning of @a inSuperview's subviews.
	inline SubviewIterator
	begin(
		HIViewRef	inSuperview)		//!< The view over whose subviews we shall iterate.
	{
		return (SubviewIterator(inSuperview));
	}
	
	//! Returns an iterator pointing to the end of @a inSuperview's subviews.
	inline SubviewIterator
	end(
		HIViewRef	/* inSuperview */)	//!< The view over whose subviews we shall iterate.
	{
		SubviewIterator it;
		
		return (it);
	}
	
	//! Returns an reverse iterator pointing to the beginning of @a inSuperview's subviews.
	inline ReverseSubviewIterator
	rbegin(
		HIViewRef	inSuperview)		//!< The view over whose subviews we shall iterate.
	{
		return (ReverseSubviewIterator(inSuperview));
	}
	
	//! Returns an reverse iterator pointing to the end of @a inSuperview's subviews.
	inline ReverseSubviewIterator
	rend(
		HIViewRef	/* inSuperview */)	//!< The view over whose subviews we shall iterate.
	{
		ReverseSubviewIterator  it;
		
		return (it);
	}
	//@}
	

// ==========================================================================================
//	ImageContent

/*!
	@brief  Helper class for custom views that display image content.
	
	This is a little wrapper class around the @c HIViewImageContentInfo structure, 
    which is used by the standard Toolbox controls to get/set image content.  It allows 
    implementers of custom views to add the ability to draw image content -- icons, 
	PICTs, CGImages -- without too much fuss.
	
	The idea is that you add to your custom view a member variable of type 
	Viewutils::ImageContent, and that within the your SetData() function, you assign 
	the incoming @c HIViewImageContentInfo to the member variable.  Likewise, within 
	Draw(), you call one of ImageContent's drawing functions, depending on whether you 
	draw to a @c GrafPort or a @c CGContextRef.
	
	@note   Not all content types can currently be drawn to both a @c GrafPort and a 
			@a CGContextRef.  You can call CanDrawIntoGrafPort() or CanDrawIntoContext() 
            to validate the content type.
    
    @note   The @c CGImageRef content type is only supported when building on 10.3 or 
            later.
*/
class ImageContent
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  Sets the object to have no content.
	ImageContent();
	//! Copy constructor.
	ImageContent(const ImageContent& inContent);
	//! @c HIViewImageContentInfo constructor.
	ImageContent(const HIViewImageContentInfo& inInfo);
	//! Destructor.
	~ImageContent();
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	ImageContent&	operator = (const ImageContent& inContent);
	//! @c HIViewImageContentInfo assignment.
	ImageContent&	operator = (const HIViewImageContentInfo& inInfo);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns whether there is any content at all.
	bool	HasContent() const;
	//! Returns the embedded content info struct.
	void	GetContentInfo(HIViewImageContentInfo& outInfo) const;
	//! Returns whether @a inContentType can be drawn in a @c GrafPort.
	static bool
            CanDrawIntoGrafPort(HIViewImageContentType inContentType);
	//! Returns whether the object's content can be drawn in a @c GrafPort.
	bool	CanDrawIntoGrafPort() const;
	//! Returns whether @a inContentType can be drawn in a @c CGContextRef.
	static bool
            CanDrawIntoContext(HIViewImageContentType inContentType);
	//! Returns whether the content can be drawn in a @c CGContextRef.
	bool	CanDrawIntoContext() const;
	//! Returns the content's "ideal" size.
	Size	GetIdealSize(const CGSize& inBaseSize) const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Releases all content.
	void	Clear();
	//! Exchanges the contents of two ImageContent objects.
	void	swap(ImageContent& ioContent);
	
	//@}
	
	//! Draws the content into the current @c GrafPort.
	void	Draw(
				const Rect&			inRect,
				IconAlignmentType	inAlignment,
				IconTransformType	inTransform) const;
	//! Draws the content into @a inContext.
	void	Draw(
				CGContextRef		inContext,
				const Rect&			inRect,
				IconAlignmentType	inAlignment,
				IconTransformType	inTransform) const;
	
private:
	
	// Types
	union ResourceData {
		CIconHandle	cIconHandle;
		Handle		iconSuite;
		PicHandle	picture;
		Handle		ICONHandle;
	};
	
	void	Init(const HIViewImageContentInfo& inInfo);
	
	// member variables
	HIViewImageContentInfo	mInfo;
	ResourceData			mData;
};

// ------------------------------------------------------------------------------------------
inline bool
ImageContent::HasContent() const
{
	return (mInfo.contentType != kControlNoContent);
}

// ------------------------------------------------------------------------------------------
inline void
ImageContent::GetContentInfo(HIViewImageContentInfo& outInfo) const
{
	outInfo = mInfo;
}

// ------------------------------------------------------------------------------------------
inline bool
ImageContent::CanDrawIntoGrafPort() const
{
    return (CanDrawIntoGrafPort(mInfo.contentType));
}

// ------------------------------------------------------------------------------------------
inline bool
ImageContent::CanDrawIntoContext() const
{
    return (CanDrawIntoContext(mInfo.contentType));
}

// ------------------------------------------------------------------------------------------
inline void
ImageContent::swap(ImageContent& ioContent)
{	
	std::swap(mInfo, ioContent.mInfo);
	std::swap(mData, ioContent.mData);
}

// ------------------------------------------------------------------------------------------
inline ImageContent&
ImageContent::operator = (const ImageContent& inContent)
{
	ImageContent(inContent).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
inline ImageContent&
ImageContent::operator = (const HIViewImageContentInfo& inInfo)
{
	ImageContent(inInfo).swap(*this);
	
	return (*this);
}


// ------------------------------------------------------------------------------------------
inline HIViewID	MakeViewID(OSType sig, SInt32 id)	{ HIViewID viewID = { sig, id }; return viewID; }

// ------------------------------------------------------------------------------------------
HIViewID		MakeViewID(SInt32 id);

// ------------------------------------------------------------------------------------------
inline HIViewID	MakeViewID()	{ HIViewID viewID = { 0, 0 }; return viewID; }

// ------------------------------------------------------------------------------------------
inline ControlKind	MakeControlKind(OSType sig, OSType kind)	{ ControlKind ctlKind = { sig, kind }; return ctlKind; }


}	// namespace ViewUtils

}	// namespace B


// ==========================================================================================
//	HIViewID Global Functions

//! @name View ID Operators
//@{

// ------------------------------------------------------------------------------------------
/*! Tests two @c HIViewIDs for equality.
	@relates	ViewUtils
*/
inline bool operator == (const HIViewID& id1, const HIViewID& id2)
			{ return ((id1.signature == id2.signature) && (id1.id == id2.id)); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c HIViewIDs.
	@return		@c true if @a id1 is less than @a id2.
	@relates	ViewUtils
*/
inline bool operator <  (const HIViewID& id1, const HIViewID& id2)
			{ return ((id1.signature < id2.signature) || 
					   ((id1.signature == id2.signature) && (id1.id < id2.id))); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c HIViewIDs.
	@return		@c true if @a id1 is greater than @a id2.
	@relates	ViewUtils
*/
inline bool operator >  (const HIViewID& id1, const HIViewID& id2)	
			{ return ((id1.signature > id2.signature) || 
					   ((id1.signature == id2.signature) && (id1.id > id2.id))); }

// ------------------------------------------------------------------------------------------
/*! Tests two @c HIViewIDs for inequality.
	@relates	ViewUtils
*/
inline bool operator != (const HIViewID& id1, const HIViewID& id2)	{ return (!(id1 == id2)); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c HIViewIDs.
	@return		@c true if @a id1 is less than or equal to @a id2.
	@relates	ViewUtils
*/
inline bool operator <= (const HIViewID& id1, const HIViewID& id2)	{ return (!(id1 >  id2)); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c HIViewIDs.
	@return		@c true if @a id1 is greater than or equal to @a id2.
	@relates	ViewUtils
*/
inline bool operator >= (const HIViewID& id1, const HIViewID& id2)	{ return (!(id1 < id2)); }

//@}


// ==========================================================================================
//	ControlKind Global Functions

//! @name ControlKind Operators
//@{

// ------------------------------------------------------------------------------------------
/*! Tests two @c ControlKinds for equality.
	@relates	ViewUtils
*/
inline bool operator == (const ControlKind& k1, const ControlKind& k2)
			{ return ((k1.signature == k2.signature) && (k1.kind == k2.kind)); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c ControlKinds.
	@return		@c true if @a k1 is less than @a k2.
	@relates	ViewUtils
*/
inline bool operator <  (const ControlKind& k1, const ControlKind& k2)
			{ return ((k1.signature < k2.signature) || 
					   ((k1.signature == k2.signature) && (k1.kind < k2.kind))); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c ControlKinds.
	@return		@c true if @a k1 is greater than @a k2.
	@relates	ViewUtils
*/
inline bool operator >  (const ControlKind& k1, const ControlKind& k2)	
			{ return ((k1.signature > k2.signature) || 
					   ((k1.signature == k2.signature) && (k1.kind > k2.kind))); }

// ------------------------------------------------------------------------------------------
/*! Tests two @c ControlKinds for inequality.
	@relates	ViewUtils
*/
inline bool operator != (const ControlKind& k1, const ControlKind& k2)	{ return (!(k1 == k2)); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c ControlKinds.
	@return		@c true if @a k1 is less than or equal to @a k2.
	@relates	ViewUtils
*/
inline bool operator <= (const ControlKind& k1, const ControlKind& k2)	{ return (!(k1 >  k2)); }

// ------------------------------------------------------------------------------------------
/*! Ordering of two @c ControlKinds.
	@return		@c true if @a k1 is greater than or equal to @a k2.
	@relates	ViewUtils
*/
inline bool operator >= (const ControlKind& k1, const ControlKind& k2)	{ return (!(k1 < k2)); }

//@}


#endif	// BViewUtils_H_
