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
#include "Browser.h"

// library headers
#include <boost/scoped_ptr.hpp>

// B headers
#include "BAbstractDocument.h"
#include "BAEDescriptor.h"
#include "BAEEvent.h"
#include "BAEWriter.h"
#include "BBundle.h"
#include "BClipboard.h"
#include "BCommandData.h"
#include "BContextualMenu.h"
#include "BDrag.h"
#include "BEvent.h"
#include "BNib.h"
#include "BPasteboardStream.h"
#include "BUndo.h"

// project headers
#include "BrowserItem.h"
#include "ModelItem.h"


namespace
{
    static ModelItemPtr
    GetModelPtrFromBrowserItem(B::DataBrowserItem* inItem)
    {
        BrowserItem*    browserItem;
        
        browserItem = dynamic_cast<BrowserItem*>(inItem);
        B_ASSERT(browserItem != NULL);
        
        return browserItem->GetModel()->GetPtr();
    }
    
    typedef B::AEObject::ObjectListSpecifier<
                std::vector<ModelItemPtr>::iterator>    AEObjectSpecifierList;
}

// ---------------------------------------------------------------------------
B::View*
Browser::Instantiate(HIViewRef inViewRef)
{
    return (new Browser(inViewRef));
}

// ---------------------------------------------------------------------------
Browser::Browser(HIViewRef inViewRef)
    : B::DataBrowser(inViewRef), 
      mEventHandler(inViewRef),
      mSelectionIsDeletable(false), mSelectionIsMoveable(false), 
      mSelectionIsSingleContainer(false), 
      mSelectionIsSingleItem(false), 
      mSelectionIsSingleNonRootItem(false), 
      mRootItem(NULL), 
      mUndoTarget(GetUserFocusEventTarget())
{
    InitEventHandler(mEventHandler);
}

// ---------------------------------------------------------------------------
void
Browser::InitEventHandler(B::EventHandler& ioHandler)
{
    ioHandler.Add(this, &Browser::ServiceCopy);
    ioHandler.Add(this, &Browser::ServiceGetTypes);
    
    ioHandler.Init();
}

// ---------------------------------------------------------------------------
void
Browser::Awaken(B::Nib* inFromNib)
{
    B_ASSERT(inFromNib != NULL);
    
    UseDragAndDrop();
    
    SetDisclosureColumn(BrowserItem::kNameProperty);
    SetSortColumn(BrowserItem::kNameProperty);
    SetColumnEditable(BrowserItem::kNameProperty, true);
    SetColumnEditable(BrowserItem::kClassProperty, true);
    SetColumnEditable(BrowserItem::kValueProperty, true);
    
    mClassMenu      = inFromNib->CreateMenu("ClassMenu");
    mContextualMenu = inFromNib->CreateMenu("BrowserCM");
    
    new B::ContextualMenu(mContextualMenu);
}

// ---------------------------------------------------------------------------
void
Browser::MakeRootItem(ModelItem* inRootModel)
{
    mRootItem = new BrowserItem(inRootModel->GetPtr(), mClassMenu);
    
    AddItem(mRootItem);
    
    if (mRootItem->IsContainer())
        mRootItem->Open();
}

// ---------------------------------------------------------------------------
void
Browser::NewSibling()
{
    if (CanAddSiblingToSelection())
    {
        B::AutoUndo autoUndo(BLocalizedString(
                                CFSTR("New Item"), 
                                "Undo text for property list item creation."), 
                             GetUndoTarget());
        
        BrowserItem*    item    = dynamic_cast<BrowserItem*>(GetFirstSelectedItem());
        
        B_ASSERT(item != NULL);
        
        item->NewSibling();
        
        autoUndo.Commit();
    }
}

// ---------------------------------------------------------------------------
void
Browser::NewChild()
{
    if (CanAddChildToSelection())
    {
        B::AutoUndo autoUndo(BLocalizedString(
                                CFSTR("New Item"), 
                                "Undo text for property list item creation."), 
                             GetUndoTarget());
        
        BrowserItem*    item    = dynamic_cast<BrowserItem*>(GetFirstSelectedItem());
        
        B_ASSERT(item != NULL);
        
        item->NewChild();
        
        autoUndo.Commit();
    }
}

// ---------------------------------------------------------------------------
void
Browser::DeleteSelection()
{
    if (IsSelectionDeletable())
    {
        B::AutoUndo autoUndo(BLocalizedString(
                                CFSTR("Delete"), 
                                "Undo text for property list item deletion."), 
                             GetUndoTarget());
        
        std::vector<ModelItemPtr>   items;
        
        GetCanonicalSelection(items);
        
        B::AEEvent<kAECoreSuite, kAEDelete>::Send(
            B::AESelfTarget(),
            AEObjectSpecifierList(items.begin(), items.end()));
        
        autoUndo.Commit();
    }
}

// ---------------------------------------------------------------------------
void
Browser::MoveSelection(ModelItemPtr inNewParent)
{
    if (IsSelectionMovable())
    {
        B::AutoUndo autoUndo(BLocalizedString(
                                CFSTR("Move"), 
                                "Undo text for property list item move."), 
                             GetUndoTarget());
        
        OSType  insertPosition;
        
        if (inNewParent->IsContainer())
        {
            insertPosition = kAEBeginning;
        }
        else
        {
            B_ASSERT(inNewParent->IsParentArray());
            
            insertPosition = kAEAfter;
        }
        
        std::vector<ModelItemPtr>   items;
        
        GetCanonicalSelection(items);
        
        B::AEEvent<kAECoreSuite, kAEMove>::Send(
            B::AESelfTarget(),
            AEObjectSpecifierList(items.begin(), items.end()), 
            B::AEObject::InsertionLocationSpecifier(insertPosition, inNewParent), 
            B::AEEventFunctor::NullResult());
        
        autoUndo.Commit();
    }
}

// ---------------------------------------------------------------------------
void
Browser::CloneSelection(ModelItem* /* inNewParent */ /* = NULL */)
{
    if (IsSelectionClonable())
    {
        B::AutoUndo autoUndo(BLocalizedString(
                                CFSTR("Duplicate"), 
                                "Undo text for property list item duplication."), 
                             GetUndoTarget());
        
//      if (inNewParent == NULL)
//      {
//          B::DataBrowserItem* item    = GetFirstSelectedItem();
//          
//          if (item != NULL)
//              item = item->GetParent();
//          
//          BrowserItem*    browserItem = dynamic_cast<BrowserItem*>(item);
//          
//          if (browserItem != NULL)
//              inNewParent = browserItem->GetModel().get();
//          
//          B_ASSERT(inNewParent != NULL);
//      }
        
//      OSType  insertPosition;
//      
//      if (inNewParent->IsContainer())
//      {
//          insertPosition = kAEBeginning;
//      }
//      else
//      {
//          B_ASSERT(inNewParent->IsParentArray());
//          
//          insertPosition = kAEAfter;
//      }

        
        // Each item in the selection gets cloned independently.  For each, if its 
        // container is an array, then the clone is placed after the original item;  
        // else, it's placed at the end of the container.
        
        std::vector<ModelItemPtr>   items;
        
        GetCanonicalSelection(items);
        
        for (std::vector<ModelItemPtr>::const_iterator it = items.begin(); 
             it != items.end(); 
             ++it)
        {
            ModelItemPtr    model   = *it;
            ModelItemPtr    target;
            OSType          insertPosition;
            
            if (model->IsParentArray())
            {
                insertPosition  = kAEAfter;
                target          = model;
            }
            else
            {
                insertPosition  = kAEEnd;
                target          = boost::dynamic_pointer_cast<ModelItem>(model->GetContainer());
                
                B_ASSERT(target != NULL);
            }
            
            model->SendCloneAppleEvent(insertPosition, target);
        }
        
        autoUndo.Commit();
    }
}

// ---------------------------------------------------------------------------
void
Browser::CopySelectionToPasteboard(
    B::Pasteboard&  ioPasteboard)
{
    ItemVector  selection;
    
    GetCanonicalSelection(selection);
    
    ioPasteboard.Clear();
    
    std::for_each(selection.begin(), selection.end(), 
                  ItemFlavorWriter(ioPasteboard));
}

// ---------------------------------------------------------------------------
void
Browser::CopySelection()
{
    if (CanCopySelection())
    {
        CopySelectionToPasteboard(B::Clipboard::Get());
    }
}

// ---------------------------------------------------------------------------
void
Browser::CutSelection()
{
#if 0
    if (CanCutSelection())
    {
        mRootItem->GetModel()->GetDocument()->GetUndoManager()->SetActionName(
                CFULocalizedString(
                    CFSTR("Cut"), 
                    "Undo text for property list item cut."));
        
        BrowserItem*    item    = dynamic_cast<BrowserItem*>(GetFirstSelectedItem());
        
        B_ASSERT(item != NULL);
        
        boost::scoped_ptr<B::ScrapOutputStream> ostr(B::Scrap::GetClipboard().MakeWriter());
        
        item->WriteFlavors(*ostr, true);
        
        item->GetModel()->DeleteObject();
    }
#endif
}

// ---------------------------------------------------------------------------
void
Browser::EditSelectionValue()
{
    if (CanEditSelectionValue())
    {
        B::DataBrowserItem* item    = GetFirstSelectedItem();
        
        B_ASSERT(item != NULL);
        
        OpenEditSession(item, BrowserItem::kValueProperty);
    }
}

// ---------------------------------------------------------------------------
UInt32
Browser::GetCanonicalSelection(ItemVector& outItems) const
{
    GetSelectedItems(outItems);
    
    for (size_t i = 0; i < outItems.size(); )
    {
        B::DataBrowserItem* item            = outItems[i];
        bool                parentSelected  = false;
        
        for (B::DataBrowserItem* parent = item->GetParent(); 
             !parentSelected && (parent != NULL); 
             parent = parent->GetParent())
        {
            parentSelected = parent->IsSelected();
        }
        
        if (parentSelected)
            outItems.erase(outItems.begin() + i);
        else
            ++i;
    }
    
    return (outItems.size());
}

// ---------------------------------------------------------------------------
UInt32
Browser::GetCanonicalSelection(std::vector<ModelItemPtr>& outModelItems) const
{
    ItemVector  browserItems;
    
    GetCanonicalSelection(browserItems);
    
    outModelItems.clear();
    outModelItems.reserve(browserItems.size());
    
    std::transform(browserItems.begin(), browserItems.end(), 
                   std::back_inserter(outModelItems), 
                   GetModelPtrFromBrowserItem);
    
    return (outModelItems.size());
}

// ---------------------------------------------------------------------------
/*! React to a selection change within the data browser.  By default, we 
    broadcast a "selection changed" notification.
*/
void
Browser::SelectionChanged()
{
    mSelectionIsDeletable = mSelectionIsMoveable = false;
    mSelectionIsSingleContainer = mSelectionIsSingleNonRootItem = false;
    mSelectionIsSingleItem = mSelection = false;
    
    if (GetFirstSelectedItem() != NULL)
    {
        mSelection = true;
        
        B::DataBrowser::ItemVector  items;
        
        GetCanonicalSelection(items);
        
        if (!items.empty())
        {
            mSelectionIsDeletable = mSelectionIsMoveable = true;
            
            for (B::DataBrowser::ItemVector::iterator it = items.begin(); 
                 it != items.end(); 
                 ++it)
            {
                BrowserItem*    browserItem;
                
                browserItem = dynamic_cast<BrowserItem*>(*it);
                B_ASSERT(browserItem != NULL);
                
                if (!browserItem->IsDeletable())
                    mSelectionIsDeletable = false;
                
                if (!browserItem->IsMovable())
                    mSelectionIsMoveable = false;
                
                if (items.size() == 1)
                {
                    mSelectionIsSingleItem = true;
                    
                    if (browserItem->IsContainer())
                        mSelectionIsSingleContainer = true;
                    
                    if (!browserItem->IsRoot())
                        mSelectionIsSingleNonRootItem = true;
                }
            }
        }
    }
    
    DataBrowser::SelectionChanged();
}

// ---------------------------------------------------------------------------
/*! This is called when the user initially drags something into the list,
    and the control needs to know whether the drag is acceptable. The inItem
    parameter is NULL if the drag is going into the top level of the hierarchy,
    otherwise it's the container object into which things are being dragged.
*/
bool
Browser::IsDragOnBrowserAcceptable(
    const B::Drag&  /* inDrag */)
{
//  B_ASSERT(mRootItem != NULL);
//  
//  return (mRootItem->IsDragAcceptable(inDragRef));
    
    return (false);
}

// ---------------------------------------------------------------------------
/*! This is called after the user drops something into the list. The inItem
    parameter is NULL if the drop is going into the top level of the hierarchy,
    otherwise it's the container object into which things have been dropped.

    The drag contains items that are acceptable, as defined by the
    DragIsAcceptable() and ItemIsAcceptable() member functions.
*/
bool
Browser::ReceiveDragOnBrowser(
    const B::Drag&  /* inDrag */)
{
//  B_ASSERT(mRootItem != NULL);
//  
//  return (mRootItem->ReceiveDrag(inDragRef));
    
    return (false);
}

// ---------------------------------------------------------------------------
void
Browser::PostProcessDrag(
    const B::Drag&  inDrag,
    OSStatus        inTrackDragResult)
{
    if (inTrackDragResult != noErr)
        return;
    
    DragActions dragAction  = inDrag.GetDropAction();
    
    if (dragAction & kDragActionCopy)
    {
        // There's nothing to do, since a copy leaves the original 
        // container intact.
    }
    else if (dragAction & kDragActionMove)
    {
        ModelItem*  modelItem   = BrowserItem::ReadDropLocation(inDrag);
        
        if (modelItem != NULL)
        {
            MoveSelection(modelItem->GetPtr());
        }
    }
    else if (dragAction & kDragActionDelete)
    {
        // This is a delete.
        
        DeleteSelection();
    }
}

// ---------------------------------------------------------------------------
void
Browser::GetContextualMenu(
    MenuRef&            outMenuRef,
    UInt32&             outHelpType,
    CFStringRef&        /* outHelpItemString */,
    AEDesc&             /* outSelection */)
{
    outMenuRef  = mContextualMenu;
    outHelpType = kCMHelpItemNoHelp;
}

// ---------------------------------------------------------------------------
bool
Browser::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
//  case kHICommandCut:
//      CutSelection();
//      break;
        
    case kHICommandCopy:
        CopySelection();
        break;
        
//  case kHICommandClear:
//      DeleteSelection();
//      break;
        
    case kEditValueCmdID:
        EditSelectionValue();
        break;
        
    default:
        handled = DataBrowser::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ---------------------------------------------------------------------------
bool
Browser::HandleUpdateStatus(
    const HICommandExtended&    inHICommand, 
    B::CommandData&             ioCmdData)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
//  case kHICommandCut:
//      ioCmdData.SetEnabled(CanCutSelection());
//      break;
        
    case kHICommandCopy:
        ioCmdData.SetEnabled(CanCopySelection());
        break;
        
//  case kHICommandClear:
//      ioCmdData.SetEnabled(IsSelectionDeletable());
//      break;
        
    case kEditValueCmdID:
        ioCmdData.SetEnabled(CanEditSelectionValue());
        break;
        
    default:
        handled = DataBrowser::HandleUpdateStatus(inHICommand, ioCmdData);
        break;
    }
    
    return (handled);
}

// ---------------------------------------------------------------------------
bool
Browser::HandleServiceCopy(
    B::Pasteboard&  ioPasteboard)
{
    if (CanCopySelection())
    {
        CopySelectionToPasteboard(ioPasteboard);
    }
    
    return (true);
}

// ---------------------------------------------------------------------------
bool
Browser::HandleServiceGetTypes(
    B::MutableArray<CFStringRef>&       ioCopyTypes, 
    B::MutableArray<CFStringRef>&       ioPasteTypes)
{
    if (CanCopySelection())
    {
        BrowserItem::GetSupportedServiceFlavors(ioCopyTypes, ioPasteTypes);
    }
    
    return (true);
}

// ---------------------------------------------------------------------------
bool
Browser::ServiceCopy(
    B::Event<kEventClassService, kEventServiceCopy>&    event)
{
    B::Pasteboard   pasteboard(event.mPasteboardRef);
    
    return (HandleServiceCopy(pasteboard));
}

// ---------------------------------------------------------------------------
bool
Browser::ServiceGetTypes(
    B::Event<kEventClassService, kEventServiceGetTypes>&    event)
{
    return (HandleServiceGetTypes(event.mCopyTypes, event.mPasteTypes));
}

// ---------------------------------------------------------------------------
Browser::ItemFlavorWriter::ItemFlavorWriter(B::Pasteboard& inPasteboard)
    : mPasteboard(inPasteboard)
{
}

// ---------------------------------------------------------------------------
void
Browser::ItemFlavorWriter::operator () (B::DataBrowserItem* item) const
{
    dynamic_cast<BrowserItem*>(item)->WriteFlavors(mPasteboard);
}
