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

#ifndef BFontPanel_H_
#define BFontPanel_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BEventTarget.h"


namespace B {


class FontPanel
{
public:
    
    static bool IsVisible();
    
    static void Show();
    static void Hide();
    static void Toggle();
    
    template <typename T>
    static void SetSelection(
                    T           inObj);
    template <typename T>
    static void SetSelection(
                    ATSUStyle   style, 
                    T           inObj);
    template <typename T, typename ITER>
    static void SetSelection(
                    ITER        begin,
                    ITER        end,
                    T           inObj);
    
    static void ClearSelection();
    
    static void ApplyFontSelectionToStyle(
                    const Event<kEventClassFont, kEventFontSelection>&  inEvent,
                    ATSUStyle                                           inStyle);
                    
private:
    
    static void PrivateSetSelection(
                    const ATSUStyle*    styles, 
                    size_t              count,
                    EventTargetRef      inEventTargetRef);
    
    FontPanel();
};

// ------------------------------------------------------------------------------------------
template <typename T> inline void
FontPanel::SetSelection(
    T           inObj)
{
    PrivateSetSelection(NULL, 0, EventTarget::GetEventTarget(inObj));
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
FontPanel::SetSelection(
    ATSUStyle   style, 
    T           inObj)
{
    PrivateSetSelection(&style, 1, EventTarget::GetEventTarget(inObj));
}

// ------------------------------------------------------------------------------------------
template <typename T, typename ITER> inline void
FontPanel::SetSelection(
    ITER        begin,
    ITER        end,
    T           inObj)
{
    EventTargetRef          target  = EventTarget::GetEventTarget(inObj);
    std::vector<ATSUStyle>  styles(begin, end);
    
    if (!styles.empty())
        PrivateSetSelection(&styles[0], styles.size(), target);
    else
        PrivateSetSelection(NULL, 0, target);
}

}   // namespace B


#endif  // BFontPanel_H_
