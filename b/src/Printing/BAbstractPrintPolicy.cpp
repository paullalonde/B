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
#include "BAbstractPrintPolicy.h"

// B headers
#include "BErrorHandler.h"
#include "BEvent.h"
#include "BEventHandler.h"
#include "BNavDialogs.h"
#include "BPageFormat.h"
#include "BPrintSession.h"
#include "BPrintSettings.h"
#include "BWindow.h"


namespace B {

// ------------------------------------------------------------------------------------------
AbstractPrintPolicy::AbstractPrintPolicy(
	EventTargetRef	inTarget, 
	const Bundle&	inBundle)
		: mEventHandler(inTarget), mBundle(inBundle), mPageFormat(NULL), mPrintEnabled(true), 
		  mPageSetupInitCallback(DefaultDialogInit), 
		  mPageSetupResultCallback(DefaultDialogResult),
		  mPrintInitCallback(DefaultDialogInit), 
		  mPrintResultCallback(DefaultDialogResult)
{
	InitEventHandler();
}

// ------------------------------------------------------------------------------------------
AbstractPrintPolicy::~AbstractPrintPolicy()
{
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::InitEventHandler()
{
	mEventHandler.Add(this, &AbstractPrintPolicy::CommandProcess);
	mEventHandler.Add(this, &AbstractPrintPolicy::CommandUpdateStatus);
	mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
PageFormat&
AbstractPrintPolicy::GetPageFormat()
{
	PrintSession	session;
	
	EnsurePageFormat(session);
	
	if (session.Validate(*mPageFormat))
		mPageFormatChangedSignal(*mPageFormat);
	
	return *mPageFormat;
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::EnsurePageFormat(PrintSession& ioPrintSession)
{
	if (mPageFormat.get() == NULL)
	{
		mPageFormat.reset(new PageFormat(ioPrintSession.MakePageFormat()));
		FillPageFormat(*mPageFormat);
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::ValidatePageFormat()
{
	boost::scoped_ptr<PrintSession>	sessionPtr;
	PrintSession*					session;
	
	if (mPrintSession.get() != NULL)
	{
		session = mPrintSession.get();
	}
	else
	{
		sessionPtr.reset(new PrintSession);
		session = sessionPtr.get();
	}
	
	if (session->Validate(*mPageFormat))
		mPageFormatChangedSignal(*mPageFormat);
}

// ------------------------------------------------------------------------------------------
PrintSettings&
AbstractPrintPolicy::GetPrintSettings()
{
	PrintSession	session;
	
	EnsurePrintSettings(session);
	
	if (session.Validate(*mPrintSettings))
		mPrintSettingsChangedSignal(*mPrintSettings);
	
	return *mPrintSettings;
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::EnsurePrintSettings(PrintSession& ioPrintSession)
{
	if (mPrintSettings.get() == NULL)
	{
		mPrintSettings.reset(new PrintSettings(ioPrintSession.MakePrintSettings()));
		
		FillPrintSettings(*mPrintSettings);
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::ValidatePrintSettings()
{
	boost::scoped_ptr<PrintSession>	sessionPtr;
	PrintSession*					session;
	
	if (mPrintSession.get() != NULL)
	{
		session = mPrintSession.get();
	}
	else
	{
		sessionPtr.reset(new PrintSession);
		session = sessionPtr.get();
	}
	
	if (session->Validate(*mPrintSettings))
		mPrintSettingsChangedSignal(*mPrintSettings);
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::ShowPageSetupDialog()
{
	mPrintSession.reset(new PrintSession);
	
	EnsurePageFormat(*mPrintSession);
	mSavedPageFormat.reset(new PageFormat(mPageFormat->copy()));
	
	mPageSetupInitCallback(*mPrintSession);
	mPrintSession->Validate(*mPageFormat);
	
	InteractWithUserDialog	interactionDialog;
	
	interactionDialog.Run();
	
	mPrintSession->PageSetupDialog(*mPageFormat, 
		boost::bind(&AbstractPrintPolicy::ForwardPageSetupResult, this, _2));
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::ForwardPageSetupResult(
	bool			inAccepted)
{
	B_ASSERT(mPrintSession.get() != NULL);
	B_ASSERT(mPageFormat.get() != NULL);
	
	if ((mSavedPageFormat.get() != NULL) && !mPageFormat->Equals(*mSavedPageFormat))
		mPageFormatChangedSignal(*mPageFormat);
	
	mSavedPageFormat.reset();
	mPageSetupResultCallback(*mPrintSession, inAccepted);
	mPrintSession.reset();
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::Print(
	PrintSettings*	inPrintSettings,	//!< The settings to use for this print job; may be @c NULL.
	Printer*		inPrinter,			//!< The destination printer; may be @c NULL.
	bool			inShowDialog)		//!< Should the Print dialog be displayed?
{
	mPrintSession.reset(new PrintSession);
	
	EnsurePageFormat(*mPrintSession);
	EnsurePrintSettings(*mPrintSession);
	if (inPrintSettings != NULL)
		*mPrintSettings = *inPrintSettings;
	mSavedPrintSettings.reset(new PrintSettings(mPrintSettings->copy()));
	
	if (inPrinter != NULL)
		mPrintSession->SetPrinter(*inPrinter);
	
	mPrintInitCallback(*mPrintSession);
	mPrintSession->Validate(*mPageFormat);
	mPrintSession->Validate(*mPrintSettings);
	
	if (inShowDialog)
	{
		InteractWithUserDialog	interactionDialog;
		
		interactionDialog.Run();
		
		mPrintSession->PrintDialog(*mPrintSettings, *mPageFormat, 
								   boost::bind(&AbstractPrintPolicy::ForwardPrintResult, 
											   this, _2));
	}
	else
	{
		ForwardPrintResult(true);
	}
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::ForwardPrintResult(
	bool				inAccepted)
{
	B_ASSERT(mPrintSession.get() != NULL);
	B_ASSERT(mPrintSettings.get() != NULL);
	B_ASSERT(mPageFormat.get() != NULL);
	
	if ((mSavedPrintSettings.get() != NULL) && !mPrintSettings->Equals(*mSavedPrintSettings))
		mPrintSettingsChangedSignal(*mPrintSettings);
	
	mSavedPrintSettings.reset();
	mPrintResultCallback(*mPrintSession, inAccepted);
	mPrintSession.reset();
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::DefaultDialogInit(
	PrintSession&		/* ioPrintSession */)
{
}

// ------------------------------------------------------------------------------------------
void
AbstractPrintPolicy::DefaultDialogResult(
	PrintSession&		/* ioPrintSession */, 
	bool				/* inAccepted */)
{
}

// ------------------------------------------------------------------------------------------
bool
AbstractPrintPolicy::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandPageSetup:
		ShowPageSetupDialog();
		break;
		
	case kHICommandPrint:
		if (mPrintEnabled)
			Print(NULL, NULL, true);
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
AbstractPrintPolicy::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	CommandData&				ioCmdData)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandPageSetup:
		ioCmdData.SetEnabled(true);
		break;
		
	case kHICommandPrint:
		ioCmdData.SetEnabled(mPrintEnabled);
		break;
		
	default:
		handled = false;
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
bool
AbstractPrintPolicy::CommandProcess(
	Event<kEventClassCommand, kEventCommandProcess>&	event)
{
	return (this->HandleCommand(event.mHICommand));
}

// ------------------------------------------------------------------------------------------
bool
AbstractPrintPolicy::CommandUpdateStatus(
	Event<kEventClassCommand, kEventCommandUpdateStatus>&	event)
{
	return (this->HandleUpdateStatus(event.mHICommand, event.mData));
}


}	// namespace B
