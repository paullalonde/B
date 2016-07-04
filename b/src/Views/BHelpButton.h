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

#ifndef BHelpButton_H_
#define BHelpButton_H_

#pragma once

// B headers
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BEventTarget.h"
#include "BString.h"
#include "BUrl.h"


namespace B {

/*! @brief  A custom view implementing a Help button.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class HelpButton : public EventTarget
{
public:
    
    // constants
    enum    { kControlKindSig   = kEventClassB, 
              kControlKind      = FOUR_CHAR_CODE('Help') };
    enum    {
              kBookTag          = FOUR_CHAR_CODE('Book'),
              kUrlTag           = FOUR_CHAR_CODE('Url '),
              kAXDescTag        = FOUR_CHAR_CODE('Desc'),
            };
    
    //! @name Object Creation
    //@{
#if 0
    //! Create an object and its HIToolbox view, from the given parameters.
    static OSPtr<HIViewRef>
            Create(
                SInt32          inViewID,
                HIViewRef       inSuperview, 
                const Rect*     inFrame,
                Nib*            inFromNib,
                const Url&      inUrl, 
                const String&   inHelpBook = String());
#endif
    //@}
    
    //! @name Instantiation
    //@{
    //! Create a C++ object given an @c HIObjectRef.
    static EventTarget* Instantiate(HIObjectRef inObjectRef);
    //@}
    
    // class ID
    static const CFStringRef    kHIObjectClassID;
    static const CFStringRef    kHIObjectBaseClassID;
    
protected:
    
    // constructors / destructor
                    HelpButton(
                        HIObjectRef     inObjectRef);
    virtual         ~HelpButton();
    
    virtual ControlKind
                    GetKind() const;

    // overrides from EventTarget
    virtual void    Initialize(EventRef inEvent);
    virtual void    PrintDebugInfo();
    
private:
    
    // types
    template <ResType TAG> struct Data : public ViewData<kControlKindSig, kControlKind, TAG> {};
    
    void    InitEventHandler();
    
    bool    ControlSetData(
                Event<kEventClassControl, kEventControlSetData>&    event);
    bool    ControlGetData(
                Event<kEventClassControl, kEventControlGetData>&    event);
    bool    CommandProcess(
                Event<kEventClassCommand, kEventCommandProcess>&    event);
    
    void    SetBook(String book);
    void    SetUrl(String url);
    
    // member variables
    const HIViewRef mViewRef;
    EventHandler    mEventHandler;
    IconRef         mIcon;
    String          mBook;
    Url             mUrl;
};


#ifndef DOXYGEN_SKIP

template <> struct ViewDataDef<
    HelpButton::kControlKindSig, 
    HelpButton::kControlKind, 
    HelpButton::kBookTag>       { typedef ViewDataTrait<CFStringRef>    Trait;  };

template <> struct ViewDataDef<
    HelpButton::kControlKindSig, 
    HelpButton::kControlKind, 
    HelpButton::kUrlTag>        { typedef ViewDataTrait<CFStringRef>    Trait;  };

template <> struct ViewDataDef<
    HelpButton::kControlKindSig, 
    HelpButton::kControlKind, 
    HelpButton::kAXDescTag>     { typedef ViewDataTrait<CFStringRef>    Trait;  };

#endif  // DOXYGEN_SKIP


}   // namespace B

#endif  // BHelpButton_H_
