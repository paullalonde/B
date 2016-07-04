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

// file header
#include "BDrag.h"

// B headers
#include "BAEDescriptor.h"
#include "BRect.h"
#include "BViewUtils.h"


namespace {

static DragRef	MakeDragRef(PasteboardRef inPasteboardRef)
{
	DragRef		dragRef;
	OSStatus	err;
	
	if (inPasteboardRef != NULL)
	{
		err = NewDragWithPasteboard(inPasteboardRef, &dragRef);
	}
	else
	{
		err = NewDrag(&dragRef);
	}
	
	B_THROW_IF_STATUS(err);
	
	return (dragRef);
}

static PasteboardRef	GetPasteboardRef(DragRef inDragRef)
{
	PasteboardRef	pbref;
	OSStatus		err;
	
	err = GetDragPasteboard(inDragRef, &pbref);
	B_THROW_IF_STATUS(err);
	
	return (pbref);
}

inline size_t	ComputeBestBytesPerRow(size_t bpr)
{
	const size_t	kBestByteAlignment	= 16;
	
	return ((bpr + kBestByteAlignment - 1) & ~(kBestByteAlignment - 1));
}

}

namespace B {

// ------------------------------------------------------------------------------------------
Drag::Drag()
	: mDragRef(MakeDragRef(NULL)), mDragOwner(true), mPasteboard(GetPasteboardRef(mDragRef))
{
}

// ------------------------------------------------------------------------------------------
Drag::Drag(DragRef inDragRef)
	: mDragRef(inDragRef), mDragOwner(false), mPasteboard(GetPasteboardRef(mDragRef))
{
}

// ------------------------------------------------------------------------------------------
Drag::Drag(PasteboardRef inPasteboardRef)
	: mDragRef(MakeDragRef(inPasteboardRef)), mDragOwner(true), mPasteboard(inPasteboardRef)
{
}

// ------------------------------------------------------------------------------------------
Drag::~Drag()
{
	if (mDragOwner)
		DisposeDrag(mDragRef);
}

// ------------------------------------------------------------------------------------------
Point
Drag::GetOrigin(HIViewRef inView) const
{
	::Point		qdPt;
	OSStatus	err;
	
	err = GetDragOrigin(mDragRef, &qdPt);
	B_THROW_IF_STATUS(err);
	
	Point	pt(qdPt);
	
	if (inView != NULL)
		pt = ViewUtils::ConvertFromGlobal(pt, inView);
	
	return (pt);
}

// ------------------------------------------------------------------------------------------
Point
Drag::GetMouse(HIViewRef inView) const
{
	::Point		qdPt;
	OSStatus	err;
	
	err = GetDragMouse(mDragRef, &qdPt, NULL);
	B_THROW_IF_STATUS(err);
	
	Point	pt(qdPt);
	
	if (inView != NULL)
		pt = ViewUtils::ConvertFromGlobal(pt, inView);
	
	return (pt);
}

// ------------------------------------------------------------------------------------------
Point
Drag::GetPinnedMouse(HIViewRef inView) const
{
	::Point		qdPt;
	OSStatus	err;
	
	err = GetDragMouse(mDragRef, NULL, &qdPt);
	B_THROW_IF_STATUS(err);
	
	Point	pt(qdPt);
	
	if (inView != NULL)
		pt = ViewUtils::ConvertFromGlobal(pt, inView);
	
	return (pt);
}

// ------------------------------------------------------------------------------------------
void
Drag::SetPinnedMouse(const Point& inPt, HIViewRef inView) const
{
	Point	pt(inPt);
	
	if (inView != NULL)
		pt = ViewUtils::ConvertToGlobal(pt, inView);
	
	::Point		qdPt	= pt;
	OSStatus	err;
	
	err = SetDragMouse(mDragRef, qdPt);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
DragActions
Drag::GetDropAction() const
{
	DragActions	action;
	OSStatus	err;
	
	err = GetDragDropAction(mDragRef, &action);
	B_THROW_IF_STATUS(err);
	
	return (action);
}

// ------------------------------------------------------------------------------------------
void
Drag::SetDropAction(DragActions inAction) const
{
	OSStatus	err;
	
	err = ::SetDragDropAction(mDragRef, inAction);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
DragActions
Drag::GetAllowableActions() const
{
	DragActions	actions;
	OSStatus	err;
	
	err = GetDragAllowableActions(mDragRef, &actions);
	B_THROW_IF_STATUS(err);
	
	return (actions);
}

// ------------------------------------------------------------------------------------------
DragAttributes
Drag::GetAttributes() const
{
	DragAttributes	attributes;
	OSStatus		err;
	
	err = GetDragAttributes(mDragRef, &attributes);
	B_THROW_IF_STATUS(err);
	
	return (attributes);
}

// ------------------------------------------------------------------------------------------
UInt32
Drag::GetModifiers() const
{
	SInt16		modifiers;
	OSStatus	err;
	
	err = GetDragModifiers(mDragRef, &modifiers, NULL, NULL);
	B_THROW_IF_STATUS(err);
	
	return static_cast<UInt16>(modifiers);
}

// ------------------------------------------------------------------------------------------
UInt32
Drag::GetMouseDownModifiers() const
{
	SInt16		modifiers;
	OSStatus	err;
	
	err = GetDragModifiers(mDragRef, NULL, &modifiers, NULL);
	B_THROW_IF_STATUS(err);
	
	return static_cast<UInt16>(modifiers);
}

// ------------------------------------------------------------------------------------------
UInt32
Drag::GetMouseUpModifiers() const
{
	SInt16		modifiers;
	OSStatus	err;
	
	err = GetDragModifiers(mDragRef, NULL, NULL, &modifiers);
	B_THROW_IF_STATUS(err);
	
	return static_cast<UInt16>(modifiers);
}

// ------------------------------------------------------------------------------------------
void
Drag::GetModifiers(
	UInt32&	outModifiers,
	UInt32&	outMouseDownModifiers,
	UInt32&	outMouseUpModifiers) const
{
	SInt16		modifiers, mouseDownModifiers, mouseUpModifiers;
	OSStatus	err;
	
	err = GetDragModifiers(mDragRef, &modifiers, &mouseDownModifiers, &mouseUpModifiers);
	B_THROW_IF_STATUS(err);
	
	outModifiers			= static_cast<UInt16>(modifiers);
	outMouseDownModifiers	= static_cast<UInt16>(mouseDownModifiers);
	outMouseUpModifiers		= static_cast<UInt16>(mouseUpModifiers);
}

// ------------------------------------------------------------------------------------------
void
Drag::GetDropLocation(AEDescriptor& outDesc) const
{
	OSStatus	err;
	
	err = ::GetDropLocation(mDragRef, outDesc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drag::SetDropLocation(const AEDesc& inDesc) const
{
	OSStatus	err;
	
	err = ::SetDropLocation(mDragRef, &inDesc);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
StandardDropLocation
Drag::GetStandardDropLocation() const
{
	StandardDropLocation	location;
	OSStatus				err;
	
	err = ::GetStandardDropLocation(mDragRef, &location);
	B_THROW_IF_STATUS(err);
	
	return (location);
}

// ------------------------------------------------------------------------------------------
void
Drag::SetStandardDropLocation(StandardDropLocation inLocation) const
{
	OSStatus	err;
	
	err = ::SetStandardDropLocation(mDragRef, inLocation);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drag::SetImage(
	const OSPtr<CGImageRef>&	inImage,
	const Point&				inMouseOffset,
	DragImageFlags				inFlags)
{
	OSPtr<CGImageRef>	dragImage;
	OSStatus			err;
	
	if (CGImageGetAlphaInfo(inImage) == kCGImageAlphaNone)
	{
		// The drag manager doesn't like drag images that don't have any alpha, 
		// so create a copy of the image that has alpha.
		
		const size_t	kBitsPerComponent	= 8;
		
		B::OSPtr<CGColorSpaceRef>	colorSpace(CGColorSpaceCreateDeviceRGB(), B::from_copy);
		size_t						width		= CGImageGetWidth(inImage);
		size_t						height		= CGImageGetHeight(inImage);
		size_t						bytesPerRow;
		
		bytesPerRow	 = width * kBitsPerComponent;
		bytesPerRow	*= CGColorSpaceGetNumberOfComponents(colorSpace) + 1;
		bytesPerRow	/= 8;
		bytesPerRow	 = ComputeBestBytesPerRow(bytesPerRow);
		
		B::OSPtr<CGContextRef>	bitmap;
		void*					data	= NULL;
		
		try
		{
			data = calloc(height, bytesPerRow);
			B_THROW_IF_NULL(data);
			
			bitmap.reset(CGBitmapContextCreate(data, width, height, kBitsPerComponent, 
											   bytesPerRow, colorSpace, 
											   kCGImageAlphaPremultipliedLast), 
						 B::from_copy);
		}
		catch (...)
		{
			if (data != NULL)
				free(data);
			
			throw;
		}
		
		B::Rect	bitmapRect(0.0f, 0.0f, width, height);
		
		CGContextClearRect(bitmap, bitmapRect);
		CGContextDrawImage(bitmap, bitmapRect, inImage);
		
		dragImage.reset(CGBitmapContextCreateImage(bitmap), B::from_copy);
	}
	else
	{
		dragImage = inImage;
	}
	
	err = SetDragImageWithCGImage(mDragRef, dragImage, &inMouseOffset, inFlags);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Rect
Drag::GetItemBounds(
	const PasteboardItem&	inItem,
	HIViewRef				inView) const
{
	::Rect		qdBounds;
	OSStatus	err;
	
	err = GetDragItemBounds(mDragRef, MakeDragItemRef(inItem.GetItemID()), &qdBounds);
	B_THROW_IF_STATUS(err);
	
	Rect	bounds(qdBounds);
	
	if (inView != NULL)
		bounds = ViewUtils::ConvertFromGlobal(bounds, inView);
	
	return bounds;
}

// ------------------------------------------------------------------------------------------
void
Drag::SetItemBounds(
	PasteboardItem&				ioItem, 
	const CGRect&				inBounds, 
	HIViewRef					inView)
{
	Rect	bounds(inBounds);
	
	if (inView != NULL)
		bounds = ViewUtils::ConvertToGlobal(bounds, inView);
	
	::Rect		qdBounds	= bounds;
	OSStatus	err;
	
	err = SetDragItemBounds(mDragRef, MakeDragItemRef(ioItem.GetItemID()), &qdBounds);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Drag::SetAllowableActions(DragActions inLocalActions, DragActions inRemoteActions)
{
	OSStatus	err;
	
	err = SetDragAllowableActions(mDragRef, inLocalActions, true);
	B_THROW_IF_STATUS(err);
	
	err = SetDragAllowableActions(mDragRef, inRemoteActions, false);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
PasteboardItemID
Drag::MakePasteboardItemID(DragItemRef itemRef)
{
	return (reinterpret_cast<PasteboardItemID>(itemRef));
}

// ------------------------------------------------------------------------------------------
DragItemRef
Drag::MakeDragItemRef(PasteboardItemID itemID)
{
	return (reinterpret_cast<DragItemRef>(itemID));
}


}	// namespace B
