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

#ifndef BToolbarItem_H_
#define BToolbarItem_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BBundle.h"
#include "BEvent.h"
#include "BEventHandler.h"
#include "BEventParams.h"
#include "BEventTarget.h"
#include "BNib.h"
#include "BString.h"


namespace B {

// forward declarations
class	Bundle;


/*!
	@brief	Wrapper class for toolbar items.
	
	@todo		%Document this class!
	@ingroup	ViewGroup
*/
class ToolbarItem : public EventTarget
{
public:
	
	//! @name Object Creation
	//@{
	//! Creates an instance of class @a T.
	template <class T>
	static OSPtr<HIToolbarItemRef>
			Create(
				const String&		inIdentifier, 
				OptionBits			inAttributes, 
				const Bundle&		inBundle,
				const String&		inPListName,
				Nib*				inNib,
				CFTypeRef			inConfigData,
				CFDictionaryRef		inCustomData);
	//! Creates an instance of class @a inClassID.
	static OSPtr<HIToolbarItemRef>
			Create(
				const String&		inClassID, 
				const String&		inIdentifier, 
				OptionBits			inAttributes, 
				const Bundle&		inBundle,
				const String&		inPListName,
				Nib*				inNib,
				CFTypeRef			inConfigData,
				CFDictionaryRef		inCustomData);
	static void	FillInitializationEvent(
				EventRef			ioEvent,
				const String&		inIdentifier,
				OptionBits			inAttributes,
				CFBundleRef			inBundle,
				CFStringRef			inPListName,
				Nib*				inNib,
				CFDictionaryRef		inCustomItemData,
				CFTypeRef			inConfigData);
	
	//@}
	
	//! @name Instantiation
	//@{
	//! Instantiates a ToolbarItem with @a inObjectRef.
	static EventTarget*		Instantiate(HIObjectRef inObjectRef);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the toolbar item's unique identifier.
	String				GetIdentifier() const;
	//! Returns the toolbar item's attributes.
	OptionBits			GetAttributes() const;
	//! Returns the toolbar item's label.
	String				GetLabel() const;
	//! Returns the toolbar item's short help tag text.
	String				GetShortHelpText() const;
	//! Returns the toolbar item's long help tag text.
	String				GetLongHelpText() const;
	//! Returns the toolbar item's command ID.
	MenuCommand			GetCommandID() const;
	//! Returns the toolbar item's image.
	OSPtr<CGImageRef>	GetImage() const;
	//! Returns the toolbar item's toolbar.
	HIToolbarRef		GetToolbar() const;
	//! Returns @c true if the toolbar item is enabled.
	bool				IsEnabled() const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Changes the toolbar item's attributes.
	void	ChangeAttributes(
				OptionBits		inAttrsToSet,
				OptionBits		inAttrsToClear);
	//! Changes the toolbar item's label.
	void	SetLabel(const String& inLabel);
	//! Changes the toolbar item's short help tag text.
	void	SetShortHelpText(const String& inHelpText);
	//! Changes the toolbar item's long help tag text.
	void	SetLongHelpText(const String& inHelpText);
	//! Changes the toolbar item's image.
	void	SetIcon(IconRef inIcon);
	//! Changes the toolbar item's image.
	void	SetImage(OSPtr<CGImageRef> inImage);
	//! Changes the toolbar item's command ID.
	void	SetCommandID(MenuCommand inCommandID);
	//! Enables or disables the toolbar item.
	void	Enable(bool inEnabled);
	//@}
	
	//! Class ID.
	static const CFStringRef	kHIObjectClassID;
	//! Base class ID.
	static const CFStringRef	kHIObjectBaseClassID;
	
protected:
	
	//! @name Constructors / Destructor
	//@{
	//! Constructor.
			ToolbarItem(HIObjectRef inObjectRef);
	//@}
	
	virtual void	ReadCustomItemData(
						CFDictionaryRef	inCustomItemData);
	bool			GetLocalisedString(
						CFDictionaryRef	inDict, 
						CFStringRef		inKey, 
						String&			outString);
	
	//! @name Handling Events
	//@{
	//! Override to extract initialisation arguments from @a inEvent.
	virtual void	Initialize(EventRef inEvent);
	//! Override to write out configuration info for the item.
	virtual OSPtr<CFTypeRef>	GetPersistentData();
	//! Override to create a custom view for the item.
	virtual OSPtr<HIViewRef>	CreateCustomView();
	//! Override to respond to clicks on the item's view.
	virtual bool	PerformAction();
	//! Override to accept drags onto the item's view.
	virtual bool	WouldAcceptDrop(DragRef inDragRef);
	//! Override to accept drops onto the item's view.
	virtual void	AcceptDrop(DragRef inDragRef);
	//! Override to perform some processing whenever the item's selected state changes.
	virtual void	SelectedStateChanged(WindowRef inWindowRef);
	//@}
	
	const Bundle&	GetBundle() const	{ return mBundle; }
	Nib*			GetNib() const		{ return mNib.get(); }
	
private:
	
	void	InitEventHandler();
	
	// Carbon %Event handlers
	bool	ToolbarItemGetPersistentData(
				Event<kEventClassToolbarItem, kEventToolbarItemGetPersistentData>&		event);
	bool	ToolbarItemCreateCustomView(
				Event<kEventClassToolbarItem, kEventToolbarItemCreateCustomView>&		event);
	bool	ToolbarItemPerformAction(
				Event<kEventClassToolbarItem, kEventToolbarItemPerformAction>&			event);
	bool	ToolbarItemWouldAcceptDrop(
				Event<kEventClassToolbarItem, kEventToolbarItemWouldAcceptDrop>&		event);
	bool	ToolbarItemAcceptDrop(
				Event<kEventClassToolbarItem, kEventToolbarItemAcceptDrop>&				event);
	bool	ToolbarItemSelectedStateChanged(
				Event<kEventClassToolbarItem, kEventToolbarItemSelectedStateChanged>&	event);

	// member variables
	EventHandler				mEventHandler;
	const HIToolbarItemRef		mToolbarItemRef;
	OSPtr<CFDictionaryRef>		mViewInfo;
	Bundle						mBundle;
	String						mPListName;
	boost::intrusive_ptr<Nib>	mNib;
};

// ------------------------------------------------------------------------------------------
/*!
	@param	T	The C/C++ type of the toolbar item.  Must be ToolbarItem or derived from 
				ToolbarItem.
*/
template <class T>
OSPtr<HIToolbarItemRef>
ToolbarItem::Create(
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
	
	return (EventTarget::Create<T>(initEvent));
}

}	// namespace B


#endif	// BToolbarItem_H_
