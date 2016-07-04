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

#ifndef BTextUtilities_H_
#define BTextUtilities_H_

#pragma once

// B headers
#include "BFwd.h"


namespace B {

// ATSU styles

typedef boost::intrusive_ptr<struct OpaqueATSUStyle>        AtsuStylePtr;

AtsuStylePtr    MakeAtsuStylePtr();
AtsuStylePtr    MakeAtsuStylePtr(ATSUStyle style);
AtsuStylePtr    MakeAtsuStylePtr(ThemeFontID inThemeFontID, ScriptCode inScriptCode);
AtsuStylePtr    CopyAtsuStylePtr(const AtsuStylePtr& src);

// ATSU layouts

typedef boost::intrusive_ptr<struct OpaqueATSUTextLayout>   AtsuLayoutPtr;

AtsuLayoutPtr   MakeAtsuLayoutPtr();
AtsuLayoutPtr   MakeAtsuLayoutPtr(ATSUTextLayout layout);
AtsuLayoutPtr   CopyAtsuLayoutPtr(const AtsuLayoutPtr& src);

}   // namespace B

// DO NOT CALL DIRECTLY !!! -- AtsuStylePtr support.
void    intrusive_ptr_add_ref(ATSUStyle style);
void    intrusive_ptr_release(ATSUStyle style);

// DO NOT CALL DIRECTLY !!! -- AtsuLayoutPtr support.
void    intrusive_ptr_add_ref(ATSUTextLayout layout);
void    intrusive_ptr_release(ATSUTextLayout layout);


#endif  // BTextUtilities_H_
