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

// B headers
#include "BAbstractDocument.h"
#include "BBundle.h"
#include "BCommandData.h"
#include "BDocumentWindow.h"
#include "BEvent.h"
#include "BEventParams.h"
#include "BIcon.h"
#include "BString.h"


namespace B {

// ------------------------------------------------------------------------------------------
template <class DOCUMENT>
DocumentWindow<DOCUMENT>::DocumentWindow(
	WindowRef	inWindowRef,
	DOCUMENT*	inDocument,
	DescType	inClassID /* = kDocWindowKind */)
		: Window(inWindowRef, inDocument->GetAEObjectPtr(), inClassID), 
		  mDocument(inDocument), mWindowRef(inWindowRef), 
		  mEventHandler(mWindowRef), mClosesDocument(true)
{
	InitEventHandler(mEventHandler);
	
	// Register for signals
	
	mClosingConn			= inDocument->GetClosingSignal().connect(
									boost::bind(&DocWindow::DocumentClosing, 
												this, _1));
	
	mUrlChangedConn			= inDocument->GetUrlChangedSignal().connect(
									boost::bind(&DocWindow::DocumentUrlChanged, 
												this, _1));
	
	mDirtyStateChangedConn	= inDocument->GetDirtyStateChangedSignal().connect(
									boost::bind(&DocWindow::DocumentDirtyStateChanged, 
												this, _1));
	
	mContentChangedConn		= inDocument->GetContentChangedSignal().connect(
									boost::bind(&DocWindow::DocumentContentChanged, 
												this, _1));
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT>
DocumentWindow<DOCUMENT>::~DocumentWindow()
{
	// Because the document can outlast us, we need to tear down our connections to its 
	// signals when we are destructed.
	
	mContentChangedConn.disconnect();
	mDirtyStateChangedConn.disconnect();
	mUrlChangedConn.disconnect();
	mClosingConn.disconnect();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::InitEventHandler(EventHandler& ioHandler)
{
	ioHandler.Add(this, &DocumentWindow<DOCUMENT>::CommandProcess);
	ioHandler.Add(this, &DocumentWindow<DOCUMENT>::CommandUpdateStatus);
	
	ioHandler.Init();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::CloseWindow(
	OSType				inSaveOption /* = kAEAsk */, 
	const Url&			inUrl /* = Url() */)
{
	if (ClosesDocument())
	{
		// Closing this window results in closing the document it belongs to.
		// So forward the event to the document.
		
		mDocument->CloseDocument(inSaveOption, inUrl, this);
	}
	else
	{
//		mDocument->WindowClosing();
	}
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::Awaken(Nib* /* inFromNib */)
{
	LoadWindowTitle();
	LoadProxyIcon();
	LoadModified();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::LoadWindowTitle()
{
	SetTitle(GetWindowTitle());
	
	String	altTitle(GetWindowAlternateTitle());
	
	if (!altTitle.empty())
	{
		SetAlternateTitle(altTitle);
	}
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::LoadModified()
{
	SetModified(mDocument->IsModified());
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::LoadProxyIcon()
{
	Url	url	= mDocument->GetUrl();
	
	if (url.Empty())
	{
		// The document has never been saved, so it shouldn't have a proxy icon.
		
		ClearProxy();
	}
	else if (url.IsFileUrl())
	{
		// If the document has a file URL, use it as the proxy instead of the icon 
		// directly.  This allows the Window Manager to extract the icon itself.
		
		SetProxy(url);
	}
	else
	{
		SetProxy(mDocument->GetIcon());
	}
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::LoadContent()
{
	// The default implementation does nothing.
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> String
DocumentWindow<DOCUMENT>::GetWindowTitle()
{
	return (mDocument->GetDisplayName());
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> String
DocumentWindow<DOCUMENT>::GetWindowAlternateTitle()
{
	String	s;
	
	return (s);
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::HandleActivated()
{
	Window::HandleActivated();
	
	mDocument->WindowActivated(this);
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::HandleDeactivated()
{
	mDocument->WindowDeactivated(this);
	
	Window::HandleDeactivated();
}

// ------------------------------------------------------------------------------------------
/*!	Make an Object Specifier for a Window
*/
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::MakeSpecifier(
	AEWriter&		ioWriter) const
{
	BuildIndexSpecifier(mDocument->GetAEObjectPtr(), GetClassID(), GetIndex(), ioWriter);
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> AEObjectPtr
DocumentWindow<DOCUMENT>::GetPropertyObject(
	DescType	inPropertyID) const
{
	AEObjectPtr	property;
	
	switch (inPropertyID)
	{
	case cDocument:
		property = mDocument->GetAEObjectPtr();
		break;

	default:
		property = Window::GetPropertyObject(inPropertyID);
		break;
	}
	
	return (property);
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::DocumentClosing(
	AbstractDocument*	/* inDocument */)
{
	Close();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::DocumentUrlChanged(
	AbstractDocument*	/* inDocument */)
{
	LoadWindowTitle();
	LoadProxyIcon();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::DocumentDirtyStateChanged(
	AbstractDocument*	/* inDocument */)
{
	LoadModified();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> void
DocumentWindow<DOCUMENT>::DocumentContentChanged(
	AbstractDocument*	/* inDocument */)
{
	LoadContent();
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> bool
DocumentWindow<DOCUMENT>::RouteEventToDocument(EventBase& event)
{
	OSStatus	err;
	
	EventParam<kEventParamWindowRef>::Set(event, *this);
	
	err = event.Send(mDocument->GetEventTarget(), std::nothrow);
	
	if ((err != noErr) && (err != eventNotHandledErr))
		B_THROW_STATUS(err);
	
	return (err == noErr);
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> bool
DocumentWindow<DOCUMENT>::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	return (RouteEventToDocument(event));
}

// ------------------------------------------------------------------------------------------
template <class DOCUMENT> bool
DocumentWindow<DOCUMENT>::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	return (RouteEventToDocument(event));
}


}	// namespace B
