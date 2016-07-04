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
#include "BNullDocumentFactory.h"

// B headers
#include "BAbstractDocument.h"
#include "BErrorHandler.h"
#include "BException.h"


namespace B {

// ------------------------------------------------------------------------------------------
/*! Since this document policy doesn't instantiate documents, this implementation throws 
    an exception.
*/
std::auto_ptr<AbstractDocument>
NullDocumentFactory::Instantiate(AEObjectPtr, DescType, SInt32)
{
    B_THROW(AECantHandleClassException());
    
    // This code will never be reached;  it's only here to keep the compiler happy.
    return (std::auto_ptr<AbstractDocument>(NULL));
}


}   // namespace B
