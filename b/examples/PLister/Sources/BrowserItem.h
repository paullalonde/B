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

#ifndef BrowserItem_H_
#define BrowserItem_H_

#pragma once

// B headers
#include "BDataBrowserItem.h"
#include "BFwd.h"
#include "BString.h"
#include "BTextFilters.h"
#include "BTextSource.h"
#include "BTextValidator.h"
#include "BUTI.h"

// project headers
#include "ModelItem.h"


// forward declarations
namespace B {
    class   AbstractDocument;  
    class   Rect;  
    class   Size;  
    class   Pasteboard;
    class   PasteboardItem;
}


class BrowserItem : public B::DataBrowserItem, public boost::signals::trackable
{
public:
    
    // constants
    static const DataBrowserPropertyID  kNameProperty   = pName;
    static const DataBrowserPropertyID  kClassProperty  = 'Clas';
    static const DataBrowserPropertyID  kValueProperty  = 'Valu';
    
    // constructors / destructors
    BrowserItem(
        ModelItemPtr        inValue, 
        B::OSPtr<MenuRef>   inClassMenu);
    
    // user interface
    void            NewChild();
    void            NewSibling();
    
    // inquiries
    ModelItemPtr    GetModel() const    { return (mValue); }
    bool            IsRoot() const;
    
    // modifers
    void            ValueChanged();
    
    // properties
    bool            IsDeletable() const;
    bool            IsMovable() const;
    virtual bool    IsEditable(
                        DataBrowserPropertyID   inProperty) const;
    virtual bool    IsContainer() const;
    virtual bool    IsContainerOpenable() const;
    virtual bool    IsContainerClosable() const;
    virtual bool    IsContainerSortable() const;
                    
    // notifications
    virtual void    ContainerOpened();
    
    // callbacks
    virtual bool    GetData(
                        B::DataBrowserItemData&     inItemData,
                        DataBrowserPropertyID       inProperty) const;
    virtual bool    SetData(
                        B::DataBrowserItemData&     inItemData,
                        DataBrowserPropertyID       inProperty);
    virtual int     CompareTo(
                        const B::DataBrowserItem&   inItem,
                        DataBrowserPropertyID       inProperty) const;
    virtual bool    AddDragFlavors(
                        B::Drag&                    ioDrag,
                        PasteboardItemID&           outItemID) const;
    virtual bool    IsDragAcceptable(
                        const B::Drag&              inDrag) const;
    virtual bool    ReceiveDrag(
                        const B::Drag&              inDrag);
    virtual bool    SupplyHelpContent(
                        DataBrowserPropertyID       inProperty,
                        B::String&                  outMinContent,
                        bool&                       outMinContentIsLocalized,
                        B::String&                  outMaxContent,
                        bool&                       outMaxContentIsLocalized);
#if 0
    virtual void    CustomDraw(
                        DataBrowserPropertyID       inProperty, 
                        DataBrowserItemState        inItemState, 
                        const B::Rect&              inRect, 
                        SInt16                      inDepth, 
                        Boolean                     inColorDevice) const;
    virtual bool    CustomHitTest(
                        DataBrowserPropertyID       inProperty, 
                        const B::Rect&              inRect, 
                        const B::Rect&              inMouseRect) const;
    virtual void    CustomDragRgn(
                        DataBrowserPropertyID       inProperty, 
                        const B::Rect&              inRect, 
                        RgnHandle                   ioDragRgn) const;
#endif
    
    // data exchange
    void        WriteDropLocation(const B::Drag& inDrag) const;
    static ModelItem*
                ReadDropLocation(const B::Drag& inDrag);
    PasteboardItemID
                WriteFlavors(B::Pasteboard& ioPasteboard) const;
    
    static bool AreAllDragItemsAcceptable(
                    const B::Drag&  inDrag, 
                    ModelItemPtr    inModelItem, 
                    bool&           outItemsSupportMove);
    
    static void GetSupportedServiceFlavors(
                    B::MutableArray<CFStringRef>&   ioCopyTypes,
                    B::MutableArray<CFStringRef>&   ioPasteTypes);
    
protected:
    
    void        Rename(const B::String& inNewName);
    
    // notifications
    void        ModelItemNameChanged();
    void        ModelItemClassChanging();
    void        ModelItemClassChanged();
    void        ModelItemValueChanged();
    void        ModelItemDeleted();
    void        ModelItemNewChild(ModelItemPtr childItem);
    void        ModelItemSelect();
    
private:
    
    class BrowserItemTextSource : public B::TextSource
    {
    public:
        BrowserItemTextSource(const B::String& inText) : mText(inText) {}
        virtual B::String   GetText() const { return (mText); }
    private:
        B::String   mText;
    };
    
    class BooleanFilter
    {
    public:

        BooleanFilter();
        
        B::TextValidator::Action
        operator () (
            bool                    inFiltering,
            const B::TextSource&    inSource,
            const B::String&        inCandidateText,
            const CFRange&          inSelection,
            B::String&              outReplacementText) const;
        
        bool        Parse(const B::String& inText) const;
        B::String   Format(bool inBoolean) const;
        
    private:
        
        class TextMatcher : public std::unary_function<B::String, bool>
        {
        public:
            TextMatcher(const B::String& inText) : mText(inText) {}
            bool operator () (const B::String& inString) const;
        private:
            const B::String& mText;
        };
        
        static bool Matches(const B::String& inText, const std::vector<B::String>& inStrings);
        
        std::vector<B::String>  mTrueStrings;
        std::vector<B::String>  mFalseStrings;
    };
    
    class DataFilter
    {
    public:

        DataFilter();
        
        B::TextValidator::Action
        operator () (
            bool                    inFiltering,
            const B::TextSource&    inSource,
            const B::String&        inCandidateText,
            const CFRange&          inSelection,
            B::String&              outReplacementText) const;
        
        B::OSPtr<CFDataRef> Parse(const B::String& inText) const;
        B::String           Format(const B::OSPtr<CFDataRef>& inData) const;
        
    private:
        
        static B::OSPtr<CFCharacterSetRef>  GetCharSet(CFCharacterSetRef ws);
        
        bool    SkipWS(const B::String& str, size_t inSize, size_t& ioIndex) const;
        
        B::OSPtr<CFCharacterSetRef>     mWhitespace;
        B::TextFilters::CharacterSet    mCharacterSetFilter;
    };
    
    DragActions DetermineDragAction(
                    const B::Drag&  inDrag, 
                    bool            inItemsSupportMove, 
                    bool            inReceiving) const;
    void        CopyDraggedItems(
                    const B::Drag&  inDrag);
#if 0
    void        CopyDraggedItemsForXMLFlavor(
                    DragRef         inDragRef);
#endif
    static void CalcStringContentSize(
                    const B::Rect&          inRect, 
                    B::String&              ioString, 
                    ThemeFontID             inFont, 
                    ThemeDrawState          inDrawState, 
                    TruncCode               inTruncCode, 
                    B::Size&                outContentSize);
    void        CalcContentRect(
                    DataBrowserPropertyID   inPropertyID, 
                    const B::Rect&          inRect, 
                    const B::Size&          inContentSize, 
                    B::Rect&                outContentRect) const;
    void        GetDrawState(
                    ThemeFontID&    outFont, 
                    ThemeBrush&     outBrush, 
                    ThemeDrawState& outState) const;
                
//  void        WriteItem(B::FlavorOutputStream& ostr) const;
    static ModelItemPtr
                ReadItem(B::PasteboardItem& pbItem);
    
    void            CalcValueString() const;
    void            CalcValidator();
    void            ParseValueString(const B::String& inText);
    MenuItemIndex   TypeIDToMenuItem() const;
    B::String       TypeIDToMenuItemText() const;
//  void            SetUpAction(const B::String& inActionName) const;
    EventTargetRef  GetUndoTarget();
    static const B::TextFilters::Number&    GetIntegerFilter();
    static const B::TextFilters::Number&    GetRealFilter();
    static const B::TextFilters::Date&      GetDateFilter();
    static const BooleanFilter&             GetBooleanFilter();
    static const DataFilter&                GetDataFilter();
    
    // member variables
    ModelItemPtr        mValue;
    bool                mContainer;
    B::OSPtr<MenuRef>   mClassMenu;
    mutable B::String   mValueString;
    B::TextValidator    mValidator;
    
    static B::AutoUTI   sItemPointerUTI;
    static B::AutoUTI   sInternalUTI;
    static B::AutoUTI   sPropertyListUTI;
};


//  Inline member function definitions

inline bool
BrowserItem::IsDeletable() const
{
    return (!IsRoot());
}

inline bool
BrowserItem::IsMovable() const
{
    return (!IsRoot());
}


#endif  // BrowserItem_H_
