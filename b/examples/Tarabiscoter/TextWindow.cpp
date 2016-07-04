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
#include "TextWindow.h"

// B headers
#include "BTextFilters.h"
#include "BToolboxViews.h"


// ==========================================================================================
//	TextWindow

// ------------------------------------------------------------------------------------------
TextWindow::TextWindow(
	WindowRef		inWindowRef,
	B::AEObjectPtr	inContainer)
		: inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
TextWindow::Awaken(B::Nib* /* inFromNib */)
{
	B::TextFilters::CharacterSet	integerFilter(kCFCharacterSetDecimalDigit);
	B::TextField*					field1	= FindView<B::TextField>(1);
	
	field1->ConnectFilter(integerFilter);
	field1->ConnectFilter(B::TextFilters::Length(10));
	field1->ConnectValidationFailed(B::BeepOnValidationFailed());
}
