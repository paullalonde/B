// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

#ifndef BWindowUtils_H_
#define BWindowUtils_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BAEObject.h"
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BNib.h"
#include "BRect.h"
#include "BViewFactory.h"


namespace B {

// forward declarations
class   MutableShape;
class   Point;
class   Rect;
class   Size;


/*! @brief  A collection of useful utility functions for getting information about 
            @c WindowRefs.
    
    These functions are here for the benefit of classes other than Window, especially 
    the view classes.  They often need certain pieces of information about the owning 
    window of a view and isolating that information here avoids introducing a dependency 
    between the view and window classes.
*/
namespace WindowUtils
{
    //! @name General
    //@{
    //! Is @a inWindowRef a composited window?
    bool        IsComposited(
                    WindowRef   inWindowRef);
    //! Is @a inWindowRef a Metal window?
    bool        IsMetal(
                    WindowRef   inWindowRef);
    //! Returns the window class of @a inWindowRef.
    WindowClass GetClass(
                    WindowRef   inWindowRef);
    //! Returns the window atributes of @a inWindowRef.
    WindowAttributes
                GetAttributes(
                    WindowRef   inWindowRef);
    //! Returns the window modality of @a inWindowRef.
    WindowModality
                GetModality(
                    WindowRef   inWindowRef);
    //@}
    
    //! @name Content View
    //@{
    
    //! Returns @a inWindowRef's content view.
    HIViewRef   GetContentView(
                    WindowRef   inWindowRef);
    /*! @overload
    */
    HIViewRef   GetContentView(
                    WindowRef   inWindowRef, 
                    bool        inComposited);
    
    //! Returns the bounds of @a inWindowRef's content view.
    Rect        GetContentViewBounds(
                    WindowRef   inWindowRef);
    /*! @overload
    */
    Rect        GetContentViewBounds(
                    WindowRef   inWindowRef, 
                    bool        inComposited);
    /*! @overload
    */
    Rect        GetContentViewBounds(
                    WindowRef   inWindowRef, 
                    bool        inComposited, 
                    HIViewRef   inContentView);
    //@}
    
    //! @name Bounds, Origins, and Sizes
    //@{
    //! Returns the bounds of @a inWindowRef's structure region, in global coordinates.
    Rect        GetStructureBounds(
                    WindowRef   inWindowRef);
    //! Returns the origin of @a inWindowRef's structure region, in global coordinates.
    Point       GetStructureOrigin(
                    WindowRef   inWindowRef);
    //! Returns the size of @a inWindowRef's structure region, in global coordinates.
    Size        GetStructureSize(
                    WindowRef   inWindowRef);
    
    //! Returns the bounds of @a inWindowRef's content region, in global coordinates.
    Rect        GetContentBounds(
                    WindowRef   inWindowRef);
    //! Returns the origin of @a inWindowRef's content region, in global coordinates.
    Point       GetContentOrigin(
                    WindowRef   inWindowRef);
    //! Returns the size of @a inWindowRef's content region, in global coordinates.
    Size        GetContentSize(
                    WindowRef   inWindowRef);
    //@}
    
    //! @name Coordinate Conversions
    //@{
    
    //! Converts @a inPoint from the window coordinates of @a inWindowRef to its local coordinates.
    CGPoint ConvertFromWindowToLocal(
                const CGPoint&  inPoint,
                WindowRef       inWindowRef);
    /*! @overload
    */
    CGRect  ConvertFromWindowToLocal(
                const CGRect&   inRect,
                WindowRef       inWindowRef);
    
    //! Converts @a inPoint from the local coordinates of @a inWindowRef to its window coordinates.
    CGPoint ConvertFromLocalToWindow(
                const CGPoint&  inPoint,
                WindowRef       inWindowRef);
    /*! @overload
    */
    CGRect  ConvertFromLocalToWindow(
                const CGRect&   inRect,
                WindowRef       inWindowRef);
    //@}
    

// ==========================================================================================
//  WindowIterator

#pragma mark WindowIterator

/*!
    @brief  Iterates over the children of an HIView.
    
    This is a forward iterator, but without the ability to modify the referenced 
    sequence.  It's suitable for use in typical "const_iterator" situations.
    
    The iteration starts with the parent view's first child, then proceeds forwards.
*/
class WindowIterator : public std::iterator<std::forward_iterator_tag, 
                                            WindowRef, 
                                            ptrdiff_t, 
                                            WindowRef*, 
                                            WindowRef&>
{
public:
    
    //! @name Constructors / Destructor
    //@{
    //! Default constructor.  It's equivalent to an end-of-sequence iterator.
    WindowIterator();
    //! Copy constructor.
    WindowIterator(const WindowIterator& it);
    //! Argument constructor.
    WindowIterator(WindowClass inClass, bool inOnlyVisibleWindows);
    //@}
    
    //! @name Assignment
    //@{
    //! Copy assignment.
    WindowIterator& operator = (const WindowIterator& it);
    //@}
    
    //! @name Iterator Specifics
    //@{
    //! Returns the @c HIViewRef that the iterator currently points to.
    value_type      operator * () const;
    //! Advances the iterator, returning the new position.
    WindowIterator& operator ++ ();
    //! Advances the iterator, returning the old position.
    WindowIterator  operator ++ (int);
    //@}
    
private:
    
    //! Constructor.
    WindowIterator(WindowClass inClass, bool inOnlyVisibleWindows, WindowRef inRef);
    
    // member variables
    WindowClass mClass;
    bool        mVisible;
    WindowRef   mRef;
};

// ------------------------------------------------------------------------------------------
inline
WindowIterator::WindowIterator()
    : mClass(kAllWindowClasses), mVisible(true), mRef(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
WindowIterator::WindowIterator(const WindowIterator& it)
    : mClass(it.mClass), mVisible(it.mVisible), mRef(it.mRef)
{
}

// ------------------------------------------------------------------------------------------
inline
WindowIterator::WindowIterator(WindowClass inClass, bool inOnlyVisibleWindows)
    : mClass(inClass), mVisible(inOnlyVisibleWindows), 
      mRef(GetFrontWindowOfClass(mClass, mVisible))
{
}

// ------------------------------------------------------------------------------------------
inline WindowIterator&
WindowIterator::operator = (const WindowIterator& it)
{
    mClass      = it.mClass;
    mVisible    = it.mVisible;
    mRef        = it.mRef;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline WindowIterator::value_type
WindowIterator::operator * () const
{
    return (mRef);
}

// ------------------------------------------------------------------------------------------
inline WindowIterator&
WindowIterator::operator ++ ()
{
    mRef = GetNextWindowOfClass(mRef, mClass, mVisible);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline WindowIterator
WindowIterator::operator ++ (int)
{
    return (WindowIterator(mClass, mVisible, GetNextWindowOfClass(mRef, mClass, mVisible)));
}


// ==========================================================================================
//  WindowIterator Global Functions

//! Compares @a it1 and @a it2 for equality.
inline bool operator == (const WindowIterator& it1, const WindowIterator& it2)  { return (*it1 == *it2); }
//! Compares @a it1 and @a it2 for inequality.
inline bool operator != (const WindowIterator& it1, const WindowIterator& it2)  { return (*it1 != *it2); }


    //! @name Iterators
    //@{
    
    //! Returns an iterator pointing to the first window matching @a inClass and @a inOnlyVisibleWindows.
    inline WindowIterator
    begin(
        WindowClass inClass,                //!< The window class to look for.  May be @c kAllWindowClasses.
        bool        inOnlyVisibleWindows)   //!< Indicates whether to iterate over invisible windows.
    {
        return (WindowIterator(inClass, inOnlyVisibleWindows));
    }
    
    //! Returns an iterator pointing to the end of the windows matching @a inClass and @a inOnlyVisibleWindows.
    inline WindowIterator
    end(
        WindowClass /* inClass */,              //!< The window class to look for.  May be @c kAllWindowClasses.
        bool        /* inOnlyVisibleWindows */) //!< Indicates whether to iterate over invisible windows.
    {
        WindowIterator it;
        
        return (it);
    }
    //@}


}   // namespace WindowUtils

}   // namespace B


#endif  // BWindowUtils_H_
