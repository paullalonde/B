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

#ifndef BAEWriter_H_
#define BAEWriter_H_

#pragma once

// standard headers
#include <functional>
#include <vector>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BAEDescParam.h"


namespace B {

// forward declarations
class   Url;


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//  AEWriterTargetConcept

#pragma mark AEWriterTargetConcept

/*!
    @brief  Concept check for objects that can be passed to the TARGET constructor 
            of AEWriter.
*/
template <class TARGET> struct AEWriterTargetConcept
{
    void    constraints()
    {
        mType   = mTarget.Type();
        mData   = mTarget.Data();
        mSize   = mTarget.Size();
    }
    
    const TARGET    mTarget;
    DescType        mType;
    const void*     mData;
    size_t          mSize;
};

#endif  // DOXYGEN_SKIP


// ==========================================================================================
//  AEWriter

#pragma mark -
#pragma mark AEWriter

/*!
    @brief  Encapsulates a writable Apple %Event descriptor "stream".  It's basically a 
            wrapper around @c AEStreamRef.
    
    Reading Apple's documentation for AEStreamRef will probably go a long way toward 
    clarifying how this class should be used.  Here's where to get it:
    
    http://developer.apple.com/technotes/tn/tn2046.html
    
    @ingroup    AppleEvents
*/
class AEWriter : public boost::noncopyable
{
public:
    
    //! @name Constructors / Destructor
    //@{
    //! Default constructor.  Opens the stream for writing a single descriptor.
    AEWriter();
    //! Constructor that opens the stream for writing an Apple %Event targeting the current process.
    AEWriter(
        AEEventClass    clazz,
        AEEventID       id,
        short           returnID = kAutoGenerateReturnID,
        long            transactionID = kAnyTransactionID);
    //! Constructor that opens the stream for writing an Apple %Event.
    template <typename TARGET>
    AEWriter(
        AEEventClass    clazz,
        AEEventID       id,
        TARGET          target,
        short           returnID = kAutoGenerateReturnID,
        long            transactionID = kAnyTransactionID);
    //! Constructor that opens the stream for appending to an existing Apple %Event.
    AEWriter(
        AppleEvent&     event);
    //! Constructor that opens the stream on an existing @c AEStreamRef.
    AEWriter(
        AEStreamRef     stream);
    //! Destructor.
    ~AEWriter();
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns whether the stream is open or not.
    bool    IsOpen() const      { return (mStream != NULL); }
    //@}
    
    //! @name Opening & Closing The Stream
    //@{
    //! Opens the stream for writing a single descriptor.
    void    Open();
    //! Opens the stream for writing an Apple %Event targeting the current process.
    void    Open(
                AEEventClass    clazz,
                AEEventID       id,
                short           returnID = kAutoGenerateReturnID,
                long            transactionID = kAnyTransactionID);
    //! Opens the stream for writing an Apple %Event.
    template <typename TARGET>
    void    Open(
                AEEventClass    clazz,
                AEEventID       id,
                TARGET          target,
                short           returnID = kAutoGenerateReturnID,
                long            transactionID = kAnyTransactionID);
    //! Opens the stream for appending to an existing Apple %Event.
    void    Open(
                AppleEvent&     event);
    //! Closes the stream, returning its output in @a outDesc.
    void    Close(AEDesc& outDesc);
    //! Closes the stream, returning a flattened form of the output descriptor in @a outBuff.
    void    Close(std::vector<UInt8>& outBuff);
    //! Closes the stream, discarding any output.
    void    Close();
    //@}
    
    /*! @name Opening & Closing Descriptors
        @note You need to open the stream before you can open a descriptor.
    */
    //@{
    //! Opens a descriptor of type @a type.
    void    OpenDesc(DescType type);
    //! Opens a descriptor of type @a type, identified by @a key.
    void    OpenDescKey(AEKeyword key, DescType type);
    //! Closes the currently open descriptor.
    void    CloseDesc();
    //@}
    
    /*! @name Opening & Closing Records
        @note You need to open the stream before you can open a record.
    */
    //@{
    // opening & closing records
    //! Opens a descriptor record of type @a type.
    void    OpenRecord(DescType type = typeAERecord);
    //! Opens a descriptor record of type @a type, identified by @a key.
    void    OpenRecordKey(AEKeyword key, DescType type = typeAERecord);
    //! Closes the currently open descriptor record.
    void    CloseRecord();
    //@}
    
    /*! @name Opening & Closing Lists
        @note You need to open the stream before you can open a list.
    */
    //@{
    //! Opens a descriptor list.
    void    OpenList();
    //! Opens a descriptor list identified by @a key.
    void    OpenListKey(AEKeyword key);
    //! Closes the currently open descriptor list.
    void    CloseList();
    //@}
    
    //! @name Writing Values
    //@{
    //! Writes a single value as a descriptor in a type-safe manner.
    template <DescType TYPE>
    AEWriter&   Write(const typename DescParam<TYPE>::ValueType& value);
    //! Writes the given AEObject as an object specifier.
    AEWriter&   WriteObject(const AEObject& value);
    //! Copies @a value into the stream.
    AEWriter&   WriteDesc(const AEDesc& value);
    //!< Appends the binary blob identified by @a inPtr and @a inSize to the currently open descriptor.
    AEWriter&   Append(const void* inPtr, size_t inSize);
    //@}
    
    //! @name Sending Events
    //@{
    // Closes the stream, then sends it.
    void        Send(AESendMode inMode = 0);
    // Closes the stream, then sends it and waits for a reply.
    void        Send(AEDesc& outResult, AESendMode inMode = 0);
    //@}
    
    //! @name Keywords
    //@{
    //! Caches @a key until the next time a descriptor is written.
    void        CacheNextKeyword(AEKeyword key)     { mNextKeyword = key; }
    //! Writes out the currently cached @c AEKeyword, if any.
    void        FlushNextKeyword();
    //!< Tags the keyword @a key as being optional.
    void        OptionalParam(AEKeyword key);
    //@}
    
private:
    
    // member variables
    AEStreamRef mStream;        //!< The underlying OS stream.
    bool        mOwned;         //!< Was the stream created by us?
    AEKeyword   mNextKeyword;   //!< Cached keyword.
    
    // friends
    friend class    AETypedObjectBase;
};

// ------------------------------------------------------------------------------------------
/*! The @a target argument allows the caller complete flexibility in specifying the 
    recipient of the event.
    
    Consult the Apple %Event Manager documentation for more information.
*/
template <typename TARGET>
AEWriter::AEWriter(
    AEEventClass    clazz,                                      //!< The Apple %Event's event class.
    AEEventID       id,                                         //!< The Apple %Event's event ID.
    TARGET          target,                                     //!< The event's receiving process.
    short           returnID /* = kAutoGenerateReturnID */,     //!< The return ID, used to pair up events with replies.
    long            transactionID /* = kAnyTransactionID */)    //!< The transaction ID, used to track a series of related events.
        : mStream(NULL), mNextKeyword(0)
{
    Open(clazz, id, target, returnID, transactionID);
}

// ------------------------------------------------------------------------------------------
/*! If the stream is already open, it is closed then re-opened.
    
    The @a target argument allows the caller complete flexibility in specifying the 
    recipient of the event.
    
    Consult the Apple %Event Manager documentation for more information.
*/
template <typename TARGET> void
AEWriter::Open(
    AEEventClass    clazz,                                      //!< The Apple %Event's event class.
    AEEventID       id,                                         //!< The Apple %Event's event ID.
    TARGET          target,                                     //!< The event's receiving process.
    short           returnID /* = kAutoGenerateReturnID */,     //!< The return ID, used to pair up events with replies.
    long            transactionID /* = kAnyTransactionID */)    //!< The transaction ID, used to track a series of related events.
{
    boost::function_requires< AEWriterTargetConcept<TARGET> >();
    
    if (IsOpen())
        Close();
    
    mStream = AEStreamCreateEvent(clazz, id, 
                                  target.Type(), target.Data(), target.Size(), 
                                  returnID, transactionID);
    B_THROW_IF_NULL(mStream);
    
    mOwned = true;
}

// ------------------------------------------------------------------------------------------
/*! The language type of @a value determines the descriptor type that is used, via the 
    magic of DescParam<TYPE>.
    
    Use this function when writing a single descriptor or to an element of a descriptor
    list.
*/
template <DescType TYPE> inline AEWriter&
AEWriter::Write(
    const typename DescParam<TYPE>::ValueType&  value)  //!< The value to write.
{
    FlushNextKeyword();
    DescParam<TYPE>::Write(mStream, value);
    
    return (*this);
}


// ==========================================================================================
//  AutoAEWriterDesc

#pragma mark -
#pragma mark AutoAEWriterDesc

/*! @brief  A stack-based class for opening a descriptor.
    
    The constructor opens the descriptor, the destructor closes it.
    
    @ingroup    AppleEvents
*/
class AutoAEWriterDesc
{
public:
    
    //! Opens a descriptor of type @a type.
    AutoAEWriterDesc(AEWriter& writer, DescType type);
    //! Opens a descriptor of type @a type, identified by @a key.
    AutoAEWriterDesc(AEWriter& writer, AEKeyword key, DescType type);
    //! Destructor.
    ~AutoAEWriterDesc();
    
private:
    
    AEWriter&   mWriter;
};


// ==========================================================================================
//  AutoAEWriterRecord

#pragma mark -
#pragma mark AutoAEWriterRecord

/*! @brief  A stack-based class for opening a descriptor record.
    
    The constructor opens the descriptor record, the destructor closes it.
    
    @ingroup    AppleEvents
*/
class AutoAEWriterRecord
{
public:
    
    //! Opens a descriptor record.
    AutoAEWriterRecord(AEWriter& writer);
    //! Opens a descriptor record of type @a type.
    AutoAEWriterRecord(AEWriter& writer, DescType type);
    //! Opens a descriptor record of type @a type, identified by @a key.
    AutoAEWriterRecord(AEWriter& writer, DescType type, AEKeyword key);
    //! Destructor.
    ~AutoAEWriterRecord();
    
private:
    
    AEWriter&   mWriter;
};


// ==========================================================================================
//  AutoAEWriterList

#pragma mark -
#pragma mark AutoAEWriterList

/*! @brief  A stack-based class for opening a descriptor list.
    
    The constructor opens the descriptor list, the destructor closes it.
    
    @ingroup    AppleEvents
*/
class AutoAEWriterList
{
public:
    
    //! Opens a descriptor list of type @a type.
    AutoAEWriterList(AEWriter& writer);
    //! Opens a descriptor list of type @a type, identified by @a key.
    AutoAEWriterList(AEWriter& writer, AEKeyword key);
    //! Destructor.
    ~AutoAEWriterList();
    
private:
    
    AEWriter&   mWriter;
};


// ==========================================================================================
//  AESelfTarget

#pragma mark -
#pragma mark AESelfTarget

/*! @brief  Functor for sending Apple %Events to the current process.
    
    This class is suitable for use as the @a TARGET parameter of AEWriter's constructor.
    It indicates that the Apple %Event should be dispatched using the "quick-dispatch" 
    method (i.e. a direct call to the event's handler, without going through the event 
    loop).
    
    @ingroup    AppleEvents
*/
class AESelfTarget
{
public:
    
    AESelfTarget()
        {
            mPSN.highLongOfPSN  = 0;
            mPSN.lowLongOfPSN   = kCurrentProcess;
        }
    
    DescType    Type() const    { return (typeProcessSerialNumber); }
    const void* Data() const    { return (&mPSN); }
    size_t      Size() const    { return (sizeof(mPSN)); }
    
private:
    
    // member variables
    ProcessSerialNumber mPSN;
};


// ==========================================================================================
//  AEPsnTarget

#pragma mark -
#pragma mark AEPsnTarget

/*! @brief  Functor for sending Apple %Events to an arbitrary (Carbon or Classic) process.
    
    This class is suitable for use as the @a TARGET parameter of AEWriter's constructor.  
    You can target any app that has a Process Manager connection;  usually that means 
    Classic, Carbon and Cocoa (but not BSD-style) applications.
    
    @ingroup    AppleEvents
*/
class AEPsnTarget
{
public:
    
    AEPsnTarget(const ProcessSerialNumber& inPSN) : mPSN(inPSN) {}
    
    DescType    Type() const    { return (typeProcessSerialNumber); }
    const void* Data() const    { return (&mPSN); }
    size_t      Size() const    { return (sizeof(mPSN)); }
    
private:
    
    // member variables
    const ProcessSerialNumber   mPSN;
};


// ==========================================================================================
//  AEPidTarget

#pragma mark -
#pragma mark AEPidTarget

/*! @brief  Functor for sending Apple %Events to an arbitrary UNIX process.
    
    This class is suitable for use as the @a TARGET parameter of AEWriter's constructor.
    You can target any true process on the machine with this target, however you can't 
    target apps running in Classic.
    
    @ingroup    AppleEvents
*/
class AEPidTarget
{
public:
    
    AEPidTarget(pid_t pid) : mPid(pid) {}
    
    DescType    Type() const    { return (typeKernelProcessID); }
    const void* Data() const    { return (&mPid); }
    size_t      Size() const    { return (sizeof(mPid)); }
    
private:
    
    // member variables
    const pid_t mPid;
};


// ==========================================================================================
//  AEUrlTarget

#pragma mark -
#pragma mark AEUrlTarget

/*! @brief  Functor for sending Apple %Events to a URL.
    
    This class is suitable for use as the @a TARGET parameter of AEWriter's constructor.
    It indicates that the Apple %Event should be dispatched to the given URL, such as 
    when issuing XML-RPC or SOAP requests.
    
    @ingroup    AppleEvents
*/
class AEUrlTarget
{
public:
    
    AEUrlTarget(const Url& inUrl);
    
    DescType    Type() const    { return (typeApplicationURL); }
    const void* Data() const    { return (mData.c_str()); }
    size_t      Size() const    { return (mData.size()); }
    
private:
    
    // member variables
    std::string mData;
};


// ==========================================================================================
//  AEKey

#pragma mark -
#pragma mark AEKey

/*! @brief  Functor for writing Apple %Event keywords to an AEWriter.
    
    Encapsulates an Apple %Event keyword for <tt>operator << </tt>.
    In the following example, an Apple %Event descriptor whose data is given 
    by @a my_value is written to the stream with a keyword of @a kMyKeyword :
    
    @code
        
        AEWriter    writer(...);
        
        writer << AEKey(kMyKeyword) << myValue;
        
    @endcode
    
    @note   The keyword isn't written immediately to the AEWriter, but rather 
            stored for later.  It actually gets written when descriptor data 
            is written, or when a descriptor/record/list is opened.
    
    @ingroup    AppleEvents
*/
class AEKey : public std::unary_function<AEWriter, AEWriter>
{
public:
    
    //! Constructor.  Saves @a keyword and @a obj into the object for later recall.
    explicit    AEKey(AEKeyword keyword)
                    : mKeyword(keyword) {}
    
    //! Tells @a writer to record our keyword.
    AEWriter&   operator () (AEWriter& writer) const
                    { writer.CacheNextKeyword(mKeyword);  return (writer); }
    
private:
    
    // member variables
    AEKeyword   mKeyword;
};


// ==========================================================================================
//  AETypedObject

#pragma mark -
#pragma mark AETypedObject

/*! @brief  Functor for writing tagged data to an AEWriter.
    
    Encapsulates the data for an Apple %Event descriptor, where type descriptor 
    type is given by the @a TYPE template parameter.  See TaggedTypeTrait for 
    more information.
    
    AETypedObject is especially useful for writing descriptors whose underlying 
    C/C++ language type may cause ambiguity.  For example, descriptors of type 
    @c typeEnumerated and @c typeType contain an @c OSType, so overloading 
    <tt> operator << (AEWriter& writer, OSType value)</tt> would be insufficient 
    for distinguishing the two cases.  AETypedObject resolves that ambiguity.
    The following example shows how to use the class:
    
    @code
        AEWriter writer(...);
        OSType   myValue;
        
        writer << AETypedObject<typeType>(myValue);
    @endcode
    
    @ingroup    AppleEvents
*/
template <DescType TYPE> class AETypedObject : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef DescType    TYPE;       //!< The template parameter.  Should be an integral four-char constant (a @c DescType).
    //@}
#endif
    
    typedef typename DescParam<TYPE>::ValueType ValueType;  //!< The language type.  See TaggedTypeTrait.
    
    //! Constructor.  Saves @a keyword and @a obj into the object for later recall.
    explicit    AETypedObject(const ValueType& value);
    
    //! Returns the object's value.
    const ValueType&    GetValue() const;
    
    //! Appends the object's value to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    
    // member variables
    ValueType   mValue;
};

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline
AETypedObject<TYPE>::AETypedObject(const ValueType& value)
    : mValue(value)
{
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline const typename AETypedObject<TYPE>::ValueType&
AETypedObject<TYPE>::GetValue() const
{
    return (mValue);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline AEWriter&
AETypedObject<TYPE>::operator () (AEWriter& writer) const
{
    return (writer.template Write<TYPE>(mValue));
}


// ==========================================================================================
//  AEOptionalTypedObject

#pragma mark -
#pragma mark AEOptionalTypedObject

/*! @brief  Functor for optionally writing tagged data to an AEWriter.
    
    This class refines AETypedObject in that the tagged data is only written if it 
    is @b not equal to a given default value.  It's useful when writing Apple %Events
    that take optional parameters.
    
    @ingroup    AppleEvents
*/
template <DescType TYPE> class AEOptionalTypedObject : private AETypedObject<TYPE>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef DescType    TYPE;       //!< The template parameter.  Should be an integral four-char constant (a @c DescType).
    //@}
#endif
    
    typedef typename DescParam<TYPE>::ValueType ValueType;  //!< The language type.  See TaggedTypeTrait.
    
    //! Constructor.  Saves @a keyword and @a obj into the object for later recall.
    explicit    AEOptionalTypedObject(
                    const ValueType&    inValue, 
                    const ValueType&    inDefaultValue);
    
    //! Returns @c true if the value isn't the default value.
    bool        HasValue() const;
    
    //! Appends the object's value to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    
    // member variables
    ValueType   mDefaultValue;
};

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline
AEOptionalTypedObject<TYPE>::AEOptionalTypedObject(
    const ValueType&    inValue, 
    const ValueType&    inDefaultValue)
        : AETypedObject<TYPE>(inValue), 
          mDefaultValue(inDefaultValue)
{
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline bool
AEOptionalTypedObject<TYPE>::HasValue() const
{
    return (AETypedObject<TYPE>::GetValue() != mDefaultValue);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> AEWriter&
AEOptionalTypedObject<TYPE>::operator () (AEWriter& writer) const
{
    if (HasValue())
        AETypedObject<TYPE>::operator () (writer);
    else
        writer.CacheNextKeyword(0);
    
    return (writer);
}


// ==========================================================================================
//  Global Functions

#pragma mark -

/*! @addtogroup AppleEvents
    @{
*/

// ------------------------------------------------------------------------------------------
/*! Writes the Apple %Event descriptor @a desc to the stream contained in @a writer.
    
    @relates    AEWriter
*/
inline AEWriter&
operator << (AEWriter& writer, const AEDesc& desc)
{
    return (writer.WriteDesc(desc));
}

// ------------------------------------------------------------------------------------------
/*! Writes a String to the stream contained in @a writer.
    
    @relates    AEWriter
*/
inline AEWriter&
operator << (AEWriter& writer, const String& value)
{
    return (writer.Write<typeUTF16ExternalRepresentation>(value));
}

// ------------------------------------------------------------------------------------------
/*! Writes a Url to the stream contained in @a writer.
    
    @relates    AEWriter
*/
inline AEWriter&
operator << (AEWriter& writer, const Url& value)
{
    return (writer.Write<typeFileURL>(value));
}

// ------------------------------------------------------------------------------------------
/*! Writes the Apple %Event keyword contained in @a key to the stream contained in @a writer.
    
    @relates    AEWriter
    @relates    AEKey
*/
inline AEWriter&
operator << (AEWriter& writer, const AEKey& key)
{
    return (key(writer));
}

// ------------------------------------------------------------------------------------------
/*! Writes the Apple %Event descriptor contained in @a value to the stream contained in @a writer.
    
    @relates    AEWriter
    @relates    AETypedObject
*/
template <DescType TYPE> inline AEWriter&
operator << (AEWriter& writer, const AETypedObject<TYPE>& value)
{
    return (value(writer));
}

// ------------------------------------------------------------------------------------------
/*! Writes the Apple %Event descriptor contained in @a value to the stream contained in @a writer.
    
    @relates    AEWriter
    @relates    AEOptionalTypedObject
*/
template <DescType TYPE> inline AEWriter&
operator << (AEWriter& writer, const AEOptionalTypedObject<TYPE>& value)
{
    return (value(writer));
}

//@}    group AppleEvents

}   // namespace B



#endif  // BAEWriter_H_
