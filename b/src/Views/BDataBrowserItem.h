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

#ifndef BDataBrowserItem_H_
#define BDataBrowserItem_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>


namespace B {

// forward declarations
class   DataBrowser;
class   DataBrowserItemData;
class   Drag;
class   Point;
class   Rect;
class   String;


/*! @brief  An item in a DataBrowser
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class DataBrowserItem : public boost::noncopyable
{
public:
    
    // constructors / destructors
                        DataBrowserItem();
    virtual             ~DataBrowserItem();
    
    // inquiries
    DataBrowserItem*    GetParent() const;
    DataBrowserItemState
                        GetState() const;
    bool                IsSelected() const;
    bool                IsOpen() const;
    UInt16              GetHeight() const;
    ::Rect              GetBounds(
                            DataBrowserPropertyID   inProperty, 
                            DataBrowserPropertyPart inPropertyPart) const;
#if 0   // DELETE
    void                GetBounds(
                            DataBrowserPropertyID   inProperty, 
                            DataBrowserPropertyPart inPropertyPart, 
                            ::Rect&                 outBounds) const;
#endif
    
    // convenience
    void                Open();
    void                Close();
    void                Select();
    void                Reveal(
                            DataBrowserPropertyID   inProperty,
                            bool                    inCenterInView = false);
    void                Update(
                            DataBrowserPropertyID   inProperty);
    
    // properties
    virtual bool        IsActive() const;
    virtual bool        IsSelectable() const;
    virtual bool        IsEditable(
                            DataBrowserPropertyID   inProperty) const;
    virtual bool        IsContainer() const;
    
    // notifications
    virtual void        ItemAdded();
    virtual void        ItemRemoved();
    virtual void        ItemSelected();
    virtual void        ItemDeselected();
    virtual void        ItemDoubleClicked();
    virtual void        ContainerOpened();

    virtual bool        IsContainerOpenable() const;
    virtual bool        IsContainerClosable() const;
    virtual bool        IsContainerSortable() const;
    virtual DataBrowserItemID
                        GetContainerAliasID() const;
    virtual DataBrowserPropertyID
                        GetMainProperty() const;
    virtual void        ContainerClosing();
    virtual void        ContainerClosed();
    virtual void        ContainerSorting();
    virtual void        ContainerSorted();
    virtual void        UserToggledContainer();
    virtual void        TargetChanged();
    virtual void        EditStarted();
    virtual void        EditStopped();
    
    // callbacks
    virtual bool        GetData(
                            DataBrowserItemData&    inItemData,
                            DataBrowserPropertyID   inProperty) const;
    virtual bool        SetData(
                            DataBrowserItemData&    inItemData,
                            DataBrowserPropertyID   inProperty);
    virtual int         CompareTo(
                            const DataBrowserItem& inItem,
                            DataBrowserPropertyID   inProperty) const;
    virtual bool        AddDragFlavors(
                            Drag&                   ioDrag,
                            PasteboardItemID&       outItemID) const;
    virtual bool        IsDragAcceptable(
                            const Drag&             inDrag) const;
    virtual bool        ReceiveDrag(
                            const Drag&             inDrag);
    virtual bool        SupplyHelpContent(
                            DataBrowserPropertyID   inProperty,
                            String&                 outMinContent,
                            bool&                   outMinContentIsLocalized,
                            String&                 outMaxContent,
                            bool&                   outMaxContentIsLocalized);
    virtual bool        KeyFilter(
                            UInt16&                 ioKeyCode, 
                            UInt16&                 ioCharCode, 
                            EventModifiers&         ioModifiers, 
                            bool&                   outPassUp);
    virtual bool        GetProperty(
                            DataBrowserItemData&    inItemData,
                            DataBrowserPropertyID   inProperty) const;
    virtual void        CustomDraw(
                            DataBrowserPropertyID   inProperty, 
                            DataBrowserItemState    inItemState, 
                            const Rect&             inRect, 
                            SInt16                  inDepth, 
                            Boolean                 inColorDevice) const;
    virtual bool        CustomEdit(
                            DataBrowserPropertyID   inProperty, 
                            const String&           inString, 
                            Rect&                   outMaxEditTextRect, 
                            bool&                   outShrinkToFit) const;
    virtual bool        CustomHitTest(
                            DataBrowserPropertyID   inProperty, 
                            const Rect&             inRect, 
                            const Rect&             inMouseRect) const;
    virtual DataBrowserTrackingResult
                        CustomTrack(
                            DataBrowserPropertyID   inProperty, 
                            const Rect&             inRect, 
                            const Point&            inStartPt, 
                            EventModifiers          inModifiers) const;
    virtual void        CustomDragRgn(
                            DataBrowserPropertyID   inProperty, 
                            const Rect&             inRect, 
                            RgnHandle               ioDragRgn) const;
    virtual bool        CustomAcceptDrag(
                            DataBrowserPropertyID   inProperty, 
                            const Rect&             inRect, 
                            DragRef                 inDragRef) const;
    virtual bool        CustomReceiveDrag(
                            DataBrowserPropertyID   inProperty, 
                            DataBrowserDragFlags    inDragFlags, 
                            DragRef                 inDragRef) const;
    
protected:
    
    // modifiers
    void                SetParent(
                            DataBrowserItem*        inParentItem);
    virtual void        SetBrowser(
                            DataBrowser*            inBrowser);
    
    // member variables
    DataBrowser*        mDataBrowser;
    
private:
                        
    // member variables
    DataBrowserItem*    mParentItem;

    // friends
    friend class        DataBrowser;
};


// ------------------------------------------------------------------------------------------
inline DataBrowserItem*
DataBrowserItem::GetParent() const
{
    return (mParentItem);
}

// ------------------------------------------------------------------------------------------
inline void
DataBrowserItem::SetParent(DataBrowserItem* inParentItem)
{
    mParentItem = inParentItem;
}

}   // namespace B


#endif  // BDataBrowserItem_H_
