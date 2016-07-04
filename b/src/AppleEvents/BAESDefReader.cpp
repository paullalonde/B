// ==========================================================================================
//  
//  Copyright (C) 2006 Paul Lalonde enrg.
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
#include "BAESDefReader.h"

// B headers
#include "BBundle.h"
#include "BStringUtilities.h"
#include "BUrl.h"


namespace {

// ------------------------------------------------------------------------------------------
inline const CFXMLElementInfo&
Tree2Element(CFXMLTreeRef tree)
{
    CFXMLNodeRef    node    = CFXMLTreeGetNode(tree);
    
    B_ASSERT(CFXMLNodeGetTypeCode(node) == kCFXMLNodeTypeElement);
    
    const void*     info    = CFXMLNodeGetInfoPtr(node);
    
    B_ASSERT(info != NULL);
    
    return (* reinterpret_cast<const CFXMLElementInfo*>(info));
}

// ------------------------------------------------------------------------------------------
bool
IsTreeAnElementOfTag(CFXMLTreeRef tree, CFStringRef tag)
{
    CFXMLNodeRef        node    = CFXMLTreeGetNode(tree);
    CFXMLNodeTypeCode   type    = CFXMLNodeGetTypeCode(node);
    
    if (type != kCFXMLNodeTypeElement)
        return (false);
    
    CFStringRef str = CFXMLNodeGetString(node);
    
    B_ASSERT(str != NULL);
    
    return (CFStringCompare(str, tag, 0) == 0);
}


// ==========================================================================================
//  ElementIterator

#pragma mark ElementIterator

class ElementIterator : public std::iterator<std::input_iterator_tag, 
                                            CFXMLTreeRef, 
                                            ptrdiff_t, 
                                            CFXMLTreeRef, 
                                            CFXMLTreeRef&>
{
public:
    
    //! @name Constructors / Destructor
    //@{
                //! Default constructor.  It's equivalent to an end-of-sequence iterator.
                ElementIterator();
                //! Copy constructor.
                ElementIterator(const ElementIterator& it);
                //! Argument constructor.
    explicit    ElementIterator(CFXMLTreeRef inParent, CFStringRef inTag);
    //@}
    
    //! @name Assignment
    //@{
    //! Copy assignment.
    ElementIterator&    operator = (const ElementIterator& it);
    //@}
    
    //! @name Iterator Specifics
    //@{
    //! Returns the @c HIViewRef that the iterator currently points to.
    value_type          operator * () const;
    //! Advances the iterator, returning the new position.
    ElementIterator&    operator ++ ();
    //! Advances the iterator, returning the old position.
    ElementIterator     operator ++ (int);
    //@}
    
private:
    
    static CFXMLTreeRef FindNext(CFXMLTreeRef inTree, CFStringRef inTag);
    
    // member variables
    CFXMLTreeRef    mTree;
    CFStringRef     mTag;
    
    // friends
    friend bool operator == (const ElementIterator& it1, const ElementIterator& it2);
    friend bool operator != (const ElementIterator& it1, const ElementIterator& it2);
};

// ------------------------------------------------------------------------------------------
inline
ElementIterator::ElementIterator()
    : mTree(NULL), mTag(NULL)
{
}

// ------------------------------------------------------------------------------------------
inline
ElementIterator::ElementIterator(const ElementIterator& it)
    : mTree(it.mTree), mTag(it.mTag)
{
}

// ------------------------------------------------------------------------------------------
inline
ElementIterator::ElementIterator(CFXMLTreeRef inParent, CFStringRef inTag)
    : mTag(inTag)
{
    mTree = FindNext(CFTreeGetFirstChild(inParent), inTag);
}

// ------------------------------------------------------------------------------------------
inline CFXMLTreeRef
ElementIterator::FindNext(CFXMLTreeRef inTree, CFStringRef inTag)
{
    for ( ; inTree != NULL; inTree = CFTreeGetNextSibling(inTree))
    {
        if (IsTreeAnElementOfTag(inTree, inTag))
            break;
    }
    
    return (inTree);
}

// ------------------------------------------------------------------------------------------
inline ElementIterator&
ElementIterator::operator = (const ElementIterator& it)
{
    mTree       = it.mTree;
    mTag        = it.mTag;
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline ElementIterator::value_type
ElementIterator::operator * () const
{
    return (mTree);
}

// ------------------------------------------------------------------------------------------
inline ElementIterator&
ElementIterator::operator ++ ()
{
    mTree = FindNext(CFTreeGetNextSibling(mTree), mTag);
    
    return (*this);
}

// ------------------------------------------------------------------------------------------
inline ElementIterator
ElementIterator::operator ++ (int)
{
    ElementIterator old(*this);
    
    mTree = FindNext(CFTreeGetNextSibling(mTree), mTag);
    
    return (old);
}

// ==========================================================================================
//  ElementIterator Global Functions

//! Compares @a it1 and @a it2 for equality.
inline bool operator == (const ElementIterator& it1, const ElementIterator& it2)
{
    return (it1.mTree == it2.mTree);
}

//! Compares @a it1 and @a it2 for inequality.
inline bool operator != (const ElementIterator& it1, const ElementIterator& it2)
{
    return (it1.mTree != it2.mTree);
}

}   // anonymous namespace


namespace B {

// ==========================================================================================
//  AESDefReader

#pragma mark AESDefReader

// ------------------------------------------------------------------------------------------
AESDefReader::AESDefReader(
    AEInfo::ClassMap&           ioClassMap,
    AEInfo::EventMap&           ioEventMap, 
    AEInfo::DefaultEventHandler inDefaultEventHandler)
        : mClassMap(ioClassMap), mEventMap(ioEventMap), 
          mDefaultEventHandler(inDefaultEventHandler)
{
}

// ------------------------------------------------------------------------------------------
#ifndef NDEBUG
void
AESDefReader::DebugPrint()
{
    for (std::vector<DescType>::const_iterator cit = mClassOrder.begin(); 
         cit != mClassOrder.end();
         ++cit)
    {
        const AEInfo::ClassInfo&    classInfo   = mClassMap.find(*cit)->second;
    
        std::cout << "Class '" << B::make_string(classInfo.mCode) << "'\n";

        std::cout << "\tParent:\t\t\t";
        if (classInfo.mParentCode != 0)
            std::cout << "'" << B::make_string(classInfo.mParentCode) << "'\n";
        else
            std::cout << "-\n";

        std::cout << "\tAncestors:\t\t";
        for (std::set<DescType>::const_iterator ait = classInfo.mAncestors.begin(); 
             ait != classInfo.mAncestors.end();
             ++ait)
        {
            std::cout << "'" << B::make_string(*ait) << "' ";
        }
        std::cout << "\n";

        std::cout << "\tDescendents:\t\t";
        for (std::set<DescType>::const_iterator dit = classInfo.mDescendents.begin(); 
             dit != classInfo.mDescendents.end();
             ++dit)
        {
            std::cout << "'" << B::make_string(*dit) << "' ";
        }
        std::cout << "\n";
        
        std::cout << "\tElements:\n";
        for (AEInfo::ElementMap::const_iterator eit = classInfo.mElements.begin(); 
             eit != classInfo.mElements.end();
             ++eit)
        {
            const AEInfo::ElementInfo&  elemInfo    = eit->second;
            
            std::cout << "\t\t'" << B::make_string(elemInfo.mName) << "'\n";
            std::cout << "\t\t\tAccess:\t\t" << std::hex << elemInfo.mAccess << "'\n";
            
            std::cout << "\t\t\tKey forms:\t";
            for (AEInfo::KeyFormSet::const_iterator kit = elemInfo.mKeyForms.begin(); 
                 kit != elemInfo.mKeyForms.end();
                 ++kit)
            {
                std::cout << "'" << B::make_string(*kit) << "' ";
            }
            std::cout << "\n";
        }
        
        std::cout << "\tProperties:\n";
        for (AEInfo::PropertyMap::const_iterator pit = classInfo.mProperties.begin(); 
             pit != classInfo.mProperties.end();
             ++pit)
        {
            const AEInfo::PropertyInfo& propInfo    = pit->second;
            
            std::cout << "\t\t'" << B::make_string(propInfo.mName) << "'\n";
            std::cout << "\t\t\tAccess:\t\t\t" << std::hex << propInfo.mAccess << "\n";
            std::cout << "\t\t\tIn Properties:\t" << (propInfo.mInProperties ? "true" : "false") << "\n";
            std::cout << "\t\t\tIs List:\t\t" << (propInfo.mIsList ? "true" : "false") << "\n";
        }
        
        std::cout << "\tAll Key forms:\t";
        for (AEInfo::KeyFormSet::const_iterator akit = classInfo.mAllKeyForms.begin(); 
             akit != classInfo.mAllKeyForms.end();
             ++akit)
        {
            std::cout << "'" << B::make_string(*akit) << "' ";
        }
        std::cout << "\n";
        
        std::cout << "\tClass Events:\n";
        for (AEInfo::ClassEventMap::const_iterator ceit = classInfo.mEvents.begin(); 
             ceit != classInfo.mEvents.end();
             ++ceit)
        {
            const AEInfo::EventKey& eventKey    = ceit->first;
            
            std::cout << "\t\t'" << B::make_string(eventKey.first) 
                      << "' / '" << B::make_string(eventKey.second) << "'\n";
        }
        
        std::cout << "\n";
    }
}
#endif

// ------------------------------------------------------------------------------------------
void
AESDefReader::Read(
    const Bundle&   inBundle, 
    const String&   inSDefName /* = String() */)
{
    OSPtr<CFXMLTreeRef> dictTree(ReadSDefFile(inBundle, inSDefName));
    
    // Record all type & event names in the scripting definition file.
    
    std::for_each(ElementIterator(dictTree, CFSTR("suite")), 
                  ElementIterator(), 
                  XmlTreeFunctor(
                    boost::bind(&AESDefReader::RecordSuiteNames, this, _1)));
    
    // Register the contents of the scripting definition file.
    
    std::for_each(ElementIterator(dictTree, CFSTR("suite")), 
                  ElementIterator(), 
                  XmlTreeFunctor(
                      boost::bind(&AESDefReader::RegisterSuite, this, _1)));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFXMLTreeRef>
AESDefReader::ReadSDefFile(
    const Bundle&       inBundle, 
    const String&       inSDefName)
{
    String  sdefName;
    
    if (!inSDefName.empty())
        sdefName = inSDefName;
    else
        sdefName = inBundle.InfoString(CFSTR("B::ScriptingDefinition"));
    
    if (sdefName.empty())
        sdefName = inBundle.Name();
    
    Url sdefUrl = inBundle.Resource(sdefName, String("sdef"));
    
    OSPtr<CFXMLTreeRef> sdefTree(CFXMLTreeCreateWithDataFromURL(NULL, sdefUrl.cf_ref(), 
                                                                kCFXMLParserSkipWhitespace, 
                                                                kCFXMLNodeCurrentVersion),
                                 from_copy);
    
    ElementIterator dictIt(sdefTree, CFSTR("dictionary"));
    
    B_THROW_IF(dictIt == ElementIterator(), 
            std::runtime_error("Malformed scripting definition file:  missing <dictionary> tag."));
    
    return (OSPtr<CFXMLTreeRef>(*dictIt));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RecordSuiteNames(
    CFXMLTreeRef    inSuiteTree)
{
    // Clear everything.
    
    mKeyFormNames.clear();
    mTypeNames.clear();
    mEventNames.clear();
    
    // Initialise the keyform name map.
    
    mKeyFormNames.insert(std::pair<String, DescType>(String("index"),    formAbsolutePosition));
    mKeyFormNames.insert(std::pair<String, DescType>(String("relative"), formRelativePosition));
    mKeyFormNames.insert(std::pair<String, DescType>(String("test"),     formTest));
    mKeyFormNames.insert(std::pair<String, DescType>(String("range"),    formRange));
    mKeyFormNames.insert(std::pair<String, DescType>(String("name"),     formName));
    mKeyFormNames.insert(std::pair<String, DescType>(String("id"),       formUniqueID));
    
    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("enumeration"), 
        boost::bind(&AESDefReader::RecordTypeName, this, _1));

    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("class"), 
        boost::bind(&AESDefReader::RecordTypeName, this, _1));

    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("record-type"), 
        boost::bind(&AESDefReader::RecordTypeName, this, _1));

    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("value-type"), 
        boost::bind(&AESDefReader::RecordTypeName, this, _1));

    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("command"), 
        boost::bind(&AESDefReader::RecordEventName, this, _1));

    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("event"), 
        boost::bind(&AESDefReader::RecordEventName, this, _1));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterSuite(
    CFXMLTreeRef    inSuiteTree)
{
    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("class"), 
        boost::bind(&AESDefReader::RegisterClass, this, _1));
    
    CompleteClassHierarchy();
    
    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("command"), 
        boost::bind(&AESDefReader::RegisterEvent, this, _1));

    InvokeFunctorOnElements(
        inSuiteTree, CFSTR("event"), 
        boost::bind(&AESDefReader::RegisterEvent, this, _1));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::InvokeFunctorOnElements(
    CFXMLTreeRef            inParentTree, 
    CFStringRef             inTagName,
    XmlTreeFunctor          inFunctor)
{
    std::for_each(ElementIterator(inParentTree, inTagName), 
                  ElementIterator(), inFunctor);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RecordTypeName(
    CFXMLTreeRef    inEnumTree)
{
    String      name    = GetElementAttribute(inEnumTree, CFSTR("name"));
    DescType    code    = GetCodeAttribute(inEnumTree);
    
    if (!mTypeNames.insert(NameMapType(name, code)).second)
        throw std::runtime_error("Malformed scripting definition file:  duplicate type code.");
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RecordEventName(CFXMLTreeRef eventTree)
{
    String              name    = GetElementAttribute(eventTree, CFSTR("name"));
    AEInfo::EventKey    code    = GetCommandCodeAttribute(eventTree);
    
    if (!mEventNames.insert(EventNameMapType(name, code)).second)
        throw std::runtime_error("Malformed scripting definition file:  duplicate event code.");
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterClass(
    CFXMLTreeRef    inClassTree)
{
    String              className   = GetElementAttribute(inClassTree, CFSTR("name"));
    AEInfo::ClassInfo   classInfo;
    String              inheritsName;
    
    classInfo.mCode         = GetCodeAttribute(inClassTree);
    classInfo.mParentCode   = 0;
    
    // If this class inherits from another class, locate the parent class' info.
    
    if (GetElementAttribute(inClassTree, CFSTR("inherits"), inheritsName))
    {
        classInfo.mParentCode = GetClassInfoForClassName(inheritsName).mCode;
    }
    
    InvokeFunctorOnElements(
        inClassTree, CFSTR("element"), 
        boost::bind(&AESDefReader::RegisterClassElement, this, _1, 
                    boost::ref(classInfo)));
    
    InvokeFunctorOnElements(
        inClassTree, CFSTR("contents"), 
        boost::bind(&AESDefReader::RegisterClassContents, this, _1, 
                    boost::ref(classInfo.mProperties)));
    
    InvokeFunctorOnElements(
        inClassTree, CFSTR("property"), 
        boost::bind(&AESDefReader::RegisterClassProperty, this, _1, 
                    boost::ref(classInfo.mProperties)));
    
    InvokeFunctorOnElements(
        inClassTree, CFSTR("responds-to"), 
        boost::bind(&AESDefReader::RegisterClassEvent, this, _1, 
                    className, boost::ref(classInfo.mEvents)));
    
    // All MOSL classes need to support the GetData and SetData events.

    AddMissingEventToClass(kAECoreSuite, kAEGetData, classInfo.mEvents);
    AddMissingEventToClass(kAECoreSuite, kAESetData, classInfo.mEvents);
                          
    if (!classInfo.mElements.empty())
    {
        // Classes containing elements need to have at least one way to access 
        // them.  If none has been specified, assume by-index access.
        
        if (classInfo.mAllKeyForms.empty())
            classInfo.mAllKeyForms.insert(formAbsolutePosition);
        
        // Classes containing elements need to support the DoObjectsExist event, 
        // and those allowing by-index access need to support the CountElements 
        // event.  Install the default handlers for these events if the class 
        // doesn't already handle them.
        
        AddMissingEventToClass(kAECoreSuite, kAEDoObjectsExist, classInfo.mEvents);

        if (classInfo.mAllKeyForms.find(formAbsolutePosition) != classInfo.mAllKeyForms.end())
        {
            AddMissingEventToClass(kAECoreSuite, kAECountElements, classInfo.mEvents);
        }
    }
    
    mClassMap.insert(ClassMapType(classInfo.mCode, classInfo));
    mClassOrder.push_back(classInfo.mCode);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterEvent(CFXMLTreeRef eventTree)
{
    AEInfo::EventKey    code    = GetCommandCodeAttribute(eventTree);
    
    if (!IsEventDefined(code.first, code.second))
    {
        // The event has not yet been defined.
        
        AEInfo::EventDOBehavior     doBehavior      = AEInfo::kEventDOIllegal;
        AEInfo::EventResultAction   resultAction    = AEInfo::kEventResultActionNone;
        ElementIterator             it;

        if ((it = ElementIterator(eventTree, CFSTR("direct-parameter"))) != ElementIterator())
        {
            // The command takes a direct parameter.
            
            if (GetElementYesNoAttribute(*it, CFSTR("optional"), false))
                doBehavior = AEInfo::kEventDOOptional;
            else
                doBehavior = AEInfo::kEventDORequired;
        }
        
        if ((it = ElementIterator(eventTree, CFSTR("result"))) != ElementIterator())
        {
            // The command returns a result.
            
            resultAction = AEInfo::kEventResultActionDefault;
        }
        
        GetDefaultEventBehavior(code, doBehavior, resultAction);
        DefineEvent(code.first, code.second, doBehavior, resultAction);
    }
}

// ------------------------------------------------------------------------------------------
AEInfo::ClassInfo&
AESDefReader::GetClassInfoForClassName(
    const String&           inClassName)
{
    NameMap::const_iterator nit = mTypeNames.find(inClassName);
    
    if (nit == mTypeNames.end())
    {
        std::ostringstream  ostr;
        
        ostr << "Class \"" << inClassName << "\" is undefined.";
        
        B_THROW(std::runtime_error(ostr.str()));
    }
    
    AEInfo::ClassMap::iterator  cit = mClassMap.find(nit->second);
    
    B_ASSERT(cit != mClassMap.end());
    
    return (cit->second);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterClassElement(
    CFXMLTreeRef        inElementTree, 
    AEInfo::ClassInfo&  ioClassInfo)
{
    AEInfo::ElementMap& elemMap     = ioClassInfo.mElements;
    AEInfo::ElementInfo elemInfo;
    
    elemInfo.mName      = GetElementTypeCode(inElementTree);
    elemInfo.mAccess    = GetAccessAttribute(inElementTree);
    
    std::for_each(ElementIterator(inElementTree, CFSTR("accessor")), 
                  ElementIterator(), 
                  XmlTreeFunctor(
                      boost::bind(&AESDefReader::RegisterElementAccessor, this, 
                                  _1, boost::ref(ioClassInfo), boost::ref(elemInfo))));
    
    B_THROW_IF(elemMap.find(elemInfo.mName) != elemMap.end(), 
            std::runtime_error("Duplicate element."));
    
    elemMap.insert(AEInfo::ElementMap::value_type(elemInfo.mName, elemInfo));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterClassContents(
    CFXMLTreeRef            inPropertyTree, 
    AEInfo::PropertyMap&    ioPropertyMap)
{
    DescType    propertyName;
    
    if (ElementContainsAttribute(inPropertyTree, CFSTR("code")))
        propertyName = GetCodeAttribute(inPropertyTree);
    else
        propertyName = pContents;
    
    RegisterClassPropertyWithName(inPropertyTree, propertyName, ioPropertyMap);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterClassProperty(
    CFXMLTreeRef            inPropertyTree, 
    AEInfo::PropertyMap&    ioPropertyMap)
{
    DescType    propertyName    = GetCodeAttribute(inPropertyTree);
    
    RegisterClassPropertyWithName(inPropertyTree, propertyName, ioPropertyMap);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterClassPropertyWithName(
    CFXMLTreeRef            inPropertyTree, 
    DescType                inPropertyName,
    AEInfo::PropertyMap&    ioPropertyMap)
{
    AEInfo::PropertyInfo    propInfo;
    
    propInfo.mName          = inPropertyName;
    propInfo.mAccess        = GetAccessAttribute(inPropertyTree);
    propInfo.mInProperties  = GetElementYesNoAttribute(inPropertyTree, CFSTR("in-properties"), true);
    propInfo.mIsList        = false;
    
    if (std::find_if(ElementIterator(inPropertyTree, CFSTR("type")), 
                     ElementIterator(), 
                     FindPropertyListType) != ElementIterator())
    {
        propInfo.mIsList = true;
    }
    
    // Explicitly setting a pInherits property will only confuse everyone.  The "inherits" 
    // attribute on the class element should be used intead.
    
    B_THROW_IF(propInfo.mName == pInherits, 
            std::runtime_error("Unexpected pInherits property."));
    
    B_THROW_IF((propInfo.mName == pProperties) && propInfo.mInProperties, 
            std::runtime_error("The pProperties property cannot be included in itself."));
    
    B_THROW_IF(ioPropertyMap.find(propInfo.mName) != ioPropertyMap.end(), 
            std::runtime_error("Duplicate property."));
    
    if (!ioPropertyMap.insert(PropertyMapType(propInfo.mName, propInfo)).second)
        throw std::runtime_error("Duplicate property.");
}

// ------------------------------------------------------------------------------------------
bool
AESDefReader::FindPropertyListType(
    CFXMLTreeRef                inElementTree)
{
    return GetElementYesNoAttribute(inElementTree, CFSTR("list"), false);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterClassEvent(
    CFXMLTreeRef            inClassEventTree, 
    const String&           inClassName, 
    AEInfo::ClassEventMap&  ioClassEventMap)
{
    String  eventName   = GetElementAttribute(inClassEventTree, CFSTR("name"));
    EventNameMap::const_iterator            eit;
    AEInfo::ClassEventMap::const_iterator   pit;
    
    // Ensure that the event this class responds to is actually defined.
    
    if ((eit = mEventNames.find(eventName)) == mEventNames.end())
    {
        std::ostringstream  ostr;
        
        ostr << "Class \"" << inClassName << "\" responds to undefined event \"" 
             << eventName << "\"";
        
        B_THROW(std::runtime_error(ostr.str()));
    }
    
    ioClassEventMap.insert(ClassEventMapType(eit->second, AEInfo::ClassEventHandler()));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::AddMissingEventToClass(
    AEEventClass            inEventClass,
    AEEventID               inEventID,
    AEInfo::ClassEventMap&  ioClassEventMap)
{
    AEInfo::EventKey                    eventKey(inEventClass, inEventID);
    AEInfo::EventMap::const_iterator    it  = mEventMap.find(eventKey);
    
    if (it == mEventMap.end())
    {
        // The event isn't in the event map, so we need to add it.
        
        DefineEvent(inEventClass, inEventID);
    }
    
    if (ioClassEventMap.find(eventKey) == ioClassEventMap.end())
    {
        // The event isn't in the class event map, so we need to add it.
        // Use a null handler for now.
        
        ioClassEventMap.insert(ClassEventMapType(eventKey, NULL));
    }
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::GetDefaultEventBehavior(
    const AEInfo::EventKey&     inEventKey,
    AEInfo::EventDOBehavior&    ioBehavior, 
    AEInfo::EventResultAction&  ioAction)
{
    switch (inEventKey.first)
    {
    case kAECoreSuite:
        switch (inEventKey.second)
        {
        case kAECountElements:
            ioBehavior  = AEInfo::kEventDORequired;
            ioAction    = AEInfo::kEventResultActionCountList;
            break;

        case kAEDoObjectsExist:
            ioBehavior  = AEInfo::kEventDOBadOK;
            ioAction    = AEInfo::kEventResultActionCollapseBooleanList;
            break;

        case kAEGetData:
            ioBehavior  = AEInfo::kEventDORequired;
            ioAction    = AEInfo::kEventResultActionDefault;

        case kAESetData:
            ioBehavior  = AEInfo::kEventDORequired;
            ioAction    = AEInfo::kEventResultActionNone;
            break;
        }
        break;
    }
}

// ------------------------------------------------------------------------------------------
DescType
AESDefReader::GetElementTypeCode(
    CFXMLTreeRef    inElementTree)
{
    String  typeName;

    if (!GetElementAttribute(inElementTree, CFSTR("type"), typeName))
    {
        ElementIterator it(inElementTree, CFSTR("type"));
        
        if (it != ElementIterator())
        {
            typeName = GetElementAttribute(*it, CFSTR("type"));
        }
        else
        {
            B_THROW(std::runtime_error(std::string("Missing type property or element.")));
        }
    }
    
    NameMap::const_iterator nit = mTypeNames.find(typeName);
    
    if (nit == mTypeNames.end())
    {
        std::ostringstream  ostr;
        
        ostr << "Type \"" << typeName << "\" is undefined.";
        
        B_THROW(std::runtime_error(ostr.str()));
    }
    
    return (nit->second);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::RegisterElementAccessor(
    CFXMLTreeRef            inAccessorTree, 
    AEInfo::ClassInfo&      ioClassInfo, 
    AEInfo::ElementInfo&    ioElementInfo)
{
    String  style   = GetElementAttribute(inAccessorTree, CFSTR("style"));
    
    NameMap::const_iterator it  = mKeyFormNames.find(style);
    
    B_THROW_IF(it == mKeyFormNames.end(), 
            std::runtime_error("Unknown key form."));
    
    B_THROW_IF(ioClassInfo.mElements.find(it->second) != ioClassInfo.mElements.end(), 
            std::runtime_error("Duplicate key form."));
    
    ioElementInfo.mKeyForms.insert(it->second);
    ioClassInfo.mAllKeyForms.insert(it->second);
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::DefineEvent(
    AEEventClass                inEventClass,           //!< The Apple Event's class.
    AEEventID                   inEventID,              //!< The Apple Event's ID.
    AEInfo::EventDOBehavior     inEventDOBehavior,      //!< Determines how we handle the direct parameter.
    AEInfo::EventResultAction   inEventResultAction)    //!< Determines how we handle the result.
{
    AEInfo::EventInfo   eventInfo;
    
    eventInfo.mEventKey.first   = inEventClass;
    eventInfo.mEventKey.second  = inEventID;
    eventInfo.mDOBehavior       = inEventDOBehavior;
    eventInfo.mResultAction     = inEventResultAction;
    eventInfo.mDefaultHandler   = mDefaultEventHandler;
    
    B_THROW_IF(IsEventDefined(inEventClass, inEventID), std::runtime_error("Event already defined"));
    
    mEventMap.insert(EventMapType(eventInfo.mEventKey, eventInfo));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::DefineEvent(
    AEEventClass        inEventClass,           //!< The Apple Event's class.
    AEEventID           inEventID)              //!< The Apple Event's ID.
{
    AEInfo::EventDOBehavior     doBehavior      = AEInfo::kEventDOIllegal;
    AEInfo::EventResultAction   resultAction    = AEInfo::kEventResultActionNone;
    
    GetDefaultEventBehavior(AEInfo::EventKey(inEventClass, inEventID), doBehavior, resultAction);
    
    DefineEvent(inEventClass, inEventID, doBehavior, resultAction);
}

// ------------------------------------------------------------------------------------------
bool
AESDefReader::IsEventDefined(
    AEEventClass        inEventClass,   //!< The Apple Event's class.
    AEEventID           inEventID)      //!< The Apple Event's ID.
    const
{
    return (mEventMap.find(AEInfo::EventKey(inEventClass, inEventID)) != mEventMap.end());
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::CompleteClassHierarchy()
{
    std::for_each(mClassMap.begin(), mClassMap.end(), 
                  boost::bind(PopulateAncestors, _1, boost::ref(mClassMap)));

    std::sort(mClassOrder.begin(), mClassOrder.end(), 
              boost::bind(ClassTopologicalLess, _1, _2, boost::ref(mClassMap)));

    std::for_each(mClassOrder.begin(), mClassOrder.end(), 
                  boost::bind(PropagateClassInfo, _1, boost::ref(mClassMap)));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::PopulateAncestors(
    ClassMapType&       ioValue, 
    AEInfo::ClassMap&   ioClassMap)
{
    AEInfo::ClassInfo&  classInfo   = ioValue.second;
    
    for (DescType parentID = classInfo.mParentCode; parentID != 0; )
    {
        AEInfo::ClassInfo&  parentInfo  = ioClassMap.find(parentID)->second;
        
        parentInfo.mDescendents.insert(classInfo.mCode);
        classInfo.mAncestors.insert(parentInfo.mCode);
        
        parentID = parentInfo.mParentCode;
    }
}

// ------------------------------------------------------------------------------------------
bool
AESDefReader::ClassTopologicalLess(
    DescType                inClassID1,
    DescType                inClassID2,
    const AEInfo::ClassMap& inClassMap)
{
    return (inClassMap.find(inClassID1)->second.mAncestors.size() < 
            inClassMap.find(inClassID2)->second.mAncestors.size());
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::PropagateClassInfo(
    DescType                inClassID,
    AEInfo::ClassMap&       ioClassMap)
{
    const AEInfo::ClassInfo&            classInfo   = ioClassMap.find(inClassID)->second;
    std::set<DescType>::const_iterator  beginIt     = classInfo.mDescendents.begin();
    std::set<DescType>::const_iterator  endIt       = classInfo.mDescendents.end();
    
    std::for_each(beginIt, endIt, 
                  boost::bind(PropagateClassInfoToClass<AEInfo::ElementMap>, 
                              boost::ref(classInfo.mElements), boost::ref(ioClassMap), _1, 
                              PropagateElement));
    
    std::for_each(beginIt, endIt, 
                  boost::bind(PropagateClassInfoToClass<AEInfo::PropertyMap>, 
                              boost::ref(classInfo.mProperties), boost::ref(ioClassMap), _1,
                              PropagateProperty));
    
    std::for_each(beginIt, endIt, 
                  boost::bind(PropagateClassInfoToClass<AEInfo::ClassEventMap>, 
                              boost::ref(classInfo.mEvents), boost::ref(ioClassMap), _1,
                              PropagateClassEvent));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::PropagateElement(
    const ElementMapType&   inElement, 
    AEInfo::ClassInfo&      ioClassInfo)
{
    std::pair<AEInfo::ElementMap::iterator, bool>   p(ioClassInfo.mElements.insert(inElement));
    
    if (!p.second)
    {
        // The element already exists in the map, so just add to its key forms.
        
        AEInfo::ElementInfo&        elemInfo    = p.first->second;
        const AEInfo::KeyFormSet&   keyForms    = inElement.second.mKeyForms;
        
        std::copy(keyForms.begin(), keyForms.end(),
                  std::inserter(elemInfo.mKeyForms, elemInfo.mKeyForms.end()));
    }
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::PropagateProperty(
    const PropertyMapType&  inProperty, 
    AEInfo::ClassInfo&      ioClassInfo)
{
    std::pair<AEInfo::PropertyMap::iterator, bool>  p(ioClassInfo.mProperties.insert(inProperty));
}

// ------------------------------------------------------------------------------------------
void
AESDefReader::PropagateClassEvent(
    const ClassEventMapType&    inClassEvent, 
    AEInfo::ClassInfo&          ioClassInfo)
{
    std::pair<AEInfo::ClassEventMap::iterator, bool>    p(ioClassInfo.mEvents.insert(inClassEvent));
    
    if (!p.second)
    {
        // The class event already exists in the map, to just change its event handler.
        
        p.first->second = inClassEvent.second;
    }
}

// ------------------------------------------------------------------------------------------
AEInfo::Access
AESDefReader::GetAccessAttribute(CFXMLTreeRef tree)
{
    typedef std::map<String, AEInfo::Access>    AccessMap;
    
    static AccessMap    sAccessMap;
    
    if (sAccessMap.empty())
    {
        sAccessMap.insert(AccessMap::value_type(String("r"),  AEInfo::kAccessRead));
        sAccessMap.insert(AccessMap::value_type(String("w"),  AEInfo::kAccessWrite));
        sAccessMap.insert(AccessMap::value_type(String("rw"), AEInfo::kAccessReadWrite));
    }
    
    AEInfo::Access  access  = AEInfo::kAccessReadWrite;
    
    if (ElementContainsAttribute(tree, CFSTR("access")))
    {
        AccessMap::const_iterator   it;
        
        it = sAccessMap.find(GetElementAttribute(tree, CFSTR("access")));
        
        if (it != sAccessMap.end())
            access = it->second;
    }
    
    return (access);
}

// ------------------------------------------------------------------------------------------
bool
AESDefReader::GetElementYesNoAttribute(CFXMLTreeRef tree, CFStringRef attr, bool defaultValue)
{
    const CFStringCompareFlags  kFlags  = kCFCompareCaseInsensitive;
    String                      value;
    
    if (GetElementAttribute(tree, attr, value))
    {
        CFStringRef valueRef    = value.cf_ref();
        
        if (CFStringCompare(valueRef, CFSTR("yes"), kFlags) == 0)
            return true;
        else if (CFStringCompare(valueRef, CFSTR("no"), kFlags) == 0)
            return false;
    }
    
    return defaultValue;
}

// ------------------------------------------------------------------------------------------
bool
AESDefReader::ElementContainsAttribute(CFXMLTreeRef tree, CFStringRef attr)
{
    const CFXMLElementInfo& elem    = Tree2Element(tree);
    
    B_ASSERT(elem.attributes != NULL);
    
    return (CFDictionaryContainsKey(elem.attributes, attr));
}

// ------------------------------------------------------------------------------------------
AEInfo::EventKey
AESDefReader::GetCommandCodeAttribute(CFXMLTreeRef tree)
{
    String          codeStr     = GetElementAttribute(tree, CFSTR("code"));
    String          classStr    = codeStr.substr(0, sizeof(AEEventClass));
    String          idStr       = codeStr.substr(sizeof(AEEventClass), sizeof(AEEventID));
    AEEventClass    cmdClass    = UTGetOSTypeFromString(classStr.cf_ref());
    AEEventID       cmdID       = UTGetOSTypeFromString(idStr.cf_ref());
    
    return (AEInfo::EventKey(cmdClass, cmdID));
}

// ------------------------------------------------------------------------------------------
DescType
AESDefReader::GetCodeAttribute(CFXMLTreeRef tree)
{
    String      codeStr = GetElementAttribute(tree, CFSTR("code"));
    
    return (UTGetOSTypeFromString(codeStr.cf_ref()));
}

// ------------------------------------------------------------------------------------------
bool
AESDefReader::GetElementAttribute(CFXMLTreeRef tree, CFStringRef attr, String& value)
{
    const CFXMLElementInfo& elem    = Tree2Element(tree);
    
    B_ASSERT(elem.attributes != NULL);
    
    OSPtr<CFStringRef>  valuePtr;
    bool                good;
    
    good = CFUGet(elem.attributes, attr, valuePtr);
    
    if (good)
        value = valuePtr;
    
    return (good);
}

// ------------------------------------------------------------------------------------------
String
AESDefReader::GetElementAttribute(CFXMLTreeRef tree, CFStringRef attr)
{
    String  value;
    
    if (!GetElementAttribute(tree, attr, value))
        B_THROW(std::runtime_error("Missing attribute"));
    
    return (value);
}

}   // namespace B
