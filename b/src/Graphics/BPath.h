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

#ifndef BPath_H_
#define BPath_H_

#pragma once

// B headers
#include "BOSPtr.h"
#include "BRect.h"


namespace B {
namespace Graphics {

class Path
{
public:
    
    // constructors / destructor
                Path();
                Path(const Path& inPath);
    explicit    Path(const OSPtr<CGPathRef>& inPath);
    explicit    Path(const OSPtr<CGMutablePathRef>& inPath);
                
    // assignment
    Path&   operator = (const Path& inPath);
    Path&   operator = (const OSPtr<CGPathRef>& inPath);
    Path&   operator = (const OSPtr<CGMutablePathRef>& inPath);
    //! Exchanges the contents of the path with @a ioPath.
    void    swap(Path& ioPath);
    
    // inquiries
    Rect    box() const     { return (Rect(CGPathGetBoundingBox(mPath))); }
    bool    empty() const   { return (CGPathIsEmpty(mPath)); }
    Point   current() const { return (Point(CGPathGetCurrentPoint(mPath))); }
    
    // conversions
    operator CGPathRef () const { return (mPath); }
    
private:
    
    static CGPathRef    GetEmptyPath();
    
    // member variables
    OSPtr<CGPathRef>    mPath;
};

// ------------------------------------------------------------------------------------------
inline
Path::Path(const Path& inPath)
    : mPath(inPath.mPath)
{
}

// ------------------------------------------------------------------------------------------
inline
Path::Path(const OSPtr<CGPathRef>& inPath)
    : mPath(inPath)
{
}

// ------------------------------------------------------------------------------------------
inline void
Path::swap(Path& ioPath)
{
    mPath.swap(ioPath.mPath);
}

// ------------------------------------------------------------------------------------------
inline Path&
Path::operator = (const Path& inPath)
{
    mPath = inPath.mPath;
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline Path&
Path::operator = (const OSPtr<CGPathRef>& inPath)
{
    mPath = inPath;
    return (*this);
}


/*! @defgroup   PathFunctions   B::Path Global Functions
*/
//@{

// ------------------------------------------------------------------------------------------
inline bool operator == (const Path& c1, const Path& c2)
{
    return (CGPathEqualToPath(c1, c2));
}

// ------------------------------------------------------------------------------------------
inline bool operator != (const Path& c1, const Path& c2)
{
    return (!CGPathEqualToPath(c1, c2));
}

//@}


//! @name Miscellaneous
//@{

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of @a c1 and @a c2.

    @relates    Path
*/
template <typename T> inline void
swap(Path& c1, Path& c2)    { c1.swap(c2); }

//@}


}   // namespace Graphics
}   // namespace B


#endif  // BPath_H_
