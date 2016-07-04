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
#include "BStringFormatter.h"

// The version of Boost.Format that's in the 1.31.0 release gives us a "possible unwanted ;" 
// warning under CodeWarrior, so suppress that warning (since we can't fix it).
#ifdef __MWERKS__
#   pragma warn_possunwant off
#endif


namespace B {

// ------------------------------------------------------------------------------------------
std::string
StringFormatter::MakeFormatString(const String& inFormatString)
{
	std::string	sstr;
	
	inFormatString.copy(sstr, kCFStringEncodingUTF8);
	
	return (sstr);
}

// ------------------------------------------------------------------------------------------
StringFormatter::StringFormatter(const String& inFormatString)
	: mFormatter(MakeFormatString(inFormatString))
{
}

// ------------------------------------------------------------------------------------------
StringFormatter::StringFormatter(const std::string& inFormatString)
	: mFormatter(inFormatString)
{
}

// ------------------------------------------------------------------------------------------
StringFormatter::StringFormatter(const char* inFormatString)
	: mFormatter(inFormatString)
{
}

// ------------------------------------------------------------------------------------------
StringFormatter::StringFormatter(const StringFormatter& inFormatter)
	: mFormatter(inFormatter.mFormatter)
{
}

// ------------------------------------------------------------------------------------------
StringFormatter&
StringFormatter::operator = (const StringFormatter& inFormatter)
{
	mFormatter = inFormatter.mFormatter;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
String
StringFormatter::extract() const
{
	return (String(mFormatter.str(), kCFStringEncodingUTF8));
}


}	// namespace B
