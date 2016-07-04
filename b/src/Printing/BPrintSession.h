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

#ifndef BPrintSession_H_
#define BPrintSession_H_

#pragma once

// B headers
#include "BAutoUPP.h"
#include "CFUtils.h"
#include "BDialogModality.h"


namespace B {

// forward declarations
class	PageFormat;
class	Printer;
class	PrintSettings;
class	String;
class	Url;
namespace Graphics {
	class	Context;
}

// ==========================================================================================
//	PrintSession

/*!
	@brief	Encapsulates a page format for printing.
	
	@todo	%Document this class!
*/
class PrintSession : public boost::noncopyable
{
public:
	
	//! @name Types
	//@{
	//! The completion callback for the Page Setup and Print dialogs.
	typedef boost::function2<void, PrintSession&, bool>	DialogDoneCallback;
	//@}
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.
	PrintSession();
	//! Destructor.
	~PrintSession();
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns a list of printers available in this printing session.
	void		GetPrinters(std::vector<String>& outNames, size_t& outCurrent) const;
	//! Returns the current printer associated with this printing session.
	Printer		GetPrinter() const;
	//! Retrieve the paper sizes available on @a inPrinter.
	void		GetPageFormats(const Printer& inPrinter, std::vector<PageFormat>& outPageFormats) const;
	//! Returns the spool file formats that can be generated for this printing session.
	void		GetDocumentFormats(std::vector<String>& outFormats) const;
	//! Returns the destination type for a print job.
	PMDestinationType
				GetDestinationType(const PrintSettings& inPrintSettings) const;
	//! Returns the destination format for a print job.
	String		GetDestinationFormat(const PrintSettings& inPrintSettings) const;
	//! Returns the destination location for a print job.
	Url			GetDestinationLocation(const PrintSettings& inPrintSettings) const;
	//! Retrieve the destination formats supported by the current print destination.
	void		GetOutputFormats(PMDestinationType inDestinationType, std::vector<String>& outOutputFormats) const;
	//! Returns the current result code for the printing session.
	OSStatus	GetError() const;
	//! Returns data the application previously stored in the printing session object.
	CFTypeRef	GetData(CFStringRef inKey) const;
	//! Returns data the application previously stored in the printing session object.
	template <typename T>
	OSPtr<T>	GetSessionData(CFStringRef inKey) const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Sets the print dialogs' modality.
	void	SetModality(const DialogModality& inModality);
	//! Changes the current printer for this printing session to the printer whose name is specified by @a inName.
	void	SetPrinter(CFStringRef inPrinterName);
	//! Changes the current printer for this printing session to the printer specified by @a inPrinter.
	void	SetPrinter(const Printer& inPrinter);
	//! Specifies the spool file format and the graphics context type to use for drawing pages within the print loop.
	void	SetDocumentFormat(CFStringRef inDocFormat, CFStringRef inGraphicsType);
	//! Sets the destination location, format, and type for a print job.
	void	SetDestination(const PrintSettings& inPrintSettings, PMDestinationType inDestinationType, const String& inOutputFormat, const Url& inLocation);
	//! Sets the current result code for the printing session.
	void	SetError(OSStatus inError);
	//! Controls whether the simplified print dialog will be displayed.
	void	ShowPrinterPresets(bool showThem);
	//! Controls whether the status dialog will be displayed during the print loop.
	void	ShowStatusDialog(bool showIt);
	//! Stores application-specific data in the printing session object.
	void	SetData(CFStringRef inKey, CFTypeRef inData);
	//! Stores application-specific data in the printing session object.
	template <typename T>
	void	SetSessionData(CFStringRef inKey, OSPtr<T> inData);
	//@}
	
	//! @name Defaults & Validation
	//@{
	//! Assigns default parameter values to @a ioPageFormat.
	void	Default(PageFormat& ioPageFormat);
	//! Assigns default parameter values to @a ioPrintSettings.
	void	Default(PrintSettings& ioPrintSettings);
	//! Updates the values in @a ioPageFormat and validates them against the current formatting printer.
	bool	Validate(PageFormat& ioPageFormat);
	//! Validates  @a ioPageFormat within the context of the printing session.
	bool	Validate(PrintSettings& ioPrintSettings);
	//! Creates a PageFormat object and gives it default values for this printing session.
	PageFormat		MakePageFormat();
	//! Creates a PrintSettings object and gives it default values for this printing session.
	PrintSettings	MakePrintSettings();
	//@}
	
	//! @name Dialogs
	//@{
	//! Displays the Page Setup dialog and records the user's selections in @a ioPageFormat.
	void	PageSetupDialog(
				PageFormat&			ioPageFormat, 
				DialogDoneCallback	inCallback);
	//! Displays the Print dialog and records the user's selections in @a ioPrintSettings.
	void	PrintDialog(
				PrintSettings&		ioPrintSettings, 
				const PageFormat&	inPageFormat, 
				DialogDoneCallback	inCallback);
	//@}
	
	//! @name Print Loop
	//@{
	//! Begins a print job.
	void	BeginDocument(
				const PrintSettings&	inPrintSettings, 
				const PageFormat&		inPageFormat);
	//! Ends a print job started by BeginDocument().
	void	EndDocument();
	//! Starts a new page for printing in the printing session.
	void	BeginPage(
				const PageFormat&		inPageFormat);
	//! Indicates the end of drawing the current page for the printing session.
	void	EndPage();
	//! Returns the CoreGraphics context for the current page.
	Graphics::Context
			GetPageContext();
	//@}
	
private:
	
	typedef OSStatus	(*BeginDocumentProc)(
							PMPrintSession	printSession,
							PMPrintSettings	printSettings,
							PMPageFormat	pageFormat);
	typedef OSStatus	(*EndDocumentProc)(
							PMPrintSession	printSession);
	typedef OSStatus	(*BeginPageProc)(
							PMPrintSession	printSession,
							PMPageFormat	pageFormat, 
							const PMRect*	unused);
	typedef OSStatus	(*EndPageProc)(
							PMPrintSession	printSession);
	
	void	PageSetupDialogDone(Boolean accepted);
	void	PrintDialogDone(Boolean accepted);
	
	static PrintSession*
				GetSessionFromPMSession(
					PMPrintSession	printSession);
	void		AddSessionToPMSession();
	
	static pascal void
			PageSetupSheetDoneProc(
				PMPrintSession	printSession, 
				WindowRef		documentWindow, 
				Boolean			accepted);
	static pascal void
			PrintSheetDoneProc(
				PMPrintSession	printSession, 
				WindowRef		documentWindow, 
				Boolean			accepted);

	// member variables
	PMPrintSession		mPrintSession;
	bool				mInDocument;
	bool				mInPage;
	DialogModality		mModality;
	DialogDoneCallback	mPageSetupDialogCallback;
	DialogDoneCallback	mPrintDialogCallback;
	BeginDocumentProc	mBeginDocument;
	EndDocumentProc		mEndDocument;
	BeginPageProc		mBeginPage;
	EndPageProc			mEndPage;
	
	// static member variables
	static const AutoPMSheetDoneUPP	sPageSetupSheetDoneUPP;
	static const AutoPMSheetDoneUPP	sPrintSheetDoneUPP;
};

// ------------------------------------------------------------------------------------------
template <typename T> inline OSPtr<T>
PrintSession::GetSessionData(
	CFStringRef	inKey)		//!< CF string uniquely identifying the data of interest.
	const
{
	CFTypeRef	ref	= GetData(inKey);
	OSPtr<T>	ptr	= CFUMakeTypedValue<T>(ref);
	
	return (ptr);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
PrintSession::SetSessionData(
	CFStringRef	inKey,		//!< CF string uniquely identifying the data of interest.
	OSPtr<T>	inData)		//!< The new value.
{
	SetData(inKey, inData.get());
}

// ------------------------------------------------------------------------------------------
inline OSStatus
PrintSession::GetError() const
{
	return (PMSessionError(mPrintSession));
}


// ==========================================================================================
//	AutoPrintDocument

class AutoPrintDocument : public boost::noncopyable
{
public:
	
	AutoPrintDocument(
		PrintSession&			ioPrintSession,
		const PrintSettings&	inPrintSettings, 
		const PageFormat&		inPageFormat);
	~AutoPrintDocument();
	
private:
	
	PrintSession&	mPrintSession;
};


// ==========================================================================================
//	AutoPrintPage

class AutoPrintPage : public boost::noncopyable
{
public:
	
	AutoPrintPage(
		PrintSession&		ioPrintSession, 
		const PageFormat&	inPageFormat);
	~AutoPrintPage();
	
private:
	
	PrintSession&	mPrintSession;
};


}	// namespace B


#endif	// BPrintSession_H_
