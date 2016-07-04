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

#ifndef BDocumentFactoryConcept_H_
#define BDocumentFactoryConcept_H_

#pragma once

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BAbstractDocument.h"


namespace B {

// forward declarations
class   AEObject;


#if DOXYGEN_SCAN

/*!
    @brief  Template for an document factory class.
    
    DOC_FACTORY isn't a class;  rather, its specifies the required interface for a 
    class in order for it to be useable as a template parameter.  So for example, 
    if a class @c Foo is declared like so:
    
    @code
        template <class DOC_POLICY> Foo { ... };
    @endcode
    
    and class @c Bar implements the interface described herein, then one can instantiate 
    @c Foo with @c Bar:
    
    @code
        class MyFoo : public Foo<Bar> { ... };
    @endcode
    
    The purpose of a DOC_FACTORY is to instantiate document objects, or more precisely 
    instances of classes derived from AbstractDocument, which is the base class for all 
    documents.  DOC_FACTORY is used by the various DOC_POLICY classes, although it could 
    be used directly.
    
    There are 3 notable examples of implementations of DOC_FACTORY:
        - NullDocumentFactory, for applications that don't use documents.
        - SingleDocumentFactory, for applications that only open one type of document.
        - MultipleDocumentFactory, for applications that can open more than one type of 
          document.
*/
class DOC_FACTORY
{
public:
    
    //! @name Constructor
    //@{
    //! Constructor.
    DOC_FACTORY();
    //@}
    
    //! @name Instantiation
    //@{
    /*! @brief  Returns a newly created document object.
        
        @return The newly created object.
        @throws An exception.  Never returns @c NULL.
    */
    std::auto_ptr<AbstractDocument>
        Instantiate(
            //! The new document's container in the AEOM hierarchy.
            AEObject*   inContainer, 
            //! The new document's AEOM class.
            DescType    inDocClass,
            //! The new document's unique id.
            SInt32      inUniqueID);
    //@}
};

#endif  // DOXYGEN_SCAN


#ifndef DOXYGEN_SKIP

template <class DOC_FACTORY> struct DocumentFactoryConcept
{
    void    constraints()
    {
        boost::function_requires< boost::DefaultConstructibleConcept<DOC_FACTORY> >();
        
        mDocument = mFactory.Instantiate(mContainer, mClass, mUniqueID);
    }
    
    DOC_FACTORY                     mFactory;
    std::auto_ptr<AbstractDocument> mDocument;
    AEObjectPtr                     mContainer;
    DescType                        mClass;
    SInt32                          mUniqueID;
};

#endif  // DOXYGEN_SKIP


}   // namespace B


#endif  // BDocumentFactoryConcept_H_
