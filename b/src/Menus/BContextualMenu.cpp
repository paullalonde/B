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
#include "BContextualMenu.h"

// B headers
#include "BEvent.h"
#include "BMenuItemProperty.h"


namespace B {

typedef MenuItemProperty<kEventClassB, 'MItH', Boolean>	MenuItemHiddenProperty;

// ------------------------------------------------------------------------------------------
ContextualMenu::ContextualMenu(MenuRef inMenuRef)
	: Menu(inMenuRef), 
	  mEventHandler(inMenuRef)
{
	InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::InitEventHandler()
{
	mEventHandler.Add(this, &ContextualMenu::MenuEnableItems);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::HandleEnableItems()
{
	MenuItemIndex	firstItem	= GetFirstItem();
	
	ShowHideCommands(firstItem);
	HideLeadingSeparators(firstItem);
	HideTrailingSeparators(firstItem);
	HideAdjacentSeparators(firstItem);
}


// ------------------------------------------------------------------------------------------
/*!	Determining the first "real" item in a contextual menu can be tricky, because passing 
	@c kCMHelpItemRemoveHelp will remove the help item entirely.  So we follow this 
	heuristic:  if the first menu item has a command ID, it's taken to be the first item;  
	else, the first item is the one following the first separator line.
	
	This depends on the fact that the Menu Manager doesn't attach a command ID when it 
	prepends a "help" item onto a contextual menu.
*/
MenuItemIndex
ContextualMenu::GetFirstItem()
{
	UInt16			count	= CountMenuItems(mMenuRef);
	MenuCommand		cmdID;
	MenuItemIndex	item;
	OSStatus		err;
	
	err = GetMenuItemCommandID(mMenuRef, 1, &cmdID);
	B_THROW_IF_STATUS(err);
	
	// If the first item in the menu has a command ID, assume this is the first 
	// "real" item.
	
	if (cmdID != 0)
		return (1);
	
	// The first item didn't have a command ID, so assume that the first item(s) is a 
	// help item.
	
	// First we skip all non-separator items...
	
	for (item = 1; item <= count; item++)
	{
		MenuItemAttributes	attrs;
		
		err = GetMenuItemAttributes(mMenuRef, item, &attrs);
		B_THROW_IF_STATUS(err);
		
		if (!(attrs & kMenuItemAttrSeparator))
			continue;
		
		// We have a separator, so return the following item.
		break;
	}
	
	return (item+1);
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::ShowHideCommands(MenuItemIndex firstItem)
{
	UInt16	count	= CountMenuItems(mMenuRef);
	
	MenuItemDataRec	data;
	Str255			name;
	OSStatus		err;
	
	data.whichData = kMenuItemDataAllDataVersionTwo;
	data.text = name;
	data.properties = NULL;
	err = CopyMenuItemData(mMenuRef, 1, false, &data);
	
	for (MenuItemIndex item = firstItem; item <= count; item++)
	{
		MenuItemAttributes	attrs;
		
		err = GetMenuItemAttributes(mMenuRef, item, &attrs);
		B_THROW_IF_STATUS(err);
		
		if (attrs & kMenuItemAttrSeparator)
			continue;
		else if (!(attrs & kMenuItemAttrDisabled))
			ShowItem(item);
		else
			HideItem(item);
	}
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::HideLeadingSeparators(MenuItemIndex firstItem)
{
	UInt16	count	= CountMenuItems(mMenuRef);
	
	for (MenuItemIndex item = firstItem; item <= count; item++)
	{
		MenuItemAttributes	attrs;
		OSStatus			err;
		
		err = GetMenuItemAttributes(mMenuRef, item, &attrs);
		B_THROW_IF_STATUS(err);
		
		// Skip hidden items.
		if (attrs & kMenuItemAttrHidden)
			continue;
		
		// If it's not a separator, we're done.
		if (!(attrs & kMenuItemAttrSeparator))
			break;
		
		// Else it's a separator, so hide it.
		HideItem(item);
	}
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::HideTrailingSeparators(MenuItemIndex firstItem)
{
	UInt16	count	= CountMenuItems(mMenuRef);
	
	for (MenuItemIndex item = count; item >= firstItem; item--)
	{
		MenuItemAttributes	attrs;
		OSStatus			err;
		
		err = GetMenuItemAttributes(mMenuRef, item, &attrs);
		B_THROW_IF_STATUS(err);
		
		// Skip hidden items.
		if (attrs & kMenuItemAttrHidden)
			continue;
		
		// If it's not a separator, we're done.
		if (!(attrs & kMenuItemAttrSeparator))
			break;
		
		// Else it's a separator, so hide it.
		HideItem(item);
	}
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::HideAdjacentSeparators(MenuItemIndex firstItem)
{
	UInt16	count	= CountMenuItems(mMenuRef);
	
	for (MenuItemIndex item1 = firstItem; item1 <= count-1; item1++)
	{
		// Find the first visible separator
		
		MenuItemAttributes	attrs1;
		OSStatus			err;
		
		err = GetMenuItemAttributes(mMenuRef, item1, &attrs1);
		B_THROW_IF_STATUS(err);
		
		// Skip hidden items.
		if (attrs1 & kMenuItemAttrHidden)
			continue;
		
		// Skip non-separators.
		if (!(attrs1 & kMenuItemAttrSeparator))
			continue;
		
		for (MenuItemIndex item2 = item1+1; item2 <= count; item2++)
		{
			MenuItemAttributes	attrs2;
			
			err = GetMenuItemAttributes(mMenuRef, item2, &attrs2);
			B_THROW_IF_STATUS(err);
			
			// Skip hidden items.
			if (attrs2 & kMenuItemAttrHidden)
				continue;
			
			// Break if non-separator.
			if (!(attrs2 & kMenuItemAttrSeparator))
			{
				item1 = item2;
				break;
			}
			
			// item1 and item2 are two adjacent separators (as displayed 
			// to the user.  Hide item2.
			
			HideItem(item2);
		}
	}
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::ShowItem(MenuItemIndex item)
{
	bool	hiddenByUs;
	
	if (MenuItemHiddenProperty::Get(mMenuRef, item, hiddenByUs, std::nothrow))
	{
		MenuItemHiddenProperty::Remove(mMenuRef, item);
	}
	else
	{
		hiddenByUs = false;
	}
	
	if (hiddenByUs)
	{
		OSStatus	err;
		
		err = ChangeMenuItemAttributes(mMenuRef, item, 0, 
									   kMenuItemAttrHidden);
		B_THROW_IF_STATUS(err);
	}
}

// ------------------------------------------------------------------------------------------
void
ContextualMenu::HideItem(MenuItemIndex item)
{
	OSStatus	err;
	
	MenuItemHiddenProperty::Set(mMenuRef, item, true);
	
	err = ChangeMenuItemAttributes(mMenuRef, item, kMenuItemAttrHidden, 0);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
ContextualMenu::MenuEnableItems(
	Event<kEventClassMenu, kEventMenuEnableItems>&	event)
{
	B_ASSERT(event.mMenuRef == mMenuRef);
	
	bool	handled	= event.Forward();
	
	HandleEnableItems();
	
	return (handled);
}


}	// namespace B
