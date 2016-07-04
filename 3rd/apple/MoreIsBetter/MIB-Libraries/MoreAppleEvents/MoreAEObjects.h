/*
	File:		MoreAEObjects.h

	Contains:	Functions to help you when you are working with Apple event objects.

	DRI:		George Warner

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

$Log: MoreAEObjects.h,v $
Revision 1.8  2002/11/08 22:49:50  eskimo1
When using framework includes, explicitly include the frameworks we need.

Revision 1.7  2002/03/07 20:28:13  geowar
General clean up and added API's:
MoreAEOCreateAliasObjectFromFSRef
MoreAEOCreateObjectSpecifierFromFSSpec
MoreAEOCreateObjectSpecifierFromFSRef

Revision 1.6  2002/02/19 18:54:01  geowar
Written by => DRI

Revision 1.5  2001/11/07 15:50:40  eskimo1
Tidy up headers, add CVS logs, update copyright.


         <4>     21/9/01    Quinn   Get rid of wacky Finder label.
         <3>     21/9/01    Quinn   Changes for CWPro7 Mach-O build.
         <2>      9/3/00    Quinn   Fix initial checkin problem.
         <1>      3/9/00    GW      Integrating AppleEvent Helper code. First Check In.
*/

#pragma once
// %%%%% need to change calls to object building function to pass a boolean
// %%%%% parameter to control the disposal of descs passed in.
//********************************************************************************
//	A private conditionals file to setup the build environment for this project.
#include "MoreSetup.h"

//**********	Universal Headers		****************************************
#if MORE_FRAMEWORK_INCLUDES
	#include <ApplicationServices/ApplicationServices.h>
#else
	#include <AERegistry.h>
	#include <AEObjects.h>
	#include <AEPackObject.h>
	#include <Aliases.h>
	#include <Icons.h>
	#include <Processes.h>
#endif
//********************************************************************************
#ifdef __cplusplus
	extern "C" {
#endif

#if PRAGMA_IMPORT
	#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
	#pragma pack(2)
#endif
/********************************************************************************
	Add a parameter of type typeAlias to an AERecord (or AppleEvent) using the provided FSSpec.

	pFSSpec			input:	Pointer to the FSSpec to use.
	pKeyword		input:	The key for the data to be added to the record.
	pAERecord		input:	Pointer to the record (or event) to add the data to.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	The value of target or alias parameter, or of
							both, is NIL, or the alias record is corrupt
	memFullErr		-108	Not enough room in heap zone	
*/
extern pascal	OSErr	MoreAEOAddAliasParameterFromFSSpec(const FSSpecPtr pFSSpec,
												const DescType pKeyword,
												AERecord *pAERecord );
/********************************************************************************
	Add a parameter of type typeAlias to an AERecord (or AppleEvent) using the provided FSRef.

	pFSRef			input:	Pointer to the FSRef to use.
	pKeyword		input:	The key for the data to be added to the record.
	pAERecord		input:	Pointer to the record (or event) to add the data to.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	The value of target or alias parameter, or of
							both, is NIL, or the alias record is corrupt
	memFullErr		-108	Not enough room in heap zone	
*/
extern pascal	OSErr	MoreAEOAddAliasParameterFromFSRef(const FSRefPtr pFSRef,
												const DescType pKeyword,
												AERecord *pAERecord );
/********************************************************************************
	Create and return an AEDesc of type typeAlias using the provided FSSpec.

	pFSSpec			input:	Pointer to the FSSpec to use.
	pAliasAEDesc	input:	Pointer to null AEDesc.
					output:	an AEDesc of type typeAlias.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	The value of target or alias parameter, or of
							both, is NIL, or the alias record is corrupt
	memFullErr		-108	Not enough room in heap zone	
*/
extern pascal	OSErr	MoreAEOCreateAliasDescFromFSSpec( const FSSpecPtr pFSSpec,
												  AEDesc *pAliasAEDesc );
/********************************************************************************
	Create and return an AEDesc of type typeAlias using the provided FSRef.

	pFSRef			input:	Pointer to the FSRef to use.
	pAliasAEDesc	input:	Pointer to null AEDesc.
					output:	an AEDesc of type typeAlias.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	The value of target or alias parameter, or of
							both, is NIL, or the alias record is corrupt
	memFullErr		-108	Not enough room in heap zone	
*/
extern pascal	OSErr	MoreAEOCreateAliasDescFromFSRef( const FSRefPtr pFSRef,
												  AEDesc *pAliasAEDesc );
/********************************************************************************
	Create and return an AEDesc of type typeAlias using the provided 
	alias record.

	aliasHandle		input:	Handle to an alias record.
	pAliasAEDesc	input:	Pointer to null AEDesc.
					output:	an AEDesc of type typeAlias.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
*/
extern pascal OSErr MoreAEOCreateAliasDesc( const AliasHandle aliasHandle,
									 AEDesc *pAliasAEDesc );
/********************************************************************************
	Given an FSSpec, return an object descriptor containing an alias,
	contained by containerObj.
	
	pFSSpec			input:	Pointer to the FSSpec to use.
	containerObjPtr	input:	Pointer to container object for object being created.
	aliasObjectPtr	input:	Pointer to null AEDesc.
					output:	an alias object.
	
	RESULT CODES
	____________
	noErr				    0	No error	
	paramErr			  -50	The value of target or alias parameter, or of
								both, is NIL, or the alias record is corrupt
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
	errAEWrongDataType	-1703	Wrong Apple event data type	
	errAENotAEDesc		-1704	Not a valid descriptor record	
	errAEBadListItem	-1705	Operation involving a list item failed	
*/
extern pascal	OSErr	MoreAEOCreateAliasObjectFromFSSpec( const FSSpecPtr pFSSpec,
													AEDesc *containerObjPtr,
													AEDesc *aliasObjectPtr );
/********************************************************************************
	Given an FSRef, return an object descriptor containing an alias,
	contained by containerObj.
	
	pFSRef			input:	Pointer to the FSRef to use.
	containerObjPtr	input:	Pointer to container object for object being created.
	aliasObjectPtr	input:	Pointer to null AEDesc.
					output:	an alias object.
	
	RESULT CODES
	____________
	noErr				    0	No error	
	paramErr			  -50	The value of target or alias parameter, or of
								both, is NIL, or the alias record is corrupt
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
	errAEWrongDataType	-1703	Wrong Apple event data type	
	errAENotAEDesc		-1704	Not a valid descriptor record	
	errAEBadListItem	-1705	Operation involving a list item failed	
*/
extern pascal	OSErr	MoreAEOCreateAliasObjectFromFSRef( const FSRefPtr pFSRef,
													AEDesc *containerObjPtr,
													AEDesc *aliasObjectPtr );
/********************************************************************************
	Given an AliasHandle, return an object descriptor containing an alias,
	contained by containerObj.
	
	aliasHandle		input:	Handle to an alias record.
	containerObjPtr	input:	Pointer to container object for object being created.
	aliasObjectPtr	input:	Pointer to null AEDesc.
					output:	an alias object.
	
	RESULT CODES
	____________
	noErr				    0	No error	
	paramErr			  -50	Error in parameter list
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
	errAEWrongDataType	-1703	Wrong Apple event data type	
	errAENotAEDesc		-1704	Not a valid descriptor record	
	errAEBadListItem	-1705	Operation involving a list item failed	
*/
extern pascal	OSErr	MoreAEOCreateAliasObject( const AliasHandle aliasHandle,
										  AEDesc *containerObjPtr,
										  AEDesc *aliasObjectPtr );
/********************************************************************************
	Given a property type, create an new object descriptor for that property,
	contained by containerObj.
	
	propType		input:	Property type to use for object.
	containerObjPtr	input:	Pointer to container object for object being created.
	propertyObjPtr	input:	Pointer to null AEDesc.
					output:	A property object.
	
	RESULT CODES
	____________
	noErr				    0	No error	
	paramErr			  -50	Error in parameter list
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
	errAEWrongDataType	-1703	Wrong Apple event data type	
	errAENotAEDesc		-1704	Not a valid descriptor record	
	errAEBadListItem	-1705	Operation involving a list item failed	
*/
extern pascal	OSErr	MoreAEOCreatePropertyObject( const DescType propType,
											 AEDesc *containerObjPtr,
											 AEDesc *propertyObjPtr );
/********************************************************************************
	Given a ProcessSerialNumber, create an new object descriptor for the PSN,
	contained by containerObj.
	
	psnPtr			input:	ProcessSerialNumber to use for object.
	containerObjPtr	input:	Pointer to container object for object being created.
	psnObjPtr		input:	Pointer to null AEDesc.
					output:	A ProcessSerialNumber object.
	
	RESULT CODES
	____________
	noErr				    0	No error	
	paramErr			  -50	Error in parameter list
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
	errAEWrongDataType	-1703	Wrong Apple event data type	
	errAENotAEDesc		-1704	Not a valid descriptor record	
	errAEBadListItem	-1705	Operation involving a list item failed	
*/
extern pascal	OSErr	MoreAEOCreateProcessObject( const ProcessSerialNumber *psnPtr,
											AEDesc *containerObjPtr,
											AEDesc *psnObjPtr );
/********************************************************************************
	Given selection type, create an new object descriptor for a selection,
	contained by containerObj.
	
	selection		input:	Selection type to use for object.
	containerObjPtr	input:	Pointer to container object for object being created.
	selectionObject	input:	Pointer to null AEDesc.
					output:	A property object.
	
	RESULT CODES
	____________
	noErr				    0	No error	
	paramErr			  -50	Error in parameter list
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
	errAEWrongDataType	-1703	Wrong Apple event data type	
	errAENotAEDesc		-1704	Not a valid descriptor record	
	errAEBadListItem	-1705	Operation involving a list item failed	
*/
extern pascal	OSErr	MoreAEOCreateSelectionObject( const DescType selection,
											  AEDesc *containerObjPtr,
											  AEDesc *selectionObject );
/********************************************************************************
	Make position list (a list containing two longs representin the x and y values
	for the position of a Finder item).
	
	position			input:	A point specifying the position.
	pPositionAEList		input:	Pointer to an AEList (contents will be lost, but not disposed).
						output:	A new AEList containing the x & y values for the position.
	
	Result Codes
	____________
	noErr				    0	No error	
	memFullErr			 -108	Not enough room in heap zone	
*/
extern pascal	OSErr	MoreAEOCreatePositionList( const Point position,
									 AEDescList *pPositionAEList );
//********************************************************************************
// A simple wrapper around CreateObjSpecifier which creates
// an object specifier using formUniqueID and the unique ID
// in pKeyData.
extern pascal OSStatus MoreAEOCreateObjSpecifierFormUniqueID(DescType pDesiredClass, const AEDesc *pContainer, 
												SInt32 pKeyData, Boolean pDisposeInputs, 
												AEDesc *pObjSpecifier);
//********************************************************************************
// A simple wrapper around CreateObjSpecifier which creates
// an object specifier using formAbsolutePosition, a key of
// typeLongInteger (rather than typeAbsoluteOrdinal) and the
// position index in pKeyData.
extern pascal OSStatus MoreAEOCreateObjSpecifierFormAbsPos(DescType pDesiredClass, const AEDesc *pContainer, 
											SInt32 pKeyData, SInt32 pDisposeInputs, 
											AEDesc *pObjSpecifier);
//********************************************************************************
// A simple wrapper around CreateObjSpecifier which creates
// an object specifier using formName and the name in pKeyData.
extern pascal OSStatus MoreAEOCreateObjSpecifierFormName(DescType pDesiredClass, const AEDesc *pContainer, 
											ConstStr255Param pKeyData, Boolean pDisposeInputs, 
											AEDesc *pObjSpecifier);
//********************************************************************************
// A simple wrapper around CreateObjSpecifier which creates
// an object specifier from a FSSpec and using formName.
extern pascal OSStatus MoreAEOCreateObjSpecifierFromFSSpec(const FSSpecPtr pFSSpecPtr,AEDesc *pObjSpecifier);
//********************************************************************************
// A simple wrapper around CreateObjSpecifier which creates
// an object specifier from a FSRef and using formName.
extern pascal OSStatus MoreAEOCreateObjSpecifierFromFSRef(const FSRefPtr pFSRefPtr,AEDesc *pObjSpecifier);
//********************************************************************************
// A simple wrapper around CreateObjSpecifier which creates
// an object specifier from a CFURLRef.
#if TARGET_API_MAC_CARBON
extern pascal OSStatus MoreAEOCreateObjSpecifierFromCFURLRef(const CFURLRef pCFURLRef,AEDesc *pObjSpecifier);
#endif TARGET_API_MAC_CARBON
//********************************************************************************

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
	#pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif