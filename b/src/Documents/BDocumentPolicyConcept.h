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

#ifndef BDocumentPolicyConcept_H_
#define BDocumentPolicyConcept_H_

#pragma once

// B headers
#include "BString.h"

#include "BDocumentFactoryConcept.h"


namespace B {

// forward declarations
class	AEObject;
class	AEObjectSupport;
class	Bundle;
class	Nib;


#if DOXYGEN_SCAN

/*!
	@brief	Template for an document policy class.
	
	DOC_POLICY isn't a class;  rather, its specifies the required interface for a 
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
	
	@todo	### COMPLETE THIS DESCRIPTION !!! ###
	
	Currently, only the Application class takes DOC_POLICY as a template parameter.
	
	There are 3 notable examples of implementations of DOC_POLICY:
		- NullDocumentPolicy, for applications that don't use documents.
		- FixedDocumentPolicy, for applications that only support a single hard-wired document.
		- MultipleDocumentPolicy, for applications that can open more than one document 
		  at a time.
*/

class DOC_POLICY
{
public:
	
	//! @name Types
	//@{
	/*! @brief	A synonym for a class capable of instantiating a document.
		
		This class is used internally to actually create the document objects.
	*/
	typedef class	FactoryType;
	//@}
	
	//! @name Constructor & Destructor
	//@{
	
	/*! Constructor.
		
		The implementation may install CarbonEvent handlers on @a inTarget for 
		document-related menu commands.  It may also keep a reference to @a inBundle 
		and @a inAppNib, so these objects' lifetimes need to be at least as long as 
		that of the document policy object.
	*/
	DOC_POLICY(
		//!< The AEObject representing the application.
		AEObject*		inApplication,
		//!< The bundle from which to obtain document type information.
		const Bundle&	inBundle,
		//!< The nib from which to load each document's UI;  it may be @c NULL, in which case it is assumed that windows will load their own nibs.
		Nib*			inAppNib);
	
	//! Destructor.  Releases resources.
	~DOC_POLICY();
	//@}
	
	//! @name Setup
	//@{
	
	/*! Post-constructor initialisation.  The application calls this once its initialisation 
		is complete.
	*/
	void	Init();
	
	/*! The application calls this to let the document policy install the AppleEvent handlers 
		it requires.
	*/
	void	RegisterAppleEvents(AEObjectSupport& ioObjectSupport);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the policy's document factory.
	FactoryType&	GetFactory();
	//@}
	
	//! @name AppleEvent Handlers
	//@{
	
	//! The application calls this to handle the Open Application ('oapp') event.
	void	OpenApplication();
	
	//! The application calls this to handle the Reopen Application ('rapp') event.
	void	ReopenApplication();
	
	//! The application calls this to handle the Quit Application ('quit') event.
	void	QuitApplication(
				//!< Hint as to what to do with unsaved documents.
				OSType			inSaveOption = kAEAsk);
	
	/*! @brief	The application calls this to handle the "make" ('crel') event for documents.
		
		@return	The newly created document.
	*/
	AEObject*
			OpenNewDocument(
				//!< The AEObject class of the document.
				DescType		inDocClass,
				//!< Initial properties for the document (optional).
				const AEDesc&	inProperties,
				//!< Initial intrinsic data for the document (optional).
				const AEDesc&	inData);
	//@}
	
//	// CarbonEvent handlers
//	bool	HandleCommand(
//				const HICommandExtended&	inHICommand);
//	bool	HandleUpdateStatus(
//				const HICommandExtended&	inHICommand, 
//				CommandData&				ioCmdData);
	
	//! @name Document Access
	//@{
	
	/*! @brief	Returns the number of documents of the given class.
	
		@throws	UnknownElementException	If the class ID is unknown to the container.
		
		@note	In general, the element count for a class A needs to include instances of 
				classes derived from A.
	*/
	SInt32		CountDocuments(
					//!< The base class ID of the elements to count;  must match the application's AppleScript dictionary.
					DescType		inDocClass) const;
	
	/*! @brief	Returns the document of the given class with the given index.
		
		@return	A valid AEObject.  In case of error an exception is thrown.
		@throws	UnknownElementException	If the class ID is unknown to the container.
		
		@note	In general, the "index-space" for an instance of class A includes instances 
				of classes derived from A.
	*/
	AEObject*	GetDocumentByIndex(
					//!< The base class ID of the elements to count;  must match the application's AppleScript dictionary.
					DescType		inDocClass, 
					//!< The element's zero-based index.
					unsigned		inIndex) const;
	
	/*! @brief	Returns the document of the given class with the given name.
		
		@return	A valid AEObject.  In case of error an exception is thrown.
		@throws	UnknownElementException	If the class ID is unknown to the container.
		
		@note	In general, the "name-space" for an instance of class A includes instances 
				of classes derived from A.
	*/
	AEObject*	GetDocumentByName(
					//!< The base class ID of the elements to count;  must match the application's AppleScript dictionary.
					DescType		inDocClass, 
					//!< The element's name.
					const String&	inName) const;
	
	/*! @brief	Returns the document of the given class with the given unique ID.
		
		@return	A valid AEObject.  In case of error an exception is thrown.
		@throws	UnknownElementException	If the class ID is unknown to the container.
		
		@note	In general, the "id-space" for an instance of class A includes instances of 
				classes derived from A.
	*/
	AEObject*	GetDocumentByUniqueID(
					//!< The base class ID of the elements to count;  must match the application's AppleScript dictionary.
					DescType		inDocClass, 
					//!< The element's unique id.
					SInt32			inUniqueID) const;
	//@}
};

#endif	// DOXYGEN_SCAN


#ifndef DOXYGEN_SKIP

template <class DOC_POLICY> struct DocumentPolicyConcept
{
	typedef typename DOC_POLICY::FactoryType	FactoryType;
	
	void	constraints()
	{
		boost::function_requires< DocumentFactoryConcept<FactoryType> >();
		
		DOC_POLICY		policy(mAEObject, mBundle, mNib);
		FactoryType&	factory	= policy.GetFactory();
		
		policy.Init();
		policy.RegisterAppleEvents(mObjectSupport);
		
		policy.OpenApplication();
		policy.ReopenApplication();
		policy.QuitApplication();
		policy.QuitApplication(mSaveOption);
		
		mFactory	= &factory;
		mAEObject	= policy.OpenNewDocument(mClass, mProperties, mData);
		mCount		= policy.CountDocuments(mClass);
		mAEObject	= policy.GetDocumentByIndex(mClass, mIndex);
		mAEObject	= policy.GetDocumentByName(mClass, mString);
		mAEObject	= policy.GetDocumentByUniqueID(mClass, mUniqueID);
	}
	
	AEObjectPtr			mAEObject;
	const Bundle&		mBundle;
	Nib*				mNib;
	FactoryType*		mFactory;
	AEObjectSupport&	mObjectSupport;
	const AEDesc&		mProperties;
	const AEDesc&		mData;
	String				mString;
	DescType			mClass;
	OSType				mSaveOption;
	SInt32				mCount;
	unsigned			mIndex;
	SInt32				mUniqueID;
};

#endif	// DOXYGEN_SKIP


}	// namespace B


#endif	// BDocumentPolicyConcept_H_
