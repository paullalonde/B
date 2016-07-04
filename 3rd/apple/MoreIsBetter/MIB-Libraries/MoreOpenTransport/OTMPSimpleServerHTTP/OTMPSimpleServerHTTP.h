/*
	File:		OTMPSimpleServerHTTP.h

	Contains:	Interface to the simple HTTP server sample (using MP tasks).

	Written by:	Quinn "The Eskimo!"

	Copyright:	Copyright (c) 2000-2001 by Apple Computer, Inc., All Rights Reserved.

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Change History (most recent first):

$Log: OTMPSimpleServerHTTP.h,v $
Revision 1.4  2001/11/07 15:57:06  eskimo1
Tidy up headers, add CVS logs, update copyright.


         <3>     21/9/01    Quinn   Project Builder needs framework includes.
         <2>      9/7/01    Quinn   Added MPLog debugging constant for debugging the server
                                    listen/accept loop.
         <1>     7/11/00    Quinn   First checked in.
*/

#pragma once

/////////////////////////////////////////////////////////////////

#include "MoreSetup.h"

#if ! MORE_FRAMEWORK_INCLUDES
	#include <MacTypes.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
	kOTMPSimpleServerHTTPListenLoopLogID = 16
};

extern pascal OSStatus RunHTTPServerMP(InetHost ipAddr, short vRefNum, long dirID);
	// See comments in implementation.

extern pascal OSStatus InitOTMPSimpleServerHTTP(void);
	// Initialise this module.  Call this before calling 
	// RunHTTPServer.
	
extern pascal void     TermOTMPSimpleServerHTTP(void);
	// Shut down this module.  Call this after a successful 
	// call to InitOTMPSimpleServerHTTP, regardless of whether 
	// you have called RunHTTPServer.  This routine will 
	// synchronously wait for all HTTP threads to terminate 
	// before returning.

#ifdef __cplusplus
}
#endif
