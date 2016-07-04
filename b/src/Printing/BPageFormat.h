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

#ifndef BPageFormat_H_
#define BPageFormat_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

/*!
	@brief	Encapsulates a page format for printing.
	
	@todo	%Document this class!
*/
class PageFormat
{
public:
	
	//! @name Constructors / Destructor
	//@{
	//! Default constructor.  Creates an empty page format.
				PageFormat();
	//! Copy constructor.
				PageFormat(const PageFormat& inPageFormat);
	//! @c OSPtr<PMPageFormat> constructor.
	explicit	PageFormat(const OSPtr<PMPageFormat>& inPageFormat);
	//! @c PMPageFormat constructor.
	explicit	PageFormat(PMPageFormat inPageFormat);
	//! @c PMPageFormat constructor.
	explicit	PageFormat(PMPageFormat inPageFormat, const from_copy_t&);
	//! Destructor.
				~PageFormat();
	//@}
	
	//! @name Assignment
	//@{
	//! Copy assignment.
	PageFormat&	operator = (const PageFormat& inPageFormat);
	//! Copy assignment.
	PageFormat&	operator = (const OSPtr<PMPageFormat>& inPageFormat);
	//@}
	
	//! @name Copying
	//@{
	//! Creates a distinct (i.e., non-shared) copy of the page format.
	PageFormat	copy() const;
	//! Exchanges the contents of @c *this and @a ioPageFormat.
	void		swap(PageFormat& ioPageFormat);
	//@}
	
	//! @name Serialisation
	//@{
	//! Serializes the page format to the given vector.
	OSPtr<CFDataRef>	Serialize() const;
	//! Deserializes the page format from the given vector.
	void				Deserialize(const OSPtr<CFDataRef>& inBlob);
	//@}
	
	//! @name Inquiries
	//@{
	//! Returns the current setting for page orientation.
	PMOrientation
			GetOrientation() const;
	//! Returns the current application drawing resolution.
	void	GetResolution(PMResolution& outResolution) const;
	//! Returns the scaling factor currently applied to the page and paper rectangles.
	double	GetScale() const;
	//! Returns the page size, taking into account orientation, application drawing resolution, and scaling settings.
	PMRect	GetAdjustedPageRect() const;
	//! Returns the paper size, taking into account orientation, application drawing resolution, and scaling settings.
	PMRect	GetAdjustedPaperRect() const;
	//! Returns the size of the imageable area in points, unaffected by orientation, resolution, or scaling.
	PMRect	GetUnadjustedPageRect() const;
	//! Returns a rectangle that specifies the size of the paper in points, unaffected by rotation, resolution, or scaling.
	PMRect	GetUnadjustedPaperRect() const;
	//! Returns extended page format data previously stored by your application.
	void	GetExtendedData(OSType dataID, size_t size, void* outData) const;
	//! Returns the size of extended page format data previously stored by your application.
	size_t	GetExtendedDataSize(OSType dataID) const;
	//! Returns extended page format data previously stored by your application in a type-safe manner.
	template <typename T>
	void	GetExtendedData(OSType dataID, T& outData) const;
	//! Compares the page format to the given page format, and returns true if they are equal.
	bool	Equals(const PageFormat& inPageFormat) const;
	//@}
	
	//! @name Modifiers
	//@{
	//! Sets the page orientation for printing.
	void	SetOrientation(PMOrientation inOrientation);
	//! Sets the current application drawing resolution.
	void	SetResolution(const PMResolution& inResolution);
	//! Sets the scaling factor for the page and paper rectangles.
	void	SetScale(double inScale);
	//! Sets the extended page format data previously stored by your application.
	void	SetExtendedData(OSType dataID, size_t size, const void* inData);
	//! Sets extended page format data previously stored by your application in a type-safe manner.
	template <typename T>
	void	SetExtendedData(OSType dataID, const T& inData);
	//@}
	
private:
	
	// member variables
	OSPtr<PMPageFormat>	mPageFormat;
	
	// friends
	friend class	PrintSession;
};

// ------------------------------------------------------------------------------------------
inline
PageFormat::PageFormat(
	const PageFormat&	inPageFormat)	//!< The input format.
		: mPageFormat(inPageFormat.mPageFormat)
{
}

// ------------------------------------------------------------------------------------------
inline
PageFormat::PageFormat(
	const OSPtr<PMPageFormat>&	inPageFormat)	//!< The input format.
		: mPageFormat(inPageFormat)
{
}

// ------------------------------------------------------------------------------------------
inline
PageFormat::PageFormat(
	PMPageFormat	inPageFormat)	//!< The input format.
		: mPageFormat(inPageFormat)
{
}

// ------------------------------------------------------------------------------------------
inline
PageFormat::PageFormat(
	PMPageFormat		inPageFormat,	//!< The input format.
	const from_copy_t&	fc)
		: mPageFormat(inPageFormat, fc)
{
}

// ------------------------------------------------------------------------------------------
inline void
PageFormat::swap(
	PageFormat&	ioPageFormat)	//!< The input format.
{
	mPageFormat.swap(ioPageFormat.mPageFormat);
}

// ------------------------------------------------------------------------------------------
inline PageFormat&
PageFormat::operator = (
	const PageFormat&	inPageFormat)	//!< The input format.
{
	mPageFormat = inPageFormat.mPageFormat;
	return *this;
}

// ------------------------------------------------------------------------------------------
inline PageFormat&
PageFormat::operator = (
	const OSPtr<PMPageFormat>&	inPageFormat)	//!< The input format.
{
	mPageFormat = inPageFormat;
	return *this;
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
PageFormat::GetExtendedData(
	OSType	dataID,		//!< Four-character constant identifying the data of interest.
	T&		outData)	//!< Holds the output value.
	const
{
	GetExtendedData(dataID, sizeof(outData), &outData);
}

// ------------------------------------------------------------------------------------------
template <typename T> inline void
PageFormat::SetExtendedData(
	OSType		dataID,		//!< Four-character constant identifying the data of interest.
	const T&	inData)		//!< The new value.
{
	SetExtendedData(dataID, sizeof(inData), &inData);
}

// ------------------------------------------------------------------------------------------
inline void
swap(PageFormat& ioPageFormat1, PageFormat& ioPageFormat2)
{
	ioPageFormat1.swap(ioPageFormat2);
}


}	// namespace B


#endif	// BPageFormat_H_
