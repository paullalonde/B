/*
	File:		MoreCFQ.c

	Contains:	Core Foundation utility Routines.

	Written by:	Quinn

	Copyright:	Copyright (c) 2001 by Apple Computer, Inc., All Rights Reserved.

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

$Log: MoreCFQ.c,v $
Revision 1.1.1.1  2003/09/25 21:57:18  paull
First Imported.

Revision 1.8  2003/04/14 15:49:05  eskimo1
Add CFQAllocate/Deallocate.  Use it to prevent "malloc(0) returns NULL" problems.

Revision 1.7  2003/04/04 15:01:45  eskimo1
Added CFQCreateBundleFromFrameworkName.

Revision 1.6  2003/02/26 20:49:21  eskimo1
The GetValueAtPath APIs are "Get" and not "Copy" APIs, so they don't need *result == NULL as a precondition.

Revision 1.5  2002/12/12 15:22:25  eskimo1
Added CFQDictionaryMerge.

Revision 1.4  2002/11/08 23:08:47  eskimo1
Convert nil to NULL. Added CFQStringCopyCString. Convert MoreAssertQ to assert.

Revision 1.3  2002/10/24 11:46:07  eskimo1
Added CFQError routines.

Revision 1.2  2002/01/22 06:14:19  eskimo1
Change CFQDictionaryAddNumber to CFQDictionarySetNumber.

Revision 1.1  2002/01/16 22:51:31  eskimo1
First checked in.


*/

/////////////////////////////////////////////////////////////////

// Our prototypes

#include "MoreCFQ.h"

// System interfaces

#if ! MORE_FRAMEWORK_INCLUDES
	#include <MacErrors.h>
	#include <Folders.h>
	#include <CFNumber.h>
	#include <CFURLAccess.h>
#endif

#include <stdlib.h>

/////////////////////////////////////////////////////////////////
#pragma mark ***** Trivial Utilities

extern pascal OSStatus CFQErrorBoolean(Boolean shouldBeTrue)
{
	OSStatus err;
	
	err = noErr;
	if (!shouldBeTrue) {
		err = coreFoundationUnknownErr;
	}
	return err;
}

extern pascal OSStatus CFQError(const void *shouldBeNotNULL)
{
	return CFQErrorBoolean(shouldBeNotNULL != NULL);
}

extern pascal CFTypeRef CFQRetain(CFTypeRef cf)
	// See comment in header.
{
	if (cf != NULL) {
		(void) CFRetain(cf);
	}
	return cf;
}

extern pascal void CFQRelease(CFTypeRef cf)
	// See comment in header.
{
	if (cf != NULL) {
		CFRelease(cf);
	}
}

extern pascal OSStatus CFQAllocate(ByteCount bytesToAllocate, void **blockPtrPtr)
	// See comment in header.
{
	OSStatus 	err;
	void *		blockPtr;
	
	assert( ((CFIndex) bytesToAllocate) >= 0 );		// CFIndex is signed
	assert( blockPtrPtr != NULL);
	assert(*blockPtrPtr == NULL);

	// CFAllocatorAllocate always returns NULL if you ask for 0 bytes. 
	// For my coding style, that's very unhelpful.  So if you ask for 
	// a block of 0 bytes, you actually get a block of 1 bytes.
	
	if (bytesToAllocate == 0) {
		bytesToAllocate += 1;
	}
	
	// Allocate the memory.
	
	err = noErr;
	// NULL is a synonym for kCFAllocatorDefault.
	blockPtr = CFAllocatorAllocate(NULL, (CFIndex) bytesToAllocate, 0);
	if (blockPtr == NULL) {
		err = memFullErr;
	}
	*blockPtrPtr = blockPtr;
	
	assert( (err == noErr) == (*blockPtrPtr != NULL) );
	
	return err;
}

extern pascal void     CFQDeallocate(void *blockPtr)
	// See comment in header.
{
	// CFAllocatorDeallocate handles blockPtr being NULL.
	
	// NULL is a synonym for kCFAllocatorDefault.
	CFAllocatorDeallocate(NULL, blockPtr);
}

extern pascal OSStatus CFQDictionaryCreateMutable(CFMutableDictionaryRef *result)
	// See comment in header.
{
	OSStatus err;
	
	assert( result != NULL);
	assert(*result == NULL);

	err = noErr;
	*result = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (*result == NULL) {
		err = coreFoundationUnknownErr;
	}
	assert( (err == noErr) == (*result != NULL) );
	return err;
}

extern pascal OSStatus CFQArrayCreateMutable(CFMutableArrayRef *result)
	// See comment in header.
{
	OSStatus err;
	
	assert( result != NULL);
	assert(*result == NULL);

	err = noErr;
	*result = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	if (*result == NULL) {
		err = coreFoundationUnknownErr;
	}
	assert( (err == noErr) == (*result != NULL) );
	return err;
}

extern pascal OSStatus CFQArrayCreateWithDictionaryKeys(CFDictionaryRef dict, CFArrayRef *result)
	// See comment in header.
{
	OSStatus 	err;
	CFTypeRef *	keys;
	
	assert( dict   != NULL);
	assert( result != NULL);
	assert(*result == NULL);

	// Allocate a buffer for the keys, get the keys into the buffer, and 
	// create the array from that buffer.
		
	keys = NULL;
	err = CFQAllocate(CFDictionaryGetCount(dict) * sizeof(CFTypeRef), (void **) &keys);
	if (err == noErr) {
		CFDictionaryGetKeysAndValues(dict, (const void **) keys, NULL);
		
		*result = CFArrayCreate(NULL, keys, CFDictionaryGetCount(dict), &kCFTypeArrayCallBacks);
		if (*result == NULL) {
			err = coreFoundationUnknownErr;
		}
	}

	// Clean up.
	
	CFQDeallocate(keys);
	
	assert( (err == noErr) == (*result != NULL) );
	assert( (*result == NULL) || (CFDictionaryGetCount(dict) == CFArrayGetCount(*result)) );
	
	return err;
}

extern pascal OSStatus CFQArrayCreateWithDictionaryValues(CFDictionaryRef dict, CFArrayRef *result)
	// See comment in header.
{
	OSStatus 	err;
	CFTypeRef *	values;
	
	assert( dict   != NULL);
	assert( result != NULL);
	assert(*result == NULL);
	
	// Allocate a buffer for the values, get the values into the buffer, and 
	// create the array from that buffer.

	values = NULL;
	err = CFQAllocate(CFDictionaryGetCount(dict) * sizeof(CFTypeRef), (void **) &values);
	if (err == noErr) {
		CFDictionaryGetKeysAndValues(dict, NULL, (const void **) values);
		
		*result = CFArrayCreate(NULL, values, CFDictionaryGetCount(dict), &kCFTypeArrayCallBacks);
		if (*result == NULL) {
			err = coreFoundationUnknownErr;
		}
	}
	
	// Clean up.
	
	CFQDeallocate(values);
	
	assert( (err == noErr) == (*result != NULL) );
	assert( (*result == NULL) || (CFDictionaryGetCount(dict) == CFArrayGetCount(*result)) );
	
	return err;
}

extern pascal OSStatus CFQDictionaryCreateWithArrayOfKeysAndValues(CFArrayRef keys, 
																   CFArrayRef values, 
																   CFDictionaryRef *result)
	// See comment in header.
{
	OSStatus    err;
	CFIndex 	count;
	CFTypeRef * keysBuffer;
	CFTypeRef * valuesBuffer;
	
	assert(keys != NULL);
	assert(values != NULL);
	assert( CFArrayGetCount(keys) == CFArrayGetCount(values) );
	assert( result != NULL);
	assert(*result == NULL);
	
	keysBuffer   = NULL;
	valuesBuffer = NULL;

	// Check that the arrays are of a like size.
		
	err = noErr;
	count = CFArrayGetCount(keys);
	if ( count != CFArrayGetCount(values) ) {
		err = paramErr;
	}
	
	// Allocate a buffer for both keys and values.
	
	if (err == noErr) {
		err = CFQAllocate(sizeof(CFTypeRef) * count, (void **) &keysBuffer);
	}
	if (err == noErr) {
		err = CFQAllocate(sizeof(CFTypeRef) * count, (void **) &valuesBuffer);
	}
	
	// Get the keys and values into their buffers, and create a 
	// dictionary based on the buffer.
	
	if (err == noErr) {
		CFArrayGetValues(keys,   CFRangeMake(0, count), keysBuffer);
		CFArrayGetValues(values, CFRangeMake(0, count), valuesBuffer);
		
		*result = CFDictionaryCreate(NULL, keysBuffer, valuesBuffer, count, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		if (*result == NULL) {
			err = coreFoundationUnknownErr;
		}
	}
	
	// Clean up.
	
	CFQDeallocate(keysBuffer);
	CFQDeallocate(valuesBuffer);
	
	return err;
}

extern pascal OSStatus CFQDictionarySetNumber(CFMutableDictionaryRef dict, const void *key, long value)
	// See comment in header.
{
	OSStatus    err;
	CFNumberRef valueNum;

	// Create a CFNumber and add it to the dictionary.
		
	err = noErr;
	valueNum = CFNumberCreate(NULL, kCFNumberLongType, &value);
	if (valueNum == NULL) {
		err = coreFoundationUnknownErr;
	}
	if (err == noErr) {
		CFDictionarySetValue(dict, key, valueNum);
	}
	CFQRelease(valueNum);
	
	return err;
}

extern pascal OSStatus CFQStringCopyCString(CFStringRef str, CFStringEncoding encoding, char **cStrPtr)
	// See comment in header.
{
	OSStatus		err;
	CFIndex			cStrLen;
	CFRange 		range;
	
	assert( str     != NULL);
	assert( cStrPtr != NULL);
	assert(*cStrPtr == NULL);

	err = noErr;
	
	range = CFRangeMake(0, CFStringGetLength(str));
		
	(void) CFStringGetBytes(str, range, encoding, 0, false, NULL, 0, &cStrLen);

	// Can't convert to CFQAllocate because the caller is required 
	// to free the string using "free", and there's no guarantee that 
	// CFQAllocate and "free" are compatible (for example, if you're 
	// compiling CFM and running on Mac OS X, "free" is from an MSL pool 
	// while CFQAllocate allocates from System framework).
	// 
	// Anyway, this isn't a problem because the size is always at 
	// least one byte, so malloc won't misbehave.
	
	*cStrPtr = (char *) malloc( ((size_t) cStrLen) + 1);
	if (*cStrPtr == NULL) {
		err = memFullErr;
	}

	if (err == noErr) {
		#if MORE_DEBUG
			(*cStrPtr)[cStrLen] = '�';
		#endif
		
		if ( CFStringGetBytes(str, range, encoding, 0, false, (UInt8 *) *cStrPtr, cStrLen, &cStrLen) != range.length ) {
			err = kCFQDataErr;
		}
	}
	if (err == noErr) {		
		assert((*cStrPtr)[cStrLen] == '�');
		(*cStrPtr)[cStrLen] = 0;
	} else {
		free(*cStrPtr);
		*cStrPtr = NULL;
	}

	assert( (err == noErr) == (*cStrPtr != NULL) );

	return err;
}

/////////////////////////////////////////////////////////////////
#pragma mark ***** Bundle Routines

extern pascal OSStatus CFQBundleCreateFromFrameworkName(CFStringRef frameworkName, 
														CFBundleRef *bundlePtr)
	// See comment in header.
{
	OSStatus 	err;
	FSRef 		frameworksFolderRef;
	CFURLRef	baseURL;
	CFURLRef	bundleURL;
	
	assert(frameworkName != NULL);
	assert( bundlePtr != NULL);
	assert(*bundlePtr == NULL);
	
	*bundlePtr = NULL;
	
	baseURL = NULL;
	bundleURL = NULL;
	
	// Find the frameworks folder and create a URL for it.
	
	err = FSFindFolder(kOnAppropriateDisk, kFrameworksFolderType, true, &frameworksFolderRef);
	if (err == noErr) {
		baseURL = CFURLCreateFromFSRef(kCFAllocatorSystemDefault, &frameworksFolderRef);
		err = CFQError(baseURL);
	}
	
	// Append the name of the framework to the URL.
	
	if (err == noErr) {
		bundleURL = CFURLCreateCopyAppendingPathComponent(kCFAllocatorSystemDefault, baseURL, frameworkName, false);
		err = CFQError(bundleURL);
	}
	
	// Create a bundle based on that URL and load the bundle into memory.
	
	if (err == noErr) {
		*bundlePtr = CFBundleCreate(kCFAllocatorSystemDefault, bundleURL);
		err = CFQError(*bundlePtr);
	}
	if (err == noErr) {
	    err = CFQErrorBoolean( CFBundleLoadExecutable( *bundlePtr ) );
	}

	// Clean up.
	
	if (err != noErr) {
		CFQRelease(*bundlePtr);
		*bundlePtr = NULL;
	}
	CFQRelease(bundleURL);
	CFQRelease(baseURL);
	
	assert( (err == noErr) == (*bundlePtr != NULL) );
	
	return err;
}

/////////////////////////////////////////////////////////////////
#pragma mark ***** Dictionary Path Routines

extern pascal OSStatus CFQDictionaryGetValueAtPath(CFDictionaryRef dict, 
												   const void *path[], CFIndex pathElementCount, 
												   const void **result)
	// See comment in header.
{
	OSStatus 		err;
	CFDictionaryRef parent;
	CFIndex 		thisElement;
	const void *	child;

	assert( dict   != NULL);
	assert( path   != NULL);
	assert( pathElementCount > 0 );		// 0 length paths aren't allowed
	assert( result != NULL);

	// Some subtleties in the following loop.
	//
	// o It's possible for dictionaries to contain NULL values, so we use 
	//   CFDictionaryGetValueIfPresent to test for the presence of the 
	//   child rather than CFDictionaryGetValue.
	//
	// o We use CFDictionaryGetValueIfPresent in place of the combination of
	//   CFDictionaryContainsKey and CFDictionaryGetValue because it does 
	//   both operations in one hit.
	// 
	// o Loop terminates in one of three ways:
	//     i) a path element isn't found (kCFQKeyNotFoundErr)
	//    ii) we find the last path element
	//   iii) we proceed to the next path element but the child we 
	//        found isn't valid (kCFQDataErr)
	//
	// o There are no releases or retains in this loop because all values are 
	//    got with "Get" operations.

	err = noErr;		
	thisElement = 0;
	parent = dict;

	while (true) {
		if ( ! CFDictionaryGetValueIfPresent(parent, path[thisElement], &child) ) {
			err = kCFQKeyNotFoundErr;
			break;
		}
	
		if (thisElement == (pathElementCount - 1)) {
			break;
		}
		
		if ( (child == NULL) || (CFGetTypeID(child) != CFDictionaryGetTypeID()) ) {
			err = kCFQDataErr;
			break;
		}

		parent = (CFDictionaryRef) child;
		thisElement += 1;
	}
	
	if (err == noErr) {
		*result = child;
	}
	
	return err;
}

static OSStatus PathArrayHelper(CFArrayRef path, CFTypeRef **pathElements)
	// pathElements is a pointer to an array of CFTypeRefs. 
	// On return, *pathElements is set to a newly allocated 
	// array which you must free using "free".
{
	OSStatus err;
	CFIndex  pathElementCount;

	assert(path != NULL);
	assert(pathElements != NULL);
	// We don't assert that *pathElements is NULL because part of 
	// our semantics is that we always set it to NULL.

	pathElementCount = CFArrayGetCount(path);
	assert(pathElementCount > 0);
	
	err = noErr;
	*pathElements = (CFTypeRef *) malloc(sizeof(CFTypeRef) * pathElementCount);		// pathElementCount > 0, so malloc(0) not possible
	if (*pathElements == NULL) {
		err = memFullErr;
	}
	if (err == noErr) {
		CFArrayGetValues(path, CFRangeMake(0, pathElementCount), *pathElements);
	}
	
	assert( (err == noErr) == (*pathElements != NULL) );
	
	return err;
}

extern pascal OSStatus CFQDictionaryGetValueAtPathArray(CFDictionaryRef dict, 
												   CFArrayRef path, 
												   const void **result)
	// See comment in header.
{
	OSStatus 	err;
	CFTypeRef * pathElements;
	
	assert( dict   != NULL);
	assert( path   != NULL);
	assert( result != NULL);

	err = PathArrayHelper(path, &pathElements);
	if (err == noErr) {
		err = CFQDictionaryGetValueAtPath(dict, pathElements, CFArrayGetCount(path), result);
	}
	if (pathElements != NULL) {
		free(pathElements);
	}

	return err;
}

static OSStatus CFQMutableDictionaryGetParentForPath(CFMutableDictionaryRef dict,
											   const void *path[], CFIndex pathElementCount, 
											   CFMutableDictionaryRef *result)
{
	OSStatus 				err;
	CFIndex 				thisElement;
	CFMutableDictionaryRef 	parent;
	CFDictionaryRef 		child;
	
	assert( dict   != NULL);
	assert( path   != NULL);
	assert( pathElementCount > 0 );		// 0 length paths aren't allowed
	assert( result != NULL);

	// Some subtleties in the following loop.
	//
	// o It's possible for dictionaries to contain NULL values, so we use 
	//   CFDictionaryGetValueIfPresent to test for the presence of the 
	//   child rather than CFDictionaryGetValue.
	//
	// o We use CFDictionaryGetValueIfPresent in place of the combination of
	//   CFDictionaryContainsKey and CFDictionaryGetValue because it does 
	//   both operations in one hit.
	// 
	// o Loop terminates in one of three ways:
	//     i) we reach the last path element
	//    ii) a path element isn't found (kCFQKeyNotFoundErr)
	//   iii) we proceed to the next path element but the child we 
	//        found isn't valid (kCFQDataErr)
	//    iv) we get an error when creating a mutable dictionary
	//
	// o Memory tracking is very tricky.  Specifically, we have to be 
	//   very careful with the mutableChild variable.  We create this 
	//   each time we progress through a dictionary in the loop.  Each 
	//   time we create it we then immediately add it to the parent 
	//   dictionary, thus ensuring a continued reference count after 
	//   we release our reference count.

	err = noErr;
	thisElement = 0;
	parent = dict;

	while (true) {
		if (thisElement == (pathElementCount - 1)) {
			break;
		}

		if ( ! CFDictionaryGetValueIfPresent(parent, path[thisElement], (const void **) &child) ) {
			err = kCFQKeyNotFoundErr;
			break;
		}
	
		if ( (child == NULL) || (CFGetTypeID(child) != CFDictionaryGetTypeID()) ) {
			err = kCFQDataErr;
			break;
		}
		
		{
			CFMutableDictionaryRef mutableChild;
			
			mutableChild = CFDictionaryCreateMutableCopy(NULL, 0, child);
			if (mutableChild == NULL) {
				err = coreFoundationUnknownErr;
			}
			if (err == noErr) {
				CFDictionarySetValue(parent, path[thisElement], mutableChild);
				
				assert( CFGetRetainCount(mutableChild) >= 2 );
				
				parent = mutableChild;
				thisElement += 1;
			}
			
			CFQRelease(mutableChild);
		}
		if (err != noErr) {
			break;
		}
	}
	if (err == noErr) {
		*result = parent;
	}
	
	assert( (err == noErr) == (*result != NULL) );
	
	return err;
}

extern pascal OSStatus CFQDictionarySetValueAtPath(CFMutableDictionaryRef dict, 
												   const void *path[], CFIndex pathElementCount, 
												   const void *value)
	// See comment in header.
{
	OSStatus 				err;
	CFMutableDictionaryRef 	parent;
	
	assert( dict   != NULL);
	assert( path   != NULL);
	assert( pathElementCount > 0 );		// 0 length paths aren't allowed

	err = CFQMutableDictionaryGetParentForPath(dict, path, pathElementCount, &parent);
	if (err == noErr) {
		CFDictionarySetValue(parent, path[pathElementCount - 1], value);
	}

	return err;	
}

extern pascal OSStatus CFQDictionarySetValueAtPathArray(CFMutableDictionaryRef dict, 
												   CFArrayRef path, 
												   const void *value)
	// See comment in header.
{
	OSStatus 	err;
	CFTypeRef *	pathElements;
	
	assert( dict   != NULL);
	assert( path   != NULL);

	err = PathArrayHelper(path, &pathElements);
	if (err == noErr) {
		err = CFQDictionarySetValueAtPath(dict, pathElements, CFArrayGetCount(path), value);
	}
	if (pathElements != NULL) {
		free(pathElements);
	}

	return err;
}

extern pascal OSStatus CFQDictionaryRemoveValueAtPath(CFMutableDictionaryRef dict, 
												   const void *path[], CFIndex pathElementCount)
	// See comment in header.
{
	OSStatus 				err;
	CFMutableDictionaryRef 	parent;
	
	assert( dict   != NULL);
	assert( path   != NULL);
	assert( pathElementCount > 0 );		// 0 length paths aren't allowed

	err = CFQMutableDictionaryGetParentForPath(dict, path, pathElementCount, &parent);
	if ( ! CFDictionaryContainsKey(parent, path[pathElementCount - 1]) ) {
		err = kCFQKeyNotFoundErr;
	}
	if (err == noErr) {
		CFDictionaryRemoveValue(parent, path[pathElementCount - 1]);
	}

	return err;	
}

extern pascal OSStatus CFQDictionaryRemoveValueAtPathArray(CFMutableDictionaryRef dict, 
												   CFArrayRef path)
	// See comment in header.
{
	OSStatus 	err;
	CFTypeRef *	pathElements;
	
	assert( dict   != NULL);
	assert( path   != NULL);

	err = PathArrayHelper(path, &pathElements);
	if (err == noErr) {
		err = CFQDictionaryRemoveValueAtPath(dict, pathElements, CFArrayGetCount(path));
	}
	if (pathElements != NULL) {
		free(pathElements);
	}

	return err;
}

/////////////////////////////////////////////////////////////////
#pragma mark ***** Property List Traversal Routines

extern pascal void CFQPropertyListDeepApplyFunction(CFPropertyListRef propList, 
													CFQPropertyListDeepApplierFunction func,
													void *context)
	// See comment in header.
{
	assert(propList != NULL);
	assert(func     != NULL);
	
	// Call "func" for this node.
	
	func(propList, context);
	
	// If this node is a dictionary or an array, call func for 
	// each element.
	
	if ( CFGetTypeID(propList) == CFDictionaryGetTypeID() ) {
		CFIndex count;
		CFIndex index;
		
		count = CFDictionaryGetCount( (CFDictionaryRef) propList);
		if (count > 0) {
			const void **keys;

			keys = (const void **) malloc( count * sizeof(const void *));
			if (keys != NULL) {
				CFDictionaryGetKeysAndValues( (CFDictionaryRef) propList, keys, NULL);
				
				for (index = 0; index < count; index++) {
					CFQPropertyListDeepApplyFunction(CFDictionaryGetValue( (CFDictionaryRef) propList, keys[index]), func, context);
				}
				free(keys);
			}
		}
	} else if ( CFGetTypeID(propList) == CFArrayGetTypeID() ) {
		CFIndex count;
		long    index;
		
		count = CFArrayGetCount( (CFArrayRef) propList);
		for (index = 0; index < count; index++) {
			CFQPropertyListDeepApplyFunction(CFArrayGetValueAtIndex( (CFArrayRef) propList, index), func, context);
		}
	}
}

extern pascal Boolean CFQPropertyListIsLeaf(CFTypeRef node)
	// See comment in header.
{
	return ! (	   (CFGetTypeID(node) == CFDictionaryGetTypeID())
				|| (CFGetTypeID(node) == CFArrayGetTypeID())      );
}

extern pascal void CFQPropertyListShallowApplyFunction(CFPropertyListRef propList, 
													   CFQPropertyListShallowApplierFunction func,
													   void *context)
	// See comment in header.
{
	assert(propList != NULL);
	assert(func     != NULL);

	// If this node is a dictionary, call "func" for each element.
	//
	// If this node is an array, call "func" for each element and 
	// pass a CFNumber of the element's array index to its "key" 
	// parameter.
	
	if ( CFGetTypeID(propList) == CFDictionaryGetTypeID() ) {
		CFIndex count;
		CFIndex index;
		
		count = CFDictionaryGetCount( (CFDictionaryRef) propList);
		
		if (count > 0) {
			const void **keys;
			
			keys = (const void **) malloc( count * sizeof(const void *));
			if (keys != NULL) {
				CFDictionaryGetKeysAndValues( (CFDictionaryRef) propList, keys, NULL);
				
				for (index = 0; index < count; index++) {
					func(keys[index], CFDictionaryGetValue( (CFDictionaryRef) propList, keys[index]), context);
				}
				free(keys);
			}
		}
	} else if ( CFGetTypeID(propList) == CFArrayGetTypeID() ) {
		CFIndex count;
		CFIndex index;
		
		count = CFArrayGetCount( (CFArrayRef) propList);
		for (index = 0; index < count; index++) {
			CFNumberRef key;
			
			key = CFNumberCreate(NULL, kCFNumberLongType, &index);
			if (key != NULL) {
				func(key, CFArrayGetValueAtIndex( (CFArrayRef) propList, index), context);
				CFRelease(key);
			}
		}
	} else {
		assert(false);
	}
}

extern pascal OSStatus CFQPropertyListCreateFromXMLFSRef(const FSRef *xmlFile, CFPropertyListMutabilityOptions options, CFPropertyListRef *result)
	// See comment in header.
{
	OSStatus  err;
	CFURLRef  xmlURL;
	
	assert(xmlFile != NULL);
	assert( result != NULL);
	assert(*result == NULL);
	
	xmlURL = CFURLCreateFromFSRef(NULL, xmlFile);
	err = CFQError(xmlURL);
	if (err == noErr) {
		err = CFQPropertyListCreateFromXMLCFURL(xmlURL, options, result);
	}
	
	CFQRelease(xmlURL);
	
	assert( (err == noErr) == (*result != NULL) );

	return err;
}

extern pascal OSStatus CFQPropertyListCreateFromXMLCFURL(CFURLRef xmlFile, CFPropertyListMutabilityOptions options, CFPropertyListRef *result)
	// See comment in header.
{
	OSStatus  err;
	CFDataRef xmlData;
	
	assert(xmlFile != NULL);
	assert( result != NULL);
	assert(*result == NULL);

	xmlData = NULL;

	err = noErr;
	if ( ! CFURLCreateDataAndPropertiesFromResource(NULL, xmlFile, &xmlData, NULL, NULL, &err) && (err == noErr) ) {
		err = coreFoundationUnknownErr;
	}
	
	if (err == noErr) {
		*result = CFPropertyListCreateFromXMLData(NULL, xmlData, options, NULL);
		if (*result == NULL) {
			err = coreFoundationUnknownErr;
		}
	}

	CFQRelease(xmlData);

	assert( (err == noErr) == (*result != NULL) );
	
	return err;
}

static void MergeOne(const void *key, const void *value, void *context)
{
	CFMutableDictionaryRef 	dst;
	
	assert(context != NULL);
	
	dst = (CFMutableDictionaryRef) context;
	assert( CFGetTypeID(dst) == CFDictionaryGetTypeID() );
	
	CFDictionarySetValue(dst, key, value);
}

extern pascal OSStatus CFQDictionaryMerge(CFMutableDictionaryRef dst, CFDictionaryRef src)
{
	OSStatus err;
	
	assert(dst != NULL);
	assert(src != NULL);
	
	err = noErr;
	CFDictionaryApplyFunction(src, MergeOne, dst);
	
	return err;
}
