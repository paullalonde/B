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

#include <algorithm>
#include <iostream>
#include <map>
#include <stdarg.h>
#include <sstream>
#include <vector>

#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFString.h>

#include "main.h"


struct CopyAscii
{
			CopyAscii(
				StringsMap&	englishKeys, 
				bool&		englishChanged);
	void	operator () (const StringsMap::value_type& val);
			
private:
	
	StringsMap&	mEnglishKeys;
	bool&		mEnglishChanged;
};

struct CopyChangedAscii
{
			CopyChangedAscii(
				StringsMap&	oldAsciiKeys, 
				StringsMap&	englishKeys, 
				bool&		englishChanged);
	void	operator () (const StringsMap::value_type& val);
			
private:
	
	StringsMap&	mOldAsciiKeys;
	StringsMap&	mEnglishKeys;
	bool&		mEnglishChanged;
};

struct RemoveDeletedAscii
{
			RemoveDeletedAscii(
				StringsMap&	englishKeys, 
				bool&		englishChanged);
	void	operator () (const std::string& val);
			
private:
	
	StringsMap&	mEnglishKeys;
	bool&		mEnglishChanged;
};

struct IdentifyDeletedKeys
{
			IdentifyDeletedKeys(
				const StringsMap&			asciiKeys, 
				std::vector<std::string>&	deletedKeys);
	void	operator () (const StringsMap::value_type& val) const;
			
private:
	
	const StringsMap&			mAsciiKeys;
	std::vector<std::string>&	mDeletedKeys;
};

struct RemoveDeletedNewAscii
{
			RemoveDeletedNewAscii(
				StringsMap&	newAsciiKeys, 
				StringsMap&	englishKeys, 
				bool&		englishChanged);
	void	operator () (const StringsMap::value_type& val);
			
private:
	
	StringsMap&	mNewAsciiKeys;
	StringsMap&	mEnglishKeys;
	bool&		mEnglishChanged;
};

static void	read_strings_file(const char* path, StringsMap& stringsMap);
static void	write_strings_file(const char* path, const StringsMap& stringsMap);
void		print_map(std::ostream& ostr, const StringsMap& stringsMap);
static void	status(const char* format, ...);
static void	usage();

bool		verbose		= true;
const char*	progname	= "";


int main(int argc, const char** argv)
{
	if (strrchr(argv[0], '/') != NULL)
		progname = strrchr(argv[0], '/') + 1;
	else
		progname = argv[0];
	
	if ((argc != 3) && (argc != 4))
		usage();
	
	StringsMap	englishMap;
	bool		englishChanged	= false;
	
	read_strings_file(argv[argc-1], englishMap);
	
	if (argc == 3)
	{
		// 2-way merge
		
		StringsMap	asciiMap;
		
		read_strings_file(argv[1], asciiMap);
		
		// Remove entries from englishMap that are not in asciiMap.
		
		std::vector<std::string>	deletedKeys;
		
		std::for_each(englishMap.begin(), englishMap.end(), 
					  IdentifyDeletedKeys(asciiMap, deletedKeys));
		std::for_each(deletedKeys.begin(), deletedKeys.end(), 
					  RemoveDeletedAscii(englishMap, englishChanged));
		
		// Add entries that are present in asciiMap but not in the englishMap.
		
		std::for_each(asciiMap.begin(), asciiMap.end(), 
					CopyAscii(englishMap, englishChanged));
	}
	else
	{
		// 3-way merge
		
		StringsMap	oldAsciiMap, newAsciiMap;
		
		read_strings_file(argv[1], oldAsciiMap);
		read_strings_file(argv[2], newAsciiMap);
		
		// Remove entries from englishMap that are in oldAsciiMap but not in newAsciiMap.
		
		std::for_each(oldAsciiMap.begin(), oldAsciiMap.end(), 
					RemoveDeletedNewAscii(newAsciiMap, englishMap, englishChanged));
		
		// Copy entries whose value has changed between oldAsciiMap and newAsciiMap to englishMap.
		
		std::for_each(newAsciiMap.begin(), newAsciiMap.end(), 
					CopyChangedAscii(oldAsciiMap, englishMap, englishChanged));
	}
	
	if (englishChanged)
	{
		write_strings_file(argv[argc-1], englishMap);
	}
	
	return 0;
}


static void	read_strings_file(const char* path, StringsMap& stringsMap)
{
	InitParser(path, stringsMap);
	
	if (yyparse() != 0)
		fatal(4, "Can't parse %s", path);
}

static void	write_strings_file(const char* path, const StringsMap& stringsMap)
{
	std::ostringstream	ostr;
	
	print_map(ostr, stringsMap);
	
	std::string	outstr	= ostr.str();
	CFStringRef	dataStr = CFStringCreateWithCString(NULL, outstr.c_str(), kCFStringEncodingUTF8);
	CFDataRef	dataRef	= CFStringCreateExternalRepresentation(NULL, dataStr, kCFStringEncodingUnicode, 0);
	
	FILE*	fp	= fopen(path, "wb");
	
	if (fp == NULL)
		fatal(2, "Can't create %s", path);
	
	fwrite(CFDataGetBytePtr(dataRef), 1, CFDataGetLength(dataRef), fp);
	fclose(fp);
	
	CFRelease(dataRef);
	CFRelease(dataStr);
}


void	print_map(std::ostream& ostr, const StringsMap& stringsMap)
{
	for (StringsMap::const_iterator it = stringsMap.begin(); 
		 it != stringsMap.end(); 
		 ++it)
	{
		ostr << it->second.comment << "\n"
			 << it->first << " = " << it->second.value << ";\n" << std::endl;
	}
}


void	fatal(int status, const char* format, ...)
{
	va_list	args;
	
	va_start(args, format);
	fprintf(stderr, "%s:  ", progname);
	vfprintf(stderr, format, args);
	fputc('\n', stderr);
	va_end(args);

	exit(status);
}

void	yyerror(const char* msg)
{
	fatal(4, "Parse error: %s", msg);
}

static void	status(const char* format, ...)
{
	if (verbose)
	{
		va_list	args;
		
		va_start(args, format);
		fprintf(stdout, "%s:  ", progname);
		vfprintf(stdout, format, args);
		fputc('\n', stdout);
		va_end(args);
	}
}

static void	usage()
{
	fprintf(stderr, "usage:  %s [old-ascii-file] new-ascii-file old-english-file\n", progname);
	exit(1);
}


CopyAscii::CopyAscii(
	StringsMap&	englishKeys, 
	bool&		englishChanged)
		: mEnglishKeys(englishKeys), mEnglishChanged(englishChanged)
{
}
	
void
CopyAscii::operator () (const StringsMap::value_type& val)
{
	StringsMap::iterator	eit	= mEnglishKeys.find(val.first);
	
	if (eit != mEnglishKeys.end())
	{
		// This is an existing entry.
		
		// Clobber the english comment if it's different from the new ascii one.
		
		if (val.second.comment != eit->second.comment)
		{
			status("changing comment for %s", val.first.c_str());
			
			eit->second.comment = val.second.comment;
			mEnglishChanged = true;
		}
		
		// Note that we preserve the english map's value.
	}
	else
	{
		// This is a new entry.  Add it to the english map.
		
		status("adding entry for %s", val.first.c_str());
		
		mEnglishKeys.insert(val);
		mEnglishChanged = true;
	}
}


CopyChangedAscii::CopyChangedAscii(
	StringsMap&	oldAsciiKeys, 
	StringsMap&	englishKeys, 
	bool&		englishChanged)
		: mOldAsciiKeys(oldAsciiKeys), mEnglishKeys(englishKeys), mEnglishChanged(englishChanged)
{
}
	
void
CopyChangedAscii::operator () (const StringsMap::value_type& val)
{
	StringsMap::iterator	oit	= mOldAsciiKeys.find(val.first);
	
	if (oit != mOldAsciiKeys.end())
	{
		// This is an existing entry.
		
		// Clobber the english comment if it's different from the new ascii one.
		
		StringsMap::iterator	eit	= mEnglishKeys.find(val.first);
		
		if ((eit != mEnglishKeys.end()) && (val.second.comment != eit->second.comment))
		{
			status("changing comment for %s", val.first.c_str());
			
			eit->second.comment = val.second.comment;
			mEnglishChanged = true;
		}
		
		if (val.second.value != oit->second.value)
		{
			// The entry's value has changed between the new & old ascii 
			// versions.  Clobber the english entry.
			
			if (eit != mEnglishKeys.end())
			{
				// Replace the english value with the new ascii one.
				
				status("replacing value for %s", val.first.c_str());
				
				eit->second.value = val.second.value;
			}
			else
			{
				// The entry was absent from the english version (??)
				
				status("adding missing value for %s", val.first.c_str());
				
				mEnglishKeys.insert(val);
			}
			
			mEnglishChanged = true;
		}
	}
	else
	{
		// This is a new entry.  Add it to map 3.
		
		status("adding entry for %s", val.first.c_str());
		
		mEnglishKeys.insert(val);
		mEnglishChanged = true;
	}
}


IdentifyDeletedKeys::IdentifyDeletedKeys(
	const StringsMap&			asciiKeys, 
	std::vector<std::string>&	deletedKeys)
		: mAsciiKeys(asciiKeys), mDeletedKeys(deletedKeys)
{
}
	
void
IdentifyDeletedKeys::operator () (const StringsMap::value_type& val) const
{
	StringsMap::const_iterator	ait	= mAsciiKeys.find(val.first);
	
	if (ait == mAsciiKeys.end())
	{
		// This is a deleted entry.
		
		status("removing entry for %s", val.first.c_str());
		
		mDeletedKeys.push_back(val.first);
	}
}


RemoveDeletedAscii::RemoveDeletedAscii(
	StringsMap&	englishKeys, 
	bool&		englishChanged)
		: mEnglishKeys(englishKeys), mEnglishChanged(englishChanged)
{
}
	
void
RemoveDeletedAscii::operator () (const std::string& val)
{
	mEnglishKeys.erase(val);
}


RemoveDeletedNewAscii::RemoveDeletedNewAscii(
	StringsMap&	newAsciiKeys, 
	StringsMap&	englishKeys, 
	bool&		englishChanged)
		: mNewAsciiKeys(newAsciiKeys), mEnglishKeys(englishKeys), mEnglishChanged(englishChanged)
{
}
	
void
RemoveDeletedNewAscii::operator () (const StringsMap::value_type& val)
{
	StringsMap::iterator	nit	= mNewAsciiKeys.find(val.first);
	
	if (nit == mNewAsciiKeys.end())
	{
		// This is a deleted entry.
		
		StringsMap::iterator	eit	= mEnglishKeys.find(val.first);
		
		if (eit != mEnglishKeys.end())
		{
			status("removing entry for %s", val.first.c_str());
			
			mEnglishKeys.erase(val.first);
			mEnglishChanged = true;
		}
	}
}
