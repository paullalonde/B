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

// file header
#include "BUndoAction.h"

// B headers
#include "BErrorHandler.h"


namespace B {

// ==========================================================================================
//  UndoAction

#pragma mark * UndoAction *

// ------------------------------------------------------------------------------------------
UndoAction::UndoAction()
    : mRefCount(0)
{
}

// ------------------------------------------------------------------------------------------
/*! @exception  none
*/
UndoAction::~UndoAction()
{
}

// ------------------------------------------------------------------------------------------
/*! This is a pure abstract function, which all derived classes must override.
*/
void
UndoAction::Perform()
{
    // The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
/*! This function is used by an implementation of UNDO_POLICY to support merging undo 
    actions.  It could be used, for example, to support undoable typing in a text field, 
    where an %Undo should remove an entire typing sequence, rather than a single typestroke.
    
    The UNDO_POLICY calls this function prior to adding this action to its collection of 
    undo actions.  If the function returns @c false, the action is added to the collection.
    If it returns @c true, it is @e not added to the collection and is destroyed shortly 
    thereafter.
    
    The function is given the last undo action in the policy's collection;  it may be 
    examined to determine whether to return @c true or @c false.
    
    Here's an example of how Coalesce() might be overridden:
    
    @code
        bool
        MyTypingAction::Coalesce(UndoAction* inLastAction)
        {
            bool coalesce = false;
            
            if (MyTypingAction* lastTypingAction = dynamic_cast<MyTypingAction*>(inLastAction))
            {
                // Make sure both typing actions refer to the same text field.
                if (GetTextField() == lastTypingAction->GetTextField())
                {
                    // transfer our saved state into the last action.
                    lastTypingAction->MergeTypingState(this);
                    coalesce = true;
                }
            }
            
            return (coalesce);
        }
    @endcode
    
    @param  inLastAction    The last undoable action.  May be @c NULL.
*/
bool
UndoAction::Coalesce(
    UndoAction* /* inLastAction */)
{
    return (false);
}

// ------------------------------------------------------------------------------------------
void    intrusive_ptr_add_ref(UndoAction* a)
{
    B_ASSERT(a != NULL);
    
    IncrementAtomic(&a->mRefCount);
}

// ------------------------------------------------------------------------------------------
void    intrusive_ptr_release(UndoAction* a)
{
    B_ASSERT(a != NULL);
    B_ASSERT(a->mRefCount > 0);
    
    if (DecrementAtomic(&a->mRefCount) == 1)
    {
        // The ref count is now zero
        delete a;
    }
}


// ==========================================================================================
//  FunctorUndoAction

#pragma mark -
#pragma mark * FunctorUndoAction *

// ------------------------------------------------------------------------------------------
FunctorUndoAction::FunctorUndoAction(
    FunctorType inFunctor)  //!< The functor.
        : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
void
FunctorUndoAction::Perform()
{
    mFunctor();
}


}   // namespace B
