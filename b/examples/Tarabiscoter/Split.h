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

#ifndef Split_H_
#define Split_H_

#pragma once

// B headers
#include "BWindow.h"


class Split : public B::Window
{
public:

	// constructor
	Split(
		WindowRef		inWindowRef,
		B::AEObjectPtr	inContainer);
	
protected:
	
	// overrides from B::Window
	virtual void	Awaken(B::Nib* inFromNib);
	
private:
	
	// types
	typedef B::Window   inherited;
};

#endif	// Split_H_
