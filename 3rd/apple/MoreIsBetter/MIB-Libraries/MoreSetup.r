/*
	File:		MoreSetup.r

	Contains:	Sets up conditions etc for MoreIsBetter.

	Written by:	Quinn

	Copyright:	Copyright (c) 1998-2001 by Apple Computer, Inc., All Rights Reserved.

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

$Log: MoreSetup.r,v $
Revision 1.4  2002/11/08 22:41:05  eskimo1
Finally worked out a way to make .r files work properly in both CW and PB. Also synced the logic here with the latest logic in "MoreSetup.h".

Revision 1.3  2001/11/07 15:55:23  eskimo1
Tidy up headers, add CVS logs, update copyright.


         <2>     11/4/00    Quinn   Tweaks to TARGET_API_MAC_CARBON handling for non-Carbon builds.
         <1>     20/3/00    Quinn   First checked in.
*/

	//
	//	We never want to use old names or locations.
	//	Since these settings must be consistent all the way through
	//	a compilation unit, and since we don't want to silently
	//	change them out from under a developer who uses a prefix
	//	file (C/C++ panel of Target Settings), we simply complain
	//	if they are already set in a way we don't like.
	//

#ifndef OLDROUTINELOCATIONS
	#define OLDROUTINELOCATIONS 0
#elif OLDROUTINELOCATIONS
	#error OLDROUTINELOCATIONS must be FALSE when compiling MoreIsBetter.
#endif

#ifndef OLDROUTINENAMES
	#define OLDROUTINENAMES 0
#elif OLDROUTINENAMES
	#error OLDROUTINENAMES must be FALSE when compiling MoreIsBetter.
#endif

	// "ConditionalMacros.r" seems to have troubles if you define
	// TARGET_API_MAC_CARBON to true without defining the alternative
	// (TARGET_API_MAC_OS8) to false.  Here we fix that up.

#ifdef TARGET_API_MAC_CARBON
	#if TARGET_API_MAC_CARBON
		#define TARGET_API_MAC_OS8 0
	#endif
#endif

	// 
	//  For Project Builder builds, we define MORE_FRAMEWORK_INCLUDES 
	//  using OTHER_REZFLAGS.  For CodeWarrior builds, we'll just 
	//  assume flat includes.
	//

#ifndef MORE_FRAMEWORK_INCLUDES
	#define MORE_FRAMEWORK_INCLUDES 0
#endif

	//
	//	Bring in "ConditionalMacros.r" in order to set up
	//	UNIVERSAL_INTERFACES_VERSION.
	//

#if MORE_FRAMEWORK_INCLUDES
	#include <CoreServices/CoreServices.r>
#else
	#include <ConditionalMacros.r>
#endif

	//	Now that we've included a Mac OS interface file,
	//	we know that the Universal Interfaces environment
	//	is set up.  MoreIsBetter requires Universal Interfaces
	//	3.2 or higher.  Check for it.

#if !defined(UNIVERSAL_INTERFACES_VERSION) || UNIVERSAL_INTERFACES_VERSION < 0x0320
	#error MoreIsBetter requires Universal Interfaces 3.2 or higher.
#endif

	//
	//	We usually want assertions and other debugging code
	//	turned on, but you can turn it all off if you like
	//	by setting MORE_DEBUG to 0.
	//

#ifndef MORE_DEBUG
	#define MORE_DEBUG 1
#endif
