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
#include "BIcon.h"

// B headers
#include "BMutableString.h"
#include "BRect.h"
#include "BUrl.h"


namespace B {


// ------------------------------------------------------------------------------------------
/*!	An Icon initialised in this way isn't really useable, since there's no 
	underlying @c IconRef.  It's allowed mainly to permit instances of Icon 
	to be placed in containers, and other situations which require a default 
	constructor.
*/
Icon::Icon()
	: mRef(NULL), mLabel(0)
{
}

// ------------------------------------------------------------------------------------------
Icon::Icon(
	const Icon&	inIcon)		//!< The source icon.
		: mRef(NULL)
{
	Assign(inIcon);
}

// ------------------------------------------------------------------------------------------
Icon::Icon(
	IconRef inIconRef,			//!< A pre-existing icon.
	bool	inFromCreateOrCopy)	//!< Are we acquiring ownership of the @a inIconRef?
		: mRef(NULL), mLabel(0)
{
	Assign(inIconRef, inFromCreateOrCopy);
}

// ------------------------------------------------------------------------------------------
/*!	Use this constructor when you need the icon of an existing file or 
	folder.
*/
Icon::Icon(
	const Url&	inFileUrl)		//!< The location of the file or folder.
		: mRef(NULL)
{
	Assign(inFileUrl);
}

// ------------------------------------------------------------------------------------------
/*!	Use this constructor when you need the icon of an existing file or 
	folder.
*/
Icon::Icon(
	const FSRef&	inFileRef)		//!< The location of the file or folder.
		: mRef(NULL)
{
	Assign(inFileRef);
}

// ------------------------------------------------------------------------------------------
/*!	Use this constructor when you need a system icon.
*/
Icon::Icon(
	OSType	inIconType)		//!< The type for the icon.
		: mRef(NULL)
{
	Assign(kSystemIconsCreator, inIconType, kOnSystemDisk);
}

// ------------------------------------------------------------------------------------------
/*!	Use this constructor when you need an icon that is already registered.
*/
Icon::Icon(
	OSType	inCreator,							//!< The creator code for the icon.
	OSType	inIconType,							//!< The type for the icon.
	SInt16	inVRefNum /* = kOnSystemDisk */)	//!< Hint on where to look for the icon.
		: mRef(NULL)
{
	Assign(inCreator, inIconType, inVRefNum);
}

// ------------------------------------------------------------------------------------------
/*!	Use this constructor when you need an icon that is already registered.
*/
Icon::Icon(
	OSType		inCreator,		//!< The creator for the icon;  may be @c 0.
	OSType		inFileType,		//!< The filetype for the icon;  may be @c 0.
	CFStringRef	inExtension,	//!< The extension for the icon;  may be @c NULL.
	CFStringRef	inMIMEType)		//!< The MIME type for the icon;  may be @c NULL.
		: mRef(NULL)
{
	Assign(inCreator, inFileType, inExtension, inMIMEType);
}

// ------------------------------------------------------------------------------------------
Icon::~Icon()
{
	if (mRef != NULL)
	{
		// It's really bad to throw from a destructor, so just ignore 
		// any error.
		OSStatus	err;
		
		err = ReleaseIconRef(mRef);
	}
}

// ------------------------------------------------------------------------------------------
Icon&
Icon::Assign(const Icon& inIcon)
{
	if (inIcon.mRef != NULL)
	{
		OSStatus	err;
		
		err = AcquireIconRef(inIcon.mRef);
		B_THROW_IF_STATUS(err);
	}
	
	if (mRef != NULL)
	{
		OSStatus	err;
		
		err = ReleaseIconRef(mRef);
		B_THROW_IF_STATUS(err);
	}
	
	mRef	= inIcon.mRef;
	mLabel	= inIcon.mLabel;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Icon&
Icon::Assign(
	IconRef inIconRef,			//!< A pre-existing icon.
	bool	inFromCreateOrCopy)	//!< Are we acquiring ownership of the @a inIconRef?
{
	B_ASSERT(inIconRef != NULL);
	
	if (!inFromCreateOrCopy)
	{
		OSStatus	err;
		
		err = AcquireIconRef(inIconRef);
		B_THROW_IF_STATUS(err);
	}
	
	if (mRef != NULL)
	{
		OSStatus	err;
		
		err = ReleaseIconRef(mRef);
		B_THROW_IF_STATUS(err);
	}
	
	mRef	= inIconRef;
	mLabel	= 0;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Icon&
Icon::Assign(
	const Url&	inFileUrl)		//!< The location of the file or folder.
{
	FSRef	fileRef;
	
	inFileUrl.Copy(fileRef);
	
	return Assign(fileRef);
}

// ------------------------------------------------------------------------------------------
Icon&
Icon::Assign(
	const FSRef& inFileRef)		//!< The location of the file or folder.
{
	IconRef		iconRef;
	SInt16		iconLabel;
	OSStatus	err;
	
	err = GetIconRefFromFileInfo(&inFileRef, 0, NULL, kFSCatInfoNone, NULL, 
								 kIconServicesNormalUsageFlag, &iconRef, 
								 &iconLabel);
	B_THROW_IF_STATUS(err);
	
	if (mRef != NULL)
	{
		err = ReleaseIconRef(mRef);
		B_THROW_IF_STATUS(err);
	}
	
	mRef	= iconRef;
	mLabel	= iconLabel;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Icon&
Icon::Assign(
	OSType	inCreator,							//!< The creator code for the icon.
	OSType	inIconType,							//!< The type for the icon.
	SInt16	inVRefNum /* = kOnSystemDisk */)	//!< Hint on where to look for the icon.
{
	IconRef		iconRef;
	OSStatus	err;
	
	err = GetIconRef(inVRefNum, inCreator, inIconType, &iconRef);
	B_THROW_IF_STATUS(err);
	
	if (mRef != NULL)
	{
		err = ReleaseIconRef(mRef);
		B_THROW_IF_STATUS(err);
	}
	
	mRef	= iconRef;
	mLabel	= 0;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Icon&
Icon::Assign(
	OSType		inCreator,		//!< The creator for the icon;  may be @c 0.
	OSType		inFileType,		//!< The filetype for the icon;  may be @c 0.
	CFStringRef	inExtension,	//!< The extension for the icon;  may be @c NULL.
	CFStringRef	inMIMEType)		//!< The MIME type for the icon;  may be @c NULL.
{
	IconRef		iconRef;
	OSStatus	err;
	
	err = GetIconRefFromTypeInfo(inCreator, inFileType, inExtension, inMIMEType, 
								 kIconServicesNormalUsageFlag, &iconRef);
	B_THROW_IF_STATUS(err);

	if (mRef != NULL)
	{
		err = ReleaseIconRef(mRef);
		B_THROW_IF_STATUS(err);
	}
	
	mRef	= iconRef;
	mLabel	= 0;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Icon::Draw(
	const Rect&			inRect,								//!< The bounds for drawing.
	SInt16				inLabel /* = 0 */,					//!< The icon's label.
	IconAlignmentType	inAlignment /* = kAlignNone */,		//!< The icon's alignment within @a inRect.
	IconTransformType	inTransform /* = kTransformNone */)	//!< The icon's transform.
	const
{
	B_ASSERT(mRef != NULL);
	
	::Rect		qdRect	= inRect;
	OSStatus	err;
	
	inTransform &= ~kTransformLabel7;
	inTransform |= ((inLabel & kTransformLabel7) << 8);
	
	err = PlotIconRef(&qdRect, inAlignment, inTransform, 
					  kPlotIconRefNormalFlags, mRef);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	This function inverts the Y-axis prior to drawing.  It's suitable for use 
	in HIViews, which assume a Quickdraw-like orientation of the Y axis.
*/
void
Icon::Draw(
	CGContextRef		inContext,							//!< The context to draw in.
	const Rect&			inRect,								//!< The bounds for drawing.
	SInt16				inLabel /* = 0 */,					//!< The icon's label.
	IconAlignmentType	inAlignment /* = kAlignNone */,		//!< The icon's alignment within @a inRect.
	IconTransformType	inTransform /* = kTransformNone */)	//!< The icon's transform.
	const
{
	CGContextSaveGState(inContext);
	CGContextTranslateCTM(inContext, 0, inRect.miny() + inRect.maxy());
	CGContextScaleCTM(inContext, 1.0f, -1.0f);
	
	DrawFlipped(inContext, inRect, inLabel, inAlignment, inTransform);
	
	CGContextRestoreGState(inContext);
}

// ------------------------------------------------------------------------------------------
/*!	This function draws the icon using the current transformation matrix of 
	@a inContext.  This usually means that the icon will draw upside down, 
	because CoreGraphics' Y-axis goes up instead of down like Quickdraw.
*/
void
Icon::DrawFlipped(
	CGContextRef		inContext,							//!< The context to draw in.
	const Rect&			inRect,								//!< The bounds for drawing.
	SInt16				inLabel /* = 0 */,					//!< The icon's label.
	IconAlignmentType	inAlignment /* = kAlignNone */,		//!< The icon's alignment within @a inRect.
	IconTransformType	inTransform /* = kTransformNone */)	//!< The icon's transform.
	const
{
	B_ASSERT(mRef != NULL);
	
	RGBColor	labelColor	= { 0, 0, 0 };
	OSStatus	err;
	
	if (inLabel != 0)
	{
		Str255	junk;
		
		err = ::GetLabel(inLabel, &labelColor, junk);
		B_THROW_IF_STATUS(err);
	}
	
	inTransform &= ~kTransformLabel7;
	
	err = PlotIconRefInContext(inContext, &inRect, inAlignment, inTransform, 
							   &labelColor, kPlotIconRefNormalFlags, mRef);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	The test is made by assuming the icon is drawn at the location given in 
	@a inRect, with the alignment given in @a inAlignment.
*/
bool
Icon::Contains(
	const Point&		inTestPoint,					//!< The point to test.
	const Rect&			inRect,							//!< The bounds for drawing.
	IconAlignmentType	inAlignment /* = kAlignNone */)	//!< The icon's alignment within @a inRect.
	const
{
	B_ASSERT(mRef != NULL);
	
	::Point	testPt		= inTestPoint;
	::Rect	iconRect	= inRect;
	
	return (PtInIconRef(&testPt, &iconRect, inAlignment, 
						kIconServicesNormalUsageFlag, mRef));
}

// ------------------------------------------------------------------------------------------
/*!	The test is made by assuming the icon is drawn at the location given in 
	@a inRect, with the alignment given in @a inAlignment.
*/
bool
Icon::Intersects(
	const Rect&			inTestRect,						//!< The rect to test.
	const Rect&			inRect,							//!< The bounds for drawing.
	IconAlignmentType	inAlignment /* = kAlignNone */)	//!< The icon's alignment within @a inRect.
	const
{
	B_ASSERT(mRef != NULL);
	
	::Rect	testRect	= inTestRect;
	::Rect	iconRect	= inRect;
	
	return (RectInIconRef(&testRect, &iconRect, inAlignment, 
						  kIconServicesNormalUsageFlag, mRef));
}

// ------------------------------------------------------------------------------------------
/*!	The conversion is made by assuming the icon is drawn at the location 
	given in @a inRect, with the alignment given in @a inAlignment.
*/
AutoRegion
Icon::GetRegion(
	const Rect&			inRect,							//!< The bounds for drawing.
	IconAlignmentType	inAlignment /* = kAlignNone */)	//!< The icon's alignment within @a inRect.
	const
{
	B_ASSERT(mRef != NULL);
	
	::Rect		iconRect	= inRect;
	AutoRegion	autoRgn;
	OSStatus	err;
	
	err = IconRefToRgn(autoRgn, &iconRect, inAlignment, 
					   kIconServicesNormalUsageFlag, mRef);
	B_THROW_IF_STATUS(err);
	
	return (autoRgn);
}

// ------------------------------------------------------------------------------------------
IconFamilyHandle
Icon::GetIconFamily(
	IconSelectorValue	inWhichIcons)
	const
{
	B_ASSERT(mRef != NULL);
	
	IconFamilyHandle	iconFamily;
	OSStatus			err;
	
	err = IconRefToIconFamily(mRef, inWhichIcons, &iconFamily);
	B_THROW_IF_STATUS(err);
	
	return (iconFamily);
}

// ------------------------------------------------------------------------------------------
Icon
Icon::Register(
	OSType		inCreator,		//!< The creator code for the icon.
	OSType		inIconType,		//!< The type for the icon.
	const Url&	inIcnsFileUrl)	//!< The location of the .icns file.
{
	FSRef		icnsRef;
	IconRef		iconRef;
	OSStatus	err;
	
	inIcnsFileUrl.Copy(icnsRef);
	
	err = RegisterIconRefFromFSRef(inCreator, inIconType, &icnsRef, &iconRef);
	B_THROW_IF_STATUS(err);
	
	return (Icon(iconRef, true));
}

// ------------------------------------------------------------------------------------------
Icon
Icon::Register(
	OSType				inCreator,		//!< The creator code for the icon.
	OSType				inIconType,		//!< The type for the icon.
	IconFamilyHandle	inIconFamily)	//!< The icon data.
{
	IconRef		iconRef;
	OSStatus	err;
	
	err = RegisterIconRefFromIconFamily(inCreator, inIconType, inIconFamily, 
										&iconRef);
	B_THROW_IF_STATUS(err);
	
	return (Icon(iconRef, true));
}

// ------------------------------------------------------------------------------------------
#if 0
Icon
Icon::Register(
	OSType		inCreator,			//!< The creator code for the icon.
	OSType		inIconType,			//!< The type for the icon.
	const Url&	inResourceFileUrl,	//!< The location of the resource file.
	SInt16		inResourceID)		//!< The 'icns' resource's ID.
{
	FSRef		resourceRef;
	FSSpec		resourceSpec;
	IconRef		iconRef;
	OSStatus	err;
	
	inResourceFileUrl.Copy(resourceRef);
	
	err = FSRefMakeFSSpec(&resourceRef, &resourceSpec);
	B_THROW_IF_STATUS(err);
	
	err = RegisterIconRefFromResource(inCreator, inIconType, &resourceSpec, 
									  inResourceID, &iconRef);
	B_THROW_IF_STATUS(err);
	
	return (Icon(iconRef, true));
}
#endif

// ------------------------------------------------------------------------------------------
void
Icon::Unregister(
	OSType		inCreator,		//!< The creator code for the icon.
	OSType		inIconType)		//!< The type for the icon.
{
	OSStatus	err;
	
	err = UnregisterIconRef(inCreator, inIconType);
	B_THROW_IF_STATUS(err);
}

}	// namespace B
