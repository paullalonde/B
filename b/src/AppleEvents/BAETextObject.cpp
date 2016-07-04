// ==========================================================================================
//	
//	Copyright (C) 2006 Paul Lalonde enrg.
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
#include "BAETextObject.h"


namespace B {

// ------------------------------------------------------------------------------------------
AETextObject::AETextObject(
	AEObjectPtr	inContainer,	//!< The object's container (as seen in AppleScript).
	DescType	inClassID,		//!< The object's class ID;  must match the application's AppleScript dictionary.
	ITextBackingStore*	inBackingStore)
		: inherited(inContainer, inClassID), 
		  mBackingStore(inBackingStore)
{
	// There's nothing else to do.
}

// ------------------------------------------------------------------------------------------
size_t
AETextObject::CountElements(
	DescType		inElementType)	//!< The base class ID of the elements to count;  must match the application's AppleScript dictionary.
	const
{
	size_t	count	= 0;
	
	switch (inElementType)
	{
	case cChar:
		count = mBackingStore->CountCharacters();
		break;
		
	case 'catr':
		count = mBackingStore->CountAttributeRuns();
		break;
		
	default:
		count = inherited::CountElements(inElementType);
		break;
	}
	
	return count;
}

// ------------------------------------------------------------------------------------------
AEObjectPtr
AETextObject::GetElementByIndex(
	DescType	inElementType,	//!< The base class ID of the element;  must match the application's AppleScript dictionary.
	size_t		inIndex)		//!< The element's zero-based index.
	const
{
	AEObjectPtr	obj;
	
	switch (inElementType)
	{
	case cChar:
		obj = mBackingStore->GetCharacterByIndex(inIndex);
		break;
		
#if 0
	case 'catr':
		obj = mBackingStore->GetParagraphByIndex(inIndex);
		break;
#endif
		
	default:
		obj = inherited::GetElementByIndex(inElementType, inIndex);
		break;
	}
	
	return obj;
}


}	// namespace B
