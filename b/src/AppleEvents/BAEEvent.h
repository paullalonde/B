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

#ifndef BAEEvent_H_
#define BAEEvent_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/concept_check.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>

// B headers
#include "BAEDescriptor.h"
#include "BAEWriter.h"
#include "BString.h"
#include "BUrl.h"


namespace B {

// forward declarations
class   AEObject;
class   Printer;
class   PrintSettings;


/*! @brief  Functors for writing out Apple %Events.
    
    The classes in this namespace are general-purpose functors for passing to the 
    various instantiations of Event.
    
    @ingroup    AppleEvents
    @todo       Complete documentation
*/
namespace AEEventFunctor
{

#ifndef DOXYGEN_SKIP

// ==========================================================================================
//  AEWritableConcept

#pragma mark AEWritableConcept

/*!
    @brief  Concept check for classes that need to be writeable to an AEWriter.
*/
template <class FUNCTOR> struct AEWritableConcept
{
    void    constraints()
    {
        mWriterPtr = &(mWriter << mFunctor);
    }
    
    AEWriter&   mWriter;
    AEWriter*   mWriterPtr;
    FUNCTOR     mFunctor;
};

#endif  // DOXYGEN_SKIP


// ==========================================================================================
//  InsertionLocation

#pragma mark -
#pragma mark InsertionLocation

/*!
    @brief  Functor that holds an insertion location.
    
    This functor is meant to be passed as the @a inInsertionLoc argument to 
    AEEvent<kAECoreSuite, kAEClone>::Send(), 
    AEEvent<kAECoreSuite, kAECreateElement>::Send(), and
    AEEvent<kAECoreSuite, kAEMove>::Send().
    
    An insertion location is an Apple %Event record containing two elements: an object 
    specifier, and an enumerated value indicating a position relative to the object 
    specifier.
    
    Accordingly, this class is defined in terms of two template parameters, @a OBJ_SPEC 
    and @a POSITION, which must be functors capable of writing out the corresponding 
    elements to the Apple %Event record.
    
    @sa operator<<(AEWriter& writer, InsertionLocation value)
*/
template <typename OBJ_SPEC, typename POSITION>
class InsertionLocation : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    POSITION;   //!< Template Parameter.  A functor that generates an insertion position.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC, B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(POSITION, B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    InsertionLocation(OBJ_SPEC obj, POSITION position);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObject;    //!< A functor that generates an object specifier.
    POSITION    mPosition;  //!< A functor that generates an insertion position.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename POSITION>
InsertionLocation<OBJ_SPEC, POSITION>::InsertionLocation(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    POSITION    position)   //!< A functor that generates an insertion position.
        : mObject(obj), mPosition(position)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes to the object 
    specifier and position to the record.
*/
template <typename OBJ_SPEC, typename POSITION> AEWriter&
InsertionLocation<OBJ_SPEC, POSITION>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoRecord(writer, typeInsertionLoc);
    
    return (writer << AEKey(keyAEPosition) << mPosition 
                   << AEKey(keyAEObject)   << mObject);
}


// ------------------------------------------------------------------------------------------
/*! Writes out the InsertionLocation @a value to the AEWriter @a writer.
    
    @relates    InsertionLocation
*/
template <typename OBJ_SPEC, typename POSITION> inline AEWriter&
operator << (AEWriter& writer, InsertionLocation<OBJ_SPEC, POSITION> value)
{
    return (value(writer));
}


// ==========================================================================================
//  IndexSpecifier

#pragma mark -
#pragma mark IndexSpecifier

/*!
    @brief  Functor that holds an object specifier with key form <tt>formAbsolutePosition</tt>.
    
    This functor is meant to be passed to any of the arguments to instantiations of 
    AEEvent that take an object specifier.
    
    Three pieces of information are required to generate an object specifier with this 
    key form:  the containing object, the desired class of one of its elements, and the 
    index of that element.
    
    Accordingly, this class is defined in terms of three template parameters, @a OBJ_SPEC, 
    @a OBJ_CLASS and @a INDEX, which must be functors capable of writing out the 
    corresponding elements to the Apple %Event record.
    
    @sa operator<<(AEWriter& writer, IndexSpecifier value)
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename INDEX>
class IndexSpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    OBJ_CLASS;  //!< Template Parameter.  A functor that generates an object class.
    typedef typename    INDEX;      //!< Template Parameter.  A functor that generates an index.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(OBJ_CLASS, B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(INDEX,     B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    IndexSpecifier(OBJ_SPEC obj, OBJ_CLASS objClass, INDEX index);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObj;       //!< A functor that generates an object specifier.
    OBJ_CLASS   mClass;     //!< A functor that generates an object class.
    INDEX       mIndex;     //!< A functor that generates an index.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename OBJ_CLASS, typename INDEX>
IndexSpecifier<OBJ_SPEC, OBJ_CLASS, INDEX>::IndexSpecifier(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    OBJ_CLASS   objClass,   //!< A functor that generates an object class.
    INDEX       index)      //!< A functor that generates an index.
        : mObj(obj), mClass(objClass), mIndex(index)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes the class, 
    container specifier and index to the record.
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename INDEX> AEWriter&
IndexSpecifier<OBJ_SPEC, OBJ_CLASS, INDEX>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoRecord(writer, typeObjectSpecifier);
    
    return (writer << AEKey(keyAEDesiredClass) << mClass 
                   << AEKey(keyAEContainer)    << mObj 
                   << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formAbsolutePosition)
                   << AEKey(keyAEKeyData)      << mIndex);
}


// ------------------------------------------------------------------------------------------
/*! Writes out the IndexSpecifier @a value to the AEWriter @a writer.
    
    @relates    IndexSpecifier
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename INDEX> inline AEWriter&
operator << (AEWriter& writer, IndexSpecifier<OBJ_SPEC, OBJ_CLASS, INDEX> value)
{
    return (value(writer));
}


// ==========================================================================================
//  NameSpecifier

#pragma mark -
#pragma mark NameSpecifier

/*!
    @brief  Functor that holds an object specifier with key form <tt>formName</tt>.
    
    This functor is meant to be passed to any of the arguments to instantiations of 
    AEEvent that take an object specifier.
    
    Three pieces of information are required to generate an object specifier with this 
    key form:  the containing object, the desired class of one of its elements, and the 
    name of that element.
    
    Accordingly, this class is defined in terms of three template parameters, @a OBJ_SPEC, 
    @a OBJ_CLASS and @a NAME, which must be functors capable of writing out the 
    corresponding elements to the Apple %Event record.
    
    @sa operator<<(AEWriter& writer, NameSpecifier value)
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename NAME>
class NameSpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    OBJ_CLASS;  //!< Template Parameter.  A functor that generates an object class.
    typedef typename    NAME;       //!< Template Parameter.  A functor that generates a name.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(OBJ_CLASS, B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(NAME,      B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    NameSpecifier(OBJ_SPEC obj, OBJ_CLASS objClass, NAME name);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObj;       //!< A functor that generates an object specifier.
    OBJ_CLASS   mClass;     //!< A functor that generates an object class.
    NAME        mName;      //!< A functor that generates a name.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename OBJ_CLASS, typename NAME>
NameSpecifier<OBJ_SPEC, OBJ_CLASS, NAME>::NameSpecifier(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    OBJ_CLASS   objClass,   //!< A functor that generates an object class.
    NAME        name)       //!< A functor that generates a name.
        : mObj(obj), mClass(objClass), mName(name)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes the class, 
    container specifier and index to the record.
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename NAME> AEWriter&
NameSpecifier<OBJ_SPEC, OBJ_CLASS, NAME>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoRecord(writer, typeObjectSpecifier);
    
    return (writer << AEKey(keyAEDesiredClass) << mClass 
                   << AEKey(keyAEContainer)    << mObj
                   << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formName)
                   << AEKey(keyAEKeyData)      << mName);
}


// ------------------------------------------------------------------------------------------
/*! Writes out the NameSpecifier @a value to the AEWriter @a writer.
    
    @relates    NameSpecifier
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename NAME> inline AEWriter&
operator << (AEWriter& writer, NameSpecifier<OBJ_SPEC, OBJ_CLASS, NAME> value)
{
    return (value(writer));
}


// ==========================================================================================
//  UniqueIDSpecifier

#pragma mark -
#pragma mark UniqueIDSpecifier

/*!
    @brief  Functor that holds an object specifier with key form <tt>formUniqueID</tt>.
    
    This functor is meant to be passed to any of the arguments to instantiations of 
    AEEvent that take an object specifier.
    
    Three pieces of information are required to generate an object specifier with this 
    key form:  the containing object, the desired class of one of its elements, and the 
    unique id of that element.
    
    Accordingly, this class is defined in terms of three template parameters, @a OBJ_SPEC, 
    @a OBJ_CLASS and @a ID, which must be functors capable of writing out the 
    corresponding elements to the Apple %Event record.
    
    @sa operator<<(AEWriter& writer, UniqueIDSpecifier value)
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename ID>
class UniqueIDSpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    OBJ_CLASS;  //!< Template Parameter.  A functor that generates an object class.
    typedef typename    ID;         //!< Template Parameter.  A functor that generates a unique id.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(OBJ_CLASS, B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(ID,        B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    UniqueIDSpecifier(OBJ_SPEC obj, OBJ_CLASS objClass, ID id);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObj;       //!< A functor that generates an object specifier.
    OBJ_CLASS   mClass;     //!< A functor that generates an object class.
    ID          mID;        //!< A functor that generates a unique id.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename OBJ_CLASS, typename ID>
UniqueIDSpecifier<OBJ_SPEC, OBJ_CLASS, ID>::UniqueIDSpecifier(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    OBJ_CLASS   objClass,   //!< A functor that generates an object class.
    ID          id)         //!< A functor that generates a unique id.
        : mObj(obj), mClass(objClass), mID(id)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes the class, 
    container specifier and index to the record.
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename ID> AEWriter&
UniqueIDSpecifier<OBJ_SPEC, OBJ_CLASS, ID>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoRecord(writer, typeObjectSpecifier);
    
    return (writer << AEKey(keyAEDesiredClass) << mClass
                   << AEKey(keyAEContainer)    << mObj
                   << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formUniqueID)
                   << AEKey(keyAEKeyData)      << mID);
}


// ------------------------------------------------------------------------------------------
/*! Writes out the UniqueIDSpecifier @a value to the AEWriter @a writer.
    
    @relates    UniqueIDSpecifier
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename ID> inline AEWriter&
operator << (AEWriter& writer, UniqueIDSpecifier<OBJ_SPEC, OBJ_CLASS, ID> value)
{
    return (value(writer));
}


// ==========================================================================================
//  PropertySpecifier

#pragma mark -
#pragma mark PropertySpecifier

/*!
    @brief  Functor that holds an object specifier for a property of an object.
    
    This functor is meant to be passed to any of the arguments to instantiations of 
    AEEvent that take an object specifier.
    
    Two pieces of information are required to generate an object specifier with this 
    key form:  the object, the desired property within it.  Accordingly, this class 
    is defined in terms of two template parameters, @a OBJ_SPEC and @a PROP_ID, which 
    must be functors capable of writing out the corresponding elements to the 
    Apple %Event record.
    
    @sa operator<<(AEWriter& writer, PropertySpecifier value)
*/
template <typename OBJ_SPEC, typename PROP_ID>
class PropertySpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    PROP_ID;    //!< Template Parameter.  A functor that generates a property id.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(PROP_ID,   B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    PropertySpecifier(OBJ_SPEC obj, PROP_ID propID);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObj;       //!< A functor that generates an object specifier.
    PROP_ID     mPropID;    //!< A functor that generates a property id.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename PROP_ID>
PropertySpecifier<OBJ_SPEC, PROP_ID>::PropertySpecifier(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    PROP_ID     propID)     //!< A functor that generates a property id.
        : mObj(obj), mPropID(propID)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes the class, 
    container specifier and index to the record.
*/
template <typename OBJ_SPEC, typename PROP_ID> AEWriter&
PropertySpecifier<OBJ_SPEC, PROP_ID>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoRecord(writer, typeObjectSpecifier);
    
    return (writer << AEKey(keyAEDesiredClass) << AETypedObject<typeType>(cProperty)
                   << AEKey(keyAEContainer)    << mObj 
                   << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formPropertyID)
                   << AEKey(keyAEKeyData)      << mPropID);
}


// ------------------------------------------------------------------------------------------
/*! Writes out the PropertySpecifier @a value to the AEWriter @a writer.
    
    @relates    PropertySpecifier
*/
template <typename OBJ_SPEC, typename PROP_ID> inline AEWriter&
operator << (AEWriter& writer, PropertySpecifier<OBJ_SPEC, PROP_ID> value)
{
    return (value(writer));
}


// ==========================================================================================
//  ComparisonSpecifier

#pragma mark -
#pragma mark ComparisonSpecifier

/*!
    @brief  Functor that holds an object specifier for a comparison.
    
    This functor is meant to be passed to any of the arguments to instantiations of 
    AEEvent that take an object specifier.
    
    @sa operator<<(AEWriter& writer, ComparisonSpecifier value)
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename CMP_OBJ1, typename CMP_OBJ2, typename CMP_OP>
class ComparisonSpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    OBJ_CLASS;  //!< Template Parameter.  A functor that generates an object class.
    typedef typename    CMP_OBJ1;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    CMP_OBJ2;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    CMP_OP;     //!< Template Parameter.  A functor that generates a comparison operator.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(OBJ_CLASS, B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(CMP_OBJ1,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(CMP_OBJ2,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(CMP_OP,    B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    ComparisonSpecifier(OBJ_SPEC obj, OBJ_CLASS objClass, CMP_OBJ1 cmpObj1, CMP_OBJ2 cmpObj2, CMP_OP cmpOp);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObj;       //!< A functor that generates an object specifier.
    OBJ_CLASS   mClass;     //!< A functor that generates an object class.
    CMP_OBJ1    mCmpObj1;   //!< A functor that generates an object specifier.
    CMP_OBJ2    mCmpObj2;   //!< A functor that generates an object specifier.
    CMP_OP      mCmpOp;     //!< A functor that generates a comparison operator.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename OBJ_CLASS, typename CMP_OBJ1, typename CMP_OBJ2, typename CMP_OP>
ComparisonSpecifier<OBJ_SPEC, OBJ_CLASS, CMP_OBJ1, CMP_OBJ2, CMP_OP>::ComparisonSpecifier(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    OBJ_CLASS   objClass,   //!< A functor that generates an object class.
    CMP_OBJ1    cmpObj1,    //!< A functor that generates an object specifier.
    CMP_OBJ2    cmpObj2,    //!< A functor that generates an object specifier.
    CMP_OP      cmpOp)      //!< A functor that generates a comparison operator.
        : mObj(obj), mClass(objClass), mCmpObj1(cmpObj1), mCmpObj2(cmpObj2), mCmpOp(cmpOp)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes the class, 
    container specifier and index to the record.
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename CMP_OBJ1, typename CMP_OBJ2, typename CMP_OP>
AEWriter&
ComparisonSpecifier<OBJ_SPEC, OBJ_CLASS, CMP_OBJ1, CMP_OBJ2, CMP_OP>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoSpecRecord(writer, typeObjectSpecifier);
    
    writer << AEKey(keyAEDesiredClass) << mClass
           << AEKey(keyAEContainer)    << mObj 
           << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formTest);
    
    {
        AutoAEWriterRecord  autoCompRecord(writer, typeCompDescriptor, keyAEKeyData, 0);
        
        writer << AEKey(keyAEObject1)      << mCmpObj1
               << AEKey(keyAEObject2)      << mCmpObj2
               << AEKey(keyAECompOperator) << mCmpOp;
    }
    
    return writer;
}


// ------------------------------------------------------------------------------------------
/*! Writes out the ComparisonSpecifier @a value to the AEWriter @a writer.
    
    @relates    ComparisonSpecifier
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename CMP_OBJ1, typename CMP_OBJ2, typename CMP_OP>
inline AEWriter&
operator << (AEWriter& writer, ComparisonSpecifier<OBJ_SPEC, OBJ_CLASS, CMP_OBJ1, CMP_OBJ2, CMP_OP> value)
{
    return (value(writer));
}


// ==========================================================================================
//  RangeSpecifier

#pragma mark -
#pragma mark RangeSpecifier

/*!
    @brief  Functor that holds an object specifier for a comparison.
    
    This functor is meant to be passed to any of the arguments to instantiations of 
    AEEvent that take an object specifier.
    
    @sa operator<<(AEWriter& writer, RangeSpecifier value)
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename RNG_OBJ1, typename RNG_OBJ2>
class RangeSpecifier : public std::unary_function<AEWriter, AEWriter>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    OBJ_SPEC;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    OBJ_CLASS;  //!< Template Parameter.  A functor that generates an object class.
    typedef typename    RNG_OBJ1;   //!< Template Parameter.  A functor that generates an object specifier.
    typedef typename    RNG_OBJ2;   //!< Template Parameter.  A functor that generates an object specifier.
    //@}
#endif
    
    // Concept checks.
    BOOST_CLASS_REQUIRE(OBJ_SPEC,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(OBJ_CLASS, B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(RNG_OBJ1,  B::AEEventFunctor, AEWritableConcept);
    BOOST_CLASS_REQUIRE(RNG_OBJ2,  B::AEEventFunctor, AEWritableConcept);

    //! Constructor.
    RangeSpecifier(OBJ_SPEC obj, OBJ_CLASS objClass, RNG_OBJ1 rngObj1, RNG_OBJ2 rngObj2);
    
    //! Appends the insertion location to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
    
private:
    OBJ_SPEC    mObj;       //!< A functor that generates an object specifier.
    OBJ_CLASS   mClass;     //!< A functor that generates an object class.
    RNG_OBJ1    mRngObj1;   //!< A functor that generates an object specifier.
    RNG_OBJ2    mRngObj2;   //!< A functor that generates an object specifier.
};

// ------------------------------------------------------------------------------------------
template <typename OBJ_SPEC, typename OBJ_CLASS, typename RNG_OBJ1, typename RNG_OBJ2>
RangeSpecifier<OBJ_SPEC, OBJ_CLASS, RNG_OBJ1, RNG_OBJ2>::RangeSpecifier(
    OBJ_SPEC    obj,        //!< A functor that generates an object specifier.
    OBJ_CLASS   objClass,   //!< A functor that generates an object class.
    RNG_OBJ1    rngObj1,    //!< A functor that generates an object specifier.
    RNG_OBJ2    rngObj2)    //!< A functor that generates an object specifier.
        : mObj(obj), mClass(objClass), mRngObj1(rngObj1), mRngObj2(rngObj2)
{
}

// ------------------------------------------------------------------------------------------
/*! The function opens an Apple %Event record on @a writer, then writes the class, 
    container specifier and index to the record.
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename RNG_OBJ1, typename RNG_OBJ2>
AEWriter&
RangeSpecifier<OBJ_SPEC, OBJ_CLASS, RNG_OBJ1, RNG_OBJ2>::operator () (
    AEWriter&   writer)     //!< The Apple %Event stream to write to.
    const
{
    AutoAEWriterRecord  autoSpecRecord(writer, typeObjectSpecifier);
    
    writer << AEKey(keyAEDesiredClass) << mClass
           << AEKey(keyAEContainer)    << mObj 
           << AEKey(keyAEKeyForm)      << AETypedObject<typeEnumerated>(formRange);
    
    {
        AutoAEWriterRecord  autoRangeRecord(writer, typeRangeDescriptor, keyAEKeyData, 0);
        
        writer << AEKey(keyAERangeStart) << mRngObj1
               << AEKey(keyAERangeStop)  << mRngObj2;
    }
    
    return writer;
}


// ------------------------------------------------------------------------------------------
/*! Writes out the ComparisonSpecifier @a value to the AEWriter @a writer.
    
    @relates    ComparisonSpecifier
*/
template <typename OBJ_SPEC, typename OBJ_CLASS, typename RNG_OBJ1, typename RNG_OBJ2>
inline AEWriter&
operator << (AEWriter& writer, RangeSpecifier<OBJ_SPEC, OBJ_CLASS, RNG_OBJ1, RNG_OBJ2> value)
{
    return (value(writer));
}


// ==========================================================================================
//  OptionalSaveOption

#pragma mark -
#pragma mark OptionalSaveOption

/*!
    @brief  Functor that holds an optional "save option".
    
    This functor is meant to be passed as the @a inSaveOption argument to 
    AEEvent<kCoreEventClass, kAEQuitApplication>::Send() and 
    AEEvent<kAECoreSuite, kAEClose>::Send().
    
    A "save option" is an enumerated type whose possible values are @c kAEYes, @c kAENo 
    and @c kAEAsk.  It is only written out if its value is different from the default 
    value of @c kAEAsk.
*/
class OptionalSaveOption : public AEOptionalTypedObject<typeEnumeration>
{
public:
    
    //! Constructor.  Takes the save option as an argument.
    OptionalSaveOption(OSType inSaveOption = kAEAsk);
};

// ------------------------------------------------------------------------------------------
inline
OptionalSaveOption::OptionalSaveOption(OSType inSaveOption /* = kAEAsk */)
    : AEOptionalTypedObject<typeEnumeration>(inSaveOption, kAEAsk)
{
}


// ==========================================================================================
//  OptionalUrl

#pragma mark -
#pragma mark OptionalUrl

/*!
    @brief  Functor that holds an optional URL.
    
    This functor is meant to be passed as the @a inUrl argument to 
    AEEvent<kAECoreSuite, kAEClose>::Send() and 
    AEEvent<kAECoreSuite, kAESave>::Send().
    
    The URL is only written out if it isn't empty, which it is by default.
*/
class OptionalUrl : public AEOptionalTypedObject<typeFileURL>
{
public:
    
    //! Constructor.  Takes the URL as an argument.
    OptionalUrl(Url inUrl = Url());
};

// ------------------------------------------------------------------------------------------
inline
OptionalUrl::OptionalUrl(Url inUrl /* = Url() */)
    : AEOptionalTypedObject<typeFileURL>(inUrl, Url())
{
}


// ==========================================================================================
//  OptionalObjectType

#pragma mark -
#pragma mark OptionalObjectType

/*!
    @brief  Functor that holds an optional object type.
    
    This functor is meant to be passed as the @a inObjectType argument to 
    AEEvent<kAECoreSuite, kAESave>::Send().
    
    The object type is only written out if it isn't empty, which it is by default.
*/
class OptionalObjectType : public AEOptionalTypedObject<typeUTF16ExternalRepresentation>
{
public:
    
    //! Constructor.  Takes the object type as an argument.
    OptionalObjectType(String inObjectType = String());
};

// ------------------------------------------------------------------------------------------
inline
OptionalObjectType::OptionalObjectType(String inObjectType /* = String() */)
    : AEOptionalTypedObject<typeUTF16ExternalRepresentation>(inObjectType, String())
{
}


// ==========================================================================================
//  OptionalRequestedType

#pragma mark -
#pragma mark OptionalRequestedType

/*!
    @brief  Functor that holds an optional requested data type.
    
    This functor is meant to be passed as the @a inRequestedType argument to 
    AEEvent<kAECoreSuite, kAEGetData>::Send().
    
    The requested data type is only written out if isn't either @c 0 or @c typeWildCard.
    Its default value is @c typeWildCard.
*/
class OptionalRequestedType : public AEOptionalTypedObject<typeType>
{
public:
    
    //! Constructor.  Takes the object type as an argument.
    OptionalRequestedType(DescType inRequestedType = typeWildCard);
};

// ------------------------------------------------------------------------------------------
inline
OptionalRequestedType::OptionalRequestedType(DescType inRequestedType /* = typeWildCard */)
    : AEOptionalTypedObject<typeType>(inRequestedType, typeWildCard)
{
}


// ==========================================================================================
//  OptionalAEDesc

#pragma mark -
#pragma mark OptionalAEDesc

/*!
    @brief  Functor that holds an optional Apple %Event descriptor.
    
    This functor is meant to be passed as the @a inProperties and @a inData arguments to 
    AEEvent<kAECoreSuite, kAECreateElement>::Send().
    
    The Apple %Event descriptor is only written out if isn't @c NULL, which it is 
    by default.
    
    @sa operator<<(AEWriter& writer, OptionalAEDesc value)
*/
class OptionalAEDesc : public std::unary_function<AEWriter, AEWriter>
{
public:
    
    //! Constructor.  Takes the Apple %Event descriptor as an argument.
    OptionalAEDesc(const AEDesc* desc = NULL);
    
    //! Appends the object's value to @a writer.
    AEWriter&   operator () (AEWriter& writer) const;
        
private:
    
    // member variables
    const AEDesc*   mDesc;  //!< The Apple %Event descriptor, or @c NULL.
};

// ------------------------------------------------------------------------------------------
inline
OptionalAEDesc::OptionalAEDesc(const AEDesc* desc /* = NULL */)
    : mDesc(desc)
{
}


// ------------------------------------------------------------------------------------------
/*! Writes out the OptionalAEDesc @a value to the AEWriter @a writer.
    
    @relates    OptionalAEDesc
*/
inline AEWriter&
operator << (AEWriter& writer, OptionalAEDesc value)
{
    return (value(writer));
}


// ==========================================================================================
//  OptionalObjectClass

#pragma mark -
#pragma mark OptionalObjectClass

/*!
    @brief  Functor that holds an optional "object class".
    
    This functor is meant to be passed as the @a inObjectClass argument to 
    AEEvent<kAECoreSuite, kAECountElements>::Send().
    
    According to that event's definition, if the object class is omitted, then the 
    responding object needs to count all of its elements.  Since MOSL translates a 
    missing object class into @c cObject, we use that value as the default.
*/
class OptionalObjectClass : public AEOptionalTypedObject<typeType>
{
public:
    
    //! Constructor.  Takes the object class as an argument.
    OptionalObjectClass(OSType inObjectClass = cObject);
};

// ------------------------------------------------------------------------------------------
inline
OptionalObjectClass::OptionalObjectClass(OSType inObjectClass /* = cObject */)
    : AEOptionalTypedObject<typeType>(inObjectClass, cObject)
{
}


// ==========================================================================================
//  DescParamResult

#pragma mark -
#pragma mark DescParamResult

/*!
    @brief  Functor returns an Apple %Event result typed as per DescParam.
    
    This functor is meant to be passed to any argument of instantiations of AEEvent 
    that return a result.  The result is coerced if necessary to the C/C++ language 
    type denoted by @a TYPE.
*/
template <DescType TYPE>
class DescParamResult : public std::unary_function<AEDesc, void>
{
public:
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef DescType    TYPE;   //!< The template parameter.  Should be an integral four-char constant (a @c DescType).
    //@}
#endif
    
    //! The language type.  See TaggedTypeTrait.
    typedef typename TaggedTypeTrait<TYPE>::Type    ValueType;
    
    //! Constructor.  Takes a reference to the output value.
    DescParamResult(ValueType& valueRef)
        : mValueRef(valueRef) {}
    
    //! Invocation.
    void    operator () (const AEDesc& inResult) const;
        
private:
    
    // member variables
    ValueType&  mValueRef;  //!< Holds the output value.
};

// ------------------------------------------------------------------------------------------
/*! Copies and optionally coerces the data in @a inResult into the area referenced by 
    @a mValueRef.
*/
template <DescType TYPE> inline void
DescParamResult<TYPE>::operator () (
    const AEDesc&   inResult)       //!< The result descriptor.
    const
{
    DescParam<TYPE>::Get(inResult, mValueRef);
}


// ==========================================================================================
//  AEDescResult

#pragma mark -
#pragma mark AEDescResult

/*!
    @brief  Functor returns an Apple %Event result as an @c AEDesc.
    
    This functor is meant to be passed to any argument of instantiations of AEEvent 
    that return a result.  The result is copied into the storage referenced by 
    @a mDescRef.
*/
class AEDescResult : public std::unary_function<AEDesc, void>
{
public:
    
    //! Constructor.  Takes a reference to the output value.
    AEDescResult(AEDesc& descRef)
        : mDescRef(descRef) {}
    
    //! Invocation.
    void    operator () (const AEDesc& inResult) const;
        
private:
    
    // member variables
    AEDesc& mDescRef;   //!< Holds the output value.
};


// ==========================================================================================
//  NullResult

#pragma mark -
#pragma mark NullResult

/*!
    @brief  Functor ignores an Apple %Event result.
    
    This functor is meant to be passed to any argument of instantiations of AEEvent 
    that return a result.  The result is completely ignored.  Use this class when 
    you don't need the result from an AEEvent which normally returns one.
*/
class NullResult : public std::unary_function<AEDesc, void>
{
public:
    
    //! Constructor.
    NullResult() {}
    
    //! Invocation.
    void    operator () (const AEDesc&) const {}
};


}   // namespace AEEventFunctor


// ==========================================================================================
//  AEEventBase

#pragma mark -
#pragma mark AEEventBase

/*!
    @brief  Common base class for representing Apple %Events.
    
    This class contains member functions and variables that are independent of the 
    specifics of a particular event.  Its main purpose is to serve as a common base 
    class for the various template instantiations of AEEvent.  It is rarely if ever 
    used by itself.
    
    AEEventBase and its derivatives have three distinct responsibilities:
        
    - Extracting parameters from incoming Apple %Events, making them accessible 
      to clients.  This is done by adding code in the derived class' constructors 
      to retrieve the parameters.
    - Writing return values in the Apple %Event reply.  This is done by 
      overriding Update() as appropriate.
    - Constructing and sending outgoing Apple %Events.  This is done by 
      adding specific static member functions to the derived classes.
    
    @ingroup    AppleEvents
*/
class AEEventBase : public boost::noncopyable
{
public:
    
    //! @name Constructors & Destructor
    //@{
    //! Constructs an AEEventBase for an incoming Apple %Event.
            AEEventBase(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEventBase();
    //@}
    
    //! @name Synchronisation
    //@{
    //! Sets output parameters.
    void    Update();
    //@}
    
    //! @name Sending Apple %Events
    //@{
    //! Sends @a inEvent using @a inMode.
    static void SendEvent(
                    const AppleEvent&   inEvent, 
                    AESendMode          inMode = 0);
    //! Sends @a inEvent using @a inMode.  The reply is returned in @a outResult.
    static void SendEvent(
                    const AppleEvent&   inEvent, 
                    AEDesc&             outResult, 
                    AESendMode          inMode = 0);
    //@}
    
    void    CheckRequiredParams();
    
    // member variables
    const AppleEvent&   mAppleEvent;    //!< The incoming Apple %Event.
    
protected:
    
    /*! @name Simple Apple %Events
        
        A 'simple' Apple %Event has the following characteristics:
            - It is sent to the application itself.
            - It has either no parameters or a single direct object parameter.
            - It doesn't return a result.
    */
    //@{
    //! Constructs and sends a simple Apple %Event with a direct object parameter.
    template <typename TARGET, typename OBJ_SPEC>
    static void SendSimpleEvent(
                    AEEventClass    inEventClass, 
                    AEEventID       inEventID, 
                    TARGET          inTarget,
                    OBJ_SPEC        inDirectObject, 
                    AESendMode      inMode = 0);
    //! Constructs and sends a simple Apple %Event with no parameters.
    template <typename TARGET>
    static void SendSimpleEvent(
                    AEEventClass    inEventClass, 
                    AEEventID       inEventID, 
                    TARGET          inTarget,
                    AESendMode      inMode = 0);
    //! Constructs a simple Apple %Event with a direct object parameter, returning it in @a outEvent.
    template <typename TARGET, typename OBJ_SPEC>
    static void MakeSimpleEvent(
                    AEEventClass    inEventClass, 
                    AEEventID       inEventID, 
                    TARGET          inTarget,
                    OBJ_SPEC        inDirectObject, 
                    AppleEvent&     outEvent);
    //! Constructs a simple Apple %Event with no parameters, returning it in @a outEvent.
    template <typename TARGET>
    static void MakeSimpleEvent(
                    AEEventClass    inEventClass, 
                    AEEventID       inEventID, 
                    TARGET          inTarget,
                    AppleEvent&     outEvent);
    //@}
    
    // member variables
    AEWriter&   mWriter;
};

// ------------------------------------------------------------------------------------------
/*! The Apple %Event has class @a inEventClass and ID @a inEventID.  @a inDirectObject 
    needs to respond to <tt>operator \<\<</tt> in order to write itself out to the 
    event.
*/
template <typename TARGET, typename OBJ_SPEC> void
AEEventBase::SendSimpleEvent(
    AEEventClass    inEventClass,       //!< The Apple %Event class of the event to send.
    AEEventID       inEventID,          //!< The Apple %Event ID of the event to send.
    TARGET          inTarget,           //!< A functor that generates the event's recipient.
    OBJ_SPEC        inDirectObject,     //!< A functor that generates the event's direct parameter.
    AESendMode      inMode /* = 0 */)   //!< The event's send mode.
{
    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
    
    AEDescriptor    event;
    
    MakeSimpleEvent(inEventClass, inEventID, inTarget, inDirectObject, event);
    
    SendEvent(event, inMode);
}

// ------------------------------------------------------------------------------------------
/*! The Apple %Event has class @a inEventClass and ID @a inEventID.
*/
template <typename TARGET> void
AEEventBase::SendSimpleEvent(
    AEEventClass    inEventClass,       //!< The Apple %Event class of the event to send.
    AEEventID       inEventID,          //!< The Apple %Event ID of the event to send.
    TARGET          inTarget,           //!< A functor that generates the event's recipient.
    AESendMode      inMode /* = 0 */)   //!< The event's send mode.
{
    AEDescriptor    event;
    
    MakeSimpleEvent(inEventClass, inEventID, inTarget, event);
    
    SendEvent(event, inMode);
}

// ------------------------------------------------------------------------------------------
/*! The Apple %Event has class @a inEventClass and ID @a inEventID.  @a inDirectObject 
    needs to respond to <tt>operator \<\<</tt> in order to write itself out to the 
    event.
*/
template <typename TARGET, typename OBJ_SPEC> void
AEEventBase::MakeSimpleEvent(
    AEEventClass    inEventClass,       //!< The Apple %Event class of the event to send.
    AEEventID       inEventID,          //!< The Apple %Event ID of the event to send.
    TARGET          inTarget,           //!< A functor that generates the event's recipient.
    OBJ_SPEC        inDirectObject,     //!< A functor that generates the event's direct parameter.
    AppleEvent&     outEvent)           //!< The constructed event.
{
    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
    
    AEWriter    writer(inEventClass, inEventID, inTarget);
    
    writer << AEKey(keyDirectObject) << inDirectObject;
    
    writer.Close(outEvent);
}

// ------------------------------------------------------------------------------------------
/*! The Apple %Event has class @a inEventClass and ID @a inEventID.
*/
template <typename TARGET> void
AEEventBase::MakeSimpleEvent(
    AEEventClass    inEventClass,       //!< The Apple %Event class of the event to send.
    AEEventID       inEventID,          //!< The Apple %Event ID of the event to send.
    TARGET          inTarget,           //!< A functor that generates the event's recipient.
    AppleEvent&     outEvent)           //!< The constructed event.
{
    AEWriter    writer(inEventClass, inEventID, inTarget);
    
    writer.Close(outEvent);
}


// ==========================================================================================
//  AEEvent

#pragma mark -
#pragma mark AEEvent<>

/*!
    @brief  Wrapper class for a single Apple %Event.
    
    This is a class template, whose template parameters EVT_CLASS and EVT_ID uniquely 
    identify a given "kind" of Apple %Event.
    
    The idea is to allow for template specialisations, thus allowing event-specific 
    functions and data members to be added.  Although this is its intended principal 
    use, it's also possible to instantiate AEEvent directly without specialisation;  
    this is mostly useful for every simple Apple %Events that take no arguments and 
    return no reply.
    
    AEEvent and its specialisations have two distinct responsibilities:
    
    - Extracting parameters from incoming Apple %Events, making them accessible 
      to clients.  This is done by adding code in the specialisations' constructors 
      to retrieve the parameters, placing them in data member specific to the 
      specialisation.
    - Writing return values in the Apple %Event reply.  This is done by 
      overriding Update() in the specialisations of events that need to return 
      a result.
    - Constructing and sending outgoing Apple %Events.  This is done by 
      adding static member functions to the specialisations.
    
    @note   The various specialisations of AEEvent don't have a member variable 
            corresponding to their direct parameter, because the object support 
            infrastructure (AEObjectSupport & MoreOSL) extract it for us.
    
    @ingroup    AppleEvents
*/
template <AEEventClass EVT_CLASS, AEEventID EVT_ID>
class AEEvent : public AEEventBase
{
public:

#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef AEEventClass    EVT_CLASS;  //!< Template Parameter.  A four-character constant identifying an Apple %Event class.
    typedef AEEventID       EVT_ID;     //!< Template Parameter.  A four-character constant identifying an Apple %Event id within @a EVT_CLASS.
    //@}
#endif
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
};

// ------------------------------------------------------------------------------------------
/*! Does nothing but pass the arguments on to the base class.
*/
template <AEEventClass EVT_CLASS, AEEventID EVT_ID> inline
AEEvent<EVT_CLASS, EVT_ID>::AEEvent(
    const AppleEvent&   inAppleEvent,   //!< The incoming Apple %Event.
    AEWriter&           ioWriter)       //!< The reponse to @a inAppleEvent.  May be null (eg, if the caller doesn't want a reply).
        : AEEventBase(inAppleEvent, ioWriter)
{
}


// ==========================================================================================
//  AEEvent Template Specialisations

#pragma mark -
#pragma mark * Required Suite Events *

#pragma mark AEEvent<kCoreEventClass, kAEQuitApplication>

/*!
    @brief  Specialisation of AEEvent for the @b quit Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kCoreEventClass, kAEQuitApplication> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Quit event.
    template <typename TARGET, typename SAVE_OPT>
    static void Send(
                    SAVE_OPT    inSaveOption,   //!< A functor that generates a "save option"
                    TARGET      inTarget,       //!< A functor that generates the event's recipient.
                    AESendMode  inMode = 0)     //!< The event's send mode.
                {
                    AEDescriptor    event;
                    
                    Make(inSaveOption, event);
                    
                    SendEvent(event, inMode);
                }
                
    //! Constructs a Quit event for later use.
    template <typename TARGET, typename SAVE_OPT>
    static void Make(
                    SAVE_OPT    inSaveOption,   //!< A functor that generates a "save option"
                    TARGET      inTarget,       //!< A functor that generates the event's recipient.
                    AppleEvent& outEvent)       //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<SAVE_OPT> >();
                    
                    // "quit"
                    
                    AEWriter    writer(kCoreEventClass, kAEQuitApplication, inTarget);
                    
                    // "quit saving <save-option>"
                    
                    writer << AEKeyedObject(keyAESaveOptions, inSaveOption);
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    OSType  mSaveOption;    //!< One of @c kAEYes, @c kAENo, @c kAEAsk.
};


#pragma mark -
#pragma mark * Standard Suite Events *

#pragma mark AEEvent<kAECoreSuite, kAEClone>

/*!
    @brief  Specialisation of AEEvent for the @b duplicate Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAEClone> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Sets output parameters.
    virtual void    Update();
    
    //! Constructs and sends a Clone event.
    template <typename TARGET, typename OBJ_SPEC, typename INS_LOC, typename PROPS, typename RESULT>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    INS_LOC     inInsertionLoc,     //!< A functor that generates an insertion location.
                    PROPS       inProperties,       //!< A functor that generates initial object properties.
                    RESULT      outNewObject,       //!< A functor that retrieves the event's result.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inDirectObject, inInsertionLoc, inProperties, event);
                    
                    SendEvent(event, result, inMode);
                    
                    outNewObject(result);
                }
                
    //! Constructs a Clone event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename INS_LOC, typename PROPS>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    INS_LOC     inInsertionLoc,     //!< A functor that generates an insertion location.
                    PROPS       inProperties,       //!< A functor that generates initial object properties.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<INS_LOC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<PROPS> >();
                    
                    // "duplicate"
                    
                    AEWriter    writer(kAECoreSuite, kAEClone, inTarget);
                    
                    // "duplicate <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "duplicate <objspec> to <insloc>"
                    
                    writer << AEKey(keyAEInsertHere) << inInsertionLoc;
                    
                    // "duplicate <objspec> to <insloc> with properties <props>"
                    
                    writer << AEKey(keyAEPropData) << inProperties;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    OSType          mInsertPosition;    //!< The insertion position, relative to @a mTarget.
    AEObjectPtr     mTarget;            //!< The target object.
    AEDescriptor    mProperties;        //!< The new element's properties.
    AEObjectPtr     mNewObject;         //!< The newly duplicated object (output).
};


#pragma mark AEEvent<kAECoreSuite, kAEClose>

/*!
    @brief  Specialisation of AEEvent for the @b close Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAEClose> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Close event.
    template <typename TARGET, typename OBJ_SPEC, typename SAVE_OPT, typename OBJECT_URL>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    SAVE_OPT    inSaveOption,       //!< A functor that generates a "save option"
                    OBJECT_URL  inObjectUrl,        //!< A functor that generates a URL
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event;
                    
                    Make(inTarget, inDirectObject, inSaveOption, inObjectUrl, event);
                    
                    SendEvent(event, inMode);
                }

    //! Constructs a Close event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename SAVE_OPT, typename OBJECT_URL>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    SAVE_OPT    inSaveOption,       //!< A functor that generates a "save option"
                    OBJECT_URL  inObjectUrl,        //!< A functor that generates a URL
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<SAVE_OPT> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJECT_URL> >();
                    
                    // "close"
                    
                    AEWriter    writer(kAECoreSuite, kAEClose, inTarget);
                    
                    // "close <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "close <object-specifier> saving <save-option>"
                    
                    writer << AEKey(keyAESaveOptions) << inSaveOption;
                    
                    // "close <object-specifier> saving <save-option> saving in <file-url>"
                    
                    writer << AEKey(keyAEFile) << inObjectUrl;

                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    Url     mObjectUrl;     //!< The object's new URL, or empty.
    OSType  mSaveOption;    //!< One of @c kAEYes, @c kAENo, @c kAEAsk.
};


#pragma mark AEEvent<kAECoreSuite, kAECountElements>

/*!
    @brief  Specialisation of AEEvent for the @b count Apple %Event.
    
    @note:  It doesn't make sense to instantiate this class because the @c kAECoreSuite / 
            @c kAECountElements event is handled automatically by MoreOSL.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAECountElements> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Sets output parameters.
    virtual void    Update();
    
    //! Constructs and sends a Count event.
    template <typename TARGET, typename OBJ_SPEC, typename OBJ_CLASS, typename RESULT>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    OBJ_CLASS   inObjectClass,      //!< A functor that generates an object class.
                    RESULT      outCount,           //!< A functor that retrieves the event's result.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inDirectObject, inObjectClass, event);
                    
                    SendEvent(event, result, inMode);
                    
                    outCount(result);
                }

    //! Constructs a Count event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename OBJ_CLASS>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    OBJ_CLASS   inObjectClass,      //!< A functor that generates an object class.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_CLASS> >();
                    
                    // "count"
                    
                    AEWriter    writer(kAECoreSuite, kAECountElements, inTarget);
                    
                    // "count <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "count <object-specifier> each <object-class>"
                    
                    writer << AEKey(keyAEObjectClass) << inObjectClass;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments
    OSType  mObjectClass;   //!< The class of the elements to count.
    size_t  mCount;         //!< The number of elements (output).

private:
    
    // illegal operations
    AEEvent();
};


#pragma mark AEEvent<kAECoreSuite, kAECreateElement>

/*!
    @brief  Specialisation of AEEvent for the @b make Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAECreateElement> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Sets output parameters.
    virtual void    Update();
    
    //! Constructs and sends a Make event.
    template <typename TARGET, typename OBJ_CLASS, typename INS_LOC, typename PROPS, typename DATA, typename RESULT>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_CLASS   inObjectClass,      //!< A functor that generates an object class.
                    INS_LOC     inInsertionLoc,     //!< A functor that generates an insertion location.
                    PROPS       inProperties,       //!< A functor that generates initial object properties.
                    DATA        inData,             //!< A functor that generates initial object data.
                    RESULT      outNewObject,       //!< A functor that retrieves the event's result.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inObjectClass, inInsertionLoc, inProperties, inData, event);
                    
                    SendEvent(event, result, inMode);
                    
                    outNewObject(result);
                }
                
    //! Constructs a Make event for later use.
    template <typename TARGET, typename OBJ_CLASS, typename INS_LOC, typename PROPS, typename DATA>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_CLASS   inObjectClass,      //!< A functor that generates an object class.
                    INS_LOC     inInsertionLoc,     //!< A functor that generates an insertion location.
                    PROPS       inProperties,       //!< A functor that generates initial object properties.
                    DATA        inData,             //!< A functor that generates initial object data.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_CLASS> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<INS_LOC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<PROPS> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<DATA> >();
                    
                    // "make new"
                    
                    B::AEWriter writer(kAECoreSuite, kAECreateElement, inTarget);
                    
                    // "make new <object-class>"
                    
                    writer << AEKey(keyAEObjectClass) << inObjectClass;
                    
                    // "make new <object-class> at <location>"
                    
                    writer << AEKey(keyAEInsertHere) << inInsertionLoc;
                    
                    // "make new <object-class> at <location> with data <data>"
                    
                    writer << AEKey(keyAEData) << inData;
                    
                    // "make new <object-class> at <location> with data <data> with properties <props>"
                    
                    writer << AEKey(keyAEPropData) << inProperties;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    OSType          mObjectClass;       //!< The desired class of the new element.
    OSType          mInsertPosition;    //!< The insertion position, relative to @a mTarget.
    AEObjectPtr     mTarget;            //!< The target object.
    AEDescriptor    mProperties;        //!< The new element's properties.
    AEDescriptor    mData;              //!< The new element's data.
    AEObjectPtr     mNewObject;         //!< The newly created object (output).
};


#pragma mark AEEvent<kAECoreSuite, kAEDelete>

/*!
    @brief  Specialisation of AEEvent for the @b delete Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAEDelete> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Delete event.
    template <typename TARGET, typename OBJ_SPEC>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    SendSimpleEvent(kAECoreSuite, kAEDelete, inTarget, inDirectObject, inMode);
                }

    //! Constructs a Delete event for later use.
    template <typename TARGET, typename OBJ_SPEC>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    MakeSimpleEvent(kAECoreSuite, kAEDelete, inTarget, inDirectObject, outEvent);
                }
};


#pragma mark AEEvent<kAECoreSuite, kAEDoObjectsExist>

/*!
    @brief  Specialisation of AEEvent for the @b exists Apple %Event.
    
    @note:  It doesn't make sense to instantiate this class because the @c kAECoreSuite / 
            @c kAECountElements event is handled automatically by MoreOSL.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAEDoObjectsExist> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Sets output parameters.
    virtual void    Update();
    
    //! Constructs and sends an Exists event.
    template <typename TARGET, typename OBJ_SPEC, typename RESULT>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    RESULT      outExists,          //!< A functor that retrieves the event's result.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inDirectObject, event);
                    
                    SendEvent(event, result, inMode);
                    
                    outExists(result);
                }

    //! Constructs an Exists event for later use.
    template <typename TARGET, typename OBJ_SPEC>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    
                    // "exists"
                    
                    AEWriter    writer(kAECoreSuite, kAEDoObjectsExist, inTarget);
                    
                    // "exists <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments
    bool    mExists;    //!< Does the direct object exist? (output)

private:
    
    // illegal operations
    AEEvent();
};


#pragma mark AEEvent<kAECoreSuite, kAEGetData>

/*!
    @brief  Specialisation of AEEvent for the @b get Apple %Event.
    
    @note:  It doesn't make sense to instantiate this class because the @c kAECoreSuite / 
            @c kAECountElements event is handled automatically by MoreOSL.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAEGetData> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Sets output parameters.
    virtual void    Update();
    
    //! Constructs and sends a Get event.
    template <typename TARGET, typename OBJ_SPEC, typename REQ_TYPE, typename RESULT>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    REQ_TYPE    inRequestedType,    //!< A functor that generates the requested data type.
                    RESULT      outData,            //!< A functor that retrieves the event's result.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inDirectObject, inRequestedType, event);
                    
                    SendEvent(event, result, inMode);
                    
                    outData(result);
                }

    //! Constructs a Get event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename REQ_TYPE>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    REQ_TYPE    inRequestedType,    //!< A functor that generates the requested data type.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<REQ_TYPE> >();
                    
                    // "get"
                    
                    AEWriter    writer(kAECoreSuite, kAEGetData, inTarget);
                    
                    // "get <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "get <object-specifier> as <type>"
                    
                    writer << AEKey(keyAERequestedType) << inRequestedType;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments
    OSType  mRequestedType; //!< The preferred format for the data.

private:
    
    // illegal operations
    AEEvent();
};


#pragma mark AEEvent<kAECoreSuite, kAEMove>

/*!
    @brief  Specialisation of AEEvent for the @b move Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAEMove> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Sets output parameters.
    virtual void    Update();
    
    //! Constructs and sends a Move event.
    template <typename TARGET, typename OBJ_SPEC, typename INS_LOC, typename RESULT>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    INS_LOC     inInsertionLoc,     //!< A functor that generates an insertion location.
                    RESULT      outMovedObject,     //!< A functor that retrieves the event's result.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inDirectObject, inInsertionLoc, event);
                    
                    SendEvent(event, inMode);
                    
                    outMovedObject(result);
                }

    //! Constructs a Move event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename INS_LOC>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    INS_LOC     inInsertionLoc,     //!< A functor that generates an insertion location.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<INS_LOC> >();
                    
                    // "move"
                    
                    AEWriter    writer(kAECoreSuite, kAEMove, inTarget);
                    
                    // "move <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "move <objspec> to <insloc>"
                    
                    writer << AEKey(keyAEInsertHere) << inInsertionLoc;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    OSType      mInsertPosition;    //!< The insertion position, relative to @a mTarget.
    AEObjectPtr mTarget;            //!< The target object.
    AEObjectPtr mNewObject;         //!< The newly moved object (output).
};


#pragma mark AEEvent<kCoreEventClass, kAEOpen>

/*!
    @brief  Specialisation of AEEvent for the @b open Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kCoreEventClass, kAEOpen> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends an Open event.
    template <typename TARGET, typename OBJ_SPEC>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    SendSimpleEvent(kCoreEventClass, kAEOpen, inTarget, inDirectObject, inMode);
                }

    //! Constructs an Open event for later use.
    template <typename TARGET, typename OBJ_SPEC>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    MakeSimpleEvent(kCoreEventClass, kAEOpen, inTarget, inDirectObject, outEvent);
                }
};


#pragma mark AEEvent<kCoreEventClass, kAEPrint>

/*!
    @brief  Specialisation of AEEvent for the @b print Apple %Event.
    
    @ingroup    AppleEvents
    @todo       ReadProperties() only handles properties for which there is a 
                PMPrintSettings API.  For the others (eg print time) we need 
                to futz around with PMTickets.
*/
template <>
class AEEvent<kCoreEventClass, kAEPrint> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent();
    
    //! Constructs and sends a Print event.
    template <typename TARGET, typename OBJ_SPEC, typename PROPS, typename SHOW_DLG>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    PROPS       inProperties,       //!< A functor that generates the print job properties.
                    SHOW_DLG    inShowDialog,       //!< A functor that generates the "show dialog" flag.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event, result;
                    
                    Make(inTarget, inDirectObject, inProperties, inShowDialog, event);
                    
                    SendEvent(event, inMode);
                }

    //! Constructs a Print event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename PROPS, typename SHOW_DLG>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    PROPS       inProperties,       //!< A functor that generates the print job properties.
                    SHOW_DLG    inShowDialog,       //!< A functor that generates the "show dialog" flag.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<PROPS> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<SHOW_DLG> >();
                    
                    // "print"
                    
                    AEWriter    writer(kCoreEventClass, kAEPrint, inTarget);
                    
                    // "print <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "print <object-specifier> with properties <props>"
                    
                    writer << AEKey(keyAEPropData) << inProperties;
                    
                    // "print <object-specifier> with properties <props> with[out] show dialog"
                    
                    writer << AEKey(kPMShowPrintDialogAEType) << inShowDialog;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    boost::scoped_ptr<Printer>          mPrinter;
    boost::scoped_ptr<PrintSettings>    mPrintSettings;
    bool                                mShowDialog;
};


#pragma mark AEEvent<kAECoreSuite, kAESave>

/*!
    @brief  Specialisation of AEEvent for the @b save Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAESave> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Save event.
    template <typename TARGET, typename OBJ_SPEC, typename OBJECT_URL, typename OBJECT_TYPE>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    OBJECT_URL  inObjectUrl,        //!< A functor that generates a URL.
                    OBJECT_TYPE inObjectType,       //!< A functor that generates an object type.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event;
                    
                    Make(inTarget, inDirectObject, inObjectUrl, inObjectType, event);
                    
                    SendEvent(event, inMode);
                }

    //! Constructs a Save event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename OBJECT_URL, typename OBJECT_TYPE>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    OBJECT_URL  inObjectUrl,        //!< A functor that generates a URL.
                    OBJECT_TYPE inObjectType,       //!< A functor that generates an object type.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJECT_URL> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJECT_TYPE> >();
                    
                    // "save"
                    
                    AEWriter    writer(kAECoreSuite, kAESave, inTarget);
                    
                    // "save <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "save <object-specifier> in <file-url>"
                    
                    writer << AEKey(keyAEFile) << inObjectUrl;
                    
                    // "save <object-specifier> in <file-url> as <file-type>"
                    
                    writer << AEKey(keyAEFileType) << inObjectType;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    Url     mObjectUrl;     //!< The object's new URL, or empty.
    String  mObjectType;    //!< The object's new type, or empty.
};


#pragma mark AEEvent<kAECoreSuite, kAESetData>

/*!
    @brief  Specialisation of AEEvent for the @b set Apple %Event.
    
    @note:  It doesn't make sense to instantiate this class because the @c kAECoreSuite / 
            @c kAECountElements event is handled automatically by MoreOSL.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAECoreSuite, kAESetData> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Set event.
    template <typename TARGET, typename OBJ_SPEC, typename DATA>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    DATA        inData,             //!< A functor that generates the new object data.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    AEDescriptor    event;
                    
                    Make(inTarget, inDirectObject, inData, event);
                    
                    SendEvent(event, inMode);
                }

    //! Constructs a Set event for later use.
    template <typename TARGET, typename OBJ_SPEC, typename DATA>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    DATA        inData,             //!< A functor that generates the new object data.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    boost::function_requires< AEEventFunctor::AEWritableConcept<OBJ_SPEC> >();
                    boost::function_requires< AEEventFunctor::AEWritableConcept<DATA> >();
                    
                    // "set"
                    
                    AEWriter    writer(kAECoreSuite, kAESetData, inTarget);
                    
                    // "set <object-specifier>"
                    
                    writer << AEKey(keyDirectObject) << inDirectObject;
                    
                    // "set <object-specifier> to <data>"
                    
                    writer << AEKey(keyAEData) << inData;
                    
                    writer.Close(outEvent);
                }
    
    // Event arguments (incoming only)
    AEDescriptor    mData;
    
private:
    
    // illegal operations
    AEEvent();
};


#pragma mark -
#pragma mark * Miscellaneous Standards Suite Events *


#pragma mark AEEvent<kAEMiscStandards, kAERedo>

/*!
    @brief  Specialisation of AEEvent for the @b redo Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAEMiscStandards, kAERedo> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Redo event.
    template <typename TARGET>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    AESendMode  inMode = 0)
                {
                    SendSimpleEvent(kAEMiscStandards, kAERedo, inTarget, inMode);
                }

    //! Constructs a Redo event for later use.
    template <typename TARGET>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    AppleEvent& outEvent)
                {
                    MakeSimpleEvent(kAEMiscStandards, kAERedo, inTarget, outEvent);
                }
};

#pragma mark AEEvent<kAEMiscStandards, kAERevert>

/*!
    @brief  Specialisation of AEEvent for the @b revert Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAEMiscStandards, kAERevert> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Revert event.
    template <typename TARGET, typename OBJ_SPEC>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    SendSimpleEvent(kAEMiscStandards, kAERevert, inTarget, inDirectObject, inMode);
                }

    //! Constructs a Revert event for later use.
    template <typename TARGET, typename OBJ_SPEC>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    MakeSimpleEvent(kAEMiscStandards, kAERevert, inTarget, inDirectObject, outEvent);
                }
};


#pragma mark AEEvent<kAEMiscStandards, kAESelect>

/*!
    @brief  Specialisation of AEEvent for the @b select Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAEMiscStandards, kAESelect> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Select event.
    template <typename TARGET, typename OBJ_SPEC>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AESendMode  inMode = 0)         //!< The event's send mode.
                {
                    SendSimpleEvent(kAEMiscStandards, kAESelect, inTarget, inDirectObject, inMode);
                }

    //! Constructs a Select event for later use.
    template <typename TARGET, typename OBJ_SPEC>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    OBJ_SPEC    inDirectObject,     //!< A functor that generates an object specifier.
                    AppleEvent& outEvent)           //!< The constructed event.
                {
                    MakeSimpleEvent(kAEMiscStandards, kAESelect, inTarget, inDirectObject, outEvent);
                }
};


#pragma mark AEEvent<kAEMiscStandards, kAEUndo>

/*!
    @brief  Specialisation of AEEvent for the @b undo Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kAEMiscStandards, kAEUndo> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Undo event.
    template <typename TARGET>
    static void Send(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    AESendMode  inMode = 0)
                {
                    SendSimpleEvent(kAEMiscStandards, kAEUndo, inTarget, inMode);
                }
                
    //! Constructs a Undo event for later use.
    template <typename TARGET>
    static void Make(
                    TARGET      inTarget,           //!< A functor that generates the event's recipient.
                    AppleEvent& outEvent)
                {
                    MakeSimpleEvent(kAEMiscStandards, kAEUndo, inTarget, outEvent);
                }
};


#pragma mark -
#pragma mark * AppleScript Suite Events *


#pragma mark AEEvent<kASAppleScriptSuite, kASCommentEvent>

/*!
    @brief  Specialisation of AEEvent for the @c kASAppleScriptSuite / @c kASCommentEvent 
            Apple %Event.
    
    @ingroup    AppleEvents
*/
template <>
class AEEvent<kASAppleScriptSuite, kASCommentEvent> : public AEEventBase
{
public:
    
    //! Constructs an AEEvent for an incoming Apple %Event.
            AEEvent(
                const AppleEvent&   inAppleEvent, 
                AEWriter&           ioWriter);
    //! Destructor.
    virtual ~AEEvent()  {}
    
    //! Constructs and sends a Comment event.
    static void Send(
                    const char*     inComment, 
                    AESendMode      inMode = kAEDontExecute);
    //! Constructs and sends a Comment event.
    static void Send(
                    const String&   inComment, 
                    AESendMode      inMode = kAEDontExecute);
    //! Constructs a Comment event for later use.
    static void Make(
                    const String&   inComment, 
                    AppleEvent&     outEvent);
};


}   // namespace B

#endif  // BAEEvent_H_
