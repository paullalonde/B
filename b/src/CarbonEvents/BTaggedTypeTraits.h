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

#ifndef BTaggedTypeTraits_H_
#define BTaggedTypeTraits_H_

#pragma once

// standard headers
#if !__MWERKS__
#	include <string>
#endif

// compiler headers
#if __MWERKS__
#	include <stringfwd>
#endif

// system headers
#include <CoreFoundation/CFArray.h>
#include <Carbon/Carbon.h>

// B headers
#include "BFwd.h"
#include "BOSPtr.h"


namespace B {

// forward declarations
class	AEObject;
class	Point;
class	Rect;
class	Shape;
class	String;
class	Url;


// ==========================================================================================
//	TaggedTypeTrait

/*!
	@struct	TaggedTypeTrait
	@brief	Maps an integral constant onto a C/C++ language type.
	
	TaggedTypeTrait maps an integer constant identifying a Carbon %Event 
	parameter type (@c EventParamType) or an Apple %Event descriptor type 
	(@c DescType) onto a C/C++ language type.  Both Carbon %Events and 
	AppleEvents use the same namespace for argument types, which is 
	why they both use this common traits class.
	
	The idea is that the language type corresponding to a given 
	parameter type can be determined via this expression:
	
		@code TaggedTypeTrait<TYPE>::Type @endcode
	
	For example, in the definition:
	
		@code TaggedTypeTrait<typeBoolean>::Type my_var; @endcode
	
	@p my_var is of type @p bool.
	
	Note that the generic definition of TaggedTypeTrait is deliberately 
	useless, because TaggedTypeTrait is meant to be specialised for 
	each parameter type.
*/

template <OSType TYPE> struct TaggedTypeTrait
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef OSType		TYPE;	//!< The template parameter.  Should be an integral four-char constant in the EventParamType/DescType namespace.
	//@}
	
	//! @name Types
	//@{
	typedef anything	Type;	//!< The language type.
	//@}
#endif
};


// ==========================================================================================
//	Template instantiations of TaggedTypeTrait<>

#ifndef DOXYGEN_SKIP

//template <> struct TaggedTypeTrait<type128BitFloatingPoint>	{ typedef long double (?) 			Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeAEList>				{ typedef aedesc 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeAERecord>				{ typedef aedesc 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeAEText>				{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeAbsoluteOrdinal>			{ typedef OSType	  				Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeAlias>					{ typedef AliasHandle /*var*/ 		Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeAppParameters>			{ typedef unsupported 				Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeApplSignature>			{ typedef OSType 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeAppleEvent>			{ typedef aedesc 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeApplicationURL>		{ typedef ??? 						Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeArc>					{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeCFArrayRef>				{ typedef OSPtr<CFArrayRef>			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeBest>					{ typedef OSType  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeBoolean>					{ typedef bool /*Boolean*/ 			Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeCFDictionaryRef>			{ typedef OSPtr<CFDictionaryRef>	Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFIndex>					{ typedef CFIndex					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFMutableArrayRef>		{ typedef OSPtr<CFMutableArrayRef>	Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFMutableDictionaryRef>	{ typedef OSPtr<CFMutableDictionaryRef> Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFMutableStringRef>		{ typedef OSPtr<CFMutableStringRef>	Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFRange>                 { typedef CFRange					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFStringRef>				{ typedef String					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCFTypeRef>				{ typedef OSPtr<CFTypeRef>			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCGContextRef>			{ typedef CGContextRef				Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeCString>				{ typedef char* /*var*/  			Type;	};	// AERegistry.h (deprecated)
//template <> struct TaggedTypeTrait<typeCell>					{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeCentimeters>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeChar>					{ typedef std::string /*var*/		Type;	};	// AEDataModel.h (deprecated)
//template <> struct TaggedTypeTrait<typeClassInfo>				{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeClickActivationResult>	{ typedef ClickActivationResult		Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCollection>				{ typedef Collection				Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeColorTable>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeColumn>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeCompDescriptor>		{ typedef OSL aedesc  				Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeComponentInstance>		{ typedef ComponentInstance 		Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeControlActionUPP>		{ typedef ControlActionUPP			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeControlPartCode>			{ typedef ControlPartCode			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeControlRef>				{ typedef HIViewRef					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeCubicCentimeter>			{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeCubicFeet>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeCubicInches>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeCubicMeters>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeCubicYards>				{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeCurrentContainer>		{ typedef OSL no data  				Type;	};	// AEObjects.h
//template <> struct TaggedTypeTrait<typeDashStyle>				{ typedef DashStyleRecord  			Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeData>					{ typedef OSPtr<CFDataRef>  		Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeDecimalStruct>			{ typedef unsupported 				Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeDegreesC>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeDegreesF>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeDegreesK>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeDragRef>					{ typedef DragRef					Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeDrawingArea>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeEPS>					{ typedef var blob  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeElemInfo>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeEncodedString>			{ typedef format not yet defined	Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeEnumerated>				{ typedef OSType 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeEnumeration>			{ typedef OSType  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeEventHotKeyID>			{ typedef EventHotKeyID				Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeEventInfo>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeEventRecord>			{ typedef ??? 						Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeEventRef>				{ typedef OSPtr<EventRef>			Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeEventTargetRef>			{ typedef EventTargetRef			Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeExtended>				{ typedef unsupported 				Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeFSRef>					{ typedef FSRef 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeFSS>						{ typedef FSSpec 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeFSVolumeRefNum>			{ typedef FSVolumeRefNum			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeFalse>					{ typedef void /*no data*/			Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeFeet>					{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeFileURL>					{ typedef Url /*var*/				Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeFinderWindow>			{ typedef FinderWindow  			Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeFixed>					{ typedef Fixed 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeFixedPoint>			{ typedef 0x66706e74  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeFixedRectangle>		{ typedef 0x66726374  				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeGWorldPtr>				{ typedef GWorldPtr					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeGallons>					{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeGlyphInfoArray>		{ typedef Glyph/FMFont info array for sub ranges of Unicode text.  See GlyphInfoArray in TextServices.h  						Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeGrafPtr>					{ typedef CGrafPtr					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeGrams>					{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeGraphicLine>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeGraphicText>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeGroupedGraphic>		{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeHICommand>				{ typedef HICommandExtended			Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeHIMenu>				{ typedef ???  						Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeHIObjectRef>				{ typedef OSPtr<HIObjectRef>		Type;	};	// HIObject.h
template <> struct TaggedTypeTrait<typeHIPoint>					{ typedef HIPoint					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIRect>					{ typedef HIRect					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIShapeRef>				{ typedef Shape						Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHISize>					{ typedef HISize					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIToolbarDisplayMode>	{ typedef HIToolbarDisplayMode		Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIToolbarDisplaySize>	{ typedef HIToolbarDisplaySize		Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIToolbarItemRef>		{ typedef OSPtr<HIToolbarItemRef>	Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIToolbarRef>			{ typedef OSPtr<HIToolbarRef>		Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeHIViewTrackingAreaRef>	{ typedef HIViewTrackingAreaRef     Type;	};	// HIView.h
//template <> struct TaggedTypeTrait<typeHIWindow>				{ typedef ???  						Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeIEEE32BitFloatingPoint>	{ typedef Float32 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeIEEE64BitFloatingPoint>	{ typedef Float64 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeISO8601DateTime>		{ typedef 0x69736f74  data is ascii text of an ISO8601 date 						Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeInches>					{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeIndexDescriptor>		{ typedef OSL 0x696e6465  			Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeIndicatorDragConstraint>	{ typedef IndicatorDragConstraint	Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeInsertionLoc>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeIntlText>				{ typedef script+lang+text (var)	Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeIntlWritingCode>			{ typedef WritingCode  				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeKernelProcessID>			{ typedef pid_t /*??*/ 				Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeKeyword>					{ typedef OSType 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeKilograms>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeKilometers>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeLiters>					{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLogicalDescriptor>		{ typedef OSL aedesc  				Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeLongDateTime>			{ typedef LongDateTime  			Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLongFixed>				{ typedef 0x6c667864  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLongFixedPoint>		{ typedef 0x6c667074  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLongFixedRectangle>	{ typedef 0x6c667263  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLongPoint>				{ typedef 0x6c706e74  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLongRectangle>			{ typedef 0x6c726374  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeLowLevelEventRecord>	{ typedef Low Level Event Record 	Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeMachPort>				{ typedef mach_port_t /*??*/ 		Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeMachineLoc>			{ typedef 0x6d4c6f63  				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeMenuCommand>				{ typedef MenuCommand				Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeMenuEventOptions>		{ typedef MenuEventOptions			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeMenuItemIndex>			{ typedef MenuItemIndex				Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeMenuRef>					{ typedef OSPtr<MenuRef>			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeMenuTrackingMode>		{ typedef MenuTrackingMode			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeMeters>					{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeMiles>					{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeMouseButton>				{ typedef EventMouseButton			Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeMouseTrackingRef>		{ typedef MouseTrackingRef			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeMouseWheelAxis>			{ typedef EventMouseWheelAxis		Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeNull>					{ typedef void /*no data*/	 		Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeOSLTokenList>			{ typedef OSL 0x6F73746C  			Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeOSStatus>				{ typedef OSStatus					Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeObjectBeingExamined>	{ typedef OSL no data  				Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeObjectSpecifier>			{ typedef	AEObjectPtr				Type;	};	// AEObjects.h
//template <> struct TaggedTypeTrait<typeOffsetArray>			{ typedef offset array 				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeOunces>					{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeOval>					{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typePasteboardRef>			{ typedef PasteboardRef				Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typePString>				{ typedef StringPtr /*var*/  		Type;	};	// AERegistry.h (deprecated)
//template <> struct TaggedTypeTrait<typeParamInfo>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typePict>					{ typedef var blob  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typePixMapMinus>			{ typedef PixMapHandle (var)		Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typePixelMap>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typePolygon>				{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typePounds>					{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeProcessSerialNumber>		{ typedef ProcessSerialNumber 		Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typePropInfo>				{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeProperty>				{ typedef OSType 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typePtr>						{ typedef void*		  				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeQDPoint>					{ typedef Point  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeQDRectangle>				{ typedef Rect	 					Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeQDRegion>				{ typedef var blob (unsupported?) 	Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeQDRgnHandle>				{ typedef RgnHandle					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeQuarts>					{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeRGB16>					{ typedef 15-bit direct color pixel	Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeRGB96>					{ typedef RGB96Record  				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeRGBColor>				{ typedef RGBColor  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeRangeDescriptor>		{ typedef OSL aedesc  				Type;	};	// AEObjects.h
//template <> struct TaggedTypeTrait<typeRectangle>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeRelativeDescriptor>	{ typedef OSL 0x72656c20  			Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeReplyPortAttr>			{ typedef mach_port_t 				Type;	};	// AEMach.h
//template <> struct TaggedTypeTrait<typeRotation>				{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeRoundedRectangle>		{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeRow>					{ typedef aedesc  					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeSInt16>					{ typedef SInt16 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeSInt32>					{ typedef SInt32 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeSInt64>					{ typedef SInt64 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeScrapRef>				{ typedef ScrapRef					Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeScrapStyles>			{ typedef var blob  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeScript>				{ typedef 0x73637074  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeSectionH>				{ typedef obsolete 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeSquareFeet>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeSquareKilometers>		{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeSquareMeters>			{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeSquareMiles>				{ typedef Float64 					Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeSquareYards>				{ typedef Float64 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeStyledText>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeStyledUnicodeText>		{ typedef format not yet defined  	Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeSuiteInfo>				{ typedef 0x7375696e  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeTIFF>					{ typedef var blob  				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeTable>					{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeTabletPointRec>		{ typedef kEventParamTabletPointRec	Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeTabletProximityRec>	{ typedef kEventParamTabletProximityRec	Type;};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeTextRange>				{ typedef TextRange 				Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeTextRangeArray>		{ typedef var blob 					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeTextStyles>			{ typedef aedesc  					Type;	};	// AERegistry.h
//template <> struct TaggedTypeTrait<typeThemeDrawState>		{ typedef ThemeDrawState			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeThemeMenuItemType>		{ typedef ThemeMenuItemType			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeThemeMenuState>			{ typedef ThemeMenuState			Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeToken>					{ typedef OSL 0x746f6b65  			Type;	};	// AEObjects.h
template <> struct TaggedTypeTrait<typeTrue>					{ typedef void /*no data*/ 			Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeType>					{ typedef OSType 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeUInt32>					{ typedef UInt32 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeUTF16ExternalRepresentation>{ typedef String /*var*/			Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeUTF8Text>				{ typedef String /*var*/			Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeUnicodeText>				{ typedef String /*var*/ 			Type;	};	// AEDataModel.h
//template <> struct TaggedTypeTrait<typeVersion>				{ typedef 0x76657273  				Type;	};	// AERegistry.h
template <> struct TaggedTypeTrait<typeVoidPtr>					{ typedef void*						Type;	};	// CarbonEvents.h
//template <> struct TaggedTypeTrait<typeWhoseDescriptor>		{ typedef OSL aedesc  				Type;	};	// AEObjects.h
//template <> struct TaggedTypeTrait<typeWhoseRange>			{ typedef OSL aedesc  				Type;	};	// AEObjects.h
//template <> struct TaggedTypeTrait<typeWildCard>				{ typedef (var) 					Type;	};	// AEDataModel.h
template <> struct TaggedTypeTrait<typeWindowDefPartCode>		{ typedef WindowDefPartCode			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeWindowRef>				{ typedef WindowRef					Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeWindowRegionCode>		{ typedef WindowRegionCode			Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeWindowTransitionAction>	{ typedef WindowTransitionAction	Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeWindowTransitionEffect>	{ typedef WindowTransitionEffect	Type;	};	// CarbonEvents.h
template <> struct TaggedTypeTrait<typeYards>					{ typedef Float64 					Type;	};	// AERegistry.h

#endif	// DOXYGEN_SKIP


// Types to watch out for:

#if 0
	template <> struct TaggedTypeTrait<typeCFMutableStringRef>	{ typedef String						Type;	};	// CarbonEvents.h
	// deprecated
	template <> struct TaggedTypeTrait<typeCString>				{ typedef char* /*var*/  				Type;	};	// AERegistry.h
	// deprecated
	template <> struct TaggedTypeTrait<typeChar>				{ typedef char* /*var*/					Type;	};	// AEDataModel.h
	template <> struct TaggedTypeTrait<typeKernelProcessID>		{ typedef pid_t /*??*/ 					Type;	};	// AEDataModel.h
	template <> struct TaggedTypeTrait<typeMachPort>			{ typedef mach_port_t /*??*/ 			Type;	};	// AEDataModel.h
	// deprecated
	template <> struct TaggedTypeTrait<typePString>				{ typedef StringPtr /*var*/  			Type;	};	// AERegistry.h
#endif


}	// namespace B


#endif	// BEventParams_H_
