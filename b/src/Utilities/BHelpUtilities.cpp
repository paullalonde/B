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
#include "BHelpUtilities.h"

// B headers
#include "BBundle.h"


namespace B {


// ------------------------------------------------------------------------------------------
bool
HelpUtilities::HasHelpBook()
{
	return (HasHelpBook(Bundle::Main()));
}

// ------------------------------------------------------------------------------------------
bool
HelpUtilities::HasHelpBook(
	const Bundle&	inBundle)	//!< The bundle in which to look for a help book name.
{
	return (!inBundle.GetHelpBook().empty());
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::RegisterHelpBook()
{
	RegisterHelpBook(Bundle::Main());
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::RegisterHelpBook(
	const Bundle&	inBundle)	//!< The bundle to register.  It's assumed to contain a help book.
{
	FSRef		ref;
	OSStatus	err;
	
	inBundle.Location().Copy(ref);
	
	err = AHRegisterHelpBook(&ref);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::GotoPage(
	const Url&		inUrl)
{
	GotoPage(Bundle::Main(), inUrl);
}

// ------------------------------------------------------------------------------------------
/*!	@a inUrl is assumed to contain a relative file URL, and may optionally contain a 
	fragment which identifies the anchor to display.
*/
void
HelpUtilities::GotoPage(
	const Bundle&	inBundle,	//!< The bundle containing the help book in which to look for @a inUrl.
	const Url&		inUrl)		//!< The url for the page.
{
	GotoPage(inBundle.GetHelpBook(), inUrl);
}

// ------------------------------------------------------------------------------------------
/*!	@a inUrl is assumed to contain a file URL relative to the help book's location, and 
	may optionally contain a fragment which identifies the anchor to display.
*/
void
HelpUtilities::GotoPage(
	const String&	inBookName,	//!< The help book in which to look for @a inUrl.
	const Url&		inUrl)		//!< The url for the page.
{
	String	path, anchor;
	bool	junk;
	
	if (inUrl.GetPath(path, junk))
	{
		path = Url::Unescape(path);
	}
	
	inUrl.GetFragment(anchor);
	
	GotoPage(inBookName, path, anchor);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::GotoPage(
	const String&	inBookName,	//!< The help book in which to look for @a inUrl.
	const String&	inPath,		//!< The path of the help page, relative to the help book's location.
	const String&	inAnchor)	//!< The anchor within the page.
{
	OSStatus	err;
	
	err = AHGotoPage(inBookName.empty() ? NULL : inBookName.cf_ref(), 
					 inPath.empty()     ? NULL : inPath.cf_ref(), 
					 inAnchor.empty()   ? NULL : inAnchor.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::GotoTOC(
	AHTOCType		inTOCType /* = kAHTOCTypeUser */)	//!< The table-of-contents to display.
{
	OSStatus	err;
	
	err = AHGotoMainTOC(inTOCType);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::Search(
	const String&	inSearchText)	//!< The text to search for.
{
	Search(Bundle::Main(), inSearchText);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::Search(
	const Bundle&	inBundle,		//!< The bundle containing the help book in which to look for @a inSearchText.
	const String&	inSearchText)	//!< The text to search for.
{
	Search(inBundle.GetHelpBook(), inSearchText);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::Search(
	const String&	inBookName,		//!< The help book in which to look for @a inSearchText.
	const String&	inSearchText)	//!< The text to search for.
{
	OSStatus	err;
	
	err = AHSearch(inBookName.cf_ref(), inSearchText.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::SearchAll(
	const String&	inSearchText)	//!< The text to search for.
{
	OSStatus	err;
	
	err = AHSearch(NULL, inSearchText.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::Lookup(
	const String&	inAnchor)		//!< The anchor to look for.
{
	Lookup(Bundle::Main(), inAnchor);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::Lookup(
	const Bundle&	inBundle,		//!< The bundle containing the help book in which to look for @a inAnchor.
	const String&	inAnchor)		//!< The anchor to look for.
{
	Lookup(inBundle.GetHelpBook(), inAnchor);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::Lookup(
	const String&	inBookName,		//!< The help book in which to look for @a inAnchor.
	const String&	inAnchor)		//!< The anchor to look for.
{
	OSStatus	err;
	
	err = AHLookupAnchor(inBookName.cf_ref(), inAnchor.cf_ref());
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
HelpUtilities::LookupAll(
	const String&	inAnchor)		//!< The anchor to look for.
{
	OSStatus	err;
	
	err = AHLookupAnchor(NULL, inAnchor.cf_ref());
	B_THROW_IF_STATUS(err);
}


}	// namespace B
