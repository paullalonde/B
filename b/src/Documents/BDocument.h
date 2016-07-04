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

#ifndef BDocument_H_
#define BDocument_H_

#pragma once

// standard headers
#include <iosfwd>

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BAbstractDocument.h"
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BFwd.h"
#include "BIcon.h"
#include "BNavDialogs.h"
#include "BString.h"

#include "BPrintPolicyConcept.h"
#include "BUndoPolicyConcept.h"


namespace B {

// forward declarations
class	PageFormat;
class	PrintSession;
class	Window;


struct DocumentSheetState 
{
	DocumentSheetState(WindowRef window, bool user, bool closing = false)
		: mWindow(window), mFromUser(user), mClosing(closing)  {}
	
	WindowRef	mWindow;
	bool		mFromUser;
	bool		mClosing;
};


/*!	@brief	A concrete %Document class
	
	@todo	%Document this class!
*/
template <class UNDO_POLICY, class PRINT_POLICY>
class Document : public AbstractDocument
{
public:
	
	// Concept checks.
	BOOST_CLASS_REQUIRE(UNDO_POLICY, B, UndoPolicyConcept);
	BOOST_CLASS_REQUIRE(PRINT_POLICY, B, PrintPolicyConcept);
	
	// types
	typedef UNDO_POLICY							UndoPolicyType;
	typedef PRINT_POLICY						PrintPolicyType;
	typedef Document<UNDO_POLICY, PRINT_POLICY>	DocType;
	typedef DocumentWindow<DocType>				WindowType;
	
	// constructor / destructor
					Document(
						AEObjectPtr		inContainer,
						DescType		inClassID, 
						SInt32			inUniqueID);
	virtual			~Document();
	
	// inquiries
	virtual bool		OwnsWindow(const Window* inWindow) const;
	virtual bool		OwnsFrontmostDocumentWindow() const;
	virtual Window*		GetFrontMostDocumentWindow() const;
	virtual String		GetDisplayName() const;
	virtual Url			GetUrl() const;
	virtual String		GetDocumentType() const;
	virtual Icon		GetIcon() const;
	UndoPolicyType&		GetUndoPolicy();
	PrintPolicyType&	GetPrintPolicy();
	
	// document management
	virtual void	MakeCurrent();
	virtual bool	AskUserToClose(
						bool			inFromUser, 
						Window*			inWindow, 
						NavAskSaveChangesAction	inContext);
	virtual bool	AskUserToSave(
						SaveInfo&		ioSaveInfo);
	virtual void	WriteDocument(
						SaveInfo&		ioSaveInfo);
	virtual void	WriteDocumentToUrl(
						SaveInfo&		ioSaveInfo);
	virtual void	WriteComplete(
						const Url&		inOldUrl, 
						const String&	inOldDocumentType);
	virtual bool	AskUserToRevert(
						Window*			inWindow);
	virtual void	Quitting(
						OSType			inSaveOption);
	virtual void	PrintDocument(
						PrintSettings*	inPrintSettings,
						Printer*		inPrinter,
						bool			inShowDialog);
	
	// document/window interaction
	virtual void	WindowActivated(Window* inWindow);
	virtual void	WindowDeactivated(Window* inWindow);
	//! Determines the modality to use for document dialogs & alerts.
	virtual DialogModality
					GetDocumentDialogModality(
						Window*			inWindowHint);
	
	// overrides from AEObject
	virtual String	GetName() const;
	virtual size_t	CountElements(
						DescType		inElementType) const;
	virtual AEObjectPtr
					GetElementByIndex(
						DescType		inElementType, 
						size_t			inIndex) const;
	
	//! @name Configuration
	//@{
	//! If @a clearsIt is true, saving the document will clear the undo stack.
	void			SaveClearsUndo(bool clearsIt)	{ mSaveClearsUndo = clearsIt; }
	//@}
	
protected:
	
	virtual const char*	GetWindowNibName() const;
	virtual void	MakeWindows(
						Nib*			inNib);
	virtual void	ShowWindows();
	virtual void	InitDocument(
						const String&	inName,
						const String&	inDocumentType, 
						const AEDesc&	inProperties, 
						const AEDesc&	inData);
	virtual void	ReadDocument(
						const Url&		inDocumentUrl, 
						String&			ioDocumentType);
	virtual void	DoneReading(
						const Url&		inDocumentUrl, 
						const String&	inDocumentType);
	virtual void	SaveDocumentWithNavReply(
						NavReplyRecord&	ioReply, 
						const SaveInfo&	inSaveInfo);
	//! Reads the document from @a inFileUrl.
	virtual void	ReadDocumentFromFile(
						const Url&		inFileUrl, 
						String&			ioDocumentType);
	virtual void	WriteDocumentToFile(
						SaveInfo&		ioSaveInfo);
	virtual void	WriteDocumentToNewFile(
						SaveInfo&		ioSaveInfo);
	virtual void	WriteDocumentToExistingFile(
						SaveInfo&		ioSaveInfo);
	virtual void	WriteDocumentCommon(
						SaveInfo&		ioSaveInfo);
	virtual bool	CreateDocumentFile(
						const Url&		inFileUrl, 
						FSRef&			outFileRef);
	//! Returns an @c std::istream suitable for reading @a inDocumentUrl.
	virtual std::auto_ptr<std::istream>
					MakeDocumentReadStream(
						const Url&		inDocumentUrl,
						const String&	inDocumentType) const;
	//! Returns an @c std::ostream suitable for writing the document.
	virtual std::auto_ptr<std::ostream>
					MakeDocumentWriteStream(
						SaveInfo&		ioSaveInfo) const;
	//! Reads the document from @a ioStream.
	virtual void	ReadDocumentFromStream(
						std::istream&				ioStream, 
						String&						ioDocumentType);
	virtual void	WriteDocumentToStream(
						std::ostream&				ioStream, 
						SaveInfo&					ioSaveInfo);
	//! Reads the document from @a inBuffer.
	virtual void	ReadDocumentFromBuffer(
						const std::vector<UInt8>&	inBuffer, 
						String&						ioDocumentType);
	virtual void	WriteDocumentToBuffer(
						std::vector<UInt8>&			outBuffer, 
						SaveInfo&					ioSaveInfo);
	virtual void	StartSafeFileSave(
						const FSRef&				inDstRef, 
						FSRef&						outTempFileRef, 
						bool&						outExtensionHidden);
	virtual void	CompleteSafeFileSave(
						const FSRef&				inDstRef, 
						const FSRef&				inTempFileRef, 
						bool						inExtensionHidden);
	virtual void	WriteFileAttributes(
						SaveInfo&					ioSaveInfo);
	virtual void	RecalcDisplayName();

	//! @name Printing
	//@{
	virtual void	StartPrinting(
						PrintSession&				ioPrintSession, 
						const PrintSettings&		inPrintSettings,
						const PageFormat&			inPageFormat);
	virtual void	CalcPrintingPageRange(
						PrintSession&				ioPrintSession, 
						const PrintSettings&		inPrintSettings,
						const PageFormat&			inPageFormat, 
						size_t&						outStartPage,
						size_t&						outEndPage);
	virtual void	GetPrintingPageRange(
						size_t&						outMinPage,
						size_t&						outMaxPage, 
						bool						inForDialog);
	virtual void	BeginPrintingDocument(
						PrintSession&				ioPrintSession, 
						const PrintSettings&		inPrintSettings,
						const PageFormat&			inPageFormat, 
						size_t						inStartPage,
						size_t						inEndPage);
	virtual void	BeginPrintingPage(
						PrintSession&				ioPrintSession, 
						const PageFormat&			inPageFormat, 
						size_t						inPage);
	virtual void	PrintPage(
						PrintSession&				ioPrintSession, 
						const PageFormat&			inPageFormat, 
						size_t						inPage);
	//@}
	
	//! @name Dialog Factory Methods
	//@{
	//! Returns a fully configured PutFileDialog.
	virtual std::auto_ptr<PutFileDialog>
                    CreateSaveDialog(
						const DialogModality&	inModality,
						const SaveInfo&			inInfo);
	//! Returns a fully configured AskSaveChangesDialog.
	virtual std::auto_ptr<AskSaveChangesDialog>
                    CreateAskSaveChangesDialog(
						const DialogModality&	inModality,
						bool					inFromUser, 
						AskSaveChangesDialog::Context	inContext);
	//! Returns a fully configured AskDiscardChangesDialog.
	virtual std::auto_ptr<AskDiscardChangesDialog>
                    CreateAskDiscardChangesDialog(
						const DialogModality&	inModality,
						bool					inFromUser);
	//@}
	
	virtual void	HandleSaveResult(
						bool						inGoodSave,
						NavReplyRecord&				ioNavReply, 
						const SaveInfo&				inSaveInfo);
	virtual void	HandleAskSaveChangesResult(
						AskSaveChangesDialog::Action inAction,
						const DocumentSheetState&	inState);
	virtual void	HandleAskDiscardChangesResult(
						bool						inDiscard, 
						const DocumentSheetState&	inState);

	virtual void	HandlePageSetupDialogInit(
						PrintSession&				ioPrintSession);
	virtual void	HandlePageSetupDialogResult(
						PrintSession&				ioPrintSession,
						bool						inAccepted);
	virtual void	HandlePrintDialogInit(
						PrintSession&				ioPrintSession);
	virtual void	HandlePrintDialogResult(
						PrintSession&				ioPrintSession,
						bool						inAccepted);
	virtual void	HandlePageFormatChanged(
						PageFormat&					ioPageFormat);
	
	virtual void	UndoManagerModified();
	virtual void	UndoManagerUnmodified();
	
	// member variables
	String			mName;
	String			mDisplayName;
	Window*			mWindow;
	Url				mUrl;
	String			mDocumentType;
	OSType			mQuittingSaveOption;
	bool			mShowingCloseUI;
	bool			mSaveClearsUndo;
	
private:
	
	void	InitEventHandler(EventHandler& ioHandler);

	void	AskSaveChangesDialogCallback(
				const AskSaveChangesDialog&		inDialog,
				AskSaveChangesDialog::Action	inAction, 
				const DocumentSheetState&		inState);
	void	AskDiscardChangesDialogCallback(
				const AskDiscardChangesDialog&	inDialog,
				bool							inDiscard, 
				const DocumentSheetState&		inState);
	void	PutFileDialogCallback(
				const PutFileDialog&			inDialog,
				bool							inGood,
				NavReplyRecord&					ioNavReply,
				const SaveInfo&					inSaveInfo);
	void	PageSetupDialogInitCallback(
				PrintSession&					ioPrintSession);
	void	PageSetupDialogResultCallback(
				PrintSession&					ioPrintSession,
				bool							inAccepted);
	void	PrintDialogInitCallback(
				PrintSession&					ioPrintSession);
	void	PrintDialogResultCallback(
				PrintSession&					ioPrintSession,
				bool							inAccepted);
	void	PageFormatChangedCallback(
				PageFormat&						ioPageFormat);
	
	// Carbon %Event handlers
	bool			BCloseDocument(
						Event<kEventClassB, kEventBCloseDocument>&		event);
	bool			BSaveDocument(
						Event<kEventClassB, kEventBSaveDocument>&		event);
	
	// member variables
	EventHandler	mEventHandler;
	UndoPolicyType	mUndoPolicy;
	PrintPolicyType	mPrintPolicy;
};

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY>
inline typename Document<UNDO_POLICY, PRINT_POLICY>::UndoPolicyType&
Document<UNDO_POLICY, PRINT_POLICY>::GetUndoPolicy()
{
	return (mUndoPolicy);
}

// ------------------------------------------------------------------------------------------
template <class UNDO_POLICY, class PRINT_POLICY>
inline typename Document<UNDO_POLICY, PRINT_POLICY>::PrintPolicyType&
Document<UNDO_POLICY, PRINT_POLICY>::GetPrintPolicy()
{
	return (mPrintPolicy);
}


}	// namespace B


#endif	// BDocument_H_
