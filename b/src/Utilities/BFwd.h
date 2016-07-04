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

/*! @file   BFwd.h
    @brief  Global macros, constants, and forward declarations.
*/

#ifndef BFWD_H_
#define BFWD_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// compiler headers
#ifdef __MWERKS__
#   include <stringfwd>
#endif


// ==========================================================================================

// These really should be in their own header

/*! @def B_VAR_EXPORT
    
    Controls exporting of global variables.  This is used when building with CodeWarrior 
    to work around its aggressive dead-code stripping, and thus allow auto-registration 
    of various classes.
*/
#if defined(__MWERKS__)
#   define B_VAR_EXPORT __declspec(export)
#else
#   define B_VAR_EXPORT
#endif

/*! @def B_BUILDING_ON_10_4_OR_LATER
    
    This macro determines if any Tiger-specific declarations are accessed in the B
    source.
    
    There appears to be no officially sanctioned way of telling what version of the OS 
    (OS SDK actually) we are building with.  Fortunately we can rely on a hack:  we test 
    for the definition of a symbol which only exists on Panther.  If it exists, we're 
    building on Panther or later;  else, we're building on Jaguar.
    
    Currently, we test for string-valued symbol from ControlDefinitions.h.  This symbol is 
    unlikely to be replaced by an enum in the future...
    
    Set the macro to a specific value in your prefix file (or other handy location) if 
    you need to override this test.
*/
#ifndef B_BUILDING_ON_10_4_OR_LATER
#   if defined(kHIUserPaneClassID)
#       define B_BUILDING_ON_10_4_OR_LATER  1
#   else
#       define B_BUILDING_ON_10_4_OR_LATER  0
#   endif
#elif B_BUILDING_ON_10_4_OR_LATER && !defined(MAC_OS_X_VERSION_10_4)
#   error "MAC_OS_X_VERSION_10_4 needs to be defined if B_BUILDING_ON_10_4_OR_LATER is non-zero !!"
#endif

/*! @def B_BUILDING_ON_10_3_OR_LATER
    
    This macro determines if any Panther-specific declarations are accessed in the B
    source.
    
    There appears to be no officially sanctioned way of telling what version of the OS 
    (OS SDK actually) we are building with.  Fortunately we can rely on a hack:  we test 
    for the definition of a symbol which only exists on Panther.  If it exists, we're 
    building on Panther or later;  else, we're building on Jaguar.
    
    Currently, we test for string-valued symbol from ControlDefinitions.h.  This symbol is 
    unlikely to be replaced by an enum in the future...
    
    Set the macro to a specific value in your prefix file (or other handy location) if 
    you need to override this test.
*/
#ifndef B_BUILDING_ON_10_3_OR_LATER
#   if B_BUILDING_ON_10_4_OR_LATER
#       define B_BUILDING_ON_10_3_OR_LATER  1
#   elif defined(kHIScrollBarClassID)
#       define B_BUILDING_ON_10_3_OR_LATER  1
#   else
#       define B_BUILDING_ON_10_3_OR_LATER  0
#   endif
#elif B_BUILDING_ON_10_3_OR_LATER && !defined(MAC_OS_X_VERSION_10_3)
#   error "MAC_OS_X_VERSION_10_3 needs to be defined if B_BUILDING_ON_10_3_OR_LATER is non-zero !!"
#endif

// Perform consistency checks.
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_MAX_ALLOWED
#   error "Inconsistent definitions of MAC_OS_X_VERSION_MIN_REQUIRED and MAC_OS_X_VERSION_MAX_ALLOWED"
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_2
#   error "B requires Jaguar at minimum"
#endif
#if !B_BUILDING_ON_10_3_OR_LATER
#   if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_2
#       error "Inconsistent definitions of B_BUILDING_ON_10_3_OR_LATER and MAC_OS_X_VERSION_MIN_REQUIRED"
#   elif MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
#       error "Inconsistent definitions of B_BUILDING_ON_10_3_OR_LATER and MAC_OS_X_VERSION_MAX_ALLOWED"
#   endif
#endif

/*! @def B_BUILDING_CAN_USE_10_3_APIS
    
    This macro is non-zero if you can call Panther-specific APIs.  Note that this 
    symbol may be true even if the application can run on Jaguar, so you may need 
    to perform runtime checks.
*/

/*! @def B_BUILDING_CAN_USE_10_3_APIS_ONLY
    
    This macro is non-zero if you can call Panther-specific APIs.  This symbol is only 
    defined when the application can only run on Panther and later.
*/

#if B_BUILDING_ON_10_3_OR_LATER
#   if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
#       define B_BUILDING_CAN_USE_10_3_APIS         1
#   else
#       define B_BUILDING_CAN_USE_10_3_APIS         0
#   endif
#   if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
#       define B_BUILDING_CAN_USE_10_3_APIS_ONLY    1
#   else
#       define B_BUILDING_CAN_USE_10_3_APIS_ONLY    0
#   endif
#else
#   define B_BUILDING_CAN_USE_10_3_APIS             0
#   define B_BUILDING_CAN_USE_10_3_APIS_ONLY        0
#endif


// ==========================================================================================

namespace B {

    class   AEObject;
    class   CommandData;
    
    struct from_copy_t;
    template <typename T>       class OSPtr;
    
    template <typename T>       class Array;
    template <typename T>       class MutableArray;
    
    template <class DOCUMENT>   class DocumentWindow;
    
    template <OSType TYPE>      struct TaggedTypeTrait;
    
    template <EventParamType TYPE>  struct EventParamTypeTrait;
    template <EventParamName NAME>  struct EventParamNameTrait;
    template <EventParamName NAME>  struct EventParam;
    template <EventParamType TYPE>  struct EventDirectParam;
    
    template <typename T>                               struct ViewDataTrait;
    template <OSType SIG, OSType KIND, ResType TAG>     struct ViewDataDef;
    template <OSType SIG, OSType KIND, ResType TAG>     struct ViewData;
    
    template <CollectionTag TAG>                        struct StandardViewCollectionItem;
    template <OSType SIG, ResType TAG, typename T>      struct CollectionItem;
    template <OSType SIG, ResType TAG, typename T>      struct MenuCommandProperty;
    template <OSType SIG, ResType TAG, typename T>      struct MenuItemProperty;
    template <OSType SIG, ResType TAG, typename T>      struct ViewProperty;
    template <OSType SIG, ResType TAG, typename T>      struct WindowProperty;
    
    template <UInt32 CLASS, UInt32 KIND>                class Event;
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID> class AEEvent;
    
    template <typename charT, class traits = std::char_traits<charT> > class    BasicPasteboardStreambuf;
    template <typename charT, class traits = std::char_traits<charT> > class    BasicPasteboardInputStream;
    template <typename charT, class traits = std::char_traits<charT> > class    BasicPasteboardOutputStream;
    
    typedef BasicPasteboardInputStream<char>    PasteboardInputStream;
    typedef BasicPasteboardOutputStream<char>   PasteboardOutputStream;

    typedef boost::shared_ptr<AEObject>         AEObjectPtr;
    typedef boost::shared_ptr<AEObject const>   ConstAEObjectPtr;
    
}   // namespace B;


#endif  // BFWD_H_
