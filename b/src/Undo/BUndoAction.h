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

#ifndef BUndoAction_H_
#define BUndoAction_H_

#pragma once

// library headers
#include <boost/function.hpp>
#include <boost/utility.hpp>


namespace B {

// forward declarations
class   UndoActionGroup;


// ==========================================================================================
//  UndoAction

#pragma mark UndoAction

/*! @brief  Abstract base class for undoable actions.
    
    Classes derived from UndoAction are meant to store state of some kind.  When 
    Perform() is called, that state should be restored.
    
    Instances of UndoAction are passed to implementations of UNDO_POLICY and are maintained 
    by them until an %Undo or %Redo action is performed.
    
    @sa         @ref using_undo, Undo, UNDO_POLICY
    @ingroup    UndoGroup
*/
class UndoAction : public boost::noncopyable
{
public:
    
    //! Destructor
    virtual ~UndoAction();
    
    /*! Invocation.  The object should perform its action.
        
        This is a pure abstract function, which all derived classes must override.
    */
    virtual void    Perform() = 0;
    
    //! Coalescing of undo actions.
    virtual bool    Coalesce(UndoAction* inLastAction);
    
protected:
    
    //! Constructor.
    UndoAction();
    
private:
    
    // member variables
    SInt32  mRefCount;
    
    // friends
    friend class    UndoActionGroup;
    friend void     intrusive_ptr_add_ref(UndoAction* a);
    friend void     intrusive_ptr_release(UndoAction* a);
};


// ==========================================================================================
//  FunctorUndoAction

#pragma mark -
#pragma mark FunctorUndoAction

/*! @brief  An undoable action that delegates to functor.
    
    This convenience class makes it easy to build an undoable action out of any object 
    convertible to a @c boost::function;  this includes objects generated by @c boost::bind, 
    meaning that one can use @c boost::bind to pass member functions to FunctorUndoAction's 
    constructor.
    
    The functor must take no arguments and return no result.  If it needs to contain state, 
    @c boost::bind may be used to bind state to the functor.
    
    Instances of FunctorUndoAction are passed to implementations of UNDO_POLICY and are 
    maintained by them until an %Undo or %Redo action is performed.
    
    @sa         @ref using_undo, Undo, UNDO_POLICY
    @ingroup    UndoGroup
*/
class FunctorUndoAction : public UndoAction
{
public:
    
    //! @name Types
    //@{
    typedef boost::function0<void>  FunctorType;    //!< The type of the functor.
    //@}

    //! Constructor.
            FunctorUndoAction(FunctorType inFunctor);
    
    //! Invocation.  The object should perform its action.
    virtual void    Perform();
    
protected:
    
    // member variables
    FunctorType mFunctor;   //!< The functor.
};


}   // namespace B


#endif  // BUndoAction_H_
