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

/*! @file   BFwd.r
    @brief  Global macros, constants, and forward declarations for Rez.
*/

#ifndef BFWD_R_
#define BFWD_R_

// system headers
#include <Carbon/Carbon.r>


// ==========================================================================================

// These really should be in their own header

// See BFwd.h for explanation.

#ifndef B_BUILDING_ON_10_4_OR_LATER
#   if defined(kThemeWindowHasToolbarButton)
#       define B_BUILDING_ON_10_4_OR_LATER  1
#   else
#       define B_BUILDING_ON_10_4_OR_LATER  0
#   endif
#endif

#ifndef B_BUILDING_ON_10_3_OR_LATER
#   if B_BUILDING_ON_10_4_OR_LATER
#       define B_BUILDING_ON_10_3_OR_LATER  1
#   elif defined(kControlContentCGImageRef)
#       define B_BUILDING_ON_10_3_OR_LATER  1
#   else
#       define B_BUILDING_ON_10_3_OR_LATER  0
#   endif
#endif


// ==========================================================================================

#if !B_BUILDING_ON_10_3_OR_LATER

// Missing declarations from pre-Panther headers.

#define kEventAppIsEventInInstantMouser 104     // 10.2 and up, but undocumented prior to 10.3
#define kEventControlVisibilityChanged  157     // 10.2 and up, but undocumented prior to 10.3

// These are symbols relating to the Panther-only extensions to the Print Apple Event.
// The new keys won't be read in on Jaguar, but defining the values here is painless 
// and avoids having to conditionally compile the code that extracts them.
// 
// The following declarations are lifted from HIToolbox/HIView.h.  Refer to that file 
// for more information.

#define kPMPrintSettingsAEType          'pset'
#define kPMShowPrintDialogAEType        'pdlg'
#define kPMPrinterAEType                'trpr'
#define kPMCopiesAEProp                 "copies"
#define kPMCopiesAEKey                  'lwcp'
#define kPMCopieAEType                  typeSInt32
#define kPMCollateAEProp                "collating"
#define kPMCollateAEKey                 'lwcl'
#define kPMCollateAEType                typeBoolean
#define kPMFirstPageAEProp              "starting page"
#define kPMFirstPageAEKey               'lwfp'
#define kPMFirstPageAEType              typeSInt32
#define kPMLastPageAEProp               "ending page"
#define kPMLastPageAEKey                'lwlp'
#define kPMLastPageAEType               typeSInt32
#define kPMLayoutAcrossAEProp           "pages across"
#define kPMLayoutAcrossAEKey            'lwla'
#define kPMLayoutAcrossAEType           typeSInt32
#define kPMLayoutDownAEProp             "pages down"
#define kPMLayoutDownAEKey              'lwld'
#define kPMLayoutDownAEType             typeSInt32
#define kPMErrorHandlingAEProp          "error handling"
#define kPMErrorHandlingAEKey           'lweh'
#define kPMErrorHandlingAEType          typeEnumerated
#define kPMPrintTimeAEProp              "requested print time"
#define kPMPrintTimeAEKey               'lwqt'
#define kPMPrintTimeAEType              cLongDateTime
#define kPMFeatureAEProp                "printer features"
#define kPMFeatureAEKey                 'lwpf'
#define kPMFeatureAEType                typeAEList
#define kPMFaxNumberAEProp              "fax number"
#define kPMFaxNumberAEKey               'faxn'
#define kPMFaxNumberAEType              typeChar
#define kPMTargetPrinterAEProp          "target printer"
#define kPMTargetPrinterAEKey           'trpr'
#define kPMTargetPrinterAEType          typeChar
#define kPMErrorHandlingStandardEnum    'lwst'
#define kPMErrorHandlingDetailedEnum    'lwdt'

// Panther-only ThemeCursor IDs.

#define kThemeResizeUpCursor            19
#define kThemeResizeDownCursor          20
#define kThemeResizeUpDownCursor        21
#define kThemePoofCursor                22

// Panther-only ControlContentType constants.

#define kControlContentCGImageRef       134

#endif  // !B_BUILDING_ON_10_3_OR_LATER


#endif  // BFWD_R_
