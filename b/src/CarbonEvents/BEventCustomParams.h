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

#ifndef BEventCustomParams_H_
#define BEventCustomParams_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class	Nib;
class	UndoAction;
class	Window;

enum {
	kEventClassB				= FOUR_CHAR_CODE('B   '),
	
	kEventBCloseDocument		= 1,
	kEventBSaveDocument			= 2,
	kEventBContinueQuit			= 5,
	kEventBUndoBegin			= 6,
	kEventBUndoCommit			= 7,
	kEventBUndoAbort			= 8,
	kEventBUndoAdd				= 9,
	kEventBUndoEnable			= 10,
	
	kEventParamBFromUser		= FOUR_CHAR_CODE('BFrm'),	/* typeBoolean */
	kEventParamBUndoName		= FOUR_CHAR_CODE('BUnN'),	/* typeCFStringRef */
	kEventParamBUndoAction		= FOUR_CHAR_CODE('BUnA'),	/* typeBUndoAction */
	kEventParamBUndoEnable		= FOUR_CHAR_CODE('BUnE'),	/* typeBoolean */
	
	kEventParamToolbarBundle	= FOUR_CHAR_CODE('TBnd'),	/* typeCFTypeRef */
	kEventParamToolbarPList		= FOUR_CHAR_CODE('TPls'),	/* typeCFStringRef */
	kEventParamToolbarNib		= FOUR_CHAR_CODE('TNib'),	/* typeBNib */
	kEventParamToolbarItemData	= FOUR_CHAR_CODE('TItd'),	/* typeCFDictionaryRef */
	
	keyBExceptionState			= FOUR_CHAR_CODE('BExS'),	/* typeBExceptionState */
	
	typeBUndoAction				= FOUR_CHAR_CODE('BUdA'),	/* B::UndoAction* */
	typeBExceptionState			= FOUR_CHAR_CODE('BExS'),	/* blob */
	typeBNib					= FOUR_CHAR_CODE('BNib'),	/* Nib* */
	
	kHICommandOpenRecent		= 'BOpR',
	kHICommandOpenRecentFile	= 'BOpF',
	kHICommandClearRecent		= 'BClR',
	kHICommandCloseAll			= 'BClA',
	kHICommandSaveAll			= 'BSvA'
};

/*
    Parameters for B events:

    kEventBCloseDocument
        -->     kEventParamPostTarget		typeEventTargetRef

    kEventBSaveDocument
        -->     kEventParamPostTarget		typeEventTargetRef
        -->     kEventParamWindowRef		typeWindowRef
        -->     kEventParamBFromUser		typeBoolean

    kEventBSheetShowing
        -->     kEventParamWindowRef		typeWindowRef

    kEventBSheetHidden
        -->     kEventParamWindowRef		typeWindowRef

    kEventBContinueQuit
    	none

    kEventBUndoBegin
        -->     kEventParamBUndoName		typeCFStringRef	(optional)

    kEventBUndoCommit
    	none

    kEventBUndoAbort
    	none

    kEventBUndoAdd
        -->     kEventParamBUndoAction		typeBUndoAction

    kEventBUndoEnable
        -->     kEventParamBUndoEnable		typeBoolean
*/


#ifndef DOXYGEN_SKIP

template <> struct TaggedTypeTrait<typeBUndoAction>					{ typedef UndoAction*	Type;	};
template <> struct TaggedTypeTrait<typeBNib>						{ typedef Nib*			Type;	};

template <> struct EventParamNameTrait<kEventParamBFromUser>		{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamBUndoName>		{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamBUndoAction>		{ enum { kTypeTag = typeBUndoAction };	};
template <> struct EventParamNameTrait<kEventParamBUndoEnable>		{ enum { kTypeTag = typeBoolean };	};
template <> struct EventParamNameTrait<kEventParamToolbarBundle>	{ enum { kTypeTag = typeCFTypeRef };	};
template <> struct EventParamNameTrait<kEventParamToolbarPList>		{ enum { kTypeTag = typeCFStringRef };	};
template <> struct EventParamNameTrait<kEventParamToolbarNib>		{ enum { kTypeTag = typeBNib };	};
template <> struct EventParamNameTrait<kEventParamToolbarItemData>	{ enum { kTypeTag = typeCFDictionaryRef };	};

#endif	// DOXYGEN_SKIP


}	// namespace B


#endif	// BEventCustomParams_H_
