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

#ifndef BCommandData_H_
#define BCommandData_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include "BOSPtr.h"


namespace B {


// forward declarations
class   String;


/*!
    @brief  Acts as an accumulator for menu item or control state changes.
    
    Instances of this class are passed to Carbon %Event handlers for the 
    @c kEventClassCommand/kEventCommandUpdateStatus event.  The handlers should call 
    appropriate member functions to indicate a menu item's enabled status, text, 
    mark character, etc.  When the handler returns, the menu item is updated to 
    reflect the new state.
    
    @ingroup    CarbonEvents
*/
class CommandData
{
public:
    
    //! @name Constructor
    //@{
    //! Constructor.
    CommandData();
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns @c true if the object has any state that needs updating.
    bool    HasData() const;
    //@}
    
    //! @name Modifiers
    //@{
    //! Changes the command's enabled state.
    void    SetEnabled(bool inEnabled);
    //! Changes the command's text.
    void    SetText(const String& inText);
    //! Change the command's mark character.
    void    SetMark(UniChar inMark);
    //! Change the command's mark character to the standard check mark.
    void    SetChecked(bool checked);
    //! Change the command's visible state.
    void    SetVisible(bool visible);
    //@}
    
    //! @name Refreshing Toolbox Data Structures
    //@{
    //! Updates the toolbox entity identified by @a inCommand.
    void    RefreshData(const HICommandExtended& inCommand) const;
    //@}
    
private:
    
    static void ReadMenuItemAttributes(
                    const HICommandExtended&    inCommand,
                    MenuItemDataRec&            ioData);
    
    // member variables
    mutable MenuItemDataRec mData;          //!< Describes current state of the command.
    OSPtr<CFStringRef>      mText;          //!< Cached command text, if any.
    boost::tribool          mVisible;
};

// ------------------------------------------------------------------------------------------
/*! The object start out without any state that needs updating.
*/
inline
CommandData::CommandData()
    : mVisible(boost::indeterminate)
{
    mData.whichData = 0;
}

// ------------------------------------------------------------------------------------------
inline bool
CommandData::HasData() const
{
    return (mData.whichData != 0);
}

// ------------------------------------------------------------------------------------------
/*! This applies to both menu items and controls.
*/
inline void
CommandData::SetEnabled(
    bool        inEnabled)  //!< The command's new enabled status.
{
    mData.whichData |= kMenuItemDataEnabled;
    mData.enabled   = inEnabled;
}

// ------------------------------------------------------------------------------------------
/*! This currently only applies to menu items.
*/
inline void
CommandData::SetMark(
    UniChar     inMark) //!< The command's new mark character.
{
    mData.whichData |= kMenuItemDataMark;
    mData.mark      = inMark;
}

// ------------------------------------------------------------------------------------------
/*! This currently only applies to menu items.
*/
inline void
CommandData::SetChecked(bool checked)
{
    mData.whichData |= kMenuItemDataMark;
    mData.mark      = checked ? kMenuCheckmarkGlyph : kMenuNullGlyph;
}

// ------------------------------------------------------------------------------------------
/*! This currently only applies to menu items.
*/
inline void
CommandData::SetVisible(bool visible)
{
    mVisible = visible;
}

}   // namespace B


#endif  // BCommandData_H_
