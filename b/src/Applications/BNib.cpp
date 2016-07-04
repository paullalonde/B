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
#include "BNib.h"

// B headers
#include "BBundle.h"
#include "BString.h"


namespace B {

// ------------------------------------------------------------------------------------------
Nib::Nib(
    const String&   inNibName)
        : mNibRef(NULL), mBundle(Bundle::Main()), mRefCount(0)
{
    B_ASSERT(!inNibName.empty());
    
    OSStatus    err;
    
    err = CreateNibReference(inNibName.cf_ref(), &mNibRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Nib::Nib(
    const char* inNibName)
        : mNibRef(NULL), mBundle(Bundle::Main()), mRefCount(0)
{
    B_ASSERT(inNibName != NULL);
    
    String      name(inNibName, kCFStringEncodingASCII);
    OSStatus    err;
    
    err = CreateNibReference(name.cf_ref(), &mNibRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Nib::Nib(
    const Bundle&   inBundle, 
    const String&   inNibName)
        : mNibRef(NULL), mBundle(inBundle), mRefCount(0)
{
    B_ASSERT(!inNibName.empty());
    
    OSStatus    err;
    
    err = CreateNibReferenceWithCFBundle(inBundle.cf_ref(), 
                                         inNibName.cf_ref(), 
                                         &mNibRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Nib::Nib(
    const Bundle&   inBundle, 
    const char*     inNibName)
        : mNibRef(NULL), mBundle(inBundle), mRefCount(0)
{
    B_ASSERT(inNibName != NULL);
    
    String      name(inNibName, kCFStringEncodingASCII);
    OSStatus    err;
    
    err = CreateNibReferenceWithCFBundle(inBundle.cf_ref(), 
                                         name.cf_ref(), 
                                         &mNibRef);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
Nib::~Nib()
{
    DisposeNibReference(mNibRef);
}

// ------------------------------------------------------------------------------------------
OSPtr<WindowRef>
Nib::CreateWindow(
    const String&       inName) const
{
    B_ASSERT(!inName.empty());
    
    WindowRef   windowRef;
    OSStatus    err;
    
    err = CreateWindowFromNib(mNibRef, inName.cf_ref(), &windowRef);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<WindowRef>(windowRef, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<WindowRef>
Nib::CreateWindow(
    const char*     inName) const
{
    B_ASSERT(inName != NULL);
    
    String      name(inName, kCFStringEncodingASCII);
    WindowRef   windowRef;
    OSStatus    err;
    
    err = CreateWindowFromNib(mNibRef, name.cf_ref(), &windowRef);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<WindowRef>(windowRef, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<MenuRef>
Nib::CreateMenu(
    const String&       inName) const
{
    B_ASSERT(!inName.empty());
    
    MenuRef     menuRef;
    OSStatus    err;
    
    err = CreateMenuFromNib(mNibRef, inName.cf_ref(), &menuRef);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<MenuRef>(menuRef, from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<MenuRef>
Nib::CreateMenu(
    const char*     inName) const
{
    B_ASSERT(inName != NULL);
    
    String      name(inName, kCFStringEncodingASCII);
    MenuRef     menuRef;
    OSStatus    err;
    
    err = CreateMenuFromNib(mNibRef, name.cf_ref(), &menuRef);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<MenuRef>(menuRef, from_copy));
}

// ------------------------------------------------------------------------------------------
Handle
Nib::CreateMenuBar(
    const String&       inName) const
{
    B_ASSERT(!inName.empty());
    
    Handle      menuBar;
    OSStatus    err;
    
    err = CreateMenuBarFromNib(mNibRef, inName.cf_ref(), &menuBar);
    B_THROW_IF_STATUS(err);
    
    return (menuBar);
}

// ------------------------------------------------------------------------------------------
Handle
Nib::CreateMenuBar(
    const char*     inName) const
{
    B_ASSERT(inName != NULL);
    
    String      name(inName, kCFStringEncodingASCII);
    Handle      menuBar;
    OSStatus    err;
    
    err = CreateMenuBarFromNib(mNibRef, name.cf_ref(), &menuBar);
    B_THROW_IF_STATUS(err);
    
    return (menuBar);
}

// ------------------------------------------------------------------------------------------
void
Nib::SetMenuBar(
    const String&       inName) const
{
    B_ASSERT(!inName.empty());
    
    OSStatus    err;
    
    err = SetMenuBarFromNib(mNibRef, inName.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Nib::SetMenuBar(
    const char*     inName) const
{
    B_ASSERT(inName != NULL);
    
    String      name(inName, kCFStringEncodingASCII);
    OSStatus    err;
    
    err = SetMenuBarFromNib(mNibRef, name.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void    intrusive_ptr_add_ref(Nib* nib)
{
    B_ASSERT(nib != NULL);
    
    IncrementAtomic(&nib->mRefCount);
}

// ------------------------------------------------------------------------------------------
void    intrusive_ptr_release(Nib* nib)
{
    B_ASSERT(nib != NULL);
    B_ASSERT(nib->mRefCount > 0);
    
    if (DecrementAtomic(&nib->mRefCount) == 1)
    {
        // The ref count is now zero
        delete nib;
    }
}

}   // namespace B
