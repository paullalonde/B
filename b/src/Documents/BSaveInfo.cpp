// ==========================================================================================
//	
//	Copyright (C) 2005-2006 Paul Lalonde enrg.
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
#include "BSaveInfo.h"

// B headers
#include "BFileUtilities.h"
#include "BLaunchUtilities.h"
#include "BString.h"
#include "BUrl.h"
#include "CFUtils.h"


namespace B {

// static member variables
const CFStringRef	SaveInfo::kUrlKey				= CFSTR("ca.paullalonde.b.saveinfo.url");
const CFStringRef	SaveInfo::kContentTypeKey		= CFSTR("ca.paullalonde.b.saveinfo.contenttype");
const CFStringRef	SaveInfo::kFromUserKey			= CFSTR("ca.paullalonde.b.saveinfo.fromuser");
const CFStringRef	SaveInfo::kClosingKey			= CFSTR("ca.paullalonde.b.saveinfo.closing");
const CFStringRef	SaveInfo::kFolderRefKey			= CFSTR("ca.paullalonde.b.saveinfo.folderref");
const CFStringRef	SaveInfo::kFileNameKey			= CFSTR("ca.paullalonde.b.saveinfo.filename");
const CFStringRef	SaveInfo::kExtensionHiddenKey	= CFSTR("ca.paullalonde.b.saveinfo.extensionhidden");
const CFStringRef	SaveInfo::kWriteAttributesKey	= CFSTR("ca.paullalonde.b.saveinfo.writeattributes");
const CFStringRef	SaveInfo::kFileRefKey			= CFSTR("ca.paullalonde.b.saveinfo.fileref");
const CFStringRef	SaveInfo::kReplacingKey			= CFSTR("ca.paullalonde.b.saveinfo.replacing");

// ------------------------------------------------------------------------------------------
SaveInfo::SaveInfo()
	: mDictionary(CFDictionaryCreateMutable(NULL, 0, &kCFCopyStringDictionaryKeyCallBacks, 
				  &kCFTypeDictionaryValueCallBacks), from_copy),
	  mInvokingWindow(NULL)
{
}

// ------------------------------------------------------------------------------------------
SaveInfo::SaveInfo(const SaveInfo& src)
	: mDictionary(CFDictionaryCreateMutableCopy(NULL, 0, src.mDictionary), from_copy),
	  mInvokingWindow(src.mInvokingWindow)
{
}

// ------------------------------------------------------------------------------------------
SaveInfo&
SaveInfo::operator = (const SaveInfo& src)
{
	SaveInfo(src).swap(*this);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::swap(SaveInfo& src)
{
	mDictionary.swap(src.mDictionary);
	std::swap(mInvokingWindow, src.mInvokingWindow);
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetFileInfo(
	const Url&		inUrl,
	const String&	inContentType)
{
	SetUrl(kUrlKey, inUrl);
	
	if (inUrl.IsFileUrl())
	{
		String				fileName(inUrl.TopPath());
		Url					folderUrl(inUrl.PopPath());
		FSRef				folderRef, fileRef;
		HFSUniStr255		hfsName;
		LSItemInfoRecord	lsInfo;
		OSStatus			err;
		
		SetFileName(fileName);
		
		if (folderUrl.Copy(folderRef, std::nothrow))
			SetFolderRef(folderRef);
		
		make_hfsstring(fileName.cf_ref(), hfsName);
		
		err = FSMakeFSRefUnicode(&folderRef, hfsName.length, hfsName.unicode, 
								 GetApplicationTextEncoding(), &fileRef);
		
		if (err == noErr)
		{
			SetFileRef(fileRef);
			
			err = LSCopyItemInfoForRef(&fileRef, kLSRequestExtensionFlagsOnly, &lsInfo);
			
			if (err == noErr)
			{
				SetExtensionHidden((lsInfo.flags & kLSItemInfoExtensionIsHidden) != 0);
			}
		}
		else
		{
			SetExtensionHidden(!DoesFinderAlwaysDisplayExtensions());
		}
	}
	
	SetContentType(inContentType);
}

// ------------------------------------------------------------------------------------------
Url
SaveInfo::GetUrl() const
{
	return (GetUrl(kUrlKey));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetUrl(const Url& inUrl)
{
	SetUrl(kUrlKey, inUrl);
}

// ------------------------------------------------------------------------------------------
String
SaveInfo::GetContentType() const
{
	return (GetString(kContentTypeKey));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetContentType(const String& inContentType)
{
	SetString(kContentTypeKey, inContentType);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetFromUser() const
{
	return (GetBool(kFromUserKey, false));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetFromUser(bool inFromUser)
{
	SetBool(kFromUserKey, inFromUser);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetClosing() const
{
	return (GetBool(kClosingKey, false));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetClosing(bool inClosing)
{
	SetBool(kClosingKey, inClosing);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetFolderRef(FSRef& outFolderRef) const
{
	return (GetData(kFolderRefKey, &outFolderRef, sizeof(outFolderRef)));
}

// ------------------------------------------------------------------------------------------
FSRef
SaveInfo::GetFolderRef() const
{
	return (GetFSRef(kFolderRefKey));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetFolderRef(const FSRef& inFolderRef)
{
	SetFSRef(kFolderRefKey, inFolderRef);
}

// ------------------------------------------------------------------------------------------
String
SaveInfo::GetFileName() const
{
	return (GetString(kFileNameKey));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetFileName(const String& inFileName)
{
	SetString(kFileNameKey, inFileName);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetExtensionHidden() const
{
	return (GetBool(kExtensionHiddenKey, true));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetExtensionHidden(bool inExtensionHidden)
{
	SetBool(kExtensionHiddenKey, inExtensionHidden);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetWriteAttributes() const
{
	return (GetBool(kWriteAttributesKey));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetWriteAttributes(bool inWriteAttributes)
{
	SetBool(kWriteAttributesKey, inWriteAttributes);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetFileRef(FSRef& outFileRef) const
{
	return (GetData(kFileRefKey, &outFileRef, sizeof(outFileRef)));
}

// ------------------------------------------------------------------------------------------
FSRef
SaveInfo::GetFileRef() const
{
	return (GetFSRef(kFileRefKey));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetFileRef(const FSRef& inFileRef)
{
	SetFSRef(kFileRefKey, inFileRef);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetReplacing() const
{
	return (GetBool(kReplacingKey, false));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetReplacing(bool inReplacing)
{
	SetBool(kReplacingKey, inReplacing);
}

// ------------------------------------------------------------------------------------------
String
SaveInfo::GetString(CFStringRef key) const
{
	return String(CFUGet<CFStringRef>(mDictionary, key, std::nothrow));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetString(CFStringRef key, const String& str)
{
	CFDictionarySetValue(mDictionary, key, str.cf_ref());
}

// ------------------------------------------------------------------------------------------
Url
SaveInfo::GetUrl(CFStringRef key) const
{
	return Url(CFUGet<CFURLRef>(mDictionary, key, std::nothrow));
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetUrl(CFStringRef key, const Url& url)
{
	CFDictionarySetValue(mDictionary, key, url.cf_ref());
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetBool(CFStringRef key) const
{
	return (CFUGetNumber<bool>(mDictionary, key));
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetBool(CFStringRef key, bool dflt) const
{
	bool	value;
	
	if (!CFUGetNumber(mDictionary, key, value))
		value = dflt;
	
	return (value);
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetBool(CFStringRef key, bool value)
{
	CFDictionarySetValue(mDictionary, key, value ? kCFBooleanTrue : kCFBooleanFalse);
}

// ------------------------------------------------------------------------------------------
bool
SaveInfo::GetData(CFStringRef key, void* ptr, size_t size) const
{
	bool				good;
	OSPtr<CFDataRef>	data;
	
	good = CFUGet(mDictionary, key, data);
	
	if (good)
	{
		if (static_cast<size_t>(CFDataGetLength(data)) != size)
			B_THROW_IF_STATUS(paramErr);
		
		CFDataGetBytes(data, CFRangeMake(0, size), reinterpret_cast<UInt8*>(ptr));
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetData(CFStringRef key, const void* ptr, size_t size)
{
	OSPtr<CFDataRef>	data(CFDataCreate(NULL, reinterpret_cast<const UInt8*>(ptr), size), 
							 from_copy);
	
	CFDictionarySetValue(mDictionary, key, data.get());
}

// ------------------------------------------------------------------------------------------
FSRef
SaveInfo::GetFSRef(CFStringRef key) const
{
	FSRef	ref;
	
	if (!GetData(key, &ref, sizeof(ref)))
		B_THROW_IF_STATUS(paramErr);
	
	return (ref);
}

// ------------------------------------------------------------------------------------------
void
SaveInfo::SetFSRef(CFStringRef key, const FSRef& ref)
{
	SetData(key, &ref, sizeof(ref));
}

}	// namespace B
