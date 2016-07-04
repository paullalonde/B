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

#ifndef BDataBrowser_H_
#define BDataBrowser_H_

#pragma once

// library headers
#include <boost/signal.hpp>

// B headers
#include "BView.h"
#include "BViewData.h"


namespace B {


// forward declarations
class	CommandData;
class	DataBrowserItem;
class	DataBrowserItemData;
class	Drag;
class	String;


/*!	@brief	Data Browser control.
	
	@todo		%Document this class!
	@ingroup	ViewGroup
*/
class DataBrowser : public PredefinedView
{
public:
	
	// types
	typedef std::vector<DataBrowserItem*>	ItemVector;
	typedef boost::signal0<void>			Signal;
	
	//! @name Instantiation
	//@{
	//! Create an object given an existing HIToolbox view.
	static View*	Instantiate(HIViewRef inViewRef);
	//! Create an object and its HIToolbox view, from the given parameters.
	static DataBrowser*
					Create(
						const HIViewID&			inViewID,
						HIViewRef				inSuperview,
						const Rect*				inFrame, 
						Nib*					inFromNib, 
						const ViewFactory*		inFactory,
						DataBrowserViewStyle	inViewStyle);
	//@}
	
	//! @name Constructor
	//@{
	//! Constructor.
					DataBrowser(HIViewRef inViewRef);
	//@}
	
	// inquiries
	DataBrowserSelectionFlags
					GetSelectionFlags() const;
	OSPtr<CFDictionaryRef>
					GetUserState() const;
	unsigned		GetHeaderHeight() const;
	unsigned		GetRowHeight() const;
	unsigned		GetColumnWidth() const;
	
	// modifiers
	void			SetDisclosureColumn(DataBrowserTableViewColumnID inColumnID);
	void			SetSortColumn(DataBrowserTableViewColumnID inColumnID);
	void			SetUserState(OSPtr<CFDictionaryRef> inUserState);
	void			SetRootContainer(DataBrowserItem* inContainer);
	void			UseDragAndDrop();
	
	// column management
	unsigned		CountColumns() const;
	DataBrowserTableViewColumnID
					GetColumnID(unsigned index) const;
	unsigned		GetColumnPosition(
						DataBrowserTableViewColumnID	inColumnID) const;
	UInt16			GetColumnWidth(
						DataBrowserTableViewColumnID	inColumnID) const;
	void			SetColumnWidth(
						DataBrowserTableViewColumnID	inColumnID, 
						UInt16							inWidth);
	void			SetColumnEditable(
						DataBrowserTableViewColumnID	inColumnID, 
						bool							editable);
	virtual void	AddColumn(
						DataBrowserListViewColumnDesc&	inColumnDesc,
						DataBrowserTableViewColumnIndex	inPosition = kDataBrowserListViewAppendColumn, 
						SInt16							inInitialWidth = 0);
	
	// item management
	void			AddItem(
						DataBrowserItem*		inItem,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	void			AddItem(
						DataBrowserItem*		inParentItem, 
						DataBrowserItem*		inItem,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	virtual void	AddItems(
						DataBrowserItem*		inParentItem, 
						const ItemVector&		inItems,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	void			RemoveItem(
						DataBrowserItem*		inItem,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	virtual void	RemoveItems(
						const ItemVector&		inItems,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	void			RemoveAllItems(
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	virtual void	RemoveChildren(
						DataBrowserItem*		inParentItem, 
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	void			UpdateItem(
						DataBrowserItem*		inItem,
						DataBrowserPropertyID	inProperty = kDataBrowserNoItem,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	virtual void	UpdateItems(
						DataBrowserItem*		inParentItem, 
						const ItemVector&		inItems,
						DataBrowserPropertyID	inProperty = kDataBrowserNoItem,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	virtual void	UpdateChildren(
						DataBrowserItem*		inParentItem, 
						DataBrowserPropertyID	inProperty = kDataBrowserNoItem,
						DataBrowserPropertyID	inPresorted = kDataBrowserItemNoProperty);
	virtual UInt32	GetItems(
						DataBrowserItem*		inParentItem, 
						bool					inRecurse,
						DataBrowserItemState	inState,
						ItemVector&				outItems) const;
	virtual UInt32	CountItems(
						DataBrowserItem*		inParentItem, 
						bool					inRecurse,
						DataBrowserItemState	inState) const;
	virtual DataBrowserItemState
					GetItemState(
						const DataBrowserItem* inItem) const;
	virtual ::Rect	GetItemBounds(
						const DataBrowserItem*	inItem, 
						DataBrowserPropertyID	inProperty, 
						DataBrowserPropertyPart	inPropertyPart) const;
#if 0	// DELETE
	virtual void	GetItemBounds(
						const DataBrowserItem*	inItem, 
						DataBrowserPropertyID	inProperty, 
						DataBrowserPropertyPart	inPropertyPart, 
						::Rect&					outBounds) const;
#endif
	virtual UInt16	GetItemHeight(
						const DataBrowserItem*	inItem) const;
	virtual void	SetItemHeight(
						const DataBrowserItem*	inItem, 
						UInt16					inHeight);
	
	// iteration
	void			ForEachItemDo(
						DataBrowserItem*		inParentItem, 
						bool					inRecurse,
						DataBrowserItemState	inState,
						DataBrowserItemProcPtr	inProc, 
						void*					inArg);
	
	// selection
	UInt32			CountSelectedItems() const;
	DataBrowserItem* GetFirstSelectedItem() const;
	UInt32			GetSelectedItems(
						ItemVector&				outItems) const;
	void			SelectItem(
						DataBrowserItem*		inItem);
	virtual void	SelectItems(
						const ItemVector&		inItems, 
						DataBrowserSetOption	inOperation = kDataBrowserItemsAssign);
	void			SelectAllItems();
	void			UnselectItem(
						DataBrowserItem*		inItem);
	void			UnselectItems(
						const ItemVector&		inItems);
	void			UnselectAllItems();
	virtual void	RevealItem(
						DataBrowserItem*		inItem, 
						DataBrowserPropertyID	inProperty,
						bool					inCenterInView = false);
					
	// containers
	virtual void	OpenContainer(
						DataBrowserItem*		inContainerItem);
	virtual void	CloseContainer(
						DataBrowserItem*		inContainerItem);
    
    // edit session
    virtual void	OpenEditSession(
    					DataBrowserItem*		inItem, 
    					DataBrowserPropertyID	inProperty);
    virtual bool	IsEditSessionOpen() const;
    virtual void	CancelEdit() const;
	
	//! @name Signals
	//@{
	//! This signal is sent when the data browser's selection changes.
	Signal&			GetSelectionChangedSignal()	{ return (mSelectionChangedSignal); }
	//! This signal is sent when the data browser's user state (column position & widths) changes.
	Signal&			GetUserStateChangedSignal()	{ return (mUserStateChangedSignal); }
	//@}
	
protected:
	
	//! Destructor.
	virtual				~DataBrowser();
	
	void				Cleanup();
	
	// notifications
	virtual void		SelectionChanged();
	virtual void		UserStateChanged();
	virtual void		DoubleClickedItem(
							DataBrowserItem*		inItem);
	
	// properties
	virtual bool		GetProperty(
							DataBrowserItemData&	inItemData,
							DataBrowserPropertyID	inProperty) const;
	virtual bool		IsRootContainerSortable() const;
	
	// callback hooks
	virtual	void		GetSetItemData(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty,
							DataBrowserItemDataRef		inItemData,
							bool						inChangeValue);
	virtual	bool		Compare(
							DataBrowserItem*			inItem1,
							DataBrowserItem*			inItem2,
							DataBrowserPropertyID		inSortProperty);
	virtual	void		ItemNotification(
							DataBrowserItem*			inItem,
							DataBrowserItemNotification	inMessage);
	virtual	bool		AddDragItem(
							Drag&						ioDrag,
							DataBrowserItem*			inItem,
							PasteboardItemID&			outItemID);
	virtual	bool		IsDragOnBrowserAcceptable(
							const Drag&					inDrag);
	virtual	bool		IsDragOnItemAcceptable(
							const Drag&					inDrag,
							const DataBrowserItem*		inItem);
	virtual	bool		ReceiveDragOnBrowser(
							const Drag&					inDrag);
	virtual	bool		ReceiveDragOnItem(
							const Drag&					inDrag,
							DataBrowserItem*			ioItem);
	virtual	void		PostProcessDrag(
							const Drag&					inDrag,
							OSStatus					inTrackDragResult);
	virtual	void		GetContextualMenu(
							MenuRef&					outMenuRef,
							UInt32&						outHelpType,
							CFStringRef&				outHelpItemString,
							AEDesc&						outSelection);
	virtual	void		SelectContextualMenu(
							MenuRef						inMenuRef,
							UInt32						inSelectionType,
							SInt16						inMenuID,
							MenuItemIndex				inMenuItem);
	virtual bool		KeyFilter(
							UInt16&						ioKeyCode, 
							UInt16&						ioCharCode, 
							EventModifiers&				ioModifiers, 
							bool&						outPassUp);
	virtual void		CustomItemDraw(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							DataBrowserItemState		inItemState, 
							const Rect&					inRect, 
							SInt16						inDepth, 
							Boolean						inColorDevice);
	virtual bool		CustomItemEdit(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							const String&				inString, 
							Rect&						outMaxEditTextRect, 
							bool&						outShrinkToFit);
	virtual bool		CustomItemHitTest(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							const Rect&					inRect, 
							const Rect&					inMouseRect);
	virtual DataBrowserTrackingResult
						CustomItemTrack(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							const Rect&					inRect, 
							const Point&				inStartPt, 
							EventModifiers				inModifiers);
	virtual void		CustomItemDragRgn(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							const Rect&					inRect, 
							RgnHandle					ioDragRgn);
	virtual bool		CustomItemAcceptDrag(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							const Rect&					inRect, 
							DragRef						inDragRef);
	virtual bool		CustomItemReceiveDrag(
							DataBrowserItem*			inItem,
							DataBrowserPropertyID		inProperty, 
							DataBrowserDragFlags		inDragFlags, 
							DragRef						inDragRef);
	
	// utility
	static DataBrowserItem*
						GetItem(
							DataBrowserItemID			inItemID);
	
	// Carbon %Event handlers
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);
	virtual bool	HandleUpdateStatus(
						const HICommandExtended&	inHICommand, 
						CommandData&				ioCmdData);
	
private:
	
	// types
	template <ResType TAG> struct Data : public ViewData<kControlKindSignatureApple, kControlKindDataBrowser, TAG> {};
	
	void	InitEventHandler();
							
	void	DisplayContextualMenu(
				MenuRef			inMenu,
				UInt32			inHelpType,
				CFStringRef		inHelpItemString,
				const AEDesc&	inSelection);
	
	// callbacks
	static pascal OSStatus	GetSetItemDataProc(
								HIViewRef					inBrowser, 
								DataBrowserItemID			inItemID, 
								DataBrowserPropertyID		inProperty, 
								DataBrowserItemDataRef		inItemData,
								Boolean						inChangeValue);
	static pascal Boolean	CompareProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID1, 
								DataBrowserItemID			inItemID2,
								DataBrowserPropertyID		inSortProperty);
	static pascal void		ItemNotificationProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserItemNotification	inMessage);
	static pascal Boolean	AddDragItemProc(
								HIViewRef					inBrowser,
								DragRef						inDragRef,
								DataBrowserItemID			inItemID,
								DragItemRef*				outItemRef);
	static pascal Boolean	AcceptDragProc(
								HIViewRef					inBrowser,
								DragRef						inDragRef,
								DataBrowserItemID			inItemID);
	static pascal Boolean	ReceiveDragProc(
								HIViewRef					inBrowser,
								DragRef						inDragRef,
								DataBrowserItemID			inItemID);
	static pascal void		PostProcessDragProc(
								HIViewRef					inBrowser,
								DragRef						inDragRef,
								OSStatus					inTrackDragResult);
	static pascal void		GetContextualMenuProc(
								HIViewRef					inBrowser,
								MenuRef*					outMenu,
								UInt32*						outHelpType,
								CFStringRef*				outHelpItemString,
								AEDesc*						outSelection);
	static pascal void		SelectContextualMenuProc(
								HIViewRef					inBrowser,
								MenuRef						inMenu,
								UInt32						inSelectionType,
								SInt16						inMenuID,
								MenuItemIndex				inMenuItem);
	static pascal void		GetHelpContentProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty,
								HMContentRequest			inRequest,
								HMContentProvidedType*		outContentProvided,
								HMHelpContentPtr			ioHelpContent);
	static pascal void		ItemProc(
								DataBrowserItemID			inItemID, 
								DataBrowserItemState		inState,
								void*						inClientData);
//	static pascal ControlKeyFilterResult
//							KeyFilterProc(
//								HIViewRef					inBrowser,
//								SInt16*						ioKeyCode, 
//								SInt16*						ioCharCode, 
//								EventModifiers*				ioModifiers);
	static pascal void		DrawItemProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								DataBrowserItemState		inItemState, 
								const ::Rect*				inRect, 
								SInt16						inDepth, 
								Boolean						inColorDevice);
	static pascal Boolean	EditItemProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								CFStringRef					inString, 
								::Rect*						outMaxEditTextRect, 
								Boolean*					outShrinkToFit);
	static pascal Boolean	HitTestProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								const ::Rect*				inRect, 
								const ::Rect*				inMouseRect);
	static pascal DataBrowserTrackingResult
							TrackingProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								const ::Rect*				inRect, 
								::Point						inStartPt, 
								EventModifiers				inModifiers);
	static pascal void		ItemDragRgnProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								const ::Rect*				inRect, 
								RgnHandle					ioDragRgn);
	static pascal DataBrowserDragFlags
							ItemAcceptDragProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								const ::Rect*				inRect, 
								DragRef						inDragRef);
	static pascal Boolean	ItemReceiveDragProc(
								HIViewRef					inBrowser,
								DataBrowserItemID			inItemID,
								DataBrowserPropertyID		inProperty, 
								DataBrowserDragFlags		inDragFlags, 
								DragRef						inDragRef);
	
	// Carbon %Event handlers
	bool	CommandProcess(
				Event<kEventClassCommand, kEventCommandProcess>&			event);
	bool	CommandUpdateStatus(
				Event<kEventClassCommand, kEventCommandUpdateStatus>&		event);
	bool	ControlGetOptimalBounds(
				Event<kEventClassControl, kEventControlGetOptimalBounds>&	event);
	
	// member variables
	EventHandler		mEventHandler;
	Boolean				mTopLevelIsSortable;
	DataBrowserItem*	mEditItem;
	Signal				mSelectionChangedSignal;
	Signal				mUserStateChangedSignal;
};


// ------------------------------------------------------------------------------------------
inline DataBrowserItem*
DataBrowser::GetItem(
	DataBrowserItemID	inItemID)
{
	return (reinterpret_cast<DataBrowserItem*>(inItemID));
}


}	// namespace B


#endif	// BDataBrowser_H_
