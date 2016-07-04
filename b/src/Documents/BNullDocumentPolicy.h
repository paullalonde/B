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

#ifndef BNullDocumentPolicy_H_
#define BNullDocumentPolicy_H_

#if defined(__MWERKS__)
#   pragma once
#endif

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class   AEObjectSupport;
class   Bundle;
class   Nib;
class   NullDocumentFactory;
class   String;


/*!
    @brief  A document policy for applications that don't have documents.
    
    Use it as the DOC_POLICY template parameter of Application.
    
    All of the member functions of NullDocumentPolicy either do nothing or throw 
    an exception, as appropriate.
    
    @sa DOC_POLICY.
*/
class NullDocumentPolicy
{
public:
    
    //! @name Types
    //@{
    //! The null document policy gets a null document factory.
    typedef NullDocumentFactory FactoryType;
    //@}
    
    //! @name Constructor & Destructor
    //@{
    //! See DOC_POLICY for description.
    NullDocumentPolicy(AEObjectPtr, const Bundle&, Nib*) {}
    //@}
    
    //! @name Setup
    //@{
    //! See DOC_POLICY for description
    void    Init() {}
    //! See DOC_POLICY for description
    void    RegisterAppleEvents(AEObjectSupport&) {}
    //@}
    
    //! @name Inquiries
    //@{
    //! See DOC_POLICY for description
    FactoryType&    GetFactory();
    //@}
    
    //! @name AppleEvent Handlers
    //@{
    //! See DOC_POLICY for description
    void        OpenApplication()   {}
    //! See DOC_POLICY for description
    void        ReopenApplication() {}
    //! See DOC_POLICY for description
    void        QuitApplication(OSType = kAEAsk);
    //! See DOC_POLICY for description
    AEObjectPtr OpenNewDocument(DescType, const AEDesc&, const AEDesc&);
    //@}
    
    //! @name Document Access
    //@{
    //! See DOC_POLICY for description
    SInt32      CountDocuments(DescType) const;
    //! See DOC_POLICY for description
    AEObjectPtr GetDocumentByIndex(DescType, unsigned) const;
    //! See DOC_POLICY for description
    AEObjectPtr GetDocumentByName(DescType, const String&) const;
    //! See DOC_POLICY for description
    AEObjectPtr GetDocumentByUniqueID(DescType, SInt32) const;
    //@}
    
private:
    
    static NullDocumentFactory  sFactory;
};

}   // namespace B


#endif  // BNullDocumentPolicy_H_
