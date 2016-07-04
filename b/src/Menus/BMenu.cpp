// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

// file header
#include "BMenu.h"

// B headers
#include "BMenuItemProperty.h"
#include "BEvent.h"


namespace B {

typedef MenuItemProperty<kEventClassB, 'Menu', Menu*>   MenuObjectProperty;

// ------------------------------------------------------------------------------------------
Menu*
Menu::Instantiate(MenuRef inMenuRef)
{
    return (new Menu(inMenuRef));
}

// ------------------------------------------------------------------------------------------
Menu::Menu(MenuRef inMenuRef)
    : mMenuRef(inMenuRef), 
      mEventHandler(inMenuRef)
{
    B_ASSERT(inMenuRef != NULL);
    
    InitEventHandler();
    
    Menu*   menu    = this;
    
    MenuObjectProperty::Set(mMenuRef, 0, menu);
}

// ------------------------------------------------------------------------------------------
Menu::~Menu()
{
    B_ASSERT(mMenuRef == NULL);
}

// ------------------------------------------------------------------------------------------
void
Menu::InitEventHandler()
{
    mEventHandler.Add(this, &Menu::MenuDispose);
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
Menu*
Menu::FromMenuRef(MenuRef inMenuRef)
{
    Menu*   menu;
    
    if (!MenuObjectProperty::Get(inMenuRef, 0, menu, std::nothrow))
        menu = NULL;
    
    return (menu);
}

// ------------------------------------------------------------------------------------------
void
Menu::HandleDispose()
{
    mMenuRef = NULL;
    
    delete this;
}

// ------------------------------------------------------------------------------------------
bool
Menu::MenuDispose(
    Event<kEventClassMenu, kEventMenuDispose>&  event)
{
    if (event.mMenuRef == mMenuRef)
    {
        HandleDispose();
    }
    
    return (false);
}


}   // namespace B
