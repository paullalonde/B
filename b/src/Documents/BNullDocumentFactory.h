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

#ifndef BNullDocumentFactory_H_
#define BNullDocumentFactory_H_

#pragma once

// library headers
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class   AbstractDocument;
class   AEObject;


/*!
    @brief  A document factory for applications that don't have documents.
    
    Use it as the DOC_FACTORY residing within your application's DOC_POLICY.
    
    All of the member functions of NullDocumentFactory either do nothing or throw 
    an exception, as appropriate.
    
    @sa DOC_FACTORY.
*/
class NullDocumentFactory : public boost::noncopyable
{
public:
    
    //! @name Constructor
    //@{
    //! Constructor.  Does nothing.
    NullDocumentFactory() {}
    //@}
    
    //! @name Instantiation
    //@{
    //! See DOC_FACTORY for description
    std::auto_ptr<AbstractDocument> Instantiate(AEObjectPtr, DescType, SInt32);
    //@}
};


}   // namespace B


#endif  // BNullDocumentFactory_H_
