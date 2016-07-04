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

#ifndef BUndoPolicyHelpers_H_
#define BUndoPolicyHelpers_H_

/*! @file   BUndoPolicyHelpers.h
    @brief  Helper classes for implementing UNDO_POLICY.
    
    Various implementations of UNDO_POLICY have common needs.  These needs have been 
    packaged as classes and placed here.
    
    Normal clients of UNDO_POLICY or Undo don't need the classes herein.
*/

#pragma once

// standard headers
#include <deque>
#include <stack>

// library headers
#include <boost/intrusive_ptr.hpp>
#include <boost/utility.hpp>

// B headers
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BString.h"
#include "BUndoAction.h"


namespace B {

// forward declarations
class   AEObjectSupport;
class   Bundle;


// ==========================================================================================
//  UndoPolicyEvents

#pragma mark UndoPolicyEvents

/*!
    @brief  Helper class for implementing UNDO_POLICY.
    
    This class implements the Apple %Event and Carbon %Event interfaces to UNDO_POLICY.
    This code is identical for all implementations, so it has been isolated here.  
    You don't need to use this class unless you are implementing your own UNDO_POLICY.
    
    @sa         UNDO_POLICY
    @ingroup    UndoGroup
*/
template <class UNDO_POLICY>
class UndoPolicyEvents : public boost::noncopyable
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    UNDO_POLICY;    //!< Template Parameter.  A class that implements the UNDO_POLICY policy.
    //@}
#endif

    //! @name Types
    //@{
    typedef UNDO_POLICY                     UndoPolicyType; //!< The concrete type of the UNDO_POLICY.
    typedef UndoPolicyEvents<UNDO_POLICY>   ImplType;       //!< The type of this class.
    //@}
    
    //! @name Constructor & Destructor
    //@{
    //! Constructor.
    UndoPolicyEvents(
        EventTargetRef  inTarget,   //!< The event target onto which the object can install handlers.
        UndoPolicyType& inPolicy);  //!< The UNDO_POLICY.
    //! Destructor.
    ~UndoPolicyEvents();
    //@}
    
    //! @name Setup
    //@{
    //! Installs Apple %Event handlers for Undo-related AppleEvents.
    void    RegisterAppleEvents(AEObjectSupport& ioObjectSupport);
    //@}
    
private:
    
    void    InitEventHandler(EventHandler& ioHandler);
    void    SendUndoCommand(UInt32 inCommandID);
    
    // Apple %Event Handlers
    void    HandleUndoAppleEvent(
                AEEvent<kAEMiscStandards, kAEUndo>& event);
    void    HandleRedoAppleEvent(
                AEEvent<kAEMiscStandards, kAERedo>& event);
    
    // Carbon %Event handlers
    bool    CommandProcess(
                Event<kEventClassCommand, kEventCommandProcess>&        event);
    bool    CommandUpdateStatus(
                Event<kEventClassCommand, kEventCommandUpdateStatus>&   event);
    bool    BUndoBegin(
                Event<kEventClassB, kEventBUndoBegin>&  event);
    bool    BUndoCommit(
                Event<kEventClassB, kEventBUndoCommit>& event);
    bool    BUndoAbort(
                Event<kEventClassB, kEventBUndoAbort>&  event);
    bool    BUndoAdd(
                Event<kEventClassB, kEventBUndoAdd>&    event);
    bool    BUndoEnable(
                Event<kEventClassB, kEventBUndoEnable>& event);
    
    // member variables
    EventHandler    mEventHandler;
    UndoPolicyType& mPolicy;
};


// ==========================================================================================
//  UndoActionGroup

#pragma mark -
#pragma mark UndoActionGroup

/*!
    @brief  Helper class for implementing UNDO_POLICY.
    
    This class is an undoable action that itself contains other undoable actions.  It's 
    used by implementations of UNDO_POLICY to support multiple nested open action groups.
    You don't need to use this class unless you are implementing your own UNDO_POLICY.
    
    @sa         UNDO_POLICY
    @ingroup    UndoGroup
*/
class UndoActionGroup : public UndoAction
{
public:
    
    //! @name Types
    //@{
    typedef boost::intrusive_ptr<UndoAction>        ActionPtr;      //!< Smart pointer to undoable action.
    typedef boost::intrusive_ptr<UndoActionGroup>   GroupPtr;       //!< Smart pointer to action group.
    typedef std::deque<ActionPtr>                   ActionStack;    //!< Stack of smart pointers to undoable actions.
    typedef std::stack<GroupPtr>                    GroupStack;     //!< Stack of smart pointers to action groups.
    //@}
    
    //! Group instantiation.
    static GroupPtr     MakeGroup(const String& inName);
    //! Action smart pointer instantiation.
    static ActionPtr    MakeActionPtr(std::auto_ptr<UndoAction> inAction);
    
    //! @name Constructor / Destructor
    //@{
    //! Constructor.
    UndoActionGroup(const String& inName);
    //@}
    
    //! @name Inquiries
    //@{
    //! Does the group contain any actions?
    bool            empty() const   { return (mActions.empty()); }
    //! Returns the "top" action.
    ActionPtr       top() const;
    //! Returns the group's name.
    const String&   GetName() const { return (mName); }
    //@}
    
    //! @name Modifiers
    //@{
    //! Sets the group's name.
    void            SetName(const String& inName)   { mName = inName; }
    //@}
    
    //! @name Sub-Groups
    //@{
    //! Creates a new group, adds it as an action of this group, and pushes it onto @a ioGroupStack.
    static void     PushSubGroup(const String& inName, GroupStack& ioGroupStack);
    //! Pops the top element of @a ioGroupStack and returns it.
    static GroupPtr PopSubGroup(GroupStack& ioGroupStack);
    //@}
    
    //! @name Actions
    //@{
    //! Appends @a inAction to the group.
    void            Add(ActionPtr inAction);
    //@}
    
    //! @name Execution
    //@{
    //! Executes the actions in the group.
    virtual void    Perform();
    //@}
    
private:
    
    ActionStack mActions;
    String      mName;
};

// ==========================================================================================
//  UndoStrings

#pragma mark -
#pragma mark UndoStrings

/*!
    @brief  Helper class for implementing UNDO_POLICY.
    
    This class holds the user-visible strings shared by the various implementations of 
    UNDO_POLICY.
    
    @sa         UNDO_POLICY
    @ingroup    UndoGroup
*/
class UndoStrings
{
public:
    
    //! Constructor.
    UndoStrings(const Bundle& inBundle);
    
    //! Returns a default string of the form "Undo".
    String  GetSimpleString(bool inUndo) const  { return (inUndo ? mSimpleUndoStr : mSimpleRedoStr); }
    //! Returns a string of the form "Can't Undo".
    String  GetCantString(bool inUndo) const    { return (inUndo ? mCantUndoStr : mCantRedoStr); }
    //! Returns a string of the form "Undo %1".
    String  GetActionString(bool inUndo) const  { return (inUndo ? mActionUndoStr : mActionRedoStr); }
    
    //! A string of the form "Undo".
    const String    mSimpleUndoStr;
    //! A string of the form "Redo".
    const String    mSimpleRedoStr;
    
    //! A string of the form "Can't Undo".
    const String    mCantUndoStr;
    //! A string of the form "Can't Redo".
    const String    mCantRedoStr;
    
    //! A string of the form "Undo %1".
    const String    mActionUndoStr;
    //! A string of the form "Redo %1".
    const String    mActionRedoStr;
    
    //! The default name for action groups.
    const String    mDefaultGroupName;
};

}   // namespace B


#endif  // BUndoPolicyHelpers_H_
