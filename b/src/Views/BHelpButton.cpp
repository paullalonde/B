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
#include "BHelpButton.h"

// B headers
#include "BBundle.h"
#include "BCollectionItem.h"
#include "BEvent.h"
#include "BEventParams.h"
#include "BHelpUtilities.h"
#include "BViewData.h"
#include "BViewUtils.h"


namespace B {

    template <> struct EventParamNameTrait<HelpButton::kBookTag>    { enum { kTypeTag = typeCFStringRef };  };
    template <> struct EventParamNameTrait<HelpButton::kUrlTag>     { enum { kTypeTag = typeCFStringRef };  };
    template <> struct EventParamNameTrait<HelpButton::kAXDescTag>  { enum { kTypeTag = typeCFStringRef };  };

}

namespace {

    typedef B::CollectionItem<
        B::HelpButton::kControlKindSig, 
        B::HelpButton::kBookTag, 
        CFStringRef>                        BookCollectionItem;

    typedef B::CollectionItem<
        B::HelpButton::kControlKindSig, 
        B::HelpButton::kUrlTag, 
        CFStringRef>                        UrlCollectionItem;

}   // anonymous namespace


namespace B {

const CFStringRef   HelpButton::kHIObjectClassID        = CFSTR("ca.paullalonde.b.view.helpbutton");
const CFStringRef   HelpButton::kHIObjectBaseClassID    = kHIRoundButtonClassID;

#if 0

// ------------------------------------------------------------------------------------------
OSPtr<HIViewRef>
HelpButton::Create(
    SInt32          inViewID,       //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef       inSuperview,    //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const Rect*     inFrame,        //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    Nib*            inFromNib,      //!< The nib from which to load other resources;  may be @c NULL.
    const Url&      inUrl, 
    const String&   inHelpBook /* = String() */)
{
    OSPtr<Collection>   collPtr(NewCollection(), true);
    OSPtr<HIViewRef>    viewPtr;
    
    SizeCollectionItem::Set(collPtr, inSize);
    BookCollectionItem::Set(collPtr, inHelpBook);
    UrlCollectionItem ::Set(collPtr, inUrl);
    
    viewPtr = CustomView::Create<HelpButton>(inViewID, inSuperview, inFrame, inFromNib, 
                                             collPtr);
    
    return (viewPtr);
}

#endif

// ------------------------------------------------------------------------------------------
EventTarget*
HelpButton::Instantiate(HIObjectRef inObjectRef)
{
    return (new HelpButton(inObjectRef));
}

// ------------------------------------------------------------------------------------------
HelpButton::HelpButton(
    HIObjectRef     inObjectRef)
        : EventTarget(inObjectRef),
          mViewRef(reinterpret_cast<HIViewRef>(inObjectRef)), mEventHandler(inObjectRef)
{
    OSStatus    err;
    
    err = GetIconRef(kOnSystemDisk, kSystemIconsCreator, kHelpIcon, &mIcon);
    B_THROW_IF_STATUS(err);
    
    InitEventHandler();
}

// ------------------------------------------------------------------------------------------
HelpButton::~HelpButton()
{
    ReleaseIconRef(mIcon);
}

// ------------------------------------------------------------------------------------------
void
HelpButton::Initialize(EventRef inEvent)
{
    // Set up the button's icon (which is the system help icon).
    
    HIViewImageContentInfo  content;
    
    content.contentType = kHIViewContentIconRef;
    content.u.iconRef   = mIcon;
    
    ViewData<kControlKindSignatureApple, kControlKindRoundButton, kControlRoundButtonContentTag>::Set(mViewRef, content);
    
    String  str;
    
    // Read the control's accessibility description.  This is a localised string key.  If 
    // no key was specified, use a default one.
    
    if (EventParam<kAXDescTag>::Get(inEvent, str, std::nothrow))
    {
        str = Bundle::Main().GetLocalisedString(str.cf_ref(), str.cf_ref(), NULL);
    }
    else
    {
        str = BLocalizedStringFromTable(CFSTR("HelpButtonAXDescription"), CFSTR("B"),
                                        "The default accessibility description for Help buttons.");
    }
    
    SetAccessibilityAttribute(kAXDescriptionAttribute, str.cf_ref());
    
    // Read the control's help book.
    
    if (!EventParam<kBookTag>::Get(inEvent, str, std::nothrow))
        str.clear();
    
    SetBook(str);
    
    // Read the control's help url.

    if (EventParam<kUrlTag>::Get(inEvent, str, std::nothrow))
    {
        SetUrl(str);
    }
}

// ------------------------------------------------------------------------------------------
/*! Fills in the view's EventHandler.  The view's flags affect the set of Carbon %Events 
    the view will respond to.
*/
void
HelpButton::InitEventHandler()
{
    mEventHandler.Add(this, &HelpButton::ControlSetData);
    mEventHandler.Add(this, &HelpButton::ControlGetData);
    mEventHandler.Add(this, &HelpButton::CommandProcess);
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
void
HelpButton::SetBook(String book)
{
    mBook = book;
    
    if (mBook.empty())
        mBook = Bundle::Main().GetHelpBook();
}

// ------------------------------------------------------------------------------------------
void
HelpButton::SetUrl(String url)
{
    mUrl = url;
}

// ------------------------------------------------------------------------------------------
bool
HelpButton::ControlSetData(
    Event<kEventClassControl, kEventControlSetData>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = true;
        
        switch (event.mDataTag)
        {
        case kBookTag:
            SetBook(Data<kBookTag>::Get(event.mDataPtr, event.mDataSize));
            break;
            
        case kUrlTag:
            SetUrl(Data<kUrlTag>::Get(event.mDataPtr, event.mDataSize));
            break;
            
        default:
            handled = false;
            break;
        }
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
HelpButton::ControlGetData(
    Event<kEventClassControl, kEventControlGetData>&    event)
{
    bool    handled = false;
    
    if (event.mViewRef == mViewRef)
    {
        handled = true;
        
        switch (event.mDataTag)
        {
        case kBookTag:
            Data<kBookTag>::Set(event.mDataPtr, event.mDataSize, mBook);
            break;
            
        case kUrlTag:
            Data<kUrlTag>::Set(event.mDataPtr, event.mDataSize, mUrl.GetString());
            break;
            
        default:
            handled = false;
            break;
        }
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
HelpButton::CommandProcess(
    Event<kEventClassCommand, kEventCommandProcess>&    event)
{
    bool    handled = false;
    
    if (event.mHICommand.commandID != 0)
    {
        UInt32      commandID;
        OSStatus    err;
        
        err = HIViewGetCommandID(mViewRef, &commandID);
        
        if ((err == noErr) && (event.mHICommand.commandID == commandID))
        {
            HelpUtilities::GotoPage(mBook, mUrl);
        }
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
ControlKind
HelpButton::GetKind() const
{
    return (ViewUtils::MakeControlKind(kControlKindSig, kControlKind));
}

// ------------------------------------------------------------------------------------------
void
HelpButton::PrintDebugInfo()
{
    std::string temp;
    Url         baseUrl;
    
    printf("B::HelpButton\n");
    
    mBook.copy(temp, kCFStringEncodingASCII);
    printf("    %-20s: \"%s\"\n", "Book", temp.c_str());
    
    mUrl.GetString().copy(temp, kCFStringEncodingASCII);
    printf("    %-20s: string = \"%s\", base = ", "Url", temp.c_str());
    
    if (mUrl.GetBase(baseUrl))
    {
        baseUrl.GetString().copy(temp, kCFStringEncodingASCII);
        printf("\"%s\"\n", temp.c_str());
    }
    else
    {
        printf("NULL\n");
    }
    
    EventTarget::PrintDebugInfo();
}


#ifndef DOXYGEN_SKIP
B_VAR_EXPORT EventTargetRegistrar<HelpButton>   gRegisterHelpButton;
#endif


}   // namespace B
