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

#ifndef BAEObject_H_
#define BAEObject_H_

#pragma once

// system headers
#include <ApplicationServices/ApplicationServices.h>

// library headers
#include <boost/concept_check.hpp>
#include <boost/utility.hpp>

// B headers
#include "BAEDescriptor.h"
#include "BAEEvent.h"
#include "BAEInfo.h"
#include "BAEWriter.h"
#include "BString.h"


namespace B {

// forward declarations
class	AEObjectSupport;
class	AEReader;
class	AEToken;
class	AEWriter;


/*!
	@brief	The root class for objects visible to AppleScript.
	
	B's support for the Apple %Event Object Model (AEOM) entails deriving classes from 
	AEObject and implementing properties, elements and events for those classes in a way 
	that matches the application's AppleScript dictionary.
	
	Some important classes, such as B::Application, B::Document and B::Window, already 
	derive from AEObject and support the standard properties, elements, and events as 
	described in the Apple Event Registry.
	
	AEObject's main services are:
	
	- Property support.  Overridable functions are provided to allow the getting & setting 
	  of properties.
	- Element support.  Overridable functions are provided to allow the counting of elements, 
	  as well as retrieving individual elements.  Element retrieval may be done with a name, 
	  an index, or a unique id, matching the "by name", "by index", and "by id" key forms in 
	  AppleScript.
	- Standard event support.  Overridable functions are provided for the commonly used 
	  events in the Standard Suite, and some events in the Miscellaneous Standards Suite.
	- Functions for creating and sending any of the Standard suite events to a AEObject.
	- Functions for building up object specifier descriptors.
	
	@ingroup	AppleEvents
*/
class AEObject : public boost::noncopyable, public boost::enable_shared_from_this<AEObject>
{
public:

	//! @name Constructors / Destructor.
	//@{
	//!	Constructor.
	explicit			AEObject(
							AEObjectPtr	inContainer,
							DescType	inClassID);
	//!	Destructor.
	virtual				~AEObject();
	//@}
	
	//! @name Default Object.
	//@{
	//! Returns the default object.  This is usually the application.
	static AEObjectPtr	GetDefaultObject();
	
	//! Sets the default object.
	static void			SetDefaultObject(AEObjectPtr inObject);
	//@}
	
	//! @name Inquiries.
	//@{
	
	//! Returns a shared pointer to the given object.
	AEObjectPtr			GetAEObjectPtr()		{ return (shared_from_this()); }
	//! Returns a shared pointer to the given object (const variant).
	ConstAEObjectPtr	GetAEObjectPtr() const	{ return (shared_from_this()); }
	
	//! Returns the object's container.
	AEObjectPtr			GetContainer() const	{ return (mContainer.lock()); }
	
	//! Returns the object's classID.
	DescType			GetClassID() const		{ return (mClassID); }

	//! Returns true if the object's class is or derives from @a inBaseClassID.
	bool				InheritsFrom(
							DescType			inBaseClassID) const;
	
	//! Returns true if @a inClassID is or derives from @a inBaseClassID.
	static bool			DoesClassInheritFrom(
							DescType			inClassID, 
							DescType			inBaseClassID);
	//@}
	
	//! @name Modifiers.
	//@{
	//! Changes the object's container.
	void				SetContainer(AEObjectPtr inContainer);
	//@}
	
	//! @name Access Keys.
	//@{
	//! Returns the object's (zero-based) index within its container.
	virtual unsigned	GetIndex() const;

	//! Returns the object's name.
	virtual String		GetName() const;

	//! Returns the object's unique id.
	virtual SInt32		GetUniqueID() const;
	//@}
	
	//! @name Elements.
	//@{
	//! Returns the number of elements of the given class.
	virtual size_t		CountElements(
							DescType		inElementType) const;
	
	//! Returns the element of the given class with the given index.
	virtual AEObjectPtr	GetElementByIndex(
							DescType		inElementType, 
							size_t			inIndex) const;
	
	//! Returns the element of the given class with the given unique ID.
	virtual AEObjectPtr	GetElementByUniqueID(
							DescType		inElementType, 
							SInt32			inUniqueID) const;
	
	//! Returns the element of the given class with the given name.
	virtual AEObjectPtr	GetElementByName(
							DescType		inElementType, 
							const String&	inName) const;
	
	//! Returns all of the elements of the given class.
	virtual void		GetAllElements(
							DescType		inElementType, 
							std::list<AEObjectPtr>&	outElements) const;
	//@}
	
	//! @name Properties.
	//@{
	//! Returns the AEObject matching a given property, if any.
	virtual AEObjectPtr	GetPropertyObject(
							DescType		inPropertyID) const;
	
	//! Writes the value of the property identified by @a inPropertyID into @a ioWriter.
	virtual void		WriteProperty(
							DescType		inPropertyID, 
							AEWriter&		ioWriter) const;
	
	//! Reads the value of the property identified by @a inPropertyID from @a ioReader.
	virtual void		ReadProperty(
							DescType		inPropertyID, 
							AEReader&		ioReader);

	//! Writes the value of the property identified by @a inPropertyID into @a ioWriter.
	template <typename ITERATOR>
	void				GetPropertyIDs(
							boost::tribool	inInProperties,
							boost::tribool	inWriteable,
							ITERATOR		inIterator) const;

	//! Writes the value of the property identified by @a inPropertyID into @a ioWriter.
	template <typename ITERATOR>
	void				WriteProperties(
							ITERATOR		inFirstPropertyID,
							ITERATOR		inLastPropertyID,
							AEWriter&		ioWriter) const;
	//@}
	
	//! @name Object Resolution.
	//@{
	//! Access elements and properties, according to @a inKeyForm and @a inKeyData.
	virtual void		AccessElements(
							const AEInfo::ClassInfo&	inClassInfo,
							DescType					inDesiredClass, 
							DescType					inKeyForm, 
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access elements according to @c formAbsolutePosition and @a inKeyData.
	virtual void		AccessElementsByAbsolutePos(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEInfo::ElementInfo&	inElementInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access elements according to @c formName and @a inKeyData.
	virtual void		AccessElementsByName(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEInfo::ElementInfo&	inElementInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access elements according to @c formUniqueID and @a inKeyData.
	virtual void		AccessElementsByUniqueID(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEInfo::ElementInfo&	inElementInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access elements according to @c formRange and @a inKeyData.
	virtual void		AccessElementsByRange(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEInfo::ElementInfo&	inElementInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access elements according to @c formRelativePosition and @a inKeyData.
	virtual void		AccessElementsByRelativePosition(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEInfo::ElementInfo&	inElementInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access elements according to @c formTest and @a inKeyData.
	virtual void		AccessElementsByTest(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEInfo::ElementInfo&	inElementInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	
	//! Access properties according to @c formPropertyID and @a inKeyData.
	virtual void		AccessProperty(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	//! Access properties according to @c formPropertyID and @a inPropertyID.
	virtual void		AccessPropertyByID(
							const AEInfo::ClassInfo&	inClassInfo,
							DescType					inPropertyID,
							AEDesc&						outTokenDesc) const;
	
	//! Access properties according to @c formUserPropertyID and @a inKeyData.
	virtual void		AccessUserProperty(
							const AEInfo::ClassInfo&	inClassInfo,
							const AEDesc&				inKeyData,
							AEDesc&						outTokenDesc) const;
	//@}
	
	/*! @name Standard Events.
		
		Derived classes wishing to implement some of the Standard Suite AppleEvents 
		supported directly by AEObject need only override the appropriate function below.
	*/
	//@{
	//! Implements the AppleScript "duplicate" event.
	virtual AEObjectPtr	CloneObject(
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							const AERecord&	inProperties);
	
	//! Implements the AppleScript "close" event.
	virtual void		CloseObject(
							OSType			inSaveOption = kAEAsk,
							const Url&		inUrl = Url());
	
	//! Implements the AppleScript "make" event.
	virtual AEObjectPtr	CreateObject(
							DescType		inObjectClass,
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							const AERecord&	inProperties,
							const AEDesc&	inData);
	
	//! Implements the AppleScript "delete" event.
	virtual void		DeleteObject();
	
	//! Implements the AppleScript "move" event.
	virtual AEObjectPtr	MoveObject(
							DescType		inPosition,
							AEObjectPtr		inTarget);
	
	//! Implements the AppleScript "open" event.
	virtual void		OpenObject();
	
	//! Implements the AppleScript "print" event.
	virtual void		PrintObject(
							PrintSettings*	inSettings, 
							Printer*		inPrinter, 
							bool			inShowDialog);
	
	//! Implements the AppleScript "save" event.
	virtual void		SaveObject(
							const Url&		inUrl = Url(),
							const String&	inObjectType = String());
	
	//! Implements the AppleScript "revert" event.
	virtual void		RevertObject();
	
	//! Implements the AppleScript "activate" event.
	virtual void		ActivateObject();
	//@}
	
	/*! @name Sending AppleEvents.
		
		These functions offer a simple way of sending one of the standard AppleEvents to the 
		object.
	*/
	//@{
	//! Sends a "duplicate" event to the object.
	AEObjectPtr			SendCloneAppleEvent(
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							const AERecord*	inProperties = NULL, 
							AESendMode		inMode = 0);
	
	//! Sends a "close" event to the object.
	void				SendCloseAppleEvent(
							OSType			inSaveOption = kAEAsk, 
							const Url&		inUrl = Url(),
							AESendMode		inMode = 0);
	
	//! Sends a "close" event to the object.
	SInt32				SendCountElementsAppleEvent(
							OSType			inObjectClass,
							AESendMode		inMode = 0) const;
	
	//! Sends a "make" event to the object.
	static AEObjectPtr	SendCreateElementAppleEvent(
							DescType		inObjectClass,
							DescType		inPosition,
							AEObjectPtr		inTarget,
							const AERecord*	inProperties = NULL, 
							const AEDesc*	inData = NULL, 
							AESendMode		inMode = 0);
	
	//! Sends a "delete" event to the object.
	void				SendDeleteAppleEvent(
							AESendMode		inMode = 0);
	
	// Note: we don't supply an "exists" event, because if we can target an 
	// object, then it exists by definition!
	
	//! Sends a "get" event to retrieve the object's data.
	void				SendGetDataAppleEvent(
							AEDesc&			outValue, 
							DescType		inRequestedType = typeWildCard,
							AESendMode		inMode = 0) const;
	
	//! Sends a "get" event to retrieve the object's data.
	template <DescType TYPE>
	void				SendGetDataAppleEvent(
							typename DescParam<TYPE>::ValueType& outValue, 
							DescType		inRequestedType = typeWildCard,
							AESendMode		inMode = 0) const;
	
	//! Sends a "get" event to retrieve one of the object's properties.
	void				SendGetPropertyAppleEvent(
							DescType		inPropertyID,
							AEDesc&			outValue, 
							DescType		inRequestedType = typeWildCard,
							AESendMode		inMode = 0) const;
	
	//! Sends a "get" event to retrieve one of the object's properties.
	template <DescType TYPE>
	void				SendGetPropertyAppleEvent(
							DescType		inPropertyID,
							typename DescParam<TYPE>::ValueType& outValue, 
							DescType		inRequestedType = typeWildCard,
							AESendMode		inMode = 0) const;
	
	//! Sends a "move" event to the object.
	AEObjectPtr			SendMoveAppleEvent(
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							AESendMode		inMode = 0);
	
	//! Sends a "open" event to the object.
	void				SendOpenAppleEvent(
							AESendMode		inMode = 0);
	
	//! Sends a "print" event to the object.
	void				SendPrintAppleEvent(
							bool			inShowDialog = false, 
							const AERecord*	inProperties = NULL, 
							AESendMode		inMode = 0);
	
	//! Sends a "save" event to the object.
	void				SendSaveAppleEvent(
							const Url&		inUrl = Url(),
							const String&	inObjectType = String(), 
							AESendMode		inMode = 0);
	
	//! Sends a "revert" event to the object.
	void				SendRevertAppleEvent(
							AESendMode		inMode = 0);
	
	//! Sends a "select" event to the object.
	void				SendSelectAppleEvent(
							AESendMode		inMode = 0);
	
	//! Sends "set" event to change the object's data.
	void				SendSetDataAppleEvent(
							const AEDesc&	inValue, 
							AESendMode		inMode = 0);
	
	//! Sends "set" event to change the object's data.
	template <DescType TYPE>
	void				SendSetDataAppleEvent(
							const typename DescParam<TYPE>::ValueType& inValue, 
							AESendMode		inMode = 0);
	
	//! Sends "set" event to change one of the object's properties.
	void				SendSetPropertyAppleEvent(
							DescType		inPropertyID,
							const AEDesc&	inValue, 
							AESendMode		inMode = 0);
	
	//! Sends "set" event to change one of the object's properties.
	template <DescType TYPE>
	void				SendSetPropertyAppleEvent(
							DescType		inPropertyID,
							const typename DescParam<TYPE>::ValueType& inValue, 
							AESendMode		inMode = 0);
	//@}
	
	/*! @name Making AppleEvents.
		
		These functions offer a simple way of creating one of the standard AppleEvents 
		(targeting the object).  An event created in this way can be sent later on.
	*/
	//@{
	//! Creates a "duplicate" event targeting the object.
	void				MakeCloneAppleEvent(
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							const AERecord*	inProperties, 
							AppleEvent&		outEvent) const;

	//! Creates a "close" event targeting the object.
	void				MakeCloseAppleEvent(
							OSType			inSaveOption, 
							const Url&		inUrl,
							AppleEvent&		outEvent) const;
	
	//! Creates a "count" event targeting the object.
	void				MakeCountElementsAppleEvent(
							OSType			inObjectClass, 
							AppleEvent&		outEvent) const;

	//! Creates a "create" event targeting the object.
	static void			MakeCreateElementAppleEvent(
							DescType		inObjectClass,
							DescType		inPosition,
							AEObjectPtr		inTarget,
							const AERecord*	inProperties, 
							const AEDesc*	inData, 
							AppleEvent&		outEvent);

	//! Creates a "delete" event targeting the object.
	void				MakeDeleteAppleEvent(
							AppleEvent&		outEvent) const;
	
	//! Creates a "get" event targeting the object.
	void				MakeGetDataAppleEvent(
							DescType		inRequestedType, 
							AppleEvent&		outEvent) const;
	
	//! Creates a "get" event targeting one of the object's properties.
	void				MakeGetPropertyAppleEvent(
							DescType		inPropertyID, 
							DescType		inRequestedType, 
							AppleEvent&		outEvent) const;
	
	//! Creates a "move" event targeting the object.
	void				MakeMoveAppleEvent(
							DescType		inPosition,
							AEObjectPtr		inTarget, 
							AppleEvent&		outEvent) const;

	//! Creates a "open" event targeting the object.
	void				MakeOpenAppleEvent(
							AppleEvent&		outEvent) const;

	//! Creates a "print" event targeting the object.
	void				MakePrintAppleEvent(
							bool			inShowDialog, 
							const AERecord*	inProperties, 
							AppleEvent&		outEvent) const;

	//! Creates a "revert" event targeting the object.
	void				MakeRevertAppleEvent(
							AppleEvent&		outEvent) const;

	//! Creates a "save" event targeting the object.
	void				MakeSaveAppleEvent(
							const Url&		inUrl,
							const String&	inObjectType, 
							AppleEvent&		outEvent) const;

	//! Creates a "select" event targeting the object.
	void				MakeSelectAppleEvent(
							AppleEvent&		outEvent) const;
	
	//! Creates a "set" event targeting the object.
	void				MakeSetDataAppleEvent(
							const AEDesc&	inValue, 
							AppleEvent&		outEvent) const;
	
	//! Creates a "set" event targeting the object.
	template <DescType TYPE>
	void				MakeSetDataAppleEvent(
							const typename DescParam<TYPE>::ValueType& inValue, 
							AppleEvent&		outEvent) const;
	
	//! Creates a "set" event targeting one of the object's properties.
	void				MakeSetPropertyAppleEvent(
							DescType		inPropertyID,
							const AEDesc&	inValue, 
							AppleEvent&		outEvent) const;
	
	//! Creates a "set" event targeting one of the object's properties.
	template <DescType TYPE>
	void				MakeSetPropertyAppleEvent(
							DescType		inPropertyID,
							const typename DescParam<TYPE>::ValueType& inValue, 
							AppleEvent&		outEvent) const;
	//@}
	
	/*! @name Object Specifiers.
		
		These functions allow conversions between AEObjects and object specifiers.
	*/
	//@{
	//! Writes out an object-specifier representation of the object.
	virtual void		MakeSpecifier(
							AEWriter&		ioWriter) const = 0;
	
	//! Writes out an object specifier representation of one of the object's properties.
	virtual void		MakePropertySpecifier(
							DescType		inPropertyID, 
							AEWriter&		ioWriter) const;
	
	//! Resolves an object specifier into a AEObject.
	static AEObjectPtr	Resolve(
							const AEDesc&	inDesc);
	
	//! Resolves an object specifier into a AEObject (nothrow variant).
	static AEObjectPtr	Resolve(
							const AEDesc&	inDesc, 
							const std::nothrow_t&);
	//@}
	
	//! @name Comparisons.
	//@{
	
	//! Returns true if the object compares to @a inObject, according to @a inComparisonOp.
	virtual bool	Compare(
						DescType			inComparisonOp, 
						ConstAEObjectPtr	inObject) const;
	
	//! Compare two objects according to @c inComparisonOp.
	static bool		CompareObjects(
						DescType			inComparisonOp, 
						ConstAEObjectPtr	inObject1, 
						ConstAEObjectPtr	inObject2);
	//@}
	
	//! @name Utilities.
	//@{
	//! Write out an object specifier for an element of a given class and name in the given container.
	static void		BuildNameSpecifier(
						ConstAEObjectPtr inContainer,
						DescType		inObjectClass,
						const String&	inName, 
						AEWriter&		ioWriter);

	//! Write out an object specifier for an element of a given class and index in the given container.
	static void		BuildIndexSpecifier(
						ConstAEObjectPtr inContainer,
						DescType		inObjectClass,
						SInt32			inIndex, 
						AEWriter&		ioWriter);

	//! Write out an object specifier for an element of a given class and unique id in the given container.
	static void		BuildUniqueIDSpecifier(
						ConstAEObjectPtr inContainer,
						DescType		inObjectClass,
						SInt32			inUniqueID, 
						AEWriter&		ioWriter);

	//! Write out an object specifier for the given property in the given object.
	static void		BuildPropertySpecifier(
						ConstAEObjectPtr inObject,
						DescType		inPropertyID, 
						AEWriter&		ioWriter);
	//@}
    
	//! @name Object values.
	//@{
	//! Writes the value of the object into @a ioWriter.
	virtual void	WriteValue(
						AEWriter&			ioWriter) const;
	//@}
    
	//! @name Tokens.
	//@{
	static void		WriteTokenValue(
						const AEToken&		inToken, 
						DescType			inDesiredType,
						AEWriter&			ioValueWriter);
	static void		WriteTokenValue(
						const AEToken&		inToken, 
						AEWriter&			ioValueWriter);
	static void		WriteTokenSpecifier(
						const AEToken&		inToken, 
						AEWriter&			ioValueWriter);
	//@}
    
	//! @name Apple %Event handler registration.
	//@{
	static void		RegisterClassEventHandlers(
						AEObjectSupport&	ioObjectSupport);
	//@}

	// Apple %Event Creation Functors
	class	ObjectWriter;
	template <class INPUT_ITERATOR>
	class	ObjectListSpecifier;
	class	InsertionLocationSpecifier;
	class	NameSpecifier;
	class	IndexSpecifier;
	class	UniqueIDSpecifier;
	class	PropertySpecifier;
	
	template <class CONTAINER>
		ObjectListSpecifier<typename CONTAINER::iterator>
		MakeObjectListSpecifier(CONTAINER& container);
	
	template <class CONTAINER>
		ObjectListSpecifier<typename CONTAINER::const_iterator>
		MakeObjectListSpecifier(const CONTAINER& container);
	
protected:

	//! Writes the @c pProperties property into @a ioWriter.
	void		WritePropertiesProperty(
					AEWriter&		ioWriter,
					bool			inWriteableOnly) const;
	static bool	TestProperty(
					const AEInfo::PropertyInfo&	inPropertyInfo,
					boost::tribool				inInProperties,
					boost::tribool				inWriteable);
	static void	WriteOneProperty(
					AEObjectPtr					inObject, 
					AEWriter&					ioWriter, 
					DescType					inPropertyID);
	//! Reads the @c pProperties property from @a ioReader.
	void		ReadPropertiesProperty(
					AEReader&		ioReader,
					bool			inIgnoreReadOnly);
	
	//! Reads the key data for @c formAbsolutePosition.
	void		ConvertIndexedKeyData(
					const AEDesc&	inKeyData,
					size_t			inNumElements,
					size_t&			outIndex,
					bool&			outWantsAll) const;
	//! Reads a boundary object for @c formRange.
	AEObjectPtr	ResolveBoundsToken(
					const AEDesc&	inKeyData,
					AEKeyword		inKeyword) const;
	
	const AEInfo::ClassInfo&	GetClassInfo() const;
	
private:
	
	template <AEEventClass EVT_CLASS, AEEventID EVT_ID>
	static void	SetClassEventHandler(
					AEObjectSupport&	ioObjectSupport);
	
	template <AEEventClass EVT_CLASS, AEEventID EVT_ID>
	static void	DispatchEvent(
					const AEToken&		inDirectObjectToken,
					const AppleEvent&	inEvent,
					AEWriter&			ioResultWriter);

	// member variables
	boost::weak_ptr<AEObject>	mContainer;	//!< The object's container in the AppleScript-visible hierarchy.
	DescType					mClassID;	//!< The object's class ID.
	
	// static member variables
	static boost::weak_ptr<AEObject>	sDefaultObject;
};


//	Inline member function definitions

// ------------------------------------------------------------------------------------------
/*!	The default object is used in a number of different ways.  First of all, it is used when 
	attempting to resolve an object specifier denoting the null container.  It's also used when 
	coercing tokens (again during object resolution) of class cApplication.  Finally, it's used 
	as the target of AppleEvents in the absence of a direct parameter.
*/
inline AEObjectPtr
AEObject::GetDefaultObject()
{
	return (sDefaultObject.lock());
}

// ------------------------------------------------------------------------------------------
/*!	Since a AEObject doesn't hold pointers to its elements, changing an AEObject's 
	container is as simple as changing the value of the @a mContainer member variable.  However, 
	if a derived class @b does hold pointers to its elements, it will need to add logic so those 
	pointers are added and removed as appropriate.
*/
inline void
AEObject::SetContainer(
	AEObjectPtr	inContainer)	//!< The object's new container.
{
	mContainer = inContainer;
}


//	Template member functions

// ------------------------------------------------------------------------------------------
template <typename ITERATOR> void
AEObject::GetPropertyIDs(
	boost::tribool	inInProperties,
	boost::tribool	inWriteable,
	ITERATOR		inIterator)
	const
{
	const AEInfo::ClassInfo&			classInfo	= GetClassInfo();
	AEInfo::PropertyMap::const_iterator	endIt		= classInfo.mProperties.end();
	
	for (AEInfo::PropertyMap::const_iterator it = classInfo.mProperties.begin();
		 it != endIt;
		 ++it)
	{
		if (TestProperty(it->second, inInProperties, inWriteable))
		{
			*inIterator = it->first;
			++inIterator;
		}
	}
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR> void
AEObject::WriteProperties(
	ITERATOR		inFirstPropertyID,
	ITERATOR		inLastPropertyID,
	AEWriter&		ioWriter)			//!< The Apple %Event descriptor stream to fill with the property's value.
	const
{
	AutoAEWriterRecord	autoRecord(ioWriter);
	
	std::for_each(inFirstPropertyID, inLastPropertyID, 
				  boost::bind(WriteOneProperty, 
							  boost::const_pointer_cast<AEObject>(GetAEObjectPtr()), 
							  boost::ref(ioWriter), _1));
}

// ------------------------------------------------------------------------------------------
/*!	The C/C++ type of @a outValue (i.e. the property's new value) is determined by the TYPE 
	template parameter, via an instantiation of the DescParam class template.
	
	@param	TYPE	Template parameter.  Should be a four-char constant in the @c DescType 
					namespace.
*/
template <DescType TYPE> void
AEObject::SendGetDataAppleEvent(
	typename DescParam<TYPE>::ValueType& outValue,			//!< The output value.
	DescType		inRequestedType /* = typeWildCard */,	//!< The requested type of the data.
	AESendMode		inMode /* = 0 */)						//!< The mode for AESend().
 	const
 {
	AEEvent<kAECoreSuite, kAEGetData>::Send(
		AESelfTarget(),
		*this, 
		AEEventFunctor::OptionalRequestedType(inRequestedType), 
		AEEventFunctor::DescParamResult<TYPE>(outValue), 
		inMode);
}

// ------------------------------------------------------------------------------------------
/*!	The C/C++ type of @a outValue (i.e. the property's new value) is determined by the TYPE 
	template parameter, via an instantiation of the DescParam class template.
	
	@param	TYPE	Template parameter.  Should be a four-char constant in the @c DescType 
					namespace.
*/
template <DescType TYPE> void
AEObject::SendGetPropertyAppleEvent(
	DescType		inPropertyID,							//!< The property ID;  must match the application's AppleScript dictionary.
	typename DescParam<TYPE>::ValueType& outValue,			//!< The output value.
	DescType		inRequestedType /* = typeWildCard */,	//!< The requested type of the data.
	AESendMode		inMode /* = 0 */)						//!< The mode for AESend().
 	const
{
	AEEvent<kAECoreSuite, kAEGetData>::Send(
		AESelfTarget(),
		PropertySpecifier(this, inPropertyID), 
		AEEventFunctor::OptionalRequestedType(inRequestedType), 
		AEEventFunctor::DescParamResult<TYPE>(outValue), 
		inMode);
}

// ------------------------------------------------------------------------------------------
/*!	The C/C++ type of @a inValue (i.e. the property's new value) is determined by the TYPE 
	template parameter, via an instantiation of the DescParam class template.
	
	@param	TYPE	Template parameter.  Should be a four-char constant in the @c DescType 
					namespace.
*/
template <DescType TYPE> void
AEObject::SendSetDataAppleEvent(
	const typename DescParam<TYPE>::ValueType& inValue, //!< The property's new value.
	AESendMode		inMode /* = 0 */)					//!< The mode for AESend().
{
	AEDescriptor	event;
	
	MakeSetDataAppleEvent<TYPE>(AESelfTarget(), inValue, event);
	
	AEEventBase::SendEvent(event, inMode);
}

// ------------------------------------------------------------------------------------------
/*!	The C/C++ type of @a inValue (i.e. the property's new value) is determined by the TYPE 
	template parameter, via an instantiation of the DescParam class template.
	
	@param	TYPE	Template parameter.  Should be a four-char constant in the @c DescType 
					namespace.
*/
template <DescType TYPE> void
AEObject::SendSetPropertyAppleEvent(
	DescType		inPropertyID,						//!< The property ID;  must match the application's AppleScript dictionary.
	const typename DescParam<TYPE>::ValueType& inValue, //!< The property's new value.
	AESendMode		inMode /* = 0 */)					//!< The mode for AESend().
{
	AEDescriptor	event;
	
	MakeSetPropertyAppleEvent<TYPE>(inPropertyID, inValue, event);
	
	AEEventBase::SendEvent(event, inMode);
}

// ------------------------------------------------------------------------------------------
/*!	The C/C++ type of @a inValue (i.e. the property's new value) is determined by the TYPE 
	template parameter, via an instantiation of the DescParam class template.
	
	@param	TYPE	Template parameter.  Should be a four-char constant in the @c DescType 
					namespace.
*/
template <DescType TYPE> void
AEObject::MakeSetDataAppleEvent(
	const typename DescParam<TYPE>::ValueType& inValue, //!< The property's new value.
	AppleEvent&		outEvent)							//!< The output Apple %Event.
	const
{
	AEEvent<kAECoreSuite, kAESetData>::Make(
		AESelfTarget(),
		*this, 
		AETypedObject<TYPE>(inValue), 
		outEvent);
}

// ------------------------------------------------------------------------------------------
/*!	The C/C++ type of @a inValue (i.e. the property's new value) is determined by the TYPE 
	template parameter, via an instantiation of the DescParam class template.
	
	@param	TYPE	Template parameter.  Should be a four-char constant in the @c DescType 
					namespace.
*/
template <DescType TYPE> void
AEObject::MakeSetPropertyAppleEvent(
	DescType		inPropertyID,						//!< The property ID;  must match the application's AppleScript dictionary.
	const typename DescParam<TYPE>::ValueType& inValue, //!< The property's new value.
	AppleEvent&		outEvent)							//!< The output Apple %Event.
	const
{
	AEEvent<kAECoreSuite, kAESetData>::Make(
		AESelfTarget(),
		PropertySpecifier(GetAEObjectPtr(), inPropertyID), 
		AETypedObject<TYPE>(inValue), 
		outEvent);
}


// ------------------------------------------------------------------------------------------
/*!	Writes out the AEObject @a obj to the AEWriter @a writer.
	
	@relates	AEObject
*/
inline AEWriter&
operator << (AEWriter& writer, const AEObject& obj)
{
	return (writer.WriteObject(obj));
}

// ------------------------------------------------------------------------------------------
/*!	Writes out the AEObject pointed to by @a obj to the AEWriter @a writer.
	
	@note		If @a obj is @c NULL, the results are undefined.
	@relates	AEObject
*/
inline AEWriter&
operator << (AEWriter& writer, const AEObject* obj)
{
	return (writer.WriteObject(*obj));
}

// ------------------------------------------------------------------------------------------
/*!	Writes out the AEObject pointed to by @a ptr to the AEWriter @a writer.
	
	@note		If @a ptr is @c NULL, the results are undefined.
	@relates	AEObject
*/
inline AEWriter&
operator << (AEWriter& writer, ConstAEObjectPtr ptr)
{
	return (writer.WriteObject(*ptr));
}


// ==========================================================================================
//	AEObject::ObjectWriter

#pragma mark -
#pragma mark AEObject::ObjectWriter

/*!
	@brief	Helper class that writes out an AEObject to an AEWriter.
	
	This functor object holds an AEWriter, which is given to it in its constructor.  
	When the functor is invoked, the AEObject passed in as an argument is written out 
	to the AEWriter.
	
	@ingroup	AppleEvents
*/
class AEObject::ObjectWriter : public std::unary_function<AEObjectPtr, void>
{
public:
	
	//! Constructor.
	ObjectWriter(AEWriter& writer)
		: mWriter(writer)  {}
	
	//! Appends @a ptr to our AEWriter.
	void	operator () (const AEObjectPtr ptr) const
				{ mWriter << *ptr; }

private:
	
	// member variables
	AEWriter&	mWriter;
};


// ==========================================================================================
//	AEObject::ObjectListSpecifier

#pragma mark -
#pragma mark AEObject::ObjectListSpecifier

/*!
	@brief	Writes out a list of object specifiers to an AEWriter.
	
	The objects are given in the form of a pair of input iterators.  The iterators are 
	assumed to point to data of type <tt>AEObject*</tt> or <tt>const AEObject*</tt>.  
	That is, dereferencing the iterators must yield one of the two types.
	
	@ingroup	AppleEvents
*/
template <class INPUT_ITERATOR>
class AEObject::ObjectListSpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
	
#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef typename	INPUT_ITERATOR;	//!< Template Parameter.  An input iterator yielding an <tt>AEObject*</tt> or a <tt>const AEObject*</tt>.
	//@}
#endif
	
	// Concept checks.
	BOOST_CLASS_REQUIRE(INPUT_ITERATOR, boost, InputIteratorConcept);

	//! Constructor.
	ObjectListSpecifier(INPUT_ITERATOR first, INPUT_ITERATOR last);
	
	//! Appends a list containing each element of the sequence [mFirst, mLast) to @a writer.
	AEWriter&	operator () (AEWriter& writer) const;
	
private:
	
	// member variables
	INPUT_ITERATOR	mFirst;
	INPUT_ITERATOR	mLast;
};

// ------------------------------------------------------------------------------------------
template <class INPUT_ITERATOR>
AEObject::ObjectListSpecifier<INPUT_ITERATOR>::ObjectListSpecifier(
	INPUT_ITERATOR	first,	//!< The start of the sequence of AEObjects to append to the list.
	INPUT_ITERATOR	last)	//!< The end of the sequence of AEObjects to append to the list.
		: mFirst(first), mLast(last)
{
}

// ------------------------------------------------------------------------------------------
template <class INPUT_ITERATOR> AEWriter&
AEObject::ObjectListSpecifier<INPUT_ITERATOR>::operator () (AEWriter& writer) const
{
	// Open a list descriptor...
	
	AutoAEWriterList	itemList(writer);
	
	// ... then add each element of the sequence to the list.
	
	std::for_each(mFirst, mLast, AEObject::ObjectWriter(writer));
	
	return (writer);
}


// ------------------------------------------------------------------------------------------
/*!	Writes out the InsertionLocation @a value to the AEWriter @a writer.
	
	@relates	AEObject::ObjectListSpecifier
*/
template <class INPUT_ITERATOR> inline AEWriter&
operator << (AEWriter& writer, AEObject::ObjectListSpecifier<INPUT_ITERATOR> value)
{
	return (value(writer));
}

// ------------------------------------------------------------------------------------------
/*!	Helper function for instantiating AEObject::ObjectListSpecifier.
	
	@relates	AEObject::ObjectListSpecifier
*/
template <class CONTAINER>
AEObject::ObjectListSpecifier<typename CONTAINER::iterator>
AEObject::MakeObjectListSpecifier(CONTAINER& container)
{
	return AEObject::ObjectListSpecifier<typename CONTAINER::iterator>(
				container.begin(), container.end());
}

// ------------------------------------------------------------------------------------------
/*!	Helper function for instantiating AEObject::ObjectListSpecifier.
	
	@relates	AEObject::ObjectListSpecifier
*/
template <class CONTAINER>
AEObject::ObjectListSpecifier<typename CONTAINER::const_iterator>
AEObject::MakeObjectListSpecifier(const CONTAINER& container)
{
	return AEObject::ObjectListSpecifier<typename CONTAINER::const_iterator>(
				container.begin(), container.end());
}


// ==========================================================================================
//	AEObject::InsertionLocationSpecifier

#pragma mark -
#pragma mark AEObject::InsertionLocationSpecifier

/*!	@brief	Convenience wrapper around AEEventFunctor::InsertionLocation<...>.
	
	This class' main use is as a template parameter to instantiations of AEEvent.
	
	@ingroup	AppleEvents
*/
class AEObject::InsertionLocationSpecifier : 
		public AEEventFunctor::InsertionLocation< 
					const AEObject&, 
					AETypedObject<typeEnumeration> >
{
public:
	
	//! Constructor.
	InsertionLocationSpecifier(OSType pos, ConstAEObjectPtr obj);
};


// ==========================================================================================
//	AEObject::IndexSpecifier

#pragma mark -
#pragma mark AEObject::IndexSpecifier

/*!	@brief	Convenience wrapper around AEEventFunctor::IndexSpecifier<...>.
	
	This class' main use is as a template parameter to instantiations of AEEvent.
	
	@ingroup	AppleEvents
*/
class AEObject::IndexSpecifier : 
		public AEEventFunctor::IndexSpecifier< 
					const AEObject&, 
					AETypedObject<typeType>, 
					AETypedObject<typeSInt32> >
{
public:
	
	//! Constructor.
	IndexSpecifier(DescType classID, ConstAEObjectPtr obj, SInt32 index);
};


// ==========================================================================================
//	AEObject::NameSpecifier

#pragma mark -
#pragma mark AEObject::NameSpecifier

/*!	@brief	Convenience wrapper around AEEventFunctor::NameSpecifier<...>.
	
	This class' main use is as a template parameter to instantiations of AEEvent.
	
	@ingroup	AppleEvents
*/
class AEObject::NameSpecifier : 
		public AEEventFunctor::NameSpecifier< 
					const AEObject&, 
					AETypedObject<typeType>, 
					const String& >
{
public:
	
	//! Constructor.
	NameSpecifier(DescType classID, ConstAEObjectPtr obj, const String& name);
};


// ==========================================================================================
//	AEObject::UniqueIDSpecifier

#pragma mark -
#pragma mark AEObject::UniqueIDSpecifier

/*!	@brief	Convenience wrapper around AEEventFunctor::UniqueIDSpecifier<...>.
	
	This class' main use is as a template parameter to instantiations of AEEvent.
	
	@ingroup	AppleEvents
*/
class AEObject::UniqueIDSpecifier : 
		public AEEventFunctor::UniqueIDSpecifier< 
					const AEObject&, 
					AETypedObject<typeType>, 
					AETypedObject<typeSInt32> >
{
public:
	
	//! Constructor.
	UniqueIDSpecifier(DescType classID, ConstAEObjectPtr obj, SInt32 inUniqueID);
};


// ==========================================================================================
//	AEObject::PropertySpecifier

#pragma mark -
#pragma mark AEObject::PropertySpecifier

/*!	@brief	Convenience wrapper around AEEventBase::PropertySpecifier<...>.
	
	This class' main use is as a template parameter to instantiations of AEEvent.
	
	@ingroup	AppleEvents
*/
class AEObject::PropertySpecifier : 
		public AEEventFunctor::PropertySpecifier< 
					const AEObject&, 
					AETypedObject<typeType> >
{
public:
	
	//! Constructor.
	PropertySpecifier(ConstAEObjectPtr obj, DescType propertyID);
};


}	// namespace B


#endif	// BAEObject_H_
