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
#include "BAutoTrackingArea.h"

// B headers
#include "BShape.h"
#include "BView.h"


namespace B {

// ------------------------------------------------------------------------------------------
AutoTrackingArea::AutoTrackingArea(
    HIViewRef               inViewRef,
    HIShapeRef              inShapeRef,
    HIViewTrackingAreaID    inID /* = 0 */)
{
    OSStatus    err;
    
    err = HIViewNewTrackingArea(inViewRef, inShapeRef, inID, &mTrackingArea);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
HIViewTrackingAreaID
AutoTrackingArea::GetID() const
{
    HIViewTrackingAreaID    areaID;
    OSStatus                err;
    
    err = HIViewGetTrackingAreaID(mTrackingArea, &areaID);
	B_THROW_IF_STATUS(err);
    
    return (areaID);
}

// ------------------------------------------------------------------------------------------
void
AutoTrackingArea::SetShape(HIShapeRef inShapeRef)
{
    OSStatus    err;
    
    err = HIViewChangeTrackingArea(mTrackingArea, inShapeRef);
	B_THROW_IF_STATUS(err);
}


}	// namespace B
