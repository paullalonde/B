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

#ifndef BNib_H_
#define BNib_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BBundle.h"


namespace B {


// forward declarations
class   String;


/*! @brief  Helper class for loading nib-based UI.
    
    @todo   %Document this class!
*/
class Nib : public boost::noncopyable
{
public:
    
    // constructors / destructor
    explicit    Nib(
                    const String&   inNibName);
    explicit    Nib(
                    const char*     inNibName);
    explicit    Nib(
                    const Bundle&   inBundle, 
                    const String&   inNibName);
    explicit    Nib(
                    const Bundle&   inBundle, 
                    const char*     inNibName);
                ~Nib();
    
    OSPtr<WindowRef>    CreateWindow(
                            const String&   inName) const;
    OSPtr<WindowRef>    CreateWindow(
                            const char*     inName) const;
    OSPtr<MenuRef>      CreateMenu(
                            const String&   inName) const;
    OSPtr<MenuRef>      CreateMenu(
                            const char*     inName) const;
    Handle              CreateMenuBar(
                            const String&   inName) const;
    Handle              CreateMenuBar(
                            const char*     inName) const;
    void                SetMenuBar(
                            const String&   inName) const;
    void                SetMenuBar(
                            const char*     inName) const;
    
    operator        IBNibRef () const   { return (mNibRef); }
    const Bundle&   GetBundle() const   { return (mBundle); }
    
private:
    
    // member variables
    IBNibRef        mNibRef;
    const Bundle    mBundle;
    SInt32          mRefCount;
    
    // friends
    friend void     intrusive_ptr_add_ref(Nib* nib);
    friend void     intrusive_ptr_release(Nib* nib);
};

}   // namespace B


#endif  // BNib_H_
