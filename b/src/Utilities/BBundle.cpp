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
#include "BBundle.h"

// system headers
#include <ApplicationServices/ApplicationServices.h>

// B headers
#include "BString.h"
#include "BStringUtilities.h"
#include "BUrl.h"
#include "CFUtils.h"


namespace
{
    // Matches an extension from a vector of extensions.  Ignores 
    // wildcard entries.
    class MatchByExtension
    {
    public:
    
        MatchByExtension(const B::String& inExtension)
            : mExtension(inExtension) {}
        bool operator () (const B::Bundle::DocumentType& docType) const;
        bool operator () (const B::String& docExt) const;
    
    private:
        const B::String&    mExtension;
    };
    
    bool
    MatchByExtension::operator () (const B::Bundle::DocumentType& docType) const
    {
        std::vector<B::String>::const_iterator  it;
        
        it = std::find_if(docType.mExtensions.begin(), 
                          docType.mExtensions.end(), 
                          *this);
        
        return (it != docType.mExtensions.end());
    }
    
    bool
    MatchByExtension::operator () (const B::String& docExt) const
    {
        if (docExt == CFSTR("*"))
            return (false);
        
        return (CFStringCompare(docExt.cf_ref(), mExtension.cf_ref(), kCFCompareCaseInsensitive) == 0);
    }
    
    // Matches a MIME type from a vector of MIME types.
    class MatchByMIMEType
    {
    public:
    
        MatchByMIMEType(const B::String& inMIMEType)
            : mMIMEType(inMIMEType) {}
        bool operator () (const B::Bundle::DocumentType& docType) const;
        bool operator () (const B::String& docExt) const;
    
    private:
        const B::String&    mMIMEType;
    };
    
    bool
    MatchByMIMEType::operator () (const B::Bundle::DocumentType& docType) const
    {
        std::vector<B::String>::const_iterator  it;
        
        it = std::find_if(docType.mMIMETypes.begin(), 
                          docType.mMIMETypes.end(), 
                          *this);
        
        return (it != docType.mMIMETypes.end());
    }
    
    bool
    MatchByMIMEType::operator () (const B::String& docExt) const
    {
        // We assume that MIME types are case sensitive
        
        return (mMIMEType == docExt);
    }
    
    // Matches an file type from a vector of file types.  Ignores 
    // wildcard entries.
    class MatchByFileType
    {
    public:
    
        MatchByFileType(OSType inFileType)
            : mFileType(inFileType) {}
        bool operator () (const B::Bundle::DocumentType& docType) const;
        bool operator () (OSType docFileType) const;
    
    private:
        OSType  mFileType;
    };
    
    bool
    MatchByFileType::operator () (const B::Bundle::DocumentType& docType) const
    {
        std::vector<OSType>::const_iterator it;
        
        it = std::find_if(docType.mOSTypes.begin(), 
                          docType.mOSTypes.end(), 
                          *this);
        
        return (it != docType.mOSTypes.end());
    }
    
    bool
    MatchByFileType::operator () (OSType docFileType) const
    {
        if (docFileType == '****')
            return (false);
        
        return (docFileType == mFileType);
    }
    
    // Matches a wildcard extension from a vector of extensions.
    bool MatchByWildcardExtension(const B::Bundle::DocumentType& docType)
    {
        std::vector<B::String>::const_iterator  it;
        
        it = std::find(docType.mExtensions.begin(), 
                       docType.mExtensions.end(), 
                       CFSTR("*"));
        
        return (it != docType.mExtensions.end());
    }
    
    // Matches a wildcard file type from a vector of file types.
    bool MatchByWildcarFileType(const B::Bundle::DocumentType& docType)
    {
        std::vector<OSType>::const_iterator it;
        
        it = std::find(docType.mOSTypes.begin(), 
                       docType.mOSTypes.end(), 
                       static_cast<OSType>('****'));
        
        return (it != docType.mOSTypes.end());
    }

    class MatchByTypeName : public std::unary_function<B::Bundle::DocumentType, bool>
    {
    public:
    
        MatchByTypeName(const B::String& inTypeName)
            : mTypeName(inTypeName) {}
        bool operator () (const B::Bundle::DocumentType& value) const;
    
    private:
        const B::String&    mTypeName;
    };
    
    bool
    MatchByTypeName::operator () (const B::Bundle::DocumentType& docType) const
    {
        return (CFStringCompare(docType.mName.cf_ref(), mTypeName.cf_ref(), kCFCompareCaseInsensitive) == 0);
    }
}

namespace B {


// ------------------------------------------------------------------------------------------
/*! @note   This function does not necessarily return what CoreFoudation 
            considers to be the "main bundle".  That's because for plug-ins, 
            the main bundle is the hosting application's bundle, not the 
            bundle containing the plugin.  However for applications they are 
            one and the same.
*/
Bundle&
Bundle::GetMainBundle()
{
    static Bundle   sMainBundle(CFBundleGetMainBundle());
    
    return (sMainBundle);
}

// ------------------------------------------------------------------------------------------
/*! Call this function when building a plug-in of some kind, where the CF 
    main bundle (as returned by @c CFBundleGetMainBundle()) does not 
    correspond to the bundle containing the executable of interest.
    
    @note   This function must be called @b very early in the initialisation 
            process, prior to anyone calling Bundle::Main().
*/
void
Bundle::SetMainBundle(const Bundle& inBundle)
{
    GetMainBundle() = inBundle;
}

// ------------------------------------------------------------------------------------------
/*! Note that this retrieves the (pre-existing) main bundle, it doesn't 
    create a new bundle.
*/
Bundle::Bundle()
    : mRef(GetMainBundle().cf_ref())
{
    B_ASSERT(mRef != NULL);
    
    CFRetain(mRef);
}

// ------------------------------------------------------------------------------------------
Bundle::Bundle(
    const Bundle&   bundle) //!< The source bundle.
        : mRef(bundle.mRef), mCreator(bundle.mCreator), mType(bundle.mType)
{
    CFRetain(mRef);
}

// ------------------------------------------------------------------------------------------
/*! Will throw an exception if the bundle can't be created (perhaps because 
    @a url is bogus or doesn't point at the right location or you don't 
    have permission to access it).
*/
Bundle::Bundle(
    const Url&  url)    //!< The new bundle's location.
{
    mRef = CFBundleCreate(NULL, url.cf_ref());
    B_THROW_IF_NULL(mRef);
    
    CFBundleGetPackageInfo(mRef, &mType, &mCreator);
}

// ------------------------------------------------------------------------------------------
/*! Note that this retrieves a pre-existing bundle, it doesn't 
    create a new one.
*/
Bundle::Bundle(
    const String&   identifier) //!< The bundle's unique identifier.
{
    mRef = CFBundleGetBundleWithIdentifier(identifier.cf_ref());
    B_THROW_IF_NULL(mRef);
    
    CFRetain(mRef);
    
    CFBundleGetPackageInfo(mRef, &mType, &mCreator);
}

// ------------------------------------------------------------------------------------------
Bundle::Bundle(
    OSPtr<CFBundleRef>  cfbundle)   //!< A pre-existing bundle.
        : mRef(cfbundle)
{
    B_ASSERT(mRef != NULL);
    
    CFRetain(mRef);
    
    CFBundleGetPackageInfo(mRef, &mType, &mCreator);
}

// ------------------------------------------------------------------------------------------
Bundle::Bundle(
    CFBundleRef cfbundle)           //!< A pre-existing bundle.
        : mRef(cfbundle)
{
    B_ASSERT(mRef != NULL);
    
    CFRetain(mRef);
    CFBundleGetPackageInfo(mRef, &mType, &mCreator);
}

// ------------------------------------------------------------------------------------------
Bundle::Bundle(
    CFBundleRef cfbundle,           //!< A pre-existing bundle.
    const from_copy_t&)
        : mRef(cfbundle)
{
    B_ASSERT(mRef != NULL);
    
    CFBundleGetPackageInfo(mRef, &mType, &mCreator);
}

// ------------------------------------------------------------------------------------------
Bundle::~Bundle()
{
    CFRelease(mRef);
}

// ------------------------------------------------------------------------------------------
Bundle&
Bundle::Assign(const Bundle& bundle)
{
    Bundle  tempBundle(bundle);
    
    swap(tempBundle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Bundle&
Bundle::Assign(const Url& url)
{
    Bundle  tempBundle(url);
    
    swap(tempBundle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Bundle&
Bundle::Assign(const String& identifier)
{
    Bundle  tempBundle(identifier);
    
    swap(tempBundle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Bundle&
Bundle::Assign(OSPtr<CFBundleRef> cfbundle)
{
    Bundle  tempBundle(cfbundle);
    
    swap(tempBundle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Bundle&
Bundle::Assign(CFBundleRef cfbundle)
{
    Bundle  tempBundle(cfbundle);
    
    swap(tempBundle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
Bundle&
Bundle::Assign(CFBundleRef cfbundle, const from_copy_t& fc)
{
    Bundle  tempBundle(cfbundle, fc);
    
    swap(tempBundle);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
void
Bundle::swap(Bundle& bundle)
{
    std::swap(mRef,     bundle.mRef);
    std::swap(mType,    bundle.mType);
    std::swap(mCreator, bundle.mCreator);
}

// ------------------------------------------------------------------------------------------
/*! This is a required key, so the function should never fail for 
    well-formed bundles.
*/
String
Bundle::Identifier() const
{
    return (String(CFBundleGetIdentifier(mRef)));
}

// ------------------------------------------------------------------------------------------
/*! This is a required key, so the function should never fail for 
    well-formed bundles.
*/
String
Bundle::Name() const
{
    return (InfoString(kCFBundleNameKey));
}

// ------------------------------------------------------------------------------------------
/*! This is a required key, so the function should never fail for 
    well-formed bundles.
*/
String
Bundle::ExecutableName() const
{
    return (InfoString(kCFBundleExecutableKey));
}

// ------------------------------------------------------------------------------------------
/*! This is a required key, so the function should never fail for 
    well-formed bundles.
*/
UInt32
Bundle::BuildNumber() const
{
    return (CFBundleGetVersionNumber(mRef));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::DevelopmentRegion() const
{
    return (String(CFBundleGetDevelopmentRegion(mRef), std::nothrow));
}

// ------------------------------------------------------------------------------------------
/*! This is a required key, so the function should never fail for 
    well-formed bundles.
*/
String
Bundle::DictionaryVersion() const
{
    return (InfoString(kCFBundleInfoDictionaryVersionKey));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns NULL.
*/
OSPtr<CFArrayRef>
Bundle::GetDocumentTypes() const
{
    return (InfoKey<CFArrayRef>(CFSTR("CFBundleDocumentTypes")));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::Copyright() const
{
    return (InfoString(CFSTR("NSHumanReadableCopyright")));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::GetInfo() const
{
    return (InfoString(CFSTR("CFBundleGetInfoString")));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::GetInfoHTML() const
{
    return (InfoString(CFSTR("CFBundleGetInfoHTML")));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::ShortVersionString() const
{
    return (InfoString(CFSTR("CFBundleShortVersionString")));
}

// ------------------------------------------------------------------------------------------
/*! This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::GetHelpBook() const
{
    return (InfoString(CFSTR("CFBundleHelpBookName")));
}

// ------------------------------------------------------------------------------------------
/*! This key is used primarily for Cocoa applications, but since we use nibs 
    too then it's convenient for us to usurp it.
    
    This is an optional key, so if it's absent the function returns an 
    empty string.
*/
String
Bundle::MainNib() const
{
    return (InfoString(CFSTR("NSMainNibFile")));
}

// ------------------------------------------------------------------------------------------
/*! If @a inKey is absent, the function returns an empty string.
    
    Calling this function for a key that isn't a string results in undefined 
    behaviour.
*/
String
Bundle::InfoString(
    CFStringRef inKey)  //!< The key to look for.
    const
{
    OSPtr<CFStringRef>  str;
    
    str = InfoKey<CFStringRef>(inKey);
    
    return (String(str != NULL ? str.get() : CFSTR("")));
}

// ------------------------------------------------------------------------------------------
Url
Bundle::Location() const
{
    OSPtr<CFURLRef> url(CFBundleCopyBundleURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
/*! This is strictly an operation on URLs.  A succesful return doesn't imply 
    that the directory actually exists.
*/
Url
Bundle::Resources() const
{
    OSPtr<CFURLRef> url(CFBundleCopyResourcesDirectoryURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
/*! This is strictly an operation on URLs.  A succesful return doesn't imply 
    that the directory actually exists.
*/
Url
Bundle::PrivateFrameworks() const
{
    OSPtr<CFURLRef> url(CFBundleCopyPrivateFrameworksURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
/*! This is strictly an operation on URLs.  A succesful return doesn't imply 
    that the directory actually exists.
*/
Url
Bundle::SharedFrameworks() const
{
    OSPtr<CFURLRef> url(CFBundleCopySharedFrameworksURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
/*! This is strictly an operation on URLs.  A succesful return doesn't imply 
    that the directory actually exists.
*/
Url
Bundle::SharedSupport() const
{
    OSPtr<CFURLRef> url(CFBundleCopySharedSupportURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
/*! This is strictly an operation on URLs.  A succesful return doesn't imply 
    that the directory actually exists.
*/
Url
Bundle::PlugIns() const
{
    OSPtr<CFURLRef> url(CFBundleCopyBuiltInPlugInsURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
Url
Bundle::Executable() const
{
    OSPtr<CFURLRef> url(CFBundleCopyExecutableURL(mRef), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
/*! This is strictly an operation on URLs.  A succesful return doesn't imply 
    that the directory actually exists.
*/
Url
Bundle::AuxiliaryExecutable(const String& inName) const
{
    OSPtr<CFURLRef> url(CFBundleCopyAuxiliaryExecutableURL(mRef, inName.cf_ref()), from_copy);
    
    return (Url(url));
}

// ------------------------------------------------------------------------------------------
Bundle::Iterator
Bundle::begin() const
{
    if (mDocumentTypes.empty())
        MakeDocumentTypes(GetDocumentTypes(), mDocumentTypes);
    
    return (mDocumentTypes.begin());
}

// ------------------------------------------------------------------------------------------
Bundle::Iterator
Bundle::end() const
{
    if (mDocumentTypes.empty())
        MakeDocumentTypes(GetDocumentTypes(), mDocumentTypes);
    
    return (mDocumentTypes.end());
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if @a inDocClass couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeByOSAClass(
    DescType        inDocClass) //!< The document class we're looking for.
    const
{
    return (FindDocumentTypeByOSAClass(begin(), end(), inDocClass));
}

// ------------------------------------------------------------------------------------------
/*! Returns @a inLast if @a inDocClass couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeByOSAClass(
    Iterator        inFirst,    //!< The start of the sequence to look at.
    Iterator        inLast,     //!< The end of the sequence to look at.
    DescType        inDocClass) //!< The document class we're looking for.
{
    return (std::find_if(inFirst, inLast, 
                boost::bind(
                    std::equal_to<DescType>(),
                    boost::bind(&DocumentType::GetOSAClass, _1),
                    inDocClass)));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if a document type matching the extension and/or file 
    file of @a inUrl couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForUrl(
    const Url&      inUrl)  //!< The url for which to find a matching document type.
    const
{
    return (FindDocumentTypeForUrl(begin(), end(), inUrl));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c inLast() if a document type matching the extension and/or file 
    file of @a inUrl couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForUrl(
    Iterator        inFirst,    //!< The start of the sequence to look at.
    Iterator        inLast,     //!< The end of the sequence to look at.
    const Url&      inUrl)      //!< The url for which to find a matching document type.
{
    LSItemInfoRecord    itemInfo;
    OSPtr<CFStringRef>  extensionStr;
    OSStatus            err;
    
    err = LSCopyItemInfoForURL(inUrl.cf_ref(), 
                               kLSRequestTypeCreator | kLSRequestExtension, 
                               &itemInfo);
    B_THROW_IF_STATUS(err);
    
    if (itemInfo.extension != NULL)
        extensionStr.reset(itemInfo.extension, from_copy);
    
    return (FindDocumentTypeForFileInfo(inFirst, inLast, 
                                        itemInfo.extension, 
                                        itemInfo.filetype));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if a document type matching @a inExtension and/or 
    @a inFileType couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForFileInfo(
    CFStringRef     inExtension,    //!< The extension for which to find a matching document type.  May be @c NULL.
    OSType          inFileType)     //!< The file type for which to find a matching document type.
    const
{
    return (FindDocumentTypeForFileInfo(begin(), end(), inExtension, inFileType));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c inLast() if a document type matching @a inExtension and/or 
    @a inFileType couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForFileInfo(
    Iterator        inFirst,        //!< The start of the sequence to look at.
    Iterator        inLast,         //!< The end of the sequence to look at.
    CFStringRef     inExtension,    //!< The extension for which to find a matching document type.  May be @c NULL.
    OSType          inFileType)     //!< The file type for which to find a matching document type.
{
    Iterator    it  = inLast;
    
    // 1. If there's an extension, try to match it while ignoring 
    // wildcard extensions.
    
    if (inExtension != NULL)
    {
        it = std::find_if(inFirst, inLast, 
                          MatchByExtension(String(inExtension)));
    }
    
    // 2.  If there's a filetype, try to match it while ignoring 
    // wildcard filetypes.
    
    if ((it == inLast) && (inFileType != 0) && (inFileType != '\?\?\?\?'))
    {
        it = std::find_if(inFirst, inLast, MatchByFileType(inFileType));
    }
    
    // 3. Try to match a wildcard extension.
    
    if (it == inLast)
    {
        it = std::find_if(inFirst, inLast, MatchByWildcardExtension);
    }
    
    // 4.  Try to match a wildcard filetype.
    
    if (it == inLast)
    {
        it = std::find_if(inFirst, inLast, MatchByWildcarFileType);
    }
    
    return (it);
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if a document type matching the type name @a inTypeName 
    couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForTypeName(
    const String&   inTypeName) //!< The document type name we're looking for.
    const
{
    return (FindDocumentTypeForTypeName(begin(), end(), inTypeName));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if a document type matching the type name @a inTypeName 
    couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForTypeName(
    Iterator        inFirst,    //!< The start of the sequence to look at.
    Iterator        inLast,     //!< The end of the sequence to look at.
    const String&   inTypeName) //!< The document type name we're looking for.
{
    return (std::find_if(inFirst, inLast, MatchByTypeName(inTypeName)));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if a document type matching the MIME type @a inTypeName 
    couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForMIMEType(
    const String&   inMIMEType) //!< The document type name we're looking for.
    const
{
    return (FindDocumentTypeForMIMEType(begin(), end(), inMIMEType));
}

// ------------------------------------------------------------------------------------------
/*! Returns @c end() if a document type matching the MIME type @a inTypeName 
    couldn't be found.
*/
Bundle::Iterator
Bundle::FindDocumentTypeForMIMEType(
    Iterator        inFirst,    //!< The start of the sequence to look at.
    Iterator        inLast,     //!< The end of the sequence to look at.
    const String&   inMIMEType) //!< The document type name we're looking for.
{
    return (std::find_if(inFirst, inLast, MatchByTypeName(inMIMEType)));
}

// ------------------------------------------------------------------------------------------
bool
Bundle::DoesFileInfoMatchDocumentType(
    CFStringRef         inExtension, 
    OSType              inFileType, 
    const DocumentType& inDocumentType)
{
    bool    matches = false;
    
    // 1. If there's an extension, try to match it while ignoring 
    // wildcard extensions.
    
    if (inExtension != NULL)
    {
        matches = MatchByExtension(String(inExtension))(inDocumentType);
    }
    
    // 2. If there's a filetype, try to match it while ignoring 
    // wildcard filetypes.
    
    if (!matches && (inFileType != 0) && (inFileType != '\?\?\?\?'))
    {
        matches = MatchByFileType(inFileType)(inDocumentType);
    }
    
    // 3. Try to match a wildcard extension.
    
    if (!matches)
    {
        matches = MatchByWildcardExtension(inDocumentType);
    }
    
    // 4. Try to match a wildcard filetype.
    
    if (!matches)
    {
        matches = MatchByWildcarFileType(inDocumentType);
    }
    
    return (matches);
}

// ------------------------------------------------------------------------------------------
bool
Bundle::DoesMIMETypeMatchDocumentType(
    CFStringRef         inMIMEType, 
    const DocumentType& inDocumentType)
{
    bool    matches = false;
    
    if (inMIMEType != NULL)
    {
        matches = MatchByMIMEType(String(inMIMEType))(inDocumentType);
    }
    
    return (matches);
}

// ------------------------------------------------------------------------------------------
void
Bundle::MakeDocumentTypes(
    OSPtr<CFArrayRef>           inDocumentTypesArray,   //!< The array of document types entries.
    std::vector<DocumentType>&  outDocumentTypesVector) //!< The output DocumentType structures.
{
    outDocumentTypesVector.clear();
    
    if (inDocumentTypesArray != NULL)
    {
        CFIndex count   = CFArrayGetCount(inDocumentTypesArray);
        
        outDocumentTypesVector.reserve(count);
        
        for (CFIndex i = 0; i < count; i++)
        {
            OSPtr<CFDictionaryRef>  docTypeDict;
            
            if (CFUGet(inDocumentTypesArray, i, docTypeDict))
            {
                DocumentType    docType;
                
                ConvertDocumentType(docTypeDict, docType);
                outDocumentTypesVector.push_back(docType);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------
void
Bundle::ConvertDocumentType(
    OSPtr<CFDictionaryRef>  inDocTypeDict,
    DocumentType&           outDocType)
{
    String  tempStr;
    
    outDocType.mName        = GetDocTypeEntry(inDocTypeDict, CFSTR("CFBundleTypeName"));
    outDocType.mRole        = GetDocTypeEntry(inDocTypeDict, CFSTR("CFBundleTypeRole"));
    outDocType.mIconFile    = GetDocTypeEntry(inDocTypeDict, CFSTR("CFBundleTypeIconFile"));
    
    CFUGetNumber(inDocTypeDict, CFSTR("LSTypeIsPackage"), outDocType.mIsPackage);
    
    tempStr = GetDocTypeEntry(inDocTypeDict, CFSTR("B::OSAClass"));
    
    if (!tempStr.empty())
        outDocType.mOSAClass = make_ostype(tempStr.cf_ref());
    else
        outDocType.mOSAClass = cDocument;
    
    OSPtr<CFArrayRef>   extensions;
    
    if (CFUGet(inDocTypeDict, CFSTR("CFBundleTypeExtensions"), extensions))
    {
        for (CFIndex i = 0; i < CFArrayGetCount(extensions); i++)
        {
            outDocType.mExtensions.push_back(GetArrayEntry(extensions, i));
        }
    }
    
    OSPtr<CFArrayRef>   mimeTypes;
    
    if (CFUGet(inDocTypeDict, CFSTR("CFBundleTypeMIMETypes"), mimeTypes))
    {
        for (CFIndex i = 0; i < CFArrayGetCount(mimeTypes); i++)
        {
            outDocType.mMIMETypes.push_back(GetArrayEntry(mimeTypes, i));
        }
    }
    
    OSPtr<CFArrayRef>   exportableTypes;
    
    if (CFUGet(inDocTypeDict, CFSTR("NSExportableAs"), exportableTypes))
    {
        for (CFIndex i = 0; i < CFArrayGetCount(exportableTypes); i++)
        {
            outDocType.mExportableTypes.push_back(GetArrayEntry(exportableTypes, i));
        }
    }
    
    OSPtr<CFArrayRef>   contentTypes;
    
    if (CFUGet(inDocTypeDict, CFSTR("LSItemContentTypes"), contentTypes))
    {
        for (CFIndex i = 0; i < CFArrayGetCount(contentTypes); i++)
        {
            outDocType.mContentTypes.push_back(GetArrayEntry(contentTypes, i));
        }
    }
    
    OSPtr<CFArrayRef>   fileTypes;
    
    if (CFUGet(inDocTypeDict, CFSTR("CFBundleTypeOSTypes"), fileTypes))
    {
        for (CFIndex i = 0; i < CFArrayGetCount(fileTypes); i++)
        {
            tempStr = GetArrayEntry(fileTypes, i);
            outDocType.mOSTypes.push_back(make_ostype(tempStr.cf_ref()));
        }
    }
}

// ------------------------------------------------------------------------------------------
String
Bundle::GetDocTypeEntry(
    OSPtr<CFDictionaryRef>  inDocTypeDict, 
    CFStringRef             inKey)
{
    OSPtr<CFStringRef>  strRef;
    String              str;
    
    if (CFUGet(inDocTypeDict, inKey, strRef))
        str = strRef;
    
    return (str);
}

// ------------------------------------------------------------------------------------------
String
Bundle::GetArrayEntry(
    OSPtr<CFArrayRef>       inArray, 
    CFIndex                 inIndex)
{
    OSPtr<CFStringRef>  strRef;
    String              str;
    
    if (CFUGet(inArray, inIndex, strRef))
        str = strRef;
    
    return (str);
}

// ------------------------------------------------------------------------------------------
Url
Bundle::Resource(
    const String&       name,                           //!< The resource's file name.
    const String&       type /* = String() */,          //!< If non-empty, specifies an extension for the file.
    const String&       subDirectory /* = String() */)  //!< If non-empty, specifies a subdirectory under Resources or Resources/\<lang\>.lproj
    const
{
    OSPtr<CFURLRef> url(CFBundleCopyResourceURL(
                            mRef, name.cf_ref(), 
                            !type.empty() ? type.cf_ref() : NULL, 
                            !subDirectory.empty() ? subDirectory.cf_ref() : NULL), 
                        from_copy);

    return (Url(url));
}


// ------------------------------------------------------------------------------------------
void
Bundle::Resources(
    std::vector<Url>&   urls, 
    const String&       type,                           //!< All of the returned files will have this extension.
    const String&       subDirectory /* = String() */)  //!< If non-empty, specifies a subdirectory under Resources or Resources/\<lang\>.lproj
    const
{
    CFArrayRef  arrayRef    = CFBundleCopyResourceURLsOfType(
                                mRef, type.cf_ref(), 
                                !subDirectory.empty() ? subDirectory.cf_ref() : NULL);
    
    urls.clear();
    
    if (arrayRef != NULL)
    {
        OSPtr<CFArrayRef>   array(arrayRef, from_copy);
        CFIndex             count   = CFArrayGetCount(array);
        
        urls.reserve(count);
        
        for (CFIndex i = 0; i < count; i++)
        {
            OSPtr<CFURLRef> url;
            
            if (!CFUGet<CFURLRef>(array, i, url))
                continue;
            
            urls.push_back(Url(url));
        }
    }
}

// ------------------------------------------------------------------------------------------
String
Bundle::GetLocalisedString(
    CFStringRef         inKey, 
    CFStringRef         inValue, 
    CFStringRef         inTableName) const
{
    return (String(CFBundleCopyLocalizedString(mRef, inKey, inValue, inTableName), from_copy));
}


}   // namespace B
