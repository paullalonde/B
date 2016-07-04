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
#include "BPath.h"


namespace B {
namespace Graphics {

// ------------------------------------------------------------------------------------------
Path::Path()
    : mPath(GetEmptyPath())
{
}

// ------------------------------------------------------------------------------------------
Path::Path(const OSPtr<CGMutablePathRef>& inPath)
    : mPath(CGPathCreateCopy(inPath), from_copy)
{
}

// ------------------------------------------------------------------------------------------
Path&
Path::operator = (const OSPtr<CGMutablePathRef>& inPath)
{
    mPath.reset(CGPathCreateCopy(inPath), from_copy);
    return (*this);
}

// ------------------------------------------------------------------------------------------
CGPathRef
Path::GetEmptyPath()
{
    static OSPtr<CGPathRef> sEmptyPath(
                                CGPathCreateCopy(
                                    OSPtr<CGMutablePathRef>(
                                        CGPathCreateMutable(), from_copy)), 
                                 from_copy);
    
    return (sEmptyPath);
}

}   // namespace Graphics
}   // namespace B
