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

#ifndef BPrinter_H_
#define BPrinter_H_

#pragma once

// B headers
#include "BOSPtr.h"
#include "BString.h"


namespace B {

// forward declarations
class	Url;


/*!
	@brief	Encapsulates a printer.
	
	@todo	%Document this class!
*/
class Printer
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Copy constructor.
				Printer(const Printer& inPrinter);
	//! @c OSPtr<PMPrinter> constructor.
	explicit	Printer(const OSPtr<PMPrinter>& inPrinter);
	//! @c PMPrinter constructor.
	explicit	Printer(PMPrinter inPrinter);
	//! @c PMPrinter constructor.
	explicit	Printer(PMPrinter inPrinter, const from_copy_t&);
	//! Destructor.
				~Printer();
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	Printer&	operator = (const Printer& inPrinter);
	//! Copy assignment.
	Printer&	operator = (const OSPtr<PMPrinter>& inPrinter);
	//@}
	
	//! @name Copying
	//@{
	//! Creates a distinct (i.e., non-shared) copy of the printer.
	Printer		copy() const;
	//! Exchanges the contents of @c *this and @a ioPrinter.
	void		swap(Printer& ioPrinter);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the printer's name.
	String		GetName() const;
	//! Returns the printer's unique identifier.
	String		GetIdentifier() const;
	//! Returns @c true if this is the default printer.
	bool		IsDefault() const;
	//! Returns @c true if the printer is in the user's favorite printer list.
	bool		IsFavorite() const;
	//! Returns the printer's location.
	String		GetLocation() const;
	//! Returns an informative string about the printer's make and model.
	String		GetMakeAndModel() const;
	//! Returns the printer's description file.
	Url			GetDescriptionUrl() const;
	//! Returns the printer's device.
	Url			GetDeviceUrl() const;
	//! Returns information about the printer's imaging language.
	void		GetLanguageInfo(PMLanguageInfo& outInfo) const;
	//! Returns @c true if this is a PostScript printer.
	bool		IsPostScript() const;
	//! Returns the number of resolution settings supported by the printer.
	unsigned	CountResolutions() const;
	//! Returns a resolution setting based on an index into the range of settings supported by the printer.
	PMResolution	GetResolution(unsigned index) const;
	//! Returns a "logical" resolution setting for the printer.
	PMResolution	GetTaggedResolution(PMTag inTag) const;
	//! Returns the printer's location.
	PMPrinterState	GetState() const;
	//@}
	
private:
	
	// member variables
	OSPtr<PMPrinter>	mPrinter;
	
	// friends
	friend class	PrintSession;
};

// ------------------------------------------------------------------------------------------
inline
Printer::Printer(
	const Printer&	inPrinter)	//!< The input printer.
		: mPrinter(inPrinter.mPrinter)
{
}

// ------------------------------------------------------------------------------------------
inline
Printer::Printer(
	const OSPtr<PMPrinter>&	inPrinter)	//!< The input printer.
		: mPrinter(inPrinter)
{
}

// ------------------------------------------------------------------------------------------
inline
Printer::Printer(
	PMPrinter	inPrinter)	//!< The input printer.
		: mPrinter(inPrinter)
{
}

// ------------------------------------------------------------------------------------------
inline
Printer::Printer(
	PMPrinter			inPrinter,	//!< The input printer.
	const from_copy_t&	fc)
		: mPrinter(inPrinter, fc)
{
}

// ------------------------------------------------------------------------------------------
inline void
Printer::swap(
	Printer&	ioPrinter)	//!< The input printer.
{
	mPrinter.swap(ioPrinter.mPrinter);
}

// ------------------------------------------------------------------------------------------
inline Printer&
Printer::operator = (
	const Printer&	inPrinter)	//!< The input printer.
{
	mPrinter = inPrinter.mPrinter;
	return *this;
}

// ------------------------------------------------------------------------------------------
inline Printer&
Printer::operator = (
	const OSPtr<PMPrinter>&	inPrinter)	//!< The input printer.
{
	mPrinter = inPrinter;
	return *this;
}

// ------------------------------------------------------------------------------------------
inline void
swap(Printer& p1, Printer& p2)
{
	p1.swap(p2);
}


}	// namespace B


#endif	// BPrinter_H_
