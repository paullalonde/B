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

#ifndef BBundle_H_
#define BBundle_H_

#pragma once

// system headers
#include <CoreFoundation/CFBundle.h>

// library headers
#include "BString.h"
#include "CFUtils.h"


namespace B {

// forward declarations
class	Url;

/*!
	@brief	Wrapper class around @c CFBundle.
	
	The member functions essentially mirror the CFBundle API, 
	except that arguments and returned values are higher-level 
	(i.e. String instead of @c CFStringRef).
	
	There are also quite a number of functions for enumerating the 
	contents of the @c CFBundleDocumentTypes array.  Elements of that 
	array are returned as DocumentType structures, allowing for more 
	convenient access.
	
	A number of related <tt>#define</tt>s provide support for localised strings, 
	and are designed to integrate with Apple's localisation tools such 
	as @c genstrings.  They are:
	
		- #BLocalizedString
		- #BLocalizedStringFromTable
		- #BLocalizedStringFromTableInBundle
		- #BLocalizedStringWithDefaultValue
*/
class Bundle
{
public:
	
	/*!	Collects the keys for an entry in the CFBundleDocumentTypes array, 
		making them easier to access.
	*/
	struct DocumentType
	{
		DescType	GetOSAClass() const	{ return (mOSAClass); }
		
		String				mName;				//!< The name of this document type.
		String				mRole;				//!< How the application interacts with this document type.
		String				mIconFile;			//!< The icon for this document type.
		DescType			mOSAClass;			//!< B extension.
		bool				mIsPackage;			//!< Is the document a package?
		std::vector<String>	mExtensions;		//!< The extensions for this document type.
		std::vector<String>	mMIMETypes;			//!< The MIME types for this document type.
		std::vector<String>	mExportableTypes;	//!< The exportable types for this document type.
		std::vector<String>	mContentTypes;		//!< The UTI types for this document type.
		std::vector<OSType>	mOSTypes;			//!< The file types for this document type.
	};
	
	typedef std::vector<DocumentType>			DocumentTypeArray;
	typedef DocumentTypeArray::const_iterator	Iterator;	//!< Used to iterate over the CFBundleDocumentTypes array.
	
	//! @name Singleton Access
	//@{
	//! Returns the "main" bundle.
	static const Bundle&	Main();
	//! Changes the "main" bundle.
	static void				SetMainBundle(const Bundle& inBundle);
	//@}
	
	//! @name Constructors & Destructor
	//@{
	//! Default constructor.  Initialises itself with the main bundle.
				Bundle();
	//! Copy constructor.
				Bundle(const Bundle& bundle);
	//! Creates a bundle from the given @a url.
	explicit	Bundle(const Url& url);
	//! Initialises itself with the bundle having the given @a identifer.
	explicit	Bundle(const String& identifier);
	//! Initialises itself with the OSPtr<CFBundleRef>.
	explicit	Bundle(OSPtr<CFBundleRef> cfbundle);
	//! Initialises itself with the CFBundleRef.
	explicit	Bundle(CFBundleRef cfbundle);
	//! Initialises itself with the CFBundleRef.
	explicit	Bundle(CFBundleRef cfbundle, const from_copy_t&);
	//! Destructor.  Releases the underlying CFBundleRef.
				~Bundle();
	//@}
	
	//! @name Assignment
	//@{
	//! Assignment operator.
	Bundle&	operator = (const Bundle& bundle);
	/*! @overload
	*/
	Bundle&	operator = (const Url& url);
	/*! @overload
	*/
	Bundle&	operator = (const String& identifier);
	/*! @overload
	*/
	Bundle&	operator = (OSPtr<CFBundleRef> cfbundle);
	//! Assignment.  The new bundle is retained, and the old one is released.
	Bundle&	Assign(const Bundle& bundle);
	/*! @overload
	*/
	Bundle&	Assign(const Url& url);
	/*! @overload
	*/
	Bundle&	Assign(const String& identifier);
	/*! @overload
	*/
	Bundle&	Assign(OSPtr<CFBundleRef> cfbundle);
	/*! @overload
	*/
	Bundle&	Assign(CFBundleRef cfbundle);
	/*! @overload
	*/
	Bundle&	Assign(CFBundleRef cfbundle, const from_copy_t&);
	//! Exchange the contents of two bundles.
	void	swap(Bundle& bundle);
	//@}
	
	//! @name Bundle Identification
	//@{
	//! Returns the bundle's unique identifier.
	String	Identifier() const;
	//! Returns the bundle's creator.
	OSType	PackageCreator() const		{ return (mCreator); }
	//! Returns the bundle's file type.
	OSType	PackageType() const			{ return (mType); }
	//@}
	
	//! @name Info.plist keys
	//@{
	//! Returns the bundle's name.
	String	Name() const;
	//! Returns the name of the bundle's executable.
	String	ExecutableName() const;
	//! Returns the bundle's version.
	UInt32	BuildNumber() const;
	//! Returns the bundle's development region.
	String	DevelopmentRegion() const;
	//! Returns the bundle's dictionary version.
	String	DictionaryVersion() const;
	//! Returns the bundle's document types array.
	OSPtr<CFArrayRef>
			GetDocumentTypes() const;
	String	Copyright() const;
	String	GetInfo() const;
	String	GetInfoHTML() const;
	String	ShortVersionString() const;
	String	GetHelpBook() const;
	//! Returns the name of the application's main nib file (minus the @c .nib extension).
	String	MainNib() const;
	//! Returns a string-valued property list key.
	String	InfoString(CFStringRef inKey) const;
	//! Returns a property list key, cast to the given type.
	template <typename T> OSPtr<T>
			InfoKey(CFStringRef inKey) const;
#if 0
	//! Returns a property list key in @a value, cast to the given type.
	template <typename T> void
			InfoKey(CFStringRef inKey, OSPtr<T>& value) const;
#endif
	//@}
	
	//! @name Locations Within The Bundle
	//@{
	//! Returns the bundle's URL.
	Url		Location() const;
	//! Returns the URL of the bundle's Resources directory.
	Url		Resources() const;
	//! Returns the URL of the bundle's PrivateFrameworks directory.
	Url		PrivateFrameworks() const;
	//! Returns the URL of the bundle's SharedFrameworks directory.
	Url		SharedFrameworks() const;
	//! Returns the URL of the bundle's SharedSupport directory.
	Url		SharedSupport() const;
	//! Returns the URL of the bundle's PlugIns directory.
	Url		PlugIns() const;
	//! Returns the URL of the bundle's executable.
	Url		Executable() const;
	//! Returns the URL of file @a name in the same directory as the bundle's executable.
	Url		AuxiliaryExecutable(const String& inName) const;
	//@}
	
	//! @name Document Types
	//@{
	//! Returns an iterator pointing to beginning of the DocumentType array.
	Iterator		begin() const;
	//! Returns an iterator pointing to beginning of the DocumentType array.
	Iterator		end() const;
	//! Returns the document type matching @a inDocClass.
	Iterator		FindDocumentTypeByOSAClass(
						DescType		inDocClass) const;
	//! Returns the document type matching @a inDocClass.
	static Iterator	FindDocumentTypeByOSAClass(
						Iterator		inFirst, 
						Iterator		inLast, 
						DescType		inDocClass);
	//! Returns the document type matching the extension and/or file type of @a inUrl.
	Iterator		FindDocumentTypeForUrl(
						const Url&		inUrl) const;
	//! Returns the document type matching the extension and/or file type of @a inUrl.
	static Iterator	FindDocumentTypeForUrl(
						Iterator		inFirst, 
						Iterator		inLast, 
						const Url&		inUrl);
	//! Returns the document type matching the given extension and/or filename.
	Iterator		FindDocumentTypeForFileInfo(
						CFStringRef		inExtension, 
						OSType			inFileType) const;
	//! Returns the document type matching @a inTypeName.
	static Iterator	FindDocumentTypeForFileInfo(
						Iterator		inFirst, 
						Iterator		inLast, 
						CFStringRef		inExtension, 
						OSType			inFileType);
	//! Returns the document type matching @a inMIMEType.
	Iterator		FindDocumentTypeForMIMEType(
						const String&	inMIMEType) const;
	//! Returns the document type matching @a inMIMEType.
	static Iterator	FindDocumentTypeForMIMEType(
						Iterator		inFirst, 
						Iterator		inLast, 
						const String&	inMIMEType);
	//! Returns the document type matching @a inTypeName.
	Iterator		FindDocumentTypeForTypeName(
						const String&	inTypeName) const;
	//! Returns the document type matching @a inTypeName.
	static Iterator	FindDocumentTypeForTypeName(
						Iterator		inFirst, 
						Iterator		inLast, 
						const String&	inTypeName);
	//! Determines if @a inExtension or @a inFileType match @a inDocumentType.
	static bool		DoesFileInfoMatchDocumentType(
						CFStringRef		inExtension, 
						OSType			inFileType, 
						const DocumentType&	inDocumentType);
	//! Determines if @a inMIMEType match @a inDocumentType.
	static bool		DoesMIMETypeMatchDocumentType(
						CFStringRef		inMIMEType, 
						const DocumentType&	inDocumentType);
	//! Converts a document types array into a vector for DocumentType structures.
	static void		MakeDocumentTypes(
						OSPtr<CFArrayRef>			inDocumentTypesArray, 
						std::vector<DocumentType>&	outDocumentTypesVector);
	//@}
	
	//! @name Resources & Strings
	//!@{
	//! Returns the Url of a file under the bundle's Resources directory with the given @a name and @a type in the given @a subDirectory.
	Url		Resource(
				const String&		name, 
				const String&		type = String(), 
				const String&		subDirectory = String()) const;
	//! Fills @a urls with the Urls of all files under the bundle's Resources directory with the given @a type in the given @a subDirectory.
	void	Resources(
				std::vector<Url>&	urls, 
				const String&		type, 
				const String&		subDirectory = String()) const;
	//! Returns the localised version of @a inKey.
	String	GetLocalisedString(
				CFStringRef			inKey, 
				CFStringRef			inValue, 
				CFStringRef			inTableName) const;
	//@}
	
	//! @name Conversions
	//@{
	//! Returns the object's underlying @c CFBundleRef.
	CFBundleRef	cf_ref() const		{ return (mRef); }
	//@}
	
	//! @name Comparisons
	//@{
	//! Tests two bundle objects for equality.
	bool	operator == (const Bundle& bundle) const;
	//! Tests two bundle objects for inequality.
	bool	operator != (const Bundle& bundle) const;
	//@}
	
private:
	
	static void		ConvertDocumentType(
						OSPtr<CFDictionaryRef>	inDocTypeDict, 
						DocumentType&			outDocType);
	static String	GetDocTypeEntry(
						OSPtr<CFDictionaryRef>	inDocTypeDict, 
						CFStringRef				inKey);
	static String	GetArrayEntry(
						OSPtr<CFArrayRef>		inArray, 
						CFIndex					inIndex);
	static Bundle&	GetMainBundle();
	
	// member variables
	CFBundleRef	mRef;
	OSType		mCreator;
	OSType		mType;
	mutable std::vector<DocumentType>	mDocumentTypes;
};


//	Inline member function definitions

// ------------------------------------------------------------------------------------------
/*!	The function looks first in InfoPlist.strings, then in Info.plist.  If 
	@a inKey is absent, the function returns a @c NULL OSPtr.
	
	Specifying a template type @a T that doesn't match the type of the key 
	results in undefined behaviour.
	
	@param T	Template parameter.  A CoreFoundation type (i.e., one that responds properly to @c CFRetain() & @c CFRelease().
*/
template <typename T> inline OSPtr<T>
Bundle::InfoKey(
	CFStringRef	inKey)	//!< The key to look for.
	const
{
	return (CFUMakeTypedValue<T>(CFBundleGetValueForInfoDictionaryKey(mRef, inKey)));
}

#if 0
// ------------------------------------------------------------------------------------------
/*!	If @a inKey is absent, the function returns a NULL OSPtr in @a value.
	
	Specifying a template type @a T that doesn't match the type of the key 
	results in undefined behaviour.
	
	@param T	Template parameter.  A CoreFoundation type (i.e., one that responds properly to @c CFRetain() & @c CFRelease().
*/
template <typename T> inline void
Bundle::InfoKey(
	CFStringRef	key, 	//!< The key to look for.
	OSPtr<T>&	value)	//!< Holds the output value.
	const
{
	value = CFUMakeTypedValue<T>(CFBundleGetValueForInfoDictionaryKey(mRef, key));
}
#endif

// ------------------------------------------------------------------------------------------
inline Bundle&
Bundle::operator = (const Bundle& bundle)
{
	return (Assign(bundle));
}

// ------------------------------------------------------------------------------------------
inline Bundle&
Bundle::operator = (const Url& url)
{
	return (Assign(url));
}

// ------------------------------------------------------------------------------------------
inline Bundle&
Bundle::operator = (const String& identifier)
{
	return (Assign(identifier));
}

// ------------------------------------------------------------------------------------------
inline Bundle&
Bundle::operator = (OSPtr<CFBundleRef> cfbundle)
{
	return (Assign(cfbundle));
}

// ------------------------------------------------------------------------------------------
inline bool
Bundle::operator == (const Bundle& bundle) const
{
	return (CFEqual(mRef, bundle.mRef));
}

// ------------------------------------------------------------------------------------------
inline bool
Bundle::operator != (const Bundle& bundle) const
{
	return (!CFEqual(mRef, bundle.mRef));
}

// ------------------------------------------------------------------------------------------
inline const Bundle&
Bundle::Main()
{
	return (GetMainBundle());
}


/*!	@defgroup	BLocalizedString	Localised Strings
*/
//@{

// ------------------------------------------------------------------------------------------
/*!	@brief			Retrieves a localised string residing in the main bundle's 
					<tt>Localizable.strings</tt> file.
	
	@param	key		A @c CFStringRef containing the name of the string to 
					retrieve.
	@param	comment	A description of the string's purpose.  Although this 
					description is not compiled into the application's code, 
					it is extracted by the @c genstrings tool and placed into 
					<tt>.strings</tt> files as an aid to translators.
	
	@return			A B::String containing the value of @a key, or @a key 
					itself if its value wasn't found.
*/
#define BLocalizedString(key, comment)	\
			B::Bundle::Main().GetLocalisedString((key), (key), NULL)

// ------------------------------------------------------------------------------------------
/*!	@brief			Retrieves a localised string residing in the file @a tbl 
					within the main bundle.
	
	@param	key		A @c CFStringRef containing the name of the string to 
					retrieve.
	@param	tbl		A @c CFStringRef containing the name of the 
					<tt>.strings</tt> file from which to read the string.
	@param	comment	A description of the string's purpose.  Although this 
					description is not compiled into the application's code, 
					it is extracted by the @c genstrings tool and placed into 
					<tt>.strings</tt> files as an aid to translators.
	
	@return			A B::String containing the value of @a key, or @a key 
					itself if its value wasn't found.
*/
#define BLocalizedStringFromTable(key, tbl, comment)	\
			B::Bundle::Main().GetLocalisedString((key), (key), (tbl))

// ------------------------------------------------------------------------------------------
/*! @brief			Retrieves a localised string residing in the file @a tbl 
					within @a bundle.
	
	@param	key		A @c CFStringRef containing the name of the string to 
					retrieve.
	@param	tbl		A @c CFStringRef containing the name of the 
					<tt>.strings</tt> file from which to read the string.
	@param	bundle	The B::Bundle in which to look for @a tbl.
	@param	comment	A description of the string's purpose.  Although this 
					description is not compiled into the application's code, 
					it is extracted by the @c genstrings tool and placed into 
					<tt>.strings</tt> files as an aid to translators.
	
	@return			A B::String containing the value of @a key, or @a key 
					itself if its value wasn't found.
*/
#define BLocalizedStringFromTableInBundle(key, tbl, bundle, comment)	\
			(bundle).GetLocalisedString((key), (key), (tbl))

// ------------------------------------------------------------------------------------------
/*! @brief			Retrieves a localised string residing in the file @a tbl 
					within @a bundle.
	
	@param	key		A @c CFStringRef containing the name of the string to 
					retrieve.
	@param	tbl		A @c CFStringRef containing the name of the 
					<tt>.strings</tt> file from which to read the string.
	@param	bundle	The B::Bundle in which to look for @a tbl.
	@param	value	A @c CFStringRef containing the default value for @a key.
	@param	comment	A description of the string's purpose.  Although this 
					description is not compiled into the application's code, 
					it is extracted by the @c genstrings tool and placed into 
					<tt>.strings</tt> files as an aid to translators.
	
	@return			A B::String containing the value of @a key, or @a value 
					if @a key's value wasn't found.
*/
#define BLocalizedStringWithDefaultValue(key, tbl, bundle, value, comment)	\
			(bundle).GetLocalisedString((key), (value), (tbl))

//@}


}	// namespace B


#endif	// BBundle_H_
