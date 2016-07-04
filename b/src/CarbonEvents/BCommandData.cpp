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
#include "BCommandData.h"

// B headers
#include "BString.h"


namespace B {


// ------------------------------------------------------------------------------------------
/*! This applies to both menu items and controls.
*/
void
CommandData::SetText(
    const String&   inText)
{
    mData.whichData |= kMenuItemDataCFString;
    mData.cfText     = mText = inText.cf_ptr();
}

// ------------------------------------------------------------------------------------------
void
CommandData::RefreshData(
    const HICommandExtended&    inCommand) const
{
    OSStatus    err;
    
    if (inCommand.attributes & kHICommandFromMenu)
    {
        // This command belongs to a menu.
        
        if (!boost::indeterminate(mVisible))
        {
            ReadMenuItemAttributes(inCommand, mData);
            
            if (mVisible)
                mData.attr &= ~kMenuItemAttrHidden;
            else
                mData.attr |= kMenuItemAttrHidden;
        }
        
        err = SetMenuItemData(inCommand.source.menu.menuRef, 
                              inCommand.source.menu.menuItemIndex, 
                              false, &mData);
        B_THROW_IF_STATUS(err);
    }
    else if (inCommand.attributes & kHICommandFromControl)
    {
        // This command belongs to a control.
        
        ControlRef  control = inCommand.source.control;
        
        if (!boost::indeterminate(mVisible))
        {
            if (mVisible)
                ShowControl(control);
            else
                HideControl(control);
        }
        
        if (mData.whichData & kMenuItemDataEnabled)
        {
            if (mData.enabled)
                err = EnableControl(control);
            else
                err = DisableControl(control);
            
            B_THROW_IF_STATUS(err);
        }
        
        if (mData.whichData & kMenuItemDataCFString)
        {
            err = SetControlTitleWithCFString(control, mData.cfText);
            B_THROW_IF_STATUS(err);
        }
    }
}

// ------------------------------------------------------------------------------------------
void
CommandData::ReadMenuItemAttributes(
    const HICommandExtended&    inCommand,
    MenuItemDataRec&            ioData)
{
    if (!(ioData.whichData & kMenuItemDataAttributes))
    {
        OSStatus    err;
        
        err = GetMenuItemAttributes(inCommand.source.menu.menuRef, 
                                    inCommand.source.menu.menuItemIndex,
                                    &ioData.attr);
        B_THROW_IF_STATUS(err);

        ioData.whichData |= kMenuItemDataAttributes;
    }
}


}   // namespace B
