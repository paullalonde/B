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

#ifndef BMultipleDocumentFactory_H_
#define BMultipleDocumentFactory_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// standard headers
#include <map>

// system headers
#include <ApplicationServices/ApplicationServices.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class	AbstractDocument;
class	AEObject;


/*!
	@brief	A document factory for applications that support multiple document types.
	
	Use it as the DOC_FACTORY residing within your application's DOC_POLICY.
	
	Since this factory can instantiate any number of different document types, it 
	relies on a registration mechanism.  Document classes are registered by 
	calling RegisterDocumentFactory() and passing it an AEOM class ID and a factory 
	function (of type @c FactoryProc).  There is also a convenience routine, 
	RegisterDocumentClass(), which avoids the need to define your own @a FactoryProc.
	
	@sa	DOC_FACTORY.
*/
class MultipleDocumentFactory : public boost::noncopyable
{
public:
	
	//! @name Types
	//@{
	//! This function type instantiates a document object, given its container and AEOM class.
	typedef std::auto_ptr<AbstractDocument>
			(*FactoryProc)(
				AEObjectPtr	inContainer, 
				DescType	inDocClass, 
				SInt32		inUniqueID);
	//@}
	
	//! @name Constructor
	//@{
	//! Constructor.
	MultipleDocumentFactory();
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
	
	//! @name Registration
	//@{
	
	//! Associates an AEOM class ID (@a inDocClass) with an instantiation function.
	void	RegisterDocumentFactory(
				DescType	inDocClass, 
				FactoryProc	inProc);
	
	/*! Convenience routine for registering a given document class.
		
		@param	DOC_TYPE	Template Parameter.  The C++ class for a document.
		@param	OSA_CLASS	Template Parameter.  The AEOM class identifier for a document.
	*/
	template <class DOC_TYPE, DescType OSA_CLASS>
	void	RegisterDocumentClass();
	//@}
	
private:
	
	// types
	
	template <class DOC_TYPE> struct Registrar
	{
		static std::auto_ptr<AbstractDocument>
			MakeDoc(
				AEObjectPtr	inContainer, 
				DescType	inDocClass,
				SInt32		inUniqueID);
	};
	
	typedef std::map<DescType, FactoryProc>	FactoryMap;
	
	// member variables
	FactoryMap	mFactories;
};

// ------------------------------------------------------------------------------------------
/*!	To use this function, a document class needs to have a public constructor of the 
	following form:
	
	@code
		DOC_TYPE(AEObject* inContainer, DescType inDocClass);
	@endcode
*/
template <class DOC_TYPE, DescType OSA_CLASS> void
MultipleDocumentFactory::RegisterDocumentClass()
{
	RegisterDocumentFactory(OSA_CLASS, Registrar<DOC_TYPE>::MakeDoc);
}

// ------------------------------------------------------------------------------------------
template <class DOC_TYPE> std::auto_ptr<AbstractDocument>
MultipleDocumentFactory::Registrar<DOC_TYPE>::MakeDoc(
	AEObjectPtr	inContainer,	//!< The new document's container in the AEOM hierarchy.
	DescType	inDocClass,		//!< The new document's AEOM class.
	SInt32		inUniqueID)		//!< The new document's unique id.
{
	return (std::auto_ptr<AbstractDocument>(new DOC_TYPE(inContainer, inDocClass, inUniqueID)));
}


}	// namespace B


#endif	// BMultipleDocumentFactory_H_
