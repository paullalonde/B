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

#ifndef BUTI_H_
#define BUTI_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

class AutoUTI
{
public:
	
	AutoUTI(OSType inOSType, CFStringRef inConformsTo);
	AutoUTI(const AutoUTI& src);
	
	AutoUTI&	operator = (const AutoUTI& src);
	
	CFStringRef	get() const		{ return (mUTI); }
	
private:
	
	OSPtr<CFStringRef>	mUTI;
};

}	// namespace B


#endif	// BUTI_H_
