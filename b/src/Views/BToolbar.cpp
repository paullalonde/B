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
#include "BToolbar.h"

// B headers
#include "BBundle.h"
#include "BEvent.h"
#include "BEventCustomParams.h"
#include "BEventParams.h"
#include "BException.h"
#include "BNib.h"
#include "BToolbarItem.h"
#include "BUrl.h"


namespace B {


const CFStringRef	Toolbar::kHIObjectClassID	= CFSTR("ca.paullalonde.b.toolbar.delegate");

// ------------------------------------------------------------------------------------------
EventTarget*
Toolbar::Instantiate(HIObjectRef inObjectRef)
{
	return new Toolbar(inObjectRef);
}

// ------------------------------------------------------------------------------------------
Toolbar::Toolbar(HIObjectRef inObjectRef)
	: EventTarget(inObjectRef), 
	  mEventHandler(inObjectRef), mToolbarRef(NULL)
{
	InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
Toolbar::InitEventHandler()
{
	mEventHandler.Add(this, &Toolbar::ToolbarGetDefaultIdentifiers);
	mEventHandler.Add(this, &Toolbar::ToolbarGetAllowedIdentifiers);
	mEventHandler.Add(this, &Toolbar::ToolbarGetSelectableIdentifiers);
	mEventHandler.Add(this, &Toolbar::ToolbarCreateItemWithIdentifier);
	mEventHandler.Add(this, &Toolbar::ToolbarCreateItemFromDrag);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
void
Toolbar::Initialize(EventRef inEvent)
{
	OSPtr<CFTypeRef>	typeRef;
	Nib*				nib	= NULL;
	
	typeRef = EventParam<kEventParamToolbarBundle>::Get(inEvent);
	mBundle.Assign(reinterpret_cast<CFBundleRef>(const_cast<void*>(typeRef.get())));
	
	mPListName = EventParam<kEventParamToolbarPList>::Get(inEvent);
	
	if (EventParam<kEventParamToolbarNib>::Get(inEvent, nib, std::nothrow))
		mNib = boost::intrusive_ptr<Nib>(nib);
	
	OSPtr<CFDictionaryRef>	propertyList	= ReadPropertyList();
	OSPtr<HIToolbarRef>		toolbar			= MakeToolbarFromPropertyList(propertyList);
	OSStatus				err;
	
	mToolbarRef = toolbar;
	
	// Install an event handler on the HIToolbarRef that will decrease our retain count 
	// when the HIToolbarRef is destroyed.  This ensures that our lifetime is tied to 
	// the HIToolbarRef's.
	
	mToolbarEventHandler.reset(new EventHandler(mToolbarRef));
	mToolbarEventHandler->Add(this, &Toolbar::ObjectDestruct);
	mToolbarEventHandler->Init();
	
	err = HIToolbarSetDelegate(mToolbarRef, GetObjectRef());
	B_THROW_IF_STATUS(err);
	
	// Artificially increase our retain count... it will be decreased when the 
	// HIToolbarRef is destroyed.
	CFRetain(GetObjectRef());
	
	ReadToolbarItems(propertyList);

	EventParam<kEventParamToolbar>::Set(inEvent, toolbar);
	
	// Placing an HIToolbarRef in a Carbon Event doesn't increase its retain count.
	// So increase it artificially here, and decrease it once the event result is 
	// read.
	CFRetain(mToolbarRef);
}

// ------------------------------------------------------------------------------------------
void
Toolbar::SetupInitializationEvent(
	EventRef				inEvent,
	const Bundle&			inBundle, 
	CFStringRef				inPListName,
	Nib*					inNib)
{
	EventParam<kEventParamToolbarBundle>::Set(inEvent, OSPtr<CFTypeRef>(inBundle.cf_ref()));
	EventParam<kEventParamToolbarPList>::Set(inEvent, String(inPListName));
	EventParam<kEventParamToolbarNib>::Set(inEvent, inNib);
}

// ------------------------------------------------------------------------------------------
OSPtr<HIToolbarRef>
Toolbar::ReadToolbarFromInitializationEvent(
	EventRef				inEvent)
{
	OSPtr<HIToolbarRef>	toolbar	= EventParam<kEventParamToolbar>::Get(inEvent);
	
	// Bring the HIToolbarRef's retain count back down to normal (it was bumped up in 
	// Initialize().
	
	CFRelease(toolbar);
	
	return toolbar;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDictionaryRef>
Toolbar::ReadPropertyList()
{
	Url					plistUrl	= mBundle.Resource(mPListName, String("plist"));
	CFStringRef			errorRef	= NULL;
	CFDataRef			dataRef;
	SInt32				errorCode;
	
	if (!CFURLCreateDataAndPropertiesFromResource(NULL, plistUrl.cf_ref(), &dataRef, NULL, 
												  NULL, &errorCode))
	{
		B_THROW_STATUS(errorCode);
	}
	
	OSPtr<CFDataRef>	dataPtr(dataRef, from_copy);
	CFTypeRef			typeRef;
	
	typeRef = CFPropertyListCreateFromXMLData(NULL, dataPtr, 
											  kCFPropertyListImmutable, 
											  &errorRef);
	
	if (typeRef == NULL)
	{
		OSPtr<CFStringRef>	errorStr(errorRef, from_copy);
		
		B_THROW(PropertyListCreateException(errorStr));
	}
	
	OSPtr<CFTypeRef>	typePtr(typeRef, from_copy);
	
	return CFUMakeTypedValue<CFDictionaryRef>(typeRef);
}

// ------------------------------------------------------------------------------------------
OSPtr<HIToolbarRef>
Toolbar::MakeToolbarFromPropertyList(
	OSPtr<CFDictionaryRef>	inPropertyList)
{
	OSPtr<CFStringRef>	identifier	= CFUGet<CFStringRef>(inPropertyList, CFSTR("Identifier"));
	SInt32				attributes;
	HIToolbarRef		toolbarRef;
	OSStatus			err;
	
	if (!CFUGetNumber(inPropertyList, CFSTR("Attributes"), attributes))
		attributes = 0;
	
	err = HIToolbarCreate(identifier, attributes, &toolbarRef);
	B_THROW_IF_STATUS(err);
	
	return OSPtr<HIToolbarRef>(toolbarRef, from_copy);
}

// ------------------------------------------------------------------------------------------
void
Toolbar::ReadToolbarItems(
	OSPtr<CFDictionaryRef>	inPropertyList)
{
	if (!CFUGet<CFArrayRef>(inPropertyList, CFSTR("DefaultIdentifiers"), mDefaultIdentifiers))
		mDefaultIdentifiers.reset();
	
	if (!CFUGet<CFArrayRef>(inPropertyList, CFSTR("AllowedIdentifiers"), mAllowedIdentifiers))
		mAllowedIdentifiers.reset();
	
	if (!CFUGet<CFArrayRef>(inPropertyList, CFSTR("SelectableIdentifiers"), mSelectableIdentifiers))
		mSelectableIdentifiers.reset();
	
	if (!CFUGet<CFDictionaryRef>(inPropertyList, CFSTR("CustomItems"), mCustomItems))
		mCustomItems.reset();
}

// ------------------------------------------------------------------------------------------
OSPtr<CFArrayRef>
Toolbar::GetDefaultIdentifiers()
{
	return mDefaultIdentifiers;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFArrayRef>
Toolbar::GetAllowedIdentifiers()
{
	return mAllowedIdentifiers;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFArrayRef>
Toolbar::GetSelectableIdentifiers()
{
	return mSelectableIdentifiers;
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::CreateItemWithIdentifier(
	const String&				inIdentifier,
	CFTypeRef					inConfigData,
	OSPtr<HIToolbarItemRef>&	outToolbarItem)
{
	CFDictionaryRef	itemDict	= NULL;
	
	if (!CFDictionaryGetValueIfPresent(mCustomItems, inIdentifier.cf_ref(), 
									  reinterpret_cast<const void **>(&itemDict)))
	{
		return false;
	}
	
	OSPtr<CFStringRef>	classID;
	SInt32				attributes;
	
	if (!CFUGet<CFStringRef>(itemDict, CFSTR("ClassID"), classID))
		classID.reset(ToolbarItem::kHIObjectClassID);
	
	if (!CFUGetNumber(itemDict, CFSTR("Attributes"), attributes))
		attributes = 0;
	
	Event<kEventClassHIObject, kEventHIObjectInitialize>	event;
	
	FillToolbarItemInitializationEvent(event, inIdentifier, attributes, 
									   itemDict, inConfigData);
	
	outToolbarItem = EventTarget::Create(classID, event);
	
	return true;
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::CreateItemFromDrag(
	DragRef						/* inDragRef */,
	OSPtr<HIToolbarItemRef>&	/* outToolbarItem */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
void
Toolbar::FillToolbarItemInitializationEvent(
	EventRef					ioEvent,
	const String&				inIdentifier,
	UInt32						inAttributes,
	CFDictionaryRef				inCustomItemData,
	CFTypeRef					inConfigData)
{
	ToolbarItem::FillInitializationEvent(ioEvent, inIdentifier, inAttributes, 
										 mBundle.cf_ref(), mPListName.cf_ref(), 
										 mNib.get(), inCustomItemData, inConfigData);
}

// ------------------------------------------------------------------------------------------
OSPtr<HIToolbarItemRef>
Toolbar::CreateToolbarItem(
	CFStringRef					inClassID,
	EventRef					inEvent)
{
	HIToolbarItemRef	toolbarItemRef;
	OSStatus			err;
	
	err = HIObjectCreate(inClassID, inEvent, &toolbarItemRef);
	B_THROW_IF_STATUS(err);
	
	return OSPtr<HIToolbarItemRef>(toolbarItemRef, from_copy);
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::ToolbarGetDefaultIdentifiers(
	Event<kEventClassToolbar, kEventToolbarGetDefaultIdentifiers>&	event)
{
	OSPtr<CFArrayRef>	identifiers	= GetDefaultIdentifiers();
	bool				good		= (identifiers.get() != NULL);
	
	if (good)
	{
		event.mIdentifiers.clear();
		CFArrayAppendArray(event.mIdentifiers.cf_ref(), identifiers, 
			CFRangeMake(0, CFArrayGetCount(identifiers)));
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::ToolbarGetAllowedIdentifiers(
	Event<kEventClassToolbar, kEventToolbarGetAllowedIdentifiers>&	event)
{
	OSPtr<CFArrayRef>	identifiers	= GetAllowedIdentifiers();
	bool				good		= (identifiers.get() != NULL);
	
	if (good)
	{
		event.mIdentifiers.clear();
		CFArrayAppendArray(event.mIdentifiers.cf_ref(), identifiers, 
			CFRangeMake(0, CFArrayGetCount(identifiers)));
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::ToolbarGetSelectableIdentifiers(
	Event<kEventClassToolbar, kEventToolbarGetSelectableIdentifiers>&	event)
{
	OSPtr<CFArrayRef>	identifiers	= GetSelectableIdentifiers();
	bool				good		= (identifiers.get() != NULL);
	
	if (good)
	{
		event.mIdentifiers.clear();
		CFArrayAppendArray(event.mIdentifiers.cf_ref(), identifiers, 
			CFRangeMake(0, CFArrayGetCount(identifiers)));
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::ToolbarCreateItemWithIdentifier(
	Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>&	event)
{
	return (CreateItemWithIdentifier(event.mItemIdentifier, event.mItemConfigData.get(), 
									 event.mToolbarItem));
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::ToolbarCreateItemFromDrag(
	Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>&	event)
{
	return (CreateItemFromDrag(event.mDragRef, event.mToolbarItem));
}

// ------------------------------------------------------------------------------------------
bool
Toolbar::ObjectDestruct(
	Event<kEventClassHIObject, kEventHIObjectDestruct>&	/* event */)
{
	CFRelease(GetObjectRef());
	
	return false;
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT EventTargetRegistrar<Toolbar>	gRegisterToolbar;
#endif


}	// namespace B
