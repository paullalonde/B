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

/*! @file   BAutoUPP.h
    
    @brief  Universal Procedure Pointer wrappers.
*/

#ifndef BAutoUPP_H_
#define BAutoUPP_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BErrorHandler.h"
#include "BFwd.h"


namespace B {


// ==========================================================================================
//  AutoUPP

/*!
    @brief  Universal Procedure Pointer (UPP) wrapper.
    
    Objects of this class help to streamline the process of declaring and using UPPs.  For 
    example, if one needs a Carbon %Event timer UPP, one can declare it like this:
    
    @code
        SetupTimer()
        {
            static AutoEventLoopTimerUPP  upp(MyTimerProc);
            OSStatus  err;
            
            err = InstallEventLoopTimer(..., upp, ...);
        }
    @endcode
    
    Of course, the AutoUPP object doesn't have to be static.  It can be automatic or 
    even dynamically allocated, as long as it isn't destructed while the system API 
    to which it was given still makes use of it.
    
    @sa @link AutoUPPTypedefs Predefined template instantions @endlink.
     
*/
template <typename PROC, typename UPP, UPP (*NEW)(PROC), void (*DISPOSE)(UPP)>
class AutoUPP : public boost::noncopyable
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    PROC;               //!< Template parameter.  Should be a pointer-to-function.
    typedef typename    UPP;                //!< Template parameter.  Should be a system-defined Universal Procedure Pointer for functions having the same signature as @a PROC.
    typedef UPP         (*NEW)(PROC);       //!< Template parameter.  Should be a system-defined API that returns a Universal Procedure Pointer of type @a UPP.
    typedef void        (*DISPOSE)(UPP);    //!< Template parameter.  Should be a system-defined API that releases a Universal Procedure Pointer of type @a UPP.
    //@}
#endif
    
    //! @name Constructor / Destructor
    //@{
    //! Constructor.
    AutoUPP(PROC proc);
    //! Destructor.
    ~AutoUPP();
    //@}
    
    //! @name Conversions
    //@{
    //! Implicit conversion to @a UPP.
    operator UPP () const   { return (mUPP); }
    //@}
    
private:
    
    // member variables
    UPP mUPP;
};

// ------------------------------------------------------------------------------------------
/*! Allocates a @c UPP pointing to @a proc.
    
    @exception  std::bad_alloc  If memory for the Universal Procedure Pointer can't be 
                                allocated.
*/
template <typename PROC, typename UPP, UPP (*NEW)(PROC), void (*DISPOSE)(UPP)>
AutoUPP<PROC, UPP, NEW, DISPOSE>::AutoUPP(
    PROC    proc)           //!< The procedure pointer.
        : mUPP(NEW(proc))
{
    B_THROW_IF_NULL(mUPP);
}

// ------------------------------------------------------------------------------------------
/*! Releases the @a UPP.
    
    @exception  none
*/
template <typename PROC, typename UPP, UPP (*NEW)(PROC), void (*DISPOSE)(UPP)>
AutoUPP<PROC, UPP, NEW, DISPOSE>::~AutoUPP()
{
    DISPOSE(mUPP);
}


// ==========================================================================================
//  AutoDefUPP

/*!
    @brief  Pseudo Universal Procedure Pointer (UPP) wrapper.
    
    Used instead of AutoUPP UPPs are not really allocated or deallocated, but merely type 
    cast from their corresponding ProcPtr.
    
    @sa @link AutoUPPTypedefs Predefined template instantions @endlink.
     
*/
template <typename PROC, typename UPP>
class AutoDefUPP : public boost::noncopyable
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    PROC;               //!< Template parameter.  Should be a pointer-to-function.
    typedef typename    UPP;                //!< Template parameter.  Should be a system-defined Universal Procedure Pointer for functions having the same signature as @a PROC.
    //@}
#endif
    
    //! @name Constructor / Destructor
    //@{
    //! Constructor.
    AutoDefUPP(PROC proc);
    //@}
    
    //! @name Conversions
    //@{
    //! Implicit conversion to @a UPP.
    operator UPP () const   { return (mUPP); }
    //@}
    
private:
    
    // member variables
    UPP mUPP;
};

// ------------------------------------------------------------------------------------------
template <typename PROC, typename UPP>
AutoDefUPP<PROC, UPP>::AutoDefUPP(
    PROC    proc)           //!< The procedure pointer.
        : mUPP(reinterpret_cast<UPP>(proc))
{
    B_THROW_IF_NULL(mUPP);
}


// ==========================================================================================
//  Template instantiations of AutoUPP<PROC, UPP, NEW, DISPOSE>

/*! @defgroup   AutoUPPTypedefs AutoUPP Predefined Template Instantiations
*/
//@{

//  Apple %Events

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<AECoerceDescProcPtr, 
                AECoerceDescUPP, 
                NewAECoerceDescUPP, 
                DisposeAECoerceDescUPP>         AutoAECoerceDescUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<AECoercePtrProcPtr, 
                AECoercePtrUPP, 
                NewAECoercePtrUPP, 
                DisposeAECoercePtrUPP>          AutoAECoercePtrUPP;

//! Template Instantiation of AutoUPP for @c AEDisposeExternalUPP.
typedef AutoUPP<AEDisposeExternalProcPtr, 
                AEDisposeExternalUPP, 
                NewAEDisposeExternalUPP, 
                DisposeAEDisposeExternalUPP>    AutoAEDisposeExternalUPP;

//! Template Instantiation of AutoUPP for @c AEEventHandlerUPP.
typedef AutoUPP<AEEventHandlerProcPtr, 
                AEEventHandlerUPP, 
                NewAEEventHandlerUPP, 
                DisposeAEEventHandlerUPP>       AutoAEEventHandlerUPP;

//  Carbon %Events

//! Template Instantiation of AutoUPP for @c EventHandlerUPP.
typedef AutoUPP<EventHandlerProcPtr, 
                EventHandlerUPP, 
                NewEventHandlerUPP, 
                DisposeEventHandlerUPP>         AutoEventHandlerUPP;

//! Template Instantiation of AutoUPP for @c EventLoopTimerUPP.
typedef AutoUPP<EventLoopTimerProcPtr, 
                EventLoopTimerUPP, 
                NewEventLoopTimerUPP, 
                DisposeEventLoopTimerUPP>       AutoEventLoopTimerUPP;

//! Template Instantiation of AutoUPP for @c EventLoopIdleTimerUPP.
typedef AutoUPP<EventLoopIdleTimerProcPtr, 
                EventLoopIdleTimerUPP, 
                NewEventLoopIdleTimerUPP, 
                DisposeEventLoopIdleTimerUPP>   AutoEventLoopIdleTimerUPP;

//  Apple %Event Objects

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLAccessorProcPtr, 
                OSLAccessorUPP, 
                NewOSLAccessorUPP, 
                DisposeOSLAccessorUPP>          AutoOSLAccessorUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLCompareProcPtr, 
                OSLCompareUPP, 
                NewOSLCompareUPP, 
                DisposeOSLCompareUPP>           AutoOSLCompareUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLCountProcPtr, 
                OSLCountUPP, 
                NewOSLCountUPP, 
                DisposeOSLCountUPP>             AutoOSLCountUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLDisposeTokenProcPtr, 
                OSLDisposeTokenUPP, 
                NewOSLDisposeTokenUPP, 
                DisposeOSLDisposeTokenUPP>      AutoOSLDisposeTokenUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLGetMarkTokenProcPtr, 
                OSLGetMarkTokenUPP, 
                NewOSLGetMarkTokenUPP, 
                DisposeOSLGetMarkTokenUPP>      AutoOSLGetMarkTokenUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLGetErrDescProcPtr, 
                OSLGetErrDescUPP, 
                NewOSLGetErrDescUPP, 
                DisposeOSLGetErrDescUPP>        AutoOSLGetErrDescUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLMarkProcPtr, 
                OSLMarkUPP, 
                NewOSLMarkUPP, 
                DisposeOSLMarkUPP>              AutoOSLMarkUPP;

//! Template Instantiation of AutoUPP for @c AECoerceDescUPP.
typedef AutoUPP<OSLAdjustMarksProcPtr, 
                OSLAdjustMarksUPP, 
                NewOSLAdjustMarksUPP, 
                DisposeOSLAdjustMarksUPP>       AutoOSLAdjustMarksUPP;

//  Controls

//! Template Instantiation of AutoUPP for @c ControlActionUPP.
typedef AutoUPP<ControlActionProcPtr, 
                ControlActionUPP, 
                NewControlActionUPP, 
                DisposeControlActionUPP>                    AutoControlActionUPP;

//! Template Instantiation of AutoUPP for @c ControlKeyFilterUPP.
typedef AutoUPP<ControlKeyFilterProcPtr, 
                ControlKeyFilterUPP, 
                NewControlKeyFilterUPP, 
                DisposeControlKeyFilterUPP>                 AutoControlKeyFilterUPP;

//  Data Browser

//! Template Instantiation of AutoUPP for @c DataBrowserItemUPP.
typedef AutoUPP<DataBrowserItemProcPtr, 
                DataBrowserItemUPP, 
                NewDataBrowserItemUPP, 
                DisposeDataBrowserItemUPP>                  AutoDataBrowserItemUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemDataUPP.
typedef AutoUPP<DataBrowserItemDataProcPtr, 
                DataBrowserItemDataUPP, 
                NewDataBrowserItemDataUPP, 
                DisposeDataBrowserItemDataUPP>              AutoDataBrowserItemDataUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemCompareUPP.
typedef AutoUPP<DataBrowserItemCompareProcPtr, 
                DataBrowserItemCompareUPP, 
                NewDataBrowserItemCompareUPP, 
                DisposeDataBrowserItemCompareUPP>           AutoDataBrowserItemCompareUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemNotificationUPP.
typedef AutoUPP<DataBrowserItemNotificationProcPtr, 
                DataBrowserItemNotificationUPP, 
                NewDataBrowserItemNotificationUPP, 
                DisposeDataBrowserItemNotificationUPP>      AutoDataBrowserItemNotificationUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemNotificationWithItemUPP.
typedef AutoUPP<DataBrowserItemNotificationWithItemProcPtr, 
                DataBrowserItemNotificationWithItemUPP, 
                NewDataBrowserItemNotificationWithItemUPP, 
                DisposeDataBrowserItemNotificationWithItemUPP>  AutoDataBrowserItemNotificationWithItemUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserAddDragItemUPP.
typedef AutoUPP<DataBrowserAddDragItemProcPtr, 
                DataBrowserAddDragItemUPP, 
                NewDataBrowserAddDragItemUPP, 
                DisposeDataBrowserAddDragItemUPP>           AutoDataBrowserAddDragItemUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserAcceptDragUPP.
typedef AutoUPP<DataBrowserAcceptDragProcPtr, 
                DataBrowserAcceptDragUPP, 
                NewDataBrowserAcceptDragUPP, 
                DisposeDataBrowserAcceptDragUPP>            AutoDataBrowserAcceptDragUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserReceiveDragUPP.
typedef AutoUPP<DataBrowserReceiveDragProcPtr, 
                DataBrowserReceiveDragUPP, 
                NewDataBrowserReceiveDragUPP, 
                DisposeDataBrowserReceiveDragUPP>           AutoDataBrowserReceiveDragUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserPostProcessDragUPP.
typedef AutoUPP<DataBrowserPostProcessDragProcPtr, 
                DataBrowserPostProcessDragUPP, 
                NewDataBrowserPostProcessDragUPP, 
                DisposeDataBrowserPostProcessDragUPP>       AutoDataBrowserPostProcessDragUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemHelpContentUPP.
typedef AutoUPP<DataBrowserItemHelpContentProcPtr, 
                DataBrowserItemHelpContentUPP, 
                NewDataBrowserItemHelpContentUPP, 
                DisposeDataBrowserItemHelpContentUPP>       AutoDataBrowserItemHelpContentUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserGetContextualMenuUPP.
typedef AutoUPP<DataBrowserGetContextualMenuProcPtr, 
                DataBrowserGetContextualMenuUPP, 
                NewDataBrowserGetContextualMenuUPP, 
                DisposeDataBrowserGetContextualMenuUPP>     AutoDataBrowserGetContextualMenuUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserSelectContextualMenuUPP.
typedef AutoUPP<DataBrowserSelectContextualMenuProcPtr, 
                DataBrowserSelectContextualMenuUPP, 
                NewDataBrowserSelectContextualMenuUPP, 
                DisposeDataBrowserSelectContextualMenuUPP>  AutoDataBrowserSelectContextualMenuUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserDrawItemUPP.
typedef AutoUPP<DataBrowserDrawItemProcPtr, 
                DataBrowserDrawItemUPP, 
                NewDataBrowserDrawItemUPP, 
                DisposeDataBrowserDrawItemUPP>              AutoDataBrowserDrawItemUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserEditItemUPP.
typedef AutoUPP<DataBrowserEditItemProcPtr, 
                DataBrowserEditItemUPP, 
                NewDataBrowserEditItemUPP, 
                DisposeDataBrowserEditItemUPP>              AutoDataBrowserEditItemUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserHitTestUPP.
typedef AutoUPP<DataBrowserHitTestProcPtr, 
                DataBrowserHitTestUPP, 
                NewDataBrowserHitTestUPP, 
                DisposeDataBrowserHitTestUPP>               AutoDataBrowserHitTestUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserTrackingUPP.
typedef AutoUPP<DataBrowserTrackingProcPtr, 
                DataBrowserTrackingUPP, 
                NewDataBrowserTrackingUPP, 
                DisposeDataBrowserTrackingUPP>              AutoDataBrowserTrackingUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemDragRgnUPP.
typedef AutoUPP<DataBrowserItemDragRgnProcPtr, 
                DataBrowserItemDragRgnUPP, 
                NewDataBrowserItemDragRgnUPP, 
                DisposeDataBrowserItemDragRgnUPP>           AutoDataBrowserItemDragRgnUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemAcceptDragUPP.
typedef AutoUPP<DataBrowserItemAcceptDragProcPtr, 
                DataBrowserItemAcceptDragUPP, 
                NewDataBrowserItemAcceptDragUPP, 
                DisposeDataBrowserItemAcceptDragUPP>        AutoDataBrowserItemAcceptDragUPP;

//! Template Instantiation of AutoUPP for @c DataBrowserItemReceiveDragUPP.
typedef AutoUPP<DataBrowserItemReceiveDragProcPtr, 
                DataBrowserItemReceiveDragUPP, 
                NewDataBrowserItemReceiveDragUPP, 
                DisposeDataBrowserItemReceiveDragUPP>       AutoDataBrowserItemReceiveDragUPP;

//  Navigation Services

//! Template Instantiation of AutoUPP for @c NavEventUPP.
typedef AutoUPP<NavEventProcPtr, 
                NavEventUPP, 
                NewNavEventUPP, 
                DisposeNavEventUPP>         AutoNavEventUPP;

//! Template Instantiation of AutoUPP for @c NavPreviewUPP.
typedef AutoUPP<NavPreviewProcPtr, 
                NavPreviewUPP, 
                NewNavPreviewUPP, 
                DisposeNavPreviewUPP>       AutoNavPreviewUPP;

//! Template Instantiation of AutoUPP for @c NavObjectFilterUPP.
typedef AutoUPP<NavObjectFilterProcPtr, 
                NavObjectFilterUPP, 
                NewNavObjectFilterUPP, 
                DisposeNavObjectFilterUPP>  AutoNavObjectFilterUPP;

//  Apple Events

//! Template Instantiation of AutoUPP for @c AECoercionHandlerUPP.
typedef AutoUPP<AECoerceDescProcPtr, 
                AECoercionHandlerUPP, 
                NewAECoerceDescUPP, 
                DisposeAECoerceDescUPP>     AutoAECoerceDescUPP;

//! Template Instantiation of AutoUPP for @c AECoercePtrUPP.
typedef AutoUPP<AECoercePtrProcPtr, 
                AECoercePtrUPP, 
                NewAECoercePtrUPP, 
                DisposeAECoercePtrUPP>      AutoAECoercePtrUPP;

//! Template Instantiation of AutoUPP for @c AEEventHandlerUPP.
typedef AutoUPP<AEEventHandlerProcPtr, 
                AEEventHandlerUPP, 
                NewAEEventHandlerUPP, 
                DisposeAEEventHandlerUPP>   AutoAEEventHandlerUPP;

//  Appearance

//! Template Instantiation of AutoUPP for @c ThemeTabTitleDrawUPP.
typedef AutoUPP<ThemeTabTitleDrawProcPtr, 
                ThemeTabTitleDrawUPP, 
                NewThemeTabTitleDrawUPP, 
                DisposeThemeTabTitleDrawUPP>    AutoThemeTabTitleDrawUPP;

//! Template Instantiation of AutoUPP for @c ThemeEraseUPP.
typedef AutoUPP<ThemeEraseProcPtr, 
                ThemeEraseUPP, 
                NewThemeEraseUPP, 
                DisposeThemeEraseUPP>           AutoThemeEraseUPP;

//! Template Instantiation of AutoUPP for @c ThemeButtonDrawUPP.
typedef AutoUPP<ThemeButtonDrawProcPtr, 
                ThemeButtonDrawUPP, 
                NewThemeButtonDrawUPP, 
                DisposeThemeButtonDrawUPP>      AutoThemeButtonDrawUPP;

//! Template Instantiation of AutoUPP for @c WindowTitleDrawingUPP.
typedef AutoUPP<WindowTitleDrawingProcPtr, 
                WindowTitleDrawingUPP, 
                NewWindowTitleDrawingUPP, 
                DisposeWindowTitleDrawingUPP>   AutoWindowTitleDrawingUPP;

//  Dialogs

//! Template Instantiation of AutoUPP for @c ModalFilterUPP.
typedef AutoUPP<ModalFilterProcPtr, 
                ModalFilterUPP, 
                NewModalFilterUPP, 
                DisposeModalFilterUPP>          AutoModalFilterUPP;

//! Template Instantiation of AutoUPP for @c ModalFilterYDUPP.
typedef AutoUPP<ModalFilterYDProcPtr, 
                ModalFilterYDUPP, 
                NewModalFilterYDUPP, 
                DisposeModalFilterYDUPP>        AutoModalFilterYDUPP;

//! Template Instantiation of AutoUPP for @c UserItemUPP.
typedef AutoUPP<UserItemProcPtr, 
                UserItemUPP, 
                NewUserItemUPP, 
                DisposeUserItemUPP>             AutoUserItemUPP;

//  Quickdraw

//! Template Instantiation of AutoUPP for @c QDTextUPP.
typedef AutoUPP<QDTextProcPtr, 
                QDTextUPP, 
                NewQDTextUPP, 
                DisposeQDTextUPP>                   AutoQDTextUPP;

//! Template Instantiation of AutoUPP for @c QDLineUPP.
typedef AutoUPP<QDLineProcPtr, 
                QDLineUPP, 
                NewQDLineUPP, 
                DisposeQDLineUPP>                   AutoQDLineUPP;

//! Template Instantiation of AutoUPP for @c QDRectUPP.
typedef AutoUPP<QDRectProcPtr, 
                QDRectUPP, 
                NewQDRectUPP, 
                DisposeQDRectUPP>                   AutoQDRectUPP;

//! Template Instantiation of AutoUPP for @c QDRRectUPP.
typedef AutoUPP<QDRRectProcPtr, 
                QDRRectUPP, 
                NewQDRRectUPP, 
                DisposeQDRRectUPP>                  AutoQDRRectUPP;

//! Template Instantiation of AutoUPP for @c QDOvalUPP.
typedef AutoUPP<QDOvalProcPtr, 
                QDOvalUPP, 
                NewQDOvalUPP, 
                DisposeQDOvalUPP>                   AutoQDOvalUPP;

//! Template Instantiation of AutoUPP for @c QDArcUPP.
typedef AutoUPP<QDArcProcPtr, 
                QDArcUPP, 
                NewQDArcUPP, 
                DisposeQDArcUPP>                    AutoQDArcUPP;

//! Template Instantiation of AutoUPP for @c QDPolyUPP.
typedef AutoUPP<QDPolyProcPtr, 
                QDPolyUPP, 
                NewQDPolyUPP, 
                DisposeQDPolyUPP>                   AutoQDPolyUPP;

//! Template Instantiation of AutoUPP for @c QDRgnUPP.
typedef AutoUPP<QDRgnProcPtr, 
                QDRgnUPP, 
                NewQDRgnUPP, 
                DisposeQDRgnUPP>                    AutoQDRgnUPP;

//! Template Instantiation of AutoUPP for @c QDBitsUPP.
typedef AutoUPP<QDBitsProcPtr, 
                QDBitsUPP, 
                NewQDBitsUPP, 
                DisposeQDBitsUPP>                   AutoQDBitsUPP;

//! Template Instantiation of AutoUPP for @c QDCommentUPP.
typedef AutoUPP<QDCommentProcPtr, 
                QDCommentUPP, 
                NewQDCommentUPP, 
                DisposeQDCommentUPP>                AutoQDCommentUPP;

//! Template Instantiation of AutoUPP for @c QDTxMeasUPP.
typedef AutoUPP<QDTxMeasProcPtr, 
                QDTxMeasUPP, 
                NewQDTxMeasUPP, 
                DisposeQDTxMeasUPP>                 AutoQDTxMeasUPP;

//! Template Instantiation of AutoUPP for @c QDGetPicUPP.
typedef AutoUPP<QDGetPicProcPtr, 
                QDGetPicUPP, 
                NewQDGetPicUPP, 
                DisposeQDGetPicUPP>                 AutoQDGetPicUPP;

//! Template Instantiation of AutoUPP for @c QDPutPicUPP.
typedef AutoUPP<QDPutPicProcPtr, 
                QDPutPicUPP, 
                NewQDPutPicUPP, 
                DisposeQDPutPicUPP>                 AutoQDPutPicUPP;

//! Template Instantiation of AutoUPP for @c QDOpcodeUPP.
typedef AutoUPP<QDOpcodeProcPtr, 
                QDOpcodeUPP, 
                NewQDOpcodeUPP, 
                DisposeQDOpcodeUPP>                 AutoQDOpcodeUPP;

//! Template Instantiation of AutoUPP for @c QDStdGlyphsUPP.
typedef AutoUPP<QDStdGlyphsProcPtr, 
                QDStdGlyphsUPP, 
                NewQDStdGlyphsUPP, 
                DisposeQDStdGlyphsUPP>              AutoQDStdGlyphsUPP;

//! Template Instantiation of AutoUPP for @c QDJShieldCursorUPP.
typedef AutoUPP<QDJShieldCursorProcPtr, 
                QDJShieldCursorUPP, 
                NewQDJShieldCursorUPP, 
                DisposeQDJShieldCursorUPP>          AutoQDJShieldCursorUPP;

//! Template Instantiation of AutoUPP for @c DragGrayRgnUPP.
typedef AutoUPP<DragGrayRgnProcPtr, 
                DragGrayRgnUPP, 
                NewDragGrayRgnUPP, 
                DisposeDragGrayRgnUPP>              AutoDragGrayRgnUPP;

//! Template Instantiation of AutoUPP for @c ColorSearchUPP.
typedef AutoUPP<ColorSearchProcPtr, 
                ColorSearchUPP, 
                NewColorSearchUPP, 
                DisposeColorSearchUPP>              AutoColorSearchUPP;

//! Template Instantiation of AutoUPP for @c ColorComplementUPP.
typedef AutoUPP<ColorComplementProcPtr, 
                ColorComplementUPP, 
                NewColorComplementUPP, 
                DisposeColorComplementUPP>          AutoColorComplementUPP;

//! Template Instantiation of AutoUPP for @c DeviceLoopDrawingUPP.
typedef AutoUPP<DeviceLoopDrawingProcPtr, 
                DeviceLoopDrawingUPP, 
                NewDeviceLoopDrawingUPP, 
                DisposeDeviceLoopDrawingUPP>        AutoDeviceLoopDrawingUPP;

//! Template Instantiation of AutoUPP for @c RegionToRectsUPP.
typedef AutoUPP<RegionToRectsProcPtr, 
                RegionToRectsUPP, 
                NewRegionToRectsUPP, 
                DisposeRegionToRectsUPP>            AutoRegionToRectsUPP;

//  Display Manager

//! Template Instantiation of AutoUPP for @c DMNotificationUPP.
typedef AutoUPP<DMNotificationProcPtr, 
                DMNotificationUPP, 
                NewDMNotificationUPP, 
                DisposeDMNotificationUPP>               AutoDMNotificationUPP;

//! Template Instantiation of AutoUPP for @c DMExtendedNotificationUPP.
typedef AutoUPP<DMExtendedNotificationProcPtr, 
                DMExtendedNotificationUPP, 
                NewDMExtendedNotificationUPP, 
                DisposeDMExtendedNotificationUPP>       AutoDMExtendedNotificationUPP;

//! Template Instantiation of AutoUPP for @c DMComponentListIteratorUPP.
typedef AutoUPP<DMComponentListIteratorProcPtr, 
                DMComponentListIteratorUPP, 
                NewDMComponentListIteratorUPP, 
                DisposeDMComponentListIteratorUPP>      AutoDMComponentListIteratorUPP;

//! Template Instantiation of AutoUPP for @c DMDisplayModeListIteratorUPP.
typedef AutoUPP<DMDisplayModeListIteratorProcPtr, 
                DMDisplayModeListIteratorUPP, 
                NewDMDisplayModeListIteratorUPP, 
                DisposeDMDisplayModeListIteratorUPP>    AutoDMDisplayModeListIteratorUPP;

//! Template Instantiation of AutoUPP for @c DMProfileListIteratorUPP.
typedef AutoUPP<DMProfileListIteratorProcPtr, 
                DMProfileListIteratorUPP, 
                NewDMProfileListIteratorUPP, 
                DisposeDMProfileListIteratorUPP>        AutoDMProfileListIteratorUPP;

//! Template Instantiation of AutoUPP for @c DMDisplayListIteratorUPP.
typedef AutoUPP<DMDisplayListIteratorProcPtr, 
                DMDisplayListIteratorUPP, 
                NewDMDisplayListIteratorUPP, 
                DisposeDMDisplayListIteratorUPP>        AutoDMDisplayListIteratorUPP;

//  PMApplication

//! Template Instantiation of AutoUPP for @c PMItemUPP.
typedef AutoUPP<PMItemProcPtr, 
                PMItemUPP, 
                NewPMItemUPP, 
                DisposePMItemUPP>                   AutoPMItemUPP;

//! Template Instantiation of AutoUPP for @c PMPrintDialogInitUPP.
typedef AutoUPP<PMPrintDialogInitProcPtr, 
                PMPrintDialogInitUPP, 
                NewPMPrintDialogInitUPP, 
                DisposePMPrintDialogInitUPP>        AutoPMPrintDialogInitUPP;

//! Template Instantiation of AutoUPP for @c PMPageSetupDialogInitUPP.
typedef AutoUPP<PMPageSetupDialogInitProcPtr, 
                PMPageSetupDialogInitUPP, 
                NewPMPageSetupDialogInitUPP, 
                DisposePMPageSetupDialogInitUPP>    AutoPMPageSetupDialogInitUPP;

//! Template Instantiation of AutoUPP for @c PMSheetDoneUPP.
#ifdef NewPMSheetDoneUPP
    // Under 10.5, NewPMSheetDoneUPP is a macro.
    typedef AutoDefUPP<PMSheetDoneProcPtr, 
                    PMSheetDoneUPP>                 AutoPMSheetDoneUPP;
#else
    typedef AutoUPP<PMSheetDoneProcPtr, 
                    PMSheetDoneUPP, 
                    NewPMSheetDoneUPP, 
                    DisposePMSheetDoneUPP>          AutoPMSheetDoneUPP;
#endif

//@}

}   // namespace B

#endif  // BAutoUPP_H_
