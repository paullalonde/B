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

#ifndef BNibUtils_H_
#define BNibUtils_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

// forward declarations
class   Nib;


namespace NibUtils {

    //! Creates a window and its views from a nib, and returns the view identified by @a inID.
    OSPtr<HIViewRef>    CreateViewFromNib(
                            Nib&            inNib,
                            const char*     inWindowName, 
                            const HIViewID& inID);

    //! Creates a window and its views from a nib, then moves the views under @a inSuperview.
    void                CreateViewsIntoSuperview(
                            Nib&            inNib,
                            const char*     inWindowName, 
                            const HIViewID& inParentID,
                            HIViewRef       inSuperview,
                            bool            inResizeSuperview);

    void                ReplaceViewInSuperview(
                            Nib&            inNib,
                            const char*     inWindowName, 
                            const HIViewID& inParentID,
                            HIViewRef       inSuperview,
                            const HIViewID& inViewID,
                            bool            inResizeSuperview);

    OSPtr<HIViewRef>    CreateViewsIntoNewSuperview(
                            Nib&            inNib,
                            const char*     inWindowName, 
                            const HIViewID& inParentID,
                            CFStringRef     inSuperviewClassID,
                            const HIViewID& inSuperviewID,
                            HIViewRef       inSuperviewSuperview,
                            EventRef        inEvent,
                            bool            inResizeSuperview);

}   // namespace NibUtils
}   // namespace B


#endif  // BNibUtils_H_
