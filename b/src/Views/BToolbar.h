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

#ifndef BToolbar_H_
#define BToolbar_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BArray.h"
#include "BBundle.h"
#include "BEvent.h"
#include "BEventHandler.h"
#include "BEventTarget.h"
#include "BString.h"


namespace B {

// forward declarations
class   Nib;


/*! @brief  An HIToolbox toolbar.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class Toolbar : public EventTarget
{
public:
    
    //! @name Object Creation
    //@{
    //! Creates an instance of class @a T.
    template <class T>
    static OSPtr<HIToolbarRef>
            Create(
                const Bundle&   inBundle, 
                CFStringRef     inPListName,
                Nib*            inNib);
    //@}
    
    //! @name Instantiation
    //@{
    //! Create a C++ object given an @c HIObjectRef.
    static EventTarget* Instantiate(HIObjectRef inObjectRef);
    //@}
    
    // conversions
    //! Retrieves the @a HIToolbarRef associated with this C++ object.
    operator            HIToolbarRef () const   { return (mToolbarRef); }
    
    static const CFStringRef    kHIObjectClassID;
    
protected:
    
    //! @name Constructors / Destructor
    //@{
    //! Constructor.
            Toolbar(HIObjectRef inObjectRef);
    //@}
    
    //! @name Handling Events
    //@{
    //! Override to extract initialisation arguments from @a inEvent.
    virtual void    Initialize(EventRef inEvent);
    //@}
    
    virtual void    FillToolbarItemInitializationEvent(
                        EventRef                    ioEvent,
                        const String&               inIdentifier,
                        UInt32                      inAttributes,
                        CFDictionaryRef             inCustomItemData,
                        CFTypeRef                   inConfigData);
    virtual OSPtr<HIToolbarItemRef>
                    CreateToolbarItem(
                        CFStringRef                 inClassID,
                        EventRef                    inEvent);
                        
    virtual OSPtr<CFArrayRef>   GetDefaultIdentifiers();
    virtual OSPtr<CFArrayRef>   GetAllowedIdentifiers();
    virtual OSPtr<CFArrayRef>   GetSelectableIdentifiers();
    
    virtual bool    CreateItemWithIdentifier(
                        const String&               inIdentifier,
                        CFTypeRef                   inConfigData,
                        OSPtr<HIToolbarItemRef>&    outToolbarItem);
    virtual bool    CreateItemFromDrag(
                        DragRef                     inDragRef,
                        OSPtr<HIToolbarItemRef>&    outToolbarItem);
    
private:
    
    static void     SetupInitializationEvent(
                        EventRef                inEvent,
                        const Bundle&           inBundle, 
                        CFStringRef             inPlistName,
                        Nib*                    inNib);
    static OSPtr<HIToolbarRef>
                    ReadToolbarFromInitializationEvent(
                        EventRef                inEvent);
    virtual OSPtr<CFDictionaryRef>
                    ReadPropertyList();
    virtual OSPtr<HIToolbarRef>
                    MakeToolbarFromPropertyList(
                        OSPtr<CFDictionaryRef>  inPropertyList);
    virtual void    ReadToolbarItems(
                        OSPtr<CFDictionaryRef>  inPropertyList);
    
    void    InitEventHandler();
    
    // Carbon %Event handlers
    bool    ToolbarGetDefaultIdentifiers(
                B::Event<kEventClassToolbar, kEventToolbarGetDefaultIdentifiers>&       event);
    bool    ToolbarGetAllowedIdentifiers(
                B::Event<kEventClassToolbar, kEventToolbarGetAllowedIdentifiers>&       event);
    bool    ToolbarGetSelectableIdentifiers(
                B::Event<kEventClassToolbar, kEventToolbarGetSelectableIdentifiers>&    event);
    bool    ToolbarCreateItemWithIdentifier(
                B::Event<kEventClassToolbar, kEventToolbarCreateItemWithIdentifier>&    event);
    bool    ToolbarCreateItemFromDrag(
                B::Event<kEventClassToolbar, kEventToolbarCreateItemFromDrag>&          event);
    bool    ObjectDestruct(
                B::Event<kEventClassHIObject, kEventHIObjectDestruct>&                  event);
    
    // member variables
    EventHandler                    mEventHandler;
    Bundle                          mBundle;
    String                          mPListName;
    boost::intrusive_ptr<Nib>       mNib;
    HIToolbarRef                    mToolbarRef;
    boost::scoped_ptr<EventHandler> mToolbarEventHandler;
    OSPtr<CFArrayRef>               mDefaultIdentifiers;
    OSPtr<CFArrayRef>               mAllowedIdentifiers;
    OSPtr<CFArrayRef>               mSelectableIdentifiers;
    OSPtr<CFDictionaryRef>          mCustomItems;
};

// ------------------------------------------------------------------------------------------
/*!
    @param  T   The C/C++ type of the toolbar.  Must be Toolbar or derived from 
                Toolbar.
*/
template <class T>
OSPtr<HIToolbarRef>
Toolbar::Create(
    const Bundle&   inBundle, 
    CFStringRef     inPListName,
    Nib*            inNib)
{
    Event<kEventClassHIObject, kEventHIObjectInitialize>    event;
    
    SetupInitializationEvent(event, inBundle, inPListName, inNib);
    
    OSPtr<HIObjectRef>  delegate    = EventTarget::Create<T>(event);
    
    return ReadToolbarFromInitializationEvent(event);
}

}   // namespace B


#endif  // BToolbar_H_
