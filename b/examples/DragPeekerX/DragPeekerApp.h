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

#ifndef DragPeekerXApp_H_
#define DragPeekerXApp_H_

#pragma once

#include "BApplication.h"
#include "BNullDocumentFactory.h"
#include "BNullDocumentPolicy.h"
#include "BNullPrintPolicy.h"
#include "BNullUndoPolicy.h"
#include "BUndoPolicyHelpers.h"


class Application : public B::Application<B::NullDocumentPolicy, B::NullUndoPolicy, B::NullPrintPolicy>
{
public:
            Application();
    virtual ~Application();
    
protected:
    
    // overrides from B::Application
    virtual void    RegisterViews(B::ViewFactory& ioFactory);
    
    // AppleEvent handlers
    virtual void    OpenApplication();
    virtual void    ReopenApplication();
    virtual B::AEObjectPtr
                    CreateObject(
                        DescType        inObjectClass,
                        DescType        inPosition,
                        B::AEObjectPtr  inTarget, 
                        const AERecord& inProperties,
                        const AEDesc&   inData);
    
    // CarbonEvent handlers
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        B::CommandData&             ioCmdData);

private:
    
    // types
    typedef B::Application<B::NullDocumentPolicy, B::NullUndoPolicy, B::NullPrintPolicy>    inherited;
    
    void    CreateDragPeekerWindow();
    
    // member variables
    unsigned    mNextWindowNumber;
};


#endif // DragPeekerXApp_H_
