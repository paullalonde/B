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

#ifndef BContextualMenu_H_
#define BContextualMenu_H_

#pragma once

// B headers
#include "BMenu.h"


namespace B {


/*! @brief  Helper class implementing a contextual menu.
    
    @todo   %Document this class!
*/
class ContextualMenu : public Menu
{
public:

    // constructor  
                ContextualMenu(MenuRef inMenuRef);
    
protected:
    
    // Carbon %Event handlers
    virtual void    HandleEnableItems();
    
protected:
    
    //! Returns the first menu item after the help item, if any.
    MenuItemIndex   GetFirstItem();
    void            ShowHideCommands(MenuItemIndex firstItem);
    void            HideLeadingSeparators(MenuItemIndex firstItem);
    void            HideTrailingSeparators(MenuItemIndex firstItem);
    void            HideAdjacentSeparators(MenuItemIndex firstItem);
    
    void            ShowItem(MenuItemIndex item);
    void            HideItem(MenuItemIndex item);
    
private:
    
    void    InitEventHandler();
    
    // Carbon %Event handlers
    bool    MenuEnableItems(
                Event<kEventClassMenu, kEventMenuEnableItems>&      event);
    
    // member variables
    EventHandler    mEventHandler;
};


}   // namespace B


#endif  // BContextualMenu_H_
