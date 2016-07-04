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

#pragma once

#include <list>
#include <map>
#include <string>

struct StringsEntry
{
    std::string value;
    std::string comment;
};

typedef std::map<std::string, StringsEntry> StringsMap;

extern void InitParser(const char* path, StringsMap& stringsMap);
extern void InitScanner(const char* path);

extern void fatal(int status, const char* format, ...);

extern int  yyparse(void);
extern void yyerror(const char* msg);
