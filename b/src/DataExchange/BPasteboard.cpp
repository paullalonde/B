// ==========================================================================================
//	
//	Copyright (C) 2005-2006 Paul Lalonde enrg.
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
#include "BPasteboard.h"

// B headers
#include "BString.h"
#include "BUrl.h"
#include "CFUtils.h"


namespace B {

// ==========================================================================================
//  Pasteboard

// ------------------------------------------------------------------------------------------
Pasteboard::Pasteboard()
	: mSyncFlags(0), mChangeCount(0)
{
	PasteboardRef	pbref;
	OSStatus		err;
	
	err = PasteboardCreate(kPasteboardUniqueName, &pbref);
	B_THROW_IF_STATUS(err);
	
	mPasteboard.reset(pbref, from_copy);
	Synchronize();
}

// ------------------------------------------------------------------------------------------
Pasteboard::Pasteboard(CFStringRef inName)
	: mSyncFlags(0), mChangeCount(0)
{
	PasteboardRef	pbref;
	OSStatus		err;
	
	err = PasteboardCreate(inName, &pbref);
	B_THROW_IF_STATUS(err);
	
	mPasteboard.reset(pbref, from_copy);
	Synchronize();
}

// ------------------------------------------------------------------------------------------
Pasteboard::Pasteboard(PasteboardRef inPasteboardRef)
	: mPasteboard(inPasteboardRef), mChangeCount(0)
{
	Synchronize();
}

// ------------------------------------------------------------------------------------------
String
Pasteboard::GetName() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = PasteboardCopyName(mPasteboard, &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
Url
Pasteboard::GetPasteLocation() const
{
	CFURLRef	cfurl;
	OSStatus	err;
	
	err = PasteboardCopyPasteLocation(mPasteboard, &cfurl);
	B_THROW_IF_STATUS(err);
	
	return (Url(OSPtr<CFURLRef>(cfurl, from_copy)));
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::SetPasteLocation(const Url& inUrl)
{
	OSStatus	err;
	
	err = PasteboardSetPasteLocation(mPasteboard, inUrl.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::Synchronize()
{
	mSyncFlags = PasteboardSynchronize(mPasteboard);
	
	if (mSyncFlags & kPasteboardModified)
		mChangeCount++;
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::Clear()
{
	OSStatus	err;
	
	err = PasteboardClear(mPasteboard);
	B_THROW_IF_STATUS(err);
	
	mSyncFlags = (kPasteboardModified| kPasteboardClientIsOwner);
	mChangeCount++;
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::Clear(PromiseKeeper inKeeper)
{
	Clear();
	SetPromiseKeeper(inKeeper);
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::SetPromiseKeeper(PromiseKeeper inKeeper)
{
	OSStatus	err;
	
	err = PasteboardSetPromiseKeeper(mPasteboard, PromiseKeeperProc, this);
	B_THROW_IF_STATUS(err);
	
	mKeeper = inKeeper;
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::ResolvePromises()
{
	OSStatus	err;
	
	err = PasteboardResolvePromises(mPasteboard);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::ResolveAllPromises()
{
	OSStatus	err;
	
	err = PasteboardResolvePromises(kPasteboardResolveAllPromises);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
size_t
Pasteboard::size() const
{
	ItemCount	count;
	OSStatus	err;
	
	err = PasteboardGetItemCount(mPasteboard, &count);
	B_THROW_IF_STATUS(err);
	
	return (count);
}

// ------------------------------------------------------------------------------------------
PasteboardItem
Pasteboard::operator [] (size_t index) const
{
	PasteboardItemID	itemID;
	OSStatus			err;
	
	err = PasteboardGetItemIdentifier(mPasteboard, index+1, &itemID);
	B_THROW_IF_STATUS(err);
	
	return (PasteboardItem(mPasteboard, itemID));
}

// ------------------------------------------------------------------------------------------
PasteboardItem
Pasteboard::NewItem(PasteboardItemID inItemID)
{
	return PasteboardItem(mPasteboard, inItemID);
}

// ------------------------------------------------------------------------------------------
Pasteboard::const_iterator
Pasteboard::begin() const
{
	return (const_iterator(this, 0));
}

// ------------------------------------------------------------------------------------------
Pasteboard::const_iterator
Pasteboard::end() const
{
	return (const_iterator(this, size()));
}

// ------------------------------------------------------------------------------------------
Pasteboard::const_reverse_iterator
Pasteboard::rbegin() const
{
	return (const_reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
Pasteboard::const_reverse_iterator
Pasteboard::rend() const
{
	return (const_reverse_iterator(end()));
}

// ------------------------------------------------------------------------------------------
void
Pasteboard::InvokePromiseKeeper(
	PasteboardRef		pasteboard, 
	PasteboardItemID	itemID, 
	CFStringRef			flavorType)
{
	PasteboardItem	item(OSPtr<PasteboardRef>(pasteboard), itemID);
	
	mKeeper(item, flavorType);
}

// ------------------------------------------------------------------------------------------
OSStatus
Pasteboard::PromiseKeeperProc(
	PasteboardRef		pasteboard, 
	PasteboardItemID	itemID, 
	CFStringRef			flavorType, 
	void*				context)
{
	OSStatus	err	= noErr;
	
	try
	{
		Pasteboard*	pb	= reinterpret_cast<Pasteboard*>(context);
		
		pb->InvokePromiseKeeper(pasteboard, itemID, flavorType);
	}
	catch (...)
	{
	}
	
	return (err);
}


// ==========================================================================================
//  PasteboardItem

// ------------------------------------------------------------------------------------------
PasteboardItem::PasteboardItem(
	const OSPtr<PasteboardRef>&	inPasteboard, 
	PasteboardItemID			inItemID)
		: mPasteboard(inPasteboard), mItemID(inItemID)
{
}

// ------------------------------------------------------------------------------------------
PasteboardItem::PasteboardItem(const PasteboardItem& src)
	: mPasteboard(src.mPasteboard), mItemID(src.mItemID), 
	  mFlavorTypes(src.mFlavorTypes)
{
}

// ------------------------------------------------------------------------------------------
PasteboardItem&
PasteboardItem::operator = (const PasteboardItem& src)
{
	mPasteboard		= src.mPasteboard;
	mItemID			= src.mItemID;
	mFlavorTypes	= src.mFlavorTypes;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
size_t
PasteboardItem::size() const
{
	CFArrayRef	flavorTypes	= GetFlavorTypes();
	
	return (CFArrayGetCount(flavorTypes));
}

// ------------------------------------------------------------------------------------------
PasteboardFlavor
PasteboardItem::operator [] (size_t inIndex) const
{
	CFArrayRef	flavorTypes	= GetFlavorTypes();
	
	return (PasteboardFlavor(mPasteboard, mItemID, 
							 (CFStringRef) CFArrayGetValueAtIndex(flavorTypes, inIndex)));
}

// ------------------------------------------------------------------------------------------
PasteboardFlavor
PasteboardItem::NewFlavor(CFStringRef inFlavorType) const
{
	return (PasteboardFlavor(mPasteboard, mItemID, inFlavorType));
}

// ------------------------------------------------------------------------------------------
bool
PasteboardItem::Contains(CFStringRef inFlavorType) const
{
	PasteboardFlavorFlags	flags;
	OSStatus				err;
	
	err = PasteboardGetItemFlavorFlags(mPasteboard, mItemID, inFlavorType, &flags);
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
CFArrayRef
PasteboardItem::GetFlavorTypes() const
{
	if (mFlavorTypes.get() == NULL)
	{
		CFArrayRef	cfarr;
		OSStatus	err;
		
		err = PasteboardCopyItemFlavors(mPasteboard, mItemID, &cfarr);
		B_THROW_IF_STATUS(err);
		
		mFlavorTypes.reset(cfarr, from_copy);
	}
	
	return (mFlavorTypes);
}

// ------------------------------------------------------------------------------------------
PasteboardItem::const_iterator
PasteboardItem::begin() const
{
	return (const_iterator(this, 0));
}

// ------------------------------------------------------------------------------------------
PasteboardItem::const_iterator
PasteboardItem::end() const
{
	return (const_iterator(this, size()));
}

// ------------------------------------------------------------------------------------------
PasteboardItem::const_reverse_iterator
PasteboardItem::rbegin() const
{
	return (const_reverse_iterator(begin()));
}

// ------------------------------------------------------------------------------------------
PasteboardItem::const_reverse_iterator
PasteboardItem::rend() const
{
	return (const_reverse_iterator(end()));
}


// ==========================================================================================
//  PasteboardFlavor

// ------------------------------------------------------------------------------------------
PasteboardFlavor::PasteboardFlavor()
	: mItemID(NULL), mFlavorType(NULL), mFlags(kPasteboardFlavorNoFlags), mGotFlags(false)
{
}

// ------------------------------------------------------------------------------------------
PasteboardFlavor::PasteboardFlavor(
	const OSPtr<PasteboardRef>&	inPasteboard, 
	PasteboardItemID			inItemID,
	CFStringRef					inFlavor)
		: mPasteboard(inPasteboard), mItemID(inItemID), mFlavorType(inFlavor), 
		  mFlags(kPasteboardFlavorNoFlags), mGotFlags(false)
{
}

// ------------------------------------------------------------------------------------------
PasteboardFlavor::PasteboardFlavor(const PasteboardFlavor& src)
	: mPasteboard(src.mPasteboard), mItemID(src.mItemID), mFlavorType(src.mFlavorType), 
	  mFlags(src.mFlags), mGotFlags(src.mGotFlags)
{
}

// ------------------------------------------------------------------------------------------
PasteboardFlavor&
PasteboardFlavor::operator = (const PasteboardFlavor& src)
{
	mPasteboard	= src.mPasteboard;
	mItemID		= src.mItemID;
	mFlavorType	= src.mFlavorType;
	mFlags		= src.mFlags;
	mGotFlags	= src.mGotFlags;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
PasteboardFlavorFlags
PasteboardFlavor::GetFlags() const
{
	if (!mGotFlags)
	{
		OSStatus	err;
		
		err = PasteboardGetItemFlavorFlags(mPasteboard, mItemID, mFlavorType, &mFlags);
		B_THROW_IF_STATUS(err);
		
		mGotFlags = true;
	}
	
	return (mFlags);
}

// ------------------------------------------------------------------------------------------
void
PasteboardFlavor::SetFlags(PasteboardFlavorFlags inFlags)
{
	mFlags = inFlags;
	mGotFlags = true;
}

// ------------------------------------------------------------------------------------------
void
PasteboardFlavor::SetFlagBit(PasteboardFlavorFlags inBit, bool inSet)
{
	if (inSet)
		mFlags |= inBit;
	else
		mFlags &= ~inBit;
	
	mGotFlags = true;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDataRef>
PasteboardFlavor::GetData() const
{
	CFDataRef	cfdata;
	OSStatus	err;
	
	err = PasteboardCopyItemFlavorData(mPasteboard, mItemID, mFlavorType, &cfdata);
	B_THROW_IF_STATUS(err);
	
	return (OSPtr<CFDataRef>(cfdata, from_copy));
}

// ------------------------------------------------------------------------------------------
bool
PasteboardFlavor::GetData(OSPtr<CFDataRef>& outData) const
{
	CFDataRef	cfdata;
	bool		good;
	OSStatus	err;
	
	err		= PasteboardCopyItemFlavorData(mPasteboard, mItemID, mFlavorType, &cfdata);
	good	= (err == noErr);
	
	if (good)
	{
		outData.reset(cfdata, from_copy);
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
void
PasteboardFlavor::SetData(const OSPtr<CFDataRef>& inData)
{
	OSStatus	err;
	
	err = PasteboardPutItemFlavor(mPasteboard, mItemID, mFlavorType, inData.get(), mFlags);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
PasteboardFlavor::SetData(const OSPtr<CFDataRef>& inData, const std::nothrow_t&)
{
	OSStatus	err;
	
	err = PasteboardPutItemFlavor(mPasteboard, mItemID, mFlavorType, inData.get(), mFlags);
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
void
PasteboardFlavor::SetData(const OSPtr<CFMutableDataRef>& inData)
{
	OSStatus	err;
	
	err = PasteboardPutItemFlavor(mPasteboard, mItemID, mFlavorType, inData.get(), mFlags);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
PasteboardFlavor::SetData(const OSPtr<CFMutableDataRef>& inData, const std::nothrow_t&)
{
	OSStatus	err;
	
	err = PasteboardPutItemFlavor(mPasteboard, mItemID, mFlavorType, inData.get(), mFlags);
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
void
PasteboardFlavor::SetData()
{
	OSStatus	err;
	
	err = PasteboardPutItemFlavor(mPasteboard, mItemID, mFlavorType, 
								  kPasteboardPromisedData, mFlags);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
PasteboardFlavor::SetData(const std::nothrow_t&)
{
	OSStatus	err;
	
	err = PasteboardPutItemFlavor(mPasteboard, mItemID, mFlavorType, 
								  kPasteboardPromisedData, mFlags);
	
	return (err == noErr);
}

}	// namespace B
