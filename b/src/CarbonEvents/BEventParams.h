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

#ifndef BEventParams_H_
#define BEventParams_H_

#pragma once

// standard headers
#include <new>

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BErrorHandler.h"
#include "BTaggedTypeTraits.h"


namespace B {

// forward declarations
class	AEObject;

// ==========================================================================================
//	EventParamTypeTraitBase

/*!
	@brief	Helper class that serves as the common ancestor of instantiations of 
			EventParamTypeTrait.
	
	EventParamTypeTrait requires the ability to determine if a given Carbon %Event parameter 
	with a given type is present in a Carbon %Event.  However, this check doesn't actually 
	depend on the language type which the EventParamTypeTrait's template parameter maps 
	to.  So it has been factored out into this little helper class.
	
	@ingroup	CarbonEvents
*/
struct EventParamTypeTraitBase
{
	//! Determines whether a given parameter exists in a Carbon %Event.
	static bool	Exists(
					EventRef		inEvent, 
					EventParamName	inName, 
					EventParamType	inType);
};


// ==========================================================================================
//	EventParamTypeTrait

/*!
	@brief	Gets & sets Carbon %Event parameters in a type-safe manner.
	
	EventParamTypeTrait provides a type-safe wrapper around the Carbon  
	Event Manager calls @c GetEventParameter() and @c SetEventParameter().
	
	The class's type-safety is provided by the related type TaggedTypeTrait, 
	which maps an @c EventParamType onto a C/C++ language type.  Thus 
	EventParamTypeTrait augments TaggedTypeTrait with Carbon %Event 
	parameters accessors.
	
	@ingroup	CarbonEvents
*/
template <EventParamType TYPE> struct EventParamTypeTrait : public EventParamTypeTraitBase
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef EventParamType	TYPE;	//!< The template parameter.  Should be an integral four-char constant (an @c EventParamType).
	//@}
#endif
	
	//! @name Types
	//@{
	typedef typename TaggedTypeTrait<TYPE>::Type	Type;	//!< The language type.  See TaggedTypeTrait.
	//@}
	
	//! @name Getting & Setting Parameters
	//@{
	//! Retrieves a Carbon %Event parameter.
	static OSStatus	Get(
						EventRef		inEvent, 
						EventParamName	inName, 
						Type&			outValue);
	//!Changes a Carbon %Event parameter.
	static OSStatus	Set(
						EventRef		inEvent, 
						EventParamName	inName, 
						const Type&		inValue);
};

// ------------------------------------------------------------------------------------------
/*!	@return		An OS status code.
	@exception	none
*/
template <EventParamType TYPE> OSStatus
EventParamTypeTrait<TYPE>::Get(
	EventRef		inEvent,	//!< The Carbon %Event.
	EventParamName	inName,		//!< The parameter's name.
	Type&			outValue)	//!< Holds the output.
{
	return (GetEventParameter(inEvent, inName, TYPE, NULL, sizeof(outValue), NULL, &outValue));
}

// ------------------------------------------------------------------------------------------
/*!	@return		An OS status code.
	@exception	none
*/
template <EventParamType TYPE> OSStatus
EventParamTypeTrait<TYPE>::Set(
	EventRef		inEvent,	//!< The Carbon %Event.
	EventParamName	inName,		//!< The parameter's name.
	const Type&		inValue)	//!< The new value.
{
	return (SetEventParameter(inEvent, inName, TYPE, sizeof(inValue), &inValue));
}


// ==========================================================================================
//	Template instantiations of EventParamTypeTrait<TYPE>

#ifndef DOXYGEN_SKIP

/*!	@brief	EventParamTypeTrait template specialisation for @c typeBoolean.
	
	Converts to and from @c Boolean (the type actually read/written) and 
	@c bool, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeBoolean> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeBoolean>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeQDPoint.
	
	Converts to and from @c ::Point (the type actually read/written) and 
	Point, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeQDPoint> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeQDPoint>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeQDRectangle.
	
	Converts to and from @c ::Rect (the type actually read/written) and 
	Rect, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeQDRectangle> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeQDRectangle>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFArrayRef.
	
	Converts to and from @c CFArrayRef (the type actually read/written) and 
	OSPtr<CFArrayRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFArrayRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFArrayRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFDictionaryRef.
	
	Converts to and from @c CFDictionaryRef (the type actually read/written) and 
	OSPtr<CFDictionaryRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFDictionaryRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFDictionaryRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFMutableArrayRef.
	
	Converts to and from @c CFMutableArrayRef (the type actually read/written) and 
	OSPtr<CFMutableArrayRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFMutableArrayRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFMutableArrayRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFMutableDictionaryRef.
	
	Converts to and from @c CFMutableDictionaryRef (the type actually read/written) and 
	OSPtr<CFMutableDictionaryRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFMutableDictionaryRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFMutableDictionaryRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFMutableStringRef.
	
	Converts to and from @c CFMutableStringRef (the type actually read/written) and 
	OSPtr<CFMutableStringRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFMutableStringRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFMutableStringRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFStringRef.
	
	Converts to and from @c CFStringRef (the type actually read/written) and 
	String, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFStringRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFStringRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeCFTypeRef.
	
	Converts to and from @c CFTypeRef (the type actually read/written) and 
	OSPtr<CFTypeRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeCFTypeRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeCFTypeRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeHIObjectRef.
	
	Converts to and from @c HIObjectRef (the type actually read/written) and 
	OSPtr<HIObjectRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeHIObjectRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeHIObjectRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeHIToolbarRef.
	
	Converts to and from @c HIToolbarRef (the type actually read/written) and 
	OSPtr<HIToolbarRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeHIToolbarRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeHIToolbarRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeHIToolbarItemRef.
	
	Converts to and from @c HIToolbarItemRef (the type actually read/written) and 
	OSPtr<HIToolbarItemRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeHIToolbarItemRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeHIToolbarItemRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeEventRef.
	
	Converts to and from @c EventRef (the type actually read/written) and 
	OSPtr<EventRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeEventRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeEventRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeHIShapeRef.
	
	Converts to and from @c HIShapeRef (the type actually read/written) and 
	Shape, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeHIShapeRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeHIShapeRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeMenuRef.
	
	Converts to and from @c MenuRef (the type actually read/written) and 
	OSPtr<MenuRef>, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeMenuRef> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeMenuRef>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

/*!	@brief	EventParamTypeTrait template specialisation for @c typeChar.
	
	Converts to and from char[] (the type actually read/written) and 
	std::string, the C/C++ language type passed to the member functions.
*/
template <> struct EventParamTypeTrait<typeChar> : public EventParamTypeTraitBase
{
	typedef TaggedTypeTrait<typeChar>::Type	Type;
	
	static OSStatus	Get(EventRef inEvent, EventParamName inName, Type& value);
	static OSStatus	Set(EventRef inEvent, EventParamName inName, const Type& value);
};

#endif	// DOXYGEN_SKIP


// ==========================================================================================
//	EventParamNameTrait

/*!
	@brief	Maps a Carbon %Event parameter name onto its parameter type.
	
	Carbon %Event parameters often (but not always) have a predefined 
	type associated with them.  This class captures that association.
	
	Note that the generic definition of EventParamNameTrait is deliberately 
	useless, because EventParamNameTrait is meant to be specialised for 
	each parameter name.
	
	@ingroup	CarbonEvents
*/
template <EventParamName NAME> struct EventParamNameTrait
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef EventParamName NAME;	//!< The template parameter.  Should be an integral four-char constant (an @c EventParamName).
	//@}

	//! @name Enumerations
	//@{
	enum {
		kTypeTag					//!< The parameter type associated with @a NAME.  Should be an integral four-char constant (an @c EventParamType).
	};
	//@}
#endif
};


// ==========================================================================================
//	Template instantiations of EventParamNameTrait<NAME>

#ifndef DOXYGEN_SKIP

template <> struct EventParamNameTrait<kEventParamAEEventClass>							{ enum { kTypeTag = typeType };	};
template <> struct EventParamNameTrait<kEventParamAEEventID>							{ enum { kTypeTag = typeType };	};
template <> struct EventParamNameTrait<kEventParamATSUFontID>							{ enum { kTypeTag = typeATSUFontID };	};
template <> struct EventParamNameTrait<kEventParamATSUFontSize>							{ enum { kTypeTag = typeATSUSize };	};
template <> struct EventParamNameTrait<kEventParamAccessibleActionDescription>			{ enum { kTypeTag = typeCFMutableStringRef };	};
template <> struct EventParamNameTrait<kEventParamAccessibleActionName>					{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamAccessibleActionNames>				{ enum { kTypeTag = typeCFMutableArrayRef };	};
template <> struct EventParamNameTrait<kEventParamAccessibleAttributeName>				{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamAccessibleAttributeNames>				{ enum { kTypeTag = typeCFMutableArrayRef };	};
template <> struct EventParamNameTrait<kEventParamAccessibleAttributeSettable>			{ enum { kTypeTag = typeBoolean };	};
//template <> struct EventParamNameTrait<kEventParamAccessibleAttributeValue>			{ typedef /* variable*/		TypeType;	};
//template <> struct EventParamNameTrait<kEventParamAccessibleChild>					{ typedef /* typeCFTypeRef with an AXUIElementRef*/		TypeType;	};
//template <> struct EventParamNameTrait<kEventParamAccessibleObject>					{ typedef /* typeCFTypeRef with an AXUIElementRef*/		TypeType;	};
template <> struct EventParamNameTrait<kEventParamAttributes>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamAvailableBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamBounds>								{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamCandidateText>						{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamCGContextRef>							{ enum { kTypeTag = typeCGContextRef };	};
template <> struct EventParamNameTrait<kEventParamClickActivation>						{ enum { kTypeTag = typeClickActivationResult };	};
template <> struct EventParamNameTrait<kEventParamClickCount>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamComboBoxListSelectedItemIndex>		{ enum { kTypeTag = typeCFIndex };	};
template <> struct EventParamNameTrait<kEventParamControlAction>						{ enum { kTypeTag = typeControlActionUPP };	};
template <> struct EventParamNameTrait<kEventParamControlClickActivationResult>			{ enum { kTypeTag = typeClickActivationResult };	};
template <> struct EventParamNameTrait<kEventParamControlCurrentOwningWindow>			{ enum { kTypeTag = typeWindowRef };	};
template <> struct EventParamNameTrait<kEventParamControlCurrentPart>					{ enum { kTypeTag = typeControlPartCode };	};
template <> struct EventParamNameTrait<kEventParamControlDataBuffer>					{ enum { kTypeTag = typePtr };	};
template <> struct EventParamNameTrait<kEventParamControlDataBufferSize>				{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamControlDataTag>						{ enum { kTypeTag = typeEnumeration };	};
template <> struct EventParamNameTrait<kEventParamControlDrawDepth>						{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamControlDrawInColor>					{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamControlFeatures>						{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamControlFocusEverything>				{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamControlIndicatorDragConstraint>		{ enum { kTypeTag = typeIndicatorDragConstraint };	};
template <> struct EventParamNameTrait<kEventParamControlIndicatorOffset>				{ enum { kTypeTag = typeQDPoint };	};
template <> struct EventParamNameTrait<kEventParamControlIndicatorRegion>				{ enum { kTypeTag = typeQDRgnHandle };	};
template <> struct EventParamNameTrait<kEventParamControlInvalRgn>						{ enum { kTypeTag = typeQDRgnHandle };	};
template <> struct EventParamNameTrait<kEventParamControlIsGhosting>					{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamControlWouldAcceptDrop>				{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamControlMessage>						{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamControlOptimalBaselineOffset>			{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamControlOptimalBounds>					{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamControlOriginalOwningWindow>			{ enum { kTypeTag = typeWindowRef };	};
template <> struct EventParamNameTrait<kEventParamControlParam>							{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamControlPart>							{ enum { kTypeTag = typeControlPartCode };	};
template <> struct EventParamNameTrait<kEventParamControlPartBounds>					{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamControlPrefersShape>					{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamControlPreviousPart>					{ enum { kTypeTag = typeControlPartCode };	};
template <> struct EventParamNameTrait<kEventParamControlRef>							{ enum { kTypeTag = typeControlRef };	};
template <> struct EventParamNameTrait<kEventParamControlRegion>						{ enum { kTypeTag = typeQDRgnHandle };	};
template <> struct EventParamNameTrait<kEventParamControlResult>						{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamControlSubControl>					{ enum { kTypeTag = typeControlRef };	};
template <> struct EventParamNameTrait<kEventParamControlSubview>						{ enum { kTypeTag = typeControlRef };	};
template <> struct EventParamNameTrait<kEventParamControlValue>							{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamCurrentBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamCurrentDockRect>						{ enum { kTypeTag = typeHIRect };	};
template <> struct EventParamNameTrait<kEventParamCurrentMenuTrackingMode>				{ enum { kTypeTag = typeMenuTrackingMode };	};
template <> struct EventParamNameTrait<kEventParamCurrentWindow>						{ enum { kTypeTag = typeWindowRef };	};
template <> struct EventParamNameTrait<kEventParamDeviceColor>							{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamDeviceDepth>							{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamDictionary>							{ enum { kTypeTag = typeCFDictionaryRef };	};
template <> struct EventParamNameTrait<kEventParamDimensions>							{ enum { kTypeTag = typeQDPoint };	};
//template <> struct EventParamNameTrait<kEventParamDirectObject>						{ typedef /* type varies depending on event*/		TypeType;	};
template <> struct EventParamNameTrait<kEventParamDragRef>								{ enum { kTypeTag = typeDragRef };	};
template <> struct EventParamNameTrait<kEventParamEnableMenuForKeyEvent>				{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamEnabled>								{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamEventRef>								{ enum { kTypeTag = typeEventRef };	};
template <> struct EventParamNameTrait<kEventParamFMFontFamily>							{ enum { kTypeTag = typeFMFontFamily };	};
template <> struct EventParamNameTrait<kEventParamFMFontSize>							{ enum { kTypeTag = typeFMFontSize };	};
template <> struct EventParamNameTrait<kEventParamFMFontStyle>							{ enum { kTypeTag = typeFMFontStyle };	};
template <> struct EventParamNameTrait<kEventParamFontColor>							{ enum { kTypeTag = typeFontColor };	};
template <> struct EventParamNameTrait<kEventParamGrafPort>								{ enum { kTypeTag = typeGrafPtr };	};
template <> struct EventParamNameTrait<kEventParamHIArchive>							{ enum { kTypeTag = typeCFTypeRef };	};
template <> struct EventParamNameTrait<kEventParamHICommand>							{ enum { kTypeTag = typeHICommand };	};
//template <> struct EventParamNameTrait<kEventParamHIObjectInstance>					{ typedef /* ??? */		TypeType;	};
template <> struct EventParamNameTrait<kEventParamHIViewTrackingArea>					{ enum { kTypeTag = typeHIViewTrackingAreaRef };	};
template <> struct EventParamNameTrait<kEventParamImageSize>							{ enum { kTypeTag = typeHISize };	};
template <> struct EventParamNameTrait<kEventParamInitCollection>						{ enum { kTypeTag = typeCollection };	};
template <> struct EventParamNameTrait<kEventParamIsInInstantMouser>					{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamKeyCode>								{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamKeyMacCharCodes>						{ enum { kTypeTag = typeChar };	};
template <> struct EventParamNameTrait<kEventParamKeyModifiers>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamKeyUnicodes>							{ enum { kTypeTag = typeUnicodeText };	};
template <> struct EventParamNameTrait<kEventParamKeyboardType>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamLaunchErr>							{ enum { kTypeTag = typeOSStatus };	};
template <> struct EventParamNameTrait<kEventParamLaunchRefCon>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamLineSize>								{ enum { kTypeTag = typeHISize };	};
template <> struct EventParamNameTrait<kEventParamMaximumSize>							{ enum { kTypeTag = typeHISize };	};
template <> struct EventParamNameTrait<kEventParamMenuCommand>							{ enum { kTypeTag = typeMenuCommand };	};
template <> struct EventParamNameTrait<kEventParamMenuCommandKeyBounds>					{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamMenuContext>							{ enum { kTypeTag = typeUInt32 };	};
//template <> struct EventParamNameTrait<kEventParamMenuDrawState>						{ enum { kTypeTag = typeThemeDrawState };	};
template <> struct EventParamNameTrait<kEventParamMenuEventOptions>						{ enum { kTypeTag = typeMenuEventOptions };	};
template <> struct EventParamNameTrait<kEventParamMenuFirstOpen>						{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamMenuIconBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamMenuItemBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamMenuItemHeight>						{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamMenuItemIndex>						{ enum { kTypeTag = typeMenuItemIndex };	};
template <> struct EventParamNameTrait<kEventParamMenuItemType>							{ enum { kTypeTag = typeThemeMenuItemType };	};
template <> struct EventParamNameTrait<kEventParamMenuItemWidth>						{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamMenuMarkBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamMenuRef>								{ enum { kTypeTag = typeMenuRef };	};
template <> struct EventParamNameTrait<kEventParamMenuTextBaseline>						{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamMenuTextBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamMenuVirtualBottom>					{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamMenuVirtualTop>						{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamMinimumSize>							{ enum { kTypeTag = typeHISize };	};
template <> struct EventParamNameTrait<kEventParamMouseButton>							{ enum { kTypeTag = typeMouseButton };	};
template <> struct EventParamNameTrait<kEventParamMouseChord>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamMouseDelta>							{ enum { kTypeTag = typeQDPoint };	};
template <> struct EventParamNameTrait<kEventParamMouseLocation>						{ enum { kTypeTag = typeHIPoint };	};
//template <> struct EventParamNameTrait<kEventParamMouseTrackingRef>					{ enum { kTypeTag = typeMouseTrackingRef };	};
template <> struct EventParamNameTrait<kEventParamMouseWheelAxis>						{ enum { kTypeTag = typeMouseWheelAxis };	};
template <> struct EventParamNameTrait<kEventParamMouseWheelDelta>						{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamMutableArray>							{ enum { kTypeTag = typeCFMutableArrayRef };	};
template <> struct EventParamNameTrait<kEventParamNewMenuTrackingMode>					{ enum { kTypeTag = typeMenuTrackingMode };	};
template <> struct EventParamNameTrait<kEventParamNewScrollBarVariant>					{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamNextControl>							{ enum { kTypeTag = typeControlRef };	};
template <> struct EventParamNameTrait<kEventParamOrigin>								{ enum { kTypeTag = typeHIPoint };	};
template <> struct EventParamNameTrait<kEventParamOriginalBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamPasteboardRef>						{ enum { kTypeTag = typePasteboardRef };	};
//template <> struct EventParamNameTrait<kEventParamPDEHeight>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamPostTarget>							{ enum { kTypeTag = typeEventTargetRef };	};
template <> struct EventParamNameTrait<kEventParamPreviousBounds>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamPreviousDockRect>						{ enum { kTypeTag = typeHIRect };	};
template <> struct EventParamNameTrait<kEventParamPreviousWindow>						{ enum { kTypeTag = typeWindowRef };	};
template <> struct EventParamNameTrait<kEventParamProcessID>							{ enum { kTypeTag = typeProcessSerialNumber };	};
template <> struct EventParamNameTrait<kEventParamReplacementText>						{ enum { kTypeTag = typeCFStringRef };	};
//template <> struct EventParamNameTrait<kEventParamResult>								{ typedef /* any type - depends on event like direct object*/		TypeType;	};
template <> struct EventParamNameTrait<kEventParamRgnHandle>							{ enum { kTypeTag = typeQDRgnHandle };	};
template <> struct EventParamNameTrait<kEventParamScrapRef>								{ enum { kTypeTag = typeScrapRef };	};
template <> struct EventParamNameTrait<kEventParamServiceCopyTypes>						{ enum { kTypeTag = typeCFMutableArrayRef };	};
template <> struct EventParamNameTrait<kEventParamServiceMessageName>					{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamServicePasteTypes>					{ enum { kTypeTag = typeCFMutableArrayRef };	};
template <> struct EventParamNameTrait<kEventParamServiceUserData>						{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamShape>                                { enum { kTypeTag = typeHIShapeRef };	};
template <> struct EventParamNameTrait<kEventParamStartControl>							{ enum { kTypeTag = typeControlRef };	};
template <> struct EventParamNameTrait<kEventParamSystemUIMode>							{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamTabletEventType>						{ enum { kTypeTag = typeUInt32 };	};
//template <> struct EventParamNameTrait<kEventParamTabletPointRec>						{ enum { kTypeTag = typeTabletPointRec };	};
//template <> struct EventParamNameTrait<kEventParamTabletProximityRec>					{ enum { kTypeTag = typeTabletProximityRec };	};
//template <> struct EventParamNameTrait<kEventParamTextInputGlyphInfoArray>			{ enum { kTypeTag = typeGlyphInfoArray };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyFMFont>					{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyFont>					{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyLeadingEdge>			{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyLineAscent>				{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyLineHeight>				{ enum { kTypeTag = typeShortInteger };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyPoint>					{ enum { kTypeTag = typeQDPoint };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyPointSize>				{ enum { kTypeTag = typeFixed };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyRegionClass>			{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplySLRec>					{ enum { kTypeTag = typeIntlWritingCode };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyShowHide>				{ enum { kTypeTag = typeBoolean };	};
//template <> struct EventParamNameTrait<kEventParamTextInputReplyText>					{ typedef /*    typeUnicodeText (if TSMDocument is Unicode), otherwise typeChar*/		TypeType;	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyTextAngle>				{ enum { kTypeTag = typeFixed };	};
template <> struct EventParamNameTrait<kEventParamTextInputReplyTextOffset>				{ enum { kTypeTag = typeSInt32 };	};
//template <> struct EventParamNameTrait<kEventParamTextInputSendClauseRng>				{ enum { kTypeTag = typeOffsetArray };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendComponentInstance>		{ enum { kTypeTag = typeComponentInstance };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendCurrentPoint>			{ enum { kTypeTag = typeQDPoint };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendDraggingMode>			{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendFixLen>					{ enum { kTypeTag = typeSInt32 };	};
//template <> struct EventParamNameTrait<kEventParamTextInputSendHiliteRng>				{ enum { kTypeTag = typeTextRangeArray };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendKeyboardEvent>			{ enum { kTypeTag = typeEventRef };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendLeadingEdge>				{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendPinRng>					{ enum { kTypeTag = typeTextRange };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendRefCon>					{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendSLRec>					{ enum { kTypeTag = typeIntlWritingCode };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendShowHide>				{ enum { kTypeTag = typeBoolean };	};
//template <> struct EventParamNameTrait<kEventParamTextInputSendText>					{ typedef /*    typeUnicodeText (if TSMDocument is Unicode), otherwise typeChar*/		TypeType;	};
template <> struct EventParamNameTrait<kEventParamTextInputSendTextOffset>				{ enum { kTypeTag = typeSInt32 };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendTextServiceEncoding>		{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamTextInputSendTextServiceMacEncoding>	{ enum { kTypeTag = typeUInt32 };	};
//template <> struct EventParamNameTrait<kEventParamTextInputSendUpdateRng>				{ enum { kTypeTag = typeTextRangeArray };	};
template <> struct EventParamNameTrait<kEventParamTextSelection>						{ enum { kTypeTag = typeCFRange };	};
template <> struct EventParamNameTrait<kEventParamToolbar>								{ enum { kTypeTag = typeHIToolbarRef };	};
template <> struct EventParamNameTrait<kEventParamToolbarDisplayMode>					{ enum { kTypeTag = typeHIToolbarDisplayMode };	};
template <> struct EventParamNameTrait<kEventParamToolbarDisplaySize>					{ enum { kTypeTag = typeHIToolbarDisplaySize };	};
template <> struct EventParamNameTrait<kEventParamToolbarItem>							{ enum { kTypeTag = typeHIToolbarItemRef };	};
template <> struct EventParamNameTrait<kEventParamToolbarItemConfigData>				{ enum { kTypeTag = typeCFTypeRef };	};
template <> struct EventParamNameTrait<kEventParamToolbarItemIdentifier>				{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamUnconfirmedRange>						{ enum { kTypeTag = typeCFRange };	};
template <> struct EventParamNameTrait<kEventParamUnconfirmedText>						{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamUserData>								{ enum { kTypeTag = typeVoidPtr };	};
template <> struct EventParamNameTrait<kEventParamViewSize>								{ enum { kTypeTag = typeHISize };	};
template <> struct EventParamNameTrait<kEventParamWindowDefPart>						{ enum { kTypeTag = typeWindowDefPartCode };	};
template <> struct EventParamNameTrait<kEventParamWindowDragHiliteFlag>					{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamWindowFeatures>						{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamWindowGrowRect>						{ enum { kTypeTag = typeQDRectangle };	};
template <> struct EventParamNameTrait<kEventParamWindowModifiedFlag>					{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamWindowMouseLocation>					{ enum { kTypeTag = typeHIPoint };	};
template <> struct EventParamNameTrait<kEventParamWindowProxyGWorldPtr>					{ enum { kTypeTag = typeGWorldPtr };	};
template <> struct EventParamNameTrait<kEventParamWindowProxyImageRgn>					{ enum { kTypeTag = typeQDRgnHandle };	};
template <> struct EventParamNameTrait<kEventParamWindowProxyOutlineRgn>				{ enum { kTypeTag = typeQDRgnHandle };	};
template <> struct EventParamNameTrait<kEventParamWindowRef>							{ enum { kTypeTag = typeWindowRef };	};
template <> struct EventParamNameTrait<kEventParamWindowRegionCode>						{ enum { kTypeTag = typeWindowRegionCode };	};
template <> struct EventParamNameTrait<kEventParamWindowStateChangedFlags>				{ enum { kTypeTag = typeUInt32 };	};
template <> struct EventParamNameTrait<kEventParamWindowTitleFullWidth>					{ enum { kTypeTag = typeSInt16 };	};
template <> struct EventParamNameTrait<kEventParamWindowTitleTextWidth>					{ enum { kTypeTag = typeSInt16 };	};
template <> struct EventParamNameTrait<kEventParamWindowTransitionAction>				{ enum { kTypeTag = typeWindowTransitionAction };	};
template <> struct EventParamNameTrait<kEventParamWindowTransitionEffect>				{ enum { kTypeTag = typeWindowTransitionEffect };	};

#endif	// DOXYGEN_SKIP


// ==========================================================================================
//	EventParamAndType

/*!
	@brief	Gets & sets a specific Carbon %Event parameter of a specific type in a type-safe manner.
	
	EventParamAndType utilises EventParamTypeTrait<TYPE> to actually retrieve & store the data.
	See EventParamTypeTrait for more information.
	
	@ingroup	CarbonEvents
	@sa			@ref using_events
*/
template <EventParamName NAME, EventParamType TYPE> struct EventParamAndType
{
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef EventParamName		NAME;	//!< Template parameter.  Should be an integral four-char constant (an @c EventParamName).
	typedef EventParamAndType	TYPE;	//!< Template parameter.  Should be an integral four-char constant (an @c EventParamType).
	//@}
#endif
	
	//! @name Types
	//@{
	typedef EventParamTypeTrait<TYPE>	IOType;		//!< The traits for TYPE.  This is the class that actually gets & sets values.
	typedef typename IOType::Type		ValueType;	//!< The language type.
	//@}
	
	//! @name Inquiries
	//@{
	//!	Determines whether the parameter exists in a Carbon %Event.
	static bool			Exists(EventRef inEvent);
	//@}
	
	//! @name Getting & Setting Parameters
	//@{
	//!	Retrieves a Carbon %Event parameter, returning it as the function result.
	static ValueType	Get(EventRef inEvent);
	
	//!	Retrieves a Carbon %Event parameter, returning it in @a value.
	static void			Get(EventRef inEvent, ValueType& value);
	
	//!	Retrieves a Carbon %Event parameter, returning it in @a value and without throwing an exception.
	static bool			Get(EventRef inEvent, ValueType& value, const std::nothrow_t&);
	
	//!	Sets a Carbon %Event parameter.
	static void			Set(EventRef inEvent, const ValueType& inValue);
	//@}
};

// ------------------------------------------------------------------------------------------
/*!	The parameter's name and type are determined by the NAME and TYPE template 
	parameters, respectively.
	
	@return		A boolean indication of success or failure.
	@exception	none
*/
template <EventParamName NAME, EventParamType TYPE> bool
EventParamAndType<NAME, TYPE>::Exists(
	EventRef	inEvent)	//!< The Carbon %Event.
{
	return (IOType::Exists(inEvent, NAME, TYPE));
}

// ------------------------------------------------------------------------------------------
/*!	The parameter's name and type are determined by the NAME and TYPE template 
	parameters, respectively.
	
	@return		The value.
*/
template <EventParamName NAME, EventParamType TYPE>
typename EventParamAndType<NAME, TYPE>::ValueType
EventParamAndType<NAME, TYPE>::Get(
	EventRef	inEvent)	//!< The Carbon %Event.
{
	ValueType	value;
	OSStatus	err;
	
	err = IOType::Get(inEvent, NAME, value);
	B_THROW_IF_STATUS(err);
	
	return (value);
}

// ------------------------------------------------------------------------------------------
/*!	The parameter's name and type are determined by the NAME and TYPE template 
	parameters, respectively.
*/
template <EventParamName NAME, EventParamType TYPE> void
EventParamAndType<NAME, TYPE>::Get(
	EventRef	inEvent,	//!< The Carbon %Event.
	ValueType&	value)		//!< Holds the output value.
{
	OSStatus	err;
	
	err = IOType::Get(inEvent, NAME, value);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	The parameter's name and type are determined by the NAME and TYPE template 
	parameters, respectively.
	
	@return		Boolean indication of sucess or failure.
	@exception	none
*/
template <EventParamName NAME, EventParamType TYPE> bool
EventParamAndType<NAME, TYPE>::Get(
	EventRef	inEvent,	//!< The Carbon %Event.
	ValueType&	value,		//!< Holds the output value.
	const std::nothrow_t&)		//!< An indication that the caller doesn't want to have the function throw an exception.
{
	bool		good		= false;
	ValueType	oldValue	= value;
	
	good = (IOType::Get(inEvent, NAME, value) == noErr);
	
	if (!good)
		value = oldValue;
	
	return (good);
}

// ------------------------------------------------------------------------------------------
/*!	The parameter's name and type are determined by the NAME and TYPE template 
	parameters, respectively.
*/
template <EventParamName NAME, EventParamType TYPE> void
EventParamAndType<NAME, TYPE>::Set(
	EventRef			inEvent, 	//!< The Carbon %Event.
	const ValueType&	inValue)	//!< The new value.
{
	OSStatus	err;
	
	err = IOType::Set(inEvent, NAME, inValue);
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	EventParam

/*!
	@brief	Gets & sets a specific Carbon %Event parameter in a type-safe manner.
	
	EventParam utilises EventParamNameTrait to determine the parameter type.  The parameter 
	type is then used in turn to determine the language type.
	
	This class is basically for convenience.  It allows you replace:
	
		@code EventParamAndType<NAME, TYPE> @endcode
	
	with
	
		@code EventParam<NAME> @endcode
	
	See EventParamAndType and EventParamNameTrait for more information.
	
	@ingroup	CarbonEvents
	@sa			@ref using_events
*/
template <EventParamName NAME> struct EventParam 
	: public EventParamAndType<NAME, EventParamNameTrait<NAME>::kTypeTag>
{
};


// ==========================================================================================
//	EventDirectParam

/*!
	@brief	Gets & sets a Carbon %Event's direct parameter in a type-safe manner.
	
	The parameter @c kEventParamDirectObject can have different types, depending on the 
	particular event.  So this class hard-codes the parameter name and leaves its type as 
	a template parameter.
	
	This class is basically for convenience.  It allows you replace:
	
		@code EventParamAndType<kEventParamDirectObject, TYPE> @endcode
	
	with
	
		@code EventDirectParam<TYPE> @endcode
	
	See EventParamAndType for more information.
	
	@ingroup	CarbonEvents
	@sa			@ref using_events
*/
template <EventParamType TYPE> struct EventDirectParam 
	: public EventParamAndType<kEventParamDirectObject, TYPE>
{
};


// ==========================================================================================
//	EventResultParam

/*!
	@brief	Gets & sets a Carbon %Event's result parameter in a type-safe manner.
	
	The parameter @c kEventParamResult can have different types, depending on the 
	particular event.  So this class hard-codes the parameter name and leaves its type as 
	a template parameter.
	
	This class is basically for convenience.  It allows you replace:
	
		@code EventParamAndType<kEventParamResult, TYPE> @endcode
	
	with
	
		@code EventResultParam<TYPE> @endcode
	
	See EventParamAndType for more information.
	
	@ingroup	CarbonEvents
	@sa			@ref using_events
*/
template <EventParamType TYPE> struct EventResultParam 
	: public EventParamAndType<kEventParamResult, TYPE>
{
};


}	// namespace B


#endif	// BEventParams_H_
