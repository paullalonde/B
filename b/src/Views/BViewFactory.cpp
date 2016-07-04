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
#include "BViewFactory.h"

// B headers
#include "BOSPtr.h"
#include "BView.h"


namespace B {


// static member variables
ViewFactory::FactoryProc    ViewFactory::sDefaultProc   = ViewFactory::DefaultFactoryProc;
OSType                      ViewFactory::sDefaultIDSig  = 0;

// ------------------------------------------------------------------------------------------
ViewFactory&
ViewFactory::Default()
{
    static ViewFactory  sDefaultFactory(true);
    
    return (sDefaultFactory);
}

// ------------------------------------------------------------------------------------------
OSType
ViewFactory::GetDefaultSignature()
{
    return (sDefaultIDSig);
}

// ------------------------------------------------------------------------------------------
/*! The default signature is used when registering View subclasses by ID only.  It allows 
    registration code to be more concise, by avoiding having to pass in the signature of 
    each View instance.
*/
void
ViewFactory::SetDefaultSignature(OSType inSig)
{
    sDefaultIDSig = inSig;
}

// ------------------------------------------------------------------------------------------
ViewFactory::FactoryProc
ViewFactory::GetDefaultFactoryProc()
{
    return (sDefaultProc);
}

// ------------------------------------------------------------------------------------------
/*! The default factory is used when no matching factory function has been found for a 
    given @c HIViewRef.  It's meant to be a factory of last resort.
*/
void
ViewFactory::SetDefaultFactoryProc(FactoryProc inProc)
{
    sDefaultProc = inProc;
}

// ------------------------------------------------------------------------------------------
ViewFactory::ViewFactory()
    : mDefault(false)
{
}

// ------------------------------------------------------------------------------------------
ViewFactory::ViewFactory(bool)
    : mDefault(true)
{
}

// ------------------------------------------------------------------------------------------
void
ViewFactory::Register(
    CFStringRef     inClassId, 
    const HIViewID& inViewId,
    FactoryProc     inProc)
{
    B_ASSERT((inClassId != NULL) && (inProc != NULL));
    B_ASSERT((inViewId.signature == 0) || (inViewId.id != 0));
    
    mMap.insert(FactoryMap::value_type(Key(inClassId, inViewId), inProc));
}

// ------------------------------------------------------------------------------------------
void
ViewFactory::Unregister(
    CFStringRef     inClassId, 
    const HIViewID& inViewId)
{
    B_ASSERT(inClassId != NULL);
    B_ASSERT((inViewId.signature == 0) || (inViewId.id != 0));
    
    mMap.erase(Key(inClassId, inViewId));
}

// ------------------------------------------------------------------------------------------
/*! @return The newly instantiated View.  Never returns @c NULL (an exception is thrown if something goes wrong).
*/
View*
ViewFactory::Instantiate(
    HIViewRef   inViewRef) const
{
    B_ASSERT(inViewRef != NULL);
    
    View*   view    = View::GetViewFromRef(inViewRef, std::nothrow);
    
    if (view != NULL)
        return view;
    
    FactoryProc proc    = NULL;
    HIViewID    viewId;
    OSStatus    err;
    
    err = HIViewGetID(inViewRef, &viewId);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CFStringRef>  classId(HIObjectCopyClassID(reinterpret_cast<HIObjectRef>(inViewRef)), from_copy);
    
    proc = FindMatch(classId, viewId);
    
    if ((proc == NULL) && !IsDefault())
    {
        proc = Default().FindMatch(classId, viewId);
    }
    
    if (proc == NULL)
    {
        proc = sDefaultProc;
    }
    
    B_ASSERT(proc != NULL);
    
    view = (*proc)(inViewRef);
    
    B_THROW_IF_NULL(view);
    
    return (view);
}

// ------------------------------------------------------------------------------------------
ViewFactory::FactoryProc
ViewFactory::FindMatch(
    CFStringRef         inClassID, 
    const HIViewID&     inID) const
{
    FactoryProc proc    = NULL;
    
    if (!mMap.empty())
    {
        // First, look for a complete match.
        
        FactoryMap::const_iterator  it;
        
        it = mMap.find(Key(inClassID, inID));
        
        if (it == mMap.end())
        {
            // Next, look for a match on class ID and HIViewID.id.
            
            HIViewID    tempId  = { 0, inID.id };
            
            it = mMap.find(Key(inClassID, tempId));
            
            if (it == mMap.end())
            {
                // Finally, look for a match on class ID only.
                
                tempId.id = 0;
                
                it = mMap.find(Key(inClassID, tempId));
            }
        }
        
        if (it != mMap.end())
        {
            proc = it->second;
        }
    }
    
    return (proc);
}

// ------------------------------------------------------------------------------------------
View*
ViewFactory::DefaultFactoryProc(
    HIViewRef   inViewRef)
{
    return (new PredefinedView(inViewRef));
}

// ------------------------------------------------------------------------------------------
bool
ViewFactory::KeyCompare::operator()(const Key& x, const Key& y) const
{
    int result  = CFStringCompare(x.classId, y.classId, 0);
    
    return ((result < 0) || 
            ((result == 0) && (x.viewId < y.viewId)));
}

}   // namespace B
