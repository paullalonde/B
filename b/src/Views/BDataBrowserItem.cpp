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
#include "BDataBrowserItem.h"

// B headers
#include "BDataBrowser.h"
#include "BDataBrowserItemData.h"


namespace B {

// ------------------------------------------------------------------------------------------
DataBrowserItem::DataBrowserItem()
	: mDataBrowser(NULL), mParentItem(NULL)
{
}

// ------------------------------------------------------------------------------------------
DataBrowserItem::~DataBrowserItem()
{
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::SetBrowser(
	DataBrowser*			inBrowser)
{
	mDataBrowser = inBrowser;
}

// ------------------------------------------------------------------------------------------
DataBrowserItemState
DataBrowserItem::GetState() const
{
	B_ASSERT(mDataBrowser != NULL);
	
	return (mDataBrowser->GetItemState(this));
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::IsSelected() const
{
	return ((GetState() & kDataBrowserItemIsSelected) != 0);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::IsOpen() const
{
	return ((GetState() & kDataBrowserContainerIsOpen) != 0);
}

// ------------------------------------------------------------------------------------------
UInt16
DataBrowserItem::GetHeight() const
{
	return (mDataBrowser->GetItemHeight(this));
}

// ------------------------------------------------------------------------------------------
::Rect
DataBrowserItem::GetBounds(
	DataBrowserPropertyID	inProperty, 
	DataBrowserPropertyPart	inPropertyPart) const
{
	return mDataBrowser->GetItemBounds(this, inProperty, inPropertyPart);
}

#if 0	// DELETE
// ------------------------------------------------------------------------------------------
void
DataBrowserItem::GetBounds(
	DataBrowserPropertyID	inProperty, 
	DataBrowserPropertyPart	inPropertyPart, 
	::Rect&					outBounds) const
{
	mDataBrowser->GetItemBounds(this, inProperty, inPropertyPart, outBounds);
}
#endif

// ------------------------------------------------------------------------------------------
/*!	Opens the item (assuming it's a container).
*/
void
DataBrowserItem::Open()
{
	B_ASSERT(mDataBrowser != NULL);
	
	mDataBrowser->OpenContainer(this);
}

// ------------------------------------------------------------------------------------------
/*!	Closes the item (assuming it's a container).
*/
void
DataBrowserItem::Close()
{
	B_ASSERT(mDataBrowser != NULL);
	
	mDataBrowser->CloseContainer(this);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::Select()
{
	B_ASSERT(mDataBrowser != NULL);
	
	mDataBrowser->SelectItem(this);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::Reveal(
	DataBrowserPropertyID	inProperty, 
	bool					inCenterInView /* = false */)
{
	B_ASSERT(mDataBrowser != NULL);
	
	mDataBrowser->RevealItem(this, inProperty, inCenterInView);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::Update(
	DataBrowserPropertyID		inProperty)
{
	B_ASSERT(mDataBrowser != NULL);
	
	mDataBrowser->UpdateItem(this, inProperty);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	active.  Override it if you want inactive items.
*/
bool
DataBrowserItem::IsActive() const
{
	return (true);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	selectable.  Override it if you want selectable items.
*/
bool
DataBrowserItem::IsSelectable() const
{
	return (true);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	editable.  Override it if you want editable items.
*/
bool
DataBrowserItem::IsEditable(
	DataBrowserPropertyID	/* inProperty */) const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	a container.  Override it if you want to implement containers.
*/
bool
DataBrowserItem::IsContainer() const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	a container and is openable.  Override it if you want to implement 
	openable containers.
*/
bool
DataBrowserItem::IsContainerOpenable() const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	a container and is closeable.  Override it if you want to implement 
	closeable containers.
*/
bool
DataBrowserItem::IsContainerClosable() const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function to determine if the item is 
	a container and is sortable.  Override it if you want to implement 
	sortable containers.
*/
bool
DataBrowserItem::IsContainerSortable() const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
DataBrowserItemID
DataBrowserItem::GetContainerAliasID() const
{
	return (kDataBrowserNoItem);
}

// ------------------------------------------------------------------------------------------
DataBrowserPropertyID
DataBrowserItem::GetMainProperty() const
{
	return (kDataBrowserItemNoProperty);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function just after the item has been 
	added to the browser.  Override it to perform any special processing.
*/
void
DataBrowserItem::ItemAdded()
{
    //Trace("DataBrowserItem::ItemAdded(%p)", this);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function just after it removes the 
	item from the browser.  Override it to perform any special processing.  
	The default behaviour is to delete the object.
*/
void
DataBrowserItem::ItemRemoved()
{
    //Trace("DataBrowserItem::ItemRemoved(%p)", this);
    
	delete this;
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it selects the item.  
	Override it to perform any special processing.
*/
void
DataBrowserItem::ItemSelected()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it deselects the item.  
	Override it to perform any special processing.
*/
void
DataBrowserItem::ItemDeselected()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when the user double-clicks 
	the item.  Override it to perform any special processing.
*/
void
DataBrowserItem::ItemDoubleClicked()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it has opened the item 
	(which is assumed to be a container).  Override it to perform any special 
	processing.  For example, a derived class that implements container 
	hierarchies will probably want to populate the container from within an 
	override of this function.
*/
void
DataBrowserItem::ContainerOpened()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it is about to close the 
	item (which is assumed to be a container).  Override it to perform any 
	special processing.
*/
void
DataBrowserItem::ContainerClosing()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it has finished closing 
	the item (which is assumed to be a container).  Override it to perform 
	any special processing.
*/
void
DataBrowserItem::ContainerClosed()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it is about to sort the 
	item (which is assumed to be a container).  Override it to perform any 
	special processing.
*/
void
DataBrowserItem::ContainerSorting()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it has finished sorting 
	the item (which is assumed to be a container).  Override it to perform 
	any special processing.
*/
void
DataBrowserItem::ContainerSorted()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::UserToggledContainer()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::TargetChanged()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::EditStarted()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::EditStopped()
{
	// default implementation does nothing
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it wants to compare two 
	items (in order to sort them).  inProperty is the property (i.e., column) 
	on which to perform the comparison.  You need to override this member 
	function in your derived class if you want to implement sorted columns.
	The function should return a value greater than, equal to, or less than 
	zero, depending on whether the item compares greater than, equal to, or 
	less than inItem.
*/
int
DataBrowserItem::CompareTo(
	const DataBrowserItem&	/* inItem */,
	DataBrowserPropertyID	/* inProperty */) const
{
	// default implementation does nothing
	
	return (0);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it wants to display one 
	column's worth of data for one item.  You need to override this member 
	function in order to supply the data for your column properties.
*/
bool
DataBrowserItem::GetData(
	DataBrowserItemData&		/* inItemData */,
	DataBrowserPropertyID		/* inProperty */) const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The data browser calls this member function when it wants to modify one 
	column's worth of data for one item.  You need to override this member 
	function in order to fetch the data for your column properties.
*/
bool
DataBrowserItem::SetData(
	DataBrowserItemData&		/* inItemData */,
	DataBrowserPropertyID		/* inProperty */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	This item is being dragged.
	Add this item's flavor data (or promised flavors) to the drag reference.
*/
bool
DataBrowserItem::AddDragFlavors(
	Drag&				/* ioDrag */,
	PasteboardItemID&	/* outItemID */) const
{
	// must override!
	return false;
}

// ------------------------------------------------------------------------------------------
/*!	Returns whether a given drag to this item is acceptable.
	This will be false unless the item is a container.
*/
bool
DataBrowserItem::IsDragAcceptable(
	const Drag&			/* inDrag */) const
{
	// must override to check for the flavor types your subclass supports!
	return false;
}

// ------------------------------------------------------------------------------------------
/*!	Receive the given drag, and return success or failure.
*/
bool
DataBrowserItem::ReceiveDrag(
	const Drag&			/* inDrag */)
{
	// must override to receive the flavor types your subclass supports!
	return false;
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::SupplyHelpContent(
	DataBrowserPropertyID	/* inProperty */,
	String&					/* outMinContent */,
	bool&					/* outMinContentIsLocalized */,
	String&					/* outMaxContent */,
	bool&					/* outMaxContentIsLocalized */)
{
	return (false);
}

// ------------------------------------------------------------------------------------------
/*!	The function determines if the character given in ioKeyCode/ioCharCode is 
	acceptable.  Returns true if the character is accepted.  If it isn't, 
	outPassUp determines if the character should be passed to the browser and 
	on up through the commander chain.
*/
bool
DataBrowserItem::KeyFilter(
	UInt16&					/* ioKeyCode */, 
	UInt16&					/* ioCharCode */, 
	EventModifiers&			/* ioModifiers */, 
	bool&					outPassUp)
{
	outPassUp = true;
	
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::GetProperty(
	DataBrowserItemData&		inItemData,
	DataBrowserPropertyID		inProperty) const
{
	bool	handled	= true;
	
	switch (inProperty)
	{
	case kDataBrowserItemIsActiveProperty:
		//Trace("Getting %p [IsActive]", this);
		inItemData.SetBoolean(IsActive());
		break;
		
	case kDataBrowserItemIsSelectableProperty:
		//Trace("Getting %p [IsSelectable]", this);
		inItemData.SetBoolean(IsSelectable());
		break;
		
	case kDataBrowserItemIsEditableProperty:
		//Trace("Getting %p [IsEditable]", this);
		inItemData.SetBoolean(IsEditable(inItemData.GetProperty()));
		break;
		
	case kDataBrowserItemIsContainerProperty:
		//Trace("Getting %p [IsContainer]", this);
		inItemData.SetBoolean(IsContainer());
		break;
		
	case kDataBrowserContainerIsOpenableProperty:
		//Trace("Getting %p [IsOpenable]", this);
		inItemData.SetBoolean(IsContainerOpenable());
		break;
		
	case kDataBrowserContainerIsClosableProperty:
		//Trace("Getting %p [IsClosable]", this);
		inItemData.SetBoolean(IsContainerClosable());
		break;
		
	case kDataBrowserContainerIsSortableProperty:
		//Trace("Getting %p [IsSortable]", this);
		inItemData.SetBoolean(IsContainerSortable());
		break;
		
	case kDataBrowserContainerAliasIDProperty:
		//Trace("Getting %p [ContainerAliasID]", this);
		inItemData.SetItemID(GetContainerAliasID());
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::CustomDraw(
	DataBrowserPropertyID	/* inProperty */, 
	DataBrowserItemState	/* inItemState */, 
	const Rect&				/* inRect */, 
	SInt16					/* inDepth */, 
	Boolean					/* inColorDevice */)
	const
{
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::CustomEdit(
	DataBrowserPropertyID	/* inProperty */, 
	const String&			/* inString */, 
	Rect&					/* outMaxEditTextRect */, 
	bool&					/* outShrinkToFit */)
	const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::CustomHitTest(
	DataBrowserPropertyID	/* inProperty */, 
	const Rect&				/* inRect */, 
	const Rect&				/* inMouseRect */)
	const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
DataBrowserTrackingResult
DataBrowserItem::CustomTrack(
	DataBrowserPropertyID	/* inProperty */, 
	const Rect&				/* inRect */, 
	const Point&			/* inStartPt */, 
	EventModifiers			/* inModifiers */)
	const
{
	return (kDataBrowserContentHit);
}

// ------------------------------------------------------------------------------------------
void
DataBrowserItem::CustomDragRgn(
	DataBrowserPropertyID	/* inProperty */, 
	const Rect&				/* inRect */, 
	RgnHandle				/* ioDragRgn */)
	const
{
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::CustomAcceptDrag(
	DataBrowserPropertyID	/* inProperty */, 
	const Rect&				/* inRect */, 
	DragRef					/* inDragRef */)
	const
{
	return (false);
}

// ------------------------------------------------------------------------------------------
bool
DataBrowserItem::CustomReceiveDrag(
	DataBrowserPropertyID	/* inProperty */, 
	DataBrowserDragFlags	/* inDragFlags*/, 
	DragRef					/* inDragRef */)
	const
{
	return (false);
}

}	// namespace B
