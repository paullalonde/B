// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

// file header
#include "BDataBrowser.h"

// system headers
#include <Carbon/Carbon.h>

// project headers
#include "BAutoUPP.h"
#include "BDataBrowserItem.h"
#include "BDataBrowserItemData.h"
#include "BDrag.h"
#include "BEvent.h"
#include "BEventParams.h"
#include "BString.h"
#include "BViewFactory.h"


namespace
{
	template <class T> class ToolboxViewRegistrar
	{
	public:
		ToolboxViewRegistrar(CFStringRef inClassID)
			{
				B::ViewFactory::Default().template Register<T>(inClassID);
			}
	};
}


namespace B {

// ------------------------------------------------------------------------------------------
View*
DataBrowser::Instantiate(HIViewRef inViewRef)
{
	return (new DataBrowser(inViewRef));
}

// ------------------------------------------------------------------------------------------
/*!	@note	The view is initially invisible.
*/
DataBrowser*
DataBrowser::Create(
	const HIViewID&			inViewID,		//!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
	HIViewRef				inSuperview,	//!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
	const Rect*				inFrame,		//!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
	Nib*					inFromNib,		//!< The nib from which to load other resources;  may be @c NULL.
	const ViewFactory*		inFactory,		//!< The factory to use for instantiating the derivative of View;; may be @c NULL, in which case the default view factory is used instead.
	DataBrowserViewStyle	inViewStyle)
{
	::Rect			qdRect	= { 0, 0, 100, 100 };
	HIViewRef		viewRef;
	DataBrowser*	viewObj;
	OSStatus		err;
	
	if (inFrame != NULL)
		qdRect = *inFrame;
	
	err = CreateDataBrowserControl(NULL, &qdRect, inViewStyle, &viewRef);
	B_THROW_IF_STATUS(err);
	
	viewObj = SetupTypedView<DataBrowser>(
					inViewID, viewRef, inSuperview, inFrame, inFromNib, inFactory);
	
	return (viewObj);
}

// ------------------------------------------------------------------------------------------
DataBrowser::DataBrowser(HIViewRef inViewRef)
	: PredefinedView(inViewRef),
	  mEventHandler(inViewRef),
	  mEditItem(NULL)
{
	InitEventHandler();
	
	UInt32	features = 0;
	
	::GetControlFeatures(*this, &features);
	::SetControlDragTrackingEnabled(*this, false);

	// Set up the data browser callbacks.
	
	static AutoDataBrowserItemDataUPP				sItemData(GetSetItemDataProc);
	static AutoDataBrowserItemCompareUPP			sItemCompare(CompareProc);
	static AutoDataBrowserItemNotificationUPP		sItemNotification(ItemNotificationProc);
	static AutoDataBrowserAddDragItemUPP			sAddDragItem(AddDragItemProc);
	static AutoDataBrowserAcceptDragUPP				sAcceptDrag(AcceptDragProc);
	static AutoDataBrowserReceiveDragUPP			sReceiveDrag(ReceiveDragProc);
	static AutoDataBrowserPostProcessDragUPP		sPostProcessDrag(PostProcessDragProc);
	static AutoDataBrowserItemHelpContentUPP		sItemHelpContent(GetHelpContentProc);
	static AutoDataBrowserGetContextualMenuUPP		sGetContextualMenu(GetContextualMenuProc);
	static AutoDataBrowserSelectContextualMenuUPP	sSelectContextualMenu(SelectContextualMenuProc);
	
	DataBrowserCallbacks	callbacks;
	
	InitializeDataBrowserCallbacks(&callbacks, kDataBrowserLatestCallbacks);

	callbacks.u.v1.itemDataCallback				= sItemData;
	callbacks.u.v1.itemCompareCallback			= sItemCompare;
	callbacks.u.v1.itemNotificationCallback		= sItemNotification;
	callbacks.u.v1.addDragItemCallback			= sAddDragItem;
	callbacks.u.v1.acceptDragCallback			= sAcceptDrag;
	callbacks.u.v1.receiveDragCallback			= sReceiveDrag;
	callbacks.u.v1.postProcessDragCallback		= sPostProcessDrag;
	callbacks.u.v1.itemHelpContentCallback		= sItemHelpContent;
	callbacks.u.v1.getContextualMenuCallback	= sGetContextualMenu;
	callbacks.u.v1.selectContextualMenuCallback	= sSelectContextualMenu;
	
	::SetDataBrowserCallbacks(*this, &callbacks);
	
	// Set up the *custom* data browser callbacks.
	
	static AutoDataBrowserDrawItemUPP			sDrawItem(DrawItemProc);
	static AutoDataBrowserEditItemUPP			sEditItem(EditItemProc);
	static AutoDataBrowserHitTestUPP			sHitTest(HitTestProc);
	static AutoDataBrowserTrackingUPP			sTracking(TrackingProc);
	static AutoDataBrowserItemDragRgnUPP		sItemDragRgn(ItemDragRgnProc);
	static AutoDataBrowserItemAcceptDragUPP		sItemAcceptDrag(ItemAcceptDragProc);
	static AutoDataBrowserItemReceiveDragUPP	sItemReceiveDrag(ItemReceiveDragProc);

	DataBrowserCustomCallbacks	customCallbacks;
	
	InitializeDataBrowserCustomCallbacks(&customCallbacks, kDataBrowserLatestCustomCallbacks);

	customCallbacks.u.v1.drawItemCallback		= sDrawItem;
	customCallbacks.u.v1.editTextCallback		= sEditItem;
	customCallbacks.u.v1.hitTestCallback		= sHitTest;
	customCallbacks.u.v1.trackingCallback		= sTracking;
	customCallbacks.u.v1.dragRegionCallback		= sItemDragRgn;
	customCallbacks.u.v1.acceptDragCallback		= sItemAcceptDrag;
	customCallbacks.u.v1.receiveDragCallback	= sItemReceiveDrag;
	
	::SetDataBrowserCustomCallbacks(*this, &customCallbacks);
}

// ------------------------------------------------------------------------------------------
DataBrowser::~DataBrowser()
{
	try
	{
		Cleanup();
	}
	catch (...)
	{
		// There's not much we can do here...
	}
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::InitEventHandler()
{
	mEventHandler.Add(this, &DataBrowser::CommandProcess);
	mEventHandler.Add(this, &DataBrowser::CommandUpdateStatus);
	mEventHandler.Add(this, &DataBrowser::ControlGetOptimalBounds);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
/*!	Removes all items.
*/
void
DataBrowser::Cleanup()
{
	B_ASSERT(*this != NULL);
    
    // disconnect everyone
    
    mSelectionChangedSignal.disconnect_all_slots();
    mUserStateChangedSignal.disconnect_all_slots();
    
    OSStatus    err;
    
    if (mEditItem != NULL)
    {
        err = SetDataBrowserEditItem(*this, kDataBrowserNoItem, 0);
    }
    
	// remove all items
	RemoveAllItems();
	
	// clear out the callbacks for this instance
	
	DataBrowserCallbacks	callbacks;
	
	callbacks.version = kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&callbacks);
	SetDataBrowserCallbacks(*this, &callbacks);
}

// ------------------------------------------------------------------------------------------
/*!	Returns selections flags for databrowser object
*/
DataBrowserSelectionFlags
DataBrowser::GetSelectionFlags() const
{
	DataBrowserSelectionFlags	theFlags;
	OSStatus					err;
	
	err = ::GetDataBrowserSelectionFlags(*this, &theFlags);
	B_THROW_IF_STATUS(err);
	
	return (theFlags);
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDictionaryRef>
DataBrowser::GetUserState() const
{
	CFDictionaryRef	userState;
	OSStatus		err;
	
	// NOTE:  The prototypes in ControlDefinitions.h incorrectly state that 
	// userState is a CFDataRef when it is in fact a CFDictionaryRef.  This 
	// error has been documented in QA 1276.
	//
	//	See http://developer.apple.com/qa/qa2001/qa1270.html
	
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
	err = ::GetDataBrowserUserState(*this, &userState);
#else
	err = ::GetDataBrowserUserState(*this, reinterpret_cast<CFDataRef*>(&userState));
#endif
	B_THROW_IF_STATUS(err);
    
	return (OSPtr<CFDictionaryRef>(userState, from_copy));
}

// ------------------------------------------------------------------------------------------
unsigned
DataBrowser::GetHeaderHeight() const
{
	UInt16		height;
	OSStatus	err;
	
	err = ::GetDataBrowserListViewHeaderBtnHeight(*this, &height);
	B_THROW_IF_STATUS(err);
	
	return (height);
}

// ------------------------------------------------------------------------------------------
unsigned
DataBrowser::GetRowHeight() const
{
	UInt16		height;
	OSStatus	err;
	
	err = ::GetDataBrowserTableViewRowHeight(*this, &height);
	B_THROW_IF_STATUS(err);
	
	return (height);
}

unsigned
DataBrowser::GetColumnWidth() const
{
	UInt16		width;
	OSStatus	err;
	
	err = ::GetDataBrowserTableViewColumnWidth(*this, &width);
	B_THROW_IF_STATUS(err);
	
	return (width);
}

void
DataBrowser::SetDisclosureColumn(DataBrowserTableViewColumnID inColumnID)
{
	OSStatus	err;
	
	err = ::SetDataBrowserListViewDisclosureColumn(*this, 
												   inColumnID, 
												   false);
	B_THROW_IF_STATUS(err);
}

void
DataBrowser::SetSortColumn(DataBrowserTableViewColumnID inColumnID)
{
	OSStatus	err;
	
	err = ::SetDataBrowserSortProperty(*this, inColumnID);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
unsigned
DataBrowser::CountColumns() const
{
	UInt32		numColumns;
	OSStatus	err;
	
	err = ::GetDataBrowserTableViewColumnCount(*this, &numColumns);
	B_THROW_IF_STATUS(err);
	
	return (numColumns);
}

// ------------------------------------------------------------------------------------------
DataBrowserTableViewColumnID
DataBrowser::GetColumnID(unsigned index) const
{
	DataBrowserTableViewColumnID	columnID;
	OSStatus						err;
	
	err = ::GetDataBrowserTableViewColumnProperty(*this, index, &columnID);
	B_THROW_IF_STATUS(err);
	
	return (columnID);
}

// ------------------------------------------------------------------------------------------
unsigned
DataBrowser::GetColumnPosition(
	DataBrowserTableViewColumnID	inColumnID) const
{
	DataBrowserTableViewColumnIndex	index;
	OSStatus						err;
	
	err = ::GetDataBrowserTableViewColumnPosition(*this, inColumnID, &index);
	B_THROW_IF_STATUS(err);
	
	return (index);
}

// ------------------------------------------------------------------------------------------
UInt16
DataBrowser::GetColumnWidth(
	DataBrowserTableViewColumnID	inColumnID) const
{
	UInt16		width;
	OSStatus	err;
	
	err = ::GetDataBrowserTableViewNamedColumnWidth(*this, inColumnID, &width);
	B_THROW_IF_STATUS(err);
	
	return (width);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::SetColumnWidth(
	DataBrowserTableViewColumnID	inColumnID, 
	UInt16							inWidth)
{
	OSStatus	err;
	
	err = ::SetDataBrowserTableViewNamedColumnWidth(*this, inColumnID, inWidth);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::SetColumnEditable(DataBrowserTableViewColumnID inColumnID, bool editable)
{
	DataBrowserPropertyFlags	flags;
	OSStatus					err;
	
	err = ::GetDataBrowserPropertyFlags(*this, inColumnID, &flags);
	B_THROW_IF_STATUS(err);
	
	if (editable)
		flags |= kDataBrowserPropertyIsMutable;
	else
		flags &= ~kDataBrowserPropertyIsMutable;
	
	err = ::SetDataBrowserPropertyFlags(*this, inColumnID, flags);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::SetUserState(OSPtr<CFDictionaryRef> inUserState)
{
	OSStatus	err;
	
	// NOTE:  The prototypes in ControlDefinitions.h incorrectly state that 
	// userState is a CFDataRef when it is in fact a CFDictionaryRef.  This 
	// error has been documented in QA 1276.
	//
	//	See http://developer.apple.com/qa/qa2001/qa1276.html
	
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
	err = ::SetDataBrowserUserState(*this, inUserState.get());
#else
	err = ::SetDataBrowserUserState(*this, reinterpret_cast<CFDataRef>(inUserState.get()));
#endif
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::SetRootContainer(DataBrowserItem* inContainer)
{
	OSStatus	err;
	
	inContainer->SetBrowser(this);
	
	err = SetDataBrowserTarget(*this, 
							   reinterpret_cast<DataBrowserItemID>(inContainer));
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::UseDragAndDrop()
{
	OSStatus	err;
	
	err = ::SetControlDragTrackingEnabled(*this, true);
	B_THROW_IF_STATUS(err);
}


#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Adds the column describes by inColumnDesc to the data browser.
*/
void
DataBrowser::AddColumn(
	DataBrowserListViewColumnDesc&	inColumnDesc,
	DataBrowserTableViewColumnIndex	inPosition /* = kDataBrowserListViewAppendColumn */, 
	SInt16							inInitialWidth /* = 0 */)
{
	Boolean		varColWidth, varRowHeight;
	OSStatus	err;
	
	err = AddDataBrowserListViewColumn(*this, 
									   &inColumnDesc, 
									   inPosition);
	B_THROW_IF_STATUS(err);
	
	err = GetDataBrowserTableViewGeometry(*this,
										  &varColWidth, 
										  &varRowHeight);
	B_THROW_IF_STATUS(err);
	
	if (varColWidth && (inInitialWidth > 0))
	{
		err = SetDataBrowserTableViewNamedColumnWidth(*this,
													  inColumnDesc.propertyDesc.propertyID,
													  inInitialWidth);
		B_THROW_IF_STATUS(err);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Insert the given item into the top level of the data browser.
*/
void
DataBrowser::AddItem(
	DataBrowserItem*		inItem,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	ItemVector	items(1, inItem);
	
	AddItems(NULL, items, inPresorted);
}

// ------------------------------------------------------------------------------------------
/*!	Insert the given item into the data browser. If inParentItem is NULL, it 
	will be placed as a top-level item; otherwise it will be a child of 
	inParentItem.
*/
void
DataBrowser::AddItem(
	DataBrowserItem*		inParentItem, 
	DataBrowserItem*		inItem,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	ItemVector	items(1, inItem);
	
	AddItems(inParentItem, items, inPresorted);
}

// ------------------------------------------------------------------------------------------
/*!	Insert each item from inItems into the data browser.  If inParentItem is 
	NULL, the items will be placed in the top level;  otherwise they will be 
	inserted as children of inParentItem.  This routine allows a number of 
	items to be batch-processed and inserted all at once, rather than 
	individually, to avoid redraws after each.
*/
void
DataBrowser::AddItems(
	DataBrowserItem*		inParentItem, 
	const ItemVector&		inItems,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	if (!inItems.empty())
	{
		OSStatus	err;
		
		// Connnect each item to the data browser and to its parent item.
		
		for (UInt32 i = 0; i < inItems.size(); i++)
		{
			DataBrowserItem*	anItem	= inItems[i];
			
			anItem->SetBrowser(this);
			anItem->SetParent(inParentItem);
		}
		
		// Add the items.
		
		err = AddDataBrowserItems(*this,
								  reinterpret_cast<DataBrowserItemID>(inParentItem),
								  inItems.size(),
								  reinterpret_cast<const DataBrowserItemID*>(&inItems[0]),
								  inPresorted);
		B_THROW_IF_STATUS(err);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Remove the given item from the data browser.
*/
void
DataBrowser::RemoveItem(
	DataBrowserItem*		inItem,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	ItemVector	items(1, inItem);
	
	RemoveItems(items, inPresorted);
}

// ------------------------------------------------------------------------------------------
/*!	Remove each item from inItems from the data browser.
*/
void
DataBrowser::RemoveItems(
	const ItemVector&		inItems, 
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	if (!inItems.empty())
	{
		OSStatus	err;
		
	  	err = RemoveDataBrowserItems(*this,
	  								 kDataBrowserNoItem,
	  								 inItems.size(),
	  								 reinterpret_cast<const DataBrowserItemID*>(&inItems[0]),
	  								 inPresorted);
	  	B_THROW_IF_STATUS(err);
  }
}

// ------------------------------------------------------------------------------------------
/*!	Remove every item from the data browser.
*/
void
DataBrowser::RemoveAllItems(
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	RemoveChildren(NULL, inPresorted);
}

// ------------------------------------------------------------------------------------------
/*!	Remove every child of the given item from the data browser.
*/
void
DataBrowser::RemoveChildren(
	DataBrowserItem*		inParentItem, 
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	OSStatus	err;
	
	err = RemoveDataBrowserItems(*this,
								 reinterpret_cast<DataBrowserItemID>(inParentItem),
								 0, NULL, inPresorted);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Instruct the data browser to re-fetch property inProperty of inItem.
*/
void
DataBrowser::UpdateItem(
	DataBrowserItem*		inItem,
	DataBrowserPropertyID	inProperty /* = kDataBrowserNoItem */,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	DataBrowserItem*	parent	= (inItem != NULL) ? inItem->GetParent() : NULL;
	ItemVector			items(1, inItem);
	
	UpdateItems(parent, items, inProperty, inPresorted);
}

// ------------------------------------------------------------------------------------------
/*!	Instruct the data browser to re-fetch property inProperty of each item in 
	inItems.
*/
void
DataBrowser::UpdateItems(
	DataBrowserItem*		inParentItem, 
	const ItemVector&		inItems,
	DataBrowserPropertyID	inProperty /* = kDataBrowserNoItem */,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	if (!inItems.empty())
	{
		OSStatus	err;
		
		err = UpdateDataBrowserItems(*this,
									 reinterpret_cast<DataBrowserItemID>(inParentItem),
									 inItems.size(),
									 reinterpret_cast<const DataBrowserItemID*>(&inItems[0]),
									 inPresorted,
									 inProperty);
		B_THROW_IF_STATUS(err);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Instruct the data browser to re-fetch property inProperty of each child of 
	inParentItem.
*/
void
DataBrowser::UpdateChildren(
	DataBrowserItem*		inParentItem, 
	DataBrowserPropertyID	inProperty /* = kDataBrowserNoItem */,
	DataBrowserPropertyID	inPresorted /* = kDataBrowserItemNoProperty */)
{
	OSStatus	err;
	
	err = UpdateDataBrowserItems(*this,
								 reinterpret_cast<DataBrowserItemID>(inParentItem),
								 0, NULL, 
								 inPresorted,
								 inProperty);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Return the items under inParentItem with the given state.
*/
UInt32
DataBrowser::GetItems(
	DataBrowserItem*		inParentItem, 
	bool					inRecurse,
	DataBrowserItemState	inState,
	ItemVector&				outItems) const
{
	Handle	itemsHand	= NULL;
	
	try
	{
		OSStatus	err;
		
		outItems.clear();
		
		itemsHand = NewHandle(0);
		B_THROW_IF_NULL(itemsHand);
		
		err = GetDataBrowserItems(*this, 
								  reinterpret_cast<DataBrowserItemID>(inParentItem), 
								  inRecurse, 
								  inState, 
								  itemsHand);
		
		if (err == noErr)
		{
			::Size				itemsSize	= GetHandleSize(itemsHand);
			size_t				numItems;
			DataBrowserItem**	itemsPtr;
			
			HLock(itemsHand);
			
			itemsPtr	= *reinterpret_cast<DataBrowserItem***>(itemsHand);
			numItems	= itemsSize / sizeof(DataBrowserItem*);
			
			outItems.reserve(numItems);
			
			for (size_t i = 0; i < numItems; i++)
			{
				outItems.push_back(itemsPtr[i]);
			}
		}
		else if (err != errDataBrowserItemNotFound)
		{
			B_THROW_IF_STATUS(err);
		}
		
		DisposeHandle(itemsHand);
	}
	catch (...)
	{
		if (itemsHand != NULL)
			DisposeHandle(itemsHand);
		
		throw;
	}
	
	return (outItems.size());
}

// ------------------------------------------------------------------------------------------
/*!	Return the number of items under inParentItem with the given state.
*/
UInt32
DataBrowser::CountItems(
	DataBrowserItem*		inParentItem, 
	bool					inRecurse,
	DataBrowserItemState	inState) const
{
	UInt32		numItems	= 0;
	OSStatus	err;
	
	err = GetDataBrowserItemCount(*this, 
								  reinterpret_cast<DataBrowserItemID>(inParentItem), 
								  inRecurse, 
								  inState, 
								  &numItems);
	B_THROW_IF_STATUS(err);
	
	return (numItems);
}

// ------------------------------------------------------------------------------------------
/*!	Returns the state of the given item.
*/
DataBrowserItemState
DataBrowser::GetItemState(
	const DataBrowserItem*	inItem) const
{
	DataBrowserItemState	state;
	OSStatus				err;
	
	err = GetDataBrowserItemState(*this, 
								  reinterpret_cast<DataBrowserItemID>(inItem), 
								  &state);
	B_THROW_IF_STATUS(err);
	
	return (state);
}

// ------------------------------------------------------------------------------------------
::Rect
DataBrowser::GetItemBounds(
	const DataBrowserItem*	inItem, 
	DataBrowserPropertyID	inProperty, 
	DataBrowserPropertyPart	inPropertyPart) const
{
	::Rect		bounds;
	OSStatus	err;
	
	err = GetDataBrowserItemPartBounds(*this, 
									   reinterpret_cast<DataBrowserItemID>(inItem), 
									   inProperty, 
									   inPropertyPart, 
									   &bounds);
	B_THROW_IF_STATUS(err);
	
	return bounds;
}

#if 0	// DELETE
// ------------------------------------------------------------------------------------------
void
DataBrowser::GetItemBounds(
	const DataBrowserItem*	inItem, 
	DataBrowserPropertyID	inProperty, 
	DataBrowserPropertyPart	inPropertyPart, 
	::Rect&					outBounds) const
{
	OSStatus	err;
	
	err = GetDataBrowserItemPartBounds(*this, 
									   reinterpret_cast<DataBrowserItemID>(inItem), 
									   inProperty, 
									   inPropertyPart, 
									   &outBounds);
	B_THROW_IF_STATUS(err);
}
#endif

// ------------------------------------------------------------------------------------------
UInt16
DataBrowser::GetItemHeight(
	const DataBrowserItem*	inItem) const
{
	UInt16		height;
	OSStatus	err;
	
	err = GetDataBrowserTableViewItemRowHeight(*this, 
											   reinterpret_cast<DataBrowserItemID>(inItem), 
											   &height);
	B_THROW_IF_STATUS(err);
	
	return (height);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::SetItemHeight(
	const DataBrowserItem*	inItem, 
	UInt16					inHeight)
{
	OSStatus	err;
	
	err = SetDataBrowserTableViewItemRowHeight(*this, 
											   reinterpret_cast<DataBrowserItemID>(inItem), 
											   inHeight);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Call a function for each item.
*/
typedef struct
{
	DataBrowserItemProcPtr	proc;
	void*					arg;
} ItemIterInfo;

void
DataBrowser::ForEachItemDo(
	DataBrowserItem*		inParentItem, 
	bool					inRecurse,
	DataBrowserItemState	inState,
	DataBrowserItemProcPtr	inProc, 
	void*					inArg)
{
	static AutoDataBrowserItemUPP	sItem(ItemProc);
	ItemIterInfo	info;
	OSStatus		err;
	
	info.proc	= inProc;
	info.arg	= inArg;
	
	err = ForEachDataBrowserItem(*this, 
								 reinterpret_cast<DataBrowserItemID>(inParentItem),
								 inRecurse,
								 inState,
								 sItem,
								 reinterpret_cast<void*>(&info));
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Return the number of selected items in the data browser.
*/
UInt32
DataBrowser::CountSelectedItems() const
{
	return (CountItems(NULL, true, kDataBrowserItemIsSelected));
}

// ------------------------------------------------------------------------------------------
/*!	Fetch the "first" selected item.  This is actually a quickie way of 
	determining whether there are any selected items in the data browser.
*/
DataBrowserItem*
DataBrowser::GetFirstSelectedItem() const
{
	DataBrowserItemID	first	= kDataBrowserNoItem;
	DataBrowserItemID	last	= kDataBrowserNoItem;
	OSStatus			err;
	
	err = GetDataBrowserSelectionAnchor(*this, &first, &last);
	B_THROW_IF_STATUS(err);
	
	return (reinterpret_cast<DataBrowserItem*>(first));
}

// ------------------------------------------------------------------------------------------
/*!	Return the selected items.
*/
UInt32
DataBrowser::GetSelectedItems(
	ItemVector&				outItems) const
{
	return (GetItems(NULL, true, kDataBrowserItemIsSelected, outItems));
}

// ------------------------------------------------------------------------------------------
/*!	Select the given item.
*/
void
DataBrowser::SelectItem(
	DataBrowserItem*	inItem)
{
	ItemVector	items(1, inItem);
	
	SelectItems(items);
}

// ------------------------------------------------------------------------------------------
/*!	Select/Deselect/Toggle the items given in inItems, depending upon the 
	operation given in inOperation.  By default, the data browser's selection 
	set is replaced by the given items.
*/
void
DataBrowser::SelectItems(
	const ItemVector&		inItems, 
	DataBrowserSetOption	inOperation /* = kDataBrowserItemsAssign */)
{
	OSStatus		err;

	err = SetDataBrowserSelectedItems(*this, 
									  inItems.size(),
									  reinterpret_cast<const DataBrowserItemID*>(&inItems[0]),
									  inOperation);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Selects all items.
*/
void
DataBrowser::SelectAllItems()
{
	ItemVector	items;
	
	GetItems(NULL, true, kDataBrowserItemAnyState, items);
	SelectItems(items);
}

// ------------------------------------------------------------------------------------------
/*!	Unselect the given item.
*/
void
DataBrowser::UnselectItem(
	DataBrowserItem*	inItem)
{
	ItemVector	items(1, inItem);
	
	SelectItems(items, kDataBrowserItemsRemove);
}

// ------------------------------------------------------------------------------------------
/*!	Unselect the given items.
*/
void
DataBrowser::UnselectItems(
	const ItemVector&		inItems)
{
	SelectItems(inItems, kDataBrowserItemsRemove);
}

// ------------------------------------------------------------------------------------------
/*!	Unselects all items.
*/
void
DataBrowser::UnselectAllItems()
{
	ItemVector	items;
	
	GetItems(NULL, true, kDataBrowserItemIsSelected, items);
	SelectItems(items, kDataBrowserItemsRemove);
}

// ------------------------------------------------------------------------------------------
/*!	Select the given item, scrolling the data browser if necessary so that 
	the given property of the given item is visible.
*/
void
DataBrowser::RevealItem(
	DataBrowserItem*		inItem, 
	DataBrowserPropertyID	inProperty,
	bool					inCenterInView /* = false */)
{
	OSStatus	err;
	
	err = RevealDataBrowserItem(*this,
								reinterpret_cast<DataBrowserItemID>(inItem), 
								inProperty,
								inCenterInView);
	
	// Don't throw, because the CarbonLib implementation of 
	// RevealDataBrowserItem is broken and always returns an error for 
	// non-container items.  We'll just wait until Apple fixes this in 
	// CarbonLib.
	
#if 0
	B_THROW_IF_STATUS(err);
#endif
}

/*!	Open the given container.
*/
void
DataBrowser::OpenContainer(
	DataBrowserItem*	inContainerItem)
{
	OSStatus	err;
	
	err = OpenDataBrowserContainer(*this, 
								   reinterpret_cast<DataBrowserItemID>(inContainerItem));
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Close the given container.
*/
void
DataBrowser::CloseContainer(
	DataBrowserItem*	inContainerItem)
{
	OSStatus	err;
	
	err = CloseDataBrowserContainer(*this, 
									reinterpret_cast<DataBrowserItemID>(inContainerItem));
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	React to a selection change within the data browser.  By default, we 
	broadcast a "selection changed" signal.
*/
void
DataBrowser::SelectionChanged()
{
	mSelectionChangedSignal();
}

// ------------------------------------------------------------------------------------------
/*!	React to a user state change within the data browser.  By default, we 
	broadcast a "user state changed" signal.
*/
void
DataBrowser::UserStateChanged()
{
	mUserStateChangedSignal();
}

// ------------------------------------------------------------------------------------------
/*!	React to a double-click within the data browser.  By default, we 
	forward the message to the double-cliked item.
*/
void
DataBrowser::DoubleClickedItem(
	DataBrowserItem*		inItem)
{
	inItem->ItemDoubleClicked();
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::DisplayContextualMenu(
	MenuRef						inMenu,
	UInt32						inHelpType,
	CFStringRef					inHelpItemString,
	const AEDesc&				inSelection)
{
	
	Str255			helpStr;
	const AEDesc*	selPtr	= &inSelection;
	OSStatus		err;
	
	if ((inHelpItemString == NULL) || 
		!CFStringGetPascalString(inHelpItemString, helpStr, sizeof(helpStr), 
								 GetApplicationTextEncoding()))
	{
		helpStr[0] = 0;
	}
	
	if (selPtr->descriptorType == typeNull)
	{
		selPtr = NULL;
	}
	
	SInt16			menuId	= ::GetMenuID(inMenu);
	::Point			globalPt;
	UInt32			selType;
	SInt16			selMenuID;
	MenuItemIndex	selMenuItem;
	
	::InsertMenu(inMenu, hierMenu);
	::GetGlobalMouse(&globalPt);
	
	err = ::ContextualMenuSelect(inMenu,
								 globalPt,
								 false,
								 inHelpType,
								 helpStr,
								 selPtr,
								 &selType,
								 &selMenuID,
								 &selMenuItem);
	
	::DeleteMenu(menuId);
	
	if (err != noErr)
	{
		selType		= kCMNothingSelected;
		selMenuID	= 0;
		selMenuItem	= 0;
	}
	
	if (selType == kCMNothingSelected)
		selMenuID = 0;
	
	SelectContextualMenu(inMenu, selType, selMenuID, selMenuItem);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::OpenEditSession(
	DataBrowserItem*		inItem, 
	DataBrowserPropertyID	inProperty)
{
	OSStatus	err;
	
	err = SetDataBrowserEditItem(*this, reinterpret_cast<DataBrowserItemID>(inItem), 
								 inProperty);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::IsEditSessionOpen() const
{
    return (mEditItem != NULL);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::GetProperty(
	DataBrowserItemData&		inItemData,
	DataBrowserPropertyID		inProperty) const
{
	bool	handled	= true;
	
	switch (inProperty)
	{
//	case kDataBrowserItemIsActiveProperty:
//		//Trace("Getting %p [IsActive]", this);
//		inItemData.SetBoolean(IsActive());
//		break;
		
//	case kDataBrowserItemIsSelectableProperty:
//		//Trace("Getting %p [IsSelectable]", this);
//		inItemData.SetBoolean(IsSelectable());
//		break;
		
//	case kDataBrowserItemIsEditableProperty:
//		//Trace("Getting %p [IsEditable]", this);
//		inItemData.SetBoolean(IsEditable(inItemData.GetProperty()));
//		break;
		
//	case kDataBrowserItemIsContainerProperty:
//		//Trace("Getting %p [IsContainer]", this);
//		inItemData.SetBoolean(IsContainer());
//		break;
		
//	case kDataBrowserContainerIsOpenableProperty:
//		//Trace("Getting %p [IsOpenable]", this);
//		inItemData.SetBoolean(IsContainerOpenable());
//		break;
		
//	case kDataBrowserContainerIsClosableProperty:
//		//Trace("Getting %p [IsClosable]", this);
//		inItemData.SetBoolean(IsContainerClosable());
//		break;
		
	case kDataBrowserContainerIsSortableProperty:
		//Trace("Getting %p [IsSortable]", this);
		inItemData.SetBoolean(IsRootContainerSortable());
		break;
		
//	case kDataBrowserContainerAliasIDProperty:
//		//Trace("Getting %p [ContainerAliasID]", this);
//		inItemData.SetItemID(GetContainerAliasID());
//		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
/*!	Determines whether the root container is sortable.  Override it if you want to implement 
	a sortable top-level container.
*/
bool
DataBrowser::IsRootContainerSortable() const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	bool	handled	= false;
	
	if (mEditItem == NULL)
	{
		handled = HandleCommand(event.mHICommand);
		
		if (!handled)
		{
			// The command wasn't handled by us.
			
			HIViewRef		superView;
			EventTargetRef	parentTarget;
			
			if ((superView = HIViewGetSuperview(*this)) != NULL)
			{
				parentTarget = HIObjectGetEventTarget(reinterpret_cast<HIObjectRef>(superView));
			}
			else
			{
				parentTarget = GetWindowEventTarget(GetControlOwner(*this));
			}
			
			handled = (event.Send(parentTarget, std::nothrow) == noErr);
		}
	}
	else
	{
        // An edit session is open.  The Edit Menu commands are handled by the control.
        
    	DataBrowserEditCommand	browserCmd  = 0;
    	OSStatus                err;
    	
    	switch (event.mHICommand.commandID)
    	{
    	case kHICommandUndo:		browserCmd = kDataBrowserEditMsgUndo;		break;
    	case kHICommandCut:			browserCmd = kDataBrowserEditMsgCut;		break;
    	case kHICommandCopy:		browserCmd = kDataBrowserEditMsgCopy;		break;
    	case kHICommandPaste:		browserCmd = kDataBrowserEditMsgPaste;		break;
    	case kHICommandClear:		browserCmd = kDataBrowserEditMsgClear;		break;
    	case kHICommandSelectAll:	browserCmd = kDataBrowserEditMsgSelectAll;	break;
    	}
    	
    	if (browserCmd != 0)
    	{
    		err = ExecuteDataBrowserEditCommand(*this, browserCmd);
    		B_THROW_IF_STATUS(err);
    		
    		handled = true;
    	}
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	bool	handled	= false;
	
	if (mEditItem == NULL)
	{
		handled = HandleUpdateStatus(event.mHICommand, event.mData);
		
		if (!handled)
		{
			// The command wasn't handled by us.
			
			HIViewRef		superView;
			EventTargetRef	parentTarget;
			
			if ((superView = HIViewGetSuperview(*this)) != NULL)
			{
				parentTarget = HIObjectGetEventTarget(reinterpret_cast<HIObjectRef>(superView));
			}
			else
			{
				parentTarget = GetWindowEventTarget(GetControlOwner(*this));
			}
			
			handled = (event.Send(parentTarget, std::nothrow) == noErr);
		}
	}
	else
	{
        // An edit session is open.  The Edit Menu commands are handled by the control.
        
    	DataBrowserEditCommand	browserCmd  = 0;
    	
    	switch (event.mHICommand.commandID)
    	{
    	case kHICommandUndo:		browserCmd = kDataBrowserEditMsgUndo;		break;
    	case kHICommandCut:			browserCmd = kDataBrowserEditMsgCut;		break;
    	case kHICommandCopy:		browserCmd = kDataBrowserEditMsgCopy;		break;
    	case kHICommandPaste:		browserCmd = kDataBrowserEditMsgPaste;		break;
    	case kHICommandClear:		browserCmd = kDataBrowserEditMsgClear;		break;
    	case kHICommandSelectAll:	browserCmd = kDataBrowserEditMsgSelectAll;	break;
    	}
    	
    	if (browserCmd != 0)
    	{
    		event.mData.SetEnabled(EnableDataBrowserEditCommand(*this, browserCmd));
    		handled = true;
    	}
	}
	
	return (handled);
}


// ------------------------------------------------------------------------------------------
bool
DataBrowser::ControlGetOptimalBounds(
	Event<kEventClassControl, kEventControlGetOptimalBounds>&	event)
{
	bool	handled	= event.Forward();
	
	if (handled)
	{
		HIRect		origBounds	= event.mOptimalBounds;
		unsigned	numColumns	= CountColumns();
		HISize		optimalSize;
		Boolean		varWidthColumns, varHeightRows, horizScrollBar, vertScrollBar;
		SInt32		scrollBarWidth, scrollBarOverlap;
		OSStatus	err;
		
		err = GetDataBrowserTableViewGeometry(*this, &varWidthColumns, 
											  &varHeightRows);
		B_THROW_IF_STATUS(err);
		
		err = GetDataBrowserHasScrollBars(*this, &horizScrollBar, 
										  &vertScrollBar);
		B_THROW_IF_STATUS(err);
		
		err = GetThemeMetric(kThemeMetricScrollBarWidth, &scrollBarWidth);
		B_THROW_IF_STATUS(err);
		
		err = GetThemeMetric(kThemeMetricScrollBarOverlap, &scrollBarOverlap);
		B_THROW_IF_STATUS(err);
		
		if (varWidthColumns)
		{
			optimalSize.width = 0.0f;
			
			for (unsigned col = 0; col < numColumns; col++)
			{
				DataBrowserTableViewColumnID	columnID;
				
				columnID = GetColumnID(col);
				
				optimalSize.width += GetColumnWidth(columnID);
			}
			
			// ### EXPERIMENTAL ###
			optimalSize.width += 2 * scrollBarWidth - 1;
		}
		else
		{
			optimalSize.width = numColumns * GetColumnWidth();
		}
		
		if (vertScrollBar)
		{
			optimalSize.width += scrollBarWidth - scrollBarOverlap;
		}
		
		// {
		optimalSize.width = origBounds.size.width;
		// }
		
		optimalSize.height	= GetHeaderHeight();
		
		if (varHeightRows)
		{
			ItemVector	items;
			
			GetItems(NULL, true, kDataBrowserItemAnyState, items);
			
			for (ItemVector::const_iterator it = items.begin(); 
				 it != items.end(); 
				 ++it)
			{
				optimalSize.height += (*it)->GetHeight();
			}
		}
		else
		{
			unsigned	numRows	= CountItems(NULL, true, kDataBrowserItemAnyState);
			
			optimalSize.height += numRows * GetRowHeight();
		}
		
		if (horizScrollBar)
		{
			optimalSize.height += scrollBarWidth - scrollBarOverlap;
		}
		
		event.mOptimalBounds.origin	= origBounds.origin;
		event.mOptimalBounds.size	= optimalSize;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::HandleCommand(
	const HICommandExtended&	/* inHICommand */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::HandleUpdateStatus(
	const HICommandExtended&	/* inHICommand */, 
	CommandData&				/* ioCmdData */)
{
	return (false);
}


#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	This is called whenever the browser wants to get/set data
	for the indicated item.  The default behavior is to tell the item
	to get or set its own data; we don't know what the item contains.
*/
void
DataBrowser::GetSetItemData(
	DataBrowserItem*			inItem,
	DataBrowserPropertyID		inProperty,
	DataBrowserItemDataRef		inItemData,
	bool						inChangeValue)
{
	DataBrowserItemData	itemData(inItemData);
	bool				handled;
	
	if (inItem != NULL)
	{
		if (inChangeValue)
		{
			handled = inItem->SetData(itemData, inProperty);
		}
		else
		{
			if (inProperty >= 1000)
				handled = inItem->GetData(itemData, inProperty);
			else
				handled = inItem->GetProperty(itemData, inProperty);
		}
	}
	else
	{
		B_ASSERT(!inChangeValue);
		B_ASSERT(inProperty < 1000);
		
		handled = GetProperty(itemData, inProperty);
	}
	
	if (!handled)
	{
		B_THROW_STATUS(errDataBrowserPropertyNotSupported);
	}
}

// ------------------------------------------------------------------------------------------
/*!	This is called whenever the browser wants to compare two items.
*/
bool
DataBrowser::Compare(
	DataBrowserItem*			inItem1,
	DataBrowserItem*			inItem2,
	DataBrowserPropertyID		inSortProperty)
{
	B_ASSERT(inItem1 != NULL);
	B_ASSERT(inItem2 != NULL);
	
	int	result	= inItem1->CompareTo(*inItem2, inSortProperty);
	
	return (result < 0);
}

// ------------------------------------------------------------------------------------------
/*!	This is called whenever the user does something with an item. The default
	behavior is to pass along the notification to the item.
*/
void
DataBrowser::ItemNotification(
	DataBrowserItem*			inItem,
	DataBrowserItemNotification	inMessage)
{
	switch (inMessage)
	{
	case kDataBrowserItemAdded:
		//printf("Item Added %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ItemAdded();
		break;
		
	case kDataBrowserItemRemoved:
		//printf("Item Removed %p", inItem);
		inItem->ItemRemoved();
		break;
		
	case kDataBrowserEditStarted:
		//printf("Edit Started %p", inItem);
		B_ASSERT(inItem != NULL);
		mEditItem = inItem;
		inItem->EditStarted();
		break;
		
	case kDataBrowserEditStopped:
		//printf("Edit Stopped %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->EditStopped();
		mEditItem = NULL;
		break;
		
	case kDataBrowserItemSelected:
		//printf("Item Selected %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ItemSelected();
		break;
		
	case kDataBrowserItemDeselected:
		//printf("Item Deselected %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ItemDeselected();
		break;
		
	case kDataBrowserItemDoubleClicked:
		//printf("Item DoubleClicked %p", inItem);
		B_ASSERT(inItem != NULL);
		DoubleClickedItem(inItem);
		break;
		
	case kDataBrowserContainerOpened:
		//printf("Container Opened %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ContainerOpened();
		break;
		
	case kDataBrowserContainerClosing:
		//printf("Container Closing %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ContainerClosing();
		break;
		
	case kDataBrowserContainerClosed:
		//printf("Container Closed %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ContainerClosed();
		break;
		
	case kDataBrowserContainerSorting:
		//printf("Container Sorting %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ContainerSorting();
		break;
		
	case kDataBrowserContainerSorted:
		//printf("Container Sorted %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->ContainerSorted();
		break;
		
	case kDataBrowserUserToggledContainer:
		//printf("User Toggled Container %p", inItem);
		B_ASSERT(inItem != NULL);
		inItem->UserToggledContainer();
		break;
		
	case kDataBrowserTargetChanged:
		//printf("Target Changed");
		B_ASSERT(inItem != NULL);
		inItem->TargetChanged();
		break;
		
	case kDataBrowserUserStateChanged:
		printf("User State Changed\n");
		UserStateChanged();
		break;
		
	case kDataBrowserSelectionSetChanged:
		//printf("Selection Changed");
		SelectionChanged();
		break;
	}
}

// ------------------------------------------------------------------------------------------
/*!	This is called when the user drags an item. The default behavior is to
	tell the item to add whatever drag flavors are appropriate for itself.
*/
bool
DataBrowser::AddDragItem(
	Drag&						ioDrag,
	DataBrowserItem*			inItem,
	PasteboardItemID&			outItemID)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->AddDragFlavors(ioDrag, outItemID));
}

// ------------------------------------------------------------------------------------------
/*!	This is called when the user initially drags something into the list,
	and the control needs to know whether the drag is acceptable. The inItem
	parameter is NULL if the drag is going into the top level of the hierarchy,
	otherwise it's the container object into which things are being dragged.
*/
bool
DataBrowser::IsDragOnBrowserAcceptable(
	const Drag&					/* inDrag */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	This is called when the user initially drags something into the list,
	and the control needs to know whether the drag is acceptable. The inItem
	parameter is NULL if the drag is going into the top level of the hierarchy,
	otherwise it's the container object into which things are being dragged.
*/
bool
DataBrowser::IsDragOnItemAcceptable(
	const Drag&					inDrag,
	const DataBrowserItem*		inItem)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->IsDragAcceptable(inDrag));
}

// ------------------------------------------------------------------------------------------
/*!	This is called after the user drops something into the list. The inItem
	parameter is NULL if the drop is going into the top level of the hierarchy,
	otherwise it's the container object into which things have been dropped.

	The drag contains items that are acceptable, as defined by the
	DragIsAcceptable() and ItemIsAcceptable() member functions.
*/
bool
DataBrowser::ReceiveDragOnBrowser(
	const Drag&					/* inDrag */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	This is called after the user drops something into the list. The inItem
	parameter is NULL if the drop is going into the top level of the hierarchy,
	otherwise it's the container object into which things have been dropped.

	The drag contains items that are acceptable, as defined by the
	DragIsAcceptable() and ItemIsAcceptable() member functions.
*/
bool
DataBrowser::ReceiveDragOnItem(
	const Drag&			inDrag,
	DataBrowserItem*	inItem)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->ReceiveDrag(inDrag));
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::PostProcessDrag(
	const Drag&			/* inDrag */,
	OSStatus			/* inTrackDragResult */)
{
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::GetContextualMenu(
	MenuRef&			/* outMenuRef */,
	UInt32&				/* outHelpType */,
	CFStringRef&		/* outHelpItemString */,
	AEDesc&				/* outSelection */)
{
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::SelectContextualMenu(
	MenuRef				/* inMenuRef */,
	UInt32				/* inSelectionType */,
	SInt16				/* inMenuID */,
	MenuItemIndex		/* inMenuItem */)
{
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::KeyFilter(
	UInt16&					ioKeyCode, 
	UInt16&					ioCharCode, 
	EventModifiers&			ioModifiers, 
	bool&					outPassUp)
{
//	bool	letKeyThrough;
	
	if (ioModifiers & cmdKey)
	{
		// Always pass up when the command key is down
//		mLastKeyStatus = keyStatus_PassUp;
	}
	else
	{
		if (mEditItem != NULL)
		{
			if (mEditItem->KeyFilter(ioKeyCode, ioCharCode, 
									 ioModifiers, outPassUp))
			{
//				mLastKeyStatus = keyStatus_Input;
			}
			else if (outPassUp)
			{
//				mLastKeyStatus = keyStatus_PassUp;
			}
			else
			{
//				mLastKeyStatus = keyStatus_Reject;
			}
		}
		
//		if (((mEditItem == NULL) || (mLastKeyStatus == keyStatus_PassUp)) && 
//			(mKeyFilter != NULL))
//		{
//			mLastKeyStatus = (*mKeyFilter)(NULL, ioKeyCode | ioCharCode, 
//										   ioCharCode, ioModifiers);
//		}
	}
	
//	letKeyThrough = (mLastKeyStatus == keyStatus_Input);
	
//	return (letKeyThrough);
	return (true);
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::CustomItemDraw(
	DataBrowserItem*			inItem,
	DataBrowserPropertyID		inProperty, 
	DataBrowserItemState		inItemState, 
	const Rect&					inRect, 
	SInt16						inDepth, 
	Boolean						inColorDevice)
{
	B_ASSERT(inItem != NULL);
	
	inItem->CustomDraw(inProperty, inItemState, inRect, inDepth, inColorDevice);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::CustomItemEdit(
	DataBrowserItem*			inItem,
	DataBrowserPropertyID		inProperty, 
	const String&				inString, 
	Rect&						outMaxEditTextRect, 
	bool&						outShrinkToFit)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->CustomEdit(inProperty, inString, outMaxEditTextRect, outShrinkToFit));
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::CustomItemHitTest(
	DataBrowserItem*			inItem,
	DataBrowserPropertyID		inProperty, 
	const Rect&					inRect, 
	const Rect&					inMouseRect)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->CustomHitTest(inProperty, inRect, inMouseRect));
}

// ------------------------------------------------------------------------------------------
DataBrowserTrackingResult
DataBrowser::CustomItemTrack(
	DataBrowserItem*			inItem,
	DataBrowserPropertyID		inProperty, 
	const Rect&					inRect, 
	const Point&				inStartPt, 
	EventModifiers				inModifiers)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->CustomTrack(inProperty, inRect, inStartPt, inModifiers));
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::CustomItemDragRgn(
	DataBrowserItem*			inItem,
	DataBrowserPropertyID		inProperty, 
	const Rect&					inRect, 
	RgnHandle					ioDragRgn)
{
	B_ASSERT(inItem != NULL);
	
	inItem->CustomDragRgn(inProperty, inRect, ioDragRgn);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::CustomItemAcceptDrag(
	DataBrowserItem*		inItem,
	DataBrowserPropertyID	inProperty, 
	const Rect&				inRect, 
	DragRef					inDragRef)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->CustomAcceptDrag(inProperty, inRect, inDragRef));
}

// ------------------------------------------------------------------------------------------
bool
DataBrowser::CustomItemReceiveDrag(
	DataBrowserItem*		inItem,
	DataBrowserPropertyID	inProperty, 
	DataBrowserDragFlags	inDragFlags, 
	DragRef					inDragRef)
{
	B_ASSERT(inItem != NULL);
	
	return (inItem->CustomReceiveDrag(inProperty, inDragFlags, inDragRef));
}

// ------------------------------------------------------------------------------------------
void
DataBrowser::CancelEdit() const
{
    if (mEditItem != NULL)
    {
        OSStatus    err;
        
        err = SetDataBrowserEditItem(*this, kDataBrowserNoItem, 0);
    }
}


#pragma mark -

// ------------------------------------------------------------------------------------------
inline DataBrowser*	GetBrowser(HIViewRef inCtrlRef)
{
	return (View::GetTypedViewFromRef<DataBrowser>(inCtrlRef));
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	GetSetItemData method for the DataBrowser instance.
*/
pascal OSStatus
DataBrowser::GetSetItemDataProc(
	HIViewRef					inBrowser, 
	DataBrowserItemID			inItemID, 
	DataBrowserPropertyID		inProperty, 
	DataBrowserItemDataRef		inItemData,
	Boolean						inChangeValue)
{
	OSStatus	err	= noErr;
	
	try
	{
		DataBrowser*	browser	= GetBrowser(inBrowser);
		
		browser->GetSetItemData(GetItem(inItemID), 
								inProperty, 
								inItemData, 
								inChangeValue);
	}
	catch (std::exception& ex)
	{
		err = B::ErrorHandler::GetStatus(ex, errDataBrowserPropertyNotSupported);
	}
	catch (...)
	{
		err = errDataBrowserPropertyNotSupported;	// for want of a better error code
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	Compare method for the DataBrowser instance.
*/
pascal Boolean
DataBrowser::CompareProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID1, 
	DataBrowserItemID			inItemID2,
	DataBrowserPropertyID		inSortProperty)
{
	Boolean	result;
	
	try
	{
		DataBrowser*	browser	= GetBrowser(inBrowser);
		
		result = browser->Compare(GetItem(inItemID1), 
								  GetItem(inItemID2), 
								  inSortProperty);
	}
	catch (...)
	{
		result = false;
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	ItemNotification method for the DataBrowser instance.
*/
pascal void
DataBrowser::ItemNotificationProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserItemNotification	inMessage)
{
	try
	{
		DataBrowser*	browser	= GetBrowser(inBrowser);
		
		browser->ItemNotification(GetItem(inItemID), inMessage);
	}
	catch (...)
	{
		// there's nothing to do
	}
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	AddDragItemSelf method for the DataBrowser instance.
*/
pascal Boolean
DataBrowser::AddDragItemProc(
	HIViewRef					inBrowser,
	DragRef						inDragRef,
	DataBrowserItemID			inItemID,
	DragItemRef*				outItemRef)
{
	Boolean	result;
	
	try
	{
		DataBrowser*		browser	= GetBrowser(inBrowser);
		Drag				drag(inDragRef);
		PasteboardItemID	itemID;
		
		result = browser->AddDragItem(drag, GetItem(inItemID), itemID);
		
		if (result)
			*outItemRef = Drag::MakeDragItemRef(itemID);
	}
	catch (...)
	{
		result = false;
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	AcceptDragSelf method for the DataBrowser instance.
*/
pascal Boolean
DataBrowser::AcceptDragProc(
	HIViewRef					inBrowser,
	DragRef						inDragRef,
	DataBrowserItemID			inItemID)
{
	Boolean	result;
	
	try
	{
		DataBrowser*		browser	= GetBrowser(inBrowser);
		DataBrowserItem*	item	= GetItem(inItemID);
		Drag				drag(inDragRef);
		
		if (item != NULL)
			result = browser->IsDragOnItemAcceptable(drag, item);
		else
			result = browser->IsDragOnBrowserAcceptable(drag);
	}
	catch (...)
	{
		result = false;
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	ReceiveDragSelf method for the DataBrowser instance.
*/
pascal Boolean
DataBrowser::ReceiveDragProc(
	HIViewRef					inBrowser,
	DragRef						inDragRef,
	DataBrowserItemID			inItemID)
{
	Boolean	result;
	
	try
	{
		DataBrowser*		browser	= GetBrowser(inBrowser);
		DataBrowserItem*	item	= GetItem(inItemID);
		
		if (item != NULL)
			result = browser->ReceiveDragOnItem(inDragRef, item);
		else
			result = browser->ReceiveDragOnBrowser(inDragRef);
	}
	catch (...)
	{
		result = false;
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	PostProcessDragSelf method for the DataBrowser instance.
*/
pascal void
DataBrowser::PostProcessDragProc(
	HIViewRef					inBrowser,
	DragRef						inDragRef,
	OSStatus					inTrackDragResult)
{
	try
	{
		DataBrowser*		browser	= GetBrowser(inBrowser);
		
		browser->PostProcessDrag(inDragRef, 
								 inTrackDragResult);
	}
	catch (...)
	{
		// there's nothing to do
	}
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	GetContextualMenuSelf method for the DataBrowser instance.
*/
pascal void
DataBrowser::GetContextualMenuProc(
	HIViewRef					inBrowser,
	MenuRef*					outMenu,
	UInt32*						outHelpType,
	CFStringRef*				outHelpItemString,
	AEDesc*						outSelection)
{
#if 1

	try
	{
		DataBrowser*	browser	= GetBrowser(inBrowser);
		
		*outMenu			= NULL;
		*outHelpType		= kCMHelpItemNoHelp;
		*outHelpItemString	= NULL;
		
		AEInitializeDesc(outSelection);
		
		browser->GetContextualMenu(*outMenu, 
								   *outHelpType, 
								   *outHelpItemString, 
								   *outSelection);
	}
	catch (...)
	{
		// there's nothing to do
	}

#else

	try
	{
		// ### HACK ALERT !!! ###
		// 
		// The standard window handler's treatment of contextual menu 
		// clicks is currently broken with regards to the data browser.
		// It ends up sending the mouse location in global coordinates 
		// instead of local coordinates, which means that the contextual
		// menu is displayed at the wrong location.  To work around this 
		// bug, we display and dispatch the contextual menu ourselves.
		// 
		// We should revise this if Apple ever fixes the bug.
		
		DataBrowser*		browser		= GetBrowser(inBrowser);
		MenuRef				menu		= NULL;
		UInt32				helpType	= kCMHelpItemNoHelp;
		CFStringRef			helpString	= NULL;
		AEDesc				selection	= { typeNull, NULL };
		
		browser->GetContextualMenu(menu, 
								   helpType, 
								   helpString, 
								   selection);
		
		if (menu != NULL)
		{
			browser->DisplayContextualMenu(menu, 
										   helpType, 
										   helpString, 
										   selection);
		}
	}
	catch (...)
	{
		// there's nothing to do
	}

#endif
}

// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	SelectContextualMenuSelf method for the DataBrowser instance.
*/
pascal void
DataBrowser::SelectContextualMenuProc(
	HIViewRef					inBrowser,
	MenuRef						inMenu,
	UInt32						inSelectionType,
	SInt16						inMenuID,
	MenuItemIndex				inMenuItem)
{
	try
	{
		DataBrowser*	browser	= GetBrowser(inBrowser);
		
		browser->SelectContextualMenu(inMenu, 
									  inSelectionType, 
									  inMenuID, 
									  inMenuItem);
	}
	catch (...)
	{
		// there's nothing to do
	}
}

// ------------------------------------------------------------------------------------------
pascal void
DataBrowser::ItemProc(
	DataBrowserItemID			inItemID, 
	DataBrowserItemState		inState,
	void*						inClientData)
{
	ItemIterInfo*	info	= reinterpret_cast<ItemIterInfo*>(inClientData);
	
	try
	{
		(*info->proc)(inItemID, inState, info->arg);
	}
	catch (...)
	{
		// there's nothing to do
	}
}

static void	ConvertHelpContent(
	const B::String&	inContent,
	bool				inLocalized,
	HMHelpContent&		outHelp)
{
	if (inContent.empty())
	{
		outHelp.contentType = kHMNoContent;
	}
	else if (inLocalized)
	{
		outHelp.contentType		= kHMCFStringLocalizedContent;
		outHelp.u.tagCFString	= inContent.cf_ptr().release();
	}
	else
	{
		outHelp.contentType		= kHMCFStringContent;
		outHelp.u.tagCFString	= inContent.cf_ptr().release();
	}
}

static void	DisposeHelpContent(
	const HMHelpContent&	inHelp)
{
	if (((inHelp.contentType == kHMCFStringContent) || 
		 (inHelp.contentType == kHMCFStringLocalizedContent)) && 
		 (inHelp.u.tagCFString != NULL))
	{
		CFRelease(inHelp.u.tagCFString);
	}
}
	
// ------------------------------------------------------------------------------------------
/*!	Called by the data browser control, this static routine calls the
	GetHelpContentSelf method for the DataBrowser instance.
*/
pascal void
DataBrowser::GetHelpContentProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserPropertyID		inProperty,
	HMContentRequest			inRequest,
	HMContentProvidedType*		outContentProvided,
	HMHelpContentPtr			ioHelpContent)
{
	try
	{
		DataBrowser*		browser	= GetBrowser(inBrowser);
		DataBrowserItem*	item	= browser->GetItem(inItemID);
		
		B_ASSERT(item != NULL);
		
		if (inRequest == kHMSupplyContent)
		{
			String	minContent, maxContent;
			bool	minLocalized = false, maxLocalized = false;
			
			if (item->SupplyHelpContent(inProperty, minContent, minLocalized, maxContent, maxLocalized))
			{
				ConvertHelpContent(minContent, minLocalized, ioHelpContent->content[kHMMinimumContentIndex]);
				ConvertHelpContent(maxContent, maxLocalized, ioHelpContent->content[kHMMaximumContentIndex]);
				
				*outContentProvided = kHMContentProvided;
			}
			else
			{
				*outContentProvided = kHMContentNotProvided;
			}
		}
		else if (inRequest == kHMDisposeContent)
		{
			DisposeHelpContent(ioHelpContent->content[kHMMinimumContentIndex]);
			DisposeHelpContent(ioHelpContent->content[kHMMaximumContentIndex]);
		}
	}
	catch (...)
	{
		// there's nothing to do
	}
}


#if 0
// ------------------------------------------------------------------------------------------
pascal ControlKeyFilterResult
DataBrowser::KeyFilterProc(
	HIViewRef					inBrowser,
	SInt16*						ioKeyCode, 
	SInt16*						ioCharCode, 
	EventModifiers*				ioModifiers)
{
	ControlKeyFilterResult	result	= kControlKeyFilterBlockKey;
	
	try
	{
		DataBrowser*		browser	= GetBrowser(inBrowser);
		bool				passUp;
		
		if (browser->KeyFilter(*reinterpret_cast<UInt16*>(ioKeyCode), 
							   *reinterpret_cast<UInt16*>(ioCharCode), 
							   *ioModifiers, passUp))
		{
			result = kControlKeyFilterPassKey;
		}
	}
	catch (...)
	{
		// there's nothing to do
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
/*!	Key Filter for navigation characters

		> Accept navigation keys (left, right, up, down, page up, page down, 
		  home, end)
		> PassUp all other characters
*/
EKeyStatus
DataBrowser::NavigationKeyFilterProc(
	TEHandle		/* inMacTEH */,
	UInt16			inKeyCode,
	UInt16&			/* ioCharCode */,
	EventModifiers	/* inModifiers */)
{
	return (UKeyFilters::IsNavigationKey(inKeyCode) 
			? keyStatus_Input 
			: keyStatus_PassUp);
}
#endif

// ------------------------------------------------------------------------------------------
pascal void
DataBrowser::DrawItemProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserPropertyID		inProperty, 
	DataBrowserItemState		inItemState, 
	const ::Rect*				inRect, 
	SInt16						inDepth, 
	Boolean						inColorDevice)
{
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		Rect				rect(*inRect);
		
		browser->CustomItemDraw(item, inProperty, inItemState, rect, inDepth, 
								inColorDevice);
	}
	catch (...)
	{
		// there's nothing to do
	}
}

// ------------------------------------------------------------------------------------------
pascal Boolean
DataBrowser::EditItemProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserPropertyID		inProperty, 
	CFStringRef					inString, 
	::Rect*						outMaxEditTextRect, 
	Boolean*					outShrinkToFit)
{
	bool	result	= false;
	
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		String				str(inString);
		Rect				maxEditTextRect(*outMaxEditTextRect);
		bool				shrinkToFit	= *outShrinkToFit;
		
		result = browser->CustomItemEdit(item, inProperty, str, maxEditTextRect, 
										 shrinkToFit);
		
		if (result)
		{
			*outMaxEditTextRect	= maxEditTextRect;
			*outShrinkToFit		= shrinkToFit;
		}
	}
	catch (...)
	{
		// there's nothing to do
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
pascal Boolean
DataBrowser::HitTestProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserPropertyID		inProperty, 
	const ::Rect*				inRect, 
	const ::Rect*				inMouseRect)
{
	bool	result	= false;
	
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		Rect				rect(*inRect);
		Rect				mouseRect(*inMouseRect);
		
		result = browser->CustomItemHitTest(item, inProperty, rect, mouseRect);
	}
	catch (...)
	{
		// there's nothing to do
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
pascal DataBrowserTrackingResult
DataBrowser::TrackingProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserPropertyID		inProperty, 
	const ::Rect*				inRect, 
	::Point						inStartPt, 
	EventModifiers				inModifiers)
{
	DataBrowserTrackingResult	result	= kDataBrowserNothingHit;
	
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		Rect				rect(*inRect);
		Point				startPt(inStartPt);
		
		result = browser->CustomItemTrack(item, inProperty, rect, startPt, inModifiers);
	}
	catch (...)
	{
		// there's nothing to do
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
pascal void
DataBrowser::ItemDragRgnProc(
	HIViewRef					inBrowser,
	DataBrowserItemID			inItemID,
	DataBrowserPropertyID		inProperty, 
	const ::Rect*				inRect, 
	RgnHandle					ioDragRgn)
{
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		Rect				rect(*inRect);
		
		browser->CustomItemDragRgn(item, inProperty, rect, ioDragRgn);
	}
	catch (...)
	{
		// there's nothing to do
	}
}

// ------------------------------------------------------------------------------------------
pascal DataBrowserDragFlags
DataBrowser::ItemAcceptDragProc(
	HIViewRef				inBrowser,
	DataBrowserItemID		inItemID,
	DataBrowserPropertyID	inProperty, 
	const ::Rect*			inRect, 
	DragRef					inDragRef)
{
	DataBrowserDragFlags	result	= 0;
	
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		Rect				rect(*inRect);
		
		result = browser->CustomItemAcceptDrag(item, inProperty, rect, inDragRef) ? 1 : 0;
	}
	catch (...)
	{
		// there's nothing to do
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
pascal Boolean
DataBrowser::ItemReceiveDragProc(
	HIViewRef				inBrowser,
	DataBrowserItemID		inItemID,
	DataBrowserPropertyID	inProperty, 
	DataBrowserDragFlags	inDragFlags, 
	DragRef					inDragRef)
{
	bool	result	= false;
	
	try
	{
		DataBrowser*		browser		= GetBrowser(inBrowser);
		DataBrowserItem*	item		= browser->GetItem(inItemID);
		
		result = browser->CustomItemReceiveDrag(item, inProperty, inDragFlags, inDragRef);
	}
	catch (...)
	{
		// there's nothing to do
	}
	
	return (result);
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT ToolboxViewRegistrar<DataBrowser>	gRegisterDataBrowser(kHIDataBrowserClassID);
#endif

}	// namespace B
