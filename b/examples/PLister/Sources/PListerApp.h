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

#ifndef PListerApp_H_
#define PListerApp_H_

#pragma once

// B headers
#include "BApplication.h"
#include "BMultipleDocumentFactory.h"
#include "BMultipleDocumentPolicy.h"
#include "BNullUndoPolicy.h"
#include "BNullPrintPolicy.h"


class PListerAppDocPolicy : public B::MultipleDocumentPolicy< B::MultipleDocumentFactory >
{
public:
    
    // constructor
    PListerAppDocPolicy(
        B::AEObjectPtr      inApplication, 
        const B::Bundle&    inBundle, 
        B::Nib*             inAppNib);

protected:
    
    // factories
    virtual std::auto_ptr<B::GetFileDialog>
                CreateGetFileDialog(
                    UInt32                      inCommandID,
                    const B::DialogModality&    inModality);
    
private:
    
    typedef B::MultipleDocumentPolicy< B::MultipleDocumentFactory > inherited;
};


class PListerApp : public B::Application< PListerAppDocPolicy, B::NullUndoPolicy, B::NullPrintPolicy >
{
public:

    PListerApp();
    
protected:
    
    // overrides from B::Application
    virtual void    RegisterDocuments();
    virtual void    RegisterAppleEvents(B::AEObjectSupport& ioObjectSupport);
    
    // Carbon %Event handlers
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        B::CommandData&             ioCmdData);

private:
    
    // types
    typedef B::Application<PListerAppDocPolicy, B::NullUndoPolicy, B::NullPrintPolicy>  inherited;
};


#endif // PListerApp_H_
