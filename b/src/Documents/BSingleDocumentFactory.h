// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

#ifndef BDocumentFactory_H_
#define BDocumentFactory_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// library headers
#include <boost/utility.hpp>

// B headers
#include "BErrorHandler.h"


namespace B {

// forward declarations
class	AbstractDocument;
class	AEObject;


/*!
	@brief	A document factory for applications that only support a single document type.
	
	Use it as the DOC_FACTORY residing within your application's DOC_POLICY.
	
	Since the document's type is predetermined, it is specified in the @a DOC_TYPE and 
	@a OSA_CLASS template parameters.
	
	@sa	DOC_FACTORY.
*/
template <class DOC_TYPE, DescType OSA_CLASS = cDocument>
class SingleDocumentFactory : public boost::noncopyable
{
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	/*!	@brief	The C++ class for a document.
	
		The class must have a constructor of the following form:
		
		@code
			DOC_TYPE(AEObject* inContainer, DescType inDocClass);
		@endcode
	*/
	typedef class		DOC_TYPE;
	typedef DescType	OSA_CLASS;	//!< The AEOM class identifier for a document (@c cDocument by default).
	//@}
#endif
	
	//! @name Constructor
	//@{
	//! Constructor.
	SingleDocumentFactory();
	//@}
	
	//! @name Instantiation
	//@{
	//! See DOC_FACTORY for description
	std::auto_ptr<AbstractDocument>
		Instantiate(
			AEObjectPtr	inContainer, 
			DescType	inDocClass,
			SInt32		inUniqueID);
	//@}
};


// ------------------------------------------------------------------------------------------
template <class DOC_TYPE, DescType OSA_CLASS>
SingleDocumentFactory<DOC_TYPE, OSA_CLASS>::SingleDocumentFactory() {}

// ------------------------------------------------------------------------------------------
/*!	Creates an instance of @a DOC_TYPE.
	
	@throws	An exception if the document can't be constructed, or if @a inDocClass doesn't 
			match @a OSA_CLASS.
*/
template <class DOC_TYPE, DescType OSA_CLASS> std::auto_ptr<AbstractDocument>
SingleDocumentFactory<DOC_TYPE, OSA_CLASS>::Instantiate(
	AEObjectPtr	inContainer,	//!< The new document's container in the AEOM hierarchy.
	DescType	inDocClass,		//!< The new document's AEOM class.
	SInt32		inUniqueID)		//!< The new document's unique id.
{
	if (inDocClass != OSA_CLASS)
		B_THROW_STATUS(errAECantHandleClass);
	
	return (std::auto_ptr<AbstractDocument>(new DOC_TYPE(inContainer, inDocClass, inUniqueID)));
}


}	// namespace B


#endif	// BDocumentFactory_H_
