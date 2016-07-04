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

#ifndef BAESDefReader_H_
#define BAESDefReader_H_

#pragma once

// B headers
#include "BAEInfo.h"
#include "BString.h"


namespace B {


// forward declarations
class   Bundle;


class AESDefReader
{
public:
    
    // types
    typedef AEInfo::ClassEventMap::value_type   ClassEventMapType;
    
    AESDefReader(
        AEInfo::ClassMap&           ioClassMap,
        AEInfo::EventMap&           ioEventMap, 
        AEInfo::DefaultEventHandler inDefaultEventHandler);
    
    void    Read(
                const Bundle&   inBundle, 
                const String&   inSDefName = String());
    
    static void GetDefaultEventBehavior(
                    const AEInfo::EventKey&     inEventKey,
                    AEInfo::EventDOBehavior&    ioBehavior, 
                    AEInfo::EventResultAction&  ioAction);
    static void PropagateClassEvent(
                    const ClassEventMapType&    inClassEvent, 
                    AEInfo::ClassInfo&          ioClassInfo);
    
#ifndef NDEBUG
    void    DebugPrint();
#endif

private:

    // types
    typedef std::map<String, DescType>          NameMap;
    typedef std::map<String, AEInfo::EventKey>  EventNameMap;
    typedef NameMap::value_type                 NameMapType;
    typedef EventNameMap::value_type            EventNameMapType;
    typedef AEInfo::ClassMap::value_type        ClassMapType;
    typedef AEInfo::EventMap::value_type        EventMapType;
    typedef AEInfo::PropertyMap::value_type     PropertyMapType;
    typedef AEInfo::ElementMap::value_type      ElementMapType;
    
    typedef boost::function1<void, CFXMLTreeRef>    XmlTreeFunctor;
    
    OSPtr<CFXMLTreeRef>
                ReadSDefFile(
                    const Bundle&       inBundle, 
                    const String&       inSDefName);
    void        RecordSuiteNames(
                    CFXMLTreeRef        inSuiteTree);
    void        RegisterSuite(
                    CFXMLTreeRef        inSuiteTree);
    void        InvokeFunctorOnElements(
                    CFXMLTreeRef        inParentTree, 
                    CFStringRef         inTagName,
                    XmlTreeFunctor      inFunctor);
    void        RecordTypeName(
                    CFXMLTreeRef        inEnumTree);
    void        RecordEventName(
                    CFXMLTreeRef        eventTree);
    void        RegisterClass(
                    CFXMLTreeRef        inClassTree);
    void        RegisterEvent(
                    CFXMLTreeRef        eventTree);
    AEInfo::ClassInfo&  GetClassInfoForClassName(
                            const String&               inClassName);
    void                RegisterClassElement(
                            CFXMLTreeRef                inElementTree, 
                            AEInfo::ClassInfo&          ioClassInfo);
    void                RegisterClassContents(
                            CFXMLTreeRef                inPropertyTree, 
                            AEInfo::PropertyMap&        ioPropertyMap);
    void                RegisterClassProperty(
                            CFXMLTreeRef                inPropertyTree, 
                            AEInfo::PropertyMap&        ioPropertyMap);
    void                RegisterClassPropertyWithName(
                            CFXMLTreeRef                inPropertyTree, 
                            DescType                    inPropertyName,
                            AEInfo::PropertyMap&        ioPropertyMap);
    static bool         FindPropertyListType(
                            CFXMLTreeRef                inElementTree);
    void                RegisterClassEvent(
                            CFXMLTreeRef                inClassEventTree, 
                            const String&               inClassName, 
                            AEInfo::ClassEventMap&      ioClassEventMap);
    void                AddMissingEventToClass(
                            AEEventClass                inEventClass,
                            AEEventID                   inEventID,
                            AEInfo::ClassEventMap&      ioClassEventMap);
    DescType            GetElementTypeCode(
                            CFXMLTreeRef                inElementTree);
    void                RegisterElementAccessor(
                            CFXMLTreeRef                inAccessorTree, 
                            AEInfo::ClassInfo&          ioClassInfo, 
                            AEInfo::ElementInfo&        ioElementInfo);
    void                DefineEvent(
                            AEEventClass                inEventClass, 
                            AEEventID                   inEventID, 
                            AEInfo::EventDOBehavior     inEventDOBehavior, 
                            AEInfo::EventResultAction   inEventResultAction);
    void                DefineEvent(
                            AEEventClass                inEventClass, 
                            AEEventID                   inEventID);
    bool                IsEventDefined(
                            AEEventClass                inEventClass, 
                            AEEventID                   inEventID) const;
    void                CompleteClassHierarchy();
    static void         PopulateAncestors(
                            ClassMapType&               ioValue, 
                            AEInfo::ClassMap&           ioClassMap);
    static bool         ClassTopologicalLess(
                            DescType                    inClassID1,
                            DescType                    inClassID2,
                            const AEInfo::ClassMap&     ioClassMap);
    static void         PropagateClassInfo(
                            DescType                    inClassID,
                            AEInfo::ClassMap&           ioClassMap);
    static void         PropagateElement(
                            const ElementMapType&       inElement, 
                            AEInfo::ClassInfo&          ioClassInfo);
    static void         PropagateProperty(
                            const PropertyMapType&      inProperty, 
                            AEInfo::ClassInfo&          ioClassInfo);
                            
    static AEInfo::Access   GetAccessAttribute(
                                CFXMLTreeRef    tree);
    static bool             GetElementYesNoAttribute(
                                CFXMLTreeRef    tree, 
                                CFStringRef     attr, 
                                bool            defaultValue);
    static bool             ElementContainsAttribute(
                                CFXMLTreeRef    tree, 
                                CFStringRef     attr);
    static AEInfo::EventKey GetCommandCodeAttribute(
                                CFXMLTreeRef    tree);
    static DescType         GetCodeAttribute(
                                CFXMLTreeRef    tree);
    static bool             GetElementAttribute(
                                CFXMLTreeRef    tree, 
                                CFStringRef     attr, 
                                String&         value);
    static String           GetElementAttribute(
                                CFXMLTreeRef    tree, 
                                CFStringRef     attr);
    
    template <typename CONTAINER>
    static void         PropagateClassInfoToClass(
                            const CONTAINER&            inContainer, 
                            AEInfo::ClassMap&           ioClassMap,
                            DescType                    inClassID, 
                            boost::function2<void, 
                                const typename CONTAINER::value_type&, 
                                AEInfo::ClassInfo&>     inFunction);

    // member variables
    AEInfo::ClassMap&           mClassMap;
    AEInfo::EventMap&           mEventMap;
    AEInfo::DefaultEventHandler mDefaultEventHandler;
    NameMap                     mKeyFormNames;
    NameMap                     mTypeNames;
    EventNameMap                mEventNames;
    std::vector<DescType>       mClassOrder;
};

// ------------------------------------------------------------------------------------------
template <typename CONTAINER> void
AESDefReader::PropagateClassInfoToClass(
    const CONTAINER&        inContainer, 
    AEInfo::ClassMap&       ioClassMap,
    DescType                inClassID, 
    boost::function2<void, 
        const typename CONTAINER::value_type&, 
        AEInfo::ClassInfo&> inFunction)
{
    AEInfo::ClassInfo&  classInfo   = ioClassMap.find(inClassID)->second;
    
    std::for_each(inContainer.begin(), inContainer.end(), 
                  boost::bind(inFunction, _1, boost::ref(classInfo)));
}


}   // namespace B


#endif  // BAESDefReader_H_
