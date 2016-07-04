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
#include "BPrinter.h"

// B headers
#include "BErrorHandler.h"
#include "BString.h"
#include "BUrl.h"
#include "BOSPtr.h"


namespace B {

// ------------------------------------------------------------------------------------------
Printer::~Printer()
{   
}

// ------------------------------------------------------------------------------------------
String
Printer::GetName() const
{
    return (String(PMPrinterGetName(mPrinter)));
}

// ------------------------------------------------------------------------------------------
String
Printer::GetIdentifier() const
{
    return (String(PMPrinterGetID(mPrinter)));
}

// ------------------------------------------------------------------------------------------
bool
Printer::IsDefault() const
{
    return (PMPrinterIsDefault(mPrinter));
}

// ------------------------------------------------------------------------------------------
bool
Printer::IsFavorite() const
{
    return (PMPrinterIsFavorite(mPrinter));
}

// ------------------------------------------------------------------------------------------
String
Printer::GetLocation() const
{
    return (String(PMPrinterGetLocation(mPrinter)));
}

// ------------------------------------------------------------------------------------------
String
Printer::GetMakeAndModel() const
{
    CFStringRef cfstr;
    OSStatus    err;
    
    err = PMPrinterGetMakeAndModelName(mPrinter, &cfstr);
    B_THROW_IF_STATUS(err);
    
    return (String(cfstr, from_copy));
}

// ------------------------------------------------------------------------------------------
Url
Printer::GetDescriptionUrl() const
{
    CFURLRef    fileUrl;
    OSStatus    err;
    
    err = PMPrinterGetDescriptionURL(mPrinter, kPMPPDDescriptionType, &fileUrl);
    B_THROW_IF_STATUS(err);
    
    return (Url(OSPtr<CFURLRef>(fileUrl, from_copy)));
}

// ------------------------------------------------------------------------------------------
Url
Printer::GetDeviceUrl() const
{
    CFURLRef    fileUrl;
    OSStatus    err;
    
    err = PMPrinterGetDeviceURI(mPrinter, &fileUrl);
    B_THROW_IF_STATUS(err);
    
    return (Url(OSPtr<CFURLRef>(fileUrl, from_copy)));
}

// ------------------------------------------------------------------------------------------
void
Printer::GetLanguageInfo(PMLanguageInfo& outInfo) const
{
    OSStatus    err;
    
    err = PMPrinterGetLanguageInfo(mPrinter, &outInfo);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
bool
Printer::IsPostScript() const
{
    return (PMPrinterIsPostScriptCapable(mPrinter));
}

// ------------------------------------------------------------------------------------------
unsigned
Printer::CountResolutions() const
{
    UInt32      count;
    OSStatus    err;
    
    err = PMPrinterGetPrinterResolutionCount(mPrinter, &count);
    B_THROW_IF_STATUS(err);
    
    return (count);
}

// ------------------------------------------------------------------------------------------
/*! @note   @a index is zero-based, @b not 1-based.  The acceptable range for @a index 
            is therefore [0, CountResolutions() ).
*/
PMResolution
Printer::GetResolution(unsigned index) const
{
    PMResolution    resolution;
    OSStatus        err;
    
    err = PMPrinterGetIndexedPrinterResolution(mPrinter, index, &resolution);
    B_THROW_IF_STATUS(err);
    
    return (resolution);
}

// ------------------------------------------------------------------------------------------
PMResolution
Printer::GetTaggedResolution(PMTag inTag) const
{
    PMResolution    resolution;
    OSStatus        err;
    
    err = PMPrinterGetPrinterResolution(mPrinter, inTag, &resolution);
    B_THROW_IF_STATUS(err);
    
    return (resolution);
}

// ------------------------------------------------------------------------------------------
PMPrinterState
Printer::GetState() const
{
    PMPrinterState  state;
    OSStatus        err;
    
    err = PMPrinterGetState(mPrinter, &state);
    B_THROW_IF_STATUS(err);
    
    return (state);
}

}   // namespace B
