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
#include "CFUtils.h"

// system headers
#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFDate.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFURL.h>


// ------------------------------------------------------------------------------------------

// CoreFoundation

template <> CFTypeID	CFUGetType<CFArrayRef>()				{ return (CFArrayGetTypeID()); }
template <> CFTypeID	CFUGetType<CFBagRef>()					{ return (CFBagGetTypeID()); }
template <> CFTypeID	CFUGetType<CFBinaryHeapRef>()			{ return (CFBinaryHeapGetTypeID()); }
template <> CFTypeID	CFUGetType<CFBitVectorRef>()			{ return (CFBitVectorGetTypeID()); }
template <> CFTypeID	CFUGetType<CFBooleanRef>()				{ return (CFBooleanGetTypeID()); }
template <> CFTypeID	CFUGetType<CFBundleRef>()				{ return (CFBundleGetTypeID()); }
template <> CFTypeID	CFUGetType<CFCharacterSetRef>()			{ return (CFCharacterSetGetTypeID()); }
template <> CFTypeID	CFUGetType<CFDataRef>()					{ return (CFDataGetTypeID()); }
template <> CFTypeID	CFUGetType<CFDateRef>()					{ return (CFDateGetTypeID()); }
template <> CFTypeID	CFUGetType<CFDictionaryRef>()			{ return (CFDictionaryGetTypeID()); }
template <> CFTypeID	CFUGetType<CFLocaleRef>()				{ return (CFLocaleGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMachPortRef>()				{ return (CFMachPortGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMessagePortRef>()			{ return (CFMessagePortGetTypeID()); }
template <> CFTypeID	CFUGetType<CFNotificationCenterRef>()	{ return (CFNotificationCenterGetTypeID()); }
template <> CFTypeID	CFUGetType<CFNullRef>()					{ return (CFNullGetTypeID()); }
template <> CFTypeID	CFUGetType<CFNumberRef>()				{ return (CFNumberGetTypeID()); }
template <> CFTypeID	CFUGetType<CFReadStreamRef>()			{ return (CFReadStreamGetTypeID()); }
template <> CFTypeID	CFUGetType<CFRunLoopRef>()				{ return (CFRunLoopGetTypeID()); }
template <> CFTypeID	CFUGetType<CFRunLoopObserverRef>()		{ return (CFRunLoopObserverGetTypeID()); }
template <> CFTypeID	CFUGetType<CFRunLoopSourceRef>()		{ return (CFRunLoopSourceGetTypeID()); }
template <> CFTypeID	CFUGetType<CFRunLoopTimerRef>()			{ return (CFRunLoopTimerGetTypeID()); }
template <> CFTypeID	CFUGetType<CFSetRef>()					{ return (CFSetGetTypeID()); }
template <> CFTypeID	CFUGetType<CFSocketRef>()				{ return (CFSocketGetTypeID()); }
template <> CFTypeID	CFUGetType<CFStringRef>()				{ return (CFStringGetTypeID()); }
template <> CFTypeID	CFUGetType<CFTimeZoneRef>()				{ return (CFTimeZoneGetTypeID()); }
template <> CFTypeID	CFUGetType<CFTreeRef>()					{ return (CFTreeGetTypeID()); }
template <> CFTypeID	CFUGetType<CFURLRef>()					{ return (CFURLGetTypeID()); }
template <> CFTypeID	CFUGetType<CFUserNotificationRef>()		{ return (CFUserNotificationGetTypeID()); }
template <> CFTypeID	CFUGetType<CFUUIDRef>()					{ return (CFUUIDGetTypeID()); }
template <> CFTypeID	CFUGetType<CFWriteStreamRef>()			{ return (CFWriteStreamGetTypeID()); }
template <> CFTypeID	CFUGetType<CFXMLNodeRef>()				{ return (CFXMLNodeGetTypeID()); }
template <> CFTypeID	CFUGetType<CFXMLParserRef>()			{ return (CFXMLParserGetTypeID()); }

template <> CFTypeID	CFUGetType<CFMutableArrayRef>()			{ return (CFArrayGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableBagRef>()			{ return (CFBagGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableBitVectorRef>()		{ return (CFBitVectorGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableCharacterSetRef>()	{ return (CFCharacterSetGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableDataRef>()			{ return (CFDataGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableDictionaryRef>()	{ return (CFDictionaryGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableSetRef>()			{ return (CFSetGetTypeID()); }
template <> CFTypeID	CFUGetType<CFMutableStringRef>()		{ return (CFStringGetTypeID()); }

// CoreServices/CoreGraphics

template <> CFTypeID	CFUGetType<CGColorRef>()				{ return (CGColorGetTypeID()); }
template <> CFTypeID	CFUGetType<CGColorSpaceRef>()			{ return (CGColorSpaceGetTypeID()); }
template <> CFTypeID	CFUGetType<CGContextRef>()				{ return (CGContextGetTypeID()); }
template <> CFTypeID	CFUGetType<CGDataConsumerRef>()			{ return (CGDataConsumerGetTypeID()); }
template <> CFTypeID	CFUGetType<CGDataProviderRef>()			{ return (CGDataProviderGetTypeID()); }
template <> CFTypeID	CFUGetType<CGFontRef>()					{ return (CGFontGetTypeID()); }
template <> CFTypeID	CFUGetType<CGFunctionRef>()				{ return (CGFunctionGetTypeID()); }
template <> CFTypeID	CFUGetType<CGImageRef>()				{ return (CGImageGetTypeID()); }
template <> CFTypeID	CFUGetType<CGPathRef>()					{ return (CGPathGetTypeID()); }
template <> CFTypeID	CFUGetType<CGPatternRef>()				{ return (CGPatternGetTypeID()); }
template <> CFTypeID	CFUGetType<CGPDFDocumentRef>()			{ return (CGPDFDocumentGetTypeID()); }
#if B_BUILDING_CAN_USE_10_3_APIS
template <> CFTypeID	CFUGetType<CGPDFPageRef>()				{ return (CGPDFPageGetTypeID()); }
template <> CFTypeID	CFUGetType<CGPSConverterRef>()			{ return (CGPSConverterGetTypeID()); }
#endif
template <> CFTypeID	CFUGetType<CGShadingRef>()				{ return (CGShadingGetTypeID()); }

template <> CFTypeID	CFUGetType<CGMutablePathRef>()			{ return (CGPathGetTypeID()); }

// CoreServices/HIServices

template <> CFTypeID	CFUGetType<AXUIElementRef>()			{ return (AXUIElementGetTypeID()); }
template <> CFTypeID	CFUGetType<AXObserverRef>()				{ return (AXObserverGetTypeID()); }
#if B_BUILDING_CAN_USE_10_3_APIS
template <> CFTypeID	CFUGetType<AXValueRef>()				{ return (AXValueGetTypeID()); }
template <> CFTypeID	CFUGetType<PasteboardRef>()				{ return (PasteboardGetTypeID()); }
template <> CFTypeID	CFUGetType<TranslationRef>()			{ return (TranslationGetTypeID()); }
#endif

// Carbon/HIToolbox

template <> CFTypeID	CFUGetType<HIShapeRef>()				{ return (HIShapeGetTypeID()); }
template <> CFTypeID	CFUGetType<HIMutableShapeRef>()			{ return (HIShapeGetTypeID()); }


// ------------------------------------------------------------------------------------------
template <> bool
CFUGet(CFDictionaryRef coll, CFStringRef key, B::OSPtr<CFTypeRef>& valT)
{
	bool		good	= false;
	CFTypeRef	value;
	
	value = CFDictionaryGetValue(coll, key);
	
	if (value != NULL)
	{
		valT.reset(value);
		good = true;
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
template <> bool
CFUGet(CFArrayRef coll, CFIndex key, B::OSPtr<CFTypeRef>& valT)
{
	bool		good	= false;
	CFTypeRef	value;
	
	value = CFArrayGetValueAtIndex(coll, key);
	
	if (value != NULL)
	{
		valT.reset(value);
		good = true;
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
template <> bool
TypeRefToNumber(CFTypeRef typeRef, bool& value)
{
	bool	good	= false;
	
	value	= false;
	
	if ((typeRef != NULL) && (CFGetTypeID(typeRef) == CFUGetType<CFBooleanRef>()))
	{
		value	= CFBooleanGetValue(reinterpret_cast<CFBooleanRef>(typeRef));
		good	= true;
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFDataRef>	BlobToData(const std::vector<UInt8>& blob)
{
	return (B::OSPtr<CFDataRef>(CFDataCreate(NULL, &blob[0], blob.size()), B::from_copy));
}

// ------------------------------------------------------------------------------------------
void	DataToBlob(B::OSPtr<CFDataRef> data, std::vector<UInt8>& blob)
{
	blob.resize(CFDataGetLength(data));
	
	BlockMoveData(CFDataGetBytePtr(data), &blob[0], blob.size());
}
