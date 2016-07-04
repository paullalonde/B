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

// file header
#include "BCursor.h"

// B headers
#include "BErrorHandler.h"
#include "BFwd.h"
#include "BMemoryUtilities.h"

#define B_MISSING_THEME_CURSOR_BASE_ID	200


namespace B {

// ------------------------------------------------------------------------------------------
Cursor::ColorCursorWrapper::ColorCursorWrapper(CCrsrHandle inCrsrH)
	: mRefCount(0), mIsSet(false), mCrsrH(inCrsrH)
{
	B_ASSERT(inCrsrH != NULL);
}

// ------------------------------------------------------------------------------------------
Cursor::ColorCursorWrapper::~ColorCursorWrapper()
{
	// If we assert here, it means the color cursor is still set as the current cursor 
	// (or at least *we* think it is) at the same time as we're asked to dispose of it.
	B_ASSERT(!mIsSet);
	
	// Don't dispose if we are the current cursor.  It's better to leak than crash.
	if (!mIsSet)
		DisposeCCursor(mCrsrH);
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_add_ref(Cursor::ColorCursorWrapper* c)
{
	IncrementAtomic(&c->mRefCount);
}

// ------------------------------------------------------------------------------------------
void	intrusive_ptr_release(Cursor::ColorCursorWrapper* c)
{
	if (DecrementAtomic(&c->mRefCount) == 1)
		delete c;
}

// ------------------------------------------------------------------------------------------
/*!	The cursor is initialised to the arrow.
*/
Cursor::Cursor()
	: mKind(kind_Theme), mThemeCursor(kThemeArrowCursor)
{
}

// ------------------------------------------------------------------------------------------
Cursor::Cursor(const Cursor& inCursor)
	: mKind(inCursor.mKind), mThemeCursor(inCursor.mThemeCursor), 
	  mQuickdrawCursor(inCursor.mQuickdrawCursor), mColorCursor(inCursor.mColorCursor),
	  mNamedCursor(inCursor.mNamedCursor)
{
}

// ------------------------------------------------------------------------------------------
Cursor::Cursor(ThemeCursor inCursor)
	: mKind(kind_Theme), mThemeCursor(inCursor)
{
}

// ------------------------------------------------------------------------------------------
Cursor::Cursor(const QuickdrawCursor& inCursor)
	: mKind(kind_Quickdraw), mQuickdrawCursor(inCursor)
{
}

// ------------------------------------------------------------------------------------------
/*!	@note	The newly constructed object takes ownership of @a inCursor.
*/
Cursor::Cursor(CCrsrHandle inCursor)
	: mKind(kind_Color), mColorCursor(new ColorCursorWrapper(inCursor))
{
}

// ------------------------------------------------------------------------------------------
/*!	@note	The named pixmap cursor should already be registered.  See under 
			@c QDRegisterNamedPixMapCursor() in Quickdraw.h for more information.
*/
Cursor::Cursor(const std::string& inCursor)
	: mKind(kind_Named), mNamedCursor(inCursor)
{
}

// ------------------------------------------------------------------------------------------
/*!	The function uses the Resource Manager to look for a @c 'CURS' or @c 'crsr' resource 
	with ID @a inCursorID.  The particular resource type is determined by @a inColorCursor;  
	if true, we look for a @c 'crsr' resource, else we look for @c 'CURS'.
*/
Cursor::Cursor(
	short	inCursorID,		//!< The resource ID of the cursor.
	bool	inColorCursor)	//!< Should the cursor be color or black & white?
{
	B_ASSERT(!inColorCursor);
	
	if (inColorCursor)
	{
		// Color cursor.
		
		LoadColorCursor(inCursorID);
	}
	else
	{
		// Black & white cursor.
		
		LoadQuickdrawCursor(inCursorID);
	}
}

// ------------------------------------------------------------------------------------------
void
Cursor::LoadColorCursor(short inCursorID)
{
	CCrsrHandle	crsrH	= NULL;
	
	try
	{
		crsrH = GetCCursor(inCursorID);
		B_THROW_IF_NULL_RSRC(crsrH);
		
		mKind			= kind_Color;
		mColorCursor	= new ColorCursorWrapper(crsrH);
		mNamedCursor.clear();
	}
	catch (...)
	{
		if (crsrH != NULL)
			DisposeCCursor(crsrH);
	}
}

// ------------------------------------------------------------------------------------------
void
Cursor::LoadQuickdrawCursor(short inCursorID)
{
	CursHandle	cursorH	= GetCursor(inCursorID);
	
	B_THROW_IF_NULL_RSRC(cursorH);
	
	AutoTypedResourceHandle<QuickdrawCursor>	autoCursorH(cursorH);
	
	autoCursorH.load();
	
	AutoHandleLock	autoCursorLock(autoCursorH);
	
	mKind				= kind_Quickdraw;
	mQuickdrawCursor	= **autoCursorH.get();
	mColorCursor		= NULL;
	mNamedCursor.clear();
}

// ------------------------------------------------------------------------------------------
Cursor&
Cursor::operator = (const Cursor& inCursor)
{
	Cursor(inCursor).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Cursor&
Cursor::operator = (ThemeCursor inCursor)
{
	Cursor(inCursor).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Cursor&
Cursor::operator = (const QuickdrawCursor& inCursor)
{
	Cursor(inCursor).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Cursor&
Cursor::operator = (CCrsrHandle inCursor)
{
	Cursor(inCursor).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Cursor&
Cursor::operator = (const std::string& inCursor)
{
	Cursor(inCursor).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
Cursor&
Cursor::Assign(short inCursorID, bool inColorCursor)
{
	Cursor(inCursorID, inColorCursor).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
Cursor::Set()
{
	static Cursor   sCurrentCursor;
	OSStatus		err;
	
	switch (mKind)
	{
	case kind_Theme:
		{
			err = SetThemeCursor(mThemeCursor);
			
			if (err != noErr)
			{
				// Some pretty useful theme cursors weren't defined until Panther, so 
				// we check for those and try to load them from our resources instead.
				
				switch (mThemeCursor)
				{
				case kThemeResizeUpCursor:
				case kThemeResizeDownCursor:
				case kThemeResizeUpDownCursor:
					LoadQuickdrawCursor(B_MISSING_THEME_CURSOR_BASE_ID + mThemeCursor);
					B_ASSERT(mKind == kind_Quickdraw);
					SetCursor(&mQuickdrawCursor);
					err = noErr;
					break;
				
				default:
					break;
				}
			}
			
			B_THROW_IF_STATUS(err);
		}
		break;
		
	case kind_Quickdraw:
		SetCursor(&mQuickdrawCursor);
		break;
		
	case kind_Color:
		SetCCursor(*mColorCursor);
		mColorCursor->IsSet(true);
		break;
		
	case kind_Named:
		err = QDSetNamedPixMapCursor(mNamedCursor.c_str());
		B_THROW_IF_STATUS(err);
		break;
	}
	
	sCurrentCursor.UnSet();
	sCurrentCursor = *this;
}

// ------------------------------------------------------------------------------------------
void
Cursor::UnSet()
{
	if (mKind == kind_Color)
		mColorCursor->IsSet(false);
}

// ------------------------------------------------------------------------------------------
void
Cursor::swap(Cursor& ioCursor)
{
	std::swap(mKind,            ioCursor.mKind);
	std::swap(mThemeCursor,     ioCursor.mThemeCursor);
	std::swap(mQuickdrawCursor, ioCursor.mQuickdrawCursor);
	std::swap(mColorCursor,     ioCursor.mColorCursor);
	std::swap(mNamedCursor,     ioCursor.mNamedCursor);
}


}	// namespace B
