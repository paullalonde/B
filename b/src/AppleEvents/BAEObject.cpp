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

// file header
#include "BAEObject.h"

// B headers
#include "BAEDescriptor.h"
#include "BAEObjectSupport.h"
#include "BAEEvent.h"
#include "BKeyAdapter.h"
#include "BAEReader.h"
#include "BAEToken.h"
#include "BAEWriter.h"
#include "BException.h"


namespace B {

static void WriteToken(AEObjectPtr obj, AEWriter& writer)
{
    AEToken         token(obj);
    AEDescriptor    tokenDesc;
    
    token.Commit(tokenDesc);
    
    writer.WriteDesc(tokenDesc);
}


// ==========================================================================================
//  AEObject

// static member variables
boost::weak_ptr<AEObject>   AEObject::sDefaultObject;

// ------------------------------------------------------------------------------------------
/*! @attention  There shouldn't ever be a need to call this function.
    @sa         GetDefaultObject
*/
void
AEObject::SetDefaultObject(AEObjectPtr inObject)
{
    sDefaultObject = inObject;
}

// ------------------------------------------------------------------------------------------
/*! The two arguments establish (1) what kind (i.e., class) of object this is, and (2) 
    where the object is located in the AEOM hierarchy.  Note that @a inContainer may in 
    theory be @c NULL, but in practice it almost never is because that implies that the 
    object is at the top of the object hierarchy, and that place is already taken by 
    @c Application.
*/
AEObject::AEObject(
    AEObjectPtr inContainer,    //!< The object's container (as seen in AppleScript).
    DescType    inClassID)      //!< The object's class ID;  must match the application's AppleScript dictionary.
        : mContainer(inContainer), mClassID(inClassID)
{
    // There's nothing else to do.
}

// ------------------------------------------------------------------------------------------
/*! Override to release resources.
*/
AEObject::~AEObject()
{
    // There's nothing to do.
}

// ------------------------------------------------------------------------------------------
bool
AEObject::InheritsFrom(
    DescType        inBaseClassID)  //!< The base class ID;  must match the application's AppleScript dictionary.
    const
{
    return (DoesClassInheritFrom(GetClassID(), inBaseClassID));
}

// ------------------------------------------------------------------------------------------
/*! @internal   AEObjectSupport holds the AEOM hierarchy information, so we delegate to it.
*/
bool
AEObject::DoesClassInheritFrom(
    DescType        inClassID,      //!< The class ID;  must match the application's AppleScript dictionary.
    DescType        inBaseClassID)  //!< The base class ID;  must match the application's AppleScript dictionary.
{
    return ((inClassID == inBaseClassID) || 
            AEObjectSupport::Get().DoesClassInheritFrom(inClassID, inBaseClassID));
}

// ------------------------------------------------------------------------------------------
const AEInfo::ClassInfo&
AEObject::GetClassInfo() const
{
    return AEObjectSupport::Get().GetClassInfo(GetClassID());
}

// ------------------------------------------------------------------------------------------
/*! The default implementation iterates over all of the elements of the object's container, until 
    it find one that matches itself.  It then returns that element's index.
    
    Derived classes may override this function to provide a more efficient implementation.
*/
unsigned
AEObject::GetIndex() const
{
    AEObjectPtr container(mContainer);
    
    if (container.get() == NULL)
        B_THROW(AENoSuchObjectException());
    
    SInt32  count   = container->CountElements(mClassID);
    
    for (SInt32 index = 0; index < count; index++)
    {
        if (container->GetElementByIndex(mClassID, index).get() == this)
            return (index);
    }
    
    B_THROW(AENoSuchObjectException());
    
    return (0);
}

// ------------------------------------------------------------------------------------------
/*! If a derived class's instances can be identified by name, then it needs to override this 
    function to provide the name.
    
    The default implementation throws an exception.
*/
String
AEObject::GetName() const
{
    B_THROW(AENoSuchObjectException());
    
    return (String());
}

// ------------------------------------------------------------------------------------------
/*! If a derived class's instances can be identified by unique id, then it needs to override 
    this function to provide the id.
    
    The default implementation throws an exception.
*/
SInt32
AEObject::GetUniqueID() const
{
    B_THROW(AENoSuchObjectException());
    
    return (0);
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing elements needs to override this function to provide the number 
    of elements of a given class.
    
    @throws UnknownElementException If the class ID is unknown to the container.
    
    @note   In general, the element count for a class A needs to include instances of classes 
            derived from A.  It follows that if @a inElementType is @c cObject, then the 
            count should include @b all of the container's elements, regardless of class.
    
    The default implementation handles counting @c cObject;  for all other element types it 
    throws an exception.
*/
size_t
AEObject::CountElements(
    DescType        inElementType)  //!< The base class ID of the elements to count;  must match the application's AppleScript dictionary.
    const
{
    size_t  count   = 0;
    
    if (inElementType == cObject)
    {
        count = AEObjectSupport::Get().CountAllElements(GetAEObjectPtr());
    }
    else
    {
        B_THROW(AEClassHasNoElementsOfThisTypeException());
    }
    
    return (count);
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing elements @b must override this function if either of the following 
    conditions is true:
    
    -# The class supports indexed access to its elements;
    -# The class supports by-name or by-id access to its elements, and relies on the default 
       implementations of GetElementByName() and GetElementByUniqueID() for that access.
    
    @return A valid AEObject.  In case of error an exception is thrown.
    @throws UnknownElementException If the class ID is unknown to the container.
    
    @note   In general, the "index-space" for an instance of class A includes instances of 
            classes derived from A.
    
    The default implementation throws an exception.
*/
AEObjectPtr
AEObject::GetElementByIndex(
    DescType    /* inElementType */,    //!< The base class ID of the element;  must match the application's AppleScript dictionary.
    size_t      /* inIndex */)          //!< The element's zero-based index.
    const
{
    B_THROW(AEClassHasNoElementsOfThisTypeException());
    
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing elements accessible by unique id may override this function to 
    provide a more efficient implementation.
    
    @return A valid AEObject.  In case of error an exception is thrown.
    @throws UnknownElementException If the class ID is unknown to the container.
    
    @note   In general, the "id-space" for an instance of class A includes instances of classes 
            derived from A.
    
    The default implementation iterates over the elements of the given class, looking for a 
    match on the unique id (a.k.a. @c pID) property.  So the elements need to implement that 
    property.
*/
AEObjectPtr
AEObject::GetElementByUniqueID(
    DescType        inElementType,  //!< The base class ID of the element;  must match the application's AppleScript dictionary.
    SInt32          inUniqueID)     //!< The element's unique id.
    const
{
    size_t  numElements = CountElements(inElementType);
    
    for (size_t i = 0; i < numElements; i++)
    {
        AEObjectPtr obj = GetElementByIndex(inElementType, i);
        
        if (obj->GetUniqueID() == inUniqueID)
            return obj;
    }
    
    B_THROW(AENoSuchObjectException());
    
    // Prevent compiler complaints. Control can't ever actually reach here.
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing elements accessible by name may override this function to 
    provide a more efficient implementation.
    
    @return A valid AEObject.  In case of error an exception is thrown.
    @throws UnknownElementException If the class ID is unknown to the container.
    
    @note   In general, the "name-space" for an instance of class A includes instances of classes 
            derived from A.
    
    The default implementation iterates over the elements of the given class, looking for a 
    match on the name (a.k.a. @c pName) property.  So the elements need to implement that 
    property.
*/
AEObjectPtr
AEObject::GetElementByName(
    DescType        inElementType,  //!< The base class ID of the element;  must match the application's AppleScript dictionary.
    const String&   inName)         //!< The element's name.
    const
{
    size_t  numElements = CountElements(inElementType);
    
    for (size_t i = 0; i < numElements; i++)
    {
        AEObjectPtr obj = GetElementByIndex(inElementType, i);
        
        if (AEObjectSupport::CompareStrings(obj->GetName(), inName))
            return obj;
    }
    
    B_THROW(AENoSuchObjectException());
    
    // Prevent compiler complaints. Control can't ever actually reach here.
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
void
AEObject::GetAllElements(
    DescType        inElementType,  //!< The base class ID of the element;  must match the application's AppleScript dictionary.
    std::list<AEObjectPtr>& outElements)
    const
{
    size_t  numElements = CountElements(inElementType);
    
    outElements.clear();

    for (size_t i = 0; i < numElements; i++)
    {
        outElements.push_back(GetElementByIndex(inElementType, i));
    }
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing properties implemented as full-fledged AEObjects may 
    override this function.
    
    Often, properties are simple values such as integers or strings.  Sometimes however, they 
    are more complex and may even be represented by AEObjects in their own right.  This is 
    where this function becomes useful.
    
    Whenever AppleScript needs a property, this function is called, giving a container the 
    opportunity to return an object as the property's "value".  If the function returns @c NULL, 
    it's assumed that the property is "simple" (i.e., representable in an Apple %Event descriptor) 
    and the ReadProperty() or WriteProperty() function is called subsequently.
    
    The default implementation always returns @c NULL.
*/
AEObjectPtr
AEObject::GetPropertyObject(
    DescType        /* inPropertyID */) //!< The property ID;  must match the application's AppleScript dictionary.
    const
{
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing properties implemented as simple values must override this 
    function.
    
    @throws AENoSuchObjectException If the property ID is unknown to the container;  this 
                                    should not happen however, because MOSL only feeds the 
                                    class property IDs that have been declared a priori.
    
    The default implementation throws an exception.  Note that MoreOSL handles the 
    @c pProperties properties.
*/
void
AEObject::WriteProperty(
    DescType        inPropertyID,   //!< The property ID;  must match the application's AppleScript dictionary.
    AEWriter&       ioWriter)       //!< The Apple %Event descriptor stream to fill with the property's value.
    const
{
    switch (inPropertyID)
    {
    case pClass:
        ioWriter.Write<typeType>(GetClassID());
        break;
        
    case pProperties:
        WritePropertiesProperty(ioWriter, false);
        break;
        
    default:
        B_THROW(AENoSuchObjectException());
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
AEObject::WritePropertiesProperty(
    AEWriter&       ioWriter,           //!< The Apple %Event descriptor stream to fill with the property's value.
    bool            inWriteableOnly)    //!< If true, only writeable properties should be written out.
    const
{
    const AEInfo::ClassInfo&    classInfo   = GetClassInfo();
    std::vector<DescType>       propertyIDs;
    
    propertyIDs.reserve(classInfo.mProperties.size());
    
    boost::tribool  writeable   = boost::indeterminate;
    
    if (inWriteableOnly)
        writeable = true;
        
    GetPropertyIDs(true, writeable, std::back_inserter(propertyIDs));
    WriteProperties(propertyIDs.begin(), propertyIDs.end(), ioWriter);
}

// ------------------------------------------------------------------------------------------
void
AEObject::WriteOneProperty(
    AEObjectPtr     inObject, 
    AEWriter&       ioWriter, 
    DescType        inPropertyID)
{
    ioWriter << AEKey(inPropertyID);
    
    AEToken token(inObject, inPropertyID);
    
    AEObject::WriteTokenValue(token, ioWriter);
}

// ------------------------------------------------------------------------------------------
bool
AEObject::TestProperty(
    const AEInfo::PropertyInfo& inPropertyInfo,
    boost::tribool              inInProperties,
    boost::tribool              inWriteable)
{
    bool    good    = true;
    
    if (!boost::indeterminate(inInProperties))
    {
        good = (inInProperties == inPropertyInfo.mInProperties);
    }
    
    if (good && !boost::indeterminate(inWriteable))
    {
        good = (inWriteable == ((inPropertyInfo.mAccess & AEInfo::kAccessWrite) != 0));
    }
    
    return good;
}

// ------------------------------------------------------------------------------------------
/*! A derived class containing modifiable properties implemented as simple values must 
    override this function.
    
    @throws AENoSuchObjectException     If the property ID is unknown to the container;  
                                        this should not happen however, because MOSL filters 
                                        unknown property IDs prior to calling this function.
    @throws AENotModifiableException    If the property is non-modifiable;  this should not 
                                        happen however, because MOSL filters non-modifiable 
                                        properties prior to calling this function.
    
    The default implementation throws an exception.
*/
void
AEObject::ReadProperty(
    DescType        inPropertyID,   //!< The property ID;  must match the application's AppleScript dictionary.
    AEReader&       ioReader)       //!< The Apple %Event descriptor stream containing the property's new value.
{
    switch (inPropertyID)
    {
    case 0:
        B_THROW(AENotModifiableException());
        break;
        
    case pProperties:
        ReadPropertiesProperty(ioReader, false);
        break;
        
    default:
        B_THROW(AENoSuchObjectException());
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
AEObject::ReadPropertiesProperty(
    AEReader&       ioReader,
    bool            inIgnoreReadOnly)   //!< If true, only writeable properties should be read.
{
    const AEInfo::ClassInfo&    classInfo   = AEObjectSupport::Get().GetClassInfo(GetClassID());
    size_t                      count       = ioReader.Count();
    
    for (size_t i = 1; i <= count; i++)
    {
        B::AutoAEReaderDescNth  autoDesc(ioReader, i);
        DescType                propertyID  = autoDesc.GetKeyword();
        AEInfo::PropertyMap::const_iterator pit = classInfo.mProperties.find(propertyID);
        
        if (pit == classInfo.mProperties.end())
            B_THROW(AENoSuchObjectException());
        
        // If the caller only wants to set writeable properties, ignore read-only ones.
        if (inIgnoreReadOnly && !(pit->second.mAccess & AEInfo::kAccessWrite))
            continue;
        
        ReadProperty(propertyID, ioReader);
    }
}

// ------------------------------------------------------------------------------------------
void
AEObject::WriteValue(
    AEWriter&       ioWriter)       //!< The Apple %Event descriptor stream to fill with the object's value.
    const
{
    MakeSpecifier(ioWriter);
}

// ------------------------------------------------------------------------------------------
void
AEObject::AccessElements(
    const AEInfo::ClassInfo&    inClassInfo,
    DescType                    inDesiredClass, 
    DescType                    inKeyForm, 
    const AEDesc&               inKeyData,
    AEDesc&                     outTokenDesc)
    const
{
    const AEInfo::ElementInfo*  elementInfo = NULL;
    
    switch (inKeyForm)
    {
    case formAbsolutePosition:
    case formName:
    case formUniqueID:
    case formRange:
    case formRelativePosition:
    case formTest:
        {
            // Check that the requested element type supports the requested key form.
            
            AEInfo::ElementMap::const_iterator  eit = inClassInfo.mElements.find(inDesiredClass);
            
            if (eit == inClassInfo.mElements.end())
                B_THROW(AEClassHasNoElementsOfThisTypeException());
            
            elementInfo = &eit->second;
            
            if (elementInfo->mKeyForms.find(inKeyForm) == elementInfo->mKeyForms.end())
                B_THROW(AEBadKeyFormException());
        }
        break;
        
    default:
        break;
    }

    switch (inKeyForm)
    {
    case formAbsolutePosition:
        AccessElementsByAbsolutePos(inClassInfo, *elementInfo, inKeyData, outTokenDesc);
        break;
        
    case formName:
        AccessElementsByName(inClassInfo, *elementInfo, inKeyData, outTokenDesc);
        break;
        
    case formUniqueID:
        AccessElementsByUniqueID(inClassInfo, *elementInfo, inKeyData, outTokenDesc);
        break;
        
    case formRange:
        AccessElementsByRange(inClassInfo, *elementInfo, inKeyData, outTokenDesc);
        break;
        
    case formRelativePosition:
        AccessElementsByRelativePosition(inClassInfo, *elementInfo, inKeyData, outTokenDesc);
        break;
        
    case formTest:
        AccessElementsByTest(inClassInfo, *elementInfo, inKeyData, outTokenDesc);
        break;
        
    case formPropertyID:
        AccessProperty(inClassInfo, inKeyData, outTokenDesc);
        break;
        
    case formUserPropertyID:
        AccessUserProperty(inClassInfo, inKeyData, outTokenDesc);
        break;

    default:
        B_THROW(AEBadKeyFormException());
        break;
    }
}

// ------------------------------------------------------------------------------------------
// ClassAccessorByName is a sub-routine of ClassOSLAccessorProc that
// handles requests for access by formName.  The basic approach
// is to extract the name from the selectionData then call
// the class’s "accessByName" object primitive.
void
AEObject::AccessElementsByName(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEInfo::ElementInfo&  inElementInfo,
    const AEDesc&               inKeyData,
    AEDesc&                     outTokenDesc) const
{
    AEToken token;
    String  name;
    
    DescParam<typeUTF16ExternalRepresentation>::Get(inKeyData, name);
    token.SetObject(GetElementByName(inElementInfo.mName, name));
    token.Commit(outTokenDesc);
    
#if 0
    OSStatus              err;
    MOSLClassAccessByName accessByName;
    MOSLToken             valueTok;

    assert(thisClass < gClassTableSize);
    assert(containerTok  != NULL);
    assert(containerTok->tokType    != typeProperty);
    assert(containerTok->tokObjType == gClassTable[thisClass].classID);
    assert(selectionData != NULL);
    assert(value         != NULL);
    
    MoreAENullDesc(value);

    err = noErr;
    accessByName = gClassTable[thisClass].accessByName;
    if (accessByName == NULL) {
        err = errAEBadKeyForm;
    }
    if (err == noErr) {
        assert(containerTok->tokType != typeProperty);

        if (gDebugFlags & kMOSLLogCallbacksMask) {
            BBLogLine("\paccessByName");
            BBLogIndent();
            BBLogMOSLToken("\pcontainerTok", containerTok);
            BBLogDescType("\pdesiredClass", desiredClass);
            BBLogDesc("\pname", selectionData);
        }

        err = accessByName(containerTok, desiredClass, selectionData, &valueTok);

        if (gDebugFlags & kMOSLLogCallbacksMask) {
            BBLogMOSLToken("\p<valueTok", &valueTok);
            BBLogOutdentWithErr(err);
        }
    }
    if (err == noErr) {
        err = MOSLTokenToDesc(&valueTok, value);
    }

    return err;
#endif
}

// ------------------------------------------------------------------------------------------
// ClassAccessorByUniqueID is a sub-routine of ClassOSLAccessorProc that
// handles requests for access by formUniqueID.  The basic approach
// is to extract the unique ID from the selectionData then call
// the class’s "accessByUniqueID" object primitive.
void
AEObject::AccessElementsByUniqueID(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEInfo::ElementInfo&  inElementInfo,
    const AEDesc&               inKeyData,
    AEDesc&                     outTokenDesc) const
{
    AEToken token;
    SInt32  uniqueID;
    
    DescParam<typeSInt32>::Get(inKeyData, uniqueID);
    token.SetObject(GetElementByUniqueID(inElementInfo.mName, uniqueID));
    token.Commit(outTokenDesc);
    
#if 0
    OSStatus                  err;
    MOSLClassAccessByUniqueID accessByUniqueID;
    SInt32                    uniqueID;
    MOSLToken                 valueTok;

    assert(thisClass < gClassTableSize);
    assert(containerTok  != NULL);
    assert(containerTok->tokType    != typeProperty);
    assert(containerTok->tokObjType == gClassTable[thisClass].classID);
    assert(selectionData != NULL);
    assert(value         != NULL);

    MoreAENullDesc(value);
    
    accessByUniqueID = gClassTable[thisClass].accessByUniqueID;
    if (accessByUniqueID == NULL) {
        err = errAEBadKeyForm;
    } else {
        err = MOSLCoerceObjDescToPtr(selectionData, typeLongInteger, &uniqueID, sizeof(uniqueID));
    }
    if (err == noErr) {
    
        // Log the client callback if that logging is enabled.
        
        if (gDebugFlags & kMOSLLogCallbacksMask) {
            BBLogLine("\paccessByUniqueID");
            BBLogIndent();
            BBLogMOSLToken("\pcontainerTok", containerTok);
            BBLogDescType("\pdesiredClass", desiredClass);
            BBLogLong("\puniqueID", uniqueID);
        }

        err = accessByUniqueID(containerTok, desiredClass, uniqueID, &valueTok);

        if (gDebugFlags & kMOSLLogCallbacksMask) {
            BBLogMOSLToken("\p<valueTok", &valueTok);
            BBLogOutdentWithErr(err);
        }
    }
    if (err == noErr) {
        err = MOSLTokenToDesc(&valueTok, value);
    }
    
    return err;
#endif
}

// ------------------------------------------------------------------------------------------
// ClassAccessorByAbsolutePos is a sub-routine of ClassOSLAccessorProc that
// handles requests for access by formAbsolutePosition.  The basic approach
// is to extract the selectionData and build a token (or a list of tokens)
// by calling the class’s "counter" and "accessByIndex" object primitives.
void
AEObject::AccessElementsByAbsolutePos(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEInfo::ElementInfo&  inElementInfo,
    const AEDesc&               inKeyData,
    AEDesc&                     outTokenDesc) const
{
    size_t  elemCount   = CountElements(inElementInfo.mName);
    size_t  elemIndex;
    bool    wantsAll;
    
    ConvertIndexedKeyData(inKeyData, elemCount, elemIndex, wantsAll);

    if (wantsAll)
    {
        std::list<AEObjectPtr>  elements;
        AEWriter                writer;
        
        GetAllElements(inElementInfo.mName, elements);
        
        {
            AutoAEWriterList    autoList(writer);

            std::for_each(elements.begin(), elements.end(), 
                          boost::bind(WriteToken, _1, boost::ref(writer)));
        }
        
        writer.Close(outTokenDesc);
    }
    else
    {
        AEToken token(GetElementByIndex(inElementInfo.mName, elemIndex));
        
        token.Commit(outTokenDesc);
    }
    
#if 0
    OSStatus               err;
    Boolean                wantsAll;
    SInt32                 elementCount;
    SInt32                 elementIndex;
    MOSLToken              valueTok;

    assert(thisClass < gClassTableSize);
    assert(containerTok  != NULL);
    assert(containerTok->tokType    != typeProperty);
    assert(containerTok->tokObjType == gClassTable[thisClass].classID);
    assert(selectionData != NULL);
    assert(value         != NULL);
    
    MoreAENullDesc(value);

    // First call the class’s "counter" primitive to count the number
    // of elements of this type in the object.  We need this count in
    // a number of places later in this routine, so we might as well
    // just go ahead and get it now.
    
    if (gClassTable[thisClass].accessByIndex == NULL) {
        err = kMOSLClassHasNoElementsOfThisTypeErr;
    } else {
        err = CallCounter(thisClass, containerTok, desiredClass, &elementCount);
    }

    // Now extract the selectionData and use it to calculate two
    // variables, wantsAll and elementIndex.  If wantsAll is true, the
    // end of this routine calls CreateTokenList to create a list of all
    // the elements within the object.  If wantsAll is false, elementIndex
    // is set to the index of the element that we’ve selected.  This index
    // is the real index, ranging from 1 to elementCount.
    
    if (err == noErr) {
        err = ConvertIndexedSelectionData(selectionData, elementCount, &wantsAll, &elementIndex);       
    }
    
    // The above code has figured out exactly what element we want from the object.
    // Let’s go call the "accessByIndex" object primitive to get a token for it
    // (or a list of tokens if wantsAll is set).

    if (err == noErr) {
        // I’ve disabled the following check because there are circumstances under which
        // elementIndex can be out of bounds.  Specifically, if the script asks for
        // "window 6389", it’s up to the "accessByIndex" object primitive
        // to reject the request with a errAENoSuchObject error.
        //
        // assert(wantsAll || ((elementIndex >= 1) && (elementIndex <= elementCount)));
        if (wantsAll) {
            err = CreateTokenList(thisClass, containerTok, desiredClass, 1, elementCount, value);
        } else {
            if (err == noErr) {
                err = CallAccessByIndex(thisClass, containerTok, desiredClass, elementIndex, &valueTok);
            }
            if (err == noErr) {
                err = MOSLTokenToDesc(&valueTok, value);
            }
        }
    }

    if (err != noErr) {
        MoreAEDisposeDesc(value);
    }

    return err;
#endif
}

// ------------------------------------------------------------------------------------------
// ClassAccessorByRange is a sub-routine of ClassOSLAccessorProc that handles
// requests for access by formRange.  The basic approach is to extract 
// determine the range boundary objects (using ResolveBoundsToken) and 
// then run over the elements in the object looking for all elements that
// are compatible with the desiredClass and that fall between the boundary
// objects (inclusive).  This is pretty scary stuff.
void
AEObject::AccessElementsByRange(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEInfo::ElementInfo&  /* inElementInfo */,
    const AEDesc&               inKeyData,
    AEDesc&                     /* outTokenDesc */) const
{
    AEDescriptor    rangeRecord;
    AEObjectPtr     startObj, stopObj;
    
    // Extract tokens for the boundary objects from the range record
    
    AEObjectSupport::CoerceDesc(inKeyData, typeAERecord, rangeRecord);
    
    startObj    = ResolveBoundsToken(rangeRecord, keyAERangeStart);
    stopObj     = ResolveBoundsToken(rangeRecord, keyAERangeStop);
    
#if 0
    OSStatus err;
    AERecord rangeRecord;
    MOSLToken boundsBaseTok1;
    MOSLToken boundsBaseTok2;
    SInt32    elementCount;
    SInt32    elementIndex;
    enum { kLookingForFirst, kLookingForSecond, kDone } state;
    MOSLToken thisElementTok;
    MOSLToken thisElementBase;
    AEDesc    thisElementTokDesc;
    Boolean   found;
    DescType  baseClass = 0;
    
    MoreAENullDesc(value);
    MoreAENullDesc(&rangeRecord);

    // Extract tokens for the boundary objects from the range record
    
    err = AECoerceDesc(selectionData, typeAERecord, &rangeRecord);
    if (err == noErr) {
        err = ResolveBoundsToken(&rangeRecord, keyAERangeStart, &boundsBaseTok1);
    }
    if (err == noErr) {
        err = ResolveBoundsToken(&rangeRecord, keyAERangeStop, &boundsBaseTok2);
    }
    if (err == noErr) {

        // The boundary objects can’t be properties.  ResolveBoundsToken should’ve
        // errored if that was the case, so these asserts are just reminders (-:

        assert(boundsBaseTok1.tokType != typeProperty);
        assert(boundsBaseTok2.tokType != typeProperty);

        // The boundary objects must be of a compatible type.  For example,
        // you can’t ask for:
        //
        //      node windows 1 thru (node 1 of window 1)
        //
        // because node 1 isn’t derived from the same base class as window 1.
        // Because ResolveBoundsToken has already coerced the token down to its
        // base type, we can just compare the types here.
        //
        // We would catch this erroneous construct again later in this routine
        // (see where we raise kMOSLBoundaryNotInSameContainerErr) but this
        // will yield a more sensible error message.
        
        if (boundsBaseTok1.tokType != boundsBaseTok2.tokType) {
            err = kMOSLBoundaryMustBeCompatibleErr;
        }
    }
    
    // Create the output list.
    
    if (err == noErr) {
        err = AECreateList(NULL, 0, false, value);
    }

    // Determine the base type of the elements over which we’re iterating.  We do this
    // by calling the "accessByIndex" primitive to get the first object of the desired
    // type, then coercing that object down to its base type.
    //
    // This has the side effect of raising an errAENoSuchObject if there are no
    // objects of the desired class in the container, which is the right thing to do.
    
    if (err == noErr) {
        err = CallAccessByIndex(thisClass, containerTok, desiredClass, 1, &thisElementTok);
        if (err == noErr) {
            err = CallClassCoerceToken(&thisElementTok, cObject, &thisElementBase);
        }
        if (err == noErr) {
            baseClass = thisElementBase.tokType;
            if (baseClass != boundsBaseTok1.tokType) {
                err = kMOSLBoundaryMustBeCompatibleErr;
            }
        }
    }   

    // Iterate over each base class element in the container running a little state
    // machine.  First we look for the first bounding element.  Once we find it,
    // we start looking for the second bounding element.  While looking for the
    // second bounding element we append any element compatible with the desired class to
    // the output list.  If we find the second bounding element, we’re done and we leave.

    if (err == noErr) {
        err = CallCounter(thisClass, containerTok, baseClass, &elementCount);
    }
    if (err == noErr) {
        state = kLookingForFirst;
        for (elementIndex = 1; elementIndex <= elementCount; elementIndex++) {
        
            // Get the elementIndex'th element of container.  This returns a token of the
            // best type of the element, so we coerce it down to the base type.
            
            err = CallAccessByIndex(thisClass, containerTok, baseClass, elementIndex, &thisElementTok);
            if (err == noErr) {
                err = CallClassCoerceToken(&thisElementTok, cObject, &thisElementBase);
            }
            
            // Now, if we’re looking for the first boundary, compare the base token
            // of this element to the first bounds.  If it matches, we switch over
            // to looking for the second boundary, and fall through to code that will
            // put this element in the output list.  If we don’t find it, we also
            // compare against the second bounding element.  We do this because
            // both AppleScript and the Finder treat "element 1 through 3 of foo"
            // as equivalent to "element 3 through 1 of foo".
            
            if (err == noErr && state == kLookingForFirst) {
                err = CompareMOSLTokens(kAEEquals, &thisElementBase, &boundsBaseTok1, &found);
                if (err == noErr) {
                    if (found) {
                        state = kLookingForSecond;
                    } else {
                        err = CompareMOSLTokens(kAEEquals, &thisElementBase, &boundsBaseTok2, &found);
                        if (err == noErr) {
                            if (found) {
                                // Put the other bounding element into boundsBaseTok2 so that
                                // the code below will find it.
                                boundsBaseTok2 = boundsBaseTok1;
                                state = kLookingForSecond;
                            }
                        }
                    }
                }
            }
            
            // Now, if we’re looking for the second bounding element, we add the current
            // element to the output list and then compare this element to the second
            // bounding element.  If it matches, we’re done.
            
            if (err == noErr && state == kLookingForSecond) {
                if ( CallClassCoerceToken(&thisElementTok, desiredClass, NULL) == noErr ) {
                    err = MOSLTokenToDesc(&thisElementTok, &thisElementTokDesc);
                    if (err == noErr) {
                        err = AEPutDesc(value, 0, &thisElementTokDesc);
                        MoreAEDisposeDesc(&thisElementTokDesc);
                    }
                }
                if (err == noErr) {
                    err = CompareMOSLTokens(kAEEquals, &thisElementBase, &boundsBaseTok2, &found);
                }
                if (err == noErr && found) {
                    state = kDone;
                }
            }
            if (err != noErr || state == kDone) {
                break;
            }
        }
    }
    
    // If we ran off the end of the elements of the container without finding
    // both bounding elements, the bounding elements were in error (they
    // probably weren’t both elements of the container), so we spit out an error.
    
    if (err == noErr && state != kDone) {
        err = kMOSLBoundaryNotInSameContainerErr;
    }
    
    // If we didn’t find any any compatible objects within the boundary objects,
    // we raise an error.  This is inline with the Finder’s behaviour, and also
    // in the spirit of the comments on p174 of the "AppleScript Language Guide".
    
    if (err == noErr) {
        err = AECountItems(value, &elementCount);
        if ((err == noErr) && (elementCount == 0)) {
            err = errAENoSuchObject;
        }
    }
    
    MoreAEDisposeDesc(&rangeRecord);
    if (err != noErr) {
        MoreAEDisposeDesc(value);
    }
    
    return err;
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObject::AccessElementsByRelativePosition(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEInfo::ElementInfo&  /* inElementInfo */,
    const AEDesc&               /* inKeyData */,
    AEDesc&                     /* outTokenDesc */) const
{
}

// ------------------------------------------------------------------------------------------
void
AEObject::AccessElementsByTest(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEInfo::ElementInfo&  /* inElementInfo */,
    const AEDesc&               /* inKeyData */,
    AEDesc&                     /* outTokenDesc */) const
{
}

// ------------------------------------------------------------------------------------------
void
AEObject::AccessProperty(
    const AEInfo::ClassInfo&    inClassInfo,
    const AEDesc&               inKeyData, 
    AEDesc&                     outTokenDesc) const
{
    DescType    propertyID;
    
    DescParam<typeType>::Get(inKeyData, propertyID);
    
    AccessPropertyByID(inClassInfo, propertyID, outTokenDesc);
}

// ------------------------------------------------------------------------------------------
void
AEObject::AccessPropertyByID(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    DescType                    inPropertyID,
    AEDesc&                     outTokenDesc) const
{
//  const AEInfo::PropertyInfo& propertyInfo    = inClassInfo.mProperties.find(inPropertyID)->second;
    
//  if (propertyInfo.mIsList)
//  {
//  }
//  else
    {
        AEToken     token;
        
        AEObjectPtr propertyObj = GetPropertyObject(inPropertyID);
        
        if (propertyObj != NULL)
        {
            // Property token that maps to an object.
            
            token.SetObject(propertyObj);
        }
        else
        {
            // Property token that maps to data.
            
            token.SetProperty(const_cast<AEObject*>(this)->GetAEObjectPtr(), inPropertyID);
        }

        token.Commit(outTokenDesc);
    }
    
#if 0
    OSStatus  err;
    DescType  propName;
    MOSLToken valueTok;

    assert(thisClass < gClassTableSize);
    assert(containerTok  != NULL);
    assert(containerTok->tokType != typeProperty);
    assert(containerTok->tokObjType == gClassTable[thisClass].classID);
    assert(selectionData != NULL);
    assert(value         != NULL);
    
    MoreAENullDesc(value);
    
    err = MOSLCoerceObjDescToPtr(selectionData, typeType, &propName, sizeof(propName));
    if (err == noErr) {
        if (PropertyToPropEntry(thisClass, propName) == NULL) {
            err = errAENoSuchObject;
        }
    }
    if (err == noErr) {
        InitPropertyMOSLTokenFromContainer(&valueTok, containerTok, propName);
        err = MOSLTokenToDesc(&valueTok, value);
    }
    
    return err;
#endif
}

// ------------------------------------------------------------------------------------------
// If you ask for a property of an object with a property name
// that doesn’t exist (for example, wombat of document 1) AppleScript
// recognises that the property doesn’t exist in your dictionary and 
// sends you an object specifier of formUserPropertyID.  The selection
// data is the text of the property name.  Presumably you could use
// this to access properties in attached scripts, but seeing as MOSL
// doesn’t support that sort of thing we just fail out with an error.
// Note that we want to use a different error (no such object) than
// the default case (bad key form).
void
AEObject::AccessUserProperty(
    const AEInfo::ClassInfo&    /* inClassInfo */,
    const AEDesc&               /* inKeyData */, 
    AEDesc&                     /* outTokenDesc */) const
{
}

// ------------------------------------------------------------------------------------------
// This routine converts "access by index" selection data into two results:
// if wantsAll is returned true, the data was kAEAny and the caller should
// create a token that represents all objects; OTOH, if wantsAll is false,
// elementIndex is the index of the specific element.  The elementCount
// parameter is the total number of elements within the container.  It is
// needed so that the routine can correctly handle negative indexes.
//
// This routine was separated out from ClassAccessorByAbsolutePos because 
// I hoped to use it as part of my formRange support in the future.  As
// it turned out, formRange didn’t need this functionality, but I left the
// routine separate anyway because it makes things easier to understand.
void
AEObject::ConvertIndexedKeyData(
    const AEDesc&   inKeyData,
    size_t          inNumElements,
    size_t&         outIndex,
    bool&           outWantsAll) const
{
    // OK, so the selection data is either a typeAbsoluteOrdinal or an integer.
    // If it’s the former, extract the ordinal and switch on its value.  If it’s
    // an integer, extract the integer and then handle any negative values (which
    // means an index from the end).
    
    outIndex    = 0;
    outWantsAll = false;
    
    if (inKeyData.descriptorType == typeAbsoluteOrdinal)
    {
        OSType  absOrd;
        
        DescParam<typeAbsoluteOrdinal>::Get(inKeyData, absOrd);
        
        switch (absOrd)
        {
        case kAEAll:
            outWantsAll = true;
            break;
            
        case kAEFirst:
            outIndex = 0;
            break;
            
        case kAELast:
            outIndex = inNumElements - 1;
            break;
            
        case kAEMiddle:
            outIndex = (inNumElements + 1) / 2 - 1; // AppleScript Language Guide gives this formula
            break;
            
        case kAEAny:
            outIndex = rand() % inNumElements;
            break;
            
        default:
            B_THROW(AECoercionFailException());
            break;
        }
    }
    else
    {
        SInt32   absPos;
        
        DescParam<typeSInt32>::Get(inKeyData, absPos);

        if (absPos > 0)
            outIndex = absPos - 1;
        else if (absPos < 0)
            outIndex = inNumElements + absPos;
        else
            B_THROW(ConstantOSStatusException<errAENoSuchObject>());
    }
}

// ------------------------------------------------------------------------------------------
AEObjectPtr
AEObject::ResolveBoundsToken(
    const AEDesc&       inKeyData,
    AEKeyword           inKeyword) const
{
    AEAutoTokenDescriptor   tokenDesc;
    AEDescriptor            objSpec, errorDesc;
    OSStatus                err;

    err = AEGetParamDesc(&inKeyData, inKeyword, typeWildCard, objSpec);
    B_THROW_IF_STATUS(err);
    
    return AEObjectSupport::Resolve(objSpec);
    
#if 0
    err = PrivateResolve(objSpec, tokenDesc, errorDesc);
    B_THROW_IF_STATUS(err);
    
    if (AEToken::IsTokenDescriptor(tokenDesc))
    {
        AEToken token(tokenDesc);
        
        if (!token.IsPropertyToken())
        {
            return token.GetObject();
        }
    }
    
    B_THROW(AEBoundaryMustBeObjectException());
    
    // silence compiler
    return AEObjectPtr();
#endif
    
#if 0
    OSStatus err;
    AEDesc   boundsDesc;
    AEDesc   resolvedBoundsDesc;
    MOSLClassIndex junkClass;
    MOSLToken boundsTok;
    
    if (gDebugFlags & kMOSLLogOSLMask) {
        BBLogLine("\pResolveBoundsToken");
        BBLogIndent();
        BBLogDescType("\pkeyword", keyword);
    }

    assert(rangeRecord != NULL);
    assert(rangeRecord->descriptorType == typeAERecord);
    assert(baseTok != NULL);
    
    MoreAENullDesc(&boundsDesc);
    MoreAENullDesc(&resolvedBoundsDesc);

    err = AEGetParamDesc(rangeRecord, keyword, typeWildCard, &boundsDesc);
    if (err == noErr) {
        BBLogDesc("\pboundsDesc", &boundsDesc);
        err = AEResolve(&boundsDesc, kAEIDoMinimum, &resolvedBoundsDesc);
    }
    if (err == noErr) {
        BBLogDesc("\presolvedBoundsDesc", &resolvedBoundsDesc);
        if ( ClassIDToClassIndex(resolvedBoundsDesc.descriptorType, &junkClass) != noErr ) {
            err = kMOSLBoundaryMustBeObjectErr;
        }
    }
    if (err == noErr) {
        DescToMOSLToken(&resolvedBoundsDesc, &boundsTok);
    }
    if (err == noErr) {
        err = CallClassCoerceToken(&boundsTok, cObject, baseTok);
    }

    MoreAEDisposeDesc(&boundsDesc);
    MoreAEDisposeDesc(&resolvedBoundsDesc);
    
    if (gDebugFlags & kMOSLLogOSLMask) {
        BBLogMOSLToken("\p<baseTok", baseTok);
        BBLogOutdentWithErr(err);
    }

    return err;
#endif
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "duplicate" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    @return A valid AEObject.  In case of error an exception is thrown.
    
    The default implementation throws an exception.
*/
AEObjectPtr
AEObject::CloneObject(
    DescType        /* inPosition */,   //!< The position of the duplicate.
    AEObjectPtr     /* inTarget */,     //!< The container or sibling of the new AEObject.
    const AERecord& /* inProperties */) //!< The new AEObject's properties (optional).
{
    B_THROW(AEEventNotHandledException());
    
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "close" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    @return A valid AEObject.  In case of error an exception is thrown.
    
    The default implementation throws an exception.
*/
void
AEObject::CloseObject(
    OSType          /* inSaveOption */ /* = kAEAsk */,  //!< A hint as to the behaviour to adopt if the AEObject has unsaved changes (optional).
    const Url&      /* inUrl */ /* = Url() */)          //!< The URL if the file in which to save the AEObject (optional).
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "make" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    @return A valid AEObject.  In case of error an exception is thrown.
    
    @note   The object (i.e. "this") is always the container of the to-be-created AEObject.  
            @a InTarget will be either the object or one of the object's elements, depending on 
            the value of @a inPosition.
    
    The default implementation throws an exception.
*/
AEObjectPtr
AEObject::CreateObject(
    DescType        /* inObjectClass */,    //!< The class ID of the new AEObject.
    DescType        /* inPosition */,       //!< The position of the new AEObject.
    AEObjectPtr     /* inTarget */,         //!< The container or sibling of the new AEObject.
    const AERecord& /* inProperties */,     //!< The new AEObject's properties (optional).
    const AEDesc&   /* inData */)           //!< The new AEObject's intrinsic data (optional).
{
    B_THROW(AEEventNotHandledException());
    
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "delete" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    The default implementation throws an exception.
*/
void
AEObject::DeleteObject()
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "move" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    @return A valid AEObject.  In case of error an exception is thrown.  The returned object may be "this".
    
    The default implementation throws an exception.
*/
AEObjectPtr
AEObject::MoveObject(
    DescType    /* inPosition */,   //!< The new position of the object.
    AEObjectPtr /* inTarget */)     //!< The object's new container or sibling (depending on the value of @a inPosition).
{
    B_THROW(AEEventNotHandledException());
    
    return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "open" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    The default implementation throws an exception.
*/
void
AEObject::OpenObject()
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "print" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    The default implementation throws an exception.
*/
void
AEObject::PrintObject(
    PrintSettings*  /* inSettings */,   //!< The print settings to use.  May be @c NULL.
    Printer*        /* inPrinter */,    //!< The printer to print to.  May be @c NULL.
    bool            /* inShowDialog */) //!< Should we display the print dialog?
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "save" event @b must override this function.
    Consult the Apple Event Registry for more information.
    
    The default implementation throws an exception.
*/
void
AEObject::SaveObject(
    const Url&      /* inUrl */ /* = Url() */,          //!< The URL if the file in which to save the AEObject (optional).
    const String&   /* inObjectType */ /* = String() */)    //!< The HFS file type of the file in which to save the AEObject (optional).
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "revert" event @b must override this 
    function. Consult the Apple Event Registry for more information.
    
    The default implementation throws an exception.
*/
void
AEObject::RevertObject()
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! A derived class implementing the AppleScript "activate" event @b must override this 
    function. Consult the Apple Event Registry for more information.
    
    The default implementation throws an exception.
*/
void
AEObject::ActivateObject()
{
    B_THROW(AEEventNotHandledException());
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
    
    @return The newly-created AEObject.
*/
AEObjectPtr
AEObject::SendCloneAppleEvent(
    DescType        inPosition,                 //!< The position of the duplicate.
    AEObjectPtr     inTarget,                   //!< The container or sibling of the new AEObject.
    const AERecord* inProperties /* = NULL */,  //!< The new AEObject's properties (optional).
    AESendMode      inMode /* = 0 */)           //!< The mode for AESend().
{
    AEObjectPtr newObject;
    
    AEEvent<kAECoreSuite, kAEClone>::Send(
        AESelfTarget(),
        this, 
        InsertionLocationSpecifier(inPosition, inTarget), 
        AEEventFunctor::OptionalAEDesc(inProperties), 
        AEEventFunctor::DescParamResult<typeObjectSpecifier>(newObject), 
        inMode);
    
    B_ASSERT(newObject != NULL);
    
    return (newObject);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendCloseAppleEvent(
    OSType      inSaveOption /* = kAEAsk */,    //!< A hint as to the behaviour to adopt if the AEObject has unsaved changes (optional).
    const Url&  inUrl /* = Url() */,            //!< The URL if the file in which to save the AEObject (optional).
    AESendMode  inMode /* = 0 */)               //!< The mode for AESend().
{
    AEEvent<kAECoreSuite, kAEClose>::Send(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalSaveOption(inSaveOption), 
        AEEventFunctor::OptionalUrl(inUrl), 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
SInt32
AEObject::SendCountElementsAppleEvent(
    DescType    inObjectClass,      //!< The class ID of the elements to count.
    AESendMode  inMode /* = 0 */)   //!< The mode for AESend().
    const
{
    SInt32  count   = 0;
    
    AEEvent<kAECoreSuite, kAECountElements>::Send(
        AESelfTarget(),
        this, 
        AETypedObject<typeType>(inObjectClass), 
        AEEventFunctor::DescParamResult<typeSInt32>(count), 
        inMode);
    
    return (count);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
    
    @note   The newly-created AEObject will be either an element or a sibling of the 
            object, depending on the value of @a inPosition.
*/
AEObjectPtr
AEObject::SendCreateElementAppleEvent(
    DescType        inObjectClass,              //!< The class ID of the new AEObject.
    DescType        inPosition,                 //!< The position of the new AEObject.
    AEObjectPtr     inTarget,                   //!< The object to which @a inPosition is relative.
    const AERecord* inProperties /* = NULL */,  //!< The new AEObject's properties (optional).
    const AEDesc*   inData /* = NULL */,        //!< The new AEObject's intrinsic data (optional).
    AESendMode      inMode /* = 0 */)           //!< The mode for AESend().
{
    AEObjectPtr newObject;
    
    AEEvent<kAECoreSuite, kAECreateElement>::Send(
        AESelfTarget(),
        AETypedObject<typeType>(inObjectClass), 
        InsertionLocationSpecifier(inPosition, inTarget), 
        AEEventFunctor::OptionalAEDesc(inProperties), 
        AEEventFunctor::OptionalAEDesc(inData), 
        AEEventFunctor::DescParamResult<typeObjectSpecifier>(newObject), 
        inMode);
    
    B_ASSERT(newObject != NULL);
    
    return (newObject);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendDeleteAppleEvent(
    AESendMode  inMode /* = 0 */)   //!< The mode for AESend().
{
    AEEvent<kAECoreSuite, kAEDelete>::Send(
        AESelfTarget(),
        this, 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendGetDataAppleEvent(
    AEDesc&     outValue,                               //!< The output value.
    DescType    inRequestedType /* = typeWildCard */,   //!< The requested type of the data.
    AESendMode  inMode /* = 0 */)                       //!< The mode for AESend().
    const
{
    AEEvent<kAECoreSuite, kAEGetData>::Send(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalRequestedType(inRequestedType), 
        AEEventFunctor::AEDescResult(outValue), 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendGetPropertyAppleEvent(
    DescType    inPropertyID,                           //!< The property ID;  must match the application's AppleScript dictionary.
    AEDesc&     outValue,                               //!< The output value.
    DescType    inRequestedType /* = typeWildCard */,   //!< The requested type of the data.
    AESendMode  inMode /* = 0 */)                       //!< The mode for AESend().
    const
{
    AEEvent<kAECoreSuite, kAEGetData>::Send(
        AESelfTarget(),
        PropertySpecifier(GetAEObjectPtr(), inPropertyID), 
        AEEventFunctor::OptionalRequestedType(inRequestedType), 
        AEEventFunctor::AEDescResult(outValue), 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
AEObjectPtr
AEObject::SendMoveAppleEvent(
    DescType    inPosition,         //!< The new position of the object.
    AEObjectPtr inTarget,           //!< The object's new container or sibling (depending on the value of @a inPosition).
    AESendMode  inMode /* = 0 */)   //!< The mode for AESend().
{
    AEObjectPtr newObject;
    
    AEEvent<kAECoreSuite, kAEMove>::Send(
        AESelfTarget(),
        this, 
        InsertionLocationSpecifier(inPosition, inTarget), 
        AEEventFunctor::DescParamResult<typeObjectSpecifier>(newObject), 
        inMode);
    
    B_ASSERT(newObject != NULL);
    
    return (newObject);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendOpenAppleEvent(
    AESendMode  inMode /* = 0 */)   //!< The mode for AESend().
{
    AEEvent<kCoreEventClass, kAEOpen>::Send(
        AESelfTarget(),
        this, 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendPrintAppleEvent(
    bool            inShowDialog /* = false */, //!< The "show dialog" flag (optional).
    const AERecord* inProperties /* = NULL */,  //!< The print job properties (optional).
    AESendMode      inMode /* = 0 */)           //!< The mode for AESend().
{
    AEEvent<kCoreEventClass, kAEPrint>::Send(
        AESelfTarget(),
        this, 
        AEOptionalTypedObject<typeBoolean>(inShowDialog, false),
        AEEventFunctor::OptionalAEDesc(inProperties), 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendSaveAppleEvent(
    const Url&      inUrl /* = Url() */,            //!< The URL if the file in which to save the AEObject (optional).
    const String&   inObjectType /* = String() */,  //!< The HFS file type of the file in which to save the AEObject (optional).
    AESendMode      inMode /* = 0 */)               //!< The mode for AESend().
{
    AEEvent<kAECoreSuite, kAESave>::Send(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalUrl(inUrl), 
        AEEventFunctor::OptionalObjectType(inObjectType), 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendRevertAppleEvent(
    AESendMode  inMode /* = 0 */)   //!< The mode for AESend().
{
    AEEvent<kAEMiscStandards, kAERevert>::Send(
        AESelfTarget(),
        this, 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::SendSelectAppleEvent(
    AESendMode  inMode /* = 0 */)   //!< The mode for AESend().
{
    AEEvent<kAEMiscStandards, kAESelect>::Send(
        AESelfTarget(),
        this, 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! The new value is encapsulated with an Apple %Event descriptor.
*/
void
AEObject::SendSetDataAppleEvent(
    const AEDesc&   inValue,            //!< The Apple %Event descriptor containing the new value.
    AESendMode      inMode /* = 0 */)   //!< The mode for AESend().
{
    AEEvent<kAECoreSuite, kAESetData>::Send(
        AESelfTarget(),
        this, 
        inValue, 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! The new property value is encapsulated with an Apple %Event descriptor.
*/
void
AEObject::SendSetPropertyAppleEvent(
    DescType        inPropertyID,       //!< The property ID;  must match the application's AppleScript dictionary.
    const AEDesc&   inValue,            //!< The Apple %Event descriptor containing the property's new value.
    AESendMode      inMode /* = 0 */)   //!< The mode for AESend().
{
    AEEvent<kAECoreSuite, kAESetData>::Send(
        AESelfTarget(),
        PropertySpecifier(GetAEObjectPtr(), inPropertyID), 
        inValue, 
        inMode);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
    
    @note   The newly-created AEObject will be either an element or a sibling of the object 
            (i.e. "this"), depending on the value of @a inPosition.
*/
void
AEObject::MakeCloneAppleEvent(
    DescType        inPosition,     //!< The position of the duplicate.
    AEObjectPtr     inTarget,       //!< The container or sibling of the new AEObject.
    const AERecord* inProperties,   //!< The new AEObject's properties (optional).
    AppleEvent&     outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAEClone>::Make(
        AESelfTarget(),
        this, 
        InsertionLocationSpecifier(inPosition, inTarget), 
        AEEventFunctor::OptionalAEDesc(inProperties), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeCloseAppleEvent(
    OSType      inSaveOption,   //!< A hint as to the behaviour to adopt if the AEObject has unsaved changes (optional).
    const Url&  inUrl,          //!< The URL if the file in which to save the AEObject (optional).
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAEClose>::Make(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalSaveOption(inSaveOption), 
        AEEventFunctor::OptionalUrl(inUrl), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeCountElementsAppleEvent(
    OSType      inObjectClass,  //!< The class ID of the elements to count.
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAECountElements>::Make(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalObjectClass(inObjectClass), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
    
    @note   The newly-created AEObject will be either an element or a sibling of the object 
            (i.e. "this"), depending on the value of @a inPosition.
*/
void
AEObject::MakeCreateElementAppleEvent(
    DescType        inObjectClass,  //!< The class ID of the new AEObject.
    DescType        inPosition,     //!< The position of the new AEObject.
    AEObjectPtr     inTarget,       //!< The object to which @a inPosition is relative.
    const AERecord* inProperties,   //!< The new AEObject's properties (optional).
    const AEDesc*   inData,         //!< The new AEObject's intrinsic data (optional).
    AppleEvent&     outEvent)       //!< The output Apple %Event.
{
    AEEvent<kAECoreSuite, kAECreateElement>::Make(
        AESelfTarget(),
        AETypedObject<typeType>(inObjectClass), 
        InsertionLocationSpecifier(inPosition, inTarget), 
        AEEventFunctor::OptionalAEDesc(inProperties), 
        AEEventFunctor::OptionalAEDesc(inData), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeDeleteAppleEvent(
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAEDelete>::Make(
        AESelfTarget(),
        this, 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeGetDataAppleEvent(
    DescType    inRequestedType,    //!< The requested type of the data (default = typeWildCard).
    AppleEvent& outEvent)           //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAEGetData>::Make(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalRequestedType(inRequestedType), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeGetPropertyAppleEvent(
    DescType    inPropertyID,       //!< The property ID;  must match the application's AppleScript dictionary.
    DescType    inRequestedType,    //!< The requested type of the data (default = typeWildCard).
    AppleEvent& outEvent)           //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAEGetData>::Make(
        AESelfTarget(),
        PropertySpecifier(GetAEObjectPtr(), inPropertyID), 
        AEEventFunctor::OptionalRequestedType(inRequestedType), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeMoveAppleEvent(
    DescType    inPosition,     //!< The new position of the object.
    AEObjectPtr inTarget,       //!< The object's new container or sibling (depending on the value of @a inPosition).
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAEMove>::Make(
        AESelfTarget(),
        this, 
        InsertionLocationSpecifier(inPosition, inTarget), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeOpenAppleEvent(
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kCoreEventClass, kAEOpen>::Make(
        AESelfTarget(),
        this, 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakePrintAppleEvent(
    bool            inShowDialog,   //!< The "show dialog" flag (optional).
    const AERecord* inProperties,   //!< The print job properties (optional).
    AppleEvent&     outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kCoreEventClass, kAEPrint>::Make(
        AESelfTarget(),
        this, 
        AEOptionalTypedObject<typeBoolean>(inShowDialog, false),
        AEEventFunctor::OptionalAEDesc(inProperties), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeSaveAppleEvent(
    const Url&      inUrl,          //!< The URL if the file in which to save the AEObject (optional).
    const String&   inObjectType,   //!< The HFS file type of the file in which to save the AEObject (optional).
    AppleEvent&     outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAESave>::Make(
        AESelfTarget(),
        this, 
        AEEventFunctor::OptionalUrl(inUrl), 
        AEEventFunctor::OptionalObjectType(inObjectType), 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeRevertAppleEvent(
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAEMiscStandards, kAERevert>::Make(
        AESelfTarget(),
        this, 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! Consult the Apple Event Registry for more information.
*/
void
AEObject::MakeSelectAppleEvent(
    AppleEvent& outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAEMiscStandards, kAESelect>::Make(
        AESelfTarget(),
        this, 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! The new property value is encapsulated with an Apple %Event descriptor.
*/
void
AEObject::MakeSetPropertyAppleEvent(
    DescType        inPropertyID,   //!< The property ID;  must match the application's AppleScript dictionary.
    const AEDesc&   inValue,        //!< The Apple %Event descriptor containing the property's new value.
    AppleEvent&     outEvent)       //!< The output Apple %Event.
    const
{
    AEEvent<kAECoreSuite, kAESetData>::Make(
        AESelfTarget(),
        PropertySpecifier(GetAEObjectPtr(), inPropertyID), 
        inValue, 
        outEvent);
}

// ------------------------------------------------------------------------------------------
/*! @attention  All derived classes must override this function.
    
    The default implementation throws an exception.
*/
void
AEObject::MakeSpecifier(
    AEWriter&   /* ioWriter */) //!< The Apple %Event stream into which the object specifier is to be written.
    const
{
    B_THROW(AECantHandleClassException());
}

// ------------------------------------------------------------------------------------------
/*! This is a convenience routine.
*/
void
AEObject::MakePropertySpecifier(
    DescType    inPropertyID,   //!< One of the object's properties.
    AEWriter&   ioWriter)       //!< The Apple %Event stream into which the object specifier is to be written.
    const
{
    BuildPropertySpecifier(GetAEObjectPtr(), inPropertyID, ioWriter);
}

// ------------------------------------------------------------------------------------------
/*! @note   Because the element is identified by its class ID and name and not by a 
            AEObject directly, this function can be used to create object specifiers 
            of objects that don't (yet) exist.
*/
void
AEObject::BuildNameSpecifier(
    ConstAEObjectPtr inContainer,   //!< The container.
    DescType        inObjectClass,  //!< The element's class ID.
    const String&   inName,         //!< The element's name.
    AEWriter&       ioWriter)       //!< The Apple %Event stream into which the object specifier is to be written.
{
    ioWriter << NameSpecifier(inObjectClass, inContainer, inName);
}

// ------------------------------------------------------------------------------------------
/*! @note   Because the element is identified by its class ID and index and not by a 
            AEObject directly, this function can be used to create object specifiers 
            of objects that don't (yet) exist.
*/
void
AEObject::BuildIndexSpecifier(
    ConstAEObjectPtr inContainer,   //!< The container.
    DescType        inObjectClass,  //!< The element's class ID.
    SInt32          inIndex,        //!< The element's index.
    AEWriter&       ioWriter)       //!< The Apple %Event stream into which the object specifier is to be written.
{
    ioWriter << IndexSpecifier(inObjectClass, inContainer, inIndex);
}

// ------------------------------------------------------------------------------------------
/*! @note   Because the element is identified by its class ID and unique id and not by a 
            AEObject directly, this function can be used to create object specifiers 
            of objects that don't (yet) exist.
*/
void
AEObject::BuildUniqueIDSpecifier(
    ConstAEObjectPtr inContainer,   //!< The container.
    DescType        inObjectClass,  //!< The element's class ID.
    SInt32          inUniqueID,     //!< The element's unique id.
    AEWriter&       ioWriter)       //!< The Apple %Event stream into which the object specifier is to be written.
{
    ioWriter << UniqueIDSpecifier(inObjectClass, inContainer, inUniqueID);
}

// ------------------------------------------------------------------------------------------
void
AEObject::BuildPropertySpecifier(
    ConstAEObjectPtr inObject,      //!< The object.
    DescType        inPropertyID,   //!< The property ID.
    AEWriter&       ioWriter)       //!< The Apple %Event stream into which the object specifier is to be written.
{
    ioWriter << PropertySpecifier(inObject, inPropertyID);
}

// ------------------------------------------------------------------------------------------
bool
AEObject::CompareObjects(
    DescType            inComparisonOp,
    ConstAEObjectPtr    inObject1, 
    ConstAEObjectPtr    inObject2)
{
    bool    result  = false;

    if (inObject1.get() != NULL)
    {
        if (inObject2.get() != NULL)
        {
            result = inObject1->Compare(inComparisonOp, inObject2);
        }
        else
        {
            switch (inComparisonOp)
            {
            case kAEEquals:
            case kAELessThan:
            case kAELessThanEquals:
                result = false;
                break;
                
            case kAEGreaterThanEquals:
            case kAEGreaterThan:
                result = true;
                break;
                
            default:
                B_THROW(AEUnrecognisedOperatorException());
                break;
            }
        }
    }
    else if (inObject2.get() != NULL)
    {
        switch (inComparisonOp)
        {
        case kAEEquals:
        case kAEGreaterThanEquals:
        case kAEGreaterThan:
            result = false;
            break;
            
        case kAELessThan:
        case kAELessThanEquals:
            result = true;
            break;
            
        default:
            B_THROW(AEUnrecognisedOperatorException());
            break;
        }
    }
    else
    {
        switch (inComparisonOp)
        {
        case kAEEquals:
        case kAEGreaterThanEquals:
        case kAELessThanEquals:
            result = true;
            break;
            
        case kAEGreaterThan:
        case kAELessThan:
            result = false;
            break;
            
        default:
            B_THROW(AEUnrecognisedOperatorException());
            break;
        }
    }
    
    return (result);
}

// ------------------------------------------------------------------------------------------
bool
AEObject::Compare(
    DescType            inComparisonOp,
    ConstAEObjectPtr    inObject) const
{
    bool        result;
    
    switch (inComparisonOp)
    {
    case kAEEquals:
        result = (this == inObject.get());
        break;
        
    default:
        result = false;
        B_THROW(AEUnrecognisedOperatorException());
        break;
    }
    
    return (result);
}

// ------------------------------------------------------------------------------------------
/*! This is a convenience routine that calls through to AEObjectSupport.
    
    @return A valid AEObject.  If an error occurs an exception is thrown.
*/
AEObjectPtr
AEObject::Resolve(
    const AEDesc&   inDesc)     //!< The object specifier.
{
    return (AEObjectSupport::Get().Resolve(inDesc));
}

// ------------------------------------------------------------------------------------------
/*! This is a convenience routine that calls through to AEObjectSupport.
    
    @return A valid AEObject, or NULL if an error occurs.
*/
AEObjectPtr
AEObject::Resolve(
    const AEDesc&   inDesc,     //!< The object specifier.
    const std::nothrow_t&)      //!< An indication that the caller doesn't want the function to throw an exception.
{
    AEObjectPtr obj;
    OSStatus    err;
    
    err = AEObjectSupport::Get().Resolve(inDesc, obj);
    if (err != noErr)
        obj.reset();
    
    return (obj);
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID> void
AEObject::SetClassEventHandler(
    AEObjectSupport&    ioObjectSupport)
{
    ioObjectSupport.SetClassEventHandler(cObject, EVT_CLASS, EVT_ID, 
                                         boost::bind(DispatchEvent<EVT_CLASS, EVT_ID>, 
                                                     _1, _2, _3));
}

// ------------------------------------------------------------------------------------------
void
AEObject::RegisterClassEventHandlers(
    AEObjectSupport&    ioObjectSupport)
{
    SetClassEventHandler<kAECoreSuite, kAEClone>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAEClose>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAECountElements>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAECreateElement>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAEDelete>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAEDoObjectsExist>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAEGetData>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAEMove>(ioObjectSupport);
    SetClassEventHandler<kCoreEventClass, kAEOpen>(ioObjectSupport);
    SetClassEventHandler<kCoreEventClass, kAEPrint>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAESave>(ioObjectSupport);
    SetClassEventHandler<kAECoreSuite, kAESetData>(ioObjectSupport);
    SetClassEventHandler<kAEMiscStandards, kAERevert>(ioObjectSupport);
    SetClassEventHandler<kAEMiscStandards, kAEActivate>(ioObjectSupport);
}

static inline AEObjectPtr   GetObject(const AEToken& tok)
{
    return tok.GetObject();
}

// ------------------------------------------------------------------------------------------
void
AEObject::WriteTokenValue(
    const AEToken&      inToken, 
    AEWriter&           ioValueWriter)
{
    AEObjectPtr obj = inToken.GetObject();
    
    B_ASSERT(obj != NULL);
    
    if (inToken.IsPropertyToken())
    {
        // We have a property token.
        
        DescType    propertyName    = inToken.GetPropertyName();
        AEObjectPtr propertyObj     = obj->GetPropertyObject(propertyName);
        
        if (propertyObj != NULL)
        {
            // Property token that maps to an object.
            
            propertyObj->WriteValue(ioValueWriter);
        }
        else
        {
            // Property token that maps to data.
            
            obj->WriteProperty(propertyName, ioValueWriter);
        }
    }
    else
    {
        // We have an object or null token.
        
        obj->WriteValue(ioValueWriter);
    }

#if 0
#if 0
    OSStatus         err;
    OSStatus         junk;
    MOSLToken        thisTok;
    AEDesc           thisResult;
    MOSLPropIndex    propertyIndex;
    MOSLPropTablePtr propertyBase;
    DescType         thisPropName;

    assert(thisClass < gClassTableSize);
    assert(tok != NULL);
    assert(gClassTable[thisClass].classID == tok->tokObjType);
    assert((resolution == kReturnTokensAsObjects) || (resolution == kReturnTokensAsTokens));
    assert(result != NULL);
    
    MoreAENullDesc(result);
    
    err = noErr;
    if (gClassTable[thisClass].getter == NULL) {
        // We return the fixed error errAEEventNotHandled in the cases
        // where the getter is NULL.  It doesn’t make any difference what
        // error we return in this case because the getter should never
        // be NULL (otherwise we couldn’t get the object specifier for
        // a token, which is pretty much a required operation).
        err = errAEEventNotHandled;
    }
#endif
    
    // Check whether the class’s property table lists this property; if not
    // we can error without calling the getter.
    
    if (inContainerToken.IsPropertyToken())
    {
        const ClassInfo*    classInfo       = &inContainerInfo;
        DescType            propertyName    = inContainerToken.GetPropertyName();
        bool                found           = false;
        
        do
        {
            found = (classInfo->mProperties.find(propertyName) != classInfo->mProperties.end());
            
        } while (!found && ((classInfo = classInfo->mInherits) != NULL));
        
        if (!found)
            B_THROW(ConstantOSStatusException<errAENoSuchObject>());
    
        // Now we decide whether the property is the magic pProperties property
        // or just some other property.  If it’s the latter, we just call our
        // sub-routine, SubGetTokenValue, to do the job.  If it’s the former, we
        // have to iterate over the class’s property table, calling SubGetTokenValue
        // for each property and assembling them into an AERecord.  Remember that,
        // while iterating over the class’s properties, we must also follow any
        // pInherits ‘links’.
            
        if (propertyName == pProperties)
        {
#if 0
            err = AECreateList(NULL, 0, true, result);
            if (err == noErr) {
                propertyIndex = 0;
                propertyBase  = gClassTable[thisClass].properties;
                do {
                    MoreAENullDesc(&thisResult);
                    
                    thisPropName = propertyBase[propertyIndex].propName;
                    if (thisPropName != kMOSLPropNameLast) {
                        if (thisPropName == pInherits) {
                            junk = ClassIDToClassIndex((OSType) propertyBase[propertyIndex].propData, &thisClass);
                            assert(junk == noErr);
                            propertyBase = gClassTable[thisClass].properties;
                            propertyIndex = 0;
                        } else {
                            if ((thisPropName != pProperties) && !(propertyBase[propertyIndex].propData & kMOSLPropNotInProps)) {
                                InitPropertyMOSLTokenFromOtherProperty(&thisTok, tok, thisPropName);
                                err = SubGetTokenValue(thisClass, &thisTok, resolution, &thisResult);
                                if (err == noErr) {
                                    err = AEPutParamDesc(result, thisPropName, &thisResult);
                                }
                            }
                            propertyIndex += 1;
                        }
                    }

                    MoreAEDisposeDesc(&thisResult);
                } while ( (err == noErr) && (thisPropName != kMOSLPropNameLast) );
            }
#endif
        }
        else
        {
            SubGetTokenValue(inContainerInfo, inContainerToken, inReturnObjects, outValue);
        }
    }
    else
    {
        SubGetTokenValue(inContainerInfo, inContainerToken, inReturnObjects, outValue);
    }
    
#if 0
    
    if (err != noErr) {
        MoreAEDisposeDesc(result);
    }
    
    return err;
#endif
#endif
}

// ------------------------------------------------------------------------------------------
void
AEObject::WriteTokenValue(
    const AEToken&      inToken, 
    DescType            inDesiredType,
    AEWriter&           ioValueWriter)
{
    if (inDesiredType == typeWildCard)
    {
        // No coercion necessary: just write out the data directly.
        
        WriteTokenValue(inToken, ioValueWriter);
    }
#if 0
    else if (inToken.IsListToken())
    {
        // Coerced list token: we need to coerce each element of the list individually.
        
        const std::list<AEObjectPtr>&   list    = inToken.GetList();
        AutoAEWriterList                autoList(ioValueWriter);
        
        std::for_each(list.begin(), list.end(), 
                      boost::bind(WriteCoercedItem, boost::ref(ioValueWriter), _1, 
                                  inDesiredType));
    }
#endif
    else
    {
        AEWriter        innerWriter;
        AEDescriptor    valueDesc;
        AEDescriptor    coercedDesc;
        
        WriteTokenValue(inToken, innerWriter);
        innerWriter.Close(valueDesc);
        
        AEObjectSupport::CoerceDesc(valueDesc, inDesiredType, coercedDesc);
        ioValueWriter.WriteDesc(coercedDesc);
    }
}

// ------------------------------------------------------------------------------------------
// This routine calls the "getter" object primitive for the given
// class, asking it to return the value for tok.  resolution
// specifies, if the return value is a property whose
// value is an object, whether to return a token for the object or
// an object specifier for object.
//
// This routine is also the place where MOSL implements generic
// support for the pProperties property.
void
AEObject::WriteTokenSpecifier(
    const AEToken&      inContainerToken, 
    AEWriter&           ioWriter)
{
    AEObjectPtr obj = inContainerToken.GetObject();
    
    B_ASSERT(obj != NULL);
    
#if 0
    if (inContainerToken.IsListToken())
    {
        // We have a list token.
        
        const std::list<AEObjectPtr>&   list    = inContainerToken.GetList();
        AutoAEWriterList                autoList(ioWriter);
        
        std::for_each(list.begin(), list.end(), 
                      boost::bind(WriteObjSpec, boost::ref(ioWriter), _1));
    }
#endif
    if (inContainerToken.IsPropertyToken())
    {
        // We have a property token.
        
        DescType    propertyName    = inContainerToken.GetPropertyName();
        AEObjectPtr propertyObj     = obj->GetPropertyObject(propertyName);
        
        if (propertyObj != NULL)
        {
            // Property token that maps to an object.
            
            propertyObj->MakeSpecifier(ioWriter);
        }
        else
        {
            // Property token that maps to data.
            
            obj->WriteProperty(propertyName, ioWriter);
        }
    }
    else
    {
        // We have an object or null token.
        
        obj->MakeSpecifier(ioWriter);
    }

#if 0
#if 0
    OSStatus         err;
    OSStatus         junk;
    MOSLToken        thisTok;
    AEDesc           thisResult;
    MOSLPropIndex    propertyIndex;
    MOSLPropTablePtr propertyBase;
    DescType         thisPropName;

    assert(thisClass < gClassTableSize);
    assert(tok != NULL);
    assert(gClassTable[thisClass].classID == tok->tokObjType);
    assert((resolution == kReturnTokensAsObjects) || (resolution == kReturnTokensAsTokens));
    assert(result != NULL);
    
    MoreAENullDesc(result);
    
    err = noErr;
    if (gClassTable[thisClass].getter == NULL) {
        // We return the fixed error errAEEventNotHandled in the cases
        // where the getter is NULL.  It doesn’t make any difference what
        // error we return in this case because the getter should never
        // be NULL (otherwise we couldn’t get the object specifier for
        // a token, which is pretty much a required operation).
        err = errAEEventNotHandled;
    }
#endif
    
    // Check whether the class’s property table lists this property; if not
    // we can error without calling the getter.
    
    if (inContainerToken.IsPropertyToken())
    {
        const ClassInfo*    classInfo       = &inContainerInfo;
        DescType            propertyName    = inContainerToken.GetPropertyName();
        bool                found           = false;
        
        do
        {
            found = (classInfo->mProperties.find(propertyName) != classInfo->mProperties.end());
            
        } while (!found && ((classInfo = classInfo->mInherits) != NULL));
        
        if (!found)
            B_THROW(ConstantOSStatusException<errAENoSuchObject>());
    
        // Now we decide whether the property is the magic pProperties property
        // or just some other property.  If it’s the latter, we just call our
        // sub-routine, SubGetTokenValue, to do the job.  If it’s the former, we
        // have to iterate over the class’s property table, calling SubGetTokenValue
        // for each property and assembling them into an AERecord.  Remember that,
        // while iterating over the class’s properties, we must also follow any
        // pInherits ‘links’.
            
        if (propertyName == pProperties)
        {
#if 0
            err = AECreateList(NULL, 0, true, result);
            if (err == noErr) {
                propertyIndex = 0;
                propertyBase  = gClassTable[thisClass].properties;
                do {
                    MoreAENullDesc(&thisResult);
                    
                    thisPropName = propertyBase[propertyIndex].propName;
                    if (thisPropName != kMOSLPropNameLast) {
                        if (thisPropName == pInherits) {
                            junk = ClassIDToClassIndex((OSType) propertyBase[propertyIndex].propData, &thisClass);
                            assert(junk == noErr);
                            propertyBase = gClassTable[thisClass].properties;
                            propertyIndex = 0;
                        } else {
                            if ((thisPropName != pProperties) && !(propertyBase[propertyIndex].propData & kMOSLPropNotInProps)) {
                                InitPropertyMOSLTokenFromOtherProperty(&thisTok, tok, thisPropName);
                                err = SubGetTokenValue(thisClass, &thisTok, resolution, &thisResult);
                                if (err == noErr) {
                                    err = AEPutParamDesc(result, thisPropName, &thisResult);
                                }
                            }
                            propertyIndex += 1;
                        }
                    }

                    MoreAEDisposeDesc(&thisResult);
                } while ( (err == noErr) && (thisPropName != kMOSLPropNameLast) );
            }
#endif
        }
        else
        {
            SubGetTokenValue(inContainerInfo, inContainerToken, inReturnObjects, outValue);
        }
    }
    else
    {
        SubGetTokenValue(inContainerInfo, inContainerToken, inReturnObjects, outValue);
    }
    
#if 0
    
    if (err != noErr) {
        MoreAEDisposeDesc(result);
    }
    
    return err;
#endif
#endif
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::CloneObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAEClone>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAEClone> event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    event.mNewObject = GetObject(inDOToken)->CloneObject(event.mInsertPosition, 
                                                         event.mTarget, 
                                                         event.mProperties);
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::CloseObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAEClose>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAEClose> event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    GetObject(inDOToken)->CloseObject(event.mSaveOption, event.mObjectUrl);
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAECountElements>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAECountElements> event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    event.mCount = GetObject(inDOToken)->CountElements(event.mObjectClass);
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::CreateObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAECreateElement>(
    const AEToken&      /* inDOToken */,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
#warning fix me
//  B_ASSERT(inDOToken.IsEmpty());
    
    AEEvent<kAECoreSuite, kAECreateElement> event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();

    AEObjectPtr targetObj   = event.mTarget;
    AEObjectPtr obj;
    
    if (targetObj == NULL)
        targetObj = AEObject::GetDefaultObject();
    
    B_ASSERT(targetObj != NULL);
    
    switch (event.mInsertPosition)
    {
    case kAEBefore:
    case kAEAfter:
    case kAEReplace:
        obj = targetObj->GetContainer();
        break;
    
    case kAEBeginning:
    case kAEEnd:
        obj = targetObj;
        break;
    }
    
    B_ASSERT(obj != NULL);
    
    event.mNewObject = obj->CreateObject(event.mObjectClass, event.mInsertPosition, 
                                         targetObj, event.mProperties, event.mData);
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::DeleteObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAEDelete>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAEDelete>    event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    GetObject(inDOToken)->DeleteObject();
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAEDoObjectsExist>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAEDoObjectsExist>    event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    
#warning fix me
//  B_ASSERT(inDOToken.GetDescriptorType() != typeNull);
    
    // The object exists if the direct object is valid.  Note that the
    // "exists" event must have its direct object requirement field in
    // the event table set to kMOSLDOBadOK so that we ignore the error
    // from AEResolve.  The only problem with this is that we then
    // think that the application itself doesn’t exist.  So we special
    // case that by looking at the direct object of the Apple event itself.
    // If that is typeNull, the client was testing the application itself,
    // so we know that it exists.
    
#warning fix me -- should be GetDescriptorType()
    if (inDOToken.GetObjectClassID() == cApplication)
    {
        AEDescriptor    directObjDesc;
        
        if (AEGetParamDesc(&inEvent, keyDirectObject, typeWildCard, directObjDesc) == noErr)
            event.mExists = (directObjDesc.GetType() == typeNull);
        else
            event.mExists = false;
    }
    else
    {
        event.mExists = true;
    }
    
    event.Update();

#if 0
    OSStatus err;
    Boolean  exists;
    AEDesc   dirObjDesc;

    assert(dirObjTok != NULL);
    assert(theEvent  != NULL);
    assert(result    != NULL);

    if (gDebugFlags & kMOSLLogGeneralMask) {
        BBLogLine("\pMOSLGeneralExists");
        BBLogIndent();
        BBLogMOSLToken("\pdirObjTok", dirObjTok);
        BBLogAppleEvent("\ptheEvent", theEvent);
    }
    
    err = MoreAEGotRequiredParams(theEvent);
    if (err == noErr) {
        assert(dirObjTok->tokType != typeNull);

        // The object exists if the direct object is valid.  Note that the
        // "exists" event must have its direct object requirement field in
        // the event table set to kMOSLDOBadOK so that we ignore the error
        // from AEResolve.  The only problem with this is that we then
        // think that the application itself doesn’t exist.  So we special
        // case that by looking at the direct object of the Apple event itself.
        // If that is typeNull, the client was testing the application itself,
        // so we know that it exists.

        if (dirObjTok->tokType == cApplication) {
            MoreAENullDesc(&dirObjDesc);
            
            if (AEGetParamDesc(theEvent, keyDirectObject, typeWildCard, &dirObjDesc) == noErr) {
                exists = (dirObjDesc.descriptorType == typeNull);
            } else {
                exists = false;
            }
            
            MoreAEDisposeDesc(&dirObjDesc);
        } else {
            exists = true;
        }   
        
        err = AECreateDesc(typeBoolean, &exists, sizeof(exists), result);
    }

    if (gDebugFlags & kMOSLLogGeneralMask) {
        BBLogDesc("\p<result", result);
        BBLogOutdentWithErr(err);
    }

    return err;
#endif
}

// ------------------------------------------------------------------------------------------
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAEGetData>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAEGetData>   event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    WriteTokenValue(inDOToken, event.mRequestedType, ioResultWriter);
    event.Update();
    
#if 0   // good
    const AEObjectSupport&  objSupport  = AEObjectSupport::Get();
    AEDescriptor            requestedTypeDesc;
    DescType                requestedType;
    AEDescriptor            valueDesc;
    OSStatus                err;

    err = AEGetParamDesc(&inEvent, keyAERequestedType, typeWildCard, requestedTypeDesc);
    if (err != errAEDescNotFound)
        B_THROW_IF_STATUS(err);
    
    if (err == noErr)
    {
        objSupport.CoerceDesc(requestedTypeDesc, typeType, &requestedType, 
                              sizeof(requestedType));
    }
    else
    {
        requestedType = typeWildCard;
        err = noErr;
    }

    err = MoreAEGotRequiredParams(&inEvent);
    B_THROW_IF_STATUS(err);
    
    // Look up and call the class’s "getter" object primitive.
    
    objSupport.GetTokenValue(inDOToken, valueDesc);
#endif
    
#if 0
    OSStatus       err;
    MOSLClassIndex thisClass;
    AEDesc         requestedTypeDesc;
    DescType       requestedType;
    AEDesc         result2;

    assert(dirObjTok != NULL);
    assert(theEvent  != NULL);
    assert(result    != NULL);

    if (gDebugFlags & kMOSLLogGeneralMask) {
        BBLogLine("\pMOSLGeneralGetData");
        BBLogIndent();
        BBLogMOSLToken("\pdirObjTok", dirObjTok);
        BBLogAppleEvent("\ptheEvent", theEvent);
    }
    
    MoreAENullDesc(result);
    MoreAENullDesc(&requestedTypeDesc);

    // Determine the type of data the the client is requesting.
        
    err = AEGetParamDesc(theEvent, keyAERequestedType, typeWildCard, &requestedTypeDesc);
    if (err == noErr) {
        err = MOSLCoerceObjDescToPtr(&requestedTypeDesc, typeType, &requestedType, sizeof(requestedType));
    } else if (err == errAEDescNotFound) {
        requestedType = typeWildCard;
        err = noErr;
    }
    if (err == noErr) {
        if (gDebugFlags & kMOSLLogGeneralMask) {
            BBLogDescType("\prequestedType", requestedType);
        }
        err = MoreAEGotRequiredParams(theEvent);
    }
    
    // Look up and call the class’s "getter" object primitive.
    
    if (err == noErr) {
        err = ClassIDToClassIndex(dirObjTok->tokObjType, &thisClass);
    }
    if (err == noErr) {
        err = GetTokenValue(thisClass, dirObjTok, kReturnTokensAsObjects, result);
    }
#endif

    
#if 0       // good
    // If the data isn’t the right type, try coercing it.
    
    if ((requestedType != typeWildCard) && (valueDesc.GetType() != requestedType))
    {
        AEDescriptor    coercedDesc;
        
        objSupport.CoerceDesc(valueDesc, requestedType, coercedDesc);
        std::swap(static_cast<AEDesc&>(valueDesc), static_cast<AEDesc&>(coercedDesc));
    }
    
    ioResultWriter.WriteDesc(valueDesc);
#endif
    
#if 0
    if ((err == noErr) && (requestedType != typeWildCard) && (result->descriptorType != requestedType)) {
        err = MOSLCoerceObjDesc(result, requestedType, &result2);
        if (err == noErr) {
            MoreAEDisposeDesc(result);
            *result = result2;
        }
    }
    
    // Clean up.
    
    MoreAEDisposeDesc(&requestedTypeDesc);
    if (err != noErr) {
        MoreAEDisposeDesc(result);
    }

    if (gDebugFlags & kMOSLLogGeneralMask) {
        BBLogDesc("\p<result", result);
        BBLogOutdentWithErr(err);
    }

    return err;
#endif
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::MoveObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAEMove>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAEMove>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    event.mNewObject = GetObject(inDOToken)->MoveObject(event.mInsertPosition, 
                                                        event.mTarget);
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::OpenObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kCoreEventClass, kAEOpen>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kCoreEventClass, kAEOpen>   event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    GetObject(inDOToken)->OpenObject();
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::PrintObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kCoreEventClass, kAEPrint>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kCoreEventClass, kAEPrint>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    GetObject(inDOToken)->PrintObject(event.mPrintSettings.get(), 
                                      event.mPrinter.get(), 
                                      event.mShowDialog);
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::SaveObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAESave>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAESave>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    GetObject(inDOToken)->SaveObject(event.mObjectUrl, 
                                     event.mObjectType);
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <> void
AEObject::DispatchEvent<kAECoreSuite, kAESetData>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAECoreSuite, kAESetData>   event(inEvent, ioResultWriter);

    // Work out the class of the object and use that information
    // to a) determine whether the class has the property we’re
    // trying to set, and b) whether the class has a "setter"
    // object primitive.  It’s legal for a class to have no
    // "setter" primitive if it has no read/write properties.
    
    DescType    property    = 0;
    
    if (inDOToken.IsPropertyToken())
    {
        property = inDOToken.GetPropertyName();
        
        DescType                        objClass    = inDOToken.GetObjectClassID();
        const AEObjectSupport&          objSupport  = AEObjectSupport::Get();
        const AEInfo::PropertyInfo&     propInfo    = objSupport.GetPropertyInfo(
                                                        objSupport.GetClassInfo(objClass),
                                                        property);
        
        if (!(propInfo.mAccess & AEInfo::kAccessWrite))
            B_THROW(AENotModifiableException());
    }
    
    event.CheckRequiredParams();
    
    AEReader    reader(event.mData);
    
    GetObject(inDOToken)->ReadProperty(property, reader);
    event.Update();
    
#if 0
    OSStatus              err;
    MOSLClassIndex        thisClass;
    ConstMOSLPropEntryPtr propEntry;
    AEDesc                data;

    assert(dirObjTok != NULL);
    assert(theEvent  != NULL);
    assert(result    != NULL);

    if (gDebugFlags & kMOSLLogGeneralMask) {
        BBLogLine("\pMOSLGeneralSetData");
        BBLogIndent();
        BBLogMOSLToken("\pdirObjTok", dirObjTok);
        BBLogAppleEvent("\ptheEvent", theEvent);
    }

    MoreAENullDesc(result); 
    MoreAENullDesc(&data);

    // We can only set properties, not the objects themselves.
    
    if (dirObjTok->tokType == typeProperty) {
    
        // Get the data from the Apple event.
        
        err = AEGetParamDesc(theEvent, keyAEData, typeWildCard, &data);
        if (err == noErr) {
            if (gDebugFlags & kMOSLLogGeneralMask) {
                BBLogDesc("\pdata", &data);
            }
            err = MoreAEGotRequiredParams(theEvent);
        }
        
        // Work out the class of the object and use that information
        // to a) determine whether the class has the property we’re
        // trying to set, and b) whether the class has a "setter"
        // object primitive.  It’s legal for a class to have no
        // "setter" primitive if it has no read/write properties.
        
        if (err == noErr) {
            err = ClassIDToClassIndex(dirObjTok->tokObjType, &thisClass);
        }
        if (err == noErr) {
            propEntry = PropertyToPropEntry(thisClass, dirObjTok->tokPropName);
            if (propEntry == NULL) {
                err = errAENoSuchObject;
            } else if ((propEntry->propData & kMOSLPropROMask) == kMOSLPropReadOnly) {
                err = errAENotModifiable;
            }
        }
        if (err == noErr) {
            if (gClassTable[thisClass].setter == NULL) {
                err = errAENotModifiable;
            }
        }
        
        // Now we either call SetPropertiesInRecord to handle the pProperties
        // property, or just call the "setter" primitive directly.
        
        if (err == noErr) {
            if (dirObjTok->tokPropName == pProperties) {
                err = SetPropertiesInRecord(thisClass, dirObjTok, &data);
            } else {
                err = CallSetter(thisClass, dirObjTok, &data);
            }
        }
    } else {
        err = errAENotModifiable;
    }
    MoreAEDisposeDesc(&data);

    if (gDebugFlags & kMOSLLogGeneralMask) {
        BBLogOutdentWithErr(err);
    }

    return err;
#endif
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::RevertObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAEMiscStandards, kAERevert>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAEMiscStandards, kAERevert>    event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    GetObject(inDOToken)->RevertObject();
    event.Update();
}

// ------------------------------------------------------------------------------------------
/*! Collects the event's parameters (if the event defines any), then calls the appropriate 
    member function of AEObject::ActivateObject().
    
    Derived classes may override this function to dispatch it differently, or to extract 
    extra event parameters.
*/
template <> void
AEObject::DispatchEvent<kAEMiscStandards, kAEActivate>(
    const AEToken&      inDOToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter)
{
    AEEvent<kAEMiscStandards, kAEActivate>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    inDOToken.GetObject()->ActivateObject();
    event.Update();
}

#if 0   
// ------------------------------------------------------------------------------------------
void
AEObject::DispatchGetter(
    const AEToken&      inContainerToken, 
    AEDesc&             outValue)
{
    DescType    propertyName    = 0;
    AEObjectPtr obj             = inContainerToken.GetObject();
    OSStatus    err;
    
    B_ASSERT(obj != NULL);
    
    if (inContainerToken.IsPropertyToken())
        propertyName = inContainerToken.GetPropertyName();
    
    if (!inContainerToken.IsPropertyToken())
    {
        // Non-property token.
        
        AEWriter    writer;
        
        obj->MakeSpecifier(writer);
        writer.Close(outValue);
    }
    else if ((obj = obj->GetPropertyObject(propertyName)) != NULL)
    {
        // Property token that maps to an object.
        
        AEWriter    writer;
        
        obj->MakeSpecifier(writer);
        writer.Close(outValue);
    }
    else
    {
        // Property token that maps to data.
        
        AEWriter    writer;
        
        obj->WriteProperty(propertyName, writer);
        writer.Close(outValue);
    }
}
#endif


// ==========================================================================================
//  AEObject::InsertionLocationSpecifier

#pragma mark -

// ------------------------------------------------------------------------------------------
AEObject::InsertionLocationSpecifier::InsertionLocationSpecifier(
    OSType              pos,    //!< The insertion postion relative to @a obj.
    ConstAEObjectPtr    obj)    //!< The base object for the insertion.
        : AEEventFunctor::InsertionLocation< 
                const AEObject&, 
                AETypedObject<typeEnumeration> >
            (*obj, AETypedObject<typeEnumeration>(pos))
{
}


// ==========================================================================================
//  AEObject::IndexSpecifier

#pragma mark -

// ------------------------------------------------------------------------------------------
AEObject::IndexSpecifier::IndexSpecifier(
    DescType            classID,    //!< The class of the object we're looking for.
    ConstAEObjectPtr    obj,        //!< The containing object.
    SInt32              index)      //!< The index of the object we're looking for.
        : AEEventFunctor::IndexSpecifier< 
                const AEObject&, 
                AETypedObject<typeType>, 
                AETypedObject<typeSInt32> >
            (*obj, 
             AETypedObject<typeType>(classID), 
             AETypedObject<typeSInt32>(index+1))
{
}


// ==========================================================================================
//  AEObject::NameSpecifier

#pragma mark -

// ------------------------------------------------------------------------------------------
AEObject::NameSpecifier::NameSpecifier(
    DescType            classID,    //!< The class of the object we're looking for.
    ConstAEObjectPtr    obj,        //!< The containing object.
    const String&       name)       //!< The name of the object we're looking for.
        : AEEventFunctor::NameSpecifier< 
                const AEObject&, 
                AETypedObject<typeType>, 
                const String& >
            (*obj, AETypedObject<typeType>(classID), name)
{
}


// ==========================================================================================
//  AEObject::UniqueIDSpecifier

#pragma mark -

// ------------------------------------------------------------------------------------------
AEObject::UniqueIDSpecifier::UniqueIDSpecifier(
    DescType            classID,    //!< The class of the object we're looking for.
    ConstAEObjectPtr    obj,        //!< The containing object.
    SInt32              inUniqueID) //!< The unique id of the object we're looking for.
        : AEEventFunctor::UniqueIDSpecifier< 
                    const AEObject&, 
                    AETypedObject<typeType>, 
                    AETypedObject<typeSInt32> >
            (*obj, 
             AETypedObject<typeType>(classID), 
             AETypedObject<typeSInt32>(inUniqueID))
{
}


// ==========================================================================================
//  AEObject::PropertySpecifier

#pragma mark -

// ------------------------------------------------------------------------------------------
AEObject::PropertySpecifier::PropertySpecifier(
    ConstAEObjectPtr    obj,        //!< The containing object.
    DescType            propertyID) //!< The id of one of the object's properties.
        : AEEventFunctor::PropertySpecifier< 
                    const AEObject&, 
                    AETypedObject<typeType> >
            (*obj, AETypedObject<typeType>(propertyID))
{
}


}   // namespace B
