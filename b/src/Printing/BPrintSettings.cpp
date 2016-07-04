// ==========================================================================================
//  
//  Copyright (C) 2003-2006 Paul Lalonde enrg.
//  
//  This program is free software;  you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation;  
//  either version 2 of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this 
//  program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//  Suite 330, Boston, MA  02111-1307  USA
//  
// ==========================================================================================

// file header
#include "BPrintSettings.h"

// B headers
#include "BErrorHandler.h"
#include "BMemoryUtilities.h"
#include "BString.h"


namespace B {


static OSPtr<PMPrintSettings>   CreatePrintSettings()
{
    PMPrintSettings printSettings;
    OSStatus        err;
    
    err = PMCreatePrintSettings(&printSettings);
    B_THROW_IF_STATUS(err);
    
    return OSPtr<PMPrintSettings>(printSettings, from_copy);
}


// ------------------------------------------------------------------------------------------
PrintSettings::PrintSettings()
    : mPrintSettings(CreatePrintSettings())
{
}

// ------------------------------------------------------------------------------------------
PrintSettings::~PrintSettings()
{   
}

// ------------------------------------------------------------------------------------------
PrintSettings
PrintSettings::copy() const
{
    PrintSettings   newSettings;
    OSStatus        err;
    
    err = PMCopyPrintSettings(mPrintSettings, newSettings.mPrintSettings);
    B_THROW_IF_STATUS(err);
    
    return newSettings;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDataRef>
PrintSettings::Serialize() const
{
    CFDataRef   dataRef;
    OSStatus    err;
    
    err = PMFlattenPrintSettingsToCFData(mPrintSettings, &dataRef);
    B_THROW_IF_STATUS(err);
    
    return OSPtr<CFDataRef>(dataRef, from_copy);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::Deserialize(const OSPtr<CFDataRef>& inBlob)
{
    PMPrintSettings printSettngs;
    OSStatus        err;
    
    err = PMUnflattenPrintSettingsWithCFData(inBlob.get(), &printSettngs);
    B_THROW_IF_STATUS(err);
    
    OSPtr<PMPrintSettings>  newSettings(printSettngs, from_copy);
    
    err = PMCopyPrintSettings(newSettings, mPrintSettings);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
PrintSettings::Equals(const PrintSettings& inPrintSettings) const
{
    OSPtr<CFDataRef>    oldBlob = inPrintSettings.Serialize();
    size_t              oldSize = CFDataGetLength(oldBlob);
    OSPtr<CFDataRef>    newBlob = Serialize();
    size_t              newSize = CFDataGetLength(newBlob);
    bool                equal   = (oldSize == newSize);
    
    if (equal)
    {
        equal = (memcmp(CFDataGetBytePtr(oldBlob), CFDataGetBytePtr(newBlob), newSize) != 0);
    }
    
    return equal;
}

// ------------------------------------------------------------------------------------------
size_t
PrintSettings::GetFirstPage() const
{
    UInt32      firstPage;
    OSStatus    err;
    
    err = PMGetFirstPage(mPrintSettings, &firstPage);
    B_THROW_IF_STATUS(err);
    
    return (firstPage);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetFirstPage(size_t inFirstPage)
{
    OSStatus    err;
    
    err = PMSetFirstPage(mPrintSettings, inFirstPage, false);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
size_t
PrintSettings::GetLastPage() const
{
    UInt32      lastPage;
    OSStatus    err;
    
    err = PMGetFirstPage(mPrintSettings, &lastPage);
    B_THROW_IF_STATUS(err);
    
    return (lastPage);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetLastPage(size_t inLastPage)
{
    OSStatus    err;
    
    err = PMSetFirstPage(mPrintSettings, inLastPage, false);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::GetPageRange(size_t& outMinPage, size_t& outMaxPage) const
{
    UInt32      minPage, maxPage;
    OSStatus    err;
    
    err = PMGetPageRange(mPrintSettings, &minPage, &maxPage);
    B_THROW_IF_STATUS(err);
    
    outMinPage  = minPage;
    outMaxPage  = maxPage;
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetPageRange(size_t inFirstPage, size_t inLastPage)
{
    OSStatus    err;
    
    err = PMSetPageRange(mPrintSettings, inFirstPage, inLastPage);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetPageRangeAll()
{
    SetPageRange(1, static_cast<UInt32>(kPMPrintAllPages));
}

// ------------------------------------------------------------------------------------------
size_t
PrintSettings::GetCopies() const
{
    UInt32      copies;
    OSStatus    err;
    
    err = PMGetCopies(mPrintSettings, &copies);
    B_THROW_IF_STATUS(err);
    
    return (copies);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetCopies(size_t inCopies)
{
    OSStatus    err;
    
    err = PMSetCopies(mPrintSettings, inCopies, false);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
String
PrintSettings::GetJobName() const
{
    CFStringRef cfstr;
    OSStatus    err;
    
    err = PMGetJobNameCFString(mPrintSettings, &cfstr);
    B_THROW_IF_STATUS(err);
    
    return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetJobName(const String& inName)
{
    OSStatus    err;
    
    err = PMSetJobNameCFString(mPrintSettings, inName.cf_ref());
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
PrintSettings::GetCollate() const
{
    Boolean     collate;
    OSStatus    err;
    
    err = PMGetCollate(mPrintSettings, &collate);
    B_THROW_IF_STATUS(err);
    
    return (collate);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetCollate(bool inCollate)
{
    OSStatus    err;
    
    err = PMSetCollate(mPrintSettings, inCollate);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
size_t
PrintSettings::GetExtendedDataSize(
    OSType  dataID)     //!< Four-character constant identifying the data of interest.
    const
{
    UInt32      dataSize;
    OSStatus    err;
    
    err = PMGetPrintSettingsExtendedData(mPrintSettings, dataID, &dataSize, kPMDontWantData);
    B_THROW_IF_STATUS(err);
    
    return (dataSize);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::GetExtendedData(
    OSType  dataID,     //!< Four-character constant identifying the data of interest.
    size_t  size,       //!< The size of the data.
    void*   outData)    //!< A pointer to the buffer that will hold the data.
    const
{
    UInt32      dataSize    = size;
    OSStatus    err;
    
    err = PMGetPrintSettingsExtendedData(mPrintSettings, dataID, &dataSize, outData);
    if ((err == noErr) && (dataSize != size))
        err = paramErr;
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
PrintSettings::SetExtendedData(
    OSType      dataID, //!< Four-character constant identifying the data of interest.
    size_t      size,   //!< The size of the data.
    const void* inData) //!< The new value.
{
    OSStatus    err;
    
    err = PMSetPrintSettingsExtendedData(mPrintSettings, dataID, size, const_cast<void*>(inData));
    B_THROW_IF_STATUS(err);
}

}   // namespace B
