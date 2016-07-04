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

// system headers
#include <Carbon/Carbon.r>

// B headers
#include "BFwd.r"


// ------------------------------------------------------------------------------------------
//	'DITL' #200 and 'CNTL' #200 are used in the custom control area of Navigation 
//	Services dialogs.

// ### IMPORTANT: THE DEFINITION BELOW NEEDS TO MATCH THE ONE IN BNavDialogs.cpp ###
#define B_NAV_CUSTOM_AREA_ID	200

resource 'DITL' (B_NAV_CUSTOM_AREA_ID, purgeable)
{
	{
		{ 0, 0, 32, 128 },
		Control {
			enabled,
			B_NAV_CUSTOM_AREA_ID
		},
	}
};

resource 'CNTL' (B_NAV_CUSTOM_AREA_ID, purgeable)
{
	{ 0, 0, 32, 128 },
	kControlSupportsEmbedding,
	visible, 0, 0, 
	kControlUserPaneProc,
	0, ""
};

// ------------------------------------------------------------------------------------------
//	Commonly-used cursors that for some inexplicable reason weren't available via 
//	SetThemeCursor until Panther!

// ### IMPORTANT: THE DEFINITION BELOW NEEDS TO MATCH THE ONE IN BCursor.cpp ###
#define B_MISSING_THEME_CURSOR_BASE_ID	200

resource 'CURS' (B_MISSING_THEME_CURSOR_BASE_ID + kThemeResizeUpCursor, purgeable)
{
	$"0000 0180 03C0 07E0 0180 0180 0180 7FFE 7FFE 0000 0000 0000 0000 0000 0000 0000",
	$"0180 03C0 07E0 0FF0 0FF0 03C0 FFFF FFFF FFFF FFFF 0000 0000 0000 0000 0000 0000",
	{ 8, 8 }
};

resource 'CURS' (B_MISSING_THEME_CURSOR_BASE_ID + kThemeResizeDownCursor, purgeable)
{
	$"0000 0000 0000 0000 0000 0000 0000 7FFE 7FFE 0180 0180 0180 07E0 03C0 0180 0000",
	$"0000 0000 0000 0000 0000 0000 FFFF FFFF FFFF FFFF 03C0 0FF0 0FF0 07E0 03C0 03C0",
	{ 8, 8 }
};

resource 'CURS' (B_MISSING_THEME_CURSOR_BASE_ID + kThemeResizeUpDownCursor, purgeable)
{
	$"0000 0180 03C0 07E0 0180 0180 0180 7FFE 7FFE 0180 0180 0180 07E0 03C0 0180 0000",
	$"0180 03C0 07E0 0FF0 0FF0 03C0 FFFF FFFF FFFF FFFF 03C0 0FF0 0FF0 07E0 03C0 03C0",
	{ 8, 8 }
};
