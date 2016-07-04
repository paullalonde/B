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

/*! @file   BAEDescriptor.h
    @brief  <tt>AEDesc</tt> wrapper classes
*/

#ifndef BAEDescriptor_H_
#define BAEDescriptor_H_

#pragma once

// standard headers
#include <iosfwd>

// library headers
#include "BAEDescParam.h"


namespace B {

// ==========================================================================================
//  AEDescriptor

#pragma mark AEDescriptor

/*!
    @brief  Wrapper class around an Apple %Event descriptor.
    
    This class provides a simple way of controlling the lifetime of an Apple %Event 
    descriptor.  Its constructors allocate the descriptor, and its destructor 
    releases it.  It's really that simple.
    
    @ingroup    AppleEvents
*/
class AEDescriptor
{
public:
    
    //! @name Constructors / Destructor
    //@{
    //! Default constructor.  Initialises the descriptor to @c typeNull.
    AEDescriptor();
    //! Copy constructor.
    AEDescriptor(const AEDescriptor &inOriginal);
    //! Copy constructor.
    AEDescriptor(DescType inType, const void* inPtr, size_t inSize);
    //! Destructor.
    ~AEDescriptor();
    //@}
    
    //! @name Assignment
    //@{
    //! Copy assignment.
    AEDescriptor&   operator = (const AEDescriptor &inOriginal);
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns the descriptor's type.
    DescType    GetType() const { return (mDesc.descriptorType); }
    //! Returns @c true if the descriptor is of type @c typeNull.
    bool        Empty() const   { return (mDesc.descriptorType == typeNull); }
    //@}
    
    //! @name Modifiers
    //@{
    //! Resets the descriptor's contents.
    void        Clear();
    //! Exchanges the contents of two descriptors.
    void        swap(AEDescriptor& ioDescriptor);
    //@}
    
    //! @name Conversions
    //@{
    //! Returns the address of the object's embedded @c AEDesc.
            operator AEDesc* ()             { return &mDesc; }
    //! Returns a reference to the object's embedded @c AEDesc.
            operator AEDesc& ()             { return mDesc; }
    //! Returns @c true if the descriptor is of type @c typeNull.
    bool    operator ! () const             { return (mDesc.descriptorType == typeNull); }
    //@}
    
protected:
    
    AEDesc  mDesc;  //!< The embedded Apple %Event descriptor.
};

// ------------------------------------------------------------------------------------------
inline
AEDescriptor::AEDescriptor()
{
    AEInitializeDescInline(&mDesc);
}

// ------------------------------------------------------------------------------------------
inline void
AEDescriptor::swap(AEDescriptor& ioDescriptor)
{
    std::swap(mDesc.descriptorType, ioDescriptor.mDesc.descriptorType);
    std::swap(mDesc.dataHandle,     ioDescriptor.mDesc.dataHandle);
}

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of two AEDescriptor objects.
    
    @exception  none
    @relates    AEDescriptor
    @ingroup    AppleEvents
*/
inline void swap(AEDescriptor& desc1, AEDescriptor& desc2)
{
    desc1.swap(desc2);
}


// ==========================================================================================
//  AETypedDescriptor

#pragma mark -
#pragma mark AETypedDescriptor

/*!
    @brief  Type-safe wrapper class around an Apple %Event descriptor.
    
    This class provides a simple type-safe way of controlling the lifetime of an 
    Apple %Event descriptor.  Its constructors allocate the descriptor, and its 
    destructor releases it.  It's really that simple.
    
    @ingroup    AppleEvents
*/
template <DescType TYPE> class AETypedDescriptor : private AEDescriptor
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef DescType    TYPE;       //!< The template parameter.  Should be an integral four-char constant (a @c DescType).
    //@}
#endif
    
    //! @name Types
    //@{
    typedef typename TaggedTypeTrait<TYPE>::Type    ValueType;  //!< The language type.  See TaggedTypeTrait.
    //@}
    
    //! @name Constructors / Destructor
    //@{
    //! Default constructor.  Initialises the descriptor to @c typeNull.
    AETypedDescriptor();
    //! Copy constructor.
    AETypedDescriptor(const AETypedDescriptor &inOriginal);
    //! Value constructor.
    AETypedDescriptor(ValueType value);
    //@}
    
    //! @name Assignment
    //@{
    //! Copy assignment.
    AETypedDescriptor&  operator = (const AETypedDescriptor &inOriginal);
    //! Value assignment.
    AETypedDescriptor&  operator = (ValueType value);
    //@}
    
    //! @name Modifiers
    //@{
    //! Exchanges the contents of two descriptors.
    void        swap(AETypedDescriptor& ioDescriptor);
    //@}
    
    /*! @name Conversions
        
        CodeWarrior seems to have a problem with access declarations of operators, 
        so we just redefine the conversion operators here.  They're all trivial 
        anyway.
    */
    //@{
    //! Returns the address of the object's embedded @c AEDesc.
            operator AEDesc* ()             { return &mDesc; }
    //! Returns a reference to the object's embedded @c AEDesc.
            operator AEDesc& ()             { return mDesc; }
    //! Returns @c true if the descriptor is of type @c typeNull.
    bool    operator ! () const             { return (mDesc.descriptorType == typeNull); }
    //@}
    
    // Adjusting member access
    AEDescriptor::GetType;
    AEDescriptor::Empty;
    AEDescriptor::Clear;
};

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
AETypedDescriptor<TYPE>::AETypedDescriptor()
{
    DescParam<TYPE>::Put(mDesc, ValueType());
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
AETypedDescriptor<TYPE>::AETypedDescriptor(
    const AETypedDescriptor &inOriginal)
        : AEDescriptor(inOriginal)
{
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
AETypedDescriptor<TYPE>::AETypedDescriptor(
    ValueType   value)
{
    DescParam<TYPE>::Put(mDesc, value);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> AETypedDescriptor<TYPE>&
AETypedDescriptor<TYPE>::operator = (const AETypedDescriptor &inOriginal)
{
    AEDescriptor::operator = (inOriginal);
    return (*this);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> AETypedDescriptor<TYPE>&
AETypedDescriptor<TYPE>::operator = (ValueType value)
{
    AETypedDescriptor   temp(value);
    
    swap(temp);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE> inline void
AETypedDescriptor<TYPE>::swap(AETypedDescriptor& ioDescriptor)
{
    AEDescriptor::swap(ioDescriptor);
}

// ------------------------------------------------------------------------------------------
/*! Exchanges the contents of two AETypedDescriptor objects.
    
    @exception  none
    @relates    AETypedDescriptor
    @ingroup    AppleEvents
*/
template <DescType TYPE>
inline void swap(AETypedDescriptor<TYPE>& desc1, AETypedDescriptor<TYPE>& desc2)
{
    desc1.swap(desc2);
}


// ==========================================================================================
//  AEListIterator

class AEListIterator : public std::iterator<std::input_iterator_tag, 
                                            AEDesc, 
                                            ptrdiff_t, 
                                            AEDesc*,
                                            AEDesc&>
{
public:
    
    explicit    AEListIterator(
                    const AEDescList&       inList, 
                    DescType                inDescType,
                    size_t                  inIndex);
                AEListIterator(
                    const AEListIterator&   iter);
    
    const AEDesc&   operator * () const;
    const AEDesc*   operator -> () const;
    AEListIterator& operator ++ ();
    AEListIterator  operator ++ (int);
    size_t          size() const        { return mCount; }
    
private:
    
    AEListIterator(
        const AEListIterator&   iter, 
        size_t                  inIndex);

    static size_t   GetCount(const AEDescList& inList);
    
    // member variables
    const AEDescList&       mList;
    mutable AEDescriptor    mDesc;
    const size_t            mCount;
    const DescType          mDescType;
    size_t                  mIndex;
    
    // friends
    friend bool operator == (const AEListIterator& it1, const AEListIterator& it2);
    friend bool operator != (const AEListIterator& it1, const AEListIterator& it2);
};

// ------------------------------------------------------------------------------------------
inline
AEListIterator::AEListIterator(
    const AEListIterator&   iter)
        : mList(iter.mList), mCount(iter.mCount), mDescType(iter.mDescType), 
          mIndex(iter.mIndex)
{
}

// ------------------------------------------------------------------------------------------
inline
AEListIterator::AEListIterator(
    const AEListIterator&   iter, 
    size_t                  inIndex)
        : mList(iter.mList), mCount(iter.mCount), mDescType(iter.mDescType), 
          mIndex(inIndex)
{
}

// ------------------------------------------------------------------------------------------
inline AEListIterator&
AEListIterator::operator ++ ()
{
    ++mIndex;
    mDesc.Clear();
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline AEListIterator
AEListIterator::operator ++ (int)
{
    return AEListIterator(*this, mIndex++);
}

// ------------------------------------------------------------------------------------------
inline bool
operator == (const AEListIterator& it1, const AEListIterator& it2)
{
    return (it1.mIndex == it2.mIndex);
}

// ------------------------------------------------------------------------------------------
inline bool
operator != (const AEListIterator& it1, const AEListIterator& it2)
{
    return (it1.mIndex != it2.mIndex);
}

// ------------------------------------------------------------------------------------------
AEListIterator  MakeAEListBeginIterator(
                    const AEDescList&   inList, 
                    DescType            inDescType = typeWildCard);

// ------------------------------------------------------------------------------------------
AEListIterator  MakeAEListEndIterator(
                    const AEDescList&   inList);


}   // namespace B


// ==========================================================================================

/*! @addtogroup AppleEvents
    @{
*/

// ------------------------------------------------------------------------------------------
/*! Writes out a flattened representation of @a desc (as returned by @c AEFlattenDesc()) 
    to @a os.
    
    @relates    B::AEDescriptor
*/
std::ostream&   operator << (std::ostream& ostr, const AEDesc& desc);

// ------------------------------------------------------------------------------------------
/*! Reads in a flattened representation of an Apple %Event descriptor (as generated by 
    @c AEFlattenDesc()) and copies the resulting @c AEDesc to @a desc.
    
    @relates    B::AEDescriptor
*/
std::istream&   operator >> (std::istream& is, AEDesc& desc);

//@}    group AppleEvents

#endif  // BAEDescriptor
