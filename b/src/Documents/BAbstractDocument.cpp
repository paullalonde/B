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
#include "BAbstractDocument.h"

// standard headers
#include <stdint.h>

// B headers
#include "BAEDescParam.h"
#include "BAEReader.h"
#include "BAEWriter.h"
#include "BBundle.h"
#include "BCommandData.h"
#include "BEvent.h"
#include "BEventCustomParams.h"
#include "BEventParams.h"
#include "BEventTarget.h"
#include "BException.h"
#include "BFileUtilities.h"
#include "BSaveInfo.h"
#include "BWindow.h"


namespace B {

// ==========================================================================================
//	AbstractDocument::EventHelper

class AbstractDocument::EventHelper : public EventTarget
{
public:
	
	// instance creation
	static OSPtr<HIObjectRef>	Create(AbstractDocument* document);
	
	// instantiation
	static EventTarget*			Instantiate(HIObjectRef inObjectRef);
	
	// class ID
	static const CFStringRef	kHIObjectClassID;
	
protected:
	
	// initialisation
	virtual void	Initialize(EventRef inEvent);
	
private:
	
	// constructor
	EventHelper(HIObjectRef inObjectRef);

	void	InitEventHandler();
	
	// Carbon %Event handlers
	bool	CommandProcess(
				Event<kEventClassCommand, kEventCommandProcess>&		event);
	bool	CommandUpdateStatus(
				Event<kEventClassCommand, kEventCommandUpdateStatus>&	event);
	
	// member variables
	EventHandler		mEventHandler;
	AbstractDocument*	mDocument;
};


const CFStringRef	AbstractDocument::EventHelper::kHIObjectClassID	= 
						CFSTR("ca.paullalonde.b.abstractdocument.eventhelper");

// ------------------------------------------------------------------------------------------
OSPtr<HIObjectRef>
AbstractDocument::EventHelper::Create(AbstractDocument* document)
{
	Event<kEventClassHIObject, kEventHIObjectInitialize>	initEvent;
	
	EventDirectParam<typeVoidPtr>::Set(initEvent, document);
	
	return (EventTarget::Create<EventHelper>(initEvent));
}

// ------------------------------------------------------------------------------------------
EventTarget*
AbstractDocument::EventHelper::Instantiate(HIObjectRef inObjectRef)
{
	return (new EventHelper(inObjectRef));
}

// ------------------------------------------------------------------------------------------
AbstractDocument::EventHelper::EventHelper(HIObjectRef inObjectRef)
	: EventTarget(inObjectRef), 
	  mEventHandler(inObjectRef), 
	  mDocument(NULL)
{
	InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::EventHelper::InitEventHandler()
{
	mEventHandler.Add(this, &AbstractDocument::EventHelper::CommandProcess);
	mEventHandler.Add(this, &AbstractDocument::EventHelper::CommandUpdateStatus);
	
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::EventHelper::Initialize(
	EventRef			inEvent)
{
	mDocument = reinterpret_cast<AbstractDocument*>(EventDirectParam<typeVoidPtr>::Get(inEvent));
	B_ASSERT(mDocument != NULL);
}

// ------------------------------------------------------------------------------------------
bool
AbstractDocument::EventHelper::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	WindowRef	window;
	
	if (!EventParam<kEventParamWindowRef>::Get(event, window, std::nothrow))
		window = NULL;
	
	return (mDocument->HandleCommand(event.mHICommand, 
									 Window::FromWindowRef(window)));
}

// ------------------------------------------------------------------------------------------
bool
AbstractDocument::EventHelper::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	WindowRef	window;
	
	if (!EventParam<kEventParamWindowRef>::Get(event, window, std::nothrow))
		window = NULL;
	
	return (mDocument->HandleUpdateStatus(event.mHICommand, event.mData, 
										  Window::FromWindowRef(window)));
}

#ifndef DOXYGEN_SKIP
B_VAR_EXPORT EventTargetRegistrar<AbstractDocument::EventHelper>	gRegisterAbstractDocumentEventHelper;
#endif


// ==========================================================================================
//	AbstractDocument

#pragma mark -
#pragma mark * AbstractDocument *

// ------------------------------------------------------------------------------------------
AbstractDocument::AbstractDocument(
	AEObjectPtr	inContainer,	//!< The object's container (as seen in AppleScript).
	DescType	inClassID,		//!< The object's class ID;  must match the application's AppleScript dictionary.
	SInt32		inUniqueID)		//!< The document's unique id number.
		: AEObject(inContainer, inClassID), 
		  mUniqueID(inUniqueID), mModCount(0), mOpenForPrinting(false), mQuitting(false)
{
	mObjectRef = AbstractDocument::EventHelper::Create(this);
}

#pragma mark - Inquiries -

// ------------------------------------------------------------------------------------------
bool
AbstractDocument::IsNew() const
{
	return (GetUrl().Empty());
}

// ------------------------------------------------------------------------------------------
bool
AbstractDocument::IsModified() const
{
	return (mModCount > 0);
}

// ------------------------------------------------------------------------------------------
/*!	If the change count goes from zero to non-zero (meaning that the document is becoming 
	dirty), the function broadcasts the @c kDirtyStateChangeNotification notification.
*/
void
AbstractDocument::Modified()
{
	if (mModCount < UINT_MAX)
	{
		UInt32	oldModCount	= mModCount++;
		
		if ((oldModCount > 0) != (mModCount > 0))
		{
			mDirtyStateChangedSignal(this);
		}
	}
}

// ------------------------------------------------------------------------------------------
/*!	If the change count goes from non-zero to zero (meaning that the document is becoming 
	clean), the function broadcasts the @c kDirtyStateChangeNotification notification.
*/
void
AbstractDocument::Unmodified()
{
	if (mModCount > 0)
	{
		UInt32	oldModCount	= mModCount--;
		
		if ((oldModCount > 0) != (mModCount > 0))
		{
			mDirtyStateChangedSignal(this);
		}
	}
}

// ------------------------------------------------------------------------------------------
/*!	If the change count was previously non-zero, the function broadcasts the 
	@c kDirtyStateChangeNotification notification.
*/
void
AbstractDocument::ClearModified()
{
	UInt32	oldModCount	= mModCount;
	
	mModCount = 0;
	
	if (oldModCount != 0)
	{
		mDirtyStateChangedSignal(this);
	}
}

// ------------------------------------------------------------------------------------------
/*!	This is the string returned as the "name" property in AppleScript.
	
	The default implementation returns the last portion of the document's url's path.
*/
String
AbstractDocument::GetName() const
{
	String	name;
	Url		url(GetUrl());
	
	if (!url.Empty())
		name = url.GetFilename();
	
	return (name);
}

// ------------------------------------------------------------------------------------------
/*!	The document's id identifies it within the application;  this is the value returned 
	as the "id" property in AppleScript.
	
	The default implementation returns id given when the document was constructed.
*/
SInt32
AbstractDocument::GetUniqueID() const
{
	return (mUniqueID);
}

// ------------------------------------------------------------------------------------------
/*!	The document's display name is what is shown to the user in window title bars, etc.  
	It's different from the normal name because the Aqua HIG contains the notion of users 
	being able to "hide" a file name's extension.
	
	The default implementation calls through to the OS to compute the display name from 
	the document's URL.
*/
String
AbstractDocument::GetDisplayName() const
{
	CFStringRef	cfstr;
	OSStatus	err;
	
	err = LSCopyDisplayNameForURL(GetUrl().cf_ref(), &cfstr);
	B_THROW_IF_STATUS(err);
	
	return (String(cfstr, from_copy));
}

//------------------------------------------------------------------------------------------
/*!	The creator code is a way of tagging files as "belonging" to a given application.  It's 
	optional in Mac OS X, but was very much used in the pre-X days, so it can be convenient 
	to set a file's creator for interoperability reasons.
	
	The default implementation returns the main bundle's creator code.
*/
OSType
AbstractDocument::GetCreator() const
{
	return (Bundle::Main().PackageCreator());
}

// ------------------------------------------------------------------------------------------
EventTargetRef
AbstractDocument::GetEventTarget() const
{
	return (HIObjectGetEventTarget(mObjectRef));
}

#pragma mark - Saving -


// ------------------------------------------------------------------------------------------
/*!	This is a high-level function that can be used to perform a "Save" or a 
	"Save As", depending on the arguments.  The function displays any necessary UI 
	(putting up a Save sheet, for example), then calls lower-level functions to write 
	out the document to a URL.
	
	If the function is called with the default arguments, then it operates like a "Save".
	If it's is called with either a document type or a URL, then it operates like 
	a "Save As".  In either case, if the document has never been saved, it operates like a 
	"Save As".
	
	Note that it's possible for the file not to have been saved yet when this function 
	returns.  This can happen if the save requires user input, and that input is obtained 
	via non-modal means such as a sheet.
*/
void
AbstractDocument::SaveDocument(
	SaveInfo&	ioInfo)				//!< The document's save information.
{
	Url		oldUrl(GetUrl());
	String	oldContentType(GetDocumentType());
	Url		newUrl(ioInfo.GetUrl());
	String	newContentType(ioInfo.GetContentType());
	
	if (newContentType.empty())
	{
		newContentType = oldContentType;
		ioInfo.SetContentType(newContentType);
	}
	
	if (!newUrl.Empty())
	{	
		// The caller specified a URL, so save the document into the new URL.
		
		WriteDocumentToUrl(ioInfo);
		WriteComplete(oldUrl, oldContentType);
	}
	else
	{
		// The URL is empty.  If the document already has a URL, save it there;  else, 
		// we need to ask the user where to save the document.
		
		if (IsNew())
		{
			// New document.  Ask the user where to save it.
			
			ioInfo.SetFromUser(false);
			ioInfo.SetClosing(false);
			AskUserToSave(ioInfo);
		}
		else if (newContentType != oldContentType)
		{
			// The caller specified a new document type, so save the document with that 
			// type.
			
			ioInfo.SetContentType(newContentType);
			WriteDocumentToUrl(ioInfo);
			WriteComplete(oldUrl, oldContentType);
		}
		else
		{
			// Neither a URL nor a document type were given, and the document already 
			// has a URL, so save it there.
			ioInfo.SetWriteAttributes(false);
			WriteDocument(ioInfo);
			WriteComplete(oldUrl, oldContentType);
		}
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::WriteDocument(
	SaveInfo&	/* ioSaveInfo */)				//!< The document's save information.
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::WriteDocumentToUrl(
	SaveInfo&	/* ioSaveInfo */)				//!< The document's save information.
{
	// The default implementation does nothing.
}


// ------------------------------------------------------------------------------------------
/*!	Currently, the function performs these actions:
	
	-# Clears the change count, which will trigger a broadcast of the 
	   @c kDirtyStateChangeNotification notification.
	-# Optionally clears the document's undo stack.
	-# If the document's url or type has changed, issues the @c kUrlChangedNotification 
	   notification.
	
	Derived classes that wish to perform additional operations when a document is saved 
	should override this function.
*/
void
AbstractDocument::WriteComplete(
	const Url&				inOldUrl, 
	const String&			inOldDocumentType)
{
	ClearModified();
	
	if ((inOldUrl != GetUrl()) || (inOldDocumentType != GetDocumentType()))
	{
		NotifyUrlChanged();
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::RevertDocument()
{
	B_ASSERT(!IsNew());
	
	Url		docUrl	= GetUrl();
	String	docType	= GetDocumentType();
	
	ReadDocument(docUrl, docType);
	DoneReading(docUrl, docType);
	NotifyContentChanged();
}

// ------------------------------------------------------------------------------------------
/*!	The function takes the following steps:
		
	-# Creates an initial data set for the document, using @a inProperties and @a inData 
	   as appropriate.
	-# Creates the document's window(s).
	-# Displays the window(s).
*/
void
AbstractDocument::InitNewDocument(
	const String&	inName,			//!< The document's name.
	const String&	inDocumentType,	//!< The document's type.
	const AEDesc&	inProperties,	//!< The document's initial properties (if @c typeNull, no properties are specified).
	const AEDesc&	inData,			//!< The document's intrinsic data (if @c typeNull, no data is specified).
	Nib*			inNib)			//!< The nib from which to instantiate the document's UI.
{
	InitDocument(inName, inDocumentType, inProperties, inData);
	MakeWindows(inNib);
	NotifyContentChanged();
	ShowWindows();
}

// ------------------------------------------------------------------------------------------
/*!	The function takes the following steps:
		
	-# Creates an initial data set for the document from the contents of @a inDocumentUrl.
	-# Creates the document's window(s).
	-# If the document isn't being opened just for printing (i.e., @a inForPrinting is 
	   @c false), displays the window(s).
*/
void
AbstractDocument::InitOpenDocument(
	const Url&			inDocumentUrl, 
	const String&		inDocumentType, 
	Nib*				inNib, 
	bool				inForPrinting)
{
	mOpenForPrinting = inForPrinting;
	
	String	docType(inDocumentType);
	
	ReadDocument(inDocumentUrl, docType);
	DoneReading(inDocumentUrl, docType);
	MakeWindows(inNib);
	NotifyContentChanged();
	
	if (!mOpenForPrinting)
	{
		ShowWindows();
	}
}


// ------------------------------------------------------------------------------------------
/*!	Close a Document.  The behaviour is largely determined by inSaveOption:
	
	- kAEYes:	If inUrl isn't NULL, perform a "Save As";  else, if the 
				document has been modified, perform a "Save".  Then close 
				the document.
	
	- kAENo:	Close the document without saving.
	
	- kAEAsk:	If the document isn't modified, close it without saving;  
				else, ask the user what to do.

	The function returns true if the close was completed prior to returning.
*/
bool
AbstractDocument::CloseDocument(
	OSType					inSaveOption, 
	const Url&				inUrl /* = Url() */, 
	Window*					inWindow /* = NULL */)
{
	bool					done	= false;
	NavAskSaveChangesAction	action;
	
	if (mQuitting)
		action = kNavSaveChangesQuittingApplication;
	else
		action = kNavSaveChangesClosingDocument;
	
	// We only save modified documents.
	if (!IsModified())
		inSaveOption = kAENo;
	
	switch (inSaveOption)
	{
	case kAEYes:
		// Save the document before closing.
		{
			Url			oldUrl(GetUrl());
			String		oldDocumentType(GetDocumentType());
			SaveInfo	saveInfo;
			
			saveInfo.SetFileInfo(oldUrl, oldDocumentType);
			
			if (IsNew() && inUrl.Empty())
			{
				// The file has never been saved, and no file has been given 
				// as an argument.  We need to ask the user for a file name.
				
				saveInfo.SetFromUser(false);
				saveInfo.SetClosing(true);
				saveInfo.SetInvokingWindow(inWindow);
				
				done = AskUserToSave(saveInfo);
			}
			else if (!inUrl.Empty())
			{
				WriteDocumentToUrl(saveInfo);
				WriteComplete(oldUrl, oldDocumentType);
				done = true;
			}
			else if (IsModified())
			{
				saveInfo.SetWriteAttributes(false);
				WriteDocument(saveInfo);
				WriteComplete(oldUrl, oldDocumentType);
				done = true;
			}
		}
		break;
		
	case kAENo:
		// Do not save the document before closing.
		done = true;
		break;
		
	case kAEAsk:
		// Ask the user whether we should save the document.
		done = AskUserToClose(false, inWindow, action);
		break;
	}
	
	if (done)
	{
		Close();
	}
	
	return (done);
}

// ------------------------------------------------------------------------------------------
/*!	@return	@c true if any UI has been confirmed or infirmed by the user (in other words, 
			the UI was displayed modally);  else @ false.
*/
bool
AbstractDocument::AskUserToClose(
	bool					/* inFromUser */, 
	Window*					/* inWindow */, 
	NavAskSaveChangesAction	/* inContext */)
{
	// The default implementation does nothing.
	
	return (true);
}

// ------------------------------------------------------------------------------------------
/*!	@return	@c true if any UI has been confirmed or infirmed by the user (in other words, 
			the UI was displayed modally);  else @ false.
*/
bool
AbstractDocument::AskUserToSave(
	SaveInfo&	/* ioInfo */)
{
	// The default implementation does nothing.
	
	return (true);
}

// ------------------------------------------------------------------------------------------
/*!	@return	@c true if any UI has been confirmed or infirmed by the user (in other words, 
			the UI was displayed modally);  else @ false.
*/
bool
AbstractDocument::AskUserToRevert(
	Window*					/* inWindow */)
{
	// The default implementation does nothing.
	
	return (true);
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::Quitting(
	OSType					/* inSaveOption */)
{
	mQuitting = true;
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::CancelQuitting()
{
	mQuitting = false;
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::NotifyContentChanged()
{
	mContentChangedSignal(this);
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::NotifyActionCancelled()
{
	mAsyncActionCancelledSignal();
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::NotifyUrlChanged()
{
	mUrlChangedSignal(this);
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::MakeWindows(
	Nib*			/* inNib */)
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::ShowWindows()
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::CloseWindows()
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::CloseFiles()
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::InitDocument(
	const String&	/* inName */,
	const String&	/* inDocumentType */, 
	const AEDesc&	/* inProperties */, 
	const AEDesc&	/* inData */)
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::ReadDocument(
	const Url&		/* inDocumentUrl */, 
	String&			/* ioDocumentType */)
{
	// Default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::DoneReading(
	const Url&		/* inDocumentUrl */, 
	const String&	/* inDocumentType */)
{
	// Default implementation does nothing.
}


#pragma mark -


// ------------------------------------------------------------------------------------------
/*!	Make this AbstractDocument the current one.

	Override to do what makes sense for a particular kind of AbstractDocument. In
	most cases, this means selecting the main window for a AbstractDocument.
*/
void
AbstractDocument::MakeCurrent()
{
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::Close()
{
	CloseWindows();
	CloseFiles();
	
	// Notify all observers that the document is closing down.
	mClosingSignal(this);
	
	// Notify the document policy that we need to be deleted.
	mDeleteSignal(this);
	
	// At this point, we've been deleted, so don't reference any member variables or 
	// functions...
}

// ------------------------------------------------------------------------------------------
bool
AbstractDocument::HandleCommand(
	const HICommandExtended&	inHICommand, 
	Window*						inWindow /* = NULL */)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
//	case kHICommandClose:
//		if (IsModified())
//		{
//			// The document has unsaved changes, so ask the user if they want to 
//			// save it.
//			AskUserToClose(true, inWindow, kNavSaveChangesClosingDocument);
//		}
//		else
//		{
//			// We don't need to save, so close the document.
//			SendCloseAppleEvent();
//		}
//		break;
		
	case kHICommandSave:
		if (IsNew())
		{
			SaveInfo	saveInfo;
			
			saveInfo.SetInvokingWindow(inWindow);
			saveInfo.SetContentType(GetDocumentType());
			saveInfo.SetFromUser(true);
			saveInfo.SetClosing(false);
			
			// We don't have a file.  Display a UI asking the user to 
			// supply a file name.  Once that's done, we'll send ourselves 
			// a "Save" Apple %Event for the new file.
			AskUserToSave(saveInfo);
		}
		else if (IsModified())
		{
			// We have a file already, so there's no need to display a UI.
			// Save the file by sending ourselves an Apple %Event.
			SendSaveAppleEvent();
		}
		break;
		
	case kHICommandSaveAs:
		{
			SaveInfo	saveInfo;
			
			saveInfo.SetInvokingWindow(inWindow);
			saveInfo.SetContentType(GetDocumentType());
			saveInfo.SetFromUser(true);
			saveInfo.SetClosing(false);
			
			AskUserToSave(saveInfo);
		}
		break;
		
	case kHICommandRevert:
		if (!IsNew() && IsModified())
		{
			AskUserToRevert(inWindow);
		}
		break;
		
	default:
		handled = false;
		break;
	}

	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
AbstractDocument::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	CommandData&				ioCmdData, 
	Window*						/* inWindow */ /* = NULL */)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandClose:
	case kHICommandSaveAs:
		ioCmdData.SetEnabled(true);
		break;
		
	case kHICommandSave:
		ioCmdData.SetEnabled(IsNew() || IsModified());
		break;
		
	case kHICommandRevert:
		ioCmdData.SetEnabled(!IsNew() && IsModified());
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}


#pragma mark -

// ------------------------------------------------------------------------------------------
void
AbstractDocument::WriteProperty(
	DescType		inPropertyID,
	AEWriter&		ioWriter) const
{
	switch (inPropertyID)
	{
	case pID:
		ioWriter.Write<typeSInt32>(GetUniqueID());
		break;

	case pName:
		ioWriter.Write<typeUTF16ExternalRepresentation>(GetName());
		break;

	case pIsModified:		// Has AbstractDocument changed since last save?
		ioWriter.Write<typeBoolean>(IsModified());
		break;
		
	default:
		AEObject::WriteProperty(inPropertyID, ioWriter);
		break;
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::CloseObject(
	OSType			inSaveOption /* = kAEAsk */,
	const Url&		inUrl /* = Url() */)
{
	CloseDocument(inSaveOption, inUrl, NULL);
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::SaveObject(
	const Url&		inUrl /* = Url() */,
	const String&	inObjectType /* = String() */)
{
	Url			url(inUrl.Empty() ? GetUrl() : inUrl);
	String		documentType(inObjectType.empty() ? GetDocumentType() : inObjectType);
	SaveInfo	saveInfo;
	
	saveInfo.SetFileInfo(url, documentType);
	
	SaveDocument(saveInfo);
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::RevertObject()
{
	if (IsNew())
	{
		// We can't revert if we've never been saved.
		
		B_THROW(AEEventFailedException());
	}
	
	RevertDocument();
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::PrintObject(
	PrintSettings*	inSettings,		//!< The print settings to use.  May be @c NULL.
	Printer*		inPrinter,		//!< The printer to print to.  May be @c NULL.
	bool			inShowDialog)	//!< Should we display the print dialog?
{
	PrintDocument(inSettings, inPrinter, inShowDialog);
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::PrintDocument(
	PrintSettings*	/* inSettings */,	//!< The print settings to use.  May be @c NULL.
	Printer*		/* inPrinter */,	//!< The printer to print to.  May be @c NULL.
	bool			/* inShowDialog */)	//!< Should we display the print dialog?
{
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::DonePrinting()
{
	if (mOpenForPrinting)
	{
		CloseDocument(kAEAsk);
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractDocument::MakeSpecifier(
	AEWriter&		ioWriter) const
{
	BuildUniqueIDSpecifier(GetContainer(), GetClassID(), GetUniqueID(), ioWriter);
}


}	// namespace B
