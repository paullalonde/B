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

#ifndef ModelItem_H_
#define ModelItem_H_

#pragma once

// standard headers
#include <iosfwd>
#include <map>
#include <vector>

// library headers
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

// B headers
#include "BAEObject.h"
#include "BException.h"
#include "BFwd.h"
#include "BOSPtr.h"


// forward declarations
namespace B {
    class   AbstractDocument;
    class   AEReader;
    class   AEWriter;
    class   UndoAction;
}
class                                       ModelItem;
typedef boost::shared_ptr<ModelItem>        ModelItemPtr;
typedef boost::shared_ptr<ModelItem const>  ConstModelItemPtr;


class ModelItem : public B::AEObject
{
public:
    
    // constants
    enum            { kModelItemKind = 'MItm' };
    enum            { pModelItemValueProperty = 'pIVl' };
    enum EParent    { parent_None, parent_Dictionary, parent_Array };
    enum EClass     { class_String, class_Data, class_Integer, class_Real, 
                      class_Boolean, class_Date, class_Array, class_Dictionary };
    
    // types
    template <EClass C> struct ItemValue
    {
        typedef int ItemType;
        typedef int ValueType;
        
        static const DescType   kAEType = 0;
        
        static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
        static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
        static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
    };
    
    typedef std::map<B::String, ModelItemPtr>   SubItemMap;
    typedef SubItemMap::value_type              SubItemMapValue;
    typedef SubItemMap::iterator                SubItemMapIter;
    typedef SubItemMap::const_iterator          SubItemMapConstIter;
    typedef std::vector<ModelItemPtr>           SubItemArray;
    typedef SubItemArray::iterator              SubItemArrayIter;
    typedef SubItemArray::const_iterator        SubItemArrayConstIter;
    typedef boost::signal0<void>                Signal;
    typedef boost::signal1<void, ModelItemPtr>  NewChildSignal;
    
    // Instantiation
    static ModelItemPtr Create(
                            B::AEObjectPtr      inContainer, 
                            B::OSPtr<CFTypeRef> inValue);
    static ModelItemPtr Clone(
                            const ModelItem&    inModel);
    
    // inquiries
    bool    IsParentDictionary() const;
    bool    IsParentArray() const;
    bool    IsRoot() const;
    EClass  GetValueClass() const       { return (mClass); }
    bool    IsDictionary() const        { return (mClass == class_Dictionary); }
    bool    IsArray() const             { return (mClass == class_Array); }
    bool    IsContainer() const         { return (IsDictionary() || IsArray()); }
    SInt32  CountChildren() const;
    void    GetChildren(
                std::vector<ModelItemPtr>&  items) const;
    bool    IsInSameContainerAs(const ModelItem& inItem) const;
    ModelItemPtr        GetPtr();
    ConstModelItemPtr   GetPtr() const;
    
    // getting the value
    const B::String&    GetValueString() const;
    B::OSPtr<CFDataRef> GetValueData() const;
    SInt64              GetValueInteger() const;
    double              GetValueReal() const;
    bool                GetValueBoolean() const;
    B::OSPtr<CFDateRef> GetValueDate() const;
    
    B::OSPtr<CFTypeRef> MakeValueAsPropertyList() const;
    void                WriteItemPointerFlavor(std::ostream& ostr) const;
    static ModelItemPtr ReadItemPointerFlavor(std::istream& istr);
    void                WriteInternalFlavor(std::ostream& ostr) const;
    void                WritePropertyListFlavor(std::ostream& ostr) const;
#if 0
    void                WriteToStream(std::ostream& ostr);
    static B::OSPtr<CFTypeRef>
                        ReadFromStream(std::istream& istr, B::String& outName);
#endif
    void                Select();
    
    // overrides from AEObject
    virtual unsigned    GetIndex() const;
    virtual B::String   GetName() const;
    virtual size_t      CountElements(
                            DescType        inElementType) const;
    virtual B::AEObjectPtr  GetElementByIndex(
                            DescType        inElementType, 
                            size_t          inIndex) const;
    virtual B::AEObjectPtr  GetElementByName(
                            DescType        inElementType, 
                            const B::String& inName) const;
    virtual B::AEObjectPtr  GetPropertyObject(
                            DescType        inPropertyID) const;
    virtual void        WriteProperty(
                            DescType        inPropertyID, 
                            B::AEWriter&    ioWriter) const;
    virtual void        ReadProperty(
                            DescType        inPropertyID, 
                            B::AEReader&    ioReader);
    virtual B::AEObjectPtr  CloneObject(
                            DescType        inPosition,
                            B::AEObjectPtr  inTarget, 
                            const AERecord& inProperties);
    virtual B::AEObjectPtr  CreateObject(
                            DescType        inObjectClass,
                            DescType        inPosition,
                            B::AEObjectPtr  inTarget, 
                            const AERecord& inProperties,
                            const AEDesc&   inData);
    virtual void        DeleteObject();
    virtual B::AEObjectPtr  MoveObject(
                            DescType        inPosition,
                            B::AEObjectPtr  inTarget);
    virtual void        MakeSpecifier(
                            B::AEWriter&    ioWriter) const;
    
    // Apple %Event Object Model support
    void            SendChangeClassAppleEvent(
                        EClass              inNewClass);
    
    // conversions
    void            WriteValue(
                        B::AEWriter&    ioWriter) const;
    static B::OSPtr<CFTypeRef>
                    ReadItem(
                        B::AEReader&    ioReader);
                    
    // signals
    Signal&         GetNameChangedSignal()      { return (mNameChangedSignal); }
    Signal&         GetClassChangingSignal()    { return (mClassChangingSignal); }
    Signal&         GetClassChangedSignal()     { return (mClassChangedSignal); }
    Signal&         GetValueChangedSignal()     { return (mValueChangedSignal); }
    NewChildSignal& GetNewChildSignal()         { return (mNewChildSignal); }
    Signal&         GetDeletedSignal()          { return (mDeletedSignal); }
    Signal&         GetSelectSignal()           { return (mSelectSignal); }
    
private:
    
    // types
    class       Init;
                            
    // constructors / destructor
            ModelItem(
                B::AEObjectPtr      inContainer);
            ModelItem(
                const ModelItem&    inModel);
    
    // initialisation / cleanup
    void        InitModelItem(
                    B::OSPtr<CFTypeRef> inValue);
    void        SetModelItemValue(
                    B::OSPtr<CFTypeRef> inValue);

    B::AbstractDocument*
                    GetDocument() const;
    EventTargetRef  GetUndoTarget() const;
    
    B::String   MakeUniqueDictionaryKeyForNewItem() const;
    B::String   MakeUniqueDictionaryKeyforCopy(
                    const B::String&    inRootName) const;
    
    ModelItemPtr    MakeNewDictionarySubItem(
                        B::OSPtr<CFTypeRef> inValue,
                        const B::String&    inKey, 
                        DescType            inPosition, 
                        ModelItemPtr        inTargetItem);
    ModelItemPtr    MakeNewArraySubItem(
                        B::OSPtr<CFTypeRef> inValue,
                        DescType            inPosition, 
                        ModelItemPtr        inTargetItem);
    ModelItemPtr    CopyDictionarySubItem(
                        ModelItemPtr        inOriginal, 
                        DescType            inPosition, 
                        ModelItemPtr        inTargetItem);
    ModelItemPtr    CopyArraySubItem(
                        ModelItemPtr        inOriginal, 
                        DescType            inPosition, 
                        ModelItemPtr        inTargetItem);
    void            InsertDictionarySubItem(
                        ModelItemPtr        inSubItem, 
                        DescType            inPosition, 
                        ModelItemPtr        inTargetItem, 
                        const B::String&    inSubItemKey);
    void            InsertArraySubItem(
                        ModelItemPtr        inSubItem, 
                        DescType            inPosition, 
                        ModelItemPtr        inTargetItem);
    void            DeleteReplacedSubItem(
                        ModelItemPtr        inModelItem, 
                        ModelItemPtr        inContainer);
    void            RemoveSubItem(
                        ModelItemPtr        inSubItem);
    
    // utility
    static B::OSPtr<CFTypeRef>
            ReadPropertyList(
                    B::AEReader&    ioReader);
    
    // callbacks
    static pascal OSErr CoerceToPropertyList(
                            const AEDesc*   fromDesc, 
                            DescType        toType, 
                            long            handlerRefcon, 
                            AEDesc*         result);
    
    // member variables
    EClass              mClass;
    B::String           mValueString;
    B::OSPtr<CFDataRef> mValueData;
    SInt64              mValueInteger;
    double              mValueReal;
    bool                mValueBoolean;
    B::OSPtr<CFDateRef> mValueDate;
    SubItemArray        mValueArray;
    SubItemMap          mValueDictionary;
    Signal              mNameChangedSignal;
    Signal              mClassChangingSignal;
    Signal              mClassChangedSignal;
    Signal              mValueChangedSignal;
    NewChildSignal      mNewChildSignal;
    Signal              mDeletedSignal;
    Signal              mSelectSignal;
    
    // static member variables
    static Init     sInit;
};


template <> struct ModelItem::ItemValue<ModelItem::class_String>
{
    typedef CFStringRef ItemType;  
    typedef B::String   ValueType;
    
    static const DescType   kAEType = typeUTF8Text;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Data>
{
    typedef CFDataRef           ItemType;  
    typedef B::OSPtr<CFDataRef> ValueType;
    
    static const DescType   kAEType = typeData;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Integer>
{
    typedef CFNumberRef ItemType;  
    typedef SInt64      ValueType;
    
    static const DescType   kAEType = typeSInt64;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Real>
{
    typedef CFNumberRef ItemType;  
    typedef double      ValueType;
    
    static const DescType   kAEType = typeIEEE64BitFloatingPoint;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Boolean>
{
    typedef CFBooleanRef    ItemType;  
    typedef bool            ValueType;
    
    static const DescType   kAEType = typeBoolean;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Date>
{
    typedef CFDateRef           ItemType;  
    typedef B::OSPtr<CFDateRef> ValueType;
    
    static const DescType   kAEType = typeLongDateTime;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ValueType& outValue);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Array>
{
    typedef CFArrayRef              ItemType;  
    typedef ModelItem::SubItemArray ValueType;
    
    static const DescType   kAEType = typeAEList;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ModelItemPtr inParent);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};

template <> struct ModelItem::ItemValue<ModelItem::class_Dictionary>
{
    typedef CFDictionaryRef         ItemType;  
    typedef ModelItem::SubItemMap   ValueType;
    
    static const DescType   kAEType = typeAERecord;

    static void                 ItemToValue(B::OSPtr<CFTypeRef> inItem, ModelItemPtr inParent);
    static B::OSPtr<CFTypeRef>  ValueToItem(const ValueType& inValue);
    static B::OSPtr<CFTypeRef>  ReadItem(B::AEReader& ioReader);
};



//  Inline member function definitions

inline const B::String&
ModelItem::GetValueString() const
{
    B_ASSERT(mClass == class_String);
    
    return (mValueString);
}

inline B::OSPtr<CFDataRef>
ModelItem::GetValueData() const
{
    B_ASSERT(mClass == class_Data);
    
    return (mValueData);
}

inline SInt64
ModelItem::GetValueInteger() const
{
    B_ASSERT(mClass == class_Integer);
    
    return (mValueInteger);
}

inline double
ModelItem::GetValueReal() const
{
    B_ASSERT(mClass == class_Real);
    
    return (mValueReal);
}

inline bool
ModelItem::GetValueBoolean() const
{
    B_ASSERT(mClass == class_Boolean);
    
    return (mValueBoolean);
}

inline B::OSPtr<CFDateRef>
ModelItem::GetValueDate() const
{
    B_ASSERT(mClass == class_Date);
    
    return (mValueDate);
}


class DuplicateItemException : public std::exception
{
public:
    
    // inquiries
    virtual const char* what() const throw()    { return ("DuplicateItemException"); }
};


class MoveIntoOffspringException : public std::exception
{
public:
    
    // inquiries
    virtual const char* what() const throw()    { return ("MoveIntoOffspringException"); }
};


#endif  // ModelItem_H_
