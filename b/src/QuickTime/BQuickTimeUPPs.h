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

#ifndef BQuickTimeUPPs_H_
#define BQuickTimeUPPs_H_

#pragma once

// system headers
#include <QuickTime/QuickTime.h>

// B headers
#include "BAutoUPP.h"


namespace B {


//	QuickTime

//! Template Instantiation of AutoUPP for @c QTCallBackUPP.
typedef AutoUPP<QTCallBackProcPtr, 
				QTCallBackUPP, 
				NewQTCallBackUPP, 
				DisposeQTCallBackUPP>				AutoQTCallBackUPP;

//! Template Instantiation of AutoUPP for @c QTSyncTaskUPP.
typedef AutoUPP<QTSyncTaskProcPtr, 
				QTSyncTaskUPP, 
				NewQTSyncTaskUPP, 
				DisposeQTSyncTaskUPP>				AutoQTSyncTaskUPP;

//! Template Instantiation of AutoUPP for @c MovieRgnCoverUPP.
typedef AutoUPP<MovieRgnCoverProcPtr, 
				MovieRgnCoverUPP, 
				NewMovieRgnCoverUPP, 
				DisposeMovieRgnCoverUPP>			AutoMovieRgnCoverUPP;

//! Template Instantiation of AutoUPP for @c MovieProgressUPP.
typedef AutoUPP<MovieProgressProcPtr, 
				MovieProgressUPP, 
				NewMovieProgressUPP, 
				DisposeMovieProgressUPP>			AutoMovieProgressUPP;

//! Template Instantiation of AutoUPP for @c MovieDrawingCompleteUPP.
typedef AutoUPP<MovieDrawingCompleteProcPtr, 
				MovieDrawingCompleteUPP, 
				NewMovieDrawingCompleteUPP, 
				DisposeMovieDrawingCompleteUPP>		AutoMovieDrawingCompleteUPP;

//! Template Instantiation of AutoUPP for @c TrackTransferUPP.
typedef AutoUPP<TrackTransferProcPtr, 
				TrackTransferUPP, 
				NewTrackTransferUPP, 
				DisposeTrackTransferUPP>			AutoTrackTransferUPP;

//! Template Instantiation of AutoUPP for @c GetMovieUPP.
typedef AutoUPP<GetMovieProcPtr, 
				GetMovieUPP, 
				NewGetMovieUPP, 
				DisposeGetMovieUPP>					AutoGetMovieUPP;

//! Template Instantiation of AutoUPP for @c MoviePreviewCallOutUPP.
typedef AutoUPP<MoviePreviewCallOutProcPtr, 
				MoviePreviewCallOutUPP, 
				NewMoviePreviewCallOutUPP, 
				DisposeMoviePreviewCallOutUPP>		AutoMoviePreviewCallOutUPP;

//! Template Instantiation of AutoUPP for @c TextMediaUPP.
typedef AutoUPP<TextMediaProcPtr, 
				TextMediaUPP, 
				NewTextMediaUPP, 
				DisposeTextMediaUPP>				AutoTextMediaUPP;

//! Template Instantiation of AutoUPP for @c ActionsUPP.
typedef AutoUPP<ActionsProcPtr, 
				ActionsUPP, 
				NewActionsUPP, 
				DisposeActionsUPP>					AutoActionsUPP;

//! Template Instantiation of AutoUPP for @c DoMCActionUPP.
typedef AutoUPP<DoMCActionProcPtr, 
				DoMCActionUPP, 
				NewDoMCActionUPP, 
				DisposeDoMCActionUPP>				AutoDoMCActionUPP;

//! Template Instantiation of AutoUPP for @c MovieExecuteWiredActionsUPP.
typedef AutoUPP<MovieExecuteWiredActionsProcPtr, 
				MovieExecuteWiredActionsUPP, 
				NewMovieExecuteWiredActionsUPP, 
				DisposeMovieExecuteWiredActionsUPP>	AutoMovieExecuteWiredActionsUPP;

//! Template Instantiation of AutoUPP for @c MoviePrePrerollCompleteUPP.
typedef AutoUPP<MoviePrePrerollCompleteProcPtr, 
				MoviePrePrerollCompleteUPP, 
				NewMoviePrePrerollCompleteUPP, 
				DisposeMoviePrePrerollCompleteUPP>	AutoMoviePrePrerollCompleteUPP;

//! Template Instantiation of AutoUPP for @c QTNextTaskNeededSoonerCallbackUPP.
typedef AutoUPP<QTNextTaskNeededSoonerCallbackProcPtr, 
				QTNextTaskNeededSoonerCallbackUPP, 
				NewQTNextTaskNeededSoonerCallbackUPP, 
				DisposeQTNextTaskNeededSoonerCallbackUPP>	AutoQTNextTaskNeededSoonerCallbackUPP;

//! Template Instantiation of AutoUPP for @c MoviesErrorUPP.
typedef AutoUPP<MoviesErrorProcPtr, 
				MoviesErrorUPP, 
				NewMoviesErrorUPP, 
				DisposeMoviesErrorUPP>				AutoMoviesErrorUPP;

//! Template Instantiation of AutoUPP for @c TweenerDataUPP.
typedef AutoUPP<TweenerDataProcPtr, 
				TweenerDataUPP, 
				NewTweenerDataUPP, 
				DisposeTweenerDataUPP>				AutoTweenerDataUPP;

//! Template Instantiation of AutoUPP for @c QTEffectListFilterUPP.
typedef AutoUPP<QTEffectListFilterProcPtr, 
				QTEffectListFilterUPP, 
				NewQTEffectListFilterUPP, 
				DisposeQTEffectListFilterUPP>		AutoQTEffectListFilterUPP;

//! Template Instantiation of AutoUPP for @c MCActionFilterWithRefConUPP.
typedef AutoUPP<MCActionFilterWithRefConProcPtr, 
				MCActionFilterWithRefConUPP, 
				NewMCActionFilterWithRefConUPP, 
				DisposeMCActionFilterWithRefConUPP>	AutoMCActionFilterWithRefConUPP;

//! Template Instantiation of AutoUPP for @c MCActionNotificationUPP.
typedef AutoUPP<MCActionNotificationProcPtr, 
				MCActionNotificationUPP, 
				NewMCActionNotificationUPP, 
				DisposeMCActionNotificationUPP>		AutoMCActionNotificationUPP;

//! Template Instantiation of AutoUPP for @c QTMoviePropertyListenerUPP.
typedef AutoUPP<QTMoviePropertyListenerProcPtr, 
				QTMoviePropertyListenerUPP, 
				NewQTMoviePropertyListenerUPP, 
				DisposeQTMoviePropertyListenerUPP>	AutoQTMoviePropertyListenerUPP;

//! Template Instantiation of AutoUPP for @c QTTrackPropertyListenerUPP.
typedef AutoUPP<QTTrackPropertyListenerProcPtr, 
				QTTrackPropertyListenerUPP, 
				NewQTTrackPropertyListenerUPP, 
				DisposeQTTrackPropertyListenerUPP>	AutoQTTrackPropertyListenerUPP;

//! Template Instantiation of AutoUPP for @c QTBandwidthNotificationUPP.
typedef AutoUPP<QTBandwidthNotificationProcPtr, 
				QTBandwidthNotificationUPP, 
				NewQTBandwidthNotificationUPP, 
				DisposeQTBandwidthNotificationUPP>	AutoQTBandwidthNotificationUPP;

}	// namespace B


#endif	// BQuickTimeUPPs_H_
