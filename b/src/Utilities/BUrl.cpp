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
#include "BUrl.h"

// system headers
#include <sys/syslimits.h>

// B headers
#include "BException.h"
#include "BMutableString.h"
#include "BString.h"
#include "BStringUtilities.h"


namespace B {

#define NULL_URL    CFSTR("x-b-null:")

// ------------------------------------------------------------------------------------------
Url::Url()
    : mRef(NULL)
{
    mRef = CFURLCreateWithString(NULL, NULL_URL, NULL);
    
    B_THROW_IF(mRef == NULL, MalformedUrlException());
    
//  Url url(String("ftp://user:password@localhost:13/path/to/resource%20names;parameter%20names?query%20name#fragment%20name"));
//  
//  String  scheme, schemespecific, user, password, host, path, parameters, query, fragment;
//  bool    abs_path;
//  int     port;
//  
//  bool    standard(url.IsStandard());
//  bool    has_scheme(url.GetScheme(scheme));
//  bool    has_schemeSpecific(url.GetSchemeSpecific(schemespecific));
//  bool    has_user(url.GetUser(user));
//  bool    has_password(url.GetPassword(password));
//  bool    has_host(url.GetHost(host));
//  bool    has_port(url.GetPort(port));
//  bool    has_path(url.GetPath(path, abs_path));
//  bool    has_parameters(url.GetParameters(parameters));
//  bool    has_query(url.GetQuery(query));
//  bool    has_fragment(url.GetFragment(fragment));
}

// ------------------------------------------------------------------------------------------
Url::Url(const Url& url)
    : mRef(url.mRef)
{
    CFRetain(mRef);
}

// ------------------------------------------------------------------------------------------
Url::Url(OSPtr<CFURLRef> cfurl)
    : mRef(cfurl)
{
    CFRetain(mRef);
}

// ------------------------------------------------------------------------------------------
Url::Url(
    const String&               inString)
        : mRef(NULL)
{
    Assign(inString);
}

// ------------------------------------------------------------------------------------------
Url::Url(
    const String&               inString, 
    const Url&                  inBase)
        : mRef(NULL)
{
    Assign(inString, inBase);
}

// ------------------------------------------------------------------------------------------
Url::Url(
    const std::string&          inData, 
    const Url*                  inBase /* = NULL */)
        : mRef(NULL)
{
    Assign(inData, inBase);
}

// ------------------------------------------------------------------------------------------
Url::Url(
    const std::vector<UInt8>&   inData, 
    CFStringEncoding            inEncoding /* = kCFStringEncodingUTF8 */, 
    const Url*                  inBase /* = NULL */)
        : mRef(NULL)
{
    Assign(inData, inEncoding, inBase);
}

// ------------------------------------------------------------------------------------------
Url::Url(const FSRef& fileRef)
    : mRef(NULL)
{
    Assign(fileRef);
}

// ------------------------------------------------------------------------------------------
Url::Url(
    const B::String&    inPath, 
    bool                inIsDirectory,
    CFURLPathStyle      inPathStyle /* = kCFURLPOSIXPathStyle */)
        : mRef(NULL)
{
    AssignPath(inPath, inIsDirectory, inPathStyle);
}

// ------------------------------------------------------------------------------------------
Url::~Url()
{
    CFRelease(mRef);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(const Url& url)
{
    if (mRef != url.mRef)
    {
        CFRelease(mRef);
        mRef = url.mRef;
        CFRetain(mRef);
    }
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(OSPtr<CFURLRef> cfurl)
{
    if (mRef != cfurl)
    {
        CFRelease(mRef);
        mRef = cfurl;
        CFRetain(mRef);
    }
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(
    const String&               inString)
{
    CFURLRef    cfurl;
    
    cfurl = CFURLCreateWithString(NULL, inString.cf_ref(), NULL);
    
    B_THROW_IF(cfurl == NULL, MalformedUrlException());
    
    if (mRef != NULL)
        CFRelease(mRef);
    
    mRef = cfurl;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(
    const String&               inString, 
    const Url&                  inBase)
{
    CFURLRef    cfurl;
    
    cfurl = CFURLCreateWithString(NULL, inString.cf_ref(), inBase.cf_ref());
    
    B_THROW_IF(cfurl == NULL, MalformedUrlException());
    
    if (mRef != NULL)
        CFRelease(mRef);
    
    mRef = cfurl;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(
    const std::string&          inData, 
    const Url*                  inBase /* = NULL */)
{
    CFURLRef    cfurl;
    
    cfurl = CFURLCreateWithBytes(NULL, 
                                 reinterpret_cast<const UInt8*>(inData.c_str()), inData.size(), 
                                 kCFStringEncodingUTF8, 
                                 (inBase != NULL) ? inBase->cf_ref() : NULL);
    
    B_THROW_IF(cfurl == NULL, MalformedUrlException());
    
    if (mRef != NULL)
        CFRelease(mRef);
    
    mRef = cfurl;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(
    const std::vector<UInt8>&   inData, 
    CFStringEncoding            inEncoding /* = kCFStringEncodingUTF8 */, 
    const Url*                  inBase /* = NULL */)
{
    CFURLRef    cfurl;
    
    cfurl = CFURLCreateWithBytes(NULL, &inData[0], inData.size(), inEncoding, 
                                 (inBase != NULL) ? inBase->cf_ref() : NULL);
    
    B_THROW_IF(cfurl == NULL, MalformedUrlException());
    
    if (mRef != NULL)
        CFRelease(mRef);
    
    mRef = cfurl;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::Assign(const FSRef& fileRef)
{
    CFURLRef        cfurl;
    FSCatalogInfo   catInfo;
    bool            isDirectory;
    OSStatus        err;
    
    err = FSGetCatalogInfo(&fileRef, kFSCatInfoNodeFlags, &catInfo, 
                           NULL, NULL, NULL);
    B_THROW_IF_STATUS(err);
    
    isDirectory = ((catInfo.nodeFlags & kFSNodeIsDirectoryMask) != 0);
    
    std::vector<UInt8>  path(PATH_MAX);
    
    do
    {
        err = FSRefMakePath(&fileRef, &path[0], path.size());
        
        if (err == pathTooLongErr)
            path.resize(path.size() * 2);
        else
            B_THROW_IF_STATUS(err);
        
    } while (err != noErr);
    
    path.resize(strlen(reinterpret_cast<char*>(&path[0])));
    
    String  str(path, kCFStringEncodingUTF8, false);
    
    cfurl = CFURLCreateFromFileSystemRepresentation(NULL, 
                                                    &path[0], path.size(), 
                                                    isDirectory);
    B_THROW_IF_NULL(cfurl);
    
    if (mRef != NULL)
        CFRelease(mRef);
    
    mRef = cfurl;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Url&
Url::AssignPath(
    const B::String&    inPath, 
    bool                inIsDirectory,
    CFURLPathStyle      inPathStyle /* = kCFURLPOSIXPathStyle */)
{
    CFURLRef    cfurl;
    
    cfurl = CFURLCreateWithFileSystemPath(NULL, inPath.cf_ref(), inPathStyle, inIsDirectory);
    B_THROW_IF_NULL(cfurl);
    
    if (mRef != NULL)
        CFRelease(mRef);
    
    mRef = cfurl;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
void
Url::Clear()
{
    Assign(Url());
}

// ------------------------------------------------------------------------------------------
bool
Url::Empty() const
{
    return (CFEqual(CFURLGetString(mRef), NULL_URL));
}

// ------------------------------------------------------------------------------------------
bool
Url::IsStandard() const
{
    return (CFURLCanBeDecomposed(mRef));
}

// ------------------------------------------------------------------------------------------
String
Url::GetString() const
{
    return (String(CFURLGetString(mRef)));
}

// ------------------------------------------------------------------------------------------
bool
Url::GetBase(Url& outBase) const
{
    CFURLRef    baseUrl = CFURLGetBaseURL(mRef);
    bool        good    = (baseUrl != NULL);
    
    if (good)
    {
        outBase.Assign(OSPtr<CFURLRef>(baseUrl));
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetScheme(String& outScheme) const
{
    CFStringRef tempStr = CFURLCopyScheme(mRef);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outScheme.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetSchemeSpecific(String& outSchemeSpecific) const
{
    String          tempStr;
    bool            absolutePath;
    MutableString   specific;
    
    //outSchemeSpecific.clear();
    
    if (IsStandard())
    {
        if (GetAuthority(tempStr))
        {
            specific += "//";
            specific += tempStr;
        }
        
        if (GetPath(tempStr, absolutePath))
        {
            if (absolutePath)
                specific += "/";
            
            specific += tempStr;
        }
    }
    
    CFStringRef tempStrRef  = CFURLCopyResourceSpecifier(mRef);
    
    if (tempStrRef != NULL)
    {
        OSPtr<CFStringRef>  tempStrPtr(tempStrRef, from_copy);
        
        specific += tempStrPtr;
    };
    
    outSchemeSpecific = specific;
    
    return (!specific.empty());
}

// ------------------------------------------------------------------------------------------
bool
Url::GetAuthority(String& outAuthority) const
{
    CFStringRef tempStr = CFURLCopyNetLocation(mRef);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outAuthority.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetUser(String& outUser) const
{
    CFStringRef tempStr = CFURLCopyUserName(mRef);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outUser.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetPassword(String& outPassword) const
{
    CFStringRef tempStr = CFURLCopyPassword(mRef);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outPassword.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetHost(String& outHost) const
{
    CFStringRef tempStr = CFURLCopyHostName(mRef);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outHost.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetPort(int& outPort) const
{
    int port    = CFURLGetPortNumber(mRef);
    
    if (port != -1)
    {
        outPort = port;
    }
    
    return (port != -1);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetPath(String& outPath, bool& outAbsolute) const
{
    Boolean     isAbsolute;
    CFStringRef tempStr = CFURLCopyStrictPath(mRef, &isAbsolute);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outPath.assign(tempStr, from_copy);
        outAbsolute = isAbsolute;
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetPathExtension(String& outExtension) const
{
    CFStringRef tempStr = CFURLCopyPathExtension(mRef);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outExtension.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetParameters(String& outParameters) const
{
    CFStringRef tempStr = CFURLCopyParameterString(mRef, NULL);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outParameters.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetQuery(String& outQuery) const
{
    CFStringRef tempStr = CFURLCopyQueryString(mRef, NULL);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outQuery.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
Url::GetFragment(String& outFragment) const
{
    CFStringRef tempStr = CFURLCopyFragment(mRef, NULL);
    bool        good    = (tempStr != NULL);
    
    if (good)
    {
        outFragment.assign(tempStr, from_copy);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
void
Url::Copy(
    std::string&        outData, 
    bool                inEscapeWhitespace) const
{
    OSPtr<CFDataRef>    data(CFURLCreateData(NULL, mRef, kCFStringEncodingUTF8, 
                                             inEscapeWhitespace), 
                             from_copy);
    
    outData.assign(reinterpret_cast<const char*>(CFDataGetBytePtr(data)), 
                   CFDataGetLength(data));
}

// ------------------------------------------------------------------------------------------
void
Url::Copy(
    std::vector<UInt8>& outData, 
    bool                inEscapeWhitespace, 
    CFStringEncoding    inEncoding /* = kCFStringEncodingUTF8 */) const
{
    OSPtr<CFDataRef>    data(CFURLCreateData(NULL, mRef, inEncoding, 
                                             inEscapeWhitespace), 
                             from_copy);
    
    outData.resize(CFDataGetLength(data));
    
    BlockMoveData(CFDataGetBytePtr(data), &outData[0], outData.size());
}

// ------------------------------------------------------------------------------------------
void
Url::Copy(FSRef& fileRef) const
{
    if (!Copy(fileRef, std::nothrow))
        B_THROW(FileNotFoundException());
}

// ------------------------------------------------------------------------------------------
void
Url::Copy(FSRef& dirRef, HFSUniStr255& fileName) const
{
    String  name(TopPath());
    Url     parentUrl(PopPath());
    
    parentUrl.Copy(dirRef);
    fileName.length = name.copy(fileName.unicode, 
                                sizeof(fileName.unicode) / sizeof(fileName.unicode[0]));
}

// ------------------------------------------------------------------------------------------
void
Url::CopyPath(std::string& path) const
{
    OSPtr<CFStringRef>  pathStr(CFURLCopyFileSystemPath(mRef, kCFURLPOSIXPathStyle), from_copy);
    
    path = make_utf8_string(pathStr);
}

// ------------------------------------------------------------------------------------------
Url
Url::Absolute() const
{
    OSPtr<CFURLRef> url(CFURLCopyAbsoluteURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
Url
Url::PushPath(const String& name, bool is_dir) const
{
    OSPtr<CFURLRef> url(CFURLCreateCopyAppendingPathComponent(NULL, mRef, 
                                                              name.cf_ref(), is_dir), 
                        from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
Url
Url::PopPath() const
{
    OSPtr<CFURLRef> url(CFURLCreateCopyDeletingLastPathComponent(NULL, mRef), 
                        from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
String
Url::TopPath() const
{
    return (String(CFURLCopyLastPathComponent(mRef), from_copy));
}

// ------------------------------------------------------------------------------------------
bool
Url::IsFileUrl() const
{
    bool    fileUrl = false;
    String  scheme;
    
    if (GetScheme(scheme))
    {
        fileUrl = (CFStringCompare(scheme.cf_ref(), CFSTR("file"), kCFCompareCaseInsensitive) == 0);
    }
    
    return (fileUrl);
}

// ------------------------------------------------------------------------------------------
String
Url::GetFilename() const
{
    return (String(CFURLCopyLastPathComponent(mRef), from_copy));
}

// ------------------------------------------------------------------------------------------
Url
Url::Find(short domain, OSType folder, bool create)
{
    FSRef   ref;
    OSStatus    err;
    
    err = FSFindFolder(domain, folder, create, &ref);
    B_THROW_IF_STATUS(err);
    
    return (Url(ref));
}

// ------------------------------------------------------------------------------------------
bool
Url::Find(short domain, OSType folder, bool create, Url& url)
{
    FSRef       ref;
    OSStatus    err;
    
    url.Clear();
    
    err = FSFindFolder(domain, folder, create, &ref);
    
    if (err == noErr)
    {
        url = ref;
    }
    
    return (err == noErr);
}

// ------------------------------------------------------------------------------------------
String
Url::Escape(
    const String&       inString, 
    CFStringRef         inCharactersToLeaveUnescaped /* = NULL */, 
    CFStringRef         inLegalURLCharactersToBeEscaped /* = NULL */, 
    CFStringEncoding    inEncoding /* = kCFStringEncodingUTF8 */)
{
    return (String(CFURLCreateStringByAddingPercentEscapes(NULL, 
                       inString.cf_ref(), 
                       inCharactersToLeaveUnescaped, 
                       inLegalURLCharactersToBeEscaped, 
                       inEncoding), 
                    from_copy));
}

// ------------------------------------------------------------------------------------------
String
Url::Unescape(
    const String&       inString, 
    CFStringRef         inCharactersToLeaveEscaped /* = CFSTR("") */)
{
    return (String(CFURLCreateStringByReplacingPercentEscapes(NULL, 
                        inString.cf_ref(), 
                        inCharactersToLeaveEscaped), 
                    from_copy));
}


}   // namespace B
