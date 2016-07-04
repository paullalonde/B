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
#include "BPageFormat.h"

// B headers
#include "BErrorHandler.h"


namespace B {


static OSPtr<PMPageFormat>	CreatePageFormat()
{
	PMPageFormat	pageFormat;
	OSStatus		err;
	
	err = PMCreatePageFormat(&pageFormat);
	B_THROW_IF_STATUS(err);
	
	return OSPtr<PMPageFormat>(pageFormat, from_copy);
}


// ------------------------------------------------------------------------------------------
PageFormat::PageFormat()
	: mPageFormat(CreatePageFormat())
{
}

// ------------------------------------------------------------------------------------------
PageFormat::~PageFormat()
{	
}

// ------------------------------------------------------------------------------------------
PageFormat
PageFormat::copy() const
{
	PageFormat	newFormat;
	OSStatus	err;
	
	err = PMCopyPageFormat(mPageFormat, newFormat.mPageFormat);
	B_THROW_IF_STATUS(err);
	
	return newFormat;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDataRef>
PageFormat::Serialize() const
{
	CFDataRef	dataRef;
	OSStatus	err;
	
	err = PMFlattenPageFormatToCFData(mPageFormat, &dataRef);
	B_THROW_IF_STATUS(err);
	
	return OSPtr<CFDataRef>(dataRef, from_copy);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::Deserialize(const OSPtr<CFDataRef>& inBlob)
{
	PMPageFormat	pageFormat;
	OSStatus		err;
	
	err = PMUnflattenPageFormatWithCFData(inBlob.get(), &pageFormat);
	B_THROW_IF_STATUS(err);
	
	OSPtr<PMPageFormat>	newFormat(pageFormat, from_copy);
	
	err = PMCopyPageFormat(newFormat, mPageFormat);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
PageFormat::Equals(const PageFormat& inPageFormat) const
{
	OSPtr<CFDataRef>	oldBlob	= inPageFormat.Serialize();
	size_t				oldSize	= CFDataGetLength(oldBlob);
	OSPtr<CFDataRef>	newBlob	= Serialize();
	size_t				newSize	= CFDataGetLength(newBlob);
	bool				equal	= (oldSize == newSize);
	
	if (equal)
	{
		equal = (memcmp(CFDataGetBytePtr(oldBlob), CFDataGetBytePtr(newBlob), newSize) != 0);
	}
	
	return equal;
}

// ------------------------------------------------------------------------------------------
PMOrientation
PageFormat::GetOrientation() const
{
	PMOrientation	orientation;
	OSStatus		err;
	
	err = PMGetOrientation(mPageFormat, &orientation);
	B_THROW_IF_STATUS(err);
	
	return (orientation);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::SetOrientation(PMOrientation inOrientation)
{
	OSStatus	err;
	
	err = PMSetOrientation(mPageFormat, inOrientation, false);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::GetResolution(PMResolution& outResolution) const
{
	OSStatus	err;
	
	err = PMGetResolution(mPageFormat, &outResolution);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::SetResolution(const PMResolution& inResolution)
{
	OSStatus	err;
	
	err = PMSetResolution(mPageFormat, &inResolution);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
double
PageFormat::GetScale() const
{
	double		scale;
	OSStatus	err;
	
	err = PMGetScale(mPageFormat, &scale);
	B_THROW_IF_STATUS(err);
	
	return (scale);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::SetScale(double inScale)
{
	OSStatus	err;
	
	err = PMSetScale(mPageFormat, inScale);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
PMRect
PageFormat::GetAdjustedPageRect() const
{
	PMRect		r;
	OSStatus	err;
	
	err = PMGetAdjustedPageRect(mPageFormat, &r);
	B_THROW_IF_STATUS(err);
	
	return r;
}

// ------------------------------------------------------------------------------------------
PMRect
PageFormat::GetAdjustedPaperRect() const
{
	PMRect		r;
	OSStatus	err;
	
	err = PMGetAdjustedPaperRect(mPageFormat, &r);
	B_THROW_IF_STATUS(err);
	
	return r;
}

// ------------------------------------------------------------------------------------------
PMRect
PageFormat::GetUnadjustedPageRect() const
{
	PMRect		r;
	OSStatus	err;
	
	err = PMGetUnadjustedPageRect(mPageFormat, &r);
	B_THROW_IF_STATUS(err);
	
	return r;
}

// ------------------------------------------------------------------------------------------
PMRect
PageFormat::GetUnadjustedPaperRect() const
{
	PMRect		r;
	OSStatus	err;
	
	err = PMGetUnadjustedPaperRect(mPageFormat, &r);
	B_THROW_IF_STATUS(err);
	
	return r;
}

// ------------------------------------------------------------------------------------------
size_t
PageFormat::GetExtendedDataSize(
	OSType	dataID)		//!< Four-character constant identifying the data of interest.
	const
{
	UInt32		dataSize;
	OSStatus	err;
	
	err = PMGetPageFormatExtendedData(mPageFormat, dataID, &dataSize, kPMDontWantData);
	B_THROW_IF_STATUS(err);
	
	return (dataSize);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::GetExtendedData(
	OSType	dataID,		//!< Four-character constant identifying the data of interest.
	size_t	size,		//!< The size of the data.
	void*	outData)	//!< A pointer to the buffer that will hold the data.
	const
{
	UInt32		dataSize	= size;
	OSStatus	err;
	
	err = PMGetPageFormatExtendedData(mPageFormat, dataID, &dataSize, outData);
	if ((err == noErr) && (dataSize != size))
		err = paramErr;
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PageFormat::SetExtendedData(
	OSType		dataID,	//!< Four-character constant identifying the data of interest.
	size_t		size,	//!< The size of the data.
	const void*	inData)	//!< The new value.
{
	OSStatus	err;
	
	err = PMSetPageFormatExtendedData(mPageFormat, dataID, size, const_cast<void*>(inData));
	B_THROW_IF_STATUS(err);
}

}	// namespace B
