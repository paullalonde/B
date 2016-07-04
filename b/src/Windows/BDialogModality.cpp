// ==========================================================================================
//	
//	Copyright (C) 2004-2006 Paul Lalonde enrg.
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
#include "BDialogModality.h"

// project headers
#include "BErrorHandler.h"


namespace B {

// ------------------------------------------------------------------------------------------
DialogModality
DialogModality::Modeless()
{
	return (DialogModality(kWindowModalityNone, NULL, true));
}

// ------------------------------------------------------------------------------------------
DialogModality
DialogModality::Modal()
{
	return (DialogModality(kWindowModalityAppModal, NULL, true));
}

// ------------------------------------------------------------------------------------------
DialogModality
DialogModality::Sheet(
	WindowRef			inParent)	//!< The sheet's parent.  Must be non-NULL.
{
	B_ASSERT(inParent != NULL);
	
	return (DialogModality(kWindowModalityWindowModal, inParent, true));
}

// ------------------------------------------------------------------------------------------
DialogModality::DialogModality(
	WindowModality		inModality,	//!< The modality.  One of @c kWindowModalityNone, @c kWindowModalityAppModal, or @c kWindowModalityWindowModal.
	WindowRef			inParent)	//!< The sheet's parent.  Must be non-NULL if @a inModality is @c kWindowModalityWindowModal.
	: mModality(inModality), mParent(inParent)
{
	B_ASSERT((mModality == kWindowModalityNone)     || 
		   (mModality == kWindowModalityAppModal) || 
		   (mModality == kWindowModalityWindowModal));
	
	// If we have a sheet, the parent window must be non-NULL.
	B_ASSERT((mModality != kWindowModalityWindowModal) || (mParent != NULL));
	
	// If we *don't* have a sheet, force the parent window to NULL.  This is because some 
	// system APIs (eg Nav Services) just look at the parent field to figure out if they 
	// should use a sheet.
	
	if (mModality != kWindowModalityWindowModal)
		mParent = NULL;
}

}	// namespace B
