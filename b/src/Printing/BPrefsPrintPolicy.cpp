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
#include "BPrefsPrintPolicy.h"

// B headers
#include "BBundle.h"
#include "BPageFormat.h"


namespace B {

static const char* const	kPageFormatsKey	= "B::PrefsPrintPolicy::PageFormats";

// ------------------------------------------------------------------------------------------
PrefsPrintPolicy::PrefsPrintPolicy(
	EventTargetRef	inTarget, 
	const Bundle&	inBundle)
		: AbstractPrintPolicy(inTarget, inBundle)
{
	SetPrefsKey(Bundle::Main().Identifier());
}

// ------------------------------------------------------------------------------------------
PrefsPrintPolicy::~PrefsPrintPolicy()
{
}

// ------------------------------------------------------------------------------------------
void
PrefsPrintPolicy::SetPreferences(const Preferences& inPreferences)
{
	mPreferences = inPreferences;
}

// ------------------------------------------------------------------------------------------
void
PrefsPrintPolicy::SetPrefsKey(const B::String& inPrefsKey)
{
	mPrefsKey = inPrefsKey;
}

// ------------------------------------------------------------------------------------------
void
PrefsPrintPolicy::FillPageFormat(PageFormat& ioPageFormat)
{
	OSPtr<CFDictionaryRef>	dict(mPreferences.GetPtr<CFDictionaryRef>(kPageFormatsKey));
	
	if (dict.get() != NULL)
	{
		OSPtr<CFDataRef>	data(CFUGet<CFDataRef>(dict, mPrefsKey.cf_ref(), std::nothrow));
		
		if (data.get() != NULL)
		{
			ioPageFormat.Deserialize(data);
		}
	}
}

// ------------------------------------------------------------------------------------------
void
PrefsPrintPolicy::FillPrintSettings(PrintSettings& /* ioPrintSettings */)
{
}


}	// namespace B
