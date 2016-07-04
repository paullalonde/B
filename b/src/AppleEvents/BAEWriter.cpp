// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
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

// file header
#include "BAEWriter.h"

// system headers
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

// B headers
#include "BAEDescriptor.h"
#include "BAEEvent.h"


namespace B {

// ==========================================================================================
//	AEWriter

// ------------------------------------------------------------------------------------------
AEWriter::AEWriter()
	: mStream(NULL), mNextKeyword(0)
{
	Open();
}

// ------------------------------------------------------------------------------------------
AEWriter::AEWriter(
	AEEventClass	clazz,										//!< The Apple %Event's event class.
	AEEventID		id,											//!< The Apple %Event's event ID.
	short			returnID /* = kAutoGenerateReturnID */,		//!< The return ID, used to pair up events with replies.
	long			transactionID /* = kAnyTransactionID */)	//!< The transaction ID, used to track a series of related events.
		: mStream(NULL), mNextKeyword(0)
{
	Open(clazz, id, returnID, transactionID);
}


// ------------------------------------------------------------------------------------------
AEWriter::AEWriter(
	AppleEvent&		event)	//!< The Apple %Event to append to.
		: mStream(NULL), mNextKeyword(0)
{
	Open(event);
}

// ------------------------------------------------------------------------------------------
AEWriter::AEWriter(
	AEStreamRef		stream)	//!< The stream to append to.
		: mStream(stream), mOwned(false), mNextKeyword(0)
{
}

// ------------------------------------------------------------------------------------------
/*!	If the stream is open and owned by us (i.e. it wasn't instantiated via the 
	@c AEStreamRef constructor), then it is closed and any generated output is discarded.
*/
AEWriter::~AEWriter()
{
	OSStatus	err;
	
	if ((mStream != NULL) && mOwned)
	{
		err = AEStreamClose(mStream, NULL);
	}
}

// ------------------------------------------------------------------------------------------
/*!	If the stream is already open, it is closed then re-opened.
*/
void
AEWriter::Open()
{
	if (IsOpen())
		Close();
	
	mStream = AEStreamOpen();
	B_THROW_IF_NULL(mStream);
	
	mOwned = true;
}

// ------------------------------------------------------------------------------------------
/*!	If the stream is already open, it is closed then re-opened.
*/
void
AEWriter::Open(
	AEEventClass	clazz,										//!< The Apple %Event's event class.
	AEEventID		id,											//!< The Apple %Event's event ID.
	short			returnID /* = kAutoGenerateReturnID */,		//!< The return ID, used to pair up events with replies.
	long			transactionID /* = kAnyTransactionID */)	//!< The transaction ID, used to track a series of related events.
{
	if (IsOpen())
		Close();
	
	ProcessSerialNumber	psn	= { 0, kCurrentProcess };
	
	mStream = AEStreamCreateEvent(clazz, id, 
								  typeProcessSerialNumber, &psn, sizeof(psn), 
								  returnID, transactionID);
	B_THROW_IF_NULL(mStream);
	
	mOwned = true;
}

// ------------------------------------------------------------------------------------------
/*!	If the stream is already open, it is closed then re-opened.
*/
void
AEWriter::Open(
	AppleEvent&		event)	//!< The Apple %Event to append to.
{
	if (IsOpen())
		Close();
	
	mStream = AEStreamOpenEvent(&event);
	B_THROW_IF_NULL(mStream);
	
	mOwned = true;
}

// ------------------------------------------------------------------------------------------
void
AEWriter::Close(
	AEDesc&	outDesc)	//!< The Apple %Event descriptor result.
{
	B_ASSERT(mStream != NULL);
	B_ASSERT(mOwned);
	
	OSStatus	err;
	
	err = AEStreamClose(mStream, &outDesc);
	B_THROW_IF_STATUS(err);
	
	mStream	= NULL;
	mOwned	= false;
}

// ------------------------------------------------------------------------------------------
void
AEWriter::Close(
	std::vector<UInt8>&	outBuff)	//!< The flattened (i.e., serialised) Apple %Event descriptor result.
{
	AEDescriptor	desc;
	::Size			descSize;
	OSStatus		err;
	
	Close(desc);
	
	descSize = AESizeOfFlattenedDesc(desc);
	
	outBuff.resize(descSize);
	
	err = AEFlattenDesc(desc, reinterpret_cast<Ptr>(&outBuff[0]), descSize, &descSize);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
AEWriter::Close()
{
	B_ASSERT(mStream != NULL);
	B_ASSERT(mOwned);
	
	OSStatus	err;
	
	err = AEStreamClose(mStream, NULL);
	// ignore the error
	
	mStream	= NULL;
	mOwned	= false;
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor or an element of a descriptor
	list.
	
	If the stream itself isn't already open, it is opened via Open().
*/
void
AEWriter::OpenDesc(
	DescType	type)	//!< The descriptor's type.
{
	OSStatus	err;
	
	if (mStream == NULL)
		Open();
	
	FlushNextKeyword();
	
	err = AEStreamOpenDesc(mStream, type);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
void
AEWriter::OpenDescKey(
	AEKeyword	key,	//!< The descriptor's keyword within its enclosing @c AERecord.
	DescType	type)	//!< The descriptor's type.
{
	B_ASSERT(mNextKeyword == 0);
	
	OSStatus	err;
	
	if (mStream == NULL)
		Open();
	
	err = AEStreamWriteKey(mStream, key);
	B_THROW_IF_STATUS(err);
	err = AEStreamOpenDesc(mStream, type);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	It's an error to call this function when there isn't a currently opened descriptor.
*/
void
AEWriter::CloseDesc()
{
	B_ASSERT(mStream != NULL);
	
	OSStatus	err;
	
	err = AEStreamCloseDesc(mStream);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor or an element of a descriptor
	list.
	
	If the stream itself isn't already open, it is opened via Open().
*/
void
AEWriter::OpenRecord(
	DescType	type /* = typeAERecord */)	//!< The descriptor's type.
{
	OSStatus	err;
	
	if (mStream == NULL)
		Open();
	
	FlushNextKeyword();
	
	err = AEStreamOpenRecord(mStream, type);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
void
AEWriter::OpenRecordKey(
	AEKeyword	key,						//!< The descriptor's keyword within its enclosing @c AERecord.
	DescType	type /* = typeAERecord */)	//!< The descriptor's type.
{
	B_ASSERT(mNextKeyword == 0);
	
	OSStatus	err;
	
	if (mStream == NULL)
		Open();
	
	err = AEStreamWriteKey(mStream, key);
	B_THROW_IF_STATUS(err);
	err = AEStreamOpenRecord(mStream, type);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	It's an error to call this function when there isn't a currently opened descriptor 
	record.
*/
void
AEWriter::CloseRecord()
{
	B_ASSERT(mStream != NULL);
	
	OSStatus	err;
	
	FlushNextKeyword();
	
	err = AEStreamCloseRecord(mStream);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor or an element of a descriptor list.
	
	If the stream itself isn't already open, it is opened via Open().
*/
void
AEWriter::OpenList()
{
	OSStatus	err;
	
	if (mStream == NULL)
		Open();
	
	FlushNextKeyword();
	
	err = AEStreamOpenList(mStream);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
void
AEWriter::OpenListKey(
	AEKeyword	key)	//!< The descriptor's keyword within its enclosing @c AERecord.
{
	B_ASSERT(mNextKeyword == 0);
	
	OSStatus	err;
	
	if (mStream == NULL)
		Open();
	
	err = AEStreamWriteKey(mStream, key);
	B_THROW_IF_STATUS(err);
	err = AEStreamOpenList(mStream);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	It's an error to call this function when there isn't a currently opened descriptor 
	list.
*/
void
AEWriter::CloseList()
{
	B_ASSERT(mStream != NULL);
	
	OSStatus	err;
	
	err = AEStreamCloseList(mStream);
	B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor.
*/
AEWriter&
AEWriter::Append(
	const void*	inPtr, 	//!< The start of the data to write.
	size_t		inSize)	//!< The number of bytes to write.
{
	B_ASSERT(mStream != NULL);
	
	OSStatus	err;
	
	err = AEStreamWriteData(mStream, inPtr, inSize);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
AEWriter&
AEWriter::WriteObject(
	const AEObject&	value)
{
	B_ASSERT(mStream != NULL);
	
	FlushNextKeyword();
	DescParam<typeObjectSpecifier>::Write(mStream, value);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor or an element of a descriptor
	list.
*/
AEWriter&
AEWriter::WriteDesc(
	const AEDesc&	value)	//!< The Apple %Event descriptor to copy verbatim into the stream.
{
	B_ASSERT(mStream != NULL);
	
	OSStatus	err;
	
	FlushNextKeyword();
	
	err = AEStreamWriteAEDesc(mStream, &value);
	B_THROW_IF_STATUS(err);
	
	return (*this);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an Apple %Event.
*/
void
AEWriter::Send(
	AESendMode	inMode /* = 0 */)	//!< Flags that control how the event is sent.
{
	AEDescriptor	event;
	
	Close(event);
	
	AEEventBase::SendEvent(event, inMode);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an Apple %Event.
*/
void
AEWriter::Send(
	AEDesc&		outResult,			//!< The result returned by the event's recipient.
	AESendMode	inMode /* = 0 */)	//!< Flags that control how the event is sent.
{
	AEDescriptor	event;
	
	Close(event);
	
	AEEventBase::SendEvent(event, outResult, inMode);
}


// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an Apple %Event.
*/
void
AEWriter::OptionalParam(
	AEKeyword	key)	//!< The Apple %Event parameter being tagged as optional.
{
	B_ASSERT(mStream != NULL);
	
	OSStatus	err;
	
	err = AEStreamOptionalParam(mStream, key);
	B_THROW_IF_STATUS(err);
}


// ------------------------------------------------------------------------------------------
void
AEWriter::FlushNextKeyword()
{
	if (mNextKeyword != 0)
	{
		OSStatus	err;
		
		err = AEStreamWriteKey(mStream, mNextKeyword);
		B_THROW_IF_STATUS(err);
		
		mNextKeyword = 0;
	}
}


// ==========================================================================================
//	AutoAEWriterDesc

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor or an element of a descriptor list.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterDesc::AutoAEWriterDesc(AEWriter& writer, DescType type)
	: mWriter(writer)
{
	mWriter.OpenDesc(type);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterDesc::AutoAEWriterDesc(AEWriter& writer, AEKeyword key, DescType type)
	: mWriter(writer)
{
	mWriter.OpenDescKey(key, type);
}

// ------------------------------------------------------------------------------------------
AutoAEWriterDesc::~AutoAEWriterDesc()
{
	mWriter.CloseDesc();
}


// ==========================================================================================
//	AutoAEWriterRecord

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterRecord::AutoAEWriterRecord(AEWriter& writer)
	: mWriter(writer)
{
	mWriter.OpenRecord(typeAERecord);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterRecord::AutoAEWriterRecord(AEWriter& writer, DescType type)
	: mWriter(writer)
{
	mWriter.OpenRecord(type);
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterRecord::AutoAEWriterRecord(AEWriter& writer, DescType type, AEKeyword key)
	: mWriter(writer)
{
	mWriter.OpenRecordKey(key, type);
}

// ------------------------------------------------------------------------------------------
AutoAEWriterRecord::~AutoAEWriterRecord()
{
	mWriter.CloseRecord();
}


// ==========================================================================================
//	AutoAEWriterList

#pragma mark -

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing a single descriptor or an element of a descriptor list.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterList::AutoAEWriterList(AEWriter& writer)
	: mWriter(writer)
{
	mWriter.OpenList();
}

// ------------------------------------------------------------------------------------------
/*!	Use this function when writing an element of a descriptor record.
	
	If the stream itself isn't already open, it is opened via Open().
*/
AutoAEWriterList::AutoAEWriterList(AEWriter& writer, AEKeyword key)
	: mWriter(writer)
{
	mWriter.OpenListKey(key);
}

// ------------------------------------------------------------------------------------------
AutoAEWriterList::~AutoAEWriterList()
{
	mWriter.CloseList();
}


// ==========================================================================================
//	AEUrlTarget

#pragma mark -

// ------------------------------------------------------------------------------------------
AEUrlTarget::AEUrlTarget(const Url& inUrl)
{
	inUrl.Copy(mData, true);
}


}	// namespace B
