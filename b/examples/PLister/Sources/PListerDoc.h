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

#ifndef PListerDoc_H_
#define PListerDoc_H_

#pragma once

// system headers
#include <CoreFoundation/CFPropertyList.h>

// B headers
#include "BDocument.h"
#include "BDocumentWindow.h"
#include "BMultipleUndoPolicy.h"
#include "BOSPtr.h"
#include "BNullPrintPolicy.h"

// project headers
#include "ModelItem.h"


class PListerDoc : public B::Document<B::MultipleUndoPolicy, B::NullPrintPolicy>
{
public:
    
    // constants
    enum        { kRootItemProperty = 'RItm' };
    
    // constructors / destructor
                PListerDoc(
                    B::AEObjectPtr  inContainer, 
                    DescType        inDocClass,
                    SInt32          inUniqueID);
    
    ModelItem*  GetRootItem() const { return (mRootItem.get()); }
    
    // overrides from AEObject
    virtual B::AEObjectPtr  GetPropertyObject(
                            DescType        inPropertyID) const;
    
protected:
    
    virtual void    MakeWindows(
                        B::Nib*             inNib);
    virtual void    InitDocument(
                        const B::String&    inName,
                        const B::String&    inDocumentType, 
                        const AEDesc&       inProperties, 
                        const AEDesc&       inData);
    virtual void    ReadDocumentFromBuffer(
                        const std::vector<UInt8>&   inBuffer, 
                        B::String&                  ioDocumentType);
    virtual void    WriteDocumentToBuffer(
                        std::vector<UInt8>&         outBuffer, 
                        B::SaveInfo&                ioSaveInfo);
    
private:
    
    // types
    typedef B::Document<B::MultipleUndoPolicy, B::NullPrintPolicy>  inherited;
    
    // member variables
    ModelItemPtr    mRootItem;
};

#endif // PListerDoc_H_
