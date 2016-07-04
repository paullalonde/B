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

// file header
#include "BViewUtils.h"

// B headers
#include "BErrorHandler.h"
#include "BMutableShape.h"
#include "BRect.h"
#include "BWindowUtils.h"


namespace B {

namespace ViewUtils {

// ------------------------------------------------------------------------------------------
CGPoint
ConvertValue(
	const CGPoint&		inValue,
	HICoordinateSpace	inSrcSpace,
	void*				inSrcObject,
	HICoordinateSpace	inDstSpace,
	void*				inDstObject)
{
	CGPoint	point	= inValue;
	
	HIPointConvert(&point, inSrcSpace, inSrcObject, inDstSpace, inDstObject);
	
	return point;
}

// ------------------------------------------------------------------------------------------
CGSize
ConvertValue(
	const CGSize&		inValue,
	HICoordinateSpace	inSrcSpace,
	void*				inSrcObject,
	HICoordinateSpace	inDstSpace,
	void*				inDstObject)
{
	CGSize	size	= inValue;
	
	HISizeConvert(&size, inSrcSpace, inSrcObject, inDstSpace, inDstObject);
	
	return size;
}

// ------------------------------------------------------------------------------------------
CGRect
ConvertValue(
	const CGRect&		inValue,
	HICoordinateSpace	inSrcSpace,
	void*				inSrcObject,
	HICoordinateSpace	inDstSpace,
	void*				inDstObject)
{
	CGRect	rect	= inValue;
	
	HIRectConvert(&rect, inSrcSpace, inSrcObject, inDstSpace, inDstObject);
	
	return rect;
}

// ------------------------------------------------------------------------------------------
/*!	In a compositing window, the function returns the <em>bona fide</em> content view.  In 
	a non-compositing window, it returns the root control.
*/
HIViewRef
GetOwnerContentView(
	HIViewRef	inView)			//!< The view.
{
	B_ASSERT(inView != NULL);
	
	WindowRef	owner	= GetControlOwner(inView);
	
	B_ASSERT(owner != NULL);
	
	return (WindowUtils::GetContentView(owner));
}

// ------------------------------------------------------------------------------------------
/*!	Use this variant when you know already whether @a inView's owning window is a 
	compositing window.
*/
HIViewRef
GetOwnerContentView(
	HIViewRef	inView,			//!< The view.
	bool		inComposited)	//!< Is @a inView in a compositing window?
{
	B_ASSERT(inView != NULL);
	
	WindowRef	owner	= GetControlOwner(inView);
	
	B_ASSERT(owner != NULL);
	
	return (WindowUtils::GetContentView(owner, inComposited));
}

// ------------------------------------------------------------------------------------------
/*!	In a compositing window, the function returns the bounds of the <em>bona fide</em> 
	content view.  In a non-compositing window, it returns the bounds of the root control.
*/
Rect
GetOwnerContentViewBounds(
	HIViewRef	inView)		//!< The view.
{
	B_ASSERT(inView != NULL);
	
	WindowRef	owner	= GetControlOwner(inView);
	
	B_ASSERT(owner != NULL);
	
	return WindowUtils::GetContentViewBounds(owner);
}

// ------------------------------------------------------------------------------------------
/*!	Use this variant when you know already whether @a inView's owning window is a 
	compositing window.
*/
Rect
GetOwnerContentViewBounds(
	HIViewRef	inView,			//!< The view.
	bool		inComposited)	//!< Is @a inView in a compositing window?
{
	B_ASSERT(inView != NULL);
	
	WindowRef	owner	= GetControlOwner(inView);
	
	B_ASSERT(owner != NULL);
	
	return WindowUtils::GetContentViewBounds(owner, inComposited);
}

// ------------------------------------------------------------------------------------------
/*!	@return	The requested subview of @a inSuperview.  Never returns @c NULL (an 
			exception is thrown if something goes wrong).
*/
HIViewRef
FindSubview(
	HIViewRef		inSuperview,	//!< The root of the search.
	const HIViewID&	inID)			//!< The identity of the view we want.
{
	HIViewRef	viewRef;
	OSStatus	err;
	
	err = HIViewFindByID(inSuperview, inID, &viewRef);
	B_THROW_IF_STATUS(err);
	
	return (viewRef);
}

// ------------------------------------------------------------------------------------------
/*!	The signature portion of the HIViewID is taken from the main bundle.
	
	@return	The requested subview of @a inSuperview.  Never returns @c NULL (an 
			exception is thrown if something goes wrong).
*/
HIViewRef
FindSubview(
	HIViewRef		inSuperview,	//!< The root of the search.
	int				inID)			//!< The identity of the view we want.
{
	HIViewID	viewID	= { Bundle::Main().PackageCreator(), inID };
	
	return (FindSubview(inSuperview, viewID));
}

// ------------------------------------------------------------------------------------------
/*!	@return	The requested subview of @a inSuperview, or @c NULL if the view wasn't found.
*/
HIViewRef
FindSubview(
	HIViewRef		inSuperview,	//!< The root of the search.
	const HIViewID&	inID,			//!< The identity of the view we want.
	const std::nothrow_t&)			//!< An indication that the caller doesn't want the function to throw.
{
	HIViewRef	viewRef;
	OSStatus	err;
	
	err = HIViewFindByID(inSuperview, inID, &viewRef);
	
	if (err != noErr)
		viewRef = NULL;
	
	return (viewRef);
}

// ------------------------------------------------------------------------------------------
/*!	The signature portion of the HIViewID is taken from the main bundle.
	
	@return	The requested subview of @a inSuperview, or @c NULL if the view wasn't found.
*/
HIViewRef
FindSubview(
	HIViewRef				inSuperview,	//!< The root of the search.
	int						inID,			//!< The identity of the view we want.
	const std::nothrow_t&	nt)				//!< An indication that the caller doesn't want the function to throw.
{
	HIViewID	viewID	= { Bundle::Main().PackageCreator(), inID };
	
	return (FindSubview(inSuperview, viewID, nt));
}

// ==========================================================================================
//	ImageContent

#pragma mark -

// ------------------------------------------------------------------------------------------
ImageContent::ImageContent()
{
	mInfo.contentType = kControlNoContent;
}

// ------------------------------------------------------------------------------------------
ImageContent::ImageContent(const ImageContent& inContent)
{
	Init(inContent.mInfo);
}

// ------------------------------------------------------------------------------------------
ImageContent::ImageContent(const HIViewImageContentInfo& inInfo)
{
	Init(inInfo);
}

// ------------------------------------------------------------------------------------------
ImageContent::~ImageContent()
{
	// Clear() doesn't promise to never throw, so we need to guard against that.
	
	try
	{
		Clear();
	}
	catch (...)
	{
	}
}

// ------------------------------------------------------------------------------------------
void
ImageContent::Init(const HIViewImageContentInfo& inInfo)
{
	OSStatus	err;
	
	mInfo = inInfo;
	
	switch (mInfo.contentType)
	{
	case kControlNoContent:
		// There's nothing to do.
		break;
		
	case kControlContentIconSuiteRes:
		err = GetIconSuite(&mData.iconSuite, mInfo.u.resID, kSelectorAllAvailableData);
		B_THROW_IF_STATUS(err);
		break;
		
	case kControlContentCIconRes:
		mData.cIconHandle = GetCIcon(mInfo.u.resID);
		B_THROW_IF_NULL_RSRC(mData.cIconHandle);
		break;
		
	case kControlContentPictRes:
		mData.picture = GetPicture(mInfo.u.resID);
		B_THROW_IF_NULL_RSRC(mData.picture);
		break;
		
	case kControlContentICONRes:
		mData.ICONHandle = GetIcon(mInfo.u.resID);
		B_THROW_IF_NULL_RSRC(mData.ICONHandle);
		break;
		
	case kControlContentIconSuiteHandle:
		mData.iconSuite = mInfo.u.iconSuite;
		if (mData.iconSuite == NULL)
			B_THROW_STATUS(paramErr);
		break;
		
	case kControlContentCIconHandle:
		mData.cIconHandle = mInfo.u.cIconHandle;
		if (mData.cIconHandle == NULL)
			B_THROW_STATUS(paramErr);
		break;
		
	case kControlContentPictHandle:
		mData.picture = mInfo.u.picture;
		if (mData.picture == NULL)
			B_THROW_STATUS(paramErr);
		break;
		
	case kControlContentIconRef:
		err = AcquireIconRef(mInfo.u.iconRef);
		B_THROW_IF_STATUS(err);
		break;
		
	case kControlContentICON:
		mData.ICONHandle = mInfo.u.ICONHandle;
		if (mData.ICONHandle == NULL)
			B_THROW_STATUS(paramErr);
		break;
		
#if B_BUILDING_ON_10_3_OR_LATER
	case kControlContentCGImageRef:
		CFRetain(mInfo.u.imageRef);
		break;
#endif
		
	default:
		// Unknown content type.
		B_THROW_STATUS(paramErr);
		break;
	}
}

// ------------------------------------------------------------------------------------------
void
ImageContent::Clear()
{
	OSStatus	err;
	
	switch (mInfo.contentType)
	{
	case kControlNoContent:
		// There's nothing to do.
		break;
		
	case kControlContentIconSuiteRes:
		err = DisposeIconSuite(mData.iconSuite, false);
		B_THROW_IF_STATUS(err);
		break;
		
	case kControlContentCIconRes:
		DisposeCIcon(mData.cIconHandle);
		break;
		
	case kControlContentPictRes:
		ReleaseResource(reinterpret_cast<Handle>(mData.picture));
		break;
		
	case kControlContentICONRes:
		ReleaseResource(mData.ICONHandle);
		break;
		
	case kControlContentIconRef:
		err = ReleaseIconRef(mInfo.u.iconRef);
		B_THROW_IF_STATUS(err);
		break;
		
#if B_BUILDING_ON_10_3_OR_LATER
	case kControlContentCGImageRef:
		CFRelease(mInfo.u.imageRef);
		break;
#endif
		
	default:
		// There's nothing to do.
		break;
	}
}

// ------------------------------------------------------------------------------------------
Size
ImageContent::GetIdealSize(const CGSize& inBaseSize) const
{
	Size	idealSize;
	::Rect	qdRect;
	
	switch (mInfo.contentType)
	{
	case kControlNoContent:
	default:
		// There's nothing to do.
		idealSize = inBaseSize;
		break;
		
	case kControlContentIconSuiteRes:
	case kControlContentIconSuiteHandle:
		// ### SIMPLISTIC IMPLEMENTATION ###
		// If the requested size is greater than or equal to 32, use 32;  else use 16.
		if ((inBaseSize.width >= 32) && (inBaseSize.height >= 32))
			idealSize.width = idealSize.height = 32;
		else
			idealSize.width = idealSize.height = 16;
		break;
		
	case kControlContentCIconRes:
	case kControlContentCIconHandle:
		qdRect				= (*mData.cIconHandle)->iconPMap.bounds;
		idealSize.width		= qdRect.right - qdRect.left;
		idealSize.height	= qdRect.bottom - qdRect.top;
		break;
		
	case kControlContentPictRes:
	case kControlContentPictHandle:
		qdRect				= (*mData.picture)->picFrame;
		idealSize.width		= qdRect.right - qdRect.left;
		idealSize.height	= qdRect.bottom - qdRect.top;
		break;
		
	case kControlContentICONRes:
	case kControlContentICON:
		idealSize.width = idealSize.height = 16;
		break;
		
	case kControlContentIconRef:
		// ### SIMPLISTIC IMPLEMENTATION ###
		if ((inBaseSize.width >= 128) && (inBaseSize.height >= 128))
			idealSize.width = idealSize.height = 32;
		else if ((inBaseSize.width >= 48) && (inBaseSize.height >= 48))
			idealSize.width = idealSize.height = 32;
		else if ((inBaseSize.width >= 32) && (inBaseSize.height >= 32))
			idealSize.width = idealSize.height = 32;
		else
			idealSize.width = idealSize.height = 16;
		break;
		
#if B_BUILDING_ON_10_3_OR_LATER
	case kControlContentCGImageRef:
		idealSize.width		= CGImageGetWidth(mInfo.u.imageRef);
		idealSize.height	= CGImageGetHeight(mInfo.u.imageRef);
		break;
#endif
	}
	
	return (idealSize);
}

// ------------------------------------------------------------------------------------------
bool
ImageContent::CanDrawIntoGrafPort(HIViewImageContentType inContentType)
{
	bool	canDraw;
	
	switch (inContentType)
	{
	case kControlNoContent:
	case kControlContentIconSuiteRes:
	case kControlContentCIconRes:
	case kControlContentPictRes:
	case kControlContentICONRes:
	case kControlContentIconSuiteHandle:
	case kControlContentCIconHandle:
	case kControlContentPictHandle:
	case kControlContentIconRef:
	case kControlContentICON:
		canDraw = true;
		break;
		
	default:
		canDraw = false;
		break;
	}
	
	return (canDraw);
}

// ------------------------------------------------------------------------------------------
bool
ImageContent::CanDrawIntoContext(HIViewImageContentType inContentType)
{
	bool	canDraw;
	
	switch (inContentType)
	{
	case kControlNoContent:
	case kControlContentIconRef:
#if B_BUILDING_ON_10_3_OR_LATER
	case kControlContentCGImageRef:
#endif
		canDraw = true;
		break;
		
	default:
		canDraw = false;
		break;
	}
	
	return (canDraw);
}

// ------------------------------------------------------------------------------------------
void
ImageContent::Draw(
	const Rect&			inRect,
	IconAlignmentType	inAlignment,
	IconTransformType	inTransform) const
{
	::Rect		qdRect	= inRect;
	OSStatus	err;
	
	switch (mInfo.contentType)
	{
	case kControlNoContent:
		// There's nothing to do.
		break;
		
	case kControlContentIconSuiteRes:
	case kControlContentIconSuiteHandle:
		err = PlotIconSuite(&qdRect, inAlignment, inTransform, mData.iconSuite);
		B_THROW_IF_STATUS(err);
		break;
		
	case kControlContentCIconRes:
	case kControlContentCIconHandle:
		err = PlotCIconHandle(&qdRect, inAlignment, inTransform, mData.cIconHandle);
		B_THROW_IF_STATUS(err);
		break;
		
	case kControlContentPictRes:
	case kControlContentPictHandle:
		DrawPicture(mData.picture, &qdRect);
		break;
		
	case kControlContentICONRes:
	case kControlContentICON:
		PlotIcon(&qdRect, mData.ICONHandle);
		break;
		
	case kControlContentIconRef:
		err = PlotIconRef(&qdRect, inAlignment, inTransform, 
						  kIconServicesNormalUsageFlag, mInfo.u.iconRef);
		B_THROW_IF_STATUS(err);
		break;
	}
}

// ------------------------------------------------------------------------------------------
void
ImageContent::Draw(
	CGContextRef		inContext,
	const Rect&			inRect,
	IconAlignmentType	inAlignment,
	IconTransformType	inTransform) const
{
	::Rect		qdRect	= inRect;
	OSStatus	err;
	
	switch (mInfo.contentType)
	{
	case kControlNoContent:
		// There's nothing to do.
		break;
		
	case kControlContentIconRef:
		{
			RGBColor	labelColor  = { 0, 0, 0 };
			
			CGContextSaveGState(inContext);
		    CGContextTranslateCTM(inContext, 0, inRect.miny() + inRect.maxy());
		    CGContextScaleCTM(inContext, 1.0f, -1.0f);
		    
			err = PlotIconRefInContext(inContext, &inRect, inAlignment, inTransform, 
									   &labelColor, kPlotIconRefNormalFlags, 
									   mInfo.u.iconRef);
			
			CGContextRestoreGState(inContext);
			B_THROW_IF_STATUS(err);
		}
		break;
		
#if B_BUILDING_ON_10_3_OR_LATER
	case kControlContentCGImageRef:
		err = HIViewDrawCGImage(inContext, &inRect, mInfo.u.imageRef);
		B_THROW_IF_STATUS(err);
		break;
#endif
	}
}

// ------------------------------------------------------------------------------------------
HIViewID	MakeViewID(SInt32 id)
{
	HIViewID	viewID	= { B::ViewFactory::GetDefaultSignature(), id };
	
	return viewID;
}

}	// namespace ViewUtils

}	// namespace B
