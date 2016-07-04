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
#include "PListerDoc.h"

// standard headers
#include <string>

// library headers
#include "BOSPtr.h"
#include "BWindow.h"

// project headers
#include "ModelItem.h"
#include "PListerWin.h"

// for template instantiation
#include "BDocument.tpl.h"
#include "BDocumentWindow.tpl.h"


// ------------------------------------------------------------------------------------------
PListerDoc::PListerDoc(
	B::AEObjectPtr	inContainer, 
	DescType		inDocClass, 
	SInt32			inUniqueID)
		: inherited(inContainer, inDocClass, inUniqueID)
{
}

// ------------------------------------------------------------------------------------------
void
PListerDoc::MakeWindows(
	B::Nib*			inNib)
{
	boost::shared_ptr<PListerWin>	windowPtr;
	
	windowPtr = B::Window::CreateFromNibWithState<PListerWin, PListerDoc*>(
					*inNib, GetWindowNibName(), this);
	
	windowPtr->AddToWindowList();
	
	mWindow = windowPtr.get();
}

// ------------------------------------------------------------------------------------------
void
PListerDoc::InitDocument(
	const B::String&	inName,
	const B::String&	inDocumentType, 
	const AEDesc&		inProperties, 
	const AEDesc&		inData)
{
	inherited::InitDocument(inName, inDocumentType, inProperties, inData);
	
	B::OSPtr<CFTypeRef>	propertyList(CFDictionaryCreate(NULL, NULL, NULL, 0, 
												&kCFCopyStringDictionaryKeyCallBacks, 
												&kCFTypeDictionaryValueCallBacks), 
									 B::from_copy);
	
	mRootItem = ModelItem::Create(GetAEObjectPtr(), propertyList);
}

// ------------------------------------------------------------------------------------------
void
PListerDoc::ReadDocumentFromBuffer(
	const std::vector<UInt8>&	inBuffer, 
	B::String&					/* ioDocumentType */)
{
	B::OSPtr<CFDataRef>	data(CFDataCreateWithBytesNoCopy(NULL, 
														 &inBuffer[0], inBuffer.size(), 
														 kCFAllocatorNull), 
							 B::from_copy);
	CFTypeRef			typeRef;
	CFStringRef			errorRef	= NULL;
	
	typeRef = CFPropertyListCreateFromXMLData(NULL, data, 
											  kCFPropertyListImmutable, 
											  &errorRef);
	
	if (typeRef == NULL)
	{
		B::OSPtr<CFStringRef>	errorStr(errorRef, B::from_copy);
		
		B_THROW(B::PropertyListCreateException(errorStr));
	}
	
	mRootItem = ModelItem::Create(GetAEObjectPtr(), 
								  B::OSPtr<CFTypeRef>(typeRef, B::from_copy));
}

// ------------------------------------------------------------------------------------------
void
PListerDoc::WriteDocumentToBuffer(
	std::vector<UInt8>&			outBuffer, 
	B::SaveInfo&				/* ioSaveInfo */)
{
	B::OSPtr<CFTypeRef>	propertyList	= mRootItem->MakeValueAsPropertyList();
	B::OSPtr<CFDataRef>	data(CFPropertyListCreateXMLData(NULL, propertyList), 
							 B::from_copy);
	
	outBuffer.resize(CFDataGetLength(data));
	BlockMoveData(CFDataGetBytePtr(data), &outBuffer[0], outBuffer.size());
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
PListerDoc::GetPropertyObject(
	DescType	inPropertyID) const
{
	B::AEObjectPtr	property;
	
	switch (inPropertyID)
	{
	case kRootItemProperty:
		property = mRootItem;
		break;

	default:
		property = inherited::GetPropertyObject(inPropertyID);
		break;
	}
	
	return (property);
}
