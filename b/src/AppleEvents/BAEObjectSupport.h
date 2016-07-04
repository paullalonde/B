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

#ifndef BAEObjectSupport_H_
#define BAEObjectSupport_H_

#pragma once

// standard headers
#include <set>

// library headers
#include <boost/thread/tss.hpp>

// B headers
#include "BAEDescriptor.h"
#include "BAEDescParam.h"
#include "BAEinfo.h"
#include "BAutoUPP.h"
#include "BException.h"
#include "BString.h"


namespace B {


// forward declarations
class   AEEventHook;
class   AEObject;
class   AEToken;
class   AEWriter;
class   Bundle;


/*!
    @brief  This class provides essential support for the Apple %Event Object Model (AEOM) 
            in the B framework.
    
    AEObjectSupport is class with which an application will interact directly mostly 
    at application initialisation time.  The interaction follows these phases:
    
    -# Construction.  AEObjectSupport is a singleton, so only one global instance may be 
       created during the lifetime of the application.
    -# Event Definition (optional).  Here the caller may inform AEObjectSupport of any 
       application-defined AppleEvents that it needs to be aware of (so it knows how do 
       things like resolving the event direct parameter).  AEObjectSupport contains 
       built-in definitions for the vast majority of events in the Required, Core, and 
       Miscellaneous Standards Suite, so this phase is only required when defining your 
       own custom AppleEvents.
    -# Event Handler Registration (optional).  Most event handlers are class specific, 
       however it may sometimes be convenient to register events that aren't tied to 
       a class.  This can happen if an Apple %Event has no direct parameter, or if it's 
       not of a type resolvable into an AEObject.  One example is the "open" Apple %Event 
       taking a file as a direct parameter.
    -# Class Registration.  This involves getting the class to declare the information
       required by AEObjectSupport in order to properly dispatch events to instances 
       of the class.  This is typically done by calling Register().
    -# Activation.  The Start() function actually initialises MOSL, which in turn sets 
       up the necessary Apple %Event handlers and object accessors.
    -# Steady state.  Once AEObjectSupport has been started, the application can 
       receive AppleEvents and resolve object specifiers.  The application typically 
       doesn't interact directly with it any more.
    
    @ingroup    AppleEvents
    @todo       Complete documentation
*/
class AEObjectSupport
{
public:
    
    class ObjectResolutionException : public RuntimeOSStatusException
    {
    public:
        
        //! Copy constructor.
                    ObjectResolutionException(const ObjectResolutionException& ex);
        //! @c OSStatus constructor.
        explicit    ObjectResolutionException(OSStatus inStatus, const AEDesc& inErrorDesc);
        //! Stream constructor.
        explicit    ObjectResolutionException(std::istream& istr);
        //! Destructor.
        virtual     ~ObjectResolutionException() throw();
        
        //! Copy assignment.
        ObjectResolutionException&  operator = (const ObjectResolutionException& ex);

        //! Returns the exception's textual description.
        virtual const char* what() const throw();
        
        //! Writes out the exception's internal state to @a ostr.
        void        Write(std::ostream& ostr) const;
        
    private:
        
        // member variables
        std::vector<char>   mErrorDesc;
    };
    
    //@}
    
    //! @name Accessing The Sole Instance
    //@{
    //! Returns the AEObjectSupport singleton.
    static const AEObjectSupport&   Get();
    //@}
    
    //! @name Constructor / Destructor
    //@{
    //! Constructor.
            AEObjectSupport();
    //! Destructor.
    virtual ~AEObjectSupport();
    //@}
    
    //! @name Event Registration
    //@{
    //! Tells AEObjectSupport to install an Apple %Event handler for a given event.
    virtual void
            DefineEvent(
                AEEventClass                inEventClass, 
                AEEventID                   inEventID, 
                AEInfo::EventDOBehavior     inEventDOBehavior, 
                AEInfo::EventResultAction   inEventResultAction, 
                AEInfo::DefaultEventHandler inHandler);
    void    DefineEvent(
                AEEventClass                inEventClass, 
                AEEventID                   inEventID, 
                AEInfo::EventDOBehavior     inEventDOBehavior, 
                AEInfo::EventResultAction   inEventResultAction);
    void    DefineEvent(
                AEEventClass                inEventClass, 
                AEEventID                   inEventID);
    //! Returns @c true if the given event is known to AEObjectSupport.
    virtual bool
            IsEventDefined(
                AEEventClass                inEventClass, 
                AEEventID                   inEventID) const;
    
    //! Installs a handler for a given Apple %Event and a given AEObject class.
    void    SetClassEventHandler(
                DescType                    inClassID,
                AEEventClass                inEventClass, 
                AEEventID                   inEventID, 
                AEInfo::ClassEventHandler   inClassHandler);
    /*! @overload
    */
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    void    SetClassEventHandler(
                DescType            inClassID,
                void                (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&));
    /*! @overload
    */
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    void    SetClassEventHandler(
                OBJ*                inObj, 
                void                (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&));
    /*! @overload
    */
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    void    SetClassEventHandler(
                DescType            inClassID,
                OBJ*                inObj, 
                void                (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&));
    
    //! Installs a default handler for a given Apple Event.
    void    SetDefaultEventHandler(
                AEEventClass                inEventClass, 
                AEEventID                   inEventID, 
                AEInfo::DefaultEventHandler inDefaultHandler);
    /*! @overload
    */
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    void    SetDefaultEventHandler(
                OBJ*                inObj, 
                void                (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&));
    /*! @overload
    */
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    void    SetDefaultEventHandler(
                OBJ*                inObj, 
                void                (OBJ::*inObjMember)(const AEDesc&, AEEvent<EVT_CLASS, EVT_ID>&));
    //@}
    
    //! @name Comparison Registration
    //@{
    void        SetComparer(
                    DescType                    inDescriptorType,
                    AEInfo::DescriptorComparer  inComparer);
    template <DescType TYPE>
    void        SetSimpleComparer(
                    boost::function3<bool, 
                        DescType, 
                        typename DescParam<TYPE>::ValueType, 
                        typename DescParam<TYPE>::ValueType>
                                        inFunction);
    template <DescType TYPE>
    void        SetNumericComparer();
    //! Compare two strings according to @c inComparisonOp.
    static bool CompareStrings(
                    DescType            inOperator,
                    const String&       inString1,
                    const String&       inString2);
    //! Compare two strings for equality.
    static bool CompareStrings(
                    const String&       inString1,
                    const String&       inString2);
    //@}
    
    //! @name Activating AEOM Support
    //@{
    //! Tell AEObjectSupport to read in the terminology.
    void    RegisterScriptingDefinitions(
                const Bundle&       inBundle, 
                const String&       inSDefName = String());
    //@}
    
    //! @name Utility
    //@{
    //! Attempts to convert an Apple %Event descriptor into an AEObject.
    static AEObjectPtr
                Resolve(
                    const AEDesc&       inObjSpecifier);
    /*! @overload
    */
    static OSStatus
                Resolve(
                    const AEDesc&       inObjSpecifier, 
                    AEObjectPtr&        outObject);
    //! Returns true if @a inClassID is or derives from @a inBaseClassID.
    bool        DoesClassInheritFrom(
                    DescType            inClassID, 
                    DescType            inBaseClassID) const;
    //! Convenient-but-inefficient way of counting *all* of an object's elements.
    SInt32      CountAllElements(
                    ConstAEObjectPtr    inContainer) const;
    //! Serialises cached exception information into @a ioReply.
    static void AddCachedInfoToAppleEventReply(
                    AppleEvent&         ioReply, 
                    OSStatus            status);
    //! Throws the exception contained in @a inReply.
    static void RethrowExceptionFromAppleEventReply(
                    const AppleEvent&   inReply, 
                    OSStatus            inStatus);
    static void CoerceDesc(
                    const AEDesc&       inObjectSpecifier,
                    DescType            inDesiredType,
                    AEDesc&             outDesc);
    static void CoerceDesc(
                    const AEDesc&       inObjectSpecifier,
                    DescType            inDesiredType,
                    void*               outBuffer,
                    size_t              inBufferSize);
    static OSStatus CoerceDesc(
                    const AEDesc&       inObjectSpecifier,
                    DescType            inDesiredType,
                    AEDesc&             outDesc, 
                    const std::nothrow_t&);
    static OSStatus CoerceDesc(
                    const AEDesc&       inObjectSpecifier,
                    DescType            inDesiredType,
                    void*               outBuffer,
                    size_t              inBufferSize,
                    const std::nothrow_t&);
    const AEInfo::ClassInfo*
                FindClassInfo(
                    DescType            inClassID) const;
    const AEInfo::ClassInfo&
                GetClassInfo(
                    DescType            inClassID) const;
    const AEInfo::PropertyInfo*
                FindPropertyInfo(
                    const AEInfo::ClassInfo&    inClass, 
                    DescType                    inPropertyName) const;
    const AEInfo::PropertyInfo&
                GetPropertyInfo(
                    const AEInfo::ClassInfo&    inClass, 
                    DescType                    inPropertyName) const;

    bool    DoesCurrentEventContainHook(int key) const;
    void    AddCurrentEventHook(int key, std::auto_ptr<AEEventHook> eventHook);
    //@}
    
private:
    
    enum {
        kTokenAccessor,
        kPropertyAccessor,
        kListAccessor,
    };
    
    // types
    struct  ExInfo;
    class   ErrorDescLink;
    class   EventHookLink;
    
    typedef std::vector<AEInfo::EventKey>                   EventKeyVector;
    typedef std::map<DescType, AEInfo::DescriptorComparer>  ComparerMap;
    typedef ComparerMap::value_type                         ComparerMapType;
    typedef AEInfo::ClassEventMap::value_type               ClassEventMapType;
    typedef AEInfo::EventMap::value_type                    EventMapType;
    
    void        RegisterEventHandler(
                    const EventMapType&         inEvent);
    static void PropagateClassEventToClass(
                    const ClassEventMapType&    inClassEvent, 
                    AEInfo::ClassMap&           ioClassMap,
                    DescType                    inClassID);
    AEInfo::ClassEventHandler
                FindClassEventHandler(
                    const AEInfo::ClassInfo&    inClass, 
                    const AEInfo::EventKey&     inEventKey) const;
    
    ExInfo&     GetExInfo() const;
    void        ClearException() const;
    OSStatus    CacheExceptionInfo(const std::exception& ex) const;
    ErrorDescLink*
                SetErrorDescLink(ErrorDescLink* inLink) const throw();
    EventHookLink*
                SetEventHookLink(EventHookLink* inLink) const throw();
    static void NullDefaultEventHandler(
                    const AEDesc&       inDirectObject, 
                    const AppleEvent&   inEvent, 
                    AEDesc&             outResult);
    
    void        HandleDefaultEvent(
                    const AEDesc&       inDirectObject,
                    const AppleEvent&   inEvent,
                    AEWriter&           ioResultWriter) const;
                    
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    static void ForwardDefaultEvent(
                    const AEDesc&       inDirectObject,
                    const AppleEvent&   inEvent,
                    AEWriter&           ioResultWriter,
                    OBJ*                inObj, 
                    void                (OBJ::*inObjMember)(
                                            AEEvent<EVT_CLASS, EVT_ID>&));
    
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    static void ForwardDefaultEventWithDirectObj(
                    const AEDesc&       inDirectObject,
                    const AppleEvent&   inEvent,
                    AEWriter&           ioResultWriter,
                    OBJ*                inObj, 
                    void                (OBJ::*inObjMember)(
                                            const AEDesc&, 
                                            AEEvent<EVT_CLASS, EVT_ID>&));
    
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    static void ForwardClassEvent(
                    const AEToken&      inToken,
                    const AppleEvent&   inEvent,
                    AEWriter&           ioResultWriter,
                    void                (OBJ::*inObjMember)(
                                            AEEvent<EVT_CLASS, EVT_ID>&));
    
    template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
    static void ForwardClassEventToObject(
                    const AppleEvent&   inEvent,
                    AEWriter&           ioResultWriter,
                    OBJ*                inObj, 
                    void                (OBJ::*inObjMember)(
                                            AEEvent<EVT_CLASS, EVT_ID>&));
    
    template <DescType TYPE>
    static bool ForwardComparer(
                    DescType            inOperator,
                    const AEDesc&       inData1,
                    const AEDesc&       inData2,
                    boost::function3<bool, 
                        DescType, 
                        typename DescParam<TYPE>::ValueType, 
                        typename DescParam<TYPE>::ValueType>
                    inFunction);
    
    void        TokenAccessor(
                    DescType            inDesiredClass, 
                    const AEDesc&       inContainer, 
                    DescType            inContainerClass, 
                    DescType            inKeyForm, 
                    const AEDesc&       inKeyData, 
                    AEDesc&             outTokenDesc) const;
    void        PropertyAccessor(
                    DescType            inDesiredClass, 
                    const AEDesc&       inContainer, 
                    DescType            inContainerClass, 
                    DescType            inKeyForm, 
                    const AEDesc&       inKeyData, 
                    AEDesc&             outTokenDesc) const;
    void        ListAccessor(
                    DescType            inDesiredClass, 
                    const AEDesc&       inContainer, 
                    DescType            inContainerClass, 
                    DescType            inKeyForm, 
                    const AEDesc&       inKeyData, 
                    AEDesc&             outTokenDesc) const;
    void        ListItemAccessor(
                    DescType            inDesiredClass, 
                    DescType            inKeyForm, 
                    const AEDesc&       inKeyData, 
                    const AEDesc&       inItemDesc,
                    size_t&             ioMissingValueCount,
                    AEWriter&           ioWriter) const;
    //! Converts a MoreOSL token into an AEObject.
    AEObjectPtr TokenToObject(
                    const AEToken&      inToken) const;
    bool        HandleCompare(
                    DescType            inOperator, 
                    const AEDesc&       inTokenDesc1, 
                    const AEDesc&       inTokenDesc2) const;
    static void GetComparisonData(
                    const AEDesc&       inTokenDesc, 
                    AEDesc&             outDataDesc,
                    bool&               outIsObject,
                    bool&               outIsProperty);
    template <DescType TYPE>
    static bool CompareNumericData(
                    DescType                            inOperator,
                    typename DescParam<TYPE>::ValueType inValue1,
                    typename DescParam<TYPE>::ValueType inValue2);
    static bool CompareTypeData(
                    DescType            inOperator,
                    DescType            inType1,
                    DescType            inType2);
    static bool CompareBooleanData(
                    DescType            inOperator,
                    const AEDesc&       inData1,
                    const AEDesc&       inData2);
    static bool CompareBinaryData(
                    DescType            inOperator,
                    const AEDesc&       inData1,
                    const AEDesc&       inData2);
    static bool CompareStringData(
                    DescType            inOperator,
                    const AEDesc&       inData1,
                    const AEDesc&       inData2);
    size_t      HandleCount(
                    DescType            inDesiredType, 
                    DescType            inContainerClass, 
                    const AEToken&      inContainer) const;
    void        HandleAppleEvent(
                    const AEInfo::EventKey& inEventKey,
                    const AppleEvent&       inEvent,
                    AppleEvent&             outReply) const;
    void        RecursiveResolve(
                    const AEDesc&       inObjectSpecifier,
                    AEWriter&           ioTokenWriter,
                    bool&               ioListOpen) const;
    void        RecursiveResolveListItem(
                    const AEDesc&       inItemDesc,
                    AEWriter&           ioTokenWriter,
                    bool&               ioListOpen) const;
    static OSStatus
                PrivateResolve(
                    const AEDesc&       inObjectSpecifier,
                    AEDesc&             outTokenDesc,
                    AEDesc&             outErrorDesc,
                    short               inFlags = kAEIDoMinimum);
    void        DispatchAppleEvent(
                    const AEInfo::EventInfo&    inEventInfo,
                    const AppleEvent&           inEvent,
                    const AEDesc&               inDirectObjectDesc, 
                    AEWriter&                   ioResultWriter) const;
    void        ResultListCount(
                    const AppleEvent&   inEvent, 
                    const AEDescList&   inList, 
                    AEDesc&             outResultDesc) const;
    void        ResultListCountItem(
                    const AEDesc&       inItemDesc,
                    SInt32&             ioItemCount) const;
    void        ResultListCollapse(
                    const AppleEvent&   inEvent, 
                    const AEDescList&   inList, 
                    AEDesc&             outResultDesc) const;
    static bool IsResultBooleanFalse(
                    const AEDesc&       inDesc);
                        
    // callbacks
    static pascal OSErr     OSLAccessorProc(
                                DescType            inDesiredClass, 
                                const AEDesc*       inContainer, 
                                DescType            inContainerClass, 
                                DescType            inKeyForm, 
                                const AEDesc*       inKeyData, 
                                AEDesc*             outTokenDesc, 
                                long                inRefcon);
    static pascal OSErr     OSLCompareProc(
                                DescType            oper, 
                                const AEDesc*       obj1, 
                                const AEDesc*       obj2, 
                                Boolean*            result);
    static pascal OSErr     OSLCountProc(
                                DescType            desiredType, 
                                DescType            containerClass, 
                                const AEDesc*       container, 
                                long*               result);
    static pascal OSErr     OSLDisposeTokenProc(
                                AEDesc*             unneededToken);
    static pascal OSErr     OSLGetMarkTokenProc(
                                const AEDesc*       containerToken,
                                DescType            containerClass,
                                AEDesc*             markToken);
    static pascal OSErr     OSLMarkProc(
                                const AEDesc*       dataToken,
                                const AEDesc*       markToken,
                                long                index);
    static pascal OSErr     OSLAdjustMarksProc(
                                long                newStart,
                                long                newStop,
                                const AEDesc*       markToken);
    static pascal OSErr     OSLGetErrDescProc(
                                AEDesc**            appDesc);
    static pascal OSErr     AEEventHandlerProc(
                                const AppleEvent*   theAppleEvent, 
                                AppleEvent*         reply, 
                                long                handlerRefcon);
#ifndef NDEBUG
    static pascal OSErr     DebugOSLAccessorProc(
                                DescType            desiredClass, 
                                const AEDesc*       container, 
                                DescType            containerClass, 
                                DescType            form, 
                                const AEDesc*       selectionData, 
                                AEDesc*             value, 
                                long                accessorRefcon);
    static void             DebugPrint(
                                const char*         heading, 
                                const AEDesc*       desc);
#endif
    
    // member variables
    mutable boost::thread_specific_ptr<ExInfo>  mExInfoPtr;
    AEInfo::ClassMap        mClassMap;
    AEInfo::EventMap        mEventMap;
    EventKeyVector          mEventKeys;
    ComparerMap             mComparerMap;
    
    // static member variables
    static const AEObjectSupport*       sAEObjectSupport;
    static const AutoOSLAccessorUPP     sOSLAccessorUPP;
    static const AutoOSLCompareUPP      sOSLCompareUPP;
    static const AutoOSLCountUPP        sOSLCountUPP;
    static const AutoOSLDisposeTokenUPP sOSLDisposeTokenUPP;
    static const AutoOSLGetMarkTokenUPP sOSLGetMarkTokenUPP;
    static const AutoOSLMarkUPP         sOSLMarkUPP;
    static const AutoOSLAdjustMarksUPP  sOSLAdjustMarksUPP;
    static const AutoOSLGetErrDescUPP   sOSLGetErrDescUPP;
    static const AutoAEEventHandlerUPP  sAEEventHandlerUPP;
#ifndef NDEBUG
    static const AutoOSLAccessorUPP     sDebugOSLAccessorUPP;
#endif
    static AEDescriptor                 sNullDescriptor;
    
    // friends
    friend class    ErrorDescLink;
};


// ------------------------------------------------------------------------------------------
/*! Allows your class to register a callback for handling an Apple %Event.  The particular 
    event is identified by the @a EVT_CLASS and @a EVT_ID template parameters.
    
    Call this function if the Apple %Event in question takes a direct parameter.  The 
    callback, @a inObjMember, will point to a member function of class @a OBJ, and will 
    take as its sole argument a template instantiation of AEEvent for @a EVT_OBJ and 
    @a EVT_ID.
    
    The target object (the one used to call the member function) will be obtained by 
    resolving the event's direct parameter, which is why this function is only suitable 
    for Apple %Events that take object-specifier direct parameters.
    
    @param EVT_CLASS    Template parameter.  The Apple %Event class of the event.
    @param EVT_ID       Template parameter.  The Apple %Event id of the event.
    @param OBJ          Template parameter.  The class of the object that will handle the event.  This should be a derivative of AEObject.
*/
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
inline void
AEObjectSupport::SetClassEventHandler(
    DescType    inClassID,                                          //!< The class ID of the handler.
    void        (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&))   //!< Pointer-to-member (from class OBJ) taking the template specialisation of AEEvent<EVT_OBJ, EVT_ID>.
{
    SetClassEventHandler(inClassID, EVT_CLASS, EVT_ID, 
                         boost::bind(ForwardClassEvent<EVT_CLASS, EVT_ID, OBJ>, 
                                     _1, _2, _3, inObjMember));
}

// ------------------------------------------------------------------------------------------
/*! Allows your class to register a callback for handling an Apple %Event.  The particular 
    event is identified by the @a EVT_CLASS and @a EVT_ID template parameters.
    
    Call this function if the Apple %Event in question does @b not take a direct parameter.
    The callback, @a inObjMember, will point to a member function of class @a OBJ, and will 
    take as its sole argument a template instantiation of AEEvent for @a EVT_OBJ and 
    @a EVT_ID.
    
    The target object (the one used to call the member function) will be @a inObj, which is 
    why this function is only suitable for AppleEvents that do @b not take object-specifier 
    direct parameters.
    
    @param EVT_CLASS    Template parameter.  The Apple %Event class of the event.
    @param EVT_ID       Template parameter.  The Apple %Event id of the event.
    @param OBJ          Template parameter.  The class of the object that will handle the event.  This should be a derivative of AEObject.
*/
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
inline void
AEObjectSupport::SetClassEventHandler(
    DescType    inClassID,                                          //!< The class ID of the handler.
    OBJ*        inObj,                                              //!< The default object (the one that will receive all the the events).
    void        (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&))   //!< Pointer-to-member (from class OBJ) taking the template specialisation of AEEvent<EVT_OBJ, EVT_ID>.
{
    SetClassEventHandler(inClassID, EVT_CLASS, EVT_ID, 
                         boost::bind(ForwardClassEventToObject<EVT_CLASS, EVT_ID, OBJ>, 
                                     _2, _3, inObj, inObjMember));
}

// ------------------------------------------------------------------------------------------
/*! Allows your class to register a callback for handling an Apple %Event.  The particular 
    event is identified by the @a EVT_CLASS and @a EVT_ID template parameters.
    
    Call this function if the Apple %Event in question does @b not take a direct parameter.
    The callback, @a inObjMember, will point to a member function of class @a OBJ, and will 
    take as its sole argument a template instantiation of AEEvent for @a EVT_OBJ and 
    @a EVT_ID.
    
    The target object (the one used to call the member function) will be @a inObj, which is 
    why this function is only suitable for AppleEvents that do @b not take object-specifier 
    direct parameters.
    
    @param EVT_CLASS    Template parameter.  The Apple %Event class of the event.
    @param EVT_ID       Template parameter.  The Apple %Event id of the event.
    @param OBJ          Template parameter.  The class of the object that will handle the event.  This should be a derivative of AEObject.
*/
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
inline void
AEObjectSupport::SetClassEventHandler(
    OBJ*    inObj,                                              //!< The default object (the one that will receive all the the events).
    void    (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&))   //!< Pointer-to-member (from class OBJ) taking the template specialisation of AEEvent<EVT_OBJ, EVT_ID>.
{
    SetClassEventHandler(inObj->GetClassID(), EVT_CLASS, EVT_ID, 
                         boost::bind(ForwardClassEventToObject<EVT_CLASS, EVT_ID, OBJ>, 
                                     _2, _3, inObj, inObjMember));
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
inline void
AEObjectSupport::SetDefaultEventHandler(
    OBJ*    inObj, 
    void    (OBJ::*inObjMember)(AEEvent<EVT_CLASS, EVT_ID>&))
{
    SetDefaultEventHandler(EVT_CLASS, EVT_ID, 
                           boost::bind(ForwardDefaultEvent<EVT_CLASS, EVT_ID, OBJ>, 
                                       _1, _2, _3, inObj, inObjMember));
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
inline void
AEObjectSupport::SetDefaultEventHandler(
    OBJ*    inObj, 
    void    (OBJ::*inObjMember)(const AEDesc&, AEEvent<EVT_CLASS, EVT_ID>&))
{
    SetDefaultEventHandler(EVT_CLASS, EVT_ID, 
                           boost::bind(ForwardDefaultEventWithDirectObj<EVT_CLASS, EVT_ID, OBJ>, 
                                       _1, _2, _3, inObj, inObjMember));
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
inline void
AEObjectSupport::SetSimpleComparer(
    boost::function3<bool, 
        DescType, 
        typename DescParam<TYPE>::ValueType, 
        typename DescParam<TYPE>::ValueType>    inFunction)
{
    SetComparer(TYPE, 
                boost::bind(ForwardComparer<TYPE>, 
                            _1, _2, _3, inFunction));
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
inline void
AEObjectSupport::SetNumericComparer()
{
    SetComparer(TYPE, 
                boost::bind(ForwardComparer<TYPE>, 
                            _1, _2, _3, CompareNumericData<TYPE>));
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
bool
AEObjectSupport::CompareNumericData(
    DescType                            inOperator,
    typename DescParam<TYPE>::ValueType inValue1,
    typename DescParam<TYPE>::ValueType inValue2)
{
    // Numeric stuff.  We support all the standard relational operators.
    
    bool    result  = false;
    
    switch (inOperator)
    {
    case kAEEquals:             result = (inValue1 == inValue2);    break;
    case kAEGreaterThanEquals:  result = (inValue1 >= inValue2);    break;
    case kAEGreaterThan:        result = (inValue1 >  inValue2);    break;
    case kAELessThan:           result = (inValue1 <  inValue2);    break;
    case kAELessThanEquals:     result = (inValue1 <= inValue2);    break;
        
    case kAEBeginsWith:
    case kAEEndsWith:
    case kAEContains:
        B_THROW(AECantRelateObjectsException());
        break;
    
    default:
        B_THROW(AEUnrecognisedOperatorException());
        break;
    }
    
    return result;
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
void
AEObjectSupport::ForwardDefaultEvent(
    const AEDesc&       /* inDirectObject */,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter,
    OBJ*                inObj, 
    void                (OBJ::*inObjMember)(
                            AEEvent<EVT_CLASS, EVT_ID>&))
{
    AEEvent<EVT_CLASS, EVT_ID>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    (inObj->*inObjMember)(event);
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
void
AEObjectSupport::ForwardDefaultEventWithDirectObj(
    const AEDesc&       inDirectObject,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter,
    OBJ*                inObj, 
    void                (OBJ::*inObjMember)(
                            const AEDesc&, 
                            AEEvent<EVT_CLASS, EVT_ID>&))
{
    AEEvent<EVT_CLASS, EVT_ID>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    (inObj->*inObjMember)(inDirectObject, event);
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
void
AEObjectSupport::ForwardClassEvent(
    const AEToken&      inToken,
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter,
    void                (OBJ::*inObjMember)(
                            AEEvent<EVT_CLASS, EVT_ID>&))
{
    AEEvent<EVT_CLASS, EVT_ID>  event(inEvent, ioResultWriter);
    AEObjectPtr                 obj = sAEObjectSupport->TokenToObject(inToken);
    
    event.CheckRequiredParams();
    (dynamic_cast<OBJ&>(*obj).*inObjMember)(event);
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <AEEventClass EVT_CLASS, AEEventID EVT_ID, class OBJ>
void
AEObjectSupport::ForwardClassEventToObject(
    const AppleEvent&   inEvent,
    AEWriter&           ioResultWriter,
    OBJ*                inObj, 
    void                (OBJ::*inObjMember)(
                            AEEvent<EVT_CLASS, EVT_ID>&))
{
    AEEvent<EVT_CLASS, EVT_ID>  event(inEvent, ioResultWriter);
    
    event.CheckRequiredParams();
    (inObj->*inObjMember)(event);
    event.Update();
}

// ------------------------------------------------------------------------------------------
template <DescType TYPE>
bool
AEObjectSupport::ForwardComparer(
    DescType            inOperator,
    const AEDesc&       inData1,
    const AEDesc&       inData2,
    boost::function3<bool, 
        DescType, 
        typename DescParam<TYPE>::ValueType, 
        typename DescParam<TYPE>::ValueType>
                        inFunction)
{
    typename DescParam<TYPE>::ValueType value1, value2;
    
    DescParam<TYPE>::Get(inData1, value1);
    DescParam<TYPE>::Get(inData2, value2);
    
    return inFunction(inOperator, value1, value2);
}


template <> struct ExceptionTraits<AEObjectSupport::ObjectResolutionException>  { typedef StreamExceptionTag    Category; };

}   // namespace B


#endif  // BAEObjectSupport_H_
