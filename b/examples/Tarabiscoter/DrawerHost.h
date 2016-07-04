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

#ifndef DrawerHost_H_
#define DrawerHost_H_

#pragma once

// B headers
#include "BDrawer.h"


class DrawerHost : public B::Window
{
public:

    // constructor
    DrawerHost(
        WindowRef       inWindowRef,
        B::AEObjectPtr  inContainer);
    
protected:
    
    // overrides from B::Window
    virtual void    Awaken(B::Nib* inFromNib);
    
    // CarbonEvent handlers
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);

private:
    
    // types
    typedef B::Window   inherited;
    
    boost::shared_ptr<B::Drawer>    InitHorizontalDrawer(
                                        B::Nib&     inNib,
                                        OptionBits  inEdge);
    boost::shared_ptr<B::Drawer>    InitVerticalDrawer(
                                        B::Nib&     inNib,
                                        OptionBits  inEdge);
    
    // member variables
    boost::shared_ptr<B::Drawer>  mLeftDrawer;
    boost::shared_ptr<B::Drawer>  mRightDrawer;
    boost::shared_ptr<B::Drawer>  mBottomDrawer;
};


#endif // DrawerHost_H_
