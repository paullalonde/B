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

#ifndef BHelpUtilities_H_
#define BHelpUtilities_H_

#pragma once

// library headers
#include <boost/utility.hpp>

// B headers
#include "BString.h"
#include "BUrl.h"


namespace B {

// forward declarations
class   Bundle;
class   Url;


/*!
    @brief  Helper class that interfaces to Carbon Help Manager.
    
    This class contains a number of utility functions for registering help books, and 
    for looking up or searching help content.
*/
class HelpUtilities : public boost::noncopyable
{
public:

    //! @name Inquiries
    //@{
    //! Returns @c true if the main bundle contains a help book.
    static bool HasHelpBook();
    //! Returns @c true if @a inBundle contains a help book.
    static bool HasHelpBook(
                    const Bundle&   inBundle);
    //@}
    
    //! @name Help Book Registration
    //@{
    //! Registers the help book in the main bundle.
    static void RegisterHelpBook();
    //! Registers the help book in @a inBundle.
    static void RegisterHelpBook(
                    const Bundle&   inBundle);
    //@}
    
    //! @name Help Display
    //@{
    //! Displays the page in the main bundle's help book that is identified by @a inUrl.
    static void GotoPage(
                    const Url&      inUrl);
    //! Displays the page in @a inBundle's help book that is identified by @a inUrl.
    static void GotoPage(
                    const Bundle&   inBundle, 
                    const Url&      inUrl);
    //! Displays the page in help book @a inBookName that is identified by @a inUrl.
    static void GotoPage(
                    const String&   inBookName, 
                    const Url&      inUrl);
    //! Displays the page in help book @a inBookName that is identified by @a inPath and @a inAnchor.
    static void GotoPage(
                    const String&   inBookName, 
                    const String&   inPath, 
                    const String&   inAnchor);
    //@}
    
    //! @name Table of Contents
    //@{
    static void GotoTOC(
                    AHTOCType       inTOCType = kAHTOCTypeUser);
    //@}
    
    //! @name Searching for Content
    //@{
    //! Searches for occurrences of @a inSearchText within the main bundle's help book.
    static void Search(
                    const String&   inSearchText);
    //! Searches for occurrences of @a inSearchText within @a inBundle's help book.
    static void Search(
                    const Bundle&   inBundle, 
                    const String&   inSearchText);
    //! Searches for occurrences of @a inSearchText within the help book @a inBookName.
    static void Search(
                    const String&   inBookName, 
                    const String&   inSearchText);
    //! Searches for occurrences of @a inSearchText within all registered help books.
    static void SearchAll(
                    const String&   inSearchText);
    //@}
    
    //! @name Searching for Anchors
    //@{
    //! Searches for anchors matching @a inAnchor within the main bundle's help book.
    static void Lookup(
                    const String&   inAnchor);
    //! Searches for anchors matching @a inAnchor within @a inBundle's help book.
    static void Lookup(
                    const Bundle&   inBundle, 
                    const String&   inAnchor);
    //! Searches for anchors matching @a inAnchor within the help book @a inBookName.
    static void Lookup(
                    const String&   inBookName, 
                    const String&   inAnchor);
    //! Searches for anchors matching @a inAnchor within all registered help books.
    static void LookupAll(
                    const String&   inAnchor);
    //@}
    
private:
    
    // illegal operations
    HelpUtilities();
};


}   // namespace B

#endif  // BHelpUtilities_H_
