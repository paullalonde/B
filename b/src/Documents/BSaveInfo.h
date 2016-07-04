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

#ifndef BSaveInfo_H_
#define BSaveInfo_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

// forward class declarations
class	String;
class	Url;
class	Window;


class SaveInfo
{
public:

	//! @name Constants
	//@{
	//! The document's URL. May be absent. The value is a CFURLRef.
	static const CFStringRef	kUrlKey;
	//! The document's content type.  The value is a CFStringRef.
	static const CFStringRef	kContentTypeKey;
	//! A flag indicating whether the save was initiated by the user.  The value is a CFBooleanRef.
	static const CFStringRef	kFromUserKey;
	//! A flag indicating whether the save is part of a document close.  The value is a CFBooleanRef.
	static const CFStringRef	kClosingKey;
	//! The document's containing folder's FSRef.  The value is a CFDataRef.
	static const CFStringRef	kFolderRefKey;
	//! The document's file name.  The value is a CFStringRef.
	static const CFStringRef	kFileNameKey;
	//! A flag indicating whether file should have its extension hidden.  The value is a CFBooleanRef.
	static const CFStringRef	kExtensionHiddenKey;
	//! A flag indicating whether file attributes should be written out.  The value is a CFBooleanRef.
	static const CFStringRef	kWriteAttributesKey;
	//! The document's FSRef.  The value is a CFDataRef.
	static const CFStringRef	kFileRefKey;
	//! A flag indicating whether any existing file should be replaced.  The value is a CFBooleanRef.
	static const CFStringRef	kReplacingKey;
	//@}
	
	SaveInfo();
	SaveInfo(const SaveInfo& src);
	
	SaveInfo&	operator = (const SaveInfo& src);
	void		swap(SaveInfo& src);
	
	//! @name Inquiries
	//@{
	//! The window that invoked the save. If not NULL, dialogs should be displayed as sheets attached to this window.
	Window*	GetInvokingWindow() const	{ return (mInvokingWindow); }
	Url		GetUrl() const;
	String	GetContentType() const;
	bool	GetFromUser() const;
	bool	GetClosing() const;
	bool	GetFolderRef(FSRef& outFolderRef) const;
	FSRef	GetFolderRef() const;
	String	GetFileName() const;
	bool	GetExtensionHidden() const;
	bool	GetWriteAttributes() const;
	bool	GetFileRef(FSRef& outFileRef) const;
	FSRef	GetFileRef() const;
	bool	GetReplacing() const;
	//@}
	
	//! @name Modifiers
	//@{
	void	SetFileInfo(
				const Url&		inUrl,
				const String&	inContentType);
	void	SetInvokingWindow(Window* inWindow)	{ mInvokingWindow = inWindow; }
	void	SetUrl(const Url& inUrl);
	void	SetContentType(const String& inContentType);
	void	SetFromUser(bool inFromUser);
	void	SetClosing(bool inClosing);
	void	SetFolderRef(const FSRef& inFolderRef);
	void	SetFileName(const String& inFileName);
	void	SetExtensionHidden(bool inExtensionHidden);
	void	SetWriteAttributes(bool inWriteAttributes);
	void	SetFileRef(const FSRef& inFileRef);
	void	SetReplacing(bool inReplacing);
	//@}
	
	//! @name Generic getters & setters
	//@{
	String	GetString(CFStringRef key) const;
	void	SetString(CFStringRef key, const String& str);
	Url		GetUrl(CFStringRef key) const;
	void	SetUrl(CFStringRef key, const Url& url);
	bool	GetBool(CFStringRef key) const;
	bool	GetBool(CFStringRef key, bool dflt) const;
	void	SetBool(CFStringRef key, bool value);
	bool	GetData(CFStringRef key, void* ptr, size_t size) const;
	void	SetData(CFStringRef key, const void* ptr, size_t size);
	FSRef	GetFSRef(CFStringRef key) const;
	void	SetFSRef(CFStringRef key, const FSRef& ref);
	//@}
	
private:
	
	OSPtr<CFMutableDictionaryRef>	mDictionary;
	Window*							mInvokingWindow;
};

}	// namespace B


#endif	// BSaveInfo_H_
