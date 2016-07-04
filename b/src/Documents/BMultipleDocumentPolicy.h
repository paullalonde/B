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

#ifndef BMultipleDocumentPolicy_H_
#define BMultipleDocumentPolicy_H_

#if defined(__MWERKS__)
#	pragma once
#endif

// standard headers
#include <list>
#include <memory>

// library headers
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

// B headers
#include "BAbstractDocument.h"
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BFwd.h"
#include "BOSPtr.h"


namespace B {

// forward declarations
class	AEObject;
class	AEObjectSupport;
class	AskReviewDocumentsDialog;
class	Bundle;
class	DialogModality;
class	GetFileDialog;
class	Nib;
class	String;
class	Url;


/*!
	@brief	A document policy for applications supporting multiple open documents.
	
	Use it as the DOC_POLICY template parameter of Application.
	
	@sa		DOC_POLICY.
	
	@todo	Complete documentation.
*/
template <class DOC_FACTORY>
class MultipleDocumentPolicy : public boost::signals::trackable
{
public:

#if DOXYGEN_SCAN
	//! @name Template Parameters
	//@{
	typedef class	DOC_FACTORY;	//!< The document policy's document factory.  Must conform to the DOC_FACTORY protocol.
	//@}
#endif
	
	//! @name Types
	//@{
	//! See DOC_POLICY for description.
	typedef DOC_FACTORY		FactoryType;
	//@}
	
	//! @name Constructor & Destructor
	//@{
	//! See DOC_POLICY for description.
			MultipleDocumentPolicy(
				AEObjectPtr		inApplication, 
				const Bundle&	inBundle, 
				Nib*			inAppNib);
	//! See DOC_POLICY for description.
	virtual	~MultipleDocumentPolicy();
	//@}
	
	//! @name Setup
	//@{
	//! See DOC_POLICY for description.
	virtual void	Init();
	//! See DOC_POLICY for description.
	virtual void	RegisterAppleEvents(AEObjectSupport& ioObjectSupport);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the document policy's document factory.
	FactoryType&		GetFactory()		{ return (mFactory); }
	//@}
	
	//! @name Apple %Event Handlers
	//@{
	//! See DOC_POLICY for description.
	virtual void	OpenApplication();
	//! See DOC_POLICY for description.
	virtual void	ReopenApplication();
	//! See DOC_POLICY for description.
	virtual void	QuitApplication(
						OSType			inSaveOption = kAEAsk);
	//! See DOC_POLICY for description.
	virtual AEObjectPtr
					OpenNewDocument(
						DescType		inDocClass,
						const AEDesc&	inProperties, 
						const AEDesc&	inData);
	//@}
	
	//! @name Document Access
	//@{
	//! See DOC_POLICY for description.
	virtual SInt32		CountDocuments(
							DescType		inDocClass) const;
	//! See DOC_POLICY for description.
	virtual AEObjectPtr	GetDocumentByIndex(
							DescType		inDocClass, 
							unsigned		inIndex) const;
	//! See DOC_POLICY for description.
	virtual AEObjectPtr	GetDocumentByName(
							DescType		inDocClass, 
							const String&	inName) const;
	//! See DOC_POLICY for description.
	virtual AEObjectPtr	GetDocumentByUniqueID(
							DescType		inDocClass, 
							SInt32			inUniqueID) const;
	//@}

protected:
	
	// inquiries
	Nib*	GetAppNib() const	{ return (mAppNib.get()); }
	
	// factories
	//! Creates the "open file" dialog.
	virtual std::auto_ptr<GetFileDialog>
					CreateGetFileDialog(
						UInt32					inCommandID,
						const DialogModality&	inModality);
	virtual std::auto_ptr<AskReviewDocumentsDialog>
					CreateAskReviewDocumentsDialog(
						const DialogModality&	inModality,
						UInt32					inDocumentCount);
	
	virtual void	OpenDocument(
						const AEDesc&									inDirectObject, 
						AEEvent<kCoreEventClass, kAEOpenDocuments>&		ioEvent);
	virtual void	PrintDocument(
						const AEDesc&									inDirectObject, 
						AEEvent<kCoreEventClass, kAEPrintDocuments>&	ioEvent);
	
	// Carbon %Event handlers
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);
	virtual bool	HandleUpdateStatus(
						const HICommandExtended&	inHICommand, 
						CommandData&				ioCmdData);
	
	virtual void	HandleGetFileResult(
						bool					inGood,
						const NavReplyRecord&	inNavReply);
	
private:
	
	// UI-level functionality
	virtual void	NewDocument(UInt32 inCommandID = kHICommandNew);
	virtual void	OpenDocument(UInt32 inCommandID = kHICommandOpen);
	
	// locating open documents
	virtual AbstractDocument*	FindByUrl(const Url& inUrl) const;
	virtual AbstractDocument*	FindByWindow(WindowRef inWindowRef) const;
	
	virtual boost::shared_ptr<AbstractDocument>
					OpenDocumentForUrl(
						const Url&		inUrl, 
						bool			inForPrinting);
	virtual boost::shared_ptr<AbstractDocument>
					CreateDocumentForUrl(
						const Url&		inUrl, 
						bool			inForPrinting);
	virtual boost::shared_ptr<AbstractDocument>
					CreateNewDocument(
						DescType		inDocClass,
						const AEDesc&	inProperties,
						const AEDesc&	inData);
	
	virtual void	SaveAllDocuments();
	virtual void	CloseAllDocuments(
						OSType			inSaveOption = kAEAsk);
	
protected:
	
	virtual void	AddDocument(boost::shared_ptr<AbstractDocument> inDocumentPtr);
	virtual void	RemoveDocument(AbstractDocument* inDocument);
	
	void			AddRecentDocument(AbstractDocument* inDocument);
	virtual void	AddRecentDocumentUrl(const Url& inUrl);
	virtual void	ClearRecentDocuments();
	void			ReadRecentDocuments();
	void			SaveRecentDocuments();
	
	void		SendOpenDocumentsAppleEvent(
					const AEDescList&	inDocList);
	void		SendOpenDocumentAppleEvent(
					const Url&			inUrl);
	virtual void
				GetInfoForNewDocument(
					UInt32				inCommand, 
					DescType&			outClass, 
					AEDesc&				outProperties, 
					AEDesc&				outData);
	
	String		CreateUniqueDocumentName(
					const String&	inTemplate1,
					const String&	inTemplate2,
					const String&	inExtension);
	String		CreateNewDocumentName(
					const String&	inTemplate1,
					const String&	inTemplate2,
					int				inIndex,
					const String&	inExtension);

	UInt32		CountModifiedDocuments() const;
	UInt32		CountUnsavedModifiedDocuments() const;
	
	template <class PRED> UInt32
				CountDocumentsMatching(PRED pred) const;
	template <class PRED> AbstractDocument*
				FindDocumentMatching(PRED pred) const;
				
	DescType	FindDefaultDocClass() const;
	OSType		FindDefaultFileTypeForDocClass(
					DescType			inDocClass) const;
	String		FindDefaultExtensionForDocClass(
					DescType			inDocClass) const;
	
	// notifications
	void		DocumentClosing(
					AbstractDocument*	inDocument);
	void		DocumentDelete(
					AbstractDocument*	inDocument);
	void		DocumentUrlChanged(
					AbstractDocument*	inDocument);
	void		DocumentAsyncActionCancelled();
	
private:
	
	typedef std::map<SInt32, boost::shared_ptr<AbstractDocument> >	DocumentMap;
	typedef DocumentMap::iterator									DocumentIterator;
	typedef DocumentMap::const_iterator								DocumentConstIterator;
	typedef MultipleDocumentPolicy<DOC_FACTORY>						ThisType;
	
	void	InitEventHandler(EventHandler& ioHandler);
	
	void	HandleOpenDocumentAppleEvent(
				const AEDesc&									directObj, 
				AEEvent<kCoreEventClass, kAEOpenDocuments>&		event);
	void	HandlePrintDocumentAppleEvent(
				const AEDesc&									directObj, 
				AEEvent<kCoreEventClass, kAEPrintDocuments>&	event);
	
	Url		GetRecentDocumentUrlFromMenu(
				MenuItemIndex		item);
	void	SetRecentDocumentUrlInMenu(
				MenuItemIndex		item, 
				const Url&			url);
	void	OpenRecentDocument(
				MenuItemIndex		item);

	void	GetFileDialogCallback(
				const GetFileDialog&	inDialog,
				bool					inGood,
				const NavReplyRecord&	inNavReply);
	
	// Carbon %Event handlers
	bool	CommandProcess(
				Event<kEventClassCommand, kEventCommandProcess>&		event);
	bool	CommandUpdateStatus(
				Event<kEventClassCommand, kEventCommandUpdateStatus>&	event);
	bool	BContinueQuit(
				Event<kEventClassB, kEventBContinueQuit>&				event);
	
	// member variables
	boost::weak_ptr<AEObject>	mApplication;
	const Bundle&				mBundle;
	boost::intrusive_ptr<Nib>	mAppNib;
	DOC_FACTORY					mFactory;
	EventHandler				mEventHandler;
	MenuRef						mRecentDocumentsMenu;
	bool						mHasRecentDocuments;
	DocumentMap					mDocuments;
	OSType						mQuitOption;
	SInt32						mLastDocumentID;
};

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> template <class PRED> AbstractDocument*
MultipleDocumentPolicy<DOC_FACTORY>::FindDocumentMatching(PRED pred) const
{
	AbstractDocument*		document	= NULL;
	DocumentConstIterator	it;
	
	it = std::find_if(mDocuments.begin(), mDocuments.end(), pred);
	
	if (it != mDocuments.end())
		document = it->second.get();
	
	return (document);
}

// ------------------------------------------------------------------------------------------
template <class DOC_FACTORY> template <class PRED> UInt32
MultipleDocumentPolicy<DOC_FACTORY>::CountDocumentsMatching(PRED pred) const
{
	return (std::count_if(mDocuments.begin(), mDocuments.end(), pred));
}

}	// namespace B


#endif	// BMultipleDocumentPolicy_H_
