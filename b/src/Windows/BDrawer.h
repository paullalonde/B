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

#ifndef BDrawer_H_
#define BDrawer_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BWindow.h"


namespace B {

/*!
    @brief  Wrapper class for drawers
    
    @todo   %Document this class!
*/
class Drawer : public Window
{
public:
    
    //! @name Constructor / Destructor
    //@{
    //! Constructor.
            Drawer(
                WindowRef       inWindowRef,
                B::AEObjectPtr  inContainer,
                DescType        inClassID = cWindow);
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns the drawer's parent window.
    WindowRef   GetParent() const;
    //! Returns the drawer's preferred parent window edge.
    OptionBits  GetPreferredEdge() const;
    //! Returns the drawer's current parent window edge.
    OptionBits  GetEdge() const;
    //! Returns the drawer's offsets from the window's content area.
    void        GetOffsets(float& outLeadingOffset, float& outTrailingOffset) const;
    //! Returns whether the drawer is fully open.
    bool        IsOpen() const;
    //@}
    
    //! @name Modifiers
    //@{
    //! Sets the drawer's parent window.
    void    SetParent(WindowRef inParent);
    //! Sets the drawer's preferred parent window edge.
    void    SetPreferredEdge(OptionBits inEdge);
    //! Sets the drawer's offsets from the window's content area.
    void    SetOffsets(float inLeadingOffset, float inTrailingOffset);
    //@}
    
    //! @name Opening & Closing
    //@{
    //! Opens the drawer.
    void    Open();
    //! Closes the drawer.
    void    Close();
    //! Toggles the drawer (opens if closed, else closes).
    void    Toggle();
    //@}
    
protected:
    
    //! @name Overridables
    //@{
    //! Called when the drawer is about to open.
    virtual bool    DrawerOpening();
    //! Called when the drawer is completely open.
    virtual void    DrawerOpened();
    //! Called when the drawer is about to close.
    virtual bool    DrawerClosing();
    //! Called when the drawer is completely closed.
    virtual void    DrawerClosed();
    //@}
    
private:
    
    void    InitEventHandler();
    
    // Carbon %Event handlers
    bool    WindowDrawerOpening(
                Event<kEventClassWindow, kEventWindowDrawerOpening>&    event);
    bool    WindowDrawerOpened(
                Event<kEventClassWindow, kEventWindowDrawerOpened>&     event);
    bool    WindowDrawerClosing(
                Event<kEventClassWindow, kEventWindowDrawerClosing>&    event);
    bool    WindowDrawerClosed(
                Event<kEventClassWindow, kEventWindowDrawerClosed>&     event);
    
    // member variables
    EventHandler    mEventHandler;
};

// ------------------------------------------------------------------------------------------
inline WindowRef
Drawer::GetParent() const
{
    return (GetDrawerParent(GetWindowRef()));
}

// ------------------------------------------------------------------------------------------
inline OptionBits
Drawer::GetPreferredEdge() const
{
    return (GetDrawerPreferredEdge(GetWindowRef()));
}

// ------------------------------------------------------------------------------------------
/*! If the drawer is currently visible, the function returns the edge on which it is 
    displayed;  else, it returns the edge on which it would be displayed.
*/
inline OptionBits
Drawer::GetEdge() const
{
    return (GetDrawerCurrentEdge(GetWindowRef()));
}

// ------------------------------------------------------------------------------------------
inline bool
Drawer::IsOpen() const
{
    return (GetDrawerState(GetWindowRef()) == kWindowDrawerOpen);
}

}   // namespace B


#endif  // BDrawer_H_
