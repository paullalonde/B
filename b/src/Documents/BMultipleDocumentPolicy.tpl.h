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

// standard headers
#include <memory>

// system headers
#include <CoreFoundation/CFArray.h>

// library headers
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

// B headers
#include "BAbstractDocument.h"
#include "BAEDescriptor.h"
#include "BAEEvent.h"
#include "BAEObjectSupport.h"
#include "BAEReader.h"
#include "BAEWriter.h"
#include "BBundle.h"
#include "BEvent.h"
#include "BEventCustomParams.h"
#include "BException.h"
#include "BFileUtilities.h"
#include "BMenuItemProperty.h"
#include "BMultipleDocumentPolicy.h"
#include "BNavDialogs.h"
#include "BPreferences.h"
#include "BStringFormatter.h"
#include "BUtility.h"
#include "BWindow.h"
#include "CFUtils.h"

namespace
{
	typedef B::MenuItemProperty<B::kEventClassB, 'RUrl', CFURLRef>	MenuRecentUrlProperty;

	typedef std::pair< SInt32, boost::shared_ptr<B::AbstractDocument> >	DocPair;
	
	class MatchByDocClass : public std::unary_function<DocPair, bool>
	{
	public:
		
				MatchByDocClass(DescType inDocClass)
					: mDocClass(inDocClass) {}
		
		bool	operator () (const DocPair& inDocPair) const
				{
					return (inDocPair.second->InheritsFrom(mDocClass));
				}
		
	private:
		
		DescType	mDocClass;
	};

	class MatchByName : public MatchByDocClass
	{
	public:
		
				MatchByName(DescType inDocClass, const B::String& inName)
					: MatchByDocClass(inDocClass), mName(inName) {}
		
		bool	operator () (const DocPair& inDocPair) const
				{
					return (MatchByDocClass::operator () (inDocPair) && 
							B::AEObjectSupport::CompareStrings(mName, inDocPair.second->GetName()));
				}
		
	private:
		
		const B::String	mName;
	};

	class MatchByUrl : public std::unary_function<DocPair, bool>
	{
	public:
		
				MatchByUrl(const B::Url& inUrl)
					: mUrl(inUrl) {}
		
		bool	operator () (const DocPair& inDocPair) const
				{
					return (mUrl == inDocPair.second->GetUrl());
				}
		
	private:
		
		const B::Url	mUrl;
	};

	class MatchByWindow : public std::unary_function<DocPair, bool>
	{
	public:
		
				MatchByWindow(B::Window* inWindow)
					: mWindow(inWindow) {}
		
		bool	operator () (const DocPair& inDocPair) const
				{
					return (inDocPair.second->OwnsWindow(mWindow));
				}
		
	private:
		
		const B::Window*	mWindow;
	};

	class MatchModified : public std::unary_function<DocPair, bool>
	{
	public:
		
		bool	operator () (const DocPair& inDocPair) const
				{
					return (inDocPair.second->IsModified());
				}
	};

	class MatchUnsavedModified : public std::unary_function<DocPair, bool>
	{
	public:
		
		bool	operator () (const DocPair& inDocPair) const
				{
					return (inDocPair.second->IsModified() && 
							inDocPair.second->IsNew());
				}
	};

}	// anonymous namespace


namespace B {

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> 
MultipleDocumentPolicy<DOC_FACTORY>::MultipleDocumentPolicy(
	AEObjectPtr		inApplication, 
	const Bundle&	inBundle, 
	Nib*			inAppNib)
		: mApplication(inApplication), mBundle(inBundle), mAppNib(inAppNib), 
		  mEventHandler(GetApplicationEventTarget()), 
		  mRecentDocumentsMenu(NULL), mHasRecentDocuments(false), mQuitOption(0), 
		  mLastDocumentID(0)
{
	InitEventHandler(mEventHandler);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> 
MultipleDocumentPolicy<DOC_FACTORY>::~MultipleDocumentPolicy()
{
	SaveRecentDocuments();
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::Init()
{
	MenuRef			menu;
	MenuItemIndex	item;
	OSStatus		err;
	
	err = GetIndMenuItemWithCommandID(NULL, kHICommandOpenRecent, 1, 
									  &menu, &item);
	
	if (err == noErr)
	{
		MenuRef	subMenu;
		
		err = GetMenuItemHierarchicalMenu(menu, item, &subMenu);
		
		if (err == noErr)
		{
			mRecentDocumentsMenu = subMenu;
		}
	}
	
	if (mRecentDocumentsMenu != NULL)
	{
		ReadRecentDocuments();
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::InitEventHandler(EventHandler& ioHandler)
{
	ioHandler.Add(this, &MultipleDocumentPolicy::CommandProcess);
	ioHandler.Add(this, &MultipleDocumentPolicy::CommandUpdateStatus);
	ioHandler.Add(this, &MultipleDocumentPolicy::BContinueQuit);
	
	ioHandler.Init();
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::RegisterAppleEvents(AEObjectSupport& ioObjectSupport)
{
	// If we're using the multiple document policy, we had better define the Open 
	// Document event.
	B_ASSERT(ioObjectSupport.IsEventDefined(kCoreEventClass, kAEOpenDocuments));
	ioObjectSupport.SetDefaultEventHandler(this, 
		&MultipleDocumentPolicy::HandleOpenDocumentAppleEvent);
	
	// If we're using the multiple document policy, we had better define the Print 
	// Document event.
	B_ASSERT(ioObjectSupport.IsEventDefined(kCoreEventClass, kAEPrintDocuments));
	ioObjectSupport.SetDefaultEventHandler(this, 
		&MultipleDocumentPolicy::HandlePrintDocumentAppleEvent);
}

// ------------------------------------------------------------------------------------------
/*!	Perform actions at application start up when launched without any
	documents.
	
	Override if you wish to perform some default action, such as
	creating a new, untitled document
*/
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::OpenApplication()
{
	NewDocument();
}

// ------------------------------------------------------------------------------------------
/*!	Respond to Reopen Application Apple %Event
	
	The system sends the reopen application Apple %Event when the user
	resumes an application that has no open windows.
*/
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::ReopenApplication()
{
	if (mDocuments.empty())
	{
		NewDocument();
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::OpenDocument(
	const AEDesc&									inDirectObject, 
	AEEvent<kCoreEventClass, kAEOpenDocuments>&		/* ioEvent */)
{
	AEReader	reader(inDirectObject);
	Url			url;
	
	reader.template Read<typeFileURL>(url);
	OpenDocumentForUrl(url, false);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::PrintDocument(
	const AEDesc&									inDirectObject, 
	AEEvent<kCoreEventClass, kAEPrintDocuments>&	ioEvent)
{
	AEReader							reader(inDirectObject);
	Url									url;
	boost::shared_ptr<AbstractDocument>	document;
	
	reader.template Read<typeFileURL>(url);
	document = OpenDocumentForUrl(url, true);
	
	document->PrintObject(ioEvent.mPrintSettings.get(), ioEvent.mPrinter.get(), 
						  ioEvent.mShowDialog);
	
#if 0
	// When printing several documents at once, the Print dialog should only be 
	// displayed once.  We achieve this by cheating a little: after printing each 
	// document, we force the print event's "print dialog" parameter to false.
	//
	// IMPORTANT: this will NOT work, because the first document will display 
	// the dialog (thus potentially changing some print settings), but subsequent 
	// documents will get the original settings from the Apple Event, NOT the 
	// ones modified by the dialog.
	//
	// Commenting out for now, until we find a solution ...
	
	AppleEvent	currentEvent;
	OSStatus	err;
	
	err = AEGetTheCurrentEvent(&currentEvent);
	
	if ((err == noErr) && (currentEvent.descriptorType == typeAppleEvent))
	{
		err = AEDeleteParam(&currentEvent, kPMShowPrintDialogAEType);
		
		Boolean	showDialog	= false;
		
		err = AEPutParamPtr(&currentEvent, kPMShowPrintDialogAEType, typeBoolean, 
							&showDialog, sizeof(showDialog));
		B_THROW_IF_STATUS(err);
	}
#endif
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> AbstractDocument*
MultipleDocumentPolicy<DOC_FACTORY>::FindByUrl(const Url& inUrl) const
{
	return (FindDocumentMatching(MatchByUrl(inUrl)));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> AbstractDocument*
MultipleDocumentPolicy<DOC_FACTORY>::FindByWindow(WindowRef inWindowRef) const
{
	Window*				window		= Window::FromWindowRef(inWindowRef);
	AbstractDocument*	document	= NULL;
	
	if (window != NULL)
	{
		document = FindDocumentMatching(MatchByWindow(window));
	}
	
	return (document);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::NewDocument(UInt32 inCommandID /* = kHICommandNew */)
{
	DescType		docClass;
	AEDescriptor	docProperties, docData;
	AERecord*		propertiesPtr	= docProperties;
	AEDesc*			dataPtr			= docData;
	
	GetInfoForNewDocument(inCommandID, docClass, docProperties, docData);
	
	if (!docProperties)
		propertiesPtr = NULL;
	
	if (!docData)
		dataPtr = NULL;
	
	AEObjectPtr	appPtr	= mApplication.lock();
	
	appPtr->SendCreateElementAppleEvent(docClass, kAEBeginning, appPtr, propertiesPtr, 
										dataPtr);
}

// ------------------------------------------------------------------------------------------
/*!	Allow the user to pick a AbstractDocument (usually for opening)
*/
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::OpenDocument(UInt32 inCommandID /* = kHICommandOpen */)
{
	GetFileDialog::Enter(CreateGetFileDialog(inCommandID, DialogModality::Modal()));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> boost::shared_ptr<AbstractDocument>
MultipleDocumentPolicy<DOC_FACTORY>::OpenDocumentForUrl(
	const Url&		inUrl, 
	bool			inForPrinting)
{
	boost::shared_ptr<AbstractDocument>	documentPtr(FindByUrl(inUrl));
	
	if (!documentPtr)
	{
		documentPtr	= CreateDocumentForUrl(inUrl, inForPrinting);
		
		AddDocument(documentPtr);
	}
	else
	{
		documentPtr->MakeCurrent();
		AddRecentDocument(documentPtr.get());
	}
	
	return documentPtr;
}

// ------------------------------------------------------------------------------------------
/*!	Create a new AbstractDocument and pass back an Apple %Event Model object
	representing that AbstractDocument
*/
template <class DOC_FACTORY> AEObjectPtr
MultipleDocumentPolicy<DOC_FACTORY>::OpenNewDocument(
	DescType			inDocClass,
	const AEDesc&		inProperties,
	const AEDesc&		inData)
{
	boost::shared_ptr<AbstractDocument>	documentPtr;
	
	documentPtr	= CreateNewDocument(inDocClass, inProperties, inData);
	
	AddDocument(documentPtr);
	
	return (documentPtr);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> boost::shared_ptr<AbstractDocument>
MultipleDocumentPolicy<DOC_FACTORY>::CreateDocumentForUrl(
	const Url&		inUrl, 
	bool			inForPrinting)
{
	boost::shared_ptr<AbstractDocument>	documentPtr;
	Bundle::Iterator					it	= mBundle.FindDocumentTypeForUrl(inUrl);
	
	if (it == mBundle.end())
		B_THROW(AECantHandleClassException());
	
	std::auto_ptr<AbstractDocument>	documentAutoPtr;
	
	documentAutoPtr = mFactory.Instantiate(mApplication.lock(), it->mOSAClass, ++mLastDocumentID);
	documentPtr = documentAutoPtr;
	documentPtr->InitOpenDocument(inUrl, it->mName, mAppNib.get(), inForPrinting);
	
	return (documentPtr);
}

// ------------------------------------------------------------------------------------------
/*!	Create a new AbstractDocument and pass back an Apple %Event Model object
	representing that AbstractDocument
*/
template <class DOC_FACTORY> boost::shared_ptr<AbstractDocument>
MultipleDocumentPolicy<DOC_FACTORY>::CreateNewDocument(
	DescType			inDocClass,
	const AEDesc&		inProperties,
	const AEDesc&		inData)
{
	boost::shared_ptr<AbstractDocument>	documentPtr;
	Bundle::Iterator					it	= mBundle.FindDocumentTypeByOSAClass(inDocClass);
	
	if (it == mBundle.end())
		B_THROW(AECantHandleClassException());
	
	AEReader	reader(inProperties);
	String		docName;
	
	if (!reader.template ReadKey<typeUTF16ExternalRepresentation>(pName, docName, std::nothrow))
	{
		// Note: these strings must come from the main bundle.
		String	template1(BLocalizedStringFromTable(
								CFSTR("untitled"), CFSTR("B"), 
								"the name of the first untitled document"));
		String	template2(BLocalizedStringFromTable(
								CFSTR("untitled %1%"), CFSTR("B"), 
								"the name of the untitled documents after the first"));
		String	extension(FindDefaultExtensionForDocClass(inDocClass), false);
		
		docName = CreateUniqueDocumentName(template1, template2, extension);
	}
	
	std::auto_ptr<AbstractDocument>	documentAutoPtr;
	
	documentAutoPtr = mFactory.Instantiate(mApplication.lock(), inDocClass, ++mLastDocumentID);
	documentPtr = documentAutoPtr;
	documentPtr->InitNewDocument(docName, it->mName, inProperties, inData, mAppNib.get());
	
	return (documentPtr);
}

// ------------------------------------------------------------------------------------------

namespace
{
	class AllDirtyDocumentsSpecifier
	{
	public:
		
		//! Constructor.
		AllDirtyDocumentsSpecifier(AEObjectPtr rootObj)
			: mRootObj(rootObj) {}
		
	private:
		
		// member variables
		AEObjectPtr	mRootObj;
		
		// friends
		friend AEWriter&	operator << (AEWriter& writer, const AllDirtyDocumentsSpecifier& value);
	};
	
	AEWriter&
	operator << (AEWriter& writer, const AllDirtyDocumentsSpecifier& value)
	{
		AutoAEWriterRecord	directObj(writer, typeObjectSpecifier);
		
		writer << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formTest) 
			   << AEKey(keyAEDesiredClass) << AETypedObject<typeType>(cDocument) 
			   << AEKey(keyAEContainer)    << *value.mRootObj;
		
		{
			AutoAEWriterRecord	keyData(writer, typeCompDescriptor, keyAEKeyData);
		
			writer << AEKey(keyAECompOperator) << AETypedObject<typeEnumerated>(kAEEquals);
			
			{
				AutoAEWriterRecord	obj1(writer, typeObjectSpecifier, keyAEObject1);
				
				writer << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formPropertyID) 
					   << AEKey(keyAEDesiredClass) << AETypedObject<typeType>(cProperty) 
					   << AEKey(keyAEKeyData)      << AETypedObject<typeType>(pIsModified);
				
				{
					AutoAEWriterDesc	container(writer, keyAEContainer, typeObjectBeingExamined);
				}
			}
			
			{
				AutoAEWriterDesc	obj2(writer, keyAEObject2, typeTrue);
			}
		}
		
		return (writer);
	}
}

template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::SaveAllDocuments()
{
	// "save every document whose modified is true"
	
	AEEvent<kAECoreSuite, kAESave>::Send(
		AESelfTarget(),
		AllDirtyDocumentsSpecifier(mApplication.lock()), 
		AEEventFunctor::OptionalUrl(), 
		AEEventFunctor::OptionalObjectType());
}

// ------------------------------------------------------------------------------------------

namespace
{
	class AllDocumentsSpecifier
	{
	public:
	
		//! Constructor.
		AllDocumentsSpecifier(AEObjectPtr rootObj)
			: mRootObj(rootObj) {}
		
	private:
		
		// member variables
		AEObjectPtr	mRootObj;
		
		// friends
		friend AEWriter&	operator << (AEWriter& writer, const AllDocumentsSpecifier& value);
	};
	
	
	AEWriter&
	operator << (AEWriter& writer, const AllDocumentsSpecifier& value)
	{
		AutoAEWriterRecord	directObj(writer, typeObjectSpecifier);
		
		return (writer << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formAbsolutePosition) 
					   << AEKey(keyAEDesiredClass) << AETypedObject<typeType>(cDocument) 
					   << AEKey(keyAEContainer)    << *value.mRootObj
					   << AEKey(keyAEKeyData)      << AETypedObject<typeAbsoluteOrdinal>(kAEAll));
	}
}

template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::CloseAllDocuments(
	OSType					inSaveOption /* = kAEAsk */)
{
	// "close every document saving <inSaveOption>"
	
	AEEvent<kAECoreSuite, kAEClose>::Send(
		AESelfTarget(),
		AllDocumentsSpecifier(mApplication.lock()), 
		AEEventFunctor::OptionalSaveOption(inSaveOption), 
		AEEventFunctor::OptionalUrl());
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::QuitApplication(
	OSType			inSaveOption /* = kAEAsk */)
{
	bool	done			= true;
	UInt32	modCount		= CountModifiedDocuments();
	UInt32	unsavedCount	= CountUnsavedModifiedDocuments();
	
	if (inSaveOption == kAEAsk)
	{
		if (modCount > 1)
		{
			std::auto_ptr<AskReviewDocumentsDialog>	dialogPtr;
			
			dialogPtr = CreateAskReviewDocumentsDialog(DialogModality::Modal(), modCount);
			
			switch (dialogPtr->Run())
			{
			case AskReviewDocumentsDialog::kCancel:
				// The user cancelled the Quit.
				return;
				
			case AskReviewDocumentsDialog::kDiscard:
				inSaveOption = kAENo;
				break;
				
			case AskReviewDocumentsDialog::kReview:
				break;
			}
		}
	}
	
	// We need user interaction if (1) we need to ask the user if they want to 
	// save and there is at least one modified document, or (2) if we want to save 
	// without asking, but there is at least one modified document that has never 
	// been saved.
	
	DocumentIterator	it;
	AbstractDocument*	document;
	
	if (((inSaveOption == kAEAsk) && (modCount > 0)) || 
		((inSaveOption == kAEYes) && (unsavedCount > 0)))
	{
		mQuitOption = inSaveOption;
		
		// Bring unsaved documents on top.
		
		for (it = mDocuments.begin(); it != mDocuments.end(); )
		{
			document = it->second.get();
			it++;
			
			if (document->IsModified())
				document->MakeCurrent();
		}
		
		for (it = mDocuments.begin(); it != mDocuments.end(); )
		{
			document = it->second.get();
			it++;
			
			document->Quitting(inSaveOption);
			done = false;
		}
	}
	else
	{
		// No user interaction required.  Close the open documents directly.
		
		while (!mDocuments.empty())
		{
			mDocuments.begin()->second->CloseObject(inSaveOption);
		}
		
		done = mDocuments.empty();
	}
	
	if (done)
	{
		QuitApplicationEventLoop();
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> bool
MultipleDocumentPolicy<DOC_FACTORY>::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandNew:
		NewDocument(inHICommand.commandID);
		break;
		
	case kHICommandOpen:
		OpenDocument(inHICommand.commandID);
		break;
		
	case kHICommandOpenRecentFile:
		if (inHICommand.attributes & kHICommandFromMenu)
		{
			OpenRecentDocument(inHICommand.source.menu.menuItemIndex);
		}
		break;
		
	case kHICommandClearRecent:
		ClearRecentDocuments();
		break;
		
	case kHICommandCloseAll:
		CloseAllDocuments();
		break;
		
	case kHICommandSaveAll:
		SaveAllDocuments();
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> bool
MultipleDocumentPolicy<DOC_FACTORY>::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	CommandData&				ioCmdData)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandNew:
	case kHICommandOpen:
		ioCmdData.SetEnabled(true);
		break;
		
	case kHICommandCloseAll:
	case kHICommandSaveAll:
		ioCmdData.SetEnabled(!mDocuments.empty());
		break;
		
	case kHICommandOpenRecent:
		ioCmdData.SetEnabled(mHasRecentDocuments);
		break;
		
	case kHICommandOpenRecentFile:
		ioCmdData.SetEnabled(true);
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> String
MultipleDocumentPolicy<DOC_FACTORY>::CreateUniqueDocumentName(
	const String&	inTemplate1,
	const String&	inTemplate2,
	const String&	inExtension)
{
	String	docName;
	
	for (int index = 0; true; index++)
	{
		docName = CreateNewDocumentName(inTemplate1, inTemplate2, index,
										inExtension);
		
		// Return the first document that is an instance of cDocument (or of a class 
		// derived from cDocument) and whose name is docName.
		
		if (FindDocumentMatching(MatchByName(cDocument, docName)) == NULL)
		{
			break;
		}
	}
	
	return (docName);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> String
MultipleDocumentPolicy<DOC_FACTORY>::CreateNewDocumentName(
	const String&	inTemplate1,
	const String&	inTemplate2,
	int				inIndex,
	const String&	inExtension)
{
	String	docName;
	
	if (inIndex != 0)
	{
		docName = Extract(StringFormatter(inTemplate2) % (inIndex+1));
	}
	else
	{
		docName = inTemplate1;
	}
	
	if (!inExtension.empty())
	{
		docName = Extract(StringFormatter("%1%.%2%") % docName % inExtension);
	}
	
	return (docName);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::HandleOpenDocumentAppleEvent(
	const AEDesc&									directObj, 
	AEEvent<kCoreEventClass, kAEOpenDocuments>&		event)
{
	OpenDocument(directObj, event);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::HandlePrintDocumentAppleEvent(
	const AEDesc&									directObj, 
	AEEvent<kCoreEventClass, kAEPrintDocuments>&	event)
{
	PrintDocument(directObj, event);
}

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Self-send an Apple %Event to open a list of documents.
*/
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::SendOpenDocumentsAppleEvent(
	const AEDescList&	inDocList)
{
	AEEvent<kCoreEventClass, kAEOpen>::Send(AESelfTarget(), inDocList);
}

// ------------------------------------------------------------------------------------------
/*!	Self-send an Apple %Event to open a single document.
*/
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::SendOpenDocumentAppleEvent(
	const Url&			inUrl)
{
	AEEvent<kCoreEventClass, kAEOpen>::Send(AESelfTarget(), inUrl);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::GetInfoForNewDocument(
	UInt32				/* inCommand */, 
	DescType&			outClass, 
	AEDesc&				/* outProperties */, 
	AEDesc&				/* outData */)
{
	outClass = FindDefaultDocClass();
}

#pragma mark -

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> SInt32
MultipleDocumentPolicy<DOC_FACTORY>::CountDocuments(
	DescType		inDocClass) const
{
	B_ASSERT(AEObject::DoesClassInheritFrom(inDocClass, cDocument));
	
	return (CountDocumentsMatching(MatchByDocClass(inDocClass)));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> AEObjectPtr
MultipleDocumentPolicy<DOC_FACTORY>::GetDocumentByIndex(
	DescType		inDocClass, 
	unsigned		inIndex) const
{
	B_ASSERT(AEObject::DoesClassInheritFrom(inDocClass, cDocument));
	
	boost::shared_ptr<AbstractDocument>	document;
	unsigned							currIndex	= 0;
	
	for (DocumentMap::const_iterator it = mDocuments.begin(); 
		 it != mDocuments.end();
		 ++it)
	{
		if (!it->second->InheritsFrom(inDocClass))
			continue;
		
		if (currIndex++ == inIndex)
		{
			document = it->second;
			
			B_ASSERT(document != NULL);
			
			break;
		}
	}
	
	if (document == NULL)
	{
		B_THROW(AENoSuchObjectException());
	}
	
	return (document);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> AEObjectPtr
MultipleDocumentPolicy<DOC_FACTORY>::GetDocumentByName(
	DescType		inDocClass, 
	const String&	inName) const
{
	B_ASSERT(AEObject::DoesClassInheritFrom(inDocClass, cDocument));
	
	AbstractDocument	*document	= NULL;
	
	document = FindDocumentMatching(MatchByName(inDocClass, inName));
	
	if (document == NULL)
	{
		B_THROW(AENoSuchObjectException());
	}
	
	return (document->GetAEObjectPtr());
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> AEObjectPtr
MultipleDocumentPolicy<DOC_FACTORY>::GetDocumentByUniqueID(
	DescType		inDocClass, 
	SInt32			inUniqueID) const
{
	B_ASSERT(AEObject::DoesClassInheritFrom(inDocClass, cDocument));
	
	boost::shared_ptr<AbstractDocument>	document;
	DocumentMap::const_iterator			it(mDocuments.find(inUniqueID));
	
	if ((it != mDocuments.end()) && it->second->InheritsFrom(inDocClass))
	{
		document = it->second;
	}
	
	if (document.get() == NULL)
	{
		B_THROW(AENoSuchObjectException());
	}
	
	return (document);
}

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	The default implementation activates the file type pop-up menu if the document 
	policy's bundle contains more than one document type.
*/
template <class DOC_FACTORY> std::auto_ptr<GetFileDialog>
MultipleDocumentPolicy<DOC_FACTORY>::CreateGetFileDialog(
	UInt32					/* inCommandID */,
	const DialogModality&	inModality)
{
	std::auto_ptr<GetFileDialog>	getFileDlg;
	
	getFileDlg = GetFileDialog::Make<GetFileDialog>(
						inModality, 
						boost::bind(
							&ThisType::GetFileDialogCallback, this, _1, _2, _3));
	
	if (mBundle.begin() != mBundle.end())
	{
		getFileDlg->SetDocumentTypes(mBundle);
	}
	
	return (getFileDlg);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::GetFileDialogCallback(
	const GetFileDialog&	/* inDialog */,
	bool					inGood,
	const NavReplyRecord&	inNavReply)
{
	HandleGetFileResult(inGood, inNavReply);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::HandleGetFileResult(
	bool					inGood,
	const NavReplyRecord&	inNavReply)
{
	if (inGood)
	{
		SendOpenDocumentsAppleEvent(inNavReply.selection);
	}
}

// ------------------------------------------------------------------------------------------
/*!	The dialog must be configured to be application-modal.
*/
template <class DOC_FACTORY> std::auto_ptr<AskReviewDocumentsDialog>
MultipleDocumentPolicy<DOC_FACTORY>::CreateAskReviewDocumentsDialog(
	const DialogModality&	inModality,
	UInt32					inDocumentCount)
{
	return (AskReviewDocumentsDialog::Make<AskReviewDocumentsDialog>(inModality, 
																	 inDocumentCount));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> UInt32
MultipleDocumentPolicy<DOC_FACTORY>::CountModifiedDocuments() const
{
	return (CountDocumentsMatching(MatchModified()));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> UInt32
MultipleDocumentPolicy<DOC_FACTORY>::CountUnsavedModifiedDocuments() const
{
	return (CountDocumentsMatching(MatchUnsavedModified()));
}

#pragma mark -

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> DescType
MultipleDocumentPolicy<DOC_FACTORY>::FindDefaultDocClass() const
{
	DescType	docClass	= 0;
	
	if (mBundle.begin() != mBundle.end())
	{
		docClass = mBundle.begin()->mOSAClass;
	}
	
	if (docClass == 0)
		docClass = cDocument;
	
	return (docClass);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> OSType
MultipleDocumentPolicy<DOC_FACTORY>::FindDefaultFileTypeForDocClass(
	DescType			inDocClass) const
{
	Bundle::Iterator	it			= mBundle.FindDocumentTypeByOSAClass(inDocClass);
	OSType				fileType	= 0;
	
	if (it != mBundle.end())
	{
		if (it->mOSTypes.size() > 0)
			fileType = it->mOSTypes[0];
	}
	
	if (fileType == 0)
		fileType = '\?\?\?\?';
	
	return (fileType);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> String
MultipleDocumentPolicy<DOC_FACTORY>::FindDefaultExtensionForDocClass(
	DescType			inDocClass) const
{
	Bundle::Iterator	it	= mBundle.FindDocumentTypeByOSAClass(inDocClass);
	String				extension;
	
	if (it != mBundle.end())
	{
		if (it->mExtensions.size() > 0)
		{
			extension = it->mExtensions[0];
		}
	}
	
	return (extension);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::AddDocument(boost::shared_ptr<AbstractDocument> inDocumentPtr)
{
	AbstractDocument*	document	= inDocumentPtr.get();
	
	mDocuments.insert(DocumentMap::value_type(inDocumentPtr->GetUniqueID(), inDocumentPtr));
	AddRecentDocument(document);
	
	// Register for signals
	
	document->GetClosingSignal().connect(
					boost::bind(&ThisType::DocumentClosing, this, _1));
	
	document->GetDeleteSignal().connect(
					boost::bind(&ThisType::DocumentDelete, this, _1));
	
	document->GetUrlChangedSignal().connect(
					boost::bind(&ThisType::DocumentUrlChanged, this, _1));
	
	document->GetAsyncActionCancelledSignal().connect(
					boost::bind(&ThisType::DocumentAsyncActionCancelled, this));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::RemoveDocument(AbstractDocument* inDocument)
{
	// Note:	We don't have to worry about disconnecting ourselves from the document's 
	//			signals, because we outlive all document objects.
	
	// Because our document map contains a smart pointer, erase the document element will 
	// end up deleting the document proper.
	mDocuments.erase(inDocument->GetUniqueID());
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::AddRecentDocument(AbstractDocument* inDocument)
{
	Url	url	= inDocument->GetUrl();
	
	if (!url.Empty())
	{
		AddRecentDocumentUrl(url);
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::AddRecentDocumentUrl(const Url& inUrl)
{
	if (mRecentDocumentsMenu == NULL)
		return;
	
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = LSCopyDisplayNameForURL(inUrl.cf_ref(), &cfstr);
	B_THROW_IF_STATUS(err);
	
	String		displayName(cfstr, from_copy);
	
	for (MenuItemIndex i = 1; i < CountMenuItems(mRecentDocumentsMenu); i++)
	{
		MenuCommand	cmd;
		
		err = GetMenuItemCommandID(mRecentDocumentsMenu, i, &cmd);
		B_THROW_IF_STATUS(err);
		
		if (cmd != kHICommandOpenRecentFile)
			break;
		
		if (inUrl == GetRecentDocumentUrlFromMenu(i))
		{
			DeleteMenuItem(mRecentDocumentsMenu, i);
			
			break;
		}
	}
	
	err = InsertMenuItemTextWithCFString(mRecentDocumentsMenu, 
										 displayName.cf_ref(), 
										 0, 0, 0);
	B_THROW_IF_STATUS(err);
	
	SetRecentDocumentUrlInMenu(1, inUrl);
	
	err = SetMenuItemCommandID(mRecentDocumentsMenu, 1, 
							   kHICommandOpenRecentFile);
	B_THROW_IF_STATUS(err);
	
	mHasRecentDocuments = true;
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> Url
MultipleDocumentPolicy<DOC_FACTORY>::GetRecentDocumentUrlFromMenu(
	MenuItemIndex	item)
{
	B_ASSERT(mRecentDocumentsMenu != NULL);
	
	return (MenuRecentUrlProperty::Get(mRecentDocumentsMenu, item));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::SetRecentDocumentUrlInMenu(
	MenuItemIndex	item, 
	const Url&		url)
{
	B_ASSERT(mRecentDocumentsMenu != NULL);
	
	MenuRecentUrlProperty::Set(mRecentDocumentsMenu, item, url);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::ClearRecentDocuments()
{
	OSStatus	err;
	
	while (CountMenuItems(mRecentDocumentsMenu) > 0)
	{
		MenuCommand	cmd;
		
		err = GetMenuItemCommandID(mRecentDocumentsMenu, 1, &cmd);
		B_THROW_IF_STATUS(err);
		
		if (cmd != kHICommandOpenRecentFile)
			break;
		
		DeleteMenuItem(mRecentDocumentsMenu, 1);
	}
	
	mHasRecentDocuments = false;
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::OpenRecentDocument(
	MenuItemIndex	item)
{
	SendOpenDocumentAppleEvent(GetRecentDocumentUrlFromMenu(item));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::ReadRecentDocuments()
{
	Preferences			prefs(mBundle);
	OSPtr<CFArrayRef>	array(prefs.template GetPtr<CFArrayRef>("B RecentDocuments"));
	OSStatus			err;
	
	ClearRecentDocuments();
	
	if (array == NULL)
		return;
	
	for (CFIndex i = 0; i < CFArrayGetCount(array); i++)
	{
		OSPtr<CFDataRef>	data;
		std::vector<UInt8>	blob;
		FSRef				ref;
		
		if (!CFUGet(array, i, data))
			continue;
		
		DataToBlob(data, blob);
		
		if (!BlobToRef(blob, ref, false))
			continue;
		
		Url			url(ref);
		CFStringRef	cfstr;
		
		err = LSCopyDisplayNameForURL(url.cf_ref(), &cfstr);
		B_THROW_IF_STATUS(err);
		
		String	displayName(cfstr, from_copy);
		
		err = InsertMenuItemTextWithCFString(mRecentDocumentsMenu, 
											 displayName.cf_ref(), 
											 i, 0, 0);
		B_THROW_IF_STATUS(err);
		
		SetRecentDocumentUrlInMenu(i+1, url);
		
		err = SetMenuItemCommandID(mRecentDocumentsMenu, i+1, 
								   kHICommandOpenRecentFile);
		B_THROW_IF_STATUS(err);
		
		mHasRecentDocuments = true;
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::SaveRecentDocuments()
{
	MutableArray<CFDataRef>	array;
	std::vector<UInt8>		blob;
	OSStatus				err;
	
	for (MenuItemIndex i = 1; i < CountMenuItems(mRecentDocumentsMenu); i++)
	{
		MenuCommand	cmd;
		FSRef		ref;
		
		err = GetMenuItemCommandID(mRecentDocumentsMenu, i, &cmd);
		B_THROW_IF_STATUS(err);
		
		if (cmd != kHICommandOpenRecentFile)
			break;
		
		if (!GetRecentDocumentUrlFromMenu(i).Copy(ref, std::nothrow))
			continue;
		
		RefToBlob(ref, blob);
		array.push_back(BlobToData(blob));
	}
	
	Preferences	prefs(mBundle);
	
	prefs.SetPtr("B RecentDocuments", array.cf_ptr());
	prefs.Flush();
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::DocumentClosing(
	AbstractDocument*	/* inDocument */)
{
	if ((mQuitOption != 0) && (CountModifiedDocuments() <= 1))
	{
		Event<kEventClassB, kEventBContinueQuit>	event;
		
		event.Post(kEventPriorityHigh);
		//QuitApplication(kAENo);
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::DocumentDelete(
	AbstractDocument*	inDocument)
{
	RemoveDocument(inDocument);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::DocumentUrlChanged(
	AbstractDocument*	inDocument)
{
	AddRecentDocumentUrl(inDocument->GetUrl());
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> void
MultipleDocumentPolicy<DOC_FACTORY>::DocumentAsyncActionCancelled()
{
	if (mQuitOption != 0)
	{
		mQuitOption = 0;
		
		for (DocumentIterator it = mDocuments.begin(); it != mDocuments.end(); )
		{
			// Increment the iterator prior to calling the document, so the 
			// iterator doesn't become invalidated should the document delete 
			// itself.
			AbstractDocument*	document	= it->second.get();
			
			it++;
			
			document->CancelQuitting();
		}
	}
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> bool
MultipleDocumentPolicy<DOC_FACTORY>::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	return (this->HandleCommand(event.mHICommand));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> bool
MultipleDocumentPolicy<DOC_FACTORY>::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	return (this->HandleUpdateStatus(event.mHICommand, event.mData));
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> bool
MultipleDocumentPolicy<DOC_FACTORY>::BContinueQuit(
	Event<kEventClassB, kEventBContinueQuit>&	/* event */)
{
	QuitApplication(mQuitOption);
	
	return (true);
}


}	// namespace B
