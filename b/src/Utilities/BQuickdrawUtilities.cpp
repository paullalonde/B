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
#include "BQuickdrawUtilities.h"

// B headers
#include "BErrorHandler.h"
#include "BRect.h"


namespace B {

// ==========================================================================================
//	AutoQDSavePort

// ------------------------------------------------------------------------------------------
AutoQDSavePort::AutoQDSavePort()
{
	GetPort(&mSavedPort);
}

// ------------------------------------------------------------------------------------------
AutoQDSavePort::AutoQDSavePort(GrafPtr inPort)
{
	B_ASSERT(inPort != NULL);
	
	GetPort(&mSavedPort);
	SetPort(inPort);
}

// ------------------------------------------------------------------------------------------
AutoQDSavePort::~AutoQDSavePort()
{
	if (mSavedPort != NULL)
		SetPort(mSavedPort);
}


// ==========================================================================================
//	AutoQDSaveClip

// ------------------------------------------------------------------------------------------
AutoQDSaveClip::AutoQDSaveClip()
{
	GetClip(mSavedClip);
}

// ------------------------------------------------------------------------------------------
AutoQDSaveClip::AutoQDSaveClip(RgnHandle inNewClip, bool intersect /* = false */)
{
	B_ASSERT(inNewClip != NULL);
	
	GetClip(mSavedClip);
	
	if (intersect)
	{
		AutoRegion	sectClip;
		
		SectRgn(mSavedClip, inNewClip, sectClip);
		SetClip(sectClip);
	}
	else
	{
		SetClip(inNewClip);
	}
}

// ------------------------------------------------------------------------------------------
AutoQDSaveClip::AutoQDSaveClip(const Rect& inNewClip, bool intersect /* = false */)
{
	GetClip(mSavedClip);
	
	::Rect	qdClip	= inNewClip;
	
	if (intersect)
	{
		AutoRegion	sectClip;
		
		RectRgn(sectClip, &qdClip);
		SetClip(sectClip);
	}
	else
	{
		ClipRect(&qdClip);
	}
}

// ------------------------------------------------------------------------------------------
AutoQDSaveClip::~AutoQDSaveClip()
{
	SetClip(mSavedClip);
}


// ==========================================================================================
//	AutoQDSaveDrawingState

// ------------------------------------------------------------------------------------------
AutoQDSaveDrawingState::AutoQDSaveDrawingState(bool inNormalize /* = true */)
{
	OSStatus	err;
	
	err = GetThemeDrawingState(&mSavedState);
	B_THROW_IF_STATUS(err);
	
	if (inNormalize)
		Normalize();
}

// ------------------------------------------------------------------------------------------
AutoQDSaveDrawingState::~AutoQDSaveDrawingState()
{
	OSStatus	err;
	
	err = SetThemeDrawingState(mSavedState, true);
}

// ------------------------------------------------------------------------------------------
void
AutoQDSaveDrawingState::Normalize()
{
	OSStatus	err;
	
	err = NormalizeThemeDrawingState();
	B_THROW_IF_STATUS(err);
}


// ==========================================================================================
//	AutoDMExtendedNotify

// ------------------------------------------------------------------------------------------
AutoDMExtendedNotify::AutoDMExtendedNotify(DMExtendedNotificationProcPtr proc, void* arg)
	: mUPP(NULL), mArg(arg)
{
	ProcessSerialNumber	psn;
	bool				registered	= false;
	
	try
	{
		OSStatus	err;
		
		err = GetCurrentProcess(&psn);
		B_THROW_IF_STATUS(err);
		
		mUPP = NewDMExtendedNotificationUPP(proc);
		B_THROW_IF_NULL(mUPP);
		
		err = DMRegisterExtendedNotifyProc(mUPP, mArg, 0, &psn);
		B_THROW_IF_STATUS(err);
		registered = true;
	}
	catch (...)
	{
		if (registered)
			DMRemoveExtendedNotifyProc(mUPP, mArg, &psn, 0);
		
		if (mUPP != NULL)
			DisposeDMExtendedNotificationUPP(mUPP);
		
		throw;
	}
}

// ------------------------------------------------------------------------------------------
AutoDMExtendedNotify::~AutoDMExtendedNotify()
{
	ProcessSerialNumber	psn;
	OSStatus			err;
	
	err = GetCurrentProcess(&psn);
	
	if (err == noErr)
	{
		err = DMRemoveExtendedNotifyProc(mUPP, mArg, &psn, 0);
	}
}


// ==========================================================================================
//	AutoGWorld

// ------------------------------------------------------------------------------------------
AutoGWorld::AutoGWorld(
	GWorldPtr		inGWorld)
		: mGWorld(inGWorld)
{
	B_ASSERT(mGWorld != NULL);
}

// ------------------------------------------------------------------------------------------
AutoGWorld::AutoGWorld(
	short			depth,
	const Rect&		bounds,
	CTabHandle		ctab /* = NULL */,
	GDHandle		device /* = NULL */,
	GWorldFlags		flags /* = 0 */)
{
	::Rect		qdBounds	= bounds;
	OSStatus	err;
	
	err = NewGWorld(&mGWorld, depth, &qdBounds, ctab, device, flags);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AutoGWorld::AutoGWorld(
	UInt32			pixelFormat,
	const Rect&		bounds,
	Ptr				buffer,
	long			rowBytes,
	CTabHandle		ctab /* = NULL */,
	GDHandle		device /* = NULL */,
	GWorldFlags		flags /* = 0 */)
{
	::Rect		qdBounds	= bounds;
	OSStatus	err;
	
	err = NewGWorldFromPtr(&mGWorld, pixelFormat, &qdBounds, 
						   ctab, device, flags, buffer, rowBytes);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AutoGWorld::~AutoGWorld()
{
	DisposeGWorld(mGWorld);
}

}	// namespace B
