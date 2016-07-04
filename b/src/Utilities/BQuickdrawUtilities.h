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

#ifndef BQuickdrawUtilities_H_
#define BQuickdrawUtilities_H_

#pragma	once

// system headers
#include <ApplicationServices/ApplicationServices.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BAutoRegion.h"


namespace B {

// forward declarations
class	Rect;


// ==========================================================================================
//	AutoQDSavePort

/*!	@todo	Document me!
*/
class AutoQDSavePort : public boost::noncopyable
{
public:
	
	AutoQDSavePort();
	AutoQDSavePort(GrafPtr inPort);
	~AutoQDSavePort();

private:
	
	GrafPtr	mSavedPort;
};


// ==========================================================================================
//	AutoQDSaveClip

/*!	@todo	Document me!
*/
class AutoQDSaveClip : public boost::noncopyable
{
public:
	
	AutoQDSaveClip();
	AutoQDSaveClip(RgnHandle inNewClip, bool intersect = false);
	AutoQDSaveClip(const Rect& inNewClip, bool intersect = false);
	~AutoQDSaveClip();

private:
	
	AutoRegion	mSavedClip;
};


// ==========================================================================================
//	AutoQDSaveDrawingState

/*!	@todo	Document me!
*/
class AutoQDSaveDrawingState : public boost::noncopyable
{
public:
	
	AutoQDSaveDrawingState(bool inNormalize = true);
	~AutoQDSaveDrawingState();
	
	static void	Normalize();
	
private:
	
	ThemeDrawingState	mSavedState;
};


// ==========================================================================================
//	AutoDMExtendedNotify

/*!	@todo	Document me!
*/
class AutoDMExtendedNotify : public boost::noncopyable
{
public:
	
	AutoDMExtendedNotify(DMExtendedNotificationProcPtr proc, void* arg);
	~AutoDMExtendedNotify();
	
private:
	
	DMExtendedNotificationUPP	mUPP;
	void*						mArg;
};


// ==========================================================================================
//	AutoGWorld

/*!	@todo	Document me!
*/
class AutoGWorld : public boost::noncopyable
{
public:
	
	AutoGWorld(
		GWorldPtr		inGWorld);
	AutoGWorld(
		short			depth,
		const Rect&		bounds,
		CTabHandle		ctab = NULL,
		GDHandle		device = NULL,
		GWorldFlags		flags = 0);
	AutoGWorld(
		UInt32			pixelFormat,
		const Rect&		bounds,
		Ptr				buffer,
		long			rowBytes,
		CTabHandle		ctab = NULL,
		GDHandle		device = NULL,
		GWorldFlags		flags = 0);
	~AutoGWorld();
	
	operator GWorldPtr () const	{ return (mGWorld); }
	
private:
	
	GWorldPtr	mGWorld;
};


}	// namespace B

#endif	// BQuickdrawUtilities_H_
