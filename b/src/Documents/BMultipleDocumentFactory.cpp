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
#include "BMultipleDocumentFactory.h"

// project headers
#include "BAbstractDocument.h"
#include "BErrorHandler.h"
#include "BException.h"


namespace B {

// ------------------------------------------------------------------------------------------
MultipleDocumentFactory::MultipleDocumentFactory()
{
}

// ------------------------------------------------------------------------------------------
void
MultipleDocumentFactory::RegisterDocumentFactory(
    DescType        inDocClass, 
    FactoryProc     inProc)
{
    // Add the doc class => factory proc association to our map.
    mFactories.insert(FactoryMap::value_type(inDocClass, inProc));
}

// ------------------------------------------------------------------------------------------
std::auto_ptr<AbstractDocument>
MultipleDocumentFactory::Instantiate(
    AEObjectPtr inContainer, 
    DescType    inDocClass, 
    SInt32      inUniqueID)
{
    // Retrieve the factory proc for this doc class.
    FactoryMap::const_iterator  it  = mFactories.find(inDocClass);
    
    if (it == mFactories.end())
        B_THROW(AECantHandleClassException());
    
    return (std::auto_ptr<AbstractDocument>((*it->second)(inContainer, inDocClass, inUniqueID)));
}

}   // namespace B
