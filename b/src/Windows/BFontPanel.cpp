// ==========================================================================================
//  
//  Copyright (C) 2006 Paul Lalonde enrg.
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
#include "BFontPanel.h"

// B headers
#include "BErrorHandler.h"
#include "BEvent.h"


namespace B {

// ------------------------------------------------------------------------------------------
bool
FontPanel::IsVisible()
{
    return FPIsFontPanelVisible();
}

// ------------------------------------------------------------------------------------------
void
FontPanel::Show()
{
    if (!IsVisible())
        Toggle();
}

// ------------------------------------------------------------------------------------------
void
FontPanel::Hide()
{
    if (IsVisible())
        Toggle();
}

// ------------------------------------------------------------------------------------------
void
FontPanel::Toggle()
{
    OSStatus    err = FPShowHideFontPanel();
    
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
FontPanel::ClearSelection()
{
    OSStatus    err;
    
    err = SetFontInfoForSelection(kFontSelectionATSUIType, 0, NULL, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
FontPanel::PrivateSetSelection(
    const ATSUStyle*    styles, 
    size_t              count,
    EventTargetRef      inEventTargetRef)
{
    OSStatus    err;
    
    err = SetFontInfoForSelection(kFontSelectionATSUIType, count, 
                                  const_cast<ATSUStyle*>(styles), inEventTargetRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
FontPanel::ApplyFontSelectionToStyle(
    const Event<kEventClassFont, kEventFontSelection>&  inEvent,
    ATSUStyle                                           inStyle)
{
    std::vector<ATSUAttributeTag>       tags;
    std::vector<ByteCount>              sizes;
    std::vector<ATSUAttributeValuePtr>  values;
    
    if (inEvent.mFontIDPtr != NULL)
    {
        tags.push_back(kATSUFontTag);
        sizes.push_back(sizeof(*inEvent.mFontIDPtr));
        values.push_back(inEvent.mFontIDPtr);
    }
    
    if (inEvent.mFontSizePtr != NULL)
    {
        tags.push_back(kATSUSizeTag);
        sizes.push_back(sizeof(*inEvent.mFontSizePtr));
        values.push_back(inEvent.mFontSizePtr);
    }
    
    if (!tags.empty())
    {
        OSStatus    err;
        
        err = ATSUSetAttributes(inStyle, tags.size(), &tags[0], &sizes[0], &values[0]);
        B_THROW_IF_STATUS(err);
    }
}

}   // namespace B
