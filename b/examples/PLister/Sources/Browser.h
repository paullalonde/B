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

#ifndef Browser_H_
#define Browser_H_

#pragma once

// B headers
#include "BDataBrowser.h"
#include "BMutableArray.h"

// project headers
#include "ModelItem.h"

// forward declarations
class	BrowserItem;
namespace B { class Pasteboard; }


class Browser : public B::DataBrowser
{
public:
	
	// constants
	static const UInt32	kNewSiblingCmdID	= 'DwNs';
	static const UInt32	kNewChildCmdID		= 'DwNc';
	static const UInt32	kDuplicateCmdID		= 'Dupl';
	static const UInt32	kMoveUpCmdID		= 'MvUp';
	static const UInt32	kMoveDownCmdID		= 'MvDn';
	static const UInt32	kEditValueCmdID		= 'ChVa';
	
	// instantiation
	static B::View*	Instantiate(HIViewRef inViewRef);
	
	// constructor / destructor
	explicit	Browser(HIViewRef inViewRef);
	
	// inquiries
	bool	IsSelectionDeletable() const		{ return (mSelectionIsDeletable); }
	bool	IsSelectionMovable() const			{ return (mSelectionIsMoveable); }
	bool	IsSelectionClonable() const			{ return (mSelectionIsMoveable); }
	bool	CanCutSelection() const				{ return (mSelectionIsSingleItem && mSelectionIsDeletable); }
	bool	CanCopySelection() const			{ return (mSelection); }
	bool	CanAddChildToSelection() const		{ return (mSelectionIsSingleContainer); }
	bool	CanAddSiblingToSelection() const	{ return (mSelectionIsSingleNonRootItem); }
	bool	CanEditSelectionValue() const		{ return (mSelectionIsSingleItem && !mSelectionIsSingleContainer); }
	EventTargetRef	GetUndoTarget()				{ return (mUndoTarget); }
	
	void	MakeRootItem(ModelItem* inRootModel);
	void	SetUndoTarget(EventTargetRef inUndoTarget)	{ mUndoTarget = inUndoTarget; }
	
	// UI level
	void	NewSibling();
	void	NewChild();
	void	DeleteSelection();
	void	MoveSelection(ModelItemPtr inNewParent);
	void	CloneSelection(ModelItem* inNewParent = NULL);
	void	CutSelection();
	void	CopySelection();
	void	EditSelectionValue();
	
protected:
	
	virtual void	Awaken(B::Nib* inFromNib);

	// notifications
	virtual void		SelectionChanged();
	
	// callback hooks
	virtual	bool		IsDragOnBrowserAcceptable(
							const B::Drag&	inDrag);
	virtual	bool		ReceiveDragOnBrowser(
							const B::Drag&	inDrag);
	virtual	void		PostProcessDrag(
							const B::Drag&	inDrag,
							OSStatus		inTrackDragResult);
	virtual	void		GetContextualMenu(
							MenuRef&		outMenuRef,
							UInt32&			outHelpType,
							CFStringRef&	outHelpItemString,
							AEDesc&			outSelection);
	
	// Carbon %Event handlers
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);
	virtual bool	HandleUpdateStatus(
						const HICommandExtended&	inHICommand, 
						B::CommandData&				ioCmdData);
	virtual bool	HandleServiceCopy(
						B::Pasteboard&				ioPasteboard);
	virtual bool	HandleServiceGetTypes(
						B::MutableArray<CFStringRef>&	ioCopyTypes, 
						B::MutableArray<CFStringRef>&	ioPasteTypes);
	
private:
	
	void	InitEventHandler(B::EventHandler& ioHandler);
	
	UInt32	GetCanonicalSelection(ItemVector& outItems) const;
	UInt32	GetCanonicalSelection(std::vector<ModelItemPtr>& outModelItems) const;
	void	CopySelectionToPasteboard(B::Pasteboard& ioPasteboard);
	
	// Carbon %Event handlers
	bool	ServiceCopy(
				B::Event<kEventClassService, kEventServiceCopy>&		event);
	bool	ServiceGetTypes(
				B::Event<kEventClassService, kEventServiceGetTypes>&	event);
	
	// member variables
	B::EventHandler		mEventHandler;
	bool				mSelection;
	bool				mSelectionIsDeletable;
	bool				mSelectionIsMoveable;
	bool				mSelectionIsSingleContainer;
	bool				mSelectionIsSingleItem;
	bool				mSelectionIsSingleNonRootItem;
	B::OSPtr<MenuRef>	mClassMenu;
	B::OSPtr<MenuRef>	mContextualMenu;
	BrowserItem*		mRootItem;
	EventTargetRef		mUndoTarget;
	
	class ItemFlavorWriter
	{
	public:
				ItemFlavorWriter(B::Pasteboard& ioPasteboard);
		void	operator () (B::DataBrowserItem* item) const;
		
	private:
		B::Pasteboard&	mPasteboard;
	};
};


#endif	// Browser_H_
