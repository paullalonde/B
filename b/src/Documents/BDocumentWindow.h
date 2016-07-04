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

#ifndef BDocumentWindow_H_
#define BDocumentWindow_H_

#pragma once

// B headers
#include "BEventCustomParams.h"
#include "BWindow.h"


namespace B {

// forward declarations
class	AbstractDocument;
class	NavDialog;


/*!	@brief	Document-related window class.
	
	@todo	%Document this class!
*/
template <class DOCUMENT>
class DocumentWindow : public Window
{
public:
	
	// types
	typedef DocumentWindow<DOCUMENT>	DocWindow;
	
	// constants
	enum			{ kDocWindowKind = 'DocW' };
	
	//! @name Constructors
	//@{
	//! Constructor.  In B, Window objects are created @e after their @c WindowRef.
					DocumentWindow(
						WindowRef	inWindowRef,
						DOCUMENT*	inDocument,
						DescType	inClassID = kDocWindowKind);
	//@}
	
	// UI-level
	virtual void	CloseWindow(
						OSType			inSaveOption = kAEAsk, 
						const Url&		inUrl = Url());
	
	virtual bool	ClosesDocument() const;
	virtual void	ClosesDocument(bool inClosesDoc);
	
	// overrides from AEObject
	virtual AEObjectPtr
					GetPropertyObject(
						DescType		inPropertyID) const;

	DOCUMENT*		GetDocument() const	{ return (mDocument); }
	
protected:
	
	//! @name Object Destruction
	//@{
	//! Destructor.
	virtual			~DocumentWindow();
	//@}
	
	virtual void	Awaken(Nib* inFromNib);
	virtual void	LoadWindowTitle();
	virtual void	LoadModified();
	virtual void	LoadProxyIcon();
	virtual void	LoadContent();
	
	virtual String	GetWindowTitle();
	virtual String	GetWindowAlternateTitle();
	
	// overrides from AEObject
	virtual void	MakeSpecifier(
						AEWriter&		ioWriter) const;
	
	// Carbon %Event handlers
	virtual void	HandleActivated();
	virtual void	HandleDeactivated();
	
	// notifications
	void		DocumentClosing(
					AbstractDocument*	inDocument);
	void		DocumentUrlChanged(
					AbstractDocument*	inDocument);
	void		DocumentDirtyStateChanged(
					AbstractDocument*	inDocument);
	void		DocumentContentChanged(
					AbstractDocument*	inDocument);
	
	// member variables
	DOCUMENT*	mDocument;

private:
	
	typedef boost::signals::connection	Connection;
	
	void	InitEventHandler(EventHandler& ioHandler);
	
	bool	RouteEventToDocument(EventBase& event);
	
	// Carbon %Event handlers
	bool	CommandProcess(
				Event<kEventClassCommand, kEventCommandProcess>&		event);
	bool	CommandUpdateStatus(
				Event<kEventClassCommand, kEventCommandUpdateStatus>&	event);
	
	// member variables
	WindowRef		mWindowRef;
	EventHandler	mEventHandler;
	Connection		mClosingConn;
	Connection		mUrlChangedConn;
	Connection		mDirtyStateChangedConn;
	Connection		mContentChangedConn;
	bool			mClosesDocument;
};

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> inline bool
DocumentWindow<DOCUMENT>::ClosesDocument() const
{
	return (mClosesDocument);
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> inline void
DocumentWindow<DOCUMENT>::ClosesDocument(bool inClosesDoc)
{
	mClosesDocument = inClosesDoc;
}

}	// namespace B


#endif	// BDocumentWindow_H_
