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
#include "BrowserItem.h"

// standard headers
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

// system headers
#include <CoreFoundation/CFDate.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFNumber.h>

#include <boost/bind.hpp>

// B headers
#include "BAbstractDocument.h"
#include "BAEWriter.h"
#include "BAlert.h"
#include "BBundle.h"
#include "BDataBrowser.h"
#include "BDataBrowserItemData.h"
#include "BDrag.h"
#include "BPasteboardStream.h"
#include "BRect.h"
#include "BStringFormatter.h"
#include "BTextFilters.h"
#include "BUndo.h"
#include "CFUtils.h"

// project headers
#include "Browser.h"


namespace {
    
    enum {
        kNoClassItem = 0,
        kStringClassItem,
        kDictionaryClassItem,
        kArrayClassItem,
        kIntegerClassItem,
        kRealClassItem,
        kBooleanClassItem,
        kDateClassItem,
        kDataClassItem
    };


    ModelItem::EClass
    MenuItemToClass(MenuItemIndex inIndex)
    {
        ModelItem::EClass   theClass;

        switch (inIndex)
        {
        case kStringClassItem:      theClass = ModelItem::class_String;     break;
        case kDictionaryClassItem:  theClass = ModelItem::class_Dictionary; break;
        case kArrayClassItem:       theClass = ModelItem::class_Array;      break;
        case kIntegerClassItem:     theClass = ModelItem::class_Integer;    break;
        case kRealClassItem:        theClass = ModelItem::class_Real;       break;
        case kBooleanClassItem:     theClass = ModelItem::class_Boolean;    break;
        case kDateClassItem:        theClass = ModelItem::class_Date;       break;
        case kDataClassItem:        theClass = ModelItem::class_Data;       break;
        default:                    theClass = ModelItem::class_String;     break;
        }

        return (theClass);
    }
    
}   // anonymous namespace


// ==========================================================================================
//  BrowserItem

B::AutoUTI  BrowserItem::sItemPointerUTI('MItP', NULL);
B::AutoUTI  BrowserItem::sInternalUTI('MItm', NULL);
B::AutoUTI  BrowserItem::sPropertyListUTI('plst', NULL);

// ------------------------------------------------------------------------------------------
BrowserItem::BrowserItem(
    ModelItemPtr        inValue, 
    B::OSPtr<MenuRef>   inClassMenu)
        : mValue(inValue), mContainer(inValue->IsContainer()), 
          mClassMenu(inClassMenu)
{
    // Register for signals
    
    inValue->GetNameChangedSignal().connect(
                boost::bind(&BrowserItem::ModelItemNameChanged, this));
    
    inValue->GetClassChangingSignal().connect(
                boost::bind(&BrowserItem::ModelItemClassChanging, this));
    
    inValue->GetClassChangedSignal().connect(
                boost::bind(&BrowserItem::ModelItemClassChanged, this));
    
    inValue->GetValueChangedSignal().connect(
                boost::bind(&BrowserItem::ModelItemValueChanged, this));
    
    inValue->GetDeletedSignal().connect(
                boost::bind(&BrowserItem::ModelItemDeleted, this));
    
    inValue->GetNewChildSignal().connect(
                boost::bind(&BrowserItem::ModelItemNewChild, this, _1));
    
    inValue->GetSelectSignal().connect(
                boost::bind(&BrowserItem::ModelItemSelect, this));
    
    CalcValidator();
    CalcValueString();
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::NewChild()
{
    mValue->SendCreateElementAppleEvent(mValue->GetClassID(), kAEEnd, mValue);
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::NewSibling()
{
    mValue->SendCreateElementAppleEvent(mValue->GetClassID(), kAEAfter, mValue);
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::IsRoot() const
{
    return (mValue->IsRoot());
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ValueChanged()
{
    CalcValueString();
    B::DataBrowserItem::Update(kValueProperty);
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function to determine if the item is 
    editable.  Override it if you want editable items.
*/
bool
BrowserItem::IsEditable(
    DataBrowserPropertyID   inProperty) const
{
    bool    editable    = false;
    
    switch (inProperty)
    {
    case kNameProperty:
        // Only elements of dictionaries may have their name edited.
        editable = mValue->IsParentDictionary();
        break;
        
    case kClassProperty:
        // Every item's class may be edited.
        editable = true;
        break;
        
    case kValueProperty:
        // The values of non-containers may be edited.
        editable = !IsContainer();
        break;
    }
    
    return (editable);
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function to determine if the item is 
    a container.  Override it if you want to implement containers.
*/
bool
BrowserItem::IsContainer() const
{
    return (mContainer);
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function to determine if the item is 
    a container and is openable.  Override it if you want to implement 
    openable containers.
*/
bool
BrowserItem::IsContainerOpenable() const
{
    // All containers are openable.
    
    return (IsContainer());
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function to determine if the item is 
    a container and is closeable.  Override it if you want to implement 
    closeable containers.
*/
bool
BrowserItem::IsContainerClosable() const
{
    // All containers are closable.
    
    return (IsContainer());
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function to determine if the item is 
    a container and is sortable.  Override it if you want to implement 
    sortable containers.
*/
bool
BrowserItem::IsContainerSortable() const
{
    // All containers are sortable.
    
    return (IsContainer());
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function when it has opened the item 
    (which is assumed to be a container).  Override it to perform any special 
    processing.  For example, a derived class that implements container 
    hierarchies will probably want to populate the container from within an 
    override of this function.
*/
void
BrowserItem::ContainerOpened()
{
    // Populate the browser with this container's contents.
    
    std::vector<ModelItemPtr>           modelItems;
    B::DataBrowser::ItemVector          dbItems;
    std::vector<ModelItemPtr>::iterator it;
    
    mValue->GetChildren(modelItems);
    dbItems.reserve(modelItems.size());
    
    if (mValue->IsDictionary())
    {
        // Dictionary
        
        for (it = modelItems.begin(); it != modelItems.end(); ++it)
        {
            dbItems.push_back(new BrowserItem((*it)->GetPtr(), mClassMenu));
        }
    }
    else if (mValue->IsArray())
    {
        // Array
        
        for (it = modelItems.begin(); it != modelItems.end(); ++it)
        {
            dbItems.push_back(new BrowserItem((*it)->GetPtr(), mClassMenu));
        }
    }
    
    mDataBrowser->AddItems(this, dbItems);
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function when it wants to display one 
    column's worth of data for one item.  You need to override this member 
    function in order to supply the data for your column properties.
*/
bool
BrowserItem::GetData(
    B::DataBrowserItemData&     inItemData,
    DataBrowserPropertyID       inProperty) const
{
    bool    handled = true;
    
    switch (inProperty)
    {
    case kNameProperty:
        if (mValue->IsRoot())
        {
            // Root item
            inItemData.SetText(BLocalizedString(
                                    CFSTR("Root"), 
                                    "The name of the topmost element in a property list browser"));
        }
        else
        {
            // Child of Dictionary or Array
            inItemData.SetText(mValue->GetName());
        }
        break;
        
    case kClassProperty:
        inItemData.SetMenu(mClassMenu);
        inItemData.SetValue(TypeIDToMenuItem());
        break;
        
    default:
        inItemData.SetText(mValueString);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function when it wants to modify one 
    column's worth of data for one item.  You need to override this member 
    function in order to fetch the data for your column properties.
*/
bool
BrowserItem::SetData(
    B::DataBrowserItemData&     inItemData,
    DataBrowserPropertyID       inProperty)
{
    bool    handled = true;
    int     menuItem;
    
    try
    {
        switch (inProperty)
        {
        case kNameProperty:
            Rename(inItemData.GetText());
            break;
            
        case kClassProperty:
            {
                B::AutoUndo autoUndo(BLocalizedString(
                                        CFSTR("Class Change"), 
                                        "Undo text for property list item class change."), 
                                     GetUndoTarget());
                
//              SetUpAction(B__LocalizedString(
//                              CFSTR("Class Change"), 
//                              "Undo text for property list item class change."));
                
                menuItem = inItemData.GetValue();
                
                if (menuItem > 0)
                {
                    ModelItem::EClass   itemClass;
                    
                    itemClass = MenuItemToClass(menuItem);
                    
                    if (itemClass != mValue->GetValueClass())
                        mValue->SendChangeClassAppleEvent(itemClass);
                }
                
                autoUndo.Commit();
            }
            break;
            
        case kValueProperty:
            {
                B::AutoUndo autoUndo(BLocalizedString(
                                        CFSTR("Value Change"), 
                                        "Undo text for property list item value change."), 
                                     GetUndoTarget());
                
//              SetUpAction(B__LocalizedString(
//                              CFSTR("Value Change"), 
//                              "Undo text for property list item value change."));
                
                ParseValueString(inItemData.GetText());
                
                autoUndo.Commit();
            }
            break;
            
        default:
            handled = false;
            break;
        }
    }
    catch (std::exception& ex)
    {
#if 0
        B::String   errorStr, explanationStr, formatStr, infoStr;
        
        switch (B::Exception::get_status(ex))
        {
        case MoveIntoOffspringException::kMoveIntoOffspringError:
            errorStr        = BLocalizedString(
                                    CFSTR("Moving into children"), 
                                    "The title of the 'move into offspring' alert.");
            explanationStr  = BLocalizedString(
                                    CFSTR("An item can't be moved into one of its children."), 
                                    "The message in the 'move into offspring' alert.");
            break;
            
        default:
            errorStr        = BLocalizedString(
                                    CFSTR("Can't complete the requested operation"), 
                                    "The title of the 'generic item set-data' alert.");
            formatStr       = BLocalizedString(
                                    CFSTR("Message:  %1%"), 
                                    "The message in the 'generic item set-data' alert.");
            explanationStr.assign(B::StringFormatter(formatStr) % B::Exception::get_message(ex));
            break;
        }
        
//      B::StopAlert    alert(errorStr, explanationStr);
//      
//      alert.Run();
        
        B::AlertSheet<B::StopAlert>*    alert = new B::AlertSheet<B::StopAlert>(GetControlOwner(*mDataBrowser));
        
        alert->SetError(errorStr);
        alert->SetExplanation(explanationStr);
        alert->Display();
#endif
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! The data browser calls this member function when it wants to compare two 
    items (in order to sort them).  inProperty is the property (i.e., column) 
    on which to perform the comparison.  You need to override this member 
    function in your derived class if you want to implement sorted columns.
    The function should return a value greater than, equal to, or less than 
    zero, depending on whether the item compares greater than, equal to, or 
    less than inItem.
*/
int
BrowserItem::CompareTo(
    const DataBrowserItem&  inItem,
    DataBrowserPropertyID   inProperty) const
{
    const OptionBits    kCompareFlags   = kCFCompareNonliteral | 
                                          kCFCompareLocalized;
    const BrowserItem&  otherItem   = dynamic_cast<const BrowserItem&>(inItem);
    int                 result      = 0;
    
    switch (inProperty)
    {
    case kNameProperty:
        if (mValue->IsParentDictionary())
        {
            result = CFStringCompare(mValue->GetName().cf_ref(), 
                                     otherItem.mValue->GetName().cf_ref(), 
                                     kCompareFlags);
        }
        else if (mValue->IsParentArray())
        {
            unsigned    thisIndex   = mValue->GetIndex();
            unsigned    otherIndex  = otherItem.mValue->GetIndex();
            
            if (thisIndex < otherIndex)
                result = -1;
            else if (thisIndex > otherIndex)
                result = 1;
        }
        break;
        
    case kClassProperty:
        result = CFStringCompare(TypeIDToMenuItemText().cf_ref(), 
                                 otherItem.TypeIDToMenuItemText().cf_ref(), 
                                 kCompareFlags);
        break;
        
    case kValueProperty:
        result = CFStringCompare(mValueString.cf_ref(), 
                                 otherItem.mValueString.cf_ref(), 
                                 kCompareFlags);
        break;
    }
    
    return (result);
}

// ------------------------------------------------------------------------------------------
/*! This item is being dragged.
    Add this item's flavor data (or promised flavors) to the drag reference.
*/
bool
BrowserItem::AddDragFlavors(
    B::Drag&            ioDrag,
    PasteboardItemID&   outItemID) const
{
    bool    addedItem   = true;
    
    for (B::DataBrowserItem* parent = GetParent(); 
         (parent != NULL); 
         parent = parent->GetParent())
    {
        if (parent->IsSelected())
        {
            addedItem = false;
            break;
        }
    }
    
    if (addedItem)
    {
        if (ioDrag.GetPasteboard().size() == 0)
        {
            // This is the first item, so set some drag-wide attributes.
            
            const DragActions   kDragActions    = kDragActionCopy 
                                                | kDragActionMove 
                                                | kDragActionDelete;
            
            ioDrag.SetAllowableActions(kDragActions, kDragActions);
        }
        
        outItemID = WriteFlavors(ioDrag.GetPasteboard());
    }
    
    return (addedItem);
}

// ------------------------------------------------------------------------------------------
/*! Returns whether a given drag to this item is acceptable.
    This will be false unless the item is a container.
*/
bool
BrowserItem::IsDragAcceptable(
    const B::Drag&      inDrag) const
{
    DragActions dragAction  = kDragActionNothing;
    bool        needCursor  = true;
    bool        itemsSupportMove;
    ThemeCursor cursor;
    OSStatus    err;
    
    WriteDropLocation(inDrag);
    
    if (AreAllDragItemsAcceptable(inDrag, mValue, itemsSupportMove))
    {
        dragAction = DetermineDragAction(inDrag, itemsSupportMove, false);
    }
    
    switch (dragAction)
    {
    case kDragActionCopy:       cursor = kThemeCopyArrowCursor;     break;
    case kDragActionDelete:     cursor = kThemeArrowCursor;         break;
    case kDragActionMove:       cursor = kThemeArrowCursor;         break;
    case kDragActionNothing:    cursor = kThemeNotAllowedCursor;    break;
    
    default:
        needCursor = false;
        break;
    }
    
//  std::cerr << "drag accept " << dragAction << " " << GetState() << " " << mValue->GetName() << std::endl;;
    
    if (needCursor)
    {
        err = SetThemeCursor(cursor);
        B_THROW_IF_STATUS(err);
    }
    
//  return (dragAction != kDragActionNothing);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::AreAllDragItemsAcceptable(
    const B::Drag&  inDrag, 
    ModelItemPtr    inModelItem, 
    bool&           outItemsSupportMove)
{
    bool    allItemsAreAcceptable   = false;
    
    if (inModelItem->IsContainer())
    {
        if (!inDrag.IsInsideSender())
        {
            outItemsSupportMove = false;
        }
        
        const B::Pasteboard&    pasteboard  = inDrag.GetPasteboard();
        size_t                  itemCount   = pasteboard.size();
        
        if (itemCount > 0)
        {
            allItemsAreAcceptable = true;
            
            for (size_t i = 0; i < itemCount; i++)
            {
                B::PasteboardItem   pbItem              = pasteboard[i];
                bool                itemIsAcceptable    = false;
                
                if (outItemsSupportMove)
                {
                    ModelItemPtr    modelItem   = ReadItem(pbItem);
                    
                    itemIsAcceptable = (modelItem.get() != NULL);
                    
                    if (!itemIsAcceptable || !modelItem->IsInSameContainerAs(*inModelItem))
                    {
                        outItemsSupportMove = false;
                    }
                }
                
                if (!itemIsAcceptable)
                {
                    itemIsAcceptable = pbItem.Contains(sPropertyListUTI.get());
                }
                
                if (!itemIsAcceptable)
                    allItemsAreAcceptable = itemIsAcceptable;
            }
        }
    }
    
    return (allItemsAreAcceptable);
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::ReceiveDrag(
    const B::Drag&  inDrag)
{
    DragActions dragAction  = kDragActionNothing;
    bool        itemsSupportMove;
    
    if (AreAllDragItemsAcceptable(inDrag, mValue, itemsSupportMove))
    {
        dragAction = DetermineDragAction(inDrag, itemsSupportMove, true);
        
//      if (dragAction != kDragActionNothing)
//      {
//          AEDesc      dropLocation;
//          B::AEWriter writer;
//          
//          mValue->MakeSpecifier(writer);
//          writer.Close(dropLocation);
//          
//          err = SetDropLocation(inDragRef, &dropLocation);
//          B_THROW_IF_STATUS(err);
            
            if (dragAction == kDragActionCopy)
            {
                // Copy the dragged items into ourselves.
                
                CopyDraggedItems(inDrag);
            }
//      }
    }
    
//  std::cerr << "drag receive " << dragAction << " " << GetState() << " " << mValue->GetName() << std::endl;;
    
    return (dragAction != kDragActionNothing);
}

// ------------------------------------------------------------------------------------------
DragActions
BrowserItem::DetermineDragAction(
    const B::Drag&  inDrag, 
    bool            inItemsSupportMove, 
    bool            inReceiving) const
{
    // Determine which drag operation is being performed.  The decision 
    // is as follows:
    //  
    //  - If the drag originated in a different application, then it's a copy.
    //  - Else, if the drag originated in a different document, then it's a copy.
    //  - Else, if the user has the option key held down, then it's a copy.
    //  - Else, it's a move.
    
    DragActions dragAction  = kDragActionCopy;
    
    if (inDrag.IsInsideSender() && inItemsSupportMove)
    {
        if (!((inReceiving ? inDrag.GetMouseUpModifiers() : inDrag.GetModifiers()) & optionKey))
        {
            dragAction = kDragActionMove;
        }
    }
    
    DragActions allowableActions    = inDrag.GetAllowableActions();
    
    if ((dragAction == kDragActionMove) && !(allowableActions & kDragActionMove))
        dragAction = kDragActionCopy;
    
    if ((dragAction == kDragActionCopy) && !(allowableActions & kDragActionCopy))
        dragAction = kDragActionNothing;
    
    inDrag.SetDropAction(dragAction);
    
    return (dragAction);
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::CopyDraggedItems(
    const B::Drag&  inDrag)
{
    B_ASSERT(IsContainer());
    
    B::AutoUndo autoUndo(BLocalizedString(
                            CFSTR("Duplicate"), 
                            "Undo text for property list item duplication."), 
                         GetUndoTarget());
    
//  SetUpAction(B__LocalizedString(
//                  CFSTR("Duplicate"), 
//                  "Undo text for property list item duplication."));
    
    const B::Pasteboard&    pasteboard  = inDrag.GetPasteboard();
    size_t                  itemCount   = pasteboard.size();
    
    for (size_t i = 0; i < itemCount; i++)
    {
        B::PasteboardItem   pbItem(pasteboard[i]);
        ModelItemPtr            modelItem   = ReadItem(pbItem);
        
        if (modelItem.get() != NULL)
        {
            modelItem->SendCloneAppleEvent(kAEEnd, mValue);
        }
        else
        {
//          istr.clear();
//          istr.open('plst');
//          
//          if (istr.good())
//          {
//              B::OSPtr<CFTypeRef>     valuePtr;
//              B::OSPtr<CFStringRef>   namePtr;
//              
//              valuePtr = ModelItem::ReadFromStream(istr, namePtr);
//              istr.close();
//              
//              if (valuePtr != NULL)
//              {
//                  if (!mValue->IsParentDictionary())
//                      namePtr = B::OSPtr<CFStringRef>();
//                  
//                  mValue->SendCreateItemAppleEvent(insertPosition, 
//                                                   valuePtr, namePtr);
//              }
//          }
        }
    }
    
    autoUndo.Commit();
}


#if 0
// ------------------------------------------------------------------------------------------
void
BrowserItem::CopyDraggedItemsForXMLFlavor(
    DragRef             inDragRef)
{
    UInt16      itemCount;
    OSStatus    err;
    
    err = CountDragItems(inDragRef, &itemCount);
    B_THROW_IF_STATUS(err);
    
    for (UInt16 index = 1; index <= itemCount; index++)
    {
        DragItemRef dragItem;
        
        err = GetDragItemReferenceNumber(inDragRef, index, &dragItem);
        B_THROW_IF_STATUS(err);
        
        B::DragInputStream  istr(inDragRef, dragItem);
        B::OSPtr<CFTypeRef> valuePtr;
        B::String           name;
        
        istr.open('plst');
        valuePtr = ModelItem::ReadFromStream(istr, name);
        istr.close();
        
//      data.item->SendCloneItemAppleEvent(kAEEnd, mValue);
    }
}
#endif

// ------------------------------------------------------------------------------------------
void
BrowserItem::WriteDropLocation(const B::Drag& inDrag) const
{
    B::AEDescriptor dropLocation;
    B::AEWriter     writer;
    
    mValue->MakeSpecifier(writer);
    writer.Close(dropLocation);
    
    inDrag.SetDropLocation(dropLocation);
}

// ------------------------------------------------------------------------------------------
ModelItem*
BrowserItem::ReadDropLocation(const B::Drag& inDrag)
{
    ModelItem*      modelItem   = NULL;
    B::AEObjectPtr  modelObject;
    B::AEDescriptor dropLocation;
    
    inDrag.GetDropLocation(dropLocation);
    
    modelObject = B::AEObject::Resolve(dropLocation, std::nothrow);
    
    if (modelObject.get() != NULL)
    {
        modelItem = dynamic_cast<ModelItem*>(modelObject.get());
    }
    
    return (modelItem);
}

// ------------------------------------------------------------------------------------------
PasteboardItemID
BrowserItem::WriteFlavors(B::Pasteboard& ioPasteboard) const
{
    B::PasteboardItem           pbItem(ioPasteboard.NewItem(const_cast<BrowserItem*>(this)));
    B::PasteboardOutputStream   ostr;
    
    // Set up stream to throw exceptions.
    ostr.clear();
    ostr.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
    
    // Write the pointer flavor
    ostr.open(pbItem, sItemPointerUTI.get());
    ostr.flavor().SetSaved(false);
#ifndef NDEBUG
//  ostr.flavor().SetSenderOnly(true);
#endif
    mValue->WriteItemPointerFlavor(ostr);
    ostr.close();
    
    // Write the internal flavor.
    ostr.open(pbItem, sInternalUTI.get());
#ifndef NDEBUG
//  ostr.flavor().SetSenderOnly(true);
#endif
    mValue->WriteInternalFlavor(ostr);
    ostr.close();

    // Write the property list (XML) flavor.
    ostr.open(pbItem, sPropertyListUTI.get());
    mValue->WritePropertyListFlavor(ostr);
    ostr.close();
    
    // Write the text flavor.
#ifndef NDEBUG
    ostr.open(pbItem, kUTTypePlainText);
    mValue->WritePropertyListFlavor(ostr);
    ostr.close();
#endif
    
    return (pbItem.GetItemID());
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::GetSupportedServiceFlavors(
    B::MutableArray<CFStringRef>&       ioCopyTypes, 
    B::MutableArray<CFStringRef>&       /* ioPasteTypes */)
{
    ioCopyTypes.push_back(sItemPointerUTI.get());
    ioCopyTypes.push_back(sInternalUTI.get());
    ioCopyTypes.push_back(sPropertyListUTI.get());
#ifndef NDEBUG
    ioCopyTypes.push_back(kUTTypePlainText);
#endif
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
BrowserItem::ReadItem(B::PasteboardItem& pbItem)
{
    ModelItemPtr                modelItem;
    B::PasteboardInputStream    istr(pbItem, sItemPointerUTI.get());
    
    if (istr.good())
    {
        modelItem = ModelItem::ReadItemPointerFlavor(istr);
    }
    
    return (modelItem);
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::SupplyHelpContent(
    DataBrowserPropertyID       inProperty,
    B::String&                  outMinContent,
    bool&                       outMinContentIsLocalized,
    B::String&                  outMaxContent,
    bool&                       outMaxContentIsLocalized)
{
    CFStringRef propertyName    = NULL;
    
    switch (inProperty)
    {
    case kNameProperty:
        propertyName = CFSTR("Name");
        break;
        
    case kClassProperty:
        propertyName = CFSTR("Class");
        break;
        
    case kValueProperty:
        propertyName = CFSTR("Value");
        break;
    }
    
    B::OSPtr<CFStringRef>   smallTag(CFStringCreateWithFormat(NULL, NULL, 
                                    CFSTR("Small help tag for %1$@ property of object %2$p"), 
                                    propertyName, this), 
                                 B::from_copy);
    B::OSPtr<CFStringRef>   bigTag(CFStringCreateWithFormat(NULL, NULL, 
                                    CFSTR("Big help tag for %1$@ property of object %2$p"), 
                                    propertyName, this), 
                                B::from_copy);
    
    outMinContent   = smallTag;
    outMaxContent   = bigTag;
    
    outMinContentIsLocalized = outMaxContentIsLocalized = false;
    
    return (true);
}


#if 0
// ------------------------------------------------------------------------------------------
void
BrowserItem::CustomDraw(
    DataBrowserPropertyID   inProperty, 
    DataBrowserItemState    inItemState, 
    const B::Rect&          inRect, 
    SInt16                  inDepth, 
    Boolean                 inColorDevice)
    const
{
    if (inProperty != kValueProperty)
        return;
    
    if (mValueString.empty())
        return;
    
    B::String       drawStr(mValueString);
    ThemeFontID     font;
    ThemeBrush      brush;
    ThemeDrawState  drawState;
    B::Size         contentSize;
    B::Rect         contentRect;
    OSStatus        err;
    
    GetDrawState(font, brush, drawState);
    CalcStringContentSize(inRect, drawStr, font, drawState, truncEnd, contentSize);
    CalcContentRect(inProperty, inRect, contentSize, contentRect);
    
    if (inItemState & kDataBrowserItemIsSelected)
    {
        // The item is selected, so we need to draw the highlight 
        // color in our content area.
        
        B::Rect             selRect(contentRect);
        ThemeDrawingState   state;
        
        selRect.inset(-2.0f, 0.0f);
        
        err = GetThemeDrawingState(&state);
        B_THROW_IF_STATUS(err);
        
        err = SetThemeBackground(brush, inDepth, inColorDevice);
        B_THROW_IF_STATUS(err);
        
        EraseRect(B::Rect::GetQDRect(selRect));
        
        err = SetThemeDrawingState(state, true);
        B_THROW_IF_STATUS(err);
    }
    
    err = DrawThemeTextBox(drawStr.cf_ref(), font, drawState, false, 
                           B::Rect::GetQDRect(contentRect), 
                           teFlushDefault, NULL);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::CustomHitTest(
    DataBrowserPropertyID   inProperty, 
    const B::Rect&          inRect, 
    const B::Rect&          inMouseRect)
    const
{
    if (inProperty != kValueProperty)
        return (false);
    
    B::Rect contentRect;
    bool    contentHit;
    
    if (!mValueString.empty())
    {
        B::String       drawStr(mValueString);
        ThemeFontID     font;
        ThemeBrush      brush;
        ThemeDrawState  drawState;
        B::Size         contentSize;
        
        GetDrawState(font, brush, drawState);
        CalcStringContentSize(inRect, drawStr, font, drawState, truncEnd, contentSize);
        CalcContentRect(inProperty, inRect, contentSize, contentRect);
    }
    else
    {
        contentRect = inRect;
    }
    
    if (inMouseRect.empty())
    {
        contentHit = contentRect.contains(inMouseRect.origin);
    }
    else
    {
        contentHit = CGRectIntersectsRect(contentRect, inMouseRect);
    }
    
    printf("CustomHitTest %d\n", contentHit ? 1 : 0);
    fflush(stdout);
    
    return (contentHit);
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::CustomDragRgn(
    DataBrowserPropertyID   inProperty, 
    const B::Rect&          inRect, 
    RgnHandle               ioDragRgn)
    const
{
    if (inProperty != kValueProperty)
        return;
    
    if (mValueString.empty())
        return;
    
    B::String       drawStr(mValueString);
    ThemeFontID     font;
    ThemeBrush      brush;
    ThemeDrawState  drawState;
    B::Size         contentSize;
    B::Rect         contentRect;
    
    GetDrawState(font, brush, drawState);
    CalcStringContentSize(inRect, drawStr, font, drawState, truncEnd, contentSize);
    CalcContentRect(inProperty, inRect, contentSize, contentRect);
    
    RectRgn(ioDragRgn, B::Rect::GetQDRect(contentRect));
}
#endif

// ------------------------------------------------------------------------------------------
void
BrowserItem::CalcStringContentSize(
    const B::Rect&          inRect, 
    B::String&              ioString, 
    ThemeFontID             inFont, 
    ThemeDrawState          inDrawState, 
    TruncCode               inTruncCode, 
    B::Size&                outContentSize)
{
    ::Point     qdSize;
    SInt16      baseline;
    OSStatus    err;
    
    err = GetThemeTextDimensions(ioString.cf_ref(), inFont, inDrawState, 
                                 false, &qdSize, &baseline);
    B_THROW_IF_STATUS(err);
    
    if (inRect.size.width < qdSize.h)
    {
        // The text doesn't fit in the alloted space, so we need 
        // to truncate it.
        
        B::MutableString    truncStr(ioString);
        Boolean             truncated;
        
        err = TruncateThemeText(truncStr.cf_ref(), inFont, inDrawState, 
                                static_cast<short>(inRect.size.width), 
                                inTruncCode, &truncated);
        B_THROW_IF_STATUS(err);
        
        ioString = truncStr;
        
        // Re-measure it, because the truncated text might not 
        // exactly fill the maximum width we specified.
        
        err = GetThemeTextDimensions(ioString.cf_ref(), inFont, inDrawState, 
                                     false, &qdSize, &baseline);
        B_THROW_IF_STATUS(err);
    }
    
    outContentSize = qdSize;
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::CalcContentRect(
    DataBrowserPropertyID   /* inPropertyID */, 
    const B::Rect&          inRect, 
    const B::Size&          inContentSize, 
    B::Rect&                outContentRect) const
{
    // Determine the horizontal alignment of the cell's content.  The 
    // alignment comes from the column's header.
    
    DataBrowserListViewHeaderDesc   headerDesc;
    SInt16                          just;
    
    BlockZero(&headerDesc, sizeof(headerDesc));
    headerDesc.version = kDataBrowserListViewLatestHeaderDesc;
    
//  err = GetDataBrowserListViewHeaderDesc(*mDataBrowser, inPropertyID, 
//                                         &headerDesc);
//  B_THROW_IF_STATUS(err);
    
    if (headerDesc.btnFontStyle.flags & kControlUseJustMask)
        just = headerDesc.btnFontStyle.just;
    else
        just = teFlushDefault;
    
    if (just == teFlushDefault)
        just = GetSysDirection() ? teFlushRight : teFlushLeft;
    
    outContentRect.size = inContentSize;
    
    switch (just)
    {
    case teFlushLeft:
    default:
        outContentRect.origin.x = inRect.minx();
        break;
        
    case teFlushRight:
        outContentRect.origin.x = inRect.maxx() - inContentSize.width;
        break;
        
    case teCenter:
        outContentRect.origin.x = inRect.minx() + (inRect.width() - inContentSize.width) / 2.0f;
        break;
    }
    
    outContentRect.origin.y = inRect.miny() + (inRect.height() - inContentSize.height) / 2.0f;
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::GetDrawState(
    ThemeFontID&    outFont, 
    ThemeBrush&     outBrush, 
    ThemeDrawState& outState) const
{
    outFont = kThemeCurrentPortFont;
    
    if (mDataBrowser->IsActive())
    {
        outBrush    = kThemeBrushPrimaryHighlightColor;
        outState    = kThemeStateActive;
    }
    else
    {
        outBrush    = kThemeBrushSecondaryHighlightColor;
        outState    = kThemeStateInactive;
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::CalcValueString() const
{
    switch (mValue->GetValueClass())
    {
    case ModelItem::class_String:
        // String
        mValueString.assign(mValue->GetValueString());
        break;
        
    case ModelItem::class_Data:
        // Data (blob)
        mValueString.assign(GetDataFilter().Format(mValue->GetValueData()));
        break;
        
    case ModelItem::class_Integer:
        // Integer
        {
            SInt64                  value   = mValue->GetValueInteger();
            B::OSPtr<CFNumberRef>   number(CFNumberCreate(NULL, kCFNumberSInt64Type, &value), B::from_copy);
            B::TextFilters::Number  filter  = GetIntegerFilter();
            
            mValueString.assign(filter.Format(number));
        }
        break;
        
    case ModelItem::class_Real:
        // Real
        mValueString.assign(GetRealFilter().Format(mValue->GetValueReal()));
        break;
        
    case ModelItem::class_Boolean:
        // Boolean
        mValueString.assign(GetBooleanFilter().Format(mValue->GetValueBoolean()));
        break;
        
    case ModelItem::class_Date:
        // Date
        mValueString.assign(GetDateFilter().Format(mValue->GetValueDate()));
        break;
        
    case ModelItem::class_Array:
        // Array
        {
            CFIndex     count   = mValue->CountChildren();
            B::String   format;
            
            if (count == 1)
            {
                format = BLocalizedString(
                                CFSTR("%1% ordered object"), 
                                "The description of an array in a property list browser, singular form");
            }
            else
            {
                format = BLocalizedString(
                                CFSTR("%1% ordered objects"), 
                                "The description of an array in a property list browser, plural form");
            }
            
            mValueString = B::Extract(B::StringFormatter(format) % count);
        }
        break;
        
    case ModelItem::class_Dictionary:
        // Dictionary
        {
            CFIndex     count   = mValue->CountChildren();
            B::String   format;
            
            if (count == 1)
            {
                format = BLocalizedString(
                                CFSTR("%1% key/value pair"), 
                                "The description of a dictionary in a property list browser, singular form");
            }
            else
            {
                format = BLocalizedString(
                                CFSTR("%1% key/value pairs"), 
                                "The description of a dictionary in a property list browser, plural form");
            }
            
            mValueString = B::Extract(B::StringFormatter(format) % count);
        }
        break;
        
    default:
        // Catch-all
        mValueString.clear();
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::CalcValidator()
{
    mValidator.Clear();
    
    switch (mValue->GetValueClass())
    {
    case ModelItem::class_Data:
        // Data (blob)
        mValidator.Connect(GetDataFilter());
        break;
        
    case ModelItem::class_Integer:
        // Integer
        mValidator.Connect(GetIntegerFilter());
        break;
        
    case ModelItem::class_Real:
        // Real
        mValidator.Connect(GetRealFilter());
        break;
        
    case ModelItem::class_Boolean:
        // Boolean
        mValidator.Connect(GetBooleanFilter());
        break;
        
    case ModelItem::class_Date:
        // Date
        mValidator.Connect(GetDateFilter());
        break;
        
    case ModelItem::class_String:
    case ModelItem::class_Array:
    case ModelItem::class_Dictionary:
    default:
        // No special validation in these cases.
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ParseValueString(const B::String& inText)
{
    BrowserItemTextSource   textSource(inText);
    
    mValidator.Validate(textSource);
    
    switch (mValue->GetValueClass())
    {
    case ModelItem::class_String:
        // String
        mValue->SendSetPropertyAppleEvent<typeUTF8Text>(
                        ModelItem::pModelItemValueProperty, inText);
        break;
        
    case ModelItem::class_Data:
        // Data (blob)
        {
            B::OSPtr<CFDataRef> data(GetDataFilter().Parse(inText));
            
            mValue->SendSetPropertyAppleEvent<typeData>(
                            ModelItem::pModelItemValueProperty, data);
        }
//      valueStr.Adopt(CFStringCreateWithFormat(NULL, NULL, CFSTR("%@"), 
//                                              mValue->GetTypedValue<CFDataRef>()), 
//                     true);
        break;
        
    case ModelItem::class_Integer:
        // Integer
        {
            B::OSPtr<CFNumberRef>   number(GetIntegerFilter().Parse(inText));
            SInt64                  value;
            
            if (CFNumberGetValue(number, kCFNumberSInt64Type, &value))
            {
                if ((value >= INT_MIN) && (value <= INT_MAX))
                {
                    SInt32  intval  = value;
                    
                    mValue->SendSetPropertyAppleEvent<typeSInt32>(
                                    ModelItem::pModelItemValueProperty, intval);
                }
                else
                {
                    mValue->SendSetPropertyAppleEvent<typeSInt64>(
                                        ModelItem::pModelItemValueProperty, value);
                }
            }
        }
        break;
        
    case ModelItem::class_Real:
        // Real
        {
            B::OSPtr<CFNumberRef>   number(GetRealFilter().Parse(inText));
            double                  value;
            
            if (CFNumberGetValue(number, kCFNumberDoubleType, &value))
            {
                mValue->SendSetPropertyAppleEvent<typeIEEE64BitFloatingPoint>(
                                    ModelItem::pModelItemValueProperty, value);
            }
        }
        break;
        
    case ModelItem::class_Boolean:
        // Boolean
        {
            bool    value   = GetBooleanFilter().Parse(inText);
            
            mValue->SendSetPropertyAppleEvent<typeBoolean>(
                                ModelItem::pModelItemValueProperty, value);
        }
        break;
        
    case ModelItem::class_Date:
        // Date
        {
            B::OSPtr<CFDateRef> date(GetDateFilter().Parse(inText));
            LongDateTime        ldt;
            OSStatus            err;
            
            err = UCConvertCFAbsoluteTimeToLongDateTime(CFDateGetAbsoluteTime(date), &ldt);
            B_THROW_IF_STATUS(err);
            
            mValue->SendSetPropertyAppleEvent<typeLongDateTime>(
                            ModelItem::pModelItemValueProperty, ldt);
        }
        break;
//      
//  case ModelItem::class_Array:
//      // Array
//      {
//          CFIndex                 count   = CFArrayGetCount(mValue->GetTypedValue<CFArrayRef>());
//          B::OSPtr<CFStringRef>   format;
//          
//          if (count == 1)
//          {
//              format.Adopt(CF__CopyLocalizedStringFromTableInBundle(
//                              CFSTR("%d ordered object"), NULL, B::Bundle::Main(), 
//                              CFSTR("The description of an array element in a property list browser, singular form")), 
//                           true);
//          }
//          else
//          {
//              format.Adopt(CF__CopyLocalizedStringFromTableInBundle(
//                              CFSTR("%d ordered objects"), NULL, B::Bundle::Main(), 
//                              CFSTR("The description of an array element in a property list browser, plural form")), 
//                           true);
//          }
//          
//          valueStr.Adopt(CFStringCreateWithFormat(NULL, NULL, format, count), true);
//      }
//      break;
//      
//  case ModelItem::class_Dictionary:
//      // Dictionary
//      {
//          CFIndex                 count   = CFDictionaryGetCount(mValue->GetTypedValue<CFDictionaryRef>());
//          B::OSPtr<CFStringRef>   format;
//          
//          if (count == 1)
//          {
//              format.Adopt(CF__CopyLocalizedStringFromTableInBundle(
//                              CFSTR("%d key/value pair"), NULL, B::Bundle::Main(), 
//                              CFSTR("The description of a dictionary element in a property list browser, singular form")), 
//                           true);
//          }
//          else
//          {
//              format.Adopt(CF__CopyLocalizedStringFromTableInBundle(
//                              CFSTR("%d key/value pairs"), NULL, B::Bundle::Main(), 
//                              CFSTR("The description of a dictionary element in a property list browser, plural form")), 
//                           true);
//          }
//          
//          valueStr.Adopt(CFStringCreateWithFormat(NULL, NULL, format, count), true);
//      }
//      break;
        
    default:
        break;
    }
}

// ------------------------------------------------------------------------------------------
MenuItemIndex
BrowserItem::TypeIDToMenuItem() const
{
    MenuItemIndex   index;
    
    switch (mValue->GetValueClass())
    {
    case ModelItem::class_String:       index = kStringClassItem;       break;
    case ModelItem::class_Dictionary:   index = kDictionaryClassItem;   break;
    case ModelItem::class_Array:        index = kArrayClassItem;        break;
    case ModelItem::class_Integer:      index = kIntegerClassItem;      break;
    case ModelItem::class_Real:         index = kRealClassItem;         break;
    case ModelItem::class_Boolean:      index = kBooleanClassItem;      break;
    case ModelItem::class_Date:         index = kDateClassItem;         break;
    case ModelItem::class_Data:         index = kDataClassItem;         break;
    default:                            index = kNoClassItem;           break;
    }
    
    return (index);
}

// ------------------------------------------------------------------------------------------
B::String
BrowserItem::TypeIDToMenuItemText() const
{
    MenuItemIndex   index   = TypeIDToMenuItem();
    CFStringRef     cfstr;
    OSStatus        err;
    
    err = CopyMenuItemTextAsCFString(mClassMenu, index, &cfstr);
    
    return (B::String(cfstr, B::from_copy));
}

//// ------------------------------------------------------------------------------------------
//void
//BrowserItem::SetUpAction(const B::String& inActionName) const
//{
//  mValue->GetDocument()->GetUndoManager()->SetActionName(inActionName);
//}

// ------------------------------------------------------------------------------------------
void
BrowserItem::Rename(const B::String& inNewName)
{
    try
    {
        B::AutoUndo autoUndo(BLocalizedString(
                                CFSTR("Rename"), 
                                "Undo text for property list item name change."), 
                             GetUndoTarget());
        
//      SetUpAction(B__LocalizedString(
//                      CFSTR("Rename"), 
//                      "Undo text for property list item name change."));
        
        mValue->SendSetPropertyAppleEvent<typeUTF8Text>(pName, inNewName);
        
        autoUndo.Commit();
    }
    catch (const DuplicateItemException& ex)
    {
        B::Alert::Start<B::Alert>(
                    BLocalizedString(
                        CFSTR("Duplicate item name"), 
                        "The title of the 'duplicate item' alert."),
                    BLocalizedString(
                        CFSTR("The key you entered is already present in the dictionary."), 
                        "The message in the 'duplicate item' alert."),
                    B::DialogModality::Sheet(GetControlOwner(*mDataBrowser)));
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemNameChanged()
{
    B::DataBrowserItem::Update(kNameProperty);
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemClassChanging()
{
    if (IsOpen())
    {
        Close();
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemClassChanged()
{
    mContainer = mValue->IsContainer();
    
    B::DataBrowserItem::Update(kNameProperty);
    B::DataBrowserItem::Update(kClassProperty);
    
    CalcValidator();
//  ValueChanged();
    
//  mDataBrowser->SelectionChanged();
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemValueChanged()
{
    ValueChanged();
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemDeleted()
{
    BrowserItem*    parentItem  = dynamic_cast<BrowserItem*>(GetParent());
    B::DataBrowser* browser     = mDataBrowser;
    bool            updParent   = (mValue->IsParentArray() && GetParent()->IsOpen());
    
    B_ASSERT(parentItem != NULL);
    
    mDataBrowser->RemoveItem(this);
    
    // Beyond this point we have been deleted, so we must avoid referencing 
    // member variables.
    
    parentItem->ValueChanged();
    
    if (updParent)
    {
        browser->UpdateChildren(parentItem, kNameProperty);
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemNewChild(ModelItemPtr childItem)
{
    B_ASSERT(IsContainer());
    B_ASSERT(childItem != NULL);
    
    if (IsOpen())
    {
        BrowserItem*    browserItem;
        
        browserItem = new BrowserItem(childItem, mClassMenu);
        
        mDataBrowser->AddItem(this, browserItem);
    }
    
    ValueChanged();
    
    if (mValue->IsArray() && IsOpen())
    {
        mDataBrowser->UpdateChildren(this, kNameProperty);
    }
}

// ------------------------------------------------------------------------------------------
void
BrowserItem::ModelItemSelect()
{
    Select();
    Reveal(kNameProperty);
}

// ------------------------------------------------------------------------------------------
EventTargetRef
BrowserItem::GetUndoTarget()
{
    Browser*    browser = dynamic_cast<Browser*>(mDataBrowser);
    
    B_ASSERT(browser != NULL);
    
    return (browser->GetUndoTarget());
}

// ------------------------------------------------------------------------------------------
const B::TextFilters::Number&
BrowserItem::GetIntegerFilter()
{
    static B::TextFilters::Number   sFilter(
        B::OSPtr<CFNumberFormatterRef>(
            CFNumberFormatterCreate(
                NULL, NULL, kCFNumberFormatterNoStyle),
            B::from_copy),
        kCFNumberFormatterParseIntegersOnly);
    
    return (sFilter);
}

// ------------------------------------------------------------------------------------------
const B::TextFilters::Number&
BrowserItem::GetRealFilter()
{
    static B::TextFilters::Number   sFilter(
        B::OSPtr<CFNumberFormatterRef>(
            CFNumberFormatterCreate(
                NULL, NULL, kCFNumberFormatterDecimalStyle),
            B::from_copy));
    
    return (sFilter);
}

// ------------------------------------------------------------------------------------------
const B::TextFilters::Date&
BrowserItem::GetDateFilter()
{
    static B::TextFilters::Date sFilter(
        B::OSPtr<CFDateFormatterRef>(
            CFDateFormatterCreate(
                NULL, NULL, kCFDateFormatterMediumStyle, kCFDateFormatterMediumStyle),
            B::from_copy));
    
    return (sFilter);
}

// ------------------------------------------------------------------------------------------
const BrowserItem::BooleanFilter&
BrowserItem::GetBooleanFilter()
{
    static BooleanFilter    sFilter;
    
    return (sFilter);
}

// ------------------------------------------------------------------------------------------
const BrowserItem::DataFilter&
BrowserItem::GetDataFilter()
{
    static DataFilter   sFilter;
    
    return (sFilter);
}


// ==========================================================================================
//  BrowserItem::BooleanFilter

// ------------------------------------------------------------------------------------------
BrowserItem::BooleanFilter::BooleanFilter()
{
    mTrueStrings.push_back(
        BLocalizedString(CFSTR("Yes"), 
                         "The description of a boolean true element in a property list browser"));
    mTrueStrings.push_back(
        BLocalizedString(CFSTR("True"), 
                         "An alternative description of a boolean true element in a property list browser"));
    
    mFalseStrings.push_back(
        BLocalizedString(CFSTR("No"), 
                         "The description of a boolean false element in a property list browser"));
    mFalseStrings.push_back(
        BLocalizedString(CFSTR("False"), 
                         "An alternative of a boolean false element in a property list browser"));
}

// ------------------------------------------------------------------------------------------
B::TextValidator::Action
BrowserItem::BooleanFilter::operator () (
    bool                    inFiltering,
    const B::TextSource&    /* inSource */,
    const B::String&        inCandidateText,
    const CFRange&          /* inSelection */,
    B::String&              /* outReplacementText */) const
{
    // We don't perform any validation when filtering.
    
    if (!inFiltering)
    {
        Parse(inCandidateText);
    }
    
    return B::TextValidator::Accept;
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::BooleanFilter::Parse(const B::String& inText) const
{
    if (Matches(inText, mTrueStrings))
        return true;
    else if (Matches(inText, mFalseStrings))
        return false;
    else
        throw B::TextValidator::Exception();
}

// ------------------------------------------------------------------------------------------
B::String
BrowserItem::BooleanFilter::Format(bool inBoolean) const
{
    if (inBoolean)
        return mTrueStrings[0];
    else
        return mFalseStrings[0];
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::BooleanFilter::Matches(const B::String& inText, const std::vector<B::String>& inStrings)
{
    return (std::find_if(inStrings.begin(), inStrings.end(), TextMatcher(inText)) != inStrings.end());
}


// ==========================================================================================
//  BrowserItem::BooleanFilter::TextMatcher

// ------------------------------------------------------------------------------------------
bool
BrowserItem::BooleanFilter::TextMatcher::operator () (const B::String& inString) const
{
    return (CFStringCompare(mText.cf_ref(), inString.cf_ref(), kCFCompareCaseInsensitive) == 0);
}


// ==========================================================================================
//  BrowserItem::DataFilter

// ------------------------------------------------------------------------------------------
BrowserItem::DataFilter::DataFilter()
    : mWhitespace(CFCharacterSetGetPredefined(kCFCharacterSetWhitespace)),
      mCharacterSetFilter(GetCharSet(mWhitespace))
{
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFCharacterSetRef>
BrowserItem::DataFilter::GetCharSet(CFCharacterSetRef ws)
{
    // Start with whitespace.
    B::OSPtr<CFMutableCharacterSetRef>  cs(CFCharacterSetCreateMutableCopy(NULL, ws),
                                           B::from_copy);
    
    // Add the hex digits, plus the end markers.
    CFCharacterSetAddCharactersInString(cs, CFSTR("0123456789abcdefABCDEF<>"));
    
    return B::OSPtr<CFCharacterSetRef>(CFCharacterSetCreateCopy(NULL, cs), B::from_copy);
}

// ------------------------------------------------------------------------------------------
B::TextValidator::Action
BrowserItem::DataFilter::operator () (
    bool                    inFiltering,
    const B::TextSource&    inSource,
    const B::String&        inCandidateText,
    const CFRange&          inSelection,
    B::String&              outReplacementText) const
{
    // We don't perform any validation when filtering.
    
    if (!inFiltering)
    {
        Parse(inCandidateText);
        
        return B::TextValidator::Accept;
    }
    else
    {
        return mCharacterSetFilter(inFiltering, inSource, inCandidateText, inSelection, 
                                   outReplacementText);
    }
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFDataRef>
BrowserItem::DataFilter::Parse(const B::String& inText) const
{
    std::string str;
    
    inText.copy(str, kCFStringEncodingASCII);
    
    std::istringstream  istr(str);
    char                marker[2];
    
    istr >> std::setw(sizeof(marker)) >> marker;
    
    if (strcmp(marker, "<") != 0)
        throw B::TextValidator::Exception();
    
    std::vector<UInt8>  bytes;
    
    bytes.reserve(inText.size() /2);
    
    while (istr)
    {
        char    digits[3];
        int     n;
        
        istr >> std::setw(sizeof(digits)) >> digits;
        
        if (strcmp(digits, ">") == 0)
            break;
        
        if (sscanf(digits, "%x", &n) != 1)
            throw B::TextValidator::Exception();
        
        bytes.push_back(n);
    }
    
    size_t          size    = bytes.size();
    const UInt8*    ptr     = (size > 0) ? &bytes[0] : NULL;
    
    return B::OSPtr<CFDataRef>(CFDataCreate(NULL, ptr, size), B::from_copy);
}

// ------------------------------------------------------------------------------------------
bool
BrowserItem::DataFilter::SkipWS(const B::String& str, size_t inSize, size_t& ioIndex) const
{
    for ( ; ioIndex < inSize; ioIndex++)
    {
        if (!CFCharacterSetIsCharacterMember(mWhitespace, str[ioIndex]))
            break;
    }
    
    return (ioIndex < inSize);
}

// ------------------------------------------------------------------------------------------
B::String
BrowserItem::DataFilter::Format(const B::OSPtr<CFDataRef>& inData) const
{
    std::ostringstream  ostr;
    
    ostr << "<" << std::setfill('0') << std::hex;
    
    size_t          size    = CFDataGetLength(inData);
    const UInt8*    startp  = CFDataGetBytePtr(inData);
    const UInt8*    endp    = startp + size;
    
    for (const UInt8* ptr = startp ; ptr < endp; ptr++)
    {
        ostr << std::setw(2) << static_cast<int>(*ptr);
        
        if ((ptr + 1 - startp) % 4 == 0)
            ostr << " ";
    }
    
    ostr << ">" << std::flush;
    
    return B::String(ostr.str());
}
