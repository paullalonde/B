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
#include "BNullUndoPolicy.h"

// B headers
#include "BUndoAction.h"

// template instantiation
#include "BUndoPolicyHelpers.tpl.h"


namespace B {


// ------------------------------------------------------------------------------------------
NullUndoPolicy::NullUndoPolicy(
    EventTargetRef  inTarget,   //!< The event target onto which the object can install handlers.
    const Bundle&   inBundle)   //!< The bundle from which to retrieve resources.
        : mImpl(std::auto_ptr<ImplType>(new ImplType(inTarget, *this)))
{
    UndoStrings strings(inBundle);
    
    mUndoStr = strings.mCantUndoStr;
    mRedoStr = strings.mCantRedoStr;
}

// ------------------------------------------------------------------------------------------
void
NullUndoPolicy::RegisterAppleEvents(AEObjectSupport& ioObjectSupport)
{
    mImpl->RegisterAppleEvents(ioObjectSupport);
}

// ------------------------------------------------------------------------------------------
void
NullUndoPolicy::Add(std::auto_ptr<UndoAction> /* inUndoAction */)
{
    // Leaving the scope of this function will automatically delete @a inUndoAction.
}

// ------------------------------------------------------------------------------------------
void
NullUndoPolicy::Add(FunctorUndoAction::FunctorType /* inFunctor */)
{
}

}   // namespace B
