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

#ifndef DragPeekerView_H_
#define DragPeekerView_H_

#pragma once

// standard headers
#include <vector>

// B headers
#include "BDrag.h"
#include "BHybridView.h"
#include "BPasteboard.h"
#include "BRect.h"


class DragPeekerView : public B::HybridView
{
public:
    
    // constants
    enum    { kControlKindSig = FOUR_CHAR_CODE('DrPk'), 
              kControlKind = FOUR_CHAR_CODE('DrVw') };
    
    // instantiation
    static B::EventTarget*  Instantiate(HIObjectRef inObjectRef);
    
    //! Creates an instance of DragPeekerView.
    static std::pair<B::OSPtr<HIViewRef>, DragPeekerView*>
            Create(
                SInt32          inViewID = 0,
                HIViewRef       inSuperview = NULL, 
                const B::Rect*  inFrame = NULL, 
                B::Nib*         inFromNib = NULL);
    //@}
    
    void    FillWithPasteboard(const B::Pasteboard& inPasteboard);
    
    static const CFStringRef    kHIObjectClassID;
    
protected:
    
    // constructor
    explicit    DragPeekerView(HIObjectRef inObjectRef);
    
    virtual const ViewPartVector&
                    GetViewParts() const;
    virtual ControlKind
                    GetKind() const;
    
    // CarbonEvent handlers
    virtual void    Draw(
                        HIViewPartCode  inPart, 
                        CGContextRef    inContext, 
                        const B::Shape* inDrawShape);
    virtual B::Rect GetOptimalBounds() const;
    virtual void    GetMinimumSize(
                        B::Size&        outMinimumSize) const;
    virtual void    GetMaximumSize(
                        B::Size&        outMaximumSize) const;
    virtual void    GetScrollInfo(
                        B::Size&        outImageSize,
                        B::Size&        outViewSize,
                        B::Size&        outLineSize,
                        B::Point&       outOrigin) const;
    virtual void    ScrollTo(
                        const B::Point& intOrigin);
    virtual bool    DragEnter(
                        DragRef         inDragRef);
    virtual void    DragLeave(
                        DragRef         inDragRef);
    virtual bool    DragReceive(
                        DragRef         inDragRef);
    
    // overrides from EventTarget
    virtual void    PrintDebugInfo();
    
    //! Handles an @c HICommand.
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        B::CommandData&             ioCmdData);
    
private:
    
    // types
    
    struct FlavorData
    {
        B::String               mFlavor;
        PasteboardFlavorFlags   mFlags;
        size_t                  mLength;
        std::vector<char>       mData;
        B::String               mOSType;
        B::String               mNSType;
        B::String               mFileType;
        B::String               mMimeType;
    };
    
    struct ItemData
    {
        PasteboardItemID        mItemID;
        B::Rect                 mBounds;
        std::vector<FlavorData> mFlavors;
    };
    
    class FillItemVector
    {
    public:
                FillItemVector(const B::Drag* inDrag, std::vector<ItemData>& items, bool getData, HIViewRef view);
        void    operator () (const B::PasteboardItem& inItem) const;
        
    private:
        const B::Drag* const    mDrag;
        std::vector<ItemData>&  mItems;
        const bool              mGetData;
        const HIViewRef         mView;
    };
    
    class FillFlavorVector
    {
    public:
                FillFlavorVector(std::vector<FlavorData>& flavors, bool getData);
        void    operator () (const B::PasteboardFlavor& inFlavor) const;
        
    private:
        std::vector<FlavorData>&    mFlavors;
        const bool                  mGetData;
    };
    
    void    GetTextInfo(HIThemeTextInfo& outTextInfo, B::Rect& outTextBounds) const;
    void    FillItems(const B::Drag* inDrag, const B::Pasteboard& inPasteboard, bool getData);
    void    ShowText(CGContextRef context, const char* str);
    
    // member variables
    B::OSPtr<CFStringRef>   mEmptyStr;
    mutable B::Size         mEmptyStrSize;
    std::vector<ItemData>   mDragItems;
    DragAttributes          mDragAttributes;
    SInt16                  mDragModifiers;
    SInt16                  mDragMouseDownModifiers;
    SInt16                  mDragMouseUpModifiers;
    DragActions             mDragAllowableActions;
    DragActions             mDragAction;
    DragRef                 mCurrentDragRef;
    float                   mFontSize;
    float                   mLineHeight;
};


#endif  // DragPeekerView_H_
