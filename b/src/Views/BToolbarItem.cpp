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
#include "BToolbarItem.h"

// B headers
#include "BBundle.h"
#include "BEvent.h"
#include "BEventCustomParams.h"
#include "BEventParams.h"
#include "BIcon.h"
#include "BImageUtilities.h"
#include "BNibUtils.h"
#include "BStringUtilities.h"
#include "BUrl.h"
#include "BWindow.h"


namespace B {

const CFStringRef	ToolbarItem::kHIObjectClassID		= CFSTR("ca.paullalonde.b.toolbaritem");
const CFStringRef	ToolbarItem::kHIObjectBaseClassID	= kHIToolbarItemClassID;

// ------------------------------------------------------------------------------------------
OSPtr<HIToolbarItemRef>
ToolbarItem::Create(
	const String&		inClassID, 
	const String&		inIdentifier, 
	OptionBits			inAttributes, 
	const Bundle&		inBundle,
	const String&		inPListName,
	Nib*				inNib,
	CFTypeRef			inConfigData,
	CFDictionaryRef		inCustomData)
{
	Event<kEventClassHIObject, kEventHIObjectInitialize>	initEvent;
	
	FillInitializationEvent(initEvent, inIdentifier, inAttributes, inBundle.cf_ref(), 
							inPListName.cf_ref(), inNib, inCustomData, inConfigData);
	
	return (EventTarget::Create(inClassID.cf_ref(), initEvent));
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::FillInitializationEvent(
	EventRef			ioEvent,
	const String&		inIdentifier,
	OptionBits			inAttributes,
	CFBundleRef			inBundle,
	CFStringRef			inPListName,
	Nib*				inNib,
	CFDictionaryRef		inCustomItemData,
	CFTypeRef			inConfigData)
{
	EventParam<kEventParamToolbarItemIdentifier>::Set(ioEvent, inIdentifier);
	EventParam<kEventParamAttributes>::Set(ioEvent, inAttributes);

	if (inBundle != NULL)
		EventParam<kEventParamToolbarBundle>::Set(ioEvent, OSPtr<CFTypeRef>(inBundle));
	
	if (inPListName != NULL)
		EventParam<kEventParamToolbarPList>::Set(ioEvent, String(inPListName));
	
	if (inNib != NULL)
		EventParam<kEventParamToolbarNib>::Set(ioEvent, inNib);
	
	if (inCustomItemData != NULL)
		EventParam<kEventParamToolbarItemData>::Set(ioEvent, OSPtr<CFDictionaryRef>(inCustomItemData));
	
	if (inConfigData != NULL)
		EventParam<kEventParamToolbarItemConfigData>::Set(ioEvent, OSPtr<CFTypeRef>(inConfigData));
}

// ------------------------------------------------------------------------------------------
EventTarget*
ToolbarItem::Instantiate(
	HIObjectRef	inObjectRef)	//!< The underlying toolbox object.
{
	return (new ToolbarItem(inObjectRef));
}

// ------------------------------------------------------------------------------------------
ToolbarItem::ToolbarItem(
	HIObjectRef	inObjectRef)	//!< The underlying toolbox object.
		: EventTarget(inObjectRef), 
		  mEventHandler(inObjectRef), mToolbarItemRef(inObjectRef)
{
	InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::InitEventHandler()
{
	mEventHandler.Add(this, &ToolbarItem::ToolbarItemGetPersistentData);
	mEventHandler.Add(this, &ToolbarItem::ToolbarItemCreateCustomView);
	mEventHandler.Add(this, &ToolbarItem::ToolbarItemPerformAction);
	mEventHandler.Add(this, &ToolbarItem::ToolbarItemWouldAcceptDrop);
	mEventHandler.Add(this, &ToolbarItem::ToolbarItemAcceptDrop);
	mEventHandler.Add(this, &ToolbarItem::ToolbarItemSelectedStateChanged);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::Initialize(EventRef inEvent)
{
	OSPtr<CFDictionaryRef>	customItemData;
	
	if (!EventParam<kEventParamToolbarItemData>::Get(inEvent, customItemData, std::nothrow))
		customItemData.reset();
	
	OSPtr<CFTypeRef>	typeRef;
	Nib*				nib	= NULL;
	
	typeRef = EventParam<kEventParamToolbarBundle>::Get(inEvent);
	mBundle.Assign(reinterpret_cast<CFBundleRef>(const_cast<void*>(typeRef.get())));
	
	mPListName = EventParam<kEventParamToolbarPList>::Get(inEvent);
	
	if (EventParam<kEventParamToolbarNib>::Get(inEvent, nib, std::nothrow))
		mNib = boost::intrusive_ptr<Nib>(nib);
	
	ReadCustomItemData(customItemData);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::ReadCustomItemData(
	CFDictionaryRef	inCustomItemData)
{
	OSPtr<CFStringRef>		tempStrPtr;
	SInt32					commandID;
	String					tempStr;
	bool					tempBool;
	OSPtr<CFDictionaryRef>	tempDict;
	
	if (CFUGet<CFStringRef>(inCustomItemData, CFSTR("Image"), tempStrPtr))
	{
		Url	imageUrl	= mBundle.Resource(String(tempStrPtr));
		
		SetImage(CreateImageFromURL(imageUrl));
	}
	else if (CFUGet<CFStringRef>(inCustomItemData, CFSTR("IconType"), tempStrPtr))
	{
		OSType	iconType	= make_ostype(tempStrPtr);
		OSType	iconCreator	= kSystemIconsCreator;
		
		if (CFUGet<CFStringRef>(inCustomItemData, CFSTR("IconCreator"), tempStrPtr))
			iconCreator = make_ostype(tempStrPtr);
		
		B::Icon	icon(iconCreator, iconType);
		
		SetIcon(icon);
	}
	
	if (GetLocalisedString(inCustomItemData, CFSTR("Label"), tempStr))
	{
		SetLabel(tempStr);
	}
	
	if (GetLocalisedString(inCustomItemData, CFSTR("ShortHelp"), tempStr))
	{
		SetShortHelpText(tempStr);
	}
	
	if (GetLocalisedString(inCustomItemData, CFSTR("LongHelp"), tempStr))
	{
		SetLongHelpText(tempStr);
	}
	
	if (CFUGet<CFStringRef>(inCustomItemData, CFSTR("Command"), tempStrPtr))
	{
		SetCommandID(make_ostype(tempStrPtr));
	}
	else if (CFUGetNumber(inCustomItemData, CFSTR("Command"), commandID))
	{
		SetCommandID(commandID);
	}
	
	if (CFUGetNumber(inCustomItemData, CFSTR("Enabled"), tempBool))
	{
		Enable(tempBool);
	}

	if (CFUGet<CFDictionaryRef>(inCustomItemData, CFSTR("View"), tempDict))
	{
		mViewInfo = tempDict;
	}
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::GetLocalisedString(
	CFDictionaryRef	inDict, 
	CFStringRef		inKey, 
	String&			outString)
{
	bool				good;
	OSPtr<CFStringRef>	tempStr;
	
	outString.clear();
	
	good = CFUGet<CFStringRef>(inDict, inKey, tempStr);
	
	if (good)
	{
		outString = mBundle.GetLocalisedString(tempStr, tempStr, mPListName.cf_ref());
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
String
ToolbarItem::GetIdentifier() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = HIToolbarItemCopyIdentifier(*this, &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
OptionBits
ToolbarItem::GetAttributes() const
{
	OptionBits	attributes;
	OSStatus	err;
	
	err = HIToolbarItemGetAttributes(*this, &attributes);
	B_THROW_IF_STATUS(err);
	
	return (attributes);
}

// ------------------------------------------------------------------------------------------
String
ToolbarItem::GetLabel() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = HIToolbarItemCopyLabel(*this, &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
String
ToolbarItem::GetShortHelpText() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = HIToolbarItemCopyHelpText(*this, &cfstr, NULL);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
String
ToolbarItem::GetLongHelpText() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = HIToolbarItemCopyHelpText(*this, NULL, &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>
ToolbarItem::GetImage() const
{
	CGImageRef	image;
	OSStatus	err;
	
	err = HIToolbarItemCopyImage(*this, &image);
	B_THROW_IF_STATUS(err);
	
	return (OSPtr<CGImageRef>(image, from_copy));
}

// ------------------------------------------------------------------------------------------
MenuCommand
ToolbarItem::GetCommandID() const
{
	return (HIToolbarItemIsEnabled(*this));
}

// ------------------------------------------------------------------------------------------
HIToolbarRef
ToolbarItem::GetToolbar() const
{
	return (HIToolbarItemGetToolbar(*this));
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::ChangeAttributes(
	OptionBits	inAttrsToSet,		//!< A bitmask of attributes to set.
	OptionBits	inAttrsToClear)		//!< A bitmask of attributes to clear.
{
	OSStatus	err;
	
	err = HIToolbarItemChangeAttributes(*this, inAttrsToSet, inAttrsToClear);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::SetLabel(
	const String& inLabel)	//!< The new label.
{
	OSStatus	err;
	
	err = HIToolbarItemSetLabel(*this, inLabel.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::SetShortHelpText(const String& inHelpText)
{
	OSStatus	err;
	
	err = HIToolbarItemSetHelpText(*this, inHelpText.cf_ref(), NULL);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::SetLongHelpText(const String& inHelpText)
{
	String		shortHelpText(GetShortHelpText());
	OSStatus	err;
	
	err = HIToolbarItemSetHelpText(*this, 
								   shortHelpText.cf_ref(), 
								   inHelpText.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::SetIcon(IconRef inIcon)
{
	OSStatus	err;
	
	err = HIToolbarItemSetIconRef(*this, inIcon);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::SetImage(OSPtr<CGImageRef> inImage)
{
	OSStatus	err;
	
	err = HIToolbarItemSetImage(*this, inImage);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::SetCommandID(MenuCommand inCommandID)
{
	OSStatus	err;
	
	err = HIToolbarItemSetCommandID(*this, inCommandID);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
ToolbarItem::Enable(bool inEnabled)
{
	OSStatus	err;
	
	err = HIToolbarItemSetEnabled(*this, inEnabled);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation returns @c NULL.
*/
OSPtr<CFTypeRef>
ToolbarItem::GetPersistentData()
{
	OSPtr<CFTypeRef>	typePtr;
	
	return (typePtr);
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation returns @c NULL.
*/
OSPtr<HIViewRef>
ToolbarItem::CreateCustomView()
{
	OSPtr<CFStringRef>	tempStr;
	std::string			windowName;
	HIViewID			viewID;
	OSPtr<HIViewRef>	view;
	OSStatus			err;
	
	if ((mViewInfo == NULL) || (mNib.get() == NULL))
		return view;
	
	if (CFUGet<CFStringRef>(mViewInfo, CFSTR("Window"), tempStr))
		windowName = make_string(tempStr, kCFStringEncodingASCII);
	else
		return view;
	
	if (CFUGet<CFStringRef>(mViewInfo, CFSTR("ViewSignature"), tempStr))
		viewID.signature = make_ostype(tempStr);
	else
		viewID.signature = B::ViewFactory::GetDefaultSignature();
	
	if (!CFUGetNumber(mViewInfo, CFSTR("ViewID"), viewID.id))
		return view;
	
	view = NibUtils::CreateViewFromNib(*mNib, windowName.c_str(), viewID);
	
	err = HIViewSetVisible(view, true);
	B_THROW_IF_STATUS(err);
	
	return view;
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation does nothing.
	
	@return	@c true, if the action was handled.
*/
bool
ToolbarItem::PerformAction()
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation does nothing.
	
	@return	@c true, if the drag is acceptable.
*/
bool
ToolbarItem::WouldAcceptDrop(DragRef /* inDragRef */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation does nothing.
*/
void
ToolbarItem::AcceptDrop(DragRef /* inDragRef */)
{
}

// ------------------------------------------------------------------------------------------
/*!	The default implementation does nothing.
*/
void
ToolbarItem::SelectedStateChanged(
	WindowRef /* inWindowRef */)	//!< The window in which in change occurred. May be @c NULL.
{
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::ToolbarItemGetPersistentData(
	Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>&		event)
{
	event.mConfigData = GetPersistentData();
	
	return (event.mConfigData != NULL);
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::ToolbarItemCreateCustomView(
	Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>&		event)
{
	event.mCustomView = CreateCustomView();
	
	return (event.mCustomView != NULL);
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::ToolbarItemPerformAction(
	Event<kEventClassToolbarItem, kEventToolbarItemPerformAction>&			/* event */)
{
	return (PerformAction());
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::ToolbarItemWouldAcceptDrop(
	Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>&		event)
{
	event.mLikesDrag = WouldAcceptDrop(event.mDragRef);
	
	return (event.mLikesDrag);
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::ToolbarItemAcceptDrop(
	Event<kEventClassToolbarItem, kEventToolbarItemAcceptDrop>&				event)
{
	AcceptDrop(event.mDragRef);
	
	return (true);
}

// ------------------------------------------------------------------------------------------
bool
ToolbarItem::ToolbarItemSelectedStateChanged(
	Event<kEventClassToolbarItem, kEventToolbarItemSelectedStateChanged>&	event)
{
	SelectedStateChanged(event.mWindowRef);
	
	return (true);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT EventTargetRegistrar<ToolbarItem>	gRegisterToolbarItem;
#endif


}	// namespace B
