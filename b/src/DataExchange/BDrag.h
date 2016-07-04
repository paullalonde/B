// ==========================================================================================
//  
//  Copyright (C) 2005-2006 Paul Lalonde enrg.
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

#ifndef BDrag_H_
#define BDrag_H_

#pragma once

// B headers
#include "BPasteboard.h"


namespace B {

// forward declarations
class   AEDescriptor;
class   Point;
class   Rect;


class Drag
{
public:
    
    //! @name Constructor / destructor
    //@{
    //! Default constructor. Creates a new @c DragRef.
            Drag();
    //! Constructs a drag object from an existing @c DragRef.
            Drag(DragRef inDragRef);
    //! Constructs a drag object from an existing Pasteboard. Creates a new @c DragRef.
            Drag(PasteboardRef inPasteboard);
    //! Destructor.
    virtual ~Drag();
    //@}
    
    //! @name Drag Initiator
    //@{
    void        SetAllowableActions(
                    DragActions                 inLocalActions, 
                    DragActions                 inRemoteActions);
    void        SetImage(
                    const OSPtr<CGImageRef>&    inImage,
                    const Point&                inMouseOffset,
                    DragImageFlags              inFlags);
    void        SetItemBounds(
                    PasteboardItem&             ioItem, 
                    const CGRect&               inBounds, 
                    HIViewRef                   inView);
    DragActions GetDropAction() const;
    void        GetDropLocation(AEDescriptor& outDesc) const;
    StandardDropLocation
                GetStandardDropLocation() const;
    //@}
    
    //! @name Drag Receiver
    //@{
    DragAttributes
                GetAttributes() const;
    bool        HasLeftSenderWindow() const     { return ((GetAttributes() & kDragHasLeftSenderWindow) != 0); }
    bool        IsInsideSenderWindow() const    { return ((GetAttributes() & kDragInsideSenderWindow) != 0); }
    bool        IsInsideSender() const          { return ((GetAttributes() & kDragInsideSenderApplication) != 0); }
    Point       GetOrigin(HIViewRef inView) const;
    Point       GetMouse(HIViewRef inView) const;
    Point       GetPinnedMouse(HIViewRef inView) const;
    void        SetPinnedMouse(const Point& inPt, HIViewRef inView) const;
    UInt32      GetModifiers()const ;
    UInt32      GetMouseDownModifiers() const;
    UInt32      GetMouseUpModifiers() const;
    void        GetModifiers(
                    UInt32& outModifiers,
                    UInt32& outMouseDownModifiers,
                    UInt32& outMouseUpModifiers) const;
    DragActions GetAllowableActions() const;
    void        SetDropAction(DragActions inAction) const;
    void        SetDropLocation(const AEDesc& inDesc) const;
    void        SetStandardDropLocation(StandardDropLocation inLocation) const;
    Rect        GetItemBounds(
                    const PasteboardItem&   inItem,
                    HIViewRef               inView) const;
    //@}
    
    //! @name Inquiries
    //@{
    DragRef             GetDragRef() const      { return (mDragRef); }
    Pasteboard&         GetPasteboard()         { return (mPasteboard); }
    const Pasteboard&   GetPasteboard() const   { return (mPasteboard); }
    //@}
    
    //! @name Conversions
    //@{
    static PasteboardItemID MakePasteboardItemID(DragItemRef itemRef);
    static DragItemRef      MakeDragItemRef(PasteboardItemID itemID);
    //@}
    
private:
    
    // member variables
    DragRef     mDragRef;
    bool        mDragOwner;
    Pasteboard  mPasteboard;
};

}   // namespace B


#endif  // BDrag_H_
