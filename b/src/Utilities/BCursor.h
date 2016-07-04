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

#ifndef BCursor_H_
#define BCursor_H_

#pragma	once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/intrusive_ptr.hpp>


namespace B {

/*!
	@brief	A simple cursor class
	
	This is a very simple concrete class that allows one to load a variety of different 
	cursor types (theme, Quickdraw black & white, Quickdraw color, named pixmap).  Of 
	course once loaded you can make a cursor current (ie change the shape of the pointer 
	on the screen).  Note that this class only manages the storage of a cursor's bits, 
	it doesn't provide screen management for cursor-tracking purposes.
*/
class Cursor
{
public:
	
	//! @name Types
	//@{
	//! Synonym for @c ::Cursor.
	typedef ::Cursor	QuickdrawCursor;
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
				Cursor();
	//! Copy constructor.
				Cursor(const Cursor& inCursor);
	//! @c ThemeCursor constructor.
	explicit	Cursor(ThemeCursor inCursor);
	//! @c QuickdrawCursor constructor.
	explicit	Cursor(const QuickdrawCursor& inCursor);
	//! @c CCrsrHandle constructor.
	explicit	Cursor(CCrsrHandle inCursor);
	//! Named pixmap constructor.
	explicit	Cursor(const std::string& inCursor);
	//! Resource ID constructor.
	explicit	Cursor(short inCursorID, bool inColorCursor);
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	Cursor&	operator = (const Cursor& inCursor);
	//! @c ThemeCursor assignment.
	Cursor&	operator = (ThemeCursor inCursor);
	//! @c QuickdrawCursor assignment.
	Cursor&	operator = (const QuickdrawCursor& inCursor);
	//! @c CCrsrHandle assignment.
	Cursor&	operator = (CCrsrHandle inCursor);
	//! Named pixmap assignment.
	Cursor&	operator = (const std::string& inCursor);
	//! Resource ID assignment.
	Cursor&	Assign(short inCursorID, bool inColorCursor);
	//@}
	
	//! @name Setting
	//@{
	//! Changes the cursor.
	void	Set();
	//@}
	
	//! @name Utility
	//@{
	//! Resets the cursor to the arrow.
	void	Clear();
	//! Exchanges the contents of two cursors.
	void	swap(Cursor& ioCursor);
	//@}
	
	//! Internal class for managing color cursors.
    class ColorCursorWrapper
    {
        ColorCursorWrapper(CCrsrHandle inCrsrH);
        ~ColorCursorWrapper();
        
        void	IsSet(bool inIsSet)				{ mIsSet = inIsSet; }
                operator CCrsrHandle () const   { return (mCrsrH); }
        
        // member variables
        SInt32		mRefCount;
        bool		mIsSet;
        CCrsrHandle mCrsrH;
        
        // friends
        friend class    Cursor;
        friend void     intrusive_ptr_add_ref(ColorCursorWrapper* c);
        friend void     intrusive_ptr_release(ColorCursorWrapper* c);
    };
	
private:
	
	// types
	enum EKind
	{
		kind_Theme = 1,
		kind_Quickdraw,
		kind_Color,
		kind_Named
	};
	
	typedef boost::intrusive_ptr<ColorCursorWrapper>	ColorCursorWrapperPtr;
	
	void	UnSet();
	void	LoadColorCursor(short inCursorID);
	void	LoadQuickdrawCursor(short inCursorID);
	
	// member variables
	EKind					mKind;
	ThemeCursor				mThemeCursor;
	QuickdrawCursor			mQuickdrawCursor;
	ColorCursorWrapperPtr	mColorCursor;
	std::string				mNamedCursor;
};

// ------------------------------------------------------------------------------------------
inline void
Cursor::Clear()
{
	Cursor().swap(*this);
}

// ------------------------------------------------------------------------------------------
inline void	swap(Cursor& ioCursor1, Cursor& ioCursor2)
{
	ioCursor1.swap(ioCursor2);
}


}	// namespace B

#endif	// BCursor_H_
