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

#ifndef BPrintSettings_H_
#define BPrintSettings_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

// forward declarations
class	String;


/*!
	@brief	Encapsulates a print settings for printing.
	
	@todo	%Document this class!
*/
class PrintSettings
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  Creates an empty print settings.
				PrintSettings();
	//! Copy constructor.
				PrintSettings(const PrintSettings& inPrintSettings);
	//! @c OSPtr<PMPrintSettings> constructor.
	explicit	PrintSettings(const OSPtr<PMPrintSettings>& inPrintSettings);
	//! @c PMPrintSettings constructor.
	explicit	PrintSettings(PMPrintSettings inPrintSettings);
	//! @c PMPrintSettings constructor.
	explicit	PrintSettings(PMPrintSettings inPrintSettings, const from_copy_t&);
	//! Destructor.
				~PrintSettings();
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	PrintSettings&	operator = (const PrintSettings& inPrintSettings);
	//! Copy assignment.
	PrintSettings&	operator = (const OSPtr<PMPrintSettings>& inPrintSettings);
	//@}
	
	//! @name Copying
	//@{
	//! Creates a distinct (i.e., non-shared) copy of the print settings.
	PrintSettings	copy() const;
	//! Exchanges the contents of @c *this and @a ioPrintSettings.
	void			swap(PrintSettings& ioPrintSettings);
	//@}
	
	//! @name Serialisation
	//@{
	//! Serializes the print settings to the given vector.
	OSPtr<CFDataRef>	Serialize() const;
	//! Deserializes the print settings from the given vector.
	void				Deserialize(const OSPtr<CFDataRef>& inBlob);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the number of the first page to be printed.
	size_t		GetFirstPage() const;
	//! Returns the number of the last page to be printed.
	size_t		GetLastPage() const;
	//! Returns the valid range of pages that can be printed.
	void		GetPageRange(size_t& outMinPage, size_t& outMaxPage) const;
	//! Returns the number of copies that the user requests to be printed.
	size_t		GetCopies() const;
	//! Returns the name of the print job.
	String		GetJobName() const;
	//! Returns whether the pages will be collated.
	bool		GetCollate() const;
	//! Returns extended print settings data previously stored by your application.
	void		GetExtendedData(OSType dataID, size_t size, void* outData) const;
	//! Returns the size of extended print settings data previously stored by your application.
	size_t		GetExtendedDataSize(OSType dataID) const;
	//! Returns extended print settings data previously stored by your application in a type-safe manner.
	template <typename T>
	void		GetExtendedData(OSType dataID, T& outData) const;
	//! Compares the print settings to the given print settings, and returns true if they are equal.
	bool		Equals(const PrintSettings& inPrintSettings) const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Sets the default page number of the first page to be printed.
	void	SetFirstPage(size_t inFirstPage);
	//! Sets the page number of the last page to be printed.
	void	SetLastPage(size_t inLastPage);
	//! Sets the valid range of pages that can be printed.
	void	SetPageRange(size_t inFirstPage, size_t inLastPage);
	//! Sets the valid range of pages to "all".
	void	SetPageRangeAll();
	//! Sets the default value to be displayed in the Print dialog for the number of copies to be printed.
	void	SetCopies(size_t inCopies);
	//! Sets the name of the print job.
	void	SetJobName(const String& inName);
	//! Sets whether the pages will be collated.
	void	SetCollate(bool inCollate);
	//! Sets the extended print settings data previously stored by your application.
	void	SetExtendedData(OSType dataID, size_t size, const void* inData);
	//! Sets extended print settings data previously stored by your application in a type-safe manner.
	template <typename T>
	void	SetExtendedData(OSType dataID, const T& inData);
	//@}
	
private:
	
	// member variables
	OSPtr<PMPrintSettings>	mPrintSettings;
	
	// friends
	friend class	PrintSession;
};

// ------------------------------------------------------------------------------------------
inline
PrintSettings::PrintSettings(
	const PrintSettings&	inPrintSettings)	//!< The input print settings.
		: mPrintSettings(inPrintSettings.mPrintSettings)
{
}

// ------------------------------------------------------------------------------------------
inline
PrintSettings::PrintSettings(
	const OSPtr<PMPrintSettings>&	inPrintSettings)	//!< The input print settings.
		: mPrintSettings(inPrintSettings)
{
}

// ------------------------------------------------------------------------------------------
inline
PrintSettings::PrintSettings(
	PMPrintSettings	inPrintSettings)	//!< The input print settings.
		: mPrintSettings(inPrintSettings)
{
}

// ------------------------------------------------------------------------------------------
inline
PrintSettings::PrintSettings(
	PMPrintSettings		inPrintSettings,	//!< The input print settings.
	const from_copy_t&	fc)
		: mPrintSettings(inPrintSettings, fc)
{
}

// ------------------------------------------------------------------------------------------
inline void
PrintSettings::swap(
	PrintSettings&	ioPrintSettings)	//!< The input print settings.
{
	mPrintSettings.swap(ioPrintSettings.mPrintSettings);
}

// ------------------------------------------------------------------------------------------
inline PrintSettings&
PrintSettings::operator = (
	const PrintSettings&	inPrintSettings)	//!< The input print settings.
{
	mPrintSettings = inPrintSettings.mPrintSettings;
	return *this;
}

// ------------------------------------------------------------------------------------------
inline PrintSettings&
PrintSettings::operator = (
	const OSPtr<PMPrintSettings>&	inPrintSettings)	//!< The input print settings.
{
	mPrintSettings = inPrintSettings;
	return *this;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
PrintSettings::GetExtendedData(
	OSType	dataID,		//!< Four-character constant identifying the data of interest.
	T&		outData)	//!< Holds the output value.
	const
{
	GetExtendedData(dataID, sizeof(outData), &outData);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
PrintSettings::SetExtendedData(
	OSType		dataID,		//!< Four-character constant identifying the data of interest.
	const T&	inData)		//!< The new value.
{
	SetExtendedData(dataID, sizeof(inData), &inData);
}

// ------------------------------------------------------------------------------------------
inline void
swap(PrintSettings& ioPrintSettings1, PrintSettings& ioPrintSettings2)
{
	ioPrintSettings1.swap(ioPrintSettings2);
}


}	// namespace B


#endif	// BPrintSettings_H_
