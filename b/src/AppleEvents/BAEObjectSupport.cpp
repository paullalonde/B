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

// file header
#include "BAEObjectSupport.h"

// standard headers
#include <sstream>
#include <typeinfo>

// library headers
#include <openssl/bio.h>
#include <openssl/evp.h>

// B headers
#include "BAEEvent.h"
#include "BAEEventHook.h"
#include "BAEObject.h"
#include "BAEReader.h"
#include "BAESDefReader.h"
#include "BAEToken.h"
#include "BAEWriter.h"
#include "BBundle.h"
#include "BEventCustomParams.h"
#include "BExceptionStreamer.h"
#include "BMemoryUtilities.h"
#include "BStringUtilities.h"


namespace B {


// ==========================================================================================
//	AEObjectSupport::ErrorDescLink

#pragma mark AEObjectSupport::ErrorDescLink

class AEObjectSupport::ErrorDescLink : public boost::noncopyable
{
public:
	
	explicit	ErrorDescLink(const AEObjectSupport& inObjSupport);
				~ErrorDescLink();
				
	AEDesc*	GetErrorDesc()	{ return mErrorDesc; }
	void	SwapErrorDesc(AEDesc& outErrorDesc);
	
private:
	
	const AEObjectSupport&	mObjSupport;
	ErrorDescLink* const	mNext;
	AEDescriptor			mErrorDesc;
};

// ------------------------------------------------------------------------------------------
inline
AEObjectSupport::ErrorDescLink::ErrorDescLink(const AEObjectSupport& inObjSupport)
	: mObjSupport(inObjSupport), mNext(mObjSupport.SetErrorDescLink(this))
{
}

// ------------------------------------------------------------------------------------------
inline
AEObjectSupport::ErrorDescLink::~ErrorDescLink()
{
	mObjSupport.SetErrorDescLink(mNext);
}

// ------------------------------------------------------------------------------------------
inline void
AEObjectSupport::ErrorDescLink::SwapErrorDesc(AEDesc& outErrDesc)
{
	std::swap(outErrDesc, static_cast<AEDesc&>(mErrorDesc));
}


// ==========================================================================================
//	AEObjectSupport::EventHookLink

#pragma mark AEObjectSupport::EventHookLink

class AEObjectSupport::EventHookLink : public boost::noncopyable
{
public:
	
	explicit	EventHookLink(
					const AEObjectSupport&	inObjSupport, 
					const AppleEvent&		inEvent,
					AppleEvent*				ioReply) throw();
				~EventHookLink() throw();
	
	void	SetEventHook(int key, std::auto_ptr<AEEventHook> eventHook);
	void	Commit();
	
private:
	
	typedef std::pair<int, AEEventHook*>	HookListElem;
	typedef std::list<HookListElem>			HookList;
	
	struct Committer : std::unary_function<HookListElem, void>
	{
		void operator () (const HookListElem& elem) const
		{
			elem.second->Commit();
		}
	};
	
	struct Aborter : std::unary_function<HookListElem, void>
	{
		void operator () (const HookListElem& elem) const
		{
			elem.second->Abort();
		}
	};
	
	struct Locator : std::unary_function<HookListElem, bool>
	{
		Locator(int key)
			: mKey(key)  {}
		
		bool operator () (const HookListElem& elem) const
		{
			return (elem.first == mKey);
		}
		
		const int mKey;
	};
	
	struct Deleter : std::unary_function<HookListElem, void>
	{
		void operator () (const HookListElem& elem) const
		{
			delete elem.second;
		}
	};
	
	const AEObjectSupport&	mObjSupport;
	const AppleEvent&		mEvent;
	AppleEvent* const		mReply;
	EventHookLink* const	mNext;
	HookList				mHooks;
	bool					mCommitted;
};

// ------------------------------------------------------------------------------------------
inline
AEObjectSupport::EventHookLink::EventHookLink(
	const AEObjectSupport&	inObjSupport, 
	const AppleEvent&		inEvent,
	AppleEvent*				ioReply) throw()
		: mObjSupport(inObjSupport), mEvent(inEvent), mReply(ioReply), 
		  mNext(mObjSupport.SetEventHookLink(this)), mCommitted(false)
{
}

// ------------------------------------------------------------------------------------------
inline
AEObjectSupport::EventHookLink::~EventHookLink() throw()
{
	if (!mCommitted)
	{
		try
		{
			std::for_each(mHooks.begin(), mHooks.end(), Aborter());
		}
		catch (...)
		{
			// It's very bad to throw from a destructor, so just catch any exceptions here.
		}
	}
	
	std::for_each(mHooks.begin(), mHooks.end(), Deleter());
	mObjSupport.SetEventHookLink(mNext);
}

// ------------------------------------------------------------------------------------------
inline void
AEObjectSupport::EventHookLink::SetEventHook(int key, std::auto_ptr<AEEventHook> eventHook)
{
	if (mCommitted)
		throw std::runtime_error("event hook already committed");
	
	HookList::iterator it = std::find_if(mHooks.begin(), mHooks.end(), Locator(key));
	
	if (it != mHooks.end())
	{
		eventHook->Start();
		mHooks.push_back(HookListElem(key, eventHook.release()));
	}
}

// ------------------------------------------------------------------------------------------
inline void
AEObjectSupport::EventHookLink::Commit()
{
	if (!mCommitted)
	{
		mCommitted = true;
		std::for_each(mHooks.begin(), mHooks.end(), Committer());
	}
}


// ==========================================================================================
//	AEObjectSupport::ExInfo

#pragma mark AEObjectSupport::ExInfo

struct AEObjectSupport::ExInfo : public boost::noncopyable
{
	ExInfo();
	
	bool				mValid;
	OSStatus			mError;
	OSPtr<CFStringRef>	mMessage;
	std::string			mState;
	ErrorDescLink*		mErrorDescLink;
	EventHookLink*		mEventHookLink;
};

// ------------------------------------------------------------------------------------------
AEObjectSupport::ExInfo::ExInfo()
	: mValid(false), mError(noErr), mErrorDescLink(NULL), mEventHookLink(NULL)
{
}


// ==========================================================================================
//	AEObjectSupport

#pragma mark AEObjectSupport

// static member variables

const AEObjectSupport*			AEObjectSupport::sAEObjectSupport	= NULL;
const AutoOSLAccessorUPP		AEObjectSupport::sOSLAccessorUPP(OSLAccessorProc);
const AutoOSLCompareUPP			AEObjectSupport::sOSLCompareUPP(OSLCompareProc);
const AutoOSLCountUPP			AEObjectSupport::sOSLCountUPP(OSLCountProc);
const AutoOSLDisposeTokenUPP	AEObjectSupport::sOSLDisposeTokenUPP(OSLDisposeTokenProc);
const AutoOSLGetMarkTokenUPP	AEObjectSupport::sOSLGetMarkTokenUPP(OSLGetMarkTokenProc);
const AutoOSLMarkUPP			AEObjectSupport::sOSLMarkUPP(OSLMarkProc);
const AutoOSLAdjustMarksUPP		AEObjectSupport::sOSLAdjustMarksUPP(OSLAdjustMarksProc);
const AutoOSLGetErrDescUPP		AEObjectSupport::sOSLGetErrDescUPP(OSLGetErrDescProc);
const AutoAEEventHandlerUPP		AEObjectSupport::sAEEventHandlerUPP(AEEventHandlerProc);
#ifndef NDEBUG
const AutoOSLAccessorUPP		AEObjectSupport::sDebugOSLAccessorUPP(DebugOSLAccessorProc);
#endif
AEDescriptor					AEObjectSupport::sNullDescriptor;


// ------------------------------------------------------------------------------------------
const AEObjectSupport&
AEObjectSupport::Get()
{
	B_ASSERT(sAEObjectSupport != NULL);
	
	return (*sAEObjectSupport);
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::AEObjectSupport()
{
	B_ASSERT(sAEObjectSupport == NULL);
	
	OSStatus	err;
	
	// Initialize OSL.
	err = AEObjectInit();
	B_THROW_IF_STATUS(err);

	// Install our global OSL callbacks.
	err = AESetObjectCallbacks(sOSLCompareUPP, sOSLCountUPP, sOSLDisposeTokenUPP, 
							   sOSLGetMarkTokenUPP, sOSLMarkUPP, sOSLAdjustMarksUPP, 
							   sOSLGetErrDescUPP);
	B_THROW_IF_STATUS(err);
	
	// Install object accessors.
	
	// null to anything
	err = AEInstallObjectAccessor(typeWildCard, typeNull, sOSLAccessorUPP, 
								  kTokenAccessor, false);
	B_THROW_IF_STATUS(err);
	
	// token to anything
	err = AEInstallObjectAccessor(typeWildCard, AEToken::kBObjectToken, sOSLAccessorUPP, 
								  kTokenAccessor, false);
	B_THROW_IF_STATUS(err);
	
//	err = AEInstallObjectAccessor(typeWildCard, AEToken::typeBObjectToken, 
//								  sOSLAccessorUPP, 
//								  reinterpret_cast<long>(this), false);
//	B_THROW_IF_STATUS(err);
	
	// property to anything
//	err = AEInstallObjectAccessor(typeWildCard, typeProperty, sOSLAccessorUPP, 
//								  kPropertyAccessor, false);
//	B_THROW_IF_STATUS(err);
	
//	err = AEInstallObjectAccessor(AEToken::typeBObjectToken, typeProperty, 
//								  sOSLPseudoClassAccessorUPP, 
//								  kPseudoClassPropertyAccessor, false);
//	B_THROW_IF_STATUS(err);
	
	// list to anything
	err = AEInstallObjectAccessor(typeWildCard, typeAEList, sOSLAccessorUPP, 
								  kListAccessor, false);
	B_THROW_IF_STATUS(err);
	
#ifndef NDEBUG
	// Install a special debugging accessor.
	err = AEInstallObjectAccessor(typeWildCard, typeWildCard, 
								  sDebugOSLAccessorUPP, 0, false);
	B_THROW_IF_STATUS(err);
#endif
	
	// Install our comparison functions
	SetNumericComparer<typeSInt16>();
	SetNumericComparer<typeSInt32>();
	SetNumericComparer<typeUInt32>();
	SetNumericComparer<typeIEEE32BitFloatingPoint>();
	SetNumericComparer<typeIEEE64BitFloatingPoint>();
	SetSimpleComparer<typeType>(CompareTypeData);
	SetComparer(typeBoolean, CompareBooleanData);
	SetComparer(typeTrue,    CompareBooleanData);
	SetComparer(typeFalse,   CompareBooleanData);
	SetComparer(typeObjectSpecifier, CompareBinaryData);
	SetComparer(typeChar,							CompareStringData);
	SetComparer(typeUTF16ExternalRepresentation,	CompareStringData);
	SetComparer(typeUTF8Text,						CompareStringData);
	SetComparer(typeUnicodeText,					CompareStringData);
	
	sAEObjectSupport = this;
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::~AEObjectSupport()
{
	B_ASSERT(sAEObjectSupport == this);
	
	sAEObjectSupport = NULL;
}

#pragma mark Event Definition

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::DefineEvent(
	AEEventClass				inEventClass,			//!< The Apple Event's class.
	AEEventID					inEventID,				//!< The Apple Event's ID.
	AEInfo::EventDOBehavior		inEventDOBehavior,		//!< Determines how we handle the direct parameter.
	AEInfo::EventResultAction	inEventResultAction,	//!< Determines how we handle the result.
	AEInfo::DefaultEventHandler	inHandler)
{
	AEInfo::EventInfo	eventInfo;
	
	eventInfo.mEventKey.first	= inEventClass;
	eventInfo.mEventKey.second	= inEventID;
	eventInfo.mDOBehavior		= inEventDOBehavior;
	eventInfo.mResultAction		= inEventResultAction;
	eventInfo.mDefaultHandler	= inHandler;
	
	B_THROW_IF(IsEventDefined(inEventClass, inEventID), std::runtime_error("Event already defined"));
	
	RegisterEventHandler(*mEventMap.insert(EventMapType(eventInfo.mEventKey, eventInfo)).first);
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::DefineEvent(
	AEEventClass				inEventClass,			//!< The Apple Event's class.
	AEEventID					inEventID,				//!< The Apple Event's ID.
	AEInfo::EventDOBehavior		inEventDOBehavior,		//!< Determines how we handle the direct parameter.
	AEInfo::EventResultAction	inEventResultAction)	//!< Determines how we handle the result.
{
	DefineEvent(inEventClass, inEventID, inEventDOBehavior, inEventResultAction, 
				boost::bind(&AEObjectSupport::HandleDefaultEvent, this, _1, _2, _3));
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::DefineEvent(
	AEEventClass		inEventClass,			//!< The Apple Event's class.
	AEEventID			inEventID)				//!< The Apple Event's ID.
{
	AEInfo::EventDOBehavior		doBehavior		= AEInfo::kEventDOIllegal;
	AEInfo::EventResultAction	resultAction	= AEInfo::kEventResultActionNone;
	
	AESDefReader::GetDefaultEventBehavior(AEInfo::EventKey(inEventClass, inEventID), 
										  doBehavior, resultAction);
	
	DefineEvent(inEventClass, inEventID, doBehavior, resultAction);
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::IsEventDefined(
	AEEventClass		inEventClass,   //!< The Apple Event's class.
	AEEventID			inEventID)      //!< The Apple Event's ID.
    const
{
	return (mEventMap.find(AEInfo::EventKey(inEventClass, inEventID)) != mEventMap.end());
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::SetClassEventHandler(
	DescType					inClassID,      //!< The class ID of the handler.
	AEEventClass				inEventClass,   //!< The Apple Event's class.
	AEEventID					inEventID,      //!< The Apple Event's ID.
	AEInfo::ClassEventHandler	inClassHandler)	//!< The callback.
{
	AEInfo::EventKey	key(inEventClass, inEventID);
	
	if (!IsEventDefined(inEventClass, inEventID))
	{
		DefineEvent(inEventClass, inEventID);
	}
	
	AEInfo::ClassInfo&	classInfo	= mClassMap.find(inClassID)->second;
	ClassEventMapType	classEvent(key, inClassHandler);
	
	AESDefReader::PropagateClassEvent(classEvent, classInfo);
	
	std::for_each(classInfo.mDescendents.begin(), classInfo.mDescendents.end(), 
				  boost::bind(PropagateClassEventToClass,
							  boost::ref(classEvent), boost::ref(mClassMap), _1));
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::SetDefaultEventHandler(
	AEEventClass				inEventClass,		//!< The Apple Event's class.
	AEEventID					inEventID,			//!< The Apple Event's ID.
	AEInfo::DefaultEventHandler	inDefaultHandler)	//!< The callback.
{
	AEInfo::EventMap::iterator	it	= mEventMap.find(AEInfo::EventKey(inEventClass, inEventID));
	
	B_ASSERT(it != mEventMap.end());
	
	it->second.mDefaultHandler = inDefaultHandler;
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::SetComparer(
	DescType					inDescriptorType,
	AEInfo::DescriptorComparer	inComparer)
{
	ComparerMap::iterator	it	= mComparerMap.find(inDescriptorType);
	
	B_ASSERT(it == mComparerMap.end());
	
	mComparerMap.insert(ComparerMapType(inDescriptorType, inComparer));
}

// ------------------------------------------------------------------------------------------
/*!	Given a class ID, return its class info object (or NULL).
*/
const AEInfo::ClassInfo*
AEObjectSupport::FindClassInfo(DescType inClassID) const
{
	const AEInfo::ClassInfo*			classInfo	= NULL;
	AEInfo::ClassMap::const_iterator	it			= mClassMap.find(inClassID);
	
	if (it != mClassMap.end())
		classInfo = &it->second;
	
	return (classInfo);
}

// ------------------------------------------------------------------------------------------
const AEInfo::ClassInfo&
AEObjectSupport::GetClassInfo(
	DescType			inClassID) const
{
	AEInfo::ClassMap::const_iterator	cit	= mClassMap.find(inClassID);
	
	if (cit == mClassMap.end())
		B_THROW(ConstantOSStatusException<errAECantHandleClass>());
	
	return cit->second;
}

// ------------------------------------------------------------------------------------------
/*!	Given a ClassInfo and a property name, return its property info object (or NULL).
	This function performs a lookup in the given class, as well as all of the class's 
	ancestors.
*/
const AEInfo::PropertyInfo*
AEObjectSupport::FindPropertyInfo(
	const AEInfo::ClassInfo&	inClass, 
	DescType					inPropertyName) const
{
	AEInfo::PropertyMap::const_iterator	pit	= inClass.mProperties.find(inPropertyName);
	
	return ((pit != inClass.mProperties.end()) ? &pit->second : NULL);
}

// ------------------------------------------------------------------------------------------
/*!	Given a ClassInfo and a property name, return its property info object.  If the property 
	isn't found, an exception is thrown.
	This function performs a lookup in the given class, as well as all of the class's 
	ancestors.
*/
const AEInfo::PropertyInfo&
AEObjectSupport::GetPropertyInfo(
	const AEInfo::ClassInfo&	inClass, 
	DescType					inPropertyName) const
{
	const AEInfo::PropertyInfo*	propInfo	= FindPropertyInfo(inClass, inPropertyName);
	
	if (propInfo != NULL)
	{
		return *propInfo;
	}
	else
	{
		B_THROW(ConstantOSStatusException<errAENoSuchObject>());
		
		// GCC complains if we don't return something here.
		return *static_cast<const AEInfo::PropertyInfo*>(0);
	}
}

// ------------------------------------------------------------------------------------------
/*!	Given a ClassInfo and an event key, return its class event handler (or NULL).
	This function performs a lookup in the given class, as well as all of the class's 
	ancestors.
*/
AEInfo::ClassEventHandler
AEObjectSupport::FindClassEventHandler(
	const AEInfo::ClassInfo&	inClass, 
	const AEInfo::EventKey&		inEventKey) const
{
	AEInfo::ClassEventMap::const_iterator	ceit	= inClass.mEvents.find(inEventKey);
	
	return ((ceit != inClass.mEvents.end()) ? ceit->second : AEInfo::ClassEventHandler());
}

// ------------------------------------------------------------------------------------------
/*!	If @a inObjSpecifier is a null descriptor, the function returns AEObject's default 
	object;  else, it attempts to resolve the descriptor.
	
	Errors may be returned if resolution fails (eg the specifier names a non-existent 
	object, or the descriptor isn't an object specifier at all).
*/
AEObjectPtr
AEObjectSupport::Resolve(
	const AEDesc&	inObjSpecifier) 	//!< The Apple %Event descriptor (hopefully containing an object specifier).
{
	AEObjectPtr	obj;
	
	if (inObjSpecifier.descriptorType != typeNull)
	{
		AEAutoTokenDescriptor	tokenDesc;
		AEDescriptor			errorDesc;
		OSStatus				err;
		
		err = PrivateResolve(inObjSpecifier, tokenDesc, errorDesc);
		B_THROW_IF(err != noErr, ObjectResolutionException(err, errorDesc));
		
		if (AEToken::IsTokenDescriptor(tokenDesc))
		{
			AEToken	token(tokenDesc);
			
			if (!token.IsPropertyToken())
			{
				obj = token.GetObject();
			}
			else
			{
				B_THROW(ConstantOSStatusException<errAENotAnElement>());
			}
		}
		else
		{
			B_THROW(AECoercionFailException());
		}
	}
	else
	{
		obj = AEObject::GetDefaultObject();
	}
	
	return obj;
}

// ------------------------------------------------------------------------------------------
/*!	If @a inObjSpecifier is a null descriptor, the function returns AEObject's default 
	object;  else, it attempts to resolve the descriptor.
	
	Errors may be returned if resolution fails (eg the specifier names a non-existent 
	object, or the descriptor isn't an object specifier at all).
*/
OSStatus
AEObjectSupport::Resolve(
	const AEDesc&	inObjSpecifier, 	//!< The Apple %Event descriptor (hopefully containing an object specifier).
	AEObjectPtr&	outObject)
{
	OSStatus	err	= noErr;
	
	outObject.reset();
	
	if (inObjSpecifier.descriptorType != typeNull)
	{
		AEAutoTokenDescriptor	tokenDesc;
		AEDescriptor			errorDesc;
		
		err = PrivateResolve(inObjSpecifier, tokenDesc, errorDesc);
		
		if (err == noErr)
		{
			if (AEToken::IsTokenDescriptor(tokenDesc))
			{
				AEToken	token(tokenDesc);
				
				if (!token.IsPropertyToken())
				{
					outObject = token.GetObject();
				}
				else
				{
					err = errAENotAnElement;
				}
			}
			else
			{
				err = errAECoercionFail;
			}
		}
	}
	else
	{
		outObject = AEObject::GetDefaultObject();
		B_ASSERT(outObject != NULL);
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
SInt32
AEObjectSupport::CountAllElements(
	ConstAEObjectPtr	/* inContainer */) 	//!< The container.
	const
{
#warning stub implementation
	return 0;
#if 0
	B_ASSERT(inContainer != NULL);
	
	// Retrieve the set of element types known to the container.
	
	std::set<DescType>	elements;
	
	for (const ClassInfo* classInfo = FindClass(inContainer->GetClassID()); 
		 classInfo != NULL; 
		 classInfo = classInfo->mInherits)
	{
		std::transform(classInfo->mElements.begin(), classInfo->mElements.end(), 
					   std::inserter(elements, elements.end()), 
					   MakeElementType);
	}
	
	// Filter out element types that are derived from other element types.
	
	for (std::set<DescType>::iterator it = elements.begin(); 
		 it != elements.end(); 
		 ++it)
	{
		DescType	elemType	= *it;
		
		for (std::set<DescType>::iterator it2 = elements.begin(); 
			 it2 != elements.end(); )
		{
			if (*it2 != elemType)
			{
				if (DoesClassInheritFrom(*it2, elemType))
				{
					std::set<DescType>::iterator	tempIt(it2);
					
					++tempIt;
					elements.erase(it2);
					it2 = tempIt;
					
					continue;
				}
			}
			
			++it2;
		}
	}
	
	// Count the elements of the remaining types.
	
	SInt32	count	= 0;
	
	for (std::set<DescType>::iterator it = elements.begin(); 
		 it != elements.end(); 
		 ++it)
	{
		count += inContainer->CountElements(*it);
	}
	
	return (count);
#endif
}

// ------------------------------------------------------------------------------------------
AEObjectPtr
AEObjectSupport::TokenToObject(
	const AEToken&	inToken) const
{
	return inToken.GetObject();
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::CoerceDesc(
	const AEDesc&		inObjectSpecifier,
	DescType			inDesiredType,
	AEDesc&				outDesc)
{
	AEAutoTokenDescriptor	tokenDesc;
	AEDescriptor			resolvedDesc, errorDesc;
	OSStatus				err;
	
	err = PrivateResolve(inObjectSpecifier, tokenDesc, errorDesc);
	
	if (err == errAENotAnObjSpec)
	{
		// We don't have an object specifier.
		err = AEDuplicateDesc(&inObjectSpecifier, resolvedDesc);
		B_THROW_IF_STATUS(err);
	}
	else if (err == noErr)
	{
		// We have an object specifier.
		
		B_ASSERT(AEToken::IsTokenDescriptor(tokenDesc));
		
		AEToken		token(tokenDesc);
		AEWriter	writer;
		
		AEObject::WriteTokenSpecifier(token, writer);
		writer.Close(resolvedDesc);
	}
	else
	{
		B_THROW(ObjectResolutionException(err, errorDesc));
	}

	// Finally, if the client requested data of a specific type,
	// try the coercion.
	
	if (inDesiredType == cNumber)
		inDesiredType = typeSInt32;
	
	if ((inDesiredType != typeWildCard) && (inDesiredType != resolvedDesc.GetType()))
	{
		AEDescriptor	coercedDesc;
		
		err = AECoerceDesc(resolvedDesc, inDesiredType, coercedDesc);
		B_THROW_IF_STATUS(err);
		
		std::swap(static_cast<AEDesc&>(outDesc), static_cast<AEDesc&>(coercedDesc));
	}
	else
	{
		std::swap(static_cast<AEDesc&>(outDesc), static_cast<AEDesc&>(resolvedDesc));
	}
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::CoerceDesc(
	const AEDesc&			inObjectSpecifier,
	DescType				inDesiredType,
	void*					outBuffer,
	size_t					inBufferSize)
{
	AEDescriptor	coercedDesc;
	OSStatus		err;
	
	CoerceDesc(inObjectSpecifier, inDesiredType, coercedDesc);
	
	if (coercedDesc.GetType() == inDesiredType)
	{
		B_ASSERT(static_cast<size_t>(AEGetDescDataSize(coercedDesc)) == inBufferSize);
		
		err = AEGetDescData(coercedDesc, outBuffer, inBufferSize);
		B_THROW_IF_STATUS(err);
	}
	else
	{
		B_THROW(AECoercionFailException());
	}
}

// ------------------------------------------------------------------------------------------
OSStatus
AEObjectSupport::CoerceDesc(
	const AEDesc&		inObjectSpecifier,
	DescType			inDesiredType,
	AEDesc&				outDesc,
	const std::nothrow_t&)
{
	AEAutoTokenDescriptor	tokenDesc;
	AEDescriptor			resolvedDesc, errorDesc;
	OSStatus				err;
	
	err = PrivateResolve(inObjectSpecifier, tokenDesc, errorDesc);
	
	if (err == errAENotAnObjSpec)
	{
		// We don't have an object specifier.
		err = AEDuplicateDesc(&inObjectSpecifier, resolvedDesc);
	}
	else if (err == noErr)
	{
		// We have an object specifier (?).
		
		B_ASSERT(AEToken::IsTokenDescriptor(tokenDesc));
		
		if (AEToken::IsTokenDescriptor(tokenDesc))
		{
			AEToken						token(tokenDesc);
			DescType					classID		= token.GetObject()->GetClassID();
			const AEInfo::ClassInfo*	classInfo	= sAEObjectSupport->FindClassInfo(classID);
			
			if (classInfo != NULL)
			{
				try
				{
					AEWriter	writer;
					
					AEObject::WriteTokenSpecifier(token, writer);
					writer.Close(resolvedDesc);
				}
				catch (std::exception& ex)
				{
					err = ErrorHandler::GetStatus(ex, errAECoercionFail);
				}
				catch (...)
				{
					err = errAECoercionFail;
				}
			}
			else
			{
				err = errAECoercionFail;
			}
		}
		else
		{
			err = errAECoercionFail;
		}
	}

	// Finally, if the client requested data of a specific type,
	// try the coercion.
	
	if (err == noErr)
	{
		if (inDesiredType == cNumber)
			inDesiredType = typeSInt32;
		
		if ((inDesiredType != typeWildCard) && (inDesiredType != resolvedDesc.GetType()))
		{
			AEDescriptor	coercedDesc;
			
			err = AECoerceDesc(resolvedDesc, inDesiredType, coercedDesc);
			
			if (err == noErr)
			{
				std::swap(outDesc, static_cast<AEDesc&>(coercedDesc));
			}
		}
		else
		{
			std::swap(outDesc, static_cast<AEDesc&>(resolvedDesc));
		}
	}
	
	return err;
}

// ------------------------------------------------------------------------------------------
OSStatus
AEObjectSupport::CoerceDesc(
	const AEDesc&			inObjectSpecifier,
	DescType				inDesiredType,
	void*					outBuffer,
	size_t					inBufferSize,
	const std::nothrow_t&	nt)
{
	AEDescriptor	coercedDesc;
	OSStatus		err;
	
	err = CoerceDesc(inObjectSpecifier, inDesiredType, coercedDesc, nt);
	
	if (coercedDesc.GetType() == inDesiredType)
	{
		B_ASSERT(static_cast<size_t>(AEGetDescDataSize(coercedDesc)) == inBufferSize);
		
		err = AEGetDescData(coercedDesc, outBuffer, inBufferSize);
	}
	else
	{
		err = errAECoercionFail;
	}
	
	return err;
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::DoesClassInheritFrom(
	DescType	inClassID, 		//!< The class ID;  must match the application's AppleScript dictionary.
	DescType	inBaseClassID)	//!< The base class ID;  must match the application's AppleScript dictionary.
	const
{
	const AEInfo::ClassInfo& classInfo = GetClassInfo(inClassID);
	
	return (classInfo.mAncestors.find(inBaseClassID) != classInfo.mAncestors.end());
}

#pragma mark Reading Scripting Definitions

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::RegisterScriptingDefinitions(
	const Bundle&		inBundle, 
	const String&		inSDefName /* = String() */)
{
	AESDefReader	reader(
						mClassMap, mEventMap, 
						boost::bind(&AEObjectSupport::HandleDefaultEvent, this, _1, _2, _3));
	
	reader.Read(inBundle, inSDefName);
	
	std::for_each(mEventMap.begin(), mEventMap.end(), 
				  boost::bind(&AEObjectSupport::RegisterEventHandler, this, _1));

#ifndef NDEBUG
	reader.DebugPrint();
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::RegisterEventHandler(
	const EventMapType&			inEvent)
{
	const AEInfo::EventInfo&	eventInfo	= inEvent.second;
	size_t						eventIndex	= mEventKeys.size();
	OSStatus					err;
	
	mEventKeys.push_back(eventInfo.mEventKey);
	
	err = AEInstallEventHandler(eventInfo.mEventKey.first, eventInfo.mEventKey.second, 
								sAEEventHandlerUPP, eventIndex, false);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::PropagateClassEventToClass(
	const ClassEventMapType& inClassEvent, 
	AEInfo::ClassMap&		ioClassMap,
	DescType				inClassID)
{
	AESDefReader::PropagateClassEvent(inClassEvent, ioClassMap.find(inClassID)->second);
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::ExInfo&
AEObjectSupport::GetExInfo() const
{
	ExInfo*	info	= mExInfoPtr.get();
	
	if (info == NULL)
	{
		info = new ExInfo;
		
		mExInfoPtr.reset(info);
	}
	
	return (*info);
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::ClearException() const
{
	ExInfo&	exInfo	= GetExInfo();
	
	exInfo.mValid	= false;
	exInfo.mError	= noErr;
	
	exInfo.mMessage.reset();
	exInfo.mState.clear();
}

// ------------------------------------------------------------------------------------------
OSStatus
AEObjectSupport::CacheExceptionInfo(const std::exception& ex) const
{
	OSStatus	err;
	
	err = ErrorHandler::GetStatus(ex, errAEEventFailed);
	
	try
	{
		ExInfo&				exInfo	= GetExInfo();
		std::ostringstream	ostr;
		
		ExceptionStreamer::Get()->Externalize(ex, ostr);
		
		exInfo.mState	= ostr.str();
		exInfo.mMessage.reset(ErrorHandler::Get()->CopyExceptionMessage(ex), from_copy);
		exInfo.mError	= err;
		exInfo.mValid	= true;
	}
	catch (...)
	{
		// Just prevent exceptions from propagating.
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::ErrorDescLink*
AEObjectSupport::SetErrorDescLink(ErrorDescLink* inLink) const throw()
{
	ExInfo*			info	= mExInfoPtr.get();
	ErrorDescLink*	oldLink	= NULL;
	
	if (info != NULL)
	{
		oldLink					= info->mErrorDescLink;
		info->mErrorDescLink	= inLink;
	}
	
	return (oldLink);
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::EventHookLink*
AEObjectSupport::SetEventHookLink(EventHookLink* inLink) const throw()
{
	ExInfo*			info	= mExInfoPtr.get();
	EventHookLink*	oldLink	= NULL;
	
	if (info != NULL)
	{
		oldLink					= info->mEventHookLink;
		info->mEventHookLink	= inLink;
	}
	
	return (oldLink);
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::AddCachedInfoToAppleEventReply(AppleEvent& ioReply, OSStatus status)
{
	ExInfo&	exInfo	= Get().GetExInfo();
	
	if (exInfo.mValid && (status == exInfo.mError))
	{
		// If we have a textual message, add it to the reply.  This message isn't used 
		// by the the exception-handling machinery, but is meant as a convenience for 
		// those driving the application via AppleScript.
		
		if (exInfo.mMessage != NULL)
		{
			DescParamHelper::WriteKey<typeUTF16ExternalRepresentation>(
					ioReply, keyErrorString, String(exInfo.mMessage));
		}
		
		if (!exInfo.mState.empty())
		{
			OSStatus	err;
			
			err = AEPutParamPtr(&ioReply, 
								keyBExceptionState, typeBExceptionState, 
								exInfo.mState.data(), exInfo.mState.size());
			B_THROW_IF_STATUS(err);
		}
	}
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::RethrowExceptionFromAppleEventReply(
	const AppleEvent&	inReply, 
	OSStatus			inStatus)
{
	Size		size;
	DescType	descType;
	String		message;
	OSStatus	err;
	
	err = AESizeOfParam(&inReply, keyBExceptionState, &descType, &size);
	
	if ((err == noErr) && (descType == typeBExceptionState))
	{
		std::vector<char>	buffer(size);
		DescType			junkType;
		Size				junkSize;
		
		err = AEGetParamPtr(&inReply, keyBExceptionState, typeBExceptionState, 
							&junkType, &buffer[0], buffer.size(), &junkSize);
		B_THROW_IF_STATUS(err);
		
		std::string			str(&buffer[0], buffer.size());
		std::istringstream	istr(str);
		
		// If all goes well, this will throw an exception.
		ExceptionStreamer::Get()->Rethrow(istr);
		
		// If we're here, then something went wrong.  Fall through to the fall-back 
		// behaviour.
	}
	
	err = DescParamHelper::ReadKey<typeUTF16ExternalRepresentation>(inReply, keyErrorString, message, std::nothrow);
	if (err != noErr)
		message.clear();
	
	ErrorHandler::Get()->ThrowStatusException(inStatus, message);
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::NullDefaultEventHandler(
	const AEDesc&		/* inDirectObject */, 
	const AppleEvent&	/* inEvent */, 
	AEDesc&				/* outResult */)
{
	B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLAccessorProc(
	DescType			inDesiredClass, 
	const AEDesc*		inContainer, 
	DescType			inContainerClass, 
	DescType			inKeyForm, 
	const AEDesc*		inKeyData, 
	AEDesc*				outTokenDesc, 
	long				inRefcon)
{
	OSStatus	err	= noErr;
	
	try
	{
		sAEObjectSupport->ClearException();
		
		B_ASSERT(inContainer != NULL);
		B_ASSERT(inKeyData != NULL);
		B_ASSERT(outTokenDesc != NULL);
		
		AEInitializeDescInline(outTokenDesc);
		
		switch (inRefcon)
		{
		case kTokenAccessor:
			sAEObjectSupport->TokenAccessor(inDesiredClass, *inContainer, inContainerClass, 
											inKeyForm, *inKeyData, *outTokenDesc);
			break;

		case kPropertyAccessor:
			sAEObjectSupport->PropertyAccessor(inDesiredClass, *inContainer, inContainerClass, 
											   inKeyForm, *inKeyData, *outTokenDesc);
			break;

		case kListAccessor:
			sAEObjectSupport->ListAccessor(inDesiredClass, *inContainer, inContainerClass, 
										   inKeyForm, *inKeyData, *outTokenDesc);
			break;

		default:
			B_THROW(AENoSuchObjectException());
			break;
		}
		
#warning fix me
#if 0
		
		if (containerClass == typeNull)
			containerClass = cApplication;
		
		const AEToken	inputToken(*container);
		
		sAEObjectSupport->ClassAccessor(desiredClass, inputToken, containerClass, form, 
										*selectionData, *value);
#endif
	}
	catch (const std::exception& ex)
	{
		AEDisposeToken(outTokenDesc);
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		AEDisposeToken(outTokenDesc);
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
// ClassOSLAccessorProc is the core OSL object accessor callback we supply
// to OSL.  It is responsible for getting tokens for objects within objects
// that belong to us.  For example, if you ask for "button 1 of window 1",
// this routine is first called to get a token for window 1 within the
// application object, and is then called to get a token for button 1 within
// the window 1 object.
//
// The core work for this process is done by the object primitives that the
// client supplies in the class table.  This routine (and the sub-routines
// it calls) is simply responsible for a) converting descriptors to tokens
// and back, b) converting the selectionData to its simplest form, and
// c) finding and calling the appropriate object primitive.
//
// See also PseudoClassOSLAccessorProc, which is used to access objects
// within classes that /aren’t/ in the class table, such as objects within
// properties and objects within lists.
void
AEObjectSupport::TokenAccessor(
	DescType			inDesiredClass, 
	const AEDesc&		inContainer, 
	DescType			inContainerClass, 
	DescType			inKeyForm, 
	const AEDesc&		inKeyData, 
	AEDesc&				outTokenDesc) const
{
#if 0
#ifndef NDEBUG
	printf("TokenAccessor\n");
	printf("\tdesiredClass %.4s\n", &inDesiredClass);
	printf("\tcontainer->descriptorType %.4s\n", &inContainer.descriptorType);
	printf("\tcontainerClass %.4s\n", &inContainerClass);
	printf("\tform %.4s\n", &inKeyForm);
	printf("\tselectionData->descriptorType %.4s\n", &inKeyData.descriptorType);
#endif
#endif

#if 0
	#pragma unused(containerClass)
	OSStatus 	   err;
	MOSLClassIndex thisClass;
	MOSLToken 	   containerTok;

	assert(container      != NULL);
	assert(selectionData  != NULL);
	assert(value          != NULL);
	assert(accessorRefCon >= 0 && accessorRefCon < gClassTableSize);

	if (gDebugFlags & kMOSLLogOSLMask) {
		BBLogLine("\pClassOSLAccessorProc");
		BBLogIndent();
		BBLogDescType("\pdesiredClass", desiredClass);
		BBLogDesc("\pcontainer", container);
		BBLogDescType("\pcontainerClass", containerClass);
		BBLogDescType("\pform", form);
		BBLogDesc("\pselectionData", selectionData);
		BBLogLong("\paccessorRefCon", accessorRefCon);
	}

	MoreAENullDesc(value);
	
	// Validate some parameters.
	
	if ( accessorRefCon >= 0 && accessorRefCon < gClassTableSize ) {
		thisClass = accessorRefCon;
		err = noErr;
	} else {
		err = errAEEventFailed;
	} 
	if (err == noErr) {
		if ( container->descriptorType == typeProperty ) {
			err = errAENotAnElement;
		} else {
			DescToMOSLToken(container, &containerTok);
		}
	}
#endif
	
	const AEToken	containerToken(inContainer);
	
	// Validate some parameters.
	
	inContainerClass = containerToken.GetObjectClassID();
	
	if (containerToken.IsPropertyToken())
		B_THROW(ConstantOSStatusException<errAENotAnElement>());
	
	// Then dispatch to the appropriate sub-routine based on the key form.
	
	const AEInfo::ClassInfo&	classInfo		= GetClassInfo(inContainerClass);
	AEObjectPtr					directObject	= containerToken.GetObject();
	
	directObject->AccessElements(classInfo, inDesiredClass, inKeyForm, 
								 inKeyData, outTokenDesc);

#if 0
	// Clean up.

	if (err != noErr) {
		MoreAEDisposeDesc(value);
	}

	if (gDebugFlags & kMOSLLogOSLMask) {
		BBLogDesc("\p<value", value);
		BBLogOutdentWithErr(err);
	}

	return err;
#endif
}

#if 0
// ------------------------------------------------------------------------------------------
// SubGetTokenValue is a subroutine of GetTokenValue.  It’s not
// meant to be called directly, only as part of GetTokenValue.
// Given a getter primitive and a token, get the data for the
// object referenced by the token.  If the value is a token
// and resolution is kReturnTokensAsObjects, convert the token
// to an object specifier before returning it.
void
AEObjectSupport::SubGetTokenValue(
	const ClassInfo&	inContainerInfo, 
	const AEToken&		inContainerToken, 
	AEDesc&				outValue) const
{
	AEObject::DispatchGetter(inContainerToken, outValue);
	
	if (inReturnObjects)
	{
		if (mClassMap.find(outValue.descriptorType) != mClassMap.end())
		{
			// If the result is a token and we’re being asked to return
			// an object specifier, convert the token to an object specifier.
			// We do this by getting the token out of the descriptor and calling
			// the token’s class’s getter.
			
			AEToken	resultToken(outValue, AEToken::take_ownership);
			
			AEObject::DispatchGetter(resultToken, outValue);
		}
	}
	
#if 0
	OSStatus       err;
	MOSLClassIndex resultClass;
	MOSLToken 	   resultTok;

	assert(thisClass < gClassTableSize);
	assert(tok    != NULL);
	assert((resolution == kReturnTokensAsObjects) || (resolution == kReturnTokensAsTokens));
	assert(result != NULL);
	
	if ((tok->tokType == typeProperty) && (tok->tokPropName == pClass)) {
		err = AECreateDesc(typeType, &tok->tokObjType, sizeof(tok->tokObjType), result);
	} else {
		err = CallGetter(thisClass, tok, result);
	}
	if ((err == noErr) && (resolution == kReturnTokensAsObjects)) {
		if (ClassIDToClassIndex(result->descriptorType, &resultClass) == noErr) {
		
			// If the result is a token and we’re being asked to return
			// an object specifier, convert the token to an object specifier.
			// We do this by getting the token out of the descriptor and calling
			// the token’s class’s getter.
			
			DescToMOSLToken(result, &resultTok);
			MoreAEDisposeDesc(result);
			err = CallGetter(resultClass, &resultTok, result);
		}
	}
	return err;
#endif
}
#endif

// PseudoClassOSLAccessorProc is a secondary OSL object accessor callback we
// supply to OSL.  It is responsible for accessing objects within objects
// that don’t belong to us, such as objects of typeAEList or typeProperty.
// For example, if you ask for "name of parent of node 1 of window 1", the
// ClassOSLAccessorProc is called to get window 1 of the application, then
// to get node 1 of the window, then to get parent of the node.  But the
// parent itself is of typeProperty, so OSL needs some way to get its name.
// We archieve this by installing PseudoClassOSLAccessorProc as the accessor
// for typeProperty, with the accessorRefCon set to kPseudoCPropertyIndex.
// When OSL calls this routine, we call a sub-routine (PseudoCPropertyAccessor)
// that gets the token for the property and then redispatches the object
// access to that token.
//
// A similar approach is needed for accessing objects within lists and
// for handling AppleScript’s built-in (but not quite built-in enough)
// cFile class.
//
// See also ClassOSLAccessorProc, which is used to access objects
// within classes that are in the class table.


// There are a number of circumstances under which OSL will attempt
// to access an object within one of the standard system types,
// such as lists and properties.  While we don’t handle all of these
// cases, we have to handle some subset in order to present a reasonable
// scripting interface.

// ------------------------------------------------------------------------------------------
// PseudoCPropertyAccessor is a sub-routine of PseudoClassOSLAccessorProc that 
// handles requests for to access an object within a property.  We use a fairly
// sneaky approach to implement this.  The property must be a property within
// one of our objects, so we work out what class of object it is, then we
// call the class’s "getter" object primitive to get the token for the actual
// object that the property represents.  We then use AECallObjectAccessor
// to get OSL to dispatch the request back to the accessor appropriate for
// that token.
void
AEObjectSupport::PropertyAccessor(
	DescType			inDesiredClass, 
	const AEDesc&		inContainer, 
	DescType			inContainerClass, 
	DescType			inKeyForm, 
	const AEDesc&		inKeyData, 
	AEDesc&				/* outTokenDesc */) const
{
#ifndef NDEBUG
	std::cout << "PropertyAccessor\n";
	std::cout << "\tdesiredClass '" << make_string(inDesiredClass) << "'\n";
	std::cout << "\tcontainer->descriptorType '" << make_string(inContainer.descriptorType) << "'\n";
	std::cout << "\tcontainerClass '" << make_string(inContainerClass) << "'\n";
	std::cout << "\tform '" << make_string(inKeyForm) << "'\n";
	std::cout << "\tselectionData->descriptorType '" << make_string(inKeyData.descriptorType) << "'\n";
#endif

#if 0
	OSStatus 	   err;
	AEDesc 		   propTokDesc;
	MOSLToken 	   containerTok;
	MOSLClassIndex containerClassIndex;

	assert(container     != NULL);
	assert(selectionData != NULL);
	assert(value         != NULL);
	
	MoreAENullDesc(value);
	MoreAENullDesc(&propTokDesc);
		
	if (containerClass == typeProperty) {
	
		// Extract the container token from the descriptor, look up its class
		// and then call the class "getter" object primitive.  This yields
		// the token for object that this property points to.
		
		DescToMOSLToken(container, &containerTok);
		err = ClassIDToClassIndex(containerTok.tokObjType, &containerClassIndex);
		if (err == noErr) {
			err = GetTokenValue(containerClassIndex,
									&containerTok, 
									kReturnTokensAsTokens, 
									&propTokDesc);
		}
		
		// Once we have the object’s token, we call back into OSL to find the object
		// within that token.  This will redispatch back to either ClassOSLAccessorProc
		// or PseudoClassOSLAccessorProc.
		
		if (err == noErr) {
			err = AECallObjectAccessor(desiredClass, &propTokDesc, propTokDesc.descriptorType, form, selectionData, value);
		}
	} else {
		assert(false);
		err = errAENoSuchObject;
	}
	
	MoreAEDisposeDesc(&propTokDesc);
	
	return err;
#endif
}

// "Deep" Resolution versus "Shallow" Resolution
// ---------------------------------------------
// MOSL’s method of resolving elements of lists is called "deep" resolution.
// The distinction between deep and shallow is exemplified by the following
// AppleScript snippet.
//
// 		tell application "Finder
//			file 1 of every item of startup disk
//		end tell
//
// In shallow resolution (which is what the Finder implements), this would
// return the first file in the root directory of the startup disk.  In
// deep resolution, this would return a list, with one element per item
// in the root directory, where each element is either the first file of
// that item (if the item is a folder and it contains a file), or the
// special value "missing value" otherwise.
// 
// Deep resolution is the resolution method recommended by the AppleScript
// engineering team, and is also explicitly recommended by the AppleScript
// Language Guide (v1.3.7, p172):
//
//		If you specify an every element reference as the container for [...
//		an] object, the result is a list containing the specified [...]
//		object for each object of the container.
//
// Deep resolution has its difficulties.  See:
//
// o You have to do a special hack to make "repeat with doc in every document"
//   work properly.  See the comments at the call site for PseudoCListAccessor.
//
// o Making formRange work was very tricky.  See the comment above
//   (formRange for typeAEList) for details.

// ------------------------------------------------------------------------------------------
// PseudoCListAccessor is the sub-routine of PseudoClassOSLAccessorProc
// that handles requests to get objects from a list.  For example,
// if you execute the event "get name of every document", OSL will
// call ClassOSLAccessorProc to get every document.  That returns a list
// of tokens.  OSL will then try to get the name of that list.  When
// it does, it ends up here.
//
// The technique we use is to iterate through the list, extracting each
// element and calling AECallObjectAccessor on it, then placing the resulting
// value in the response list.  AECallObjectAccessor redispatches the object
// access to the accessor routine appropriate for the class of the element.
// Typically this involves calling back to either our ClassOSLAccessorProc
// (if the element is a real class) or PseudoClassOSLAccessorProc (if the
// element is itself a list).
//
// There are two notable special cases:
//
// 1. formRange -- If we’re accessing elements by formRange, we have to
// 	  fix up the object specifiers for the boundary objects.  This is
//    discussed in depth in the comments immediately above this routine.
//
// 2. missing value -- If the AECallObjectAccessor returns an error
//    (indicating that this object is missing for some reason), we swallow
//    the error and substitute the "missing value" value.  However, if
//    when we get to the end we find that we’ve not generated any useful
//    data, we raise an error.
//
// 3. lists of lists -- If the result we get from AECallObjectAccessor is a list, 
// 	  we append its contents to our results list rather than simply inserting a list 
// 	  into our results list.  This ensures that lists of lists come out flat
//    rather than nested, which is what our MOSLAppleEventHandler routine (and a
//    scripter targeting the application) expects.
void
AEObjectSupport::ListAccessor(
	DescType			inDesiredClass, 
	const AEDesc&		inContainer, 
	DescType			inContainerClass, 
	DescType			inKeyForm, 
	const AEDesc&		inKeyData, 
	AEDesc&				outTokenDesc) const
{
#ifndef NDEBUG
	std::cout << "ListAccessor\n";
	std::cout << "\tdesiredClass '" << make_string(inDesiredClass) << "'\n";
	std::cout << "\tcontainer->descriptorType '" << make_string(inContainer.descriptorType) << "'\n";
	std::cout << "\tcontainerClass '" << make_string(inContainerClass) << "'\n";
	std::cout << "\tform '" << make_string(inKeyForm) << "'\n";
	std::cout << "\tselectionData->descriptorType '" << make_string(inKeyData.descriptorType) << "'\n";
#endif

	// The following special case (accessing cObjects in typeAELists by
	// absolution position) is a hack to get:
	// 
	//   repeat with doc in every document
	//     ... do something with doc ...
	//   end repeat
	//
	// to work property.  This AppleScript construct generates these object 
	// specifiers, and we have to handle them a shallow fashion (see
	// "Deep" Resolution versus "Shallow" Resolution, above) in order for
	// this construct to work property [2445795].  I’m somewhat leary of this
	// hacked up solution because it assumes that the selectionData
	// is always a positive integer.  What about typeAbsoluteOrdinal?  Well,
	// AppleScript never seems to send us these in this context, so I’m going
	// to ignore the possibility told otherwise.

	OSStatus	err;

	if ((inKeyForm == formAbsolutePosition) && (inDesiredClass == cObject))
	{
		SInt32	index;
		
		DescParam<typeSInt32>::Get(inKeyData, index);
		
		err = AEGetNthDesc(&inContainer, index, typeWildCard, NULL, &outTokenDesc);
		B_THROW_IF_STATUS(err);

		return;
	}
	
	size_t		count = 0, missingValueCount = 0;
	AEWriter	writer;
	
	{
		AutoAEWriterList	autoList(writer);
		
		// Iterate through the elements in the list.  For each element,
		// call AECallObjectAccessor to access the data within that element,
		// then place the results into our output list.  Oh yeah, and handle
		// some special cases (-:
		
		AEListIterator	it(MakeAEListBeginIterator(inContainer));
		
		count = it.size();
		
		std::for_each(it, MakeAEListEndIterator(inContainer), 
					  boost::bind(&AEObjectSupport::ListItemAccessor, 
								  this, inDesiredClass, inKeyForm, boost::ref(inKeyData), 
								  _1, boost::ref(missingValueCount), 
								  boost::ref(writer)));
				
	}
	
	if (missingValueCount == count)
		B_THROW(AENoSuchObjectException());
	
	writer.Close(outTokenDesc);
	
#if 0
	OSStatus err;
	SInt32    elementCount;
	SInt32    elementIndex;
	AEDesc    thisElement;
	AEDesc 	  thisValue;
	AEDesc    thisSelectionData;
	AEKeyword junkKeyword;
	SInt32    missingValueCount;

	assert(container     != NULL);
	assert(container->descriptorType == typeAEList);
	assert(selectionData != NULL);
	assert(value         != NULL);
	
	MoreAENullDesc(value);
	
	// Count the elements in the list and create an output list.
	
	err = AECountItems(container, &elementCount);
	if (err == noErr) {
		err = AECreateList(NULL, 0, false, value);
	}
	
	// Iterate through the elements in the list.  For each element,
	// call AECallObjectAccessor to access the data within that element,
	// then place the results into our output list.  Oh yeah, and handle
	// some special cases (-:
	
	if (err == noErr) {
		missingValueCount = 0;
		for (elementIndex = 1; elementIndex <= elementCount; elementIndex++) {
			MoreAENullDesc(&thisElement);
			MoreAENullDesc(&thisValue);
			MoreAENullDesc(&thisSelectionData);
			
			err = AEGetNthDesc(container, elementIndex, typeWildCard, &junkKeyword, &thisElement);
			if (err == noErr) {

				// If we’re accessing elements by formRange, fix up the selection data
				// for this element, otherwise use the standard selection data that was
				// passed into us for each element.
				
				if (form == formRange) {
					err = AdjustFormRangeSelectionData(selectionData, elementIndex, &thisSelectionData);
				} else {
					err = AEDuplicateDesc(selectionData, &thisSelectionData);
				}
			}
			if (err == noErr) {
				err = AECallObjectAccessor(desiredClass, &thisElement, thisElement.descriptorType, form, &thisSelectionData, &thisValue);

				// If we got an error resolving this object, substitute the "missing value"
				// value.  Originally I had this test for just errAENoSuchObject, but it
				// turns out that other legitimate errors need this treatment as well.
				
				if (err != noErr) {
					assert(thisValue.descriptorType == typeNull);
					missingValueCount += 1;
					err = MoreAECreateMissingValue(&thisValue);
				}
			}
			if (err == noErr) {

				// Question:
				// Do we want to append lists in all cases, or only when the object specifier
				// is of "every foo of every bar"?  My vote, and the current implementation,
				// is that we always append and never return nested lists.  This is somewhat
				// contadictory to the exact text of the AppleScript Language Guide, but in
				// line with most applications.
				
				if (thisValue.descriptorType == typeAEList) {
					err = MoreAEAppendListToList(&thisValue, value);
				} else {
					err = AEPutDesc(value, 0, &thisValue);
				}
			}
			
			MoreAEDisposeDesc(&thisElement);
			MoreAEDisposeDesc(&thisValue);
			MoreAEDisposeDesc(&thisSelectionData);
			if (err != noErr) {
				break;
			}
		}
	}
	
	// If all elements are missing values, we raise an error rather than
	// return a list containing only "missing value" elements.
	
	if (err == noErr && missingValueCount == elementCount) {
		err = errAENoSuchObject;
	}

	if (err != noErr) {
		MoreAEDisposeDesc(value);
	}
	
	return err;
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::ListItemAccessor(
	DescType			inDesiredClass, 
	DescType			inKeyForm, 
	const AEDesc&		inKeyData, 
	const AEDesc&		inItemDesc,
	size_t&				ioMissingValueCount,
	AEWriter&			ioWriter) const
{
	AEDescriptor	itemKeyData, itemTokenDesc;
	OSStatus		err;
	
	// If we’re accessing elements by formRange, fix up the selection data
	// for this element, otherwise use the standard selection data that was
	// passed into us for each element.
	
	if (inKeyForm == formRange)
	{
#warning fix me
#if 0
		AEDescriptor	adjustedDesc;
		
		err = AdjustFormRangeSelectionData(inKeyData, i, itemKeyData);
#endif
	}
	else
	{
		err = AEDuplicateDesc(&inKeyData, itemKeyData);
		B_THROW_IF_STATUS(err);
	}
	
	DescType	itemContainerClass	= AEToken::GetContainerClass(inItemDesc);
	
	err = AECallObjectAccessor(inDesiredClass, &inItemDesc, itemContainerClass, 
							   inKeyForm, itemKeyData, itemTokenDesc);

	// If we got an error resolving this object, substitute the "missing value"
	// value.  Originally I had this test for just errAENoSuchObject, but it
	// turns out that other legitimate errors need this treatment as well.
	
	if (err != noErr)
	{
		B_ASSERT(itemTokenDesc.GetType() == typeNull);
		
		ioMissingValueCount += 1;
		
		ioWriter.Write<typeType>(cMissingValue);
	}
	else
	{
		// Question:
		// Do we want to append lists in all cases, or only when the object specifier
		// is of "every foo of every bar"?  My vote, and the current implementation,
		// is that we always append and never return nested lists.  This is somewhat
		// contadictory to the exact text of the AppleScript Language Guide, but in
		// line with most applications.
		
		if (itemTokenDesc.GetType() == typeAEList)
		{
			std::for_each(MakeAEListBeginIterator(itemTokenDesc), 
						  MakeAEListEndIterator(itemTokenDesc), 
						  boost::bind(&AEWriter::WriteDesc, boost::ref(ioWriter), _1));
		}
		else
		{
			ioWriter.WriteDesc(itemTokenDesc);
		}
	}
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLCompareProc(
	DescType			oper, 
	const AEDesc*		obj1Ptr, 
	const AEDesc*		obj2Ptr, 
	Boolean*			result)
{
	OSStatus	err	= noErr;
	
	try
	{
		DebugPrint("OSLCompareProc obj1", obj1Ptr);
		DebugPrint("OSLCompareProc obj2", obj2Ptr);
		
		sAEObjectSupport->ClearException();
		
		// Technote 1095 says that we may need to do this because older OSLs might
		// pass the address of an AEDesc that’s in an unlocked handle.
		
		AEDesc	obj1 = *obj1Ptr;
		AEDesc	obj2 = *obj2Ptr;
		
		*result = sAEObjectSupport->HandleCompare(oper, obj1, obj2);
	}
	catch (const std::exception& ex)
	{
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
// This routine is the object comparison callback we supplied to
// OSL.  It is responsible for comparing the two operands in
// obj1Param and obj2Param using the comparison operator
// in oper (eg kAEEquals), setting *result appropriately.
//
// This routine is more complicated than it really should be because
// OSL is stupider than it really should be.  [2444551]  Two examples of 
// OSL’s limitations are:
//
// 1. If both operands are built-in AppleScript types (eg typeText), there
//    are circumstances under which OSL will /still/ call your compare proc
//    rather than use the comparison engine built in to AppleScript.
//
// 2. If at least one of the operands was a property, OSL could call your
//    "get data" Apple event handler to get its data and then do the
//    data comparison itself (as in point 1.).  However, it’s just not
//    smart enough to do this.
//
// These limitations are one of the driving forces behind MOSL.  I had
// to do all this work, but I didn’t see why you should have to do it as well.
bool
AEObjectSupport::HandleCompare(
	DescType			inOperator, 
	const AEDesc&		inTokenDesc1, 
	const AEDesc&		inTokenDesc2) const
{
	// Each side of the comparison is either property, object descriptor, or data.
	// Following table explains what we do in each case.

	// item obj1 obj2   what to do
	// ---- ---- ----   ----------
	// 1.	prop prop   compare data	get obj1, coerce obj2 to same type
	// 2.	prop obj    compare data    get obj1, coerce obj2 to same type
	// 3.	prop data   compare data    get obj1, coerce obj2 to same type
	//
	// 4.	obj  prop   compare data    get obj2, coerce obj1 to same type
	// 5.	obj  obj    compare tokens
	// 6.	obj  data   compare data    coerce obj1 to type of data
	//
	// 7.	data prop   compare data    get obj2, coerce obj1 to same type
	// 8.	data obj    compare data    coerce obj2 to type of data
	// 9.	data data   compare data    coerce obj2 to type of data
	
	bool			desc1obj, desc2obj, desc1prop, desc2prop, result;
	AEDescriptor	data1, data2;
	
	GetComparisonData(inTokenDesc1, data1, desc1obj, desc1prop);
	GetComparisonData(inTokenDesc2, data2, desc2obj, desc2prop);
	
	// Now we know whether we’re comparing tokens or data.  Let’s go do it.
	
	if (!(desc1obj && desc2obj))
	{
		// This routine is a sub-routine of MOSLCompareProc.  It is called when
		// MOSLCompareProc has determined that the objects to compare are not
		// tokens.  In that case, MOSLCompareProc resolves both objects and
		// if either are properties, gets the data for the properties, eventually
		// boiling everything down to raw data.  Once it has raw data, it
		// calls this routine to compare the data.  This routine knows how to
		// compare all of the data types that are likely to be obtained as
		// the value of properties.  It omits some of the more complex types
		// because a) OSL should be doing this stuff for us anyway, b)
		// the accepted workaround for script developers is to get the data
		// and then do the comparison in AppleScript, and c) it’s a bunch of
		// work.
		
		if (desc1prop)
		{
			// items 1, 2, 3
			CoerceDesc(data2, data1.GetType(), data2);
		}
		else if (desc2prop)
		{
			// items 4, 7
			CoerceDesc(data1, data2.GetType(), data1);
		}
		else if (desc1obj)
		{
			// items 6
			CoerceDesc(data1, data2.GetType(), data1);
		}
		else
		{
			// items 8, 9
			CoerceDesc(data2, data1.GetType(), data2);
		}
		
		ComparerMap::const_iterator	it	= mComparerMap.find(data1.GetType());
		
		if (it == mComparerMap.end())
			B_THROW(AECoercionFailException());
		
		result = it->second(inOperator, data1, data2);
	}
	else
	{
		// item 5
		result = AEObject::CompareObjects(inOperator, 
										  AEToken(inTokenDesc1).GetObject(), 
										  AEToken(inTokenDesc2).GetObject());
	}
	
	return result;
	
#if 0
	OSStatus 		 err;
	AEDesc 			 obj1;
	AEDesc 			 obj2;
	MOSLToken		 tok1;
	MOSLToken		 tok2;
	MOSLToken		 baseTok1;
	MOSLToken		 baseTok2;
	AEDesc 			 data1;
	AEDesc 			 data2;
	ComparisonMethod howToCompare;
	MOSLClassIndex 	 junkClass;

	assert(obj1Param != NULL);
	assert(obj2Param != NULL);
	assert(result    != NULL);
	
	// Technote 1095 says that we may need to do this because older OSLs might
	// pass the address of an AEDesc that’s in an unlocked handle.
	
	obj1 = *obj1Param;
	obj2 = *obj2Param;
	
	if (gDebugFlags & kMOSLLogOSLMask) {
		BBLogLine("\pMOSLCompareProc");
		BBLogIndent();
		BBLogDescType("\poper", oper);
		BBLogDesc("\pobj1", &obj1);
		BBLogDesc("\pobj2", &obj2);
	}

	MoreAENullDesc(&data1);
	MoreAENullDesc(&data2);

	// Each side of the comparison is either property, object descriptor, or data.
	// Following table explains what we do in each case.

	// item obj1 obj2   what to do
	// ---- ---- ----   ----------
	// 1.	prop prop   compare data	get obj1, coerce obj2 to same type
	// 2.	prop obj    compare data    get obj1, coerce obj2 to same type
	// 3.	prop data   compare data    get obj1, coerce obj2 to same type
	//
	// 4.	obj  prop   compare data    get obj2, coerce obj1 to same type
	// 5.	obj  obj    compare tokens
	// 6.	obj  data   compare data    coerce obj1 to type of data
	//
	// 7.	data prop   compare data    get obj2, coerce obj1 to same type
	// 8.	data obj    compare data    coerce obj2 to type of data
	// 9.	data data   compare data    coerce obj2 to type of data
	
	if (obj1.descriptorType == typeProperty) {
		// items 1 through 3
		howToCompare = kCompareData;
		err = MOSLCoerceObjDesc(&obj1, typeWildCard, &data1);
		if (err == noErr) {
			err = MOSLCoerceObjDesc(&obj2, data1.descriptorType, &data2);
		}
	} else if (obj2.descriptorType == typeProperty) {
		// items 4 and 7
		howToCompare = kCompareData;
		err = MOSLCoerceObjDesc(&obj2, typeWildCard, &data2);
		if (err == noErr) {
			err = MOSLCoerceObjDesc(&obj1, data2.descriptorType, &data1);
		}
	} else if (ClassIDToClassIndex(obj1.descriptorType, &junkClass) == noErr) {
		assert(obj2.descriptorType != typeProperty);
		if (ClassIDToClassIndex(obj2.descriptorType, &junkClass) == noErr) {
			// item 5
			howToCompare = kCompareTokens;
			err = noErr;
		} else {
			// item 6
			howToCompare = kCompareData;
			err = AEDuplicateDesc(&obj2, &data2);
			if (err == noErr) {
				err = MOSLCoerceObjDesc(&obj1, data2.descriptorType, &data1);
			}
		}
	} else {
		// items 8 and 9
		howToCompare = kCompareData;
		err = AEDuplicateDesc(&obj1, &data1);
		if (err == noErr) {
			err = MOSLCoerceObjDesc(&obj2, data1.descriptorType, &data2);
		}
	}

	// Now we know whether we’re comparison tokes or data.  Let’s go do it.
	
	if (err == noErr) {
		switch (howToCompare) {
			case kCompareTokens:
				DescToMOSLToken(&obj1, &tok1);
				DescToMOSLToken(&obj2, &tok2);
				
				err = CallClassCoerceToken(&tok1, cObject, &baseTok1);
				if (err == noErr) {
					err = CallClassCoerceToken(&tok2, cObject, &baseTok2);
				}
				if (err == noErr && baseTok1.tokType != baseTok2.tokType) {
					err = errAECoercionFail;
				}
				if (err == noErr) {
					err = CompareMOSLTokens(oper, &baseTok1, &baseTok2, result);
				}
				break;
			case kCompareData:
				err = CompareDataDescriptors(oper, &data1, &data2, result);
				break;
			default:
				assert(false);
				break;
		}
	}		

	MoreAEDisposeDesc(&data1);
	MoreAEDisposeDesc(&data2);

	if (gDebugFlags & kMOSLLogOSLMask) {
		BBLogLong("\p<result", *result);
		BBLogOutdentWithErr(err);
	}
	
	return err;
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::GetComparisonData(
	const AEDesc&		inTokenDesc, 
	AEDesc&				outDataDesc,
	bool&				outIsObject,
	bool&				outIsProperty)
{
	if (AEToken::IsTokenDescriptor(inTokenDesc))
	{
		AEToken		token(inTokenDesc);
		AEWriter	writer;
		
		outIsProperty = token.IsPropertyToken();
		outIsObject = !outIsProperty;
		
		AEObject::WriteTokenSpecifier(token, writer);
		writer.Close(outDataDesc);
	}
	else
	{
		OSStatus	err;
		
		outIsObject = outIsProperty = false;
		
		err = AEDuplicateDesc(&inTokenDesc, &outDataDesc);
		B_THROW_IF_STATUS(err);
	}
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::CompareTypeData(
	DescType			inOperator,
	DescType			inType1,
	DescType			inType2)
{
	bool	result	= false;
	
	switch (inOperator)
	{
	case kAEEquals:
		result = (inType1 == inType2);
		break;
		
	case kAEGreaterThanEquals:
	case kAEGreaterThan:
	case kAELessThan:
	case kAELessThanEquals:
	case kAEBeginsWith:
	case kAEEndsWith:
	case kAEContains:
		B_THROW(AECantRelateObjectsException());
		break;
	
	default:
		B_THROW(AEUnrecognisedOperatorException());
		break;
	}
	
	return result;
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::CompareBooleanData(
	DescType			inOperator,
	const AEDesc&		inData1,
	const AEDesc&		inData2)
{
	// Boolean stuff.  Unlike Pascal and C, AppleScript doesn’t support
	// relating Booleans, so we don’t support it here.

	bool	b1, b2;
	bool	result	= false;
	
	DescParam<typeBoolean>::Get(inData1, b1);
	DescParam<typeBoolean>::Get(inData2, b2);
	
	switch (inOperator)
	{
	case kAEEquals:
		result = (b1 == b2);
		break;
		
	case kAEGreaterThanEquals:
	case kAEGreaterThan:
	case kAELessThan:
	case kAELessThanEquals:
	case kAEBeginsWith:
	case kAEEndsWith:
	case kAEContains:
		B_THROW(AECantRelateObjectsException());
		break;
	
	default:
		B_THROW(AEUnrecognisedOperatorException());
		break;
	}
	
	return result;
}

// ------------------------------------------------------------------------------------------
// This routine compares two binary objects for equality.  It’s called
// by CompareDataDescriptors (which is in turn called by MOSLCompareProc)
// when certain types of binary data are encountered (for example, typeQDPoint).
bool
AEObjectSupport::CompareBinaryData(
	DescType			inOperator,
	const AEDesc&		inData1,
	const AEDesc&		inData2)
{
	// Extract the binary data into handles, then compare the handles.

	bool	result	= false;
	
	switch (inOperator)
	{
	case kAEEquals:
		{
			size_t	size	= static_cast<size_t>(AEGetDescDataSize(&inData1));
			
			result = (size == static_cast<size_t>(AEGetDescDataSize(&inData2)));
			
			if (result && (size > 0))
			{
				std::vector<char>	v1(size), v2(size);
				OSStatus			err;
				
				err = AEGetDescData(&inData1, &v1[0], size);
				B_THROW_IF_STATUS(err);
				
				err = AEGetDescData(&inData2, &v2[0], size);
				B_THROW_IF_STATUS(err);
				
				result = (memcmp(&v1[0], &v2[0], size) == 0);
			}
		}
		break;
		
	case kAEGreaterThanEquals:
	case kAEGreaterThan:
	case kAELessThan:
	case kAELessThanEquals:
	case kAEBeginsWith:
	case kAEEndsWith:
	case kAEContains:
		B_THROW(AECantRelateObjectsException());
		break;
	
	default:
		B_THROW(AEUnrecognisedOperatorException());
		break;
	}
	
	return result;
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::CompareStringData(
	DescType			inOperator,
	const AEDesc&		inData1,
	const AEDesc&		inData2)
{
	String	s1, s2;
	
	DescParam<typeUTF16ExternalRepresentation>::Get(inData1, s1);
	DescParam<typeUTF16ExternalRepresentation>::Get(inData2, s2);
	
	return CompareStrings(inOperator, s1, s2);
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::CompareStrings(
	const String&	inString1, 
	const String&	inString2)
{
	return (CompareStrings(kAEEquals, inString1, inString2));
}

// ------------------------------------------------------------------------------------------
/*!	The strings are compared in an AppleScript-like fashion (case-insensitive, localised, etc).
*/
bool
AEObjectSupport::CompareStrings(
	DescType		inComparisonOp,
	const String&	inString1, 
	const String&	inString2)
{
	const OptionBits	kFlags	= kCFCompareCaseInsensitive | 
								  kCFCompareNonliteral | 
								  kCFCompareLocalized;
	CFStringRef	str1	= inString1.cf_ref();
	CFStringRef	str2	= inString2.cf_ref();
	CFRange		range, foundRange;
	bool		result;
	
	switch (inComparisonOp)
	{
	case kAEEquals:
		result = (CFStringCompare(str1, str2, kFlags) == 0);
		break;
		
	case kAEGreaterThanEquals:
		result = (CFStringCompare(str1, str2, kFlags) >= 0);
		break;
		
	case kAEGreaterThan:
		result = (CFStringCompare(str1, str2, kFlags) > 0);
		break;
		
	case kAELessThan:
		result = (CFStringCompare(str1, str2, kFlags) < 0);
		break;
		
	case kAELessThanEquals:
		result = (CFStringCompare(str1, str2, kFlags) <= 0);
		break;
		
	case kAEBeginsWith:
		range = CFRangeMake(0, CFStringGetLength(str1));
		result = CFStringFindWithOptions(str1, str2, range, kFlags, &foundRange);
		if (result)
			result = (foundRange.location == 0);
		break;
		
	case kAEEndsWith:
		range = CFRangeMake(0, CFStringGetLength(str1));
		result = CFStringFindWithOptions(str1, str2, range, kFlags | kCFCompareBackwards, 
										 &foundRange);
		if (result)
			result = ((foundRange.location + foundRange.length) == range.length);
		break;
		
	case kAEContains:
		range = CFRangeMake(0, CFStringGetLength(str1));
		result = CFStringFindWithOptions(str1, str2, range, kFlags, NULL);
		break;
		
	default:
		result = false;
		B_THROW(AEUnrecognisedOperatorException());
		break;
	}
	
	return (result);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLCountProc(
	DescType			desiredType, 
	DescType			containerClass, 
	const AEDesc*		container, 
	long*				result)
{
	OSStatus	err	= noErr;
	
	try
	{
		DebugPrint("OSLCountProc", container);
		
		sAEObjectSupport->ClearException();
		
		if (containerClass == typeNull)
			containerClass = cApplication;
		
		const AEToken	token(*container);
		
		*result = sAEObjectSupport->HandleCount(desiredType, containerClass, token);
	}
	catch (const std::exception& ex)
	{
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
size_t
AEObjectSupport::HandleCount(
	DescType			inDesiredType, 
	DescType			inContainerClass, 
	const AEToken&		inContainer) const
{
	AEObjectPtr	obj	= inContainer.GetObject();
	
	B_ASSERT(obj->GetClassID() == inContainerClass);
	
	return obj->CountElements(inDesiredType);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLDisposeTokenProc(
	AEDesc*				unneededToken)
{
#if 0
	DebugPrint("OSLDisposeTokenProc", unneededToken);
#endif
	
	return AEToken::Dispose(*unneededToken);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLGetMarkTokenProc(
	const AEDesc*		containerToken,
	DescType			/* containerClass */,
	AEDesc*				/* markToken */)
{
	OSStatus	err	= noErr;
	
	try
	{
		DebugPrint("OSLGetMarkTokenProc", containerToken);
		
		sAEObjectSupport->ClearException();
		
		// do stuff

		err = errAEEventNotHandled;
	}
	catch (const std::exception& ex)
	{
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLMarkProc(
	const AEDesc*		dataToken,
	const AEDesc*		/* markToken */,
	long				/* index */)
{
	OSStatus	err	= noErr;
	
	try
	{
		DebugPrint("OSLMarkProc", dataToken);
		
		sAEObjectSupport->ClearException();
		
		// do stuff

		err = errAEEventNotHandled;
	}
	catch (const std::exception& ex)
	{
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLAdjustMarksProc(
	long				/* newStart */,
	long				/* newStop */,
	const AEDesc*		markToken)
{
	OSStatus	err	= noErr;
	
	try
	{
		DebugPrint("OSLAdjustMarksProc", markToken);
		
		sAEObjectSupport->ClearException();
		
		// do stuff

		err = errAEEventNotHandled;
	}
	catch (const std::exception& ex)
	{
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::OSLGetErrDescProc(
	AEDesc**	appDesc)
{
	ExInfo*	info	= sAEObjectSupport->mExInfoPtr.get();
	
	if ((info == NULL) && (info->mErrorDescLink != NULL))
	{
		*appDesc = info->mErrorDescLink->GetErrorDesc();
	}
	else
	{
		*appDesc = sNullDescriptor;
	}
	
	AEDisposeDesc(*appDesc);
	
	return (noErr);
}

// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::AEEventHandlerProc(
	const AppleEvent*	theAppleEvent, 
	AppleEvent*			reply, 
	long				handlerRefcon)
{
	OSStatus		err	= noErr;
#if 0
	EventHookLink	eventHook(*sAEObjectSupport, *theAppleEvent, reply);
#endif

	try
	{
		B_ASSERT(theAppleEvent != NULL);
		B_ASSERT(reply != NULL);
		
		sAEObjectSupport->ClearException();
		
		// First check that we weren’t called for some event that’s not in the
		// event table.  This shouldn’t happen (hence the assert above), but
		// we don’t want to do bogus things if it does.
		
		const EventKeyVector&	eventKeys	= sAEObjectSupport->mEventKeys;
		
		if ((handlerRefcon < 0) || (static_cast<size_t>(handlerRefcon) >= eventKeys.size()))
			B_THROW(ConstantOSStatusException<errAEEventNotHandled>());
		
		AEInfo::EventKey	key	= eventKeys[handlerRefcon];
	
		sAEObjectSupport->HandleAppleEvent(key, *theAppleEvent, *reply);
		
#if 0
		eventHook.Commit();
#endif
	}
	catch (const std::exception& ex)
	{
		err = sAEObjectSupport->CacheExceptionInfo(ex);
	}
	catch (...)
	{
		err = errAEEventFailed;
	}
	
	return (err);
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::HandleAppleEvent(
	const AEInfo::EventKey&	inEventKey,
	const AppleEvent&		inEvent,
	AppleEvent&				outReply) const
{
	const AEInfo::EventInfo&	eventInfo	= mEventMap.find(inEventKey)->second;
	AEDescriptor				directObjectDesc;
	AEDescriptor				resultDesc;
	AEAutoTokenDescriptor		resolvedDesc;
	OSStatus					err;
	
	// Next extract and resolve the direct object.  If the event accepts
	// a bad direct object, swallow the error and continue with a null direct object.
	
	err = AEGetParamDesc(&inEvent, keyDirectObject, typeWildCard, directObjectDesc);
	if (err != errAEDescNotFound)
		B_THROW_IF_STATUS(err);
	
	if (!directObjectDesc.Empty())
	{
		if (eventInfo.mDOBehavior != AEInfo::kEventDOBadOK)
		{
			bool		listOpen	= false;
			AEWriter	writer;
			
			RecursiveResolve(directObjectDesc, writer, listOpen);
		
			if (listOpen)
				writer.CloseList();
			
			writer.Close(resolvedDesc);
		}
		else
		{
			// It's OK to have a bad direct object, so trap any exceptions that 
			// occur.
			
			try
			{
				AutoThrowHandler	autoThrow(true);
				bool				listOpen	= false;
				AEWriter			writer;
				
				RecursiveResolve(directObjectDesc, writer, listOpen);
				
				if (listOpen)
					writer.CloseList();
				
				writer.Close(resolvedDesc);
			}
			catch (std::exception& ex)
			{
				// The event definition says that invalid direct objects are 
				// acceptable, so swallow the exception.  We abandon whatever has 
				// been written to the writer object, and re-initialize the output 
				// descriptor to null.
				
				AEDisposeDesc(resolvedDesc);
			}
		}
	}

	// Handle the various direct object responses:
	//
	// * If the direct object is a list and this is the "count" event, forward the
	//   event direct to the PseudoCListCount routine.
	// * If the direct object is a list and this isn’t the "count", iterate
	//   through the list, repeatedly dispatching the event (DispatchEvent) to each item.
	// * If the direct object is a single object, just call DispatchEvent directly.
	
	if (resolvedDesc.GetType() == typeAEList)
	{
		AEWriter	resultItemWriter;
		
		{
			// Iterate through the list, extracting each element and dispatching
			// the event to that element.  Place the resulting data into list.
			
			AutoAEWriterList	autoList(resultItemWriter);
			
			std::for_each(MakeAEListBeginIterator(resolvedDesc), 
						  MakeAEListEndIterator(resolvedDesc), 
						  boost::bind(&AEObjectSupport::DispatchAppleEvent, 
									  this, boost::ref(eventInfo), boost::ref(inEvent), 
									  _1, boost::ref(resultItemWriter)));
		}
		
		resultItemWriter.Close(resultDesc);
		
		// Post-process the results list based on the specification for this
		// event in the event table.  There are three relevant cases:
		//
		// o kMOSLReturnNone -- If the event returns no data and the results
		//   list is empty, we make sure to return a typeNull result rather
		//   than an empty typeAEList.
		//
		// o kMOSLReturnDefault -- This is the standard action; we just return
		//   the list of results unmodified.
		//
		// o kMOSLReturnCollapseBooleanList -- This is typically set for the
		//   "exists" event and our response is to call ProcessExistsResult,
		//   which collapses a list of Booleans into a single Boolean by
		//   ANDing them together.
		
		switch (eventInfo.mResultAction)
		{
		case AEInfo::kEventResultActionNone:
			if (MakeAEListEndIterator(resultDesc).size() == 0)
				resultDesc.Clear();
			break;
			
		case AEInfo::kEventResultActionCountList:
			ResultListCount(inEvent, resultDesc, resultDesc);
			break;
			
		case AEInfo::kEventResultActionCollapseBooleanList:
			ResultListCollapse(inEvent, resultDesc, resultDesc);
			break;
		
		case AEInfo::kEventResultActionDefault:
			// do nothing
			break;
			
		default:
			B_THROW(AEEventFailedException());
			break;
		}
	}
	
#if 0
	if ((resolvedDirObj.descriptorType == typeAEList) && (gEventTable[eventIndex].resultAction == kMOSLReturnCountList))
	{
		err = PseudoCListCount(&resolvedDirObj, theEvent, &result);
	}
	else if (resolvedDirObj.descriptorType == typeAEList)
	{
		// Iterate through the list, extracting each element and dispatching
		// the event to that element.  Place the resulting data into list.
		
		err = AECountItems(&resolvedDirObj, &elementCount);
		if (err == noErr) {
			err = AECreateList(NULL, 0, false, &result);
		}
		if (err == noErr) {
			for (elementIndex = 1; elementIndex <= elementCount; elementIndex++) {
				MoreAENullDesc(&thisElement);
				MoreAENullDesc(&thisResult);

				err = AEGetNthDesc(&resolvedDirObj, elementIndex, typeWildCard, &junkKeyword, &thisElement);
				if (err == noErr) {
					err = DispatchEvent(theEvent, eventIndex, &thisElement, &thisResult);
				}
				if (err == noErr) {
					// Put the result into the resulting list, except if the event indicates 
					// that it doesn’t return any result.  For events with no result, we 
					// just leave the list empty so that the clean up code (below) can
					// ensure that the entire result for the list is typeNull rather than
					// typeAEList with no elements.
					if ((thisResult.descriptorType != typeNull) || (gEventTable[eventIndex].resultAction != kMOSLReturnNone)) {
						err = AEPutDesc(&result, 0, &thisResult);
					}
				}
				
				// Clean up.
				
				MoreAEDisposeDesc(&thisElement);
				MoreAEDisposeDesc(&thisResult);
				if (err != noErr) {
					break;
				}
			}
		}
		
		// Post-process the results list based on the specification for this
		// event in the event table.  There are three relevant cases:
		//
		// o kMOSLReturnNone -- If the event returns no data and the results
		//   list is empty, we make sure to return a typeNull result rather
		//   than an empty typeAEList.
		//
		// o kMOSLReturnDefault -- This is the standard action; we just return
		//   the list of results unmodified.
		//
		// o kMOSLReturnCollapseBooleanList -- This is typically set for the
		//   "exists" event and our response is to call ProcessExistsResult,
		//   which collapses a list of Booleans into a single Boolean by
		//   ANDing them together.
		
		if (err == noErr) {
			switch (gEventTable[eventIndex].resultAction) {
				case kMOSLReturnNone:
					err = AECountItems(&result, &elementCount);
					if ((err == noErr) && (elementCount == 0)) {
						MoreAEDisposeDesc(&result);
						assert(result.descriptorType == typeNull);
					}
					break;
				case kMOSLReturnDefault:
					// do nothing
					break;
				case kMOSLReturnCollapseBooleanList:
					err = ProcessExistsResult(&result);
					break;
				// kMOSLReturnCountList goes through otherwise branch
				default:
					assert(false);
					err = errAEEventFailed;
					break;
			}
		}
	}
#endif
	else
	{
		// The direct object is a single item.  Just send the event to the object.
		
		AEWriter	resultWriter;
		
		DispatchAppleEvent(eventInfo, inEvent, resolvedDesc, resultWriter);
		resultWriter.Close(resultDesc);
	}

	// Clean up.
	
	if (outReply.descriptorType != typeNull)
	{
		// Put the result into the reply Apple event.  We don’t do this if
		// the result is null and the event is known not to produce a result;
		// without this check the Script Editor shows "current application"
		// as the result of these "no result" Apple events.
		
#if 0
		DebugPrint("HandleAppleEvent result", resultDesc);
#endif
		
		if ((eventInfo.mResultAction != AEInfo::kEventResultActionNone) || 
			(resultDesc.GetType() != typeNull))
		{
			err = AEPutParamDesc(&outReply, keyAEResult, resultDesc);
			B_THROW_IF_STATUS(err);
		}

#if 0
		if (err != noErr) {
			PutErrorIntoReply(theReply, err);
		}
#endif
	}
}

// ------------------------------------------------------------------------------------------
// This routine is called by the primary Apple event dispatcher
// (MOSLAppleEventHandler) to resolve the direct object of an event.
// It’s recursive because the direct object might be a list of tokens.
// If it is, we want to resolve any tokens within that list.  We
// do that by iterating through the list calling ourself on each element.
// Note that, if the result of this recursive call is a list, we
// append the result to our output list rather than embedding it in
// the list.  This ensures that the resolvedObj that we eventually pass
// back to the caller is always either a single object or a flat
// list of objects.
void
AEObjectSupport::RecursiveResolve(
	const AEDesc&		inObjectSpecifier,
	AEWriter&			ioTokenWriter, 
	bool&				ioListOpen) const
{
#if 0
	OSStatus  err;
	SInt32    elementCount;
	SInt32    elementIndex;
	AEDesc    thisObj;
	AEDesc    thisResolvedObj;
	AEKeyword junkKeyword;

	assert(obj         != NULL);
	assert(resolvedObj != NULL);
	
	if (gDebugFlags & kMOSLLogDispatchMask) {
		BBLogLine("\pRecursiveResolve");
		BBLogIndent();
		BBLogDesc("\pobj", obj);
	}

	MoreAENullDesc(resolvedObj);
#endif
	
	OSStatus	err;
	
	// If obj is a list, iterate through the list calling ourself and adding
	// the results to our output list.
	
	if (inObjectSpecifier.descriptorType == typeAEList)
	{
		std::for_each(MakeAEListBeginIterator(inObjectSpecifier), 
					  MakeAEListEndIterator(inObjectSpecifier), 
					  boost::bind(&AEObjectSupport::RecursiveResolveListItem, 
								  this, _1, boost::ref(ioTokenWriter), 
								  boost::ref(ioListOpen)));
		
#if 0
		err = AECountItems(obj, &elementCount);
		if (err == noErr) {
			err = AECreateList(NULL, 0, false, resolvedObj);
		}
		if (err == noErr) {
			for (elementIndex = 1; elementIndex <= elementCount; elementIndex++) {
				MoreAENullDesc(&thisObj);
				MoreAENullDesc(&thisResolvedObj);
				
				err = AEGetNthDesc(obj, elementIndex, typeWildCard, &junkKeyword, &thisObj);
				if (err == noErr) {
					err = RecursiveResolve(&thisObj, &thisResolvedObj);
				}
				if (err == noErr) {
					if (thisResolvedObj.descriptorType == typeAEList) {
						err = MoreAEAppendListToList(&thisResolvedObj, resolvedObj);
					} else {
						err = AEPutDesc(resolvedObj, 0, &thisResolvedObj);
					}
				}
				
				MoreAEDisposeDesc(&thisObj);
				MoreAEDisposeDesc(&thisResolvedObj);
				if (err != noErr) {
					break;
				}
			}
		}
#endif
	}
	else
	{
		// Otherwise just try to resolve obj.  If obj isn’t an object,
		// we just duplicate it into the output so that things like
		// lists of aliases work correctly.
		//
		// IMPORTANT:  The result from AEResolve is always either a single
		// object or a flat list of objects because the PseudoCListAccessor
		// will always append lists to its output rather than just inserting them.
		// This, combined with the list append in this routine, ensures that
		// the result from RecursiveResolve is always either a single object
		// or a flat list of objects.
		
		AEDescriptor	tokenDesc, errorDesc;
		
		err = PrivateResolve(inObjectSpecifier, tokenDesc, errorDesc);
		
		switch (err)
		{
		case noErr:
			ioTokenWriter.WriteDesc(tokenDesc);
			break;
			
		case errAENotAnObjSpec:
			ioTokenWriter.WriteDesc(inObjectSpecifier);
			break;
			
		default:
			B_THROW(ObjectResolutionException(err, errorDesc));
			break;
		}
	}
	
#if 0
	if (err != noErr) {
		MoreAEDisposeDesc(resolvedObj);
	}
	
	if (gDebugFlags & kMOSLLogDispatchMask) {
		BBLogDesc("\p<resolvedObj", resolvedObj);
		BBLogOutdentWithErr(err);
	}

	return err;
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::RecursiveResolveListItem(
	const AEDesc&		inItemDesc,
	AEWriter&			ioTokenWriter, 
	bool&				ioListOpen) const
{
	if (!ioListOpen)
	{
		ioTokenWriter.OpenList();
		ioListOpen = true;
	}
	
	RecursiveResolve(inItemDesc, ioTokenWriter, ioListOpen);
}

// ------------------------------------------------------------------------------------------
OSStatus
AEObjectSupport::PrivateResolve(
	const AEDesc&		inObjectSpecifier,
	AEDesc&				outTokenDesc,
	AEDesc&				outErrorDesc,
	short				inFlags /* = kAEIDoMinimum */)
{
	OSStatus	err	= noErr;
	
	if (outTokenDesc.descriptorType != typeNull)
		err = AEDisposeToken(&outTokenDesc);
	
	if (err == noErr)
	{
		ErrorDescLink	errorDesc(*sAEObjectSupport);
		
		err = AEResolve(&inObjectSpecifier, inFlags, &outTokenDesc);
		
		errorDesc.SwapErrorDesc(outErrorDesc);
	}
	
	return err;
}

// The Apple event dispatch mechanism is based on the "class first" approach.
// When the client initialises MOSL, we install an event handler for all of 
// the events in the client’s event table.  In fact, we install the same
// event handler (MOSLAppleEventHandler) for each of these events.  When
// that routine runs, it attempts to resolve the direct object for the event.
// The direct object resolution yield one of 4 results:
//
// 1. single token of one of our classes --  We then dispatch the event
//    to that class’s event handler.  If we can’t find the class in the class
//    table (for example, if you don’t have a class for typeAlias and this
//    is on "open documents" event), we dispatch the event to the gDefaultHandler.
// 2. list of tokens for our classes -- In response, we iterate through
//    the list, dispatching each elementing, and accumulating the responses
//    in a results list.
// 3. no direct object -- If the event accepts no direct object (indicated
//    by a field in the event table entry for that event), we dispatch
//    the event to the cApplication class’s handler.
// 4. an error -- If the event accepts bad direct objects (indicated
//    by a field in the event table entry for that event; this is typically
//    only the case for the "exists" event), we dispatch the event to the 
//    cApplication class’s handler.

// ------------------------------------------------------------------------------------------
// This routine is a sub-routine of the main Apple event dispatcher
// (MOSLAppleEventHandler).  Once MOSLAppleEventHandler has resolved
// the direct object, it calls this routine to actually dispatch an
// event to the class of that object.
void
AEObjectSupport::DispatchAppleEvent(
	const AEInfo::EventInfo&	inEventInfo,
	const AppleEvent&			inEvent,
	const AEDesc&				inDirectObjectDesc, 
	AEWriter&					ioResultWriter) const
{
#if 0
	OSStatus 			  err;
	AEEventClass 		  dirObjClass;
	MOSLToken 			  dirObjTok;
	MOSLClassIndex 		  classIndex;
	MOSLClassEventHandler handler;

	assert(theEvent       != NULL);
	assert(eventIndex     <  gEventTableSize);
	assert(resolvedDirObj != NULL);
	assert(result         != NULL);

	if (gDebugFlags & kMOSLLogDispatchMask) {
		BBLogLine("\pDispatchEvent");
		BBLogIndent();
		BBLogAppleEvent("\ptheEvent", theEvent);
		BBLogLong("\peventIndex", eventIndex);
		BBLogDesc("\presolvedDirObj", resolvedDirObj);
	}

	MoreAENullDesc(result);
	
	err = noErr;
#endif
	
	// We now have a single resolved direct obj (in resolvedDirObj)
	// that either contains data or a token.  The descriptor type
	// is either typeNull (no direct object), cApplication (a
	// root application object token), a real token, or some class
	// we’ve not heard of.  We use the dirObjReq field of the event
	// table to make a first check of the direct object.
	
	B_ASSERT(inDirectObjectDesc.descriptorType != typeAEList);

	switch (inEventInfo.mDOBehavior)
	{
	case AEInfo::kEventDOBadOK:
	case AEInfo::kEventDOOptional:
		// do nothing
		break;
		
	case AEInfo::kEventDORequired:
		if (inDirectObjectDesc.descriptorType == typeNull)
			B_THROW(AEDirectObjectRequiredException());
		break;
		
	case AEInfo::kEventDOIllegal:
		if (inDirectObjectDesc.descriptorType != typeNull)
			B_THROW(AEDirectObjectNotAllowedException());
		break;
		
	default:
		B_ASSERT(false);
		break;
	}

	// Now we dispatch the event.  We start by determining the class
	// of the direct object.  
	
#if 0
	//This is somewhat tricky because the
	// descriptorType for a property token is always typeProperty,
	// and but we want to dispatch events for properties to the
	// class for the object containing the property.
#endif
	
#if 0
	if (resolvedDirObj->descriptorType == typeProperty) {
		DescToMOSLToken(resolvedDirObj, &dirObjTok);
		dirObjClass = dirObjTok.tokObjType;
	} else {
		dirObjClass = resolvedDirObj->descriptorType;
		if (dirObjClass == typeNull) {
			dirObjClass = cApplication;
		}
	}
#endif

	bool	handled		= false;
	
	if (AEToken::IsTokenDescriptor(inDirectObjectDesc))
	{
		AEToken		directObjectToken(inDirectObjectDesc);
		DescType	directObjectClass	= directObjectToken.GetObjectClassID();
		
		// Now we look up the class in our class table.  If we find
		// it, we call the appropriate event handler in the class’s
		// event handler table.  If we don’t find it, we send the event
		// through to the client’s default handler.
		
		const AEInfo::ClassInfo*	classInfo	= FindClassInfo(directObjectClass);
		
		if (classInfo != NULL)
		{
			// We found the class.
			
			AEInfo::ClassEventHandler	handler	= FindClassEventHandler(*classInfo, inEventInfo.mEventKey);
			
			if (handler != NULL)
			{
				handler(directObjectToken, inEvent, ioResultWriter);
				handled = true;
			}
		}
	}
	
	if (!handled)
	{
		// We couldn't find a class handler for this event, so attempt to use 
		// event's default handler, if there is one.
		
		if (inEventInfo.mDefaultHandler != NULL)
		{
			inEventInfo.mDefaultHandler(inDirectObjectDesc, inEvent, ioResultWriter);
		}
	}
	
#if 0
	err = ClassIDToClassIndex(dirObjClass, &classIndex);
	if (err == noErr) {
		handler = gClassTable[classIndex].eventHandlers[eventIndex];
		if (handler == NULL) {
			err = errAECantHandleClass;
		} else {
			DescToMOSLToken(resolvedDirObj, &dirObjTok);
			err = handler(&dirObjTok, theEvent, result);
		}
	}
	else if (err == errAECantHandleClass)
	{
		// OK, this is a bit of a hack, but it’s relatively clean so I decided
		// not to employ a more sophisticated solution.  When we resolve object
		// references on lists (for example, "node 1 of every document"), we can
		// run into a situation where some of the elements in the list don’t have
		// the appropriate value.  In that case, the PseudoCListAccessor routine
		// fills out the entry in the resulting list with the special "missing value"
		// value.
		//
		// The class of that value is typeType, which isn’t in our class
		// table.  So our default action would be to pass the value along to the
		// application’s default handler.  But the application didn’t generate
		// these values, so it’s kinda lame to offload them to the application.
		// So we special case the "get data" event on "missing value" and return
		// missing value.  This allows the "get data" event to succeed where
		// the direct object is a list containing missing values.
		//
		// After implementing this, I actually generalised it to support the "get data"
		// event on any data object that resolves but isn’t in the class table.
		// This makes constructs like the following work.
		//
		// 		tell application "TestMoreOSL"
		//			set x to "Macintosh HD:Test File"
		//			set y to file x
		//		end tell
		
		if (gEventTable[eventIndex].classID == kAECoreSuite
				&& gEventTable[eventIndex].eventID == kAEGetData) {
			err = AEDuplicateDesc(resolvedDirObj, result);
		} else {
			// Dispatch events whose direct object is an unknown class 
			// to the default application handler.
			if (gDefaultHandler == NULL) {
				err = errAEEventNotHandled;
			} else {
				err = gDefaultHandler(resolvedDirObj, theEvent, eventIndex, result);
			}
		}
	}

	if (gDebugFlags & kMOSLLogDispatchMask) {
		BBLogDesc("\p<result", result);
		BBLogOutdentWithErr(err);	
	}
	
	return err;
#endif
}

// ------------------------------------------------------------------------------------------
// This routine is called out of the primary Apple event dispatcher
// when we detect a "count" event whose direct object is a list.
// I’ve placed the routine here so as to group it with the other code
// that has to special case typeAEList within MOSL.  If we didn’t have
// this routine, "count every document" would return a list of
// N elements, where N is the number of documents, each element being 1.
// Instead, this routine just counts the elements of the list and returns
// a single result being that count.
//
// This code has a lot in common with MOSLGeneralCount, but it’s hard
// to combine the two because MOSLGeneralCount works in terms of tokens
// and we don’t have a token to represent an Apple Event Manager list.
void
AEObjectSupport::ResultListCount(
	const AppleEvent&	/* inEvent */, 
	const AEDescList&	inList, 
	AEDesc&				outResultDesc) const
{
	// If we’re not counting cObject, then the number of elements in the list
	// is not the right answer.  We have to iterate through all the list elements
	// seeing which ones are of the right type.  If the element’s type is in
	// our class table, we do type equality using the "coerceToken" object primitive.
	
	AEDescriptor	countDesc;
	SInt32			count	= 0;
	OSStatus		err;
	
	std::for_each(MakeAEListBeginIterator(inList), 
				  MakeAEListEndIterator(inList), 
				  boost::bind(&AEObjectSupport::ResultListCountItem, 
							  this, _1, boost::ref(count)));
	
	err = AECreateDesc(typeSInt32, &count, sizeof(count), countDesc);
	B_THROW_IF_STATUS(err);
	
	std::swap(outResultDesc, static_cast<AEDesc&>(countDesc));
	
#if 0
	OSStatus  err;
	AEDesc    typeOfElementToCountDesc;
	DescType  typeOfElementToCount;
	SInt32    elementCount;
	SInt32    elementIndex;
	SInt32    countResult;
	AEKeyword junkKeyword;
	AEDesc    thisElement;
	MOSLClassIndex junkClass;

	assert(dirObj   != NULL);
	assert(dirObj->descriptorType == typeAEList);
	assert(theEvent != NULL);
	assert(result   != NULL);
	
	if (gDebugFlags & kMOSLLogOSLMask) {
		BBLogLine("\pPseudoCListCount");
		BBLogIndent();
		BBLogDesc("\pdirObj", dirObj);
		BBLogAppleEvent("\ptheEvent", theEvent);
	}

	MoreAENullDesc(result);
	MoreAENullDesc(&typeOfElementToCountDesc);

	// Extract the type of element we’re supposed to be counting.
	
	err = AEGetParamDesc(theEvent, keyAEObjectClass, typeWildCard, &typeOfElementToCountDesc);
	if (err == noErr) {
		err = MOSLCoerceObjDescToPtr(&typeOfElementToCountDesc, typeType, &typeOfElementToCount, sizeof(typeOfElementToCount));
	} else if (err == errAEDescNotFound) {
		typeOfElementToCount = cObject;
		err = noErr;
	}
	if (err == noErr) {
		if (gDebugFlags & kMOSLLogOSLMask) {
			BBLogDescType("\ptypeOfElementToCount", typeOfElementToCount);
		}
		err = MoreAEGotRequiredParams(theEvent);
	}
	
	// Start by just counting the total number of elements in the list.
	
	if (err == noErr) {
		err = AECountItems(dirObj, &countResult);
	}
	
	// If we’re not counting cObject, then the number of elements in the list
	// is not the right answer.  We have to iterate through all the list elements
	// seeing which ones are of the right type.  If the element’s type is in
	// our class table, we do type equality using the "coerceToken" object primitive.
	
	if ((err == noErr) && (typeOfElementToCount != cObject)) {
		elementCount = countResult;
		countResult  = 0;
		for (elementIndex = 1; elementIndex <= elementCount; elementIndex++) {
			MoreAENullDesc(&thisElement);
			
			err = AEGetNthDesc(dirObj, elementIndex, typeWildCard, &junkKeyword, &thisElement);
			if (err == noErr) {
				if (ClassIDToClassIndex(thisElement.descriptorType, &junkClass) == noErr) {
					MOSLToken thisElementTok;

					DescToMOSLToken(&thisElement, &thisElementTok);
					if ( CallClassCoerceToken(&thisElementTok, typeOfElementToCount, NULL) == noErr ) {
						countResult += 1;
					}
				} else {
					if (thisElement.descriptorType == typeOfElementToCount) {
						countResult += 1;
					}
				}
			}
			
			MoreAEDisposeDesc(&thisElement);
			if (err != noErr) {
				break;
			}
		}
	}
	
	// Finally, place the count into the result descriptor.
	
	if (err == noErr) {
		err = AECreateDesc(typeLongInteger, &countResult, sizeof(countResult), result);
	}

	MoreAEDisposeDesc(&typeOfElementToCountDesc);
			
	if (gDebugFlags & kMOSLLogOSLMask) {
		BBLogDesc("\p<result", result);
		BBLogOutdentWithErr(err);
	}

	return err;
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::ResultListCountItem(
	const AEDesc&		inItemDesc,
	SInt32&				ioItemCount) const
{
	SInt32	count;
	
	DescParam<typeSInt32>::Get(inItemDesc, count);
	
	ioItemCount += count;
	
//	DescType	itemClass	= AEToken::GetObjectClassID(inItemDesc);
//	
//	if (DoesClassInheritFrom(itemClass, inElementType))
//		ioItemCount++;
	
#warning fix me
#if 0
	if (ClassIDToClassIndex(thisElement.descriptorType, &junkClass) == noErr) {
		MOSLToken thisElementTok;

		DescToMOSLToken(&thisElement, &thisElementTok);
		if ( CallClassCoerceToken(&thisElementTok, typeOfElementToCount, NULL) == noErr ) {
			countResult += 1;
		}
	} else {
		if (thisElement.descriptorType == typeOfElementToCount) {
			countResult += 1;
		}
	}
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::ResultListCollapse(
	const AppleEvent&	/* inEvent */, 
	const AEDescList&	inList, 
	AEDesc&				outResultDesc) const
{
	// Iterate through each item in the list.  For each item, extract
	// the item as a Boolean and ANDing it into accumulatedResult.
	
	AEListIterator	endIt(MakeAEListEndIterator(inList));
	Boolean			accumulatedResult;
	
	AEListIterator	it	= std::find_if(MakeAEListBeginIterator(inList), 
									   endIt, 
									   boost::bind(IsResultBooleanFalse, _1));
	
	accumulatedResult = (it == endIt);
	
	// Create the output descriptor and, if all goes well, dispose of the input
	// list and return the output single item.

	AEDescriptor	boolResult;
	OSStatus		err;
	
	err = AECreateDesc(typeBoolean, &accumulatedResult, 
					   sizeof(accumulatedResult), boolResult);
	B_THROW_IF_STATUS(err);
	
	std::swap(outResultDesc, static_cast<AEDesc&>(boolResult));
}

// ------------------------------------------------------------------------------------------
bool
AEObjectSupport::IsResultBooleanFalse(
	const AEDesc&		inDesc)
{
	const AEDesc*	descPtr	= &inDesc;
	AEDescriptor	coercedDesc;
	bool			value;
	OSStatus		err;
	
	switch (inDesc.descriptorType)
	{
	case typeTrue:
		value = true;
		break;
		
	case typeFalse:
		value = false;
		break;
	
	default:
		err = AECoerceDesc(descPtr, typeBoolean, coercedDesc);
		B_THROW_IF_STATUS(err);
		descPtr = coercedDesc;
		// fall through
		
	case typeBoolean:
		{
			Boolean	b;
			
			err = AEGetDescData(descPtr, &b, sizeof(b));
			B_THROW_IF_STATUS(err);
			
			value = b;
		}
		break;
	}
	
	return !value;
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::HandleDefaultEvent(
	const AEDesc&		/* inDirectObject */,
	const AppleEvent&	/* inEvent */,
	AEWriter&			/* ioResultWriter */) const
{
#warning fix me
	B_THROW_STATUS(errAEEventNotHandled);
}

#ifndef NDEBUG
// ------------------------------------------------------------------------------------------
pascal OSErr
AEObjectSupport::DebugOSLAccessorProc(
	DescType			desiredClass, 
	const AEDesc*		container, 
	DescType			containerClass, 
	DescType			form, 
	const AEDesc*		selectionData, 
	AEDesc*				/* value */, 
	long				accessorRefcon)
{
	std::cout << "DebugOSLAccessorProc\n";
	std::cout << "\tdesiredClass '" << make_string(desiredClass) << "'\n";
	std::cout << "\tcontainer->descriptorType '" << make_string(container->descriptorType) << "'\n";
	std::cout << "\tcontainerClass '" << make_string(containerClass) << "'\n";
	std::cout << "\tform '" << make_string(form) << "'\n";
	std::cout << "\tselectionData->descriptorType '" << make_string(selectionData->descriptorType) << "'\n";
	std::cout << "\taccessorRefcon " << accessorRefcon << "\n";
	
	return errAEAccessorNotFound;
}

// ------------------------------------------------------------------------------------------
void
AEObjectSupport::DebugPrint(
	const char*			heading, 
	const AEDesc*		desc)
{
	Handle		h;
	OSStatus	err;
	
	err = AEPrintDescToHandle(desc, &h);
	B_THROW_IF_STATUS(err);
	
	AutoMacHandle	autoH(h);
	AutoHandleLock	lockH(h);
	std::string		str(*h, GetHandleSize(h));
	
	printf("%s:\n%s\n", heading, str.c_str());
}
#endif


// ==========================================================================================
//	AEObjectSupport::ObjectResolutionException

#pragma mark AEObjectSupport::ObjectResolutionException

// ------------------------------------------------------------------------------------------
AEObjectSupport::ObjectResolutionException::ObjectResolutionException(
	const ObjectResolutionException& ex)
		: RuntimeOSStatusException(ex),
		  mErrorDesc(ex.mErrorDesc)
{
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::ObjectResolutionException::ObjectResolutionException(
	OSStatus		inStatus, 
	const AEDesc&	inErrorDesc)
		: RuntimeOSStatusException(inStatus)
{
	::Size		size	= AESizeOfFlattenedDesc(&inErrorDesc);
	OSStatus	err;
	
	mErrorDesc.resize(size);
	
	err = AEFlattenDesc(&inErrorDesc, reinterpret_cast<Ptr>(&mErrorDesc[0]), size, &size);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::ObjectResolutionException::ObjectResolutionException(
	std::istream&	istr)
		: RuntimeOSStatusException(istr)
{
	std::string	b64str;
	
	istr >> b64str;
	
	if (b64str.size() > 0)
	{
		BIO*	mem	= NULL;
		BIO*	b64	= NULL;
		
		try
		{
			// Create a memory buffer containing Base64 encoded string data
			mem = BIO_new_mem_buf(const_cast<char*>(b64str.c_str()), b64str.size());
			B_THROW_IF_NULL(mem);
		
			// Push a Base64 filter so that reading from the buffer decodes it
			b64 = BIO_new(BIO_f_base64());
			B_THROW_IF_NULL(b64);
			
			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
			mem = BIO_push(b64, mem);
			b64 = NULL;
			
			char	inbuf[512];
			int		inlen;
			
			while ((inlen = BIO_read(mem, inbuf, sizeof(inbuf))) > 0)
			{
				mErrorDesc.insert(mErrorDesc.end(), inbuf, inbuf + inlen);
			}

			// Clean up and go home
			BIO_free_all(mem);
		}
		catch (...)
		{
			if (b64 != NULL)
				BIO_free(b64);
			
			if (mem != NULL)
				BIO_free_all(mem);
			
			throw;
		}
	}
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::ObjectResolutionException::~ObjectResolutionException() throw()
{
}

// ------------------------------------------------------------------------------------------
AEObjectSupport::ObjectResolutionException&
AEObjectSupport::ObjectResolutionException::operator = (
	const AEObjectSupport::ObjectResolutionException&	ex)	//!< The original exception.
{
	RuntimeOSStatusException::operator = (ex);
	
	mErrorDesc = ex.mErrorDesc;
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
const char*
AEObjectSupport::ObjectResolutionException::what() const throw()
{
	return ("B::ObjectResolutionException");
}

// ------------------------------------------------------------------------------------------
/*!	Derived classes should override this function if they contain internal state.  They 
	should call the base class implementation prior to writing out their own state.
*/
void
AEObjectSupport::ObjectResolutionException::Write(std::ostream& ostr) const
{
	RuntimeOSStatusException::Write(ostr);
	
	size_t		size	= mErrorDesc.size();
	std::string	b64str;
	
	if (size > 0)
	{
		BIO*	mem	= NULL;
		BIO*	b64	= NULL;
		
		try
		{
			// Create a memory buffer which will contain the Base64 encoded string
			mem = BIO_new(BIO_s_mem());
			B_THROW_IF_NULL(mem);

			// Push on a Base64 filter so that writing to the buffer encodes the data
			b64 = BIO_new(BIO_f_base64());
			B_THROW_IF_NULL(b64);

			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
			mem = BIO_push(b64, mem);
			b64 = NULL;

			// Encode all the data
			BIO_write(mem, &mErrorDesc[0], size);
			BIO_flush(mem);

			// Create a new string from the data in the memory buffer
			char*	b64Ptr;
			size_t	b64Len	= BIO_get_mem_data(mem, &b64Ptr);
			
			b64str.assign(b64Ptr, b64Len);

			// Clean up and go home
			BIO_free_all(mem);
		}
		catch (...)
		{
			if (b64 != NULL)
				BIO_free(b64);
			
			if (mem != NULL)
				BIO_free_all(mem);
			
			throw;
		}
	}

	ostr << b64str << "\n";
}

#ifndef DOXYGEN_SKIP
bool gAEObjectSupport_ObjectResolutionException = ExceptionStreamer::Register<AEObjectSupport::ObjectResolutionException>();
#endif

}	// namespace B


extern "C" void	PutErrorIntoReply(AppleEvent *theReply, OSStatus errNum);

// ------------------------------------------------------------------------------------------
extern "C" void	PutErrorIntoReply(AppleEvent *theReply, OSStatus errNum)
{
	B::AEObjectSupport::AddCachedInfoToAppleEventReply(*theReply, errNum);
}
