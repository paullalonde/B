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

#ifndef BDialogModality_H_
#define BDialogModality_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>


namespace B {

/*!
	@brief  Helper object representing a dialog's modality
	
	This class encapsulates a @c WindowModality / @c WindowRef pair, 
	and may be used to specify a dialog's modality.  Dialog modality is what 
	distinguishes a modeless dialog from an application-modal dialog from a sheet.
	This class is used by Alert, Dialog, and NavDialog.
	
	You will typically construct a DialogModality object in one of two ways:
	
	-# If you know which modality you want, call one of the static instantiation 
	   functions.  For example, Modal() will return an object specifying 
	   application-modal modality.
	-# If you get a @c WindowModality and possibly a @c WindowRef handed to you by 
	   someone else, use DialogModality's constructor directly.
	
	@note	This class doesn't support @c kWindowModalitySystemModal.  Mac OS X 
			maps system-modal dialogs onto application-modal dialogs anyway, so it's 
			not a problem.
*/
class DialogModality
{
public:
	
	/*! @name Instantiation
		
		These are convenience routines that return object of the requested modality.
	*/
	//@{
	//! Instantiates an object of modality @c kWindowModalityNone (i.e., a modeless dialog).
	static DialogModality	Modeless();
	//! Instantiates an object of modality @c kWindowModalityAppModal (i.e., a modal dialog).
	static DialogModality	Modal();
	//! Instantiates an object of modality @c kWindowModalityWindowModal (i.e., a sheet).
	static DialogModality	Sheet(WindowRef inParent);
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Constructor.
	explicit	DialogModality(
					WindowModality		inModality,
					WindowRef			inParent);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the modality.
	WindowModality	GetModality() const	{ return (mModality); }
	//! Returns the sheet's parent, if any.
	WindowRef		GetParent() const	{ return (mParent); }
	//! Do we specify a modeless dialog?
	bool			IsModeless() const	{ return (mModality == kWindowModalityNone); }
	//! Do we specify an application-modal dialog?
	bool			IsModal() const		{ return (mModality == kWindowModalityAppModal); }
	//! Do we specify a sheet?
	bool			IsSheet() const		{ return (mModality == kWindowModalityWindowModal); }
	//@}
	
private:
	
	DialogModality(
		WindowModality		inModality,
		WindowRef			inParent, 
		bool);
	
	// member variables
	WindowModality	mModality;
	WindowRef		mParent;
};

// ------------------------------------------------------------------------------------------
inline
DialogModality::DialogModality(
	WindowModality		inModality,
	WindowRef			inParent,
	bool)
	: mModality(inModality), mParent(inParent)
{
}


}	// namespace B


#endif	// BDialogModality_H_
