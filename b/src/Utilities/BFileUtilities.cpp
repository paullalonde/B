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
#include "BFileUtilities.h"

// B headers
#include "BErrorHandler.h"
#include "BMemoryUtilities.h"
#include "BOSPtr.h"
#include "BString.h"
#include "BStringUtilities.h"


namespace B {


void
make_hfsstring(
	CFStringRef			cfstr, 
	HFSUniStr255&		hfsstr)
{
	B_ASSERT(cfstr != NULL);
	
	CFIndex	size	= CFStringGetLength(cfstr);
	
	if (size > 0)
	{
		CFIndex	done, used;
		
		done = CFStringGetBytes(cfstr, 
					CFRangeMake(0, size), kCFStringEncodingUnicode, 0, false, 
					reinterpret_cast<UInt8*>(hfsstr.unicode), 
					sizeof(hfsstr.unicode) * sizeof(UniChar), &used);
		
		if (done < size)
			B_THROW_STATUS(paramErr);
		
		hfsstr.length = done;
	}
	else
	{
		hfsstr.length = 0;
	}
}

void
make_hfsstring(
	const std::wstring&	wstr, 
	HFSUniStr255&		hfsstr)
{
	make_hfsstring(make_cfstring(wstr), hfsstr);
}

void
make_hfsstring(
	const std::string&	str, 
	CFStringEncoding	encoding,
	HFSUniStr255&		hfsstr)
{
	make_hfsstring(make_cfstring(str, encoding), hfsstr);
}


// ------------------------------------------------------------------------------------------
void	RefToBlob(const FSRef& ref, std::vector<UInt8>& blob, 
			const FSRef* baseRef /* = NULL */)
{
	AliasHandle	aliasH	= NULL;
	OSStatus	err;
	
	err = FSNewAlias(baseRef, &ref, &aliasH);
	B_THROW_IF_STATUS(err);
	
	AutoMacHandle	autoH(reinterpret_cast<Handle>(aliasH));
	
	AliasToBlob(aliasH, blob);
}

// ------------------------------------------------------------------------------------------
void	AliasToBlob(AliasHandle aliasH, std::vector<UInt8>& blob)
{
	Handle			h		= reinterpret_cast<Handle>(aliasH);
	AutoHandleLock	autoLock(h);
	::Size			size	= GetHandleSize(h);
	
	blob.resize(size);
	BlockMoveData(*h, &blob[0], size);
}

// ------------------------------------------------------------------------------------------
bool	BlobToRef(const std::vector<UInt8>& blob, FSRef& ref, bool showUI /* = true */, 
			const FSRef* baseRef /* = NULL */, bool* wasChanged /* = NULL */)
{
	AliasHandle		aliasH	= BlobToAlias(blob);
	AutoMacHandle	autoH(reinterpret_cast<Handle>(aliasH));
	bool			good	= false;
	UInt32			flags	= 0;
	Boolean			changed;
	OSStatus		err;
	
	if (!showUI)
		flags |= kResolveAliasFileNoUI;
	
	err = FSResolveAliasWithMountFlags(baseRef, aliasH, &ref, &changed, flags);
	
	if (err == noErr)
	{
		if (wasChanged != NULL)
			*wasChanged = changed;
		
		good = true;
	}
	
	return (good);
}

// ------------------------------------------------------------------------------------------
AliasHandle	BlobToAlias(const std::vector<UInt8>& blob)
{
	AutoMacHandle	autoH(blob.size());
	
	{
		AutoHandleLock	autoLock(autoH);
		
		BlockMoveData(&blob[0], *autoH.get_handle(), blob.size());
	}
	
	return reinterpret_cast<AliasHandle>(autoH.release());
}

// ------------------------------------------------------------------------------------------
bool	GetFileNameExtension(
	const B::String&	inName, 
	B::String&			outExtension)
{
	std::vector<UniChar>	buff(inName.size());
	size_t					buffsize;
	UniCharCount			extIndex;
	bool					hasExtension;
	OSStatus				err;
	
	buffsize = inName.copy(&buff[0], buff.size());
	
	err = LSGetExtensionInfo(buffsize, &buff[0], &extIndex);
	B_THROW_IF_STATUS(err);
	
	hasExtension = (extIndex != kLSInvalidExtensionIndex);
	
	if (hasExtension)
	{
		outExtension.assign(&buff[extIndex], buffsize-extIndex);
	}
	
	return (hasExtension);
}

// ------------------------------------------------------------------------------------------
bool	DoesFinderAlwaysDisplayExtensions()
{
	Boolean	value, exists;
	
	value = CFPreferencesGetAppBooleanValue(CFSTR("AppleShowAllExtensions"),
											CFSTR(".GlobalPreferences"),
											&exists);
	if (!exists)
		value = false;
	
	return (value);
}

// ------------------------------------------------------------------------------------------
String	GetDisplayName(
	const String&	inName, 
	bool			inHideExtension)
{
	String	displayName(inName);
	
	if (inHideExtension)
	{
		String	extension;
		
		if (GetFileNameExtension(inName, extension))
		{
			displayName = inName.substr(0, inName.size() - extension.size() - 1);
		}
	}
	
	return displayName;
}

}	// namespace B
