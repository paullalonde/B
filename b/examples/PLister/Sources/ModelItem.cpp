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
#include "ModelItem.h"

// system headers
#include <CoreFoundation/CFDate.h>
#include <CoreFoundation/CFTimeZone.h>

// B headers
#include "BAEDescriptor.h"
#include "BAEEvent.h"
#include "BAEReader.h"
#include "BAEUndoAction.h"
#include "BAEWriter.h"
#include "BAutoUPP.h"
#include "BBundle.h"
#include "BExceptionStreamer.h"
#include "BStringFormatter.h"
#include "BUndo.h"
#include "BUtility.h"
#include "BValueAdapter.h"
#include "CFUtils.h"

// project headers
#include "PListerDoc.h"


namespace {

class ModelUndoAction : public B::AEUndoAction
{
public:
    
    B::String   GetActionName() const       { return (mActionName); }
    
protected:
    
    // constructor
    ModelUndoAction(const B::String& inActionName)
        : mActionName(inActionName) {}
    virtual ~ModelUndoAction() throw() {}
    
private:
    
    // member variables
    B::String   mActionName;
};


class UndoRename : public ModelUndoAction
{
public:
    
    // constructor
    UndoRename(
        const ModelItemPtr  inModelItem, 
        const B::String&    inNewName);
};

UndoRename::UndoRename(
    const ModelItemPtr  inModelItem, 
    const B::String&    inNewName)
        : ModelUndoAction(
                BLocalizedString(CFSTR("Rename"), 
                                 "Undo text for property list item name change."))
{
    // Because a model item's name is used to identify it, we can't simply use 
    // the MakeXXXAppleEvent functions in AEObject, because they will identify 
    // the item's via its current name, not its new name.
    
    // "set the name of item "<new-name>" of <...> to "<old-name>"
    
    B::AEEvent<kAECoreSuite, kAESetData>::Make(
        B::AESelfTarget(),
        B::AEEventFunctor::PropertySpecifier<
            B::AEObject::NameSpecifier, 
            B::AETypedObject<typeType> >(
                B::AEObject::NameSpecifier(
                    inModelItem->GetClassID(), 
                    inModelItem->GetContainer(), 
                    inNewName), 
            B::AETypedObject<typeType>(pName)),
        inModelItem->GetName(), 
        mEvent);
    
//  RecordComment("recording UndoRename");
//  B::AEEventBase::SendEvent(mEvent, kAEDontExecute);
}

class UndoValue : public ModelUndoAction
{
public:
    
    // constructor
    UndoValue(
        const ModelItemPtr  inModelItem);
};

UndoValue::UndoValue(
    const ModelItemPtr  inModelItem)
        : ModelUndoAction(
            BLocalizedString(CFSTR("Value Change"), 
                             "Undo text for property list item value change."))
{
    // Record current value.
    
    B::AEDescriptor valueDesc;
    
    inModelItem->SendGetPropertyAppleEvent(ModelItem::pModelItemValueProperty, 
                                           valueDesc, typeWildCard, 
                                           kAEDontRecord);
    
    // "set value of item <model-item> to <model-item's-current-value>"
    
    inModelItem->MakeSetPropertyAppleEvent(ModelItem::pModelItemValueProperty, 
                                           valueDesc, mEvent);
    
//  RecordComment("recording UndoValue");
//  B::AEEventBase::SendEvent(mEvent, kAEDontExecute);
}


class UndoClone : public ModelUndoAction
{
public:
    
    // constructor
    UndoClone(
        const ModelItemPtr  inModelItem);
};

UndoClone::UndoClone(
    const ModelItemPtr  inModelItem)
        : ModelUndoAction(
                BLocalizedString(CFSTR("Duplicate"), 
                                 "Undo text for property list item duplication."))
{
    inModelItem->MakeDeleteAppleEvent(mEvent);
    
//  RecordComment("recording UndoClone");
//  B::AEEventBase::SendEvent(mEvent, kAEDontExecute);
}


class UndoCreate : public ModelUndoAction
{
public:
    
    // constructor
    UndoCreate(
        const ModelItemPtr  inModelItem);
};

UndoCreate::UndoCreate(
    const ModelItemPtr  inModelItem)
        : ModelUndoAction(
                BLocalizedString(CFSTR("New Item"), 
                                 "Undo text for property list item creation."))
{
    inModelItem->MakeDeleteAppleEvent(mEvent);
    
//  RecordComment("recording UndoCreate");
//  B::AEEventBase::SendEvent(mEvent, kAEDontExecute);
}


class UndoMove : public ModelUndoAction
{
public:
    
    // constructor
            UndoMove(
                ModelItemPtr    inModelItem);
    virtual ~UndoMove() throw() {}
    
    virtual void    Perform();
    void            Complete(ModelItemPtr inModelItem);
    
private:
    
    B::String       mOriginalName;
    B::AEDescriptor mInsertionLocation;
};

UndoMove::UndoMove(
    ModelItemPtr    inModelItem)
        : ModelUndoAction(
            BLocalizedString(CFSTR("Move"), 
                             "Undo text for property list item move."))
{
    ModelItemPtr    container   = boost::dynamic_pointer_cast<ModelItem>(inModelItem->GetContainer());
    ModelItemPtr    target      = container;
    OSType          position    = kAEEnd;
    
    B_ASSERT(container != NULL);

    if (container->IsArray())
    {
        SInt32  currentIndex    = inModelItem->GetIndex();
        
        if (currentIndex == 0)
        {
            position = kAEBeginning;
        }
        else if (currentIndex < container->CountChildren() - 1)
        {
            position = kAEBefore;
            target = inModelItem;
        }
    }
    else
    {
        mOriginalName = inModelItem->GetName();
    }
    
    // Save the insertion location for later.
    
    B::AEWriter writer;
    
    writer << B::AEObject::InsertionLocationSpecifier(position, target);
    
    writer.Close(mInsertionLocation);
}

void
UndoMove::Complete(ModelItemPtr inModelItem)
{
    B::AEEvent<kAECoreSuite, kAEMove>::Make(
        B::AESelfTarget(),
        inModelItem, 
        mInsertionLocation, 
        mEvent);
    
//  RecordComment("recording UndoMove");
//  B::AEEventBase::SendEvent(mEvent, kAEDontExecute);
}

void
UndoMove::Perform()
{
    B::AEDescriptor result;
    B::AEObjectPtr  movedModel;
    
//  RecordComment("executing UndoMove");
    B::AEEventBase::SendEvent(mEvent, result, kAEDontRecord);
    
    B::DescParam<typeObjectSpecifier>::Get(result, movedModel);
    
    ModelItemPtr    movedItem   = boost::dynamic_pointer_cast<ModelItem>(movedModel);
    
    B_ASSERT(movedItem != NULL);
    
    if (!mOriginalName.empty() && 
        movedItem->IsParentDictionary() && 
        (mOriginalName != movedItem->GetName()))
    {
        movedModel->SendSetPropertyAppleEvent<typeUTF8Text>(
            pName, mOriginalName, kAEDontRecord);
    }
}


class UndoDelete : public ModelUndoAction
{
public:
    
    // constructor
    UndoDelete(
        ConstModelItemPtr   inModelItem);
};

UndoDelete::UndoDelete(
    ConstModelItemPtr   inModelItem)
        : ModelUndoAction(
                BLocalizedString(CFSTR("Delete"), 
                                 "Undo text for property list item deletion."))
{
    // What happens when you Undo a Delete?  Why, a Create of course!
    
    // Retrieve the item's current properties.
    
    DescType        classID = inModelItem->GetClassID();
    B::AEDescriptor propertiesDesc;
    
    inModelItem->SendGetPropertyAppleEvent(pProperties, 
                                           propertiesDesc, typeWildCard, 
                                           kAEDontRecord);
    
    // Determine the insertion location.  It's normally at the end of the 
    // item's container, unless the container is an array and the item is 
    // not already at the end.
    
    B::AEObjectPtr  container   = inModelItem->GetContainer();
    B::AEObjectPtr  targetObj   = container;
    OSType          position    = kAEEnd;
    
    if (inModelItem->IsParentArray())
    {
        SInt32  index   = inModelItem->GetIndex();
        
        if (index == 0)
        {
            position    = kAEBeginning;
        }
        else if (index < container->CountElements(classID))
        {
            position    = kAEAfter;
            targetObj   = container->GetElementByIndex(classID, index - 1);
            
            B_ASSERT(targetObj != NULL);
        }
    }
    
    targetObj->MakeCreateElementAppleEvent(classID, position, targetObj, 
                                           propertiesDesc, NULL, mEvent);
    
//  RecordComment("recording UndoDelete");
//  B::AEEventBase::SendEvent(mEvent, kAEDontExecute);
}


static B::OSPtr<CFDataRef>
ReadIntoBlob(
    B::AEReader&    ioReader)
{
    CFIndex             size    = ioReader.Size();
    B::OSPtr<CFDataRef> dataPtr;
    
    if (size > 0)
    {
        CFAllocatorRef      allocator   = CFAllocatorGetDefault();
        void*               rawData     = NULL;
        
        try
        {
            rawData = CFAllocatorAllocate(allocator, size, 0);
            B_THROW_IF_NULL(rawData);
            
            ioReader.Read(rawData, size);
            
            dataPtr.reset(CFDataCreateWithBytesNoCopy(allocator, 
                                                      (const UInt8 *) rawData, 
                                                      size, 
                                                      allocator), 
                          B::from_copy);
            
            rawData = NULL;
        }
        catch (...)
        {
            if (rawData != NULL)
                CFAllocatorDeallocate(allocator, rawData);
            
            throw;
        }
    }
    else
    {
        dataPtr.reset(CFDataCreate(NULL, NULL, 0), B::from_copy);
    }
    
    return (dataPtr);
}

static LongDateTime ConvertCFDateToLongDateTime(CFDateRef cfdate)
{
    LongDateTime    ldt;
    OSStatus        err;
    
    err = UCConvertCFAbsoluteTimeToLongDateTime(CFDateGetAbsoluteTime(cfdate), &ldt);
    B_THROW_IF_STATUS(err);
    
    return (ldt);
}

static B::OSPtr<CFDateRef>  ConvertLongDateTimeToCFDate(const LongDateTime& ldt)
{
    CFAbsoluteTime  absTime;
    OSStatus        err;
    
    err = UCConvertLongDateTimeToCFAbsoluteTime(ldt, &absTime);
    B_THROW_IF_STATUS(err);
    
    return (B::OSPtr<CFDateRef>(CFDateCreate(NULL, absTime), B::from_copy));
}


static B::OSPtr<CFDateRef>  GetDateTimeNow()
{
    return (B::OSPtr<CFDateRef>(CFDateCreate(NULL, CFAbsoluteTimeGetCurrent()), B::from_copy));
}


struct AppendValue
{
    AppendValue(
        std::vector< B::OSPtr<CFTypeRef> >& valuePtrs, 
        std::vector< CFTypeRef >&           values)
        : mValuePtrs(valuePtrs), mValues(values) {}
    
    void    operator () (const ModelItemPtr inValue) const
            {
                mValuePtrs.push_back(inValue->MakeValueAsPropertyList());
                mValues.push_back(mValuePtrs.back());
            }
    
    std::vector< B::OSPtr<CFTypeRef> >& mValuePtrs;
    std::vector< CFTypeRef >&           mValues;
};

struct AppendKeyAndValue
{
    AppendKeyAndValue(
        std::vector< B::String >&           keyPtrs, 
        std::vector< CFStringRef >&         keys, 
        std::vector< B::OSPtr<CFTypeRef> >& valuePtrs, 
        std::vector< CFTypeRef >&           values)
        : mKeyPtrs(keyPtrs), mKeys(keys), mValuePtrs(valuePtrs), mValues(values) {}
    
    void    operator () (const ModelItem::SubItemMapValue& inValue) const
            {
                mKeyPtrs.push_back(inValue.first);
                mKeys.push_back(mKeyPtrs.back().cf_ref());
                
                mValuePtrs.push_back(inValue.second->MakeValueAsPropertyList());
                mValues.push_back(mValuePtrs.back());
            }
    
    std::vector< B::String >&           mKeyPtrs;
    std::vector< CFStringRef >&         mKeys;
    std::vector< B::OSPtr<CFTypeRef> >& mValuePtrs;
    std::vector< CFTypeRef >&           mValues;
};

struct ModelValueWriter
{
    ModelValueWriter(B::AEWriter& writer)
        : mWriter(writer) {}
    
    void    operator () (const ModelItemPtr inValue) const
            {
                inValue->WriteValue(mWriter);
            }
    
    void    operator () (const ModelItem::SubItemMapValue& inValue) const
            {
                mWriter.Write<typeUTF8Text>(inValue.first);
                inValue.second->WriteValue(mWriter);
            }
    
    B::AEWriter&    mWriter;
};

struct ModelFinder
{
    ModelFinder(ConstModelItemPtr model)
        : mModel(model->GetPtr()) {}
    
    bool    operator () (const ModelItemPtr inValue) const
            {
                return (inValue == mModel);
            }
    
    bool    operator () (const ModelItem::SubItemMapValue& inValue) const
            {
                return (inValue.second == mModel);
            }
    
    ConstModelItemPtr   mModel;
};

struct ModelFinderByName
{
    ModelFinderByName(const B::String& inName)
        : mName(inName) {}
    
    bool    operator () (const ModelItemPtr inValue) const
            {
                return (mName == inValue->GetName());
            }
    
    B::String   mName;
};

}   // anonymous namespace


    

// ==========================================================================================
//  ModelItem::ItemValue<>

#pragma mark -
#pragma mark ModelItem::ItemValue<>

// ------------------------------------------------------------------------------------------
/*! Convert a CF property list item into our internal representation for 
    that item's type.
*/
void
ModelItem::ItemValue<ModelItem::class_String>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ValueType&          outValue)
{
    outValue = CFUMakeTypedValue<ItemType>(inItem);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Data>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ValueType&          outValue)
{
    outValue = CFUMakeTypedValue<ItemType>(inItem);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Integer>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ValueType&          outValue)
{
    B::OSPtr<ItemType>  itemPtr(CFUMakeTypedValue<ItemType>(inItem));
    bool                good;
    
    good = CFNumberGetValue(itemPtr, kCFNumberSInt64Type, &outValue);
    B_ASSERT(good);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Real>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ValueType&          outValue)
{
    B::OSPtr<ItemType>  itemPtr(CFUMakeTypedValue<ItemType>(inItem));
    bool                good;
    
    good = CFNumberGetValue(itemPtr, kCFNumberDoubleType, &outValue);
    B_ASSERT(good);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Boolean>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ValueType&          outValue)
{
    B::OSPtr<ItemType>  itemPtr(CFUMakeTypedValue<ItemType>(inItem));
    
    outValue = CFBooleanGetValue(itemPtr);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Date>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ValueType&          outValue)
{
    outValue = CFUMakeTypedValue<ItemType>(inItem);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Array>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ModelItemPtr        inParent)
{
    B_ASSERT(inParent != NULL);
    
    B::OSPtr<ItemType>  itemPtr(CFUMakeTypedValue<ItemType>(inItem));
    CFIndex             count   = CFArrayGetCount(itemPtr);
    
    for (CFIndex i = 0; i < count; i++)
    {
        inParent->MakeNewArraySubItem(B::OSPtr<CFTypeRef>(CFArrayGetValueAtIndex(itemPtr, i)), 
                                      kAEEnd, ModelItemPtr());
    }
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ItemValue<ModelItem::class_Dictionary>::ItemToValue(
    B::OSPtr<CFTypeRef> inItem, 
    ModelItemPtr        inParent)
{
    B_ASSERT(inParent != NULL);
    
    B::OSPtr<ItemType>          itemPtr(CFUMakeTypedValue<ItemType>(inItem));
    CFIndex                     count   = CFDictionaryGetCount(itemPtr);
    std::vector<CFStringRef>    keys(count);
    std::vector<CFTypeRef>      values(count);
    
    CFDictionaryGetKeysAndValues(itemPtr, 
                                 reinterpret_cast<const void **>(&keys[0]),
                                 reinterpret_cast<const void **>(&values[0]));
    
    for (CFIndex i = 0; i < count; i++)
    {
        inParent->MakeNewDictionarySubItem(B::OSPtr<CFTypeRef>(values[i]), 
                                           B::String(keys[i]), 
                                           kAEEnd, ModelItemPtr());
    }
}

// ------------------------------------------------------------------------------------------
/*! Converts our internal representation into the corresponding CF property 
    list item.
*/
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_String>::ValueToItem(
    const ValueType& inValue)
{
    return (B::OSPtr<CFTypeRef>(inValue.cf_ref()));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Data>::ValueToItem(
    const ValueType& inValue)
{
    return (B::OSPtr<CFTypeRef>(inValue.get()));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Integer>::ValueToItem(
    const ValueType& inValue)
{
    B::OSPtr<CFTypeRef> itemPtr;
    
    if ((inValue >= INT_MIN) && (inValue <= INT_MAX))
    {
        SInt32  intval  = inValue;
        
        itemPtr.reset(CFNumberCreate(NULL, kCFNumberSInt32Type, &intval), B::from_copy);
    }
    else
    {
        itemPtr.reset(CFNumberCreate(NULL, kCFNumberSInt64Type, &inValue), B::from_copy);
    }

    return (itemPtr);
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Real>::ValueToItem(
    const ValueType& inValue)
{
    return (B::OSPtr<CFTypeRef>(CFNumberCreate(NULL, kCFNumberDoubleType, &inValue), B::from_copy));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Boolean>::ValueToItem(
    const ValueType& inValue)
{
    return (B::OSPtr<CFTypeRef>(inValue ? kCFBooleanTrue : kCFBooleanFalse));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Date>::ValueToItem(
    const ValueType& inValue)
{
    return (B::OSPtr<CFTypeRef>(inValue));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Array>::ValueToItem(
    const ValueType& inValue)
{
    size_t                              count   = inValue.size();
    std::vector< B::OSPtr<CFTypeRef> >  valuePtrs;
    std::vector< CFTypeRef >            values;
    
    valuePtrs.reserve(count);
    values.reserve(count);
    
    std::for_each(inValue.begin(), inValue.end(), 
                  AppendValue(valuePtrs, values));
    
    return (B::OSPtr<CFTypeRef>(CFArrayCreate(NULL, &values[0], values.size(), 
                                     &kCFTypeArrayCallBacks), 
                              B::from_copy));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Dictionary>::ValueToItem(
    const ValueType& inValue)
{
    size_t                              count   = inValue.size();
    std::vector< B::String >            keyPtrs;
    std::vector< B::OSPtr<CFTypeRef> >  valuePtrs;
    std::vector< CFStringRef >          keys;
    std::vector< CFTypeRef >            values;
    
    keyPtrs.reserve(count);
    valuePtrs.reserve(count);
    keys.reserve(count);
    values.reserve(count);
    
    std::for_each(inValue.begin(), inValue.end(), 
                  AppendKeyAndValue(keyPtrs, keys, valuePtrs, values));
    
    return (B::OSPtr<CFTypeRef>(CFDictionaryCreate(NULL, 
                                          reinterpret_cast<const void **>(&keys[0]), 
                                          &values[0], values.size(), 
                                          &kCFCopyStringDictionaryKeyCallBacks, 
                                          &kCFTypeDictionaryValueCallBacks), 
                              B::from_copy));
}

// ------------------------------------------------------------------------------------------
/*! Converts an Apple %Event stream into the corresponding CF property list 
    item.
*/
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_String>::ReadItem(
    B::AEReader&    ioReader)
{
    B::String   itemPtr;
    
    ioReader.Read<kAEType>(itemPtr);
    
    return (B::OSPtr<CFTypeRef>(itemPtr.cf_ref()));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Data>::ReadItem(
    B::AEReader&    ioReader)
{
    return (B::OSPtr<CFTypeRef>(ReadIntoBlob(ioReader).get()));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Integer>::ReadItem(
    B::AEReader&    ioReader)
{
    ValueType   tempval;
    
    ioReader.Read<kAEType>(tempval);
    
    return (ValueToItem(tempval));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Real>::ReadItem(
    B::AEReader&    ioReader)
{
    ValueType   tempval;
    
    ioReader.Read<kAEType>(tempval);
    
    return (ValueToItem(tempval));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Boolean>::ReadItem(
    B::AEReader&    ioReader)
{
    ValueType   tempval;
    
    ioReader.Read<kAEType>(tempval);
    
    return (ValueToItem(tempval));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Date>::ReadItem(
    B::AEReader&    ioReader)
{
    LongDateTime    ldt;
    
    ioReader.Read<kAEType>(ldt);
    
    return (ValueToItem(ConvertLongDateTimeToCFDate(ldt)));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Array>::ReadItem(
    B::AEReader&    ioReader)
{
    size_t                              count   = ioReader.Count();
    std::vector< B::OSPtr<CFTypeRef> >  valuePtrs;
    std::vector< CFTypeRef >            values;
    
    valuePtrs.reserve(count);
    values.reserve(count);
    
    for (size_t i = 1; i <= count; i++)
    {
        B::AutoAEReaderDescNth  autoNth(ioReader, i);
        
        valuePtrs.push_back(ModelItem::ReadItem(ioReader));
        values.push_back(valuePtrs.back());
    }
    
    return (B::OSPtr<CFTypeRef>(CFArrayCreate(NULL, &values[0], values.size(), 
                                     &kCFTypeArrayCallBacks), 
                              B::from_copy));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ItemValue<ModelItem::class_Dictionary>::ReadItem(
    B::AEReader&    ioReader)
{
    B::OSPtr<CFTypeRef> itemPtr;
    
    if (ioReader.Contains(keyASUserRecordFields))
    {
        B::AutoAEReaderDescKey  autoKey(ioReader, keyASUserRecordFields, typeAEList);
        
        size_t                              count   = ioReader.Count();
        std::vector< B::String >            keyPtrs;
        std::vector< B::OSPtr<CFTypeRef> >  valuePtrs;
        std::vector< CFStringRef >          keys;
        std::vector< CFTypeRef >            values;
        
        keyPtrs.reserve(count);
        valuePtrs.reserve(count);
        keys.reserve(count);
        values.reserve(count);
        
        for (size_t i = 1; i <= count; i += 2)
        {
            B::AutoAEReaderDescNth  autoNth(ioReader, i);
            B::String               name;
            
            ioReader.Read<typeUTF8Text>(name);
            
            keyPtrs.push_back(name);
            keys.push_back(keyPtrs.back().cf_ref());
        }
        
        for (size_t i = 2; i <= count; i += 2)
        {
            B::AutoAEReaderDescNth  autoNth(ioReader, i);
            
            valuePtrs.push_back(ModelItem::ReadItem(ioReader));
            values.push_back(valuePtrs.back());
        }
        
        B_ASSERT(keys.size() == values.size());
        
        itemPtr.reset(CFDictionaryCreate(NULL, 
                                         reinterpret_cast<const void **>(&keys[0]), 
                                         &values[0], values.size(), 
                                         &kCFCopyStringDictionaryKeyCallBacks, 
                                         &kCFTypeDictionaryValueCallBacks), 
                      B::from_copy);
    }
    else
    {
        itemPtr.reset(CFDictionaryCreate(NULL, NULL, NULL, 0, 
                                         &kCFCopyStringDictionaryKeyCallBacks, 
                                         &kCFTypeDictionaryValueCallBacks), 
                      B::from_copy);
    }
    
    return (itemPtr);
}


// ==========================================================================================
//  ModelItem::Init

#pragma mark -

class ModelItem::Init
{
public:
    
    Init();
    ~Init();
    
    ModelItem::EClass   GetClass(B::OSPtr<CFTypeRef> inValue) const;
    
private:
    
    // types
    typedef std::map<CFTypeID, ModelItem::EClass>   ClassMap;
    typedef ClassMap::value_type                    ClassMapValue;
    
    template <typename T> void  AddClass(ModelItem::EClass inClass)
    {
        mClassMap.insert(ClassMapValue(CFUGetType<T>(), inClass));
    }
    
    // member variables
    ClassMap    mClassMap;
    
    // static member variables
    static B::AutoAECoerceDescUPP   sCoercionUPP;
};


// static member variables
B::AutoAECoerceDescUPP  ModelItem::Init::sCoercionUPP(ModelItem::CoerceToPropertyList);


// ------------------------------------------------------------------------------------------
ModelItem::Init::Init()
{
    OSStatus    err;
    
    AddClass<CFStringRef>(ModelItem::class_String);
    AddClass<CFDataRef>(ModelItem::class_Data);
    AddClass<CFNumberRef>(ModelItem::class_Integer);
    AddClass<CFBooleanRef>(ModelItem::class_Boolean);
    AddClass<CFDateRef>(ModelItem::class_Date);
    AddClass<CFArrayRef>(ModelItem::class_Array);
    AddClass<CFDictionaryRef>(ModelItem::class_Dictionary);
    
    err = AEInstallCoercionHandler(typeWildCard, 'plst', 
                                   sCoercionUPP, 
                                   0, true, false);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
ModelItem::Init::~Init()
{
    OSStatus    err;
    
    err = AERemoveCoercionHandler(typeWildCard, 'plst', sCoercionUPP, false);
}

// ------------------------------------------------------------------------------------------
ModelItem::EClass
ModelItem::Init::GetClass(B::OSPtr<CFTypeRef> inValue) const
{
    ClassMap::const_iterator    it(mClassMap.find(CFGetTypeID(inValue)));
    
    B_ASSERT(it != mClassMap.end());
    
    ModelItem::EClass   theClass    = it->second;
    
    if (theClass == ModelItem::class_Integer)
    {
        // Special case:  CFNumberRefs can contain integers or floating-
        // point values.
        
        B::OSPtr<CFNumberRef>   numPtr  = CFUMakeTypedValue<CFNumberRef>(inValue);
        
        switch (CFNumberGetType(numPtr))
        {
        case kCFNumberFloat32Type:
        case kCFNumberFloat64Type:
        case kCFNumberFloatType:
        case kCFNumberDoubleType:
            theClass = ModelItem::class_Real;
            break;
            
        default:
            break;
        }
    }
    
    return (theClass);
}


// ==========================================================================================
//  ModelItem

#pragma mark -

namespace
{
    bool gRegisteredDuplicateItemException      = B::ExceptionStreamer::Register<DuplicateItemException>();
    bool gRegisteredMoveIntoOffspringException  = B::ExceptionStreamer::Register<MoveIntoOffspringException>();
}

// static member variables
ModelItem::Init ModelItem::sInit;


// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::Create(
    B::AEObjectPtr      inContainer, 
    B::OSPtr<CFTypeRef> inValue)
{
    ModelItemPtr item(new ModelItem(inContainer));
    
    item->SetModelItemValue(inValue);
    
    return item;
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::Clone(
    const ModelItem&    inModel)
{
    ModelItemPtr item(new ModelItem(inModel));
    
    switch (item->mClass)
    {
    case class_String:
        item->mValueString = inModel.mValueString;
        break;
        
    case class_Data:
        item->mValueData = inModel.mValueData;
        break;
        
    case class_Integer:
        item->mValueInteger = inModel.mValueInteger;
        break;
        
    case class_Real:
        item->mValueReal = inModel.mValueReal;
        break;
        
    case class_Boolean:
        item->mValueBoolean = inModel.mValueBoolean;
        break;
        
    case class_Date:
        item->mValueDate = inModel.mValueDate;
        break;
        
    case class_Array:
        item->mValueArray.reserve(inModel.mValueArray.size());
        for (SubItemArrayConstIter ait = inModel.mValueArray.begin(); 
             ait != inModel.mValueArray.end(); 
             ++ait)
        {
            item->CopyArraySubItem(*ait, kAEEnd, item);
        }
        break;
        
    case class_Dictionary:
        for (SubItemMapConstIter dit = inModel.mValueDictionary.begin(); 
             dit != inModel.mValueDictionary.end(); 
             ++dit)
        {
            item->CopyDictionarySubItem(dit->second, kAEEnd, item);
        }
        break;
    }
    
    return item;
}

// ------------------------------------------------------------------------------------------
ModelItem::ModelItem(
    B::AEObjectPtr      inContainer)
        : B::AEObject(inContainer, kModelItemKind), 
          mClass(class_String)
{
    B_ASSERT(inContainer != NULL);
}

// ------------------------------------------------------------------------------------------
ModelItem::ModelItem(
    const ModelItem&    inModel)
        : B::AEObject(inModel.GetContainer(), inModel.GetClassID()), 
          mClass(inModel.mClass)
{
}

// ------------------------------------------------------------------------------------------
void
ModelItem::SetModelItemValue(
    B::OSPtr<CFTypeRef> inValue)
{
    EClass  oldClass    = mClass;
    EClass  newClass    = sInit.GetClass(inValue);
    
    if (oldClass != newClass)
    {
        mClassChangingSignal();
        mClass = newClass;
    }
    
    mValueDictionary.clear();
    mValueArray.clear();
    mValueString.clear();
    mValueData.reset();
    
    switch (mClass)
    {
    case class_String:
        ItemValue<class_String>::ItemToValue(inValue, mValueString);
        break;
        
    case class_Data:
        ItemValue<class_Data>::ItemToValue(inValue, mValueData);
        break;
        
    case class_Integer:
        ItemValue<class_Integer>::ItemToValue(inValue, mValueInteger);
        break;
        
    case class_Real:
        ItemValue<class_Real>::ItemToValue(inValue, mValueReal);
        break;
        
    case class_Boolean:
        ItemValue<class_Boolean>::ItemToValue(inValue, mValueBoolean);
        break;
        
    case class_Date:
        ItemValue<class_Date>::ItemToValue(inValue, mValueDate);
        break;
        
    case class_Array:
        ItemValue<class_Array>::ItemToValue(inValue, GetPtr());
        break;
        
    case class_Dictionary:
        ItemValue<class_Dictionary>::ItemToValue(inValue, GetPtr());
        break;
    }
    
    if (oldClass != newClass)
        mClassChangedSignal();
    
    mValueChangedSignal();
}

// ------------------------------------------------------------------------------------------
/*! Erases the given sub item from the object, which must be a container.
    The sub item's container field is set to @c NULL prior to erasure.
    Note that if the object contains the last reference to the sub item, 
    then it will be deleted.
*/
void
ModelItem::RemoveSubItem(ModelItemPtr   inSubItem)
{
    B_ASSERT(IsContainer());
    
    if (IsDictionary())
    {
        SubItemMapIter  mit;
        
        mit = std::find_if(mValueDictionary.begin(), 
                           mValueDictionary.end(), 
                           ModelFinder(inSubItem));
        
        B_ASSERT(mit != mValueDictionary.end());
        
        mit->second->SetContainer(B::AEObjectPtr());
        mValueDictionary.erase(mit);
    }
    else if (IsArray())
    {
        SubItemArrayIter    it;
        
        it = std::find_if(mValueArray.begin(), 
                          mValueArray.end(), 
                          ModelFinder(inSubItem));
        
        B_ASSERT(it != mValueArray.end());
        
        (*it)->SetContainer(B::AEObjectPtr());
        mValueArray.erase(it);
    }
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::MakeNewDictionarySubItem(
    B::OSPtr<CFTypeRef> inValue,
    const B::String&    inKey, 
    DescType            inPosition, 
    ModelItemPtr        inTargetItem)
{
    ModelItemPtr    modelItemPtr(Create(GetPtr(), inValue));
    
    InsertDictionarySubItem(modelItemPtr, inPosition, inTargetItem, inKey);
    
    return (modelItemPtr);
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::MakeNewArraySubItem(
    B::OSPtr<CFTypeRef> inValue,
    DescType            inPosition, 
    ModelItemPtr        inTargetItem)
{
    ModelItemPtr    modelItemPtr(Create(GetPtr(), inValue));
    
    InsertArraySubItem(modelItemPtr, inPosition, inTargetItem);
    
    return (modelItemPtr);
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::CopyDictionarySubItem(
    ModelItemPtr    inOriginal, 
    DescType        inPosition, 
    ModelItemPtr    inTargetItem)
{
    B_ASSERT(inOriginal != NULL);
    
    B::String   key = inOriginal->GetName();
    
    if (key.empty())
    {
        key = MakeUniqueDictionaryKeyForNewItem();
    }
    else if (mValueDictionary.find(key) != mValueDictionary.end())
    {
        key = MakeUniqueDictionaryKeyforCopy(key);
    }
    
    ModelItemPtr    modelItemPtr(Clone(*inOriginal));
    
    InsertDictionarySubItem(modelItemPtr, inPosition, inTargetItem, key);
    
    return (modelItemPtr);
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::CopyArraySubItem(
    ModelItemPtr    inOriginal, 
    DescType        inPosition, 
    ModelItemPtr    inTargetItem)
{
    B_ASSERT(inOriginal != NULL);
    
    ModelItemPtr    modelItemPtr(Clone(*inOriginal));
    
    InsertArraySubItem(modelItemPtr, inPosition, inTargetItem);
    
    return (modelItemPtr);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::InsertDictionarySubItem(
    ModelItemPtr        inSubItem, 
    DescType            inPosition, 
    ModelItemPtr        inTargetItem, 
    const B::String&    inSubItemKey)
{
    B_ASSERT(inSubItem != NULL);
    B_ASSERT((inTargetItem == NULL) || 
           (inTargetItem.get() == this) || 
           (inTargetItem->GetContainer().get() == this));
    
    B::String   itemKey = inSubItemKey;
    
    if (itemKey.empty())
    {
        itemKey = MakeUniqueDictionaryKeyForNewItem();
    }
    
    if (inPosition == kAEReplace)
    {
        B_ASSERT((inTargetItem != NULL) && (inTargetItem != inSubItem));
        
        DeleteReplacedSubItem(inTargetItem, GetPtr());
    }
    
    if (mValueDictionary.find(itemKey) != mValueDictionary.end())
    {
        B_THROW(DuplicateItemException());
    }
    
    inSubItem->SetContainer(GetPtr());
    mValueDictionary.insert(SubItemMapValue(itemKey, inSubItem->GetPtr()));
}

// ------------------------------------------------------------------------------------------
void
ModelItem::InsertArraySubItem(
    ModelItemPtr    inSubItem, 
    DescType        inPosition, 
    ModelItemPtr    inTargetItem)
{
    B_ASSERT(inSubItem != NULL);
    B_ASSERT((inTargetItem == NULL) || 
           (inTargetItem.get() == this) || 
           (inTargetItem->GetContainer().get() == this));
    
    ModelItemPtr    subItemPtr(inSubItem->GetPtr());
    unsigned        targetIndex;
    
    inSubItem->SetContainer(GetPtr());
    
    switch (inPosition)
    {
    case kAEBefore:
        B_ASSERT(inTargetItem != NULL);
        targetIndex = inTargetItem->GetIndex();
        B_ASSERT((targetIndex >= 0) && (targetIndex < mValueArray.size()));
        mValueArray.insert(mValueArray.begin() + targetIndex, subItemPtr);
        break;
        
    case kAEAfter:
        B_ASSERT(inTargetItem != NULL);
        targetIndex = inTargetItem->GetIndex();
        B_ASSERT((targetIndex >= 0) && (targetIndex < mValueArray.size()));
        if (++targetIndex >= mValueArray.size())
            mValueArray.push_back(subItemPtr);
        else
            mValueArray.insert(mValueArray.begin() + targetIndex, subItemPtr);
        break;
        
    case kAEReplace:
        B_ASSERT(inTargetItem != NULL);
        B_ASSERT(inTargetItem != inSubItem);
        targetIndex = inTargetItem->GetIndex();
        B_ASSERT((targetIndex >= 0) && (targetIndex < mValueArray.size()));
        DeleteReplacedSubItem(inTargetItem, GetPtr());
        mValueArray.insert(mValueArray.begin() + targetIndex, subItemPtr);
        break;
    
    case kAEBeginning:
        mValueArray.insert(mValueArray.begin(), subItemPtr);
        break;
    
    case kAEEnd:
        mValueArray.push_back(subItemPtr);
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
ModelItem::DeleteReplacedSubItem(
    ModelItemPtr    inModelItem, 
    ModelItemPtr    inContainer)
{
    B_ASSERT(inModelItem != NULL);
    B_ASSERT(inContainer != NULL);
    
    ModelItemPtr                itemPtr(inModelItem->GetPtr()); // prevent premature deletion.
    std::auto_ptr<UndoDelete>   undoAction(new UndoDelete(inModelItem));
    B::AutoUndo                 autoUndo(undoAction->GetActionName(), GetUndoTarget());
    
    inContainer->RemoveSubItem(inModelItem);
    
    autoUndo.Add(undoAction);
    autoUndo.Commit();
    
    mDeletedSignal();
}

// ------------------------------------------------------------------------------------------
bool
ModelItem::IsParentDictionary() const
{
    ModelItemPtr    container   = boost::dynamic_pointer_cast<ModelItem>(GetContainer());
    
    return ((container != NULL) && container->IsDictionary());
}

// ------------------------------------------------------------------------------------------
bool
ModelItem::IsParentArray() const
{
    ModelItemPtr    container   = boost::dynamic_pointer_cast<ModelItem>(GetContainer());
    
    return ((container != NULL) && container->IsArray());
}

// ------------------------------------------------------------------------------------------
bool
ModelItem::IsRoot() const
{
    return (dynamic_cast<B::AbstractDocument*>(GetContainer().get()) != NULL);
}

// ------------------------------------------------------------------------------------------
B::AbstractDocument*
ModelItem::GetDocument() const
{
    B::AEObjectPtr          container   = GetContainer();
    B::AbstractDocument*    document;
    
    B_ASSERT(container != NULL);
    
    if ((document = dynamic_cast<B::AbstractDocument*>(container.get())) != NULL)
    {
        return (document);
    }
    else
    {
        ModelItemPtr    containerItem   = boost::dynamic_pointer_cast<ModelItem>(container);
        
        B_ASSERT(containerItem != NULL);
        
        return (containerItem->GetDocument());
    }
}

// ------------------------------------------------------------------------------------------
bool
ModelItem::IsInSameContainerAs(const ModelItem& inItem) const
{
    return (GetDocument() == inItem.GetDocument());
}

// ------------------------------------------------------------------------------------------
EventTargetRef
ModelItem::GetUndoTarget() const
{
    return (GetDocument()->GetEventTarget());
}

// ------------------------------------------------------------------------------------------
unsigned
ModelItem::GetIndex() const
{
    unsigned            index   = 0;
    ConstModelItemPtr   container;
    
    if (IsParentDictionary())
    {
        container = boost::dynamic_pointer_cast<const ModelItem>(GetContainer());
        B_ASSERT(container != NULL);
        
        SubItemMapConstIter mit;
        
        for (mit = container->mValueDictionary.begin(); 
             mit != container->mValueDictionary.end(); 
             ++mit, ++index)
        {
            if (mit->second.get() == this)
                break;
        }
        
        B_ASSERT(mit != container->mValueDictionary.end());
    }
    else if (IsParentArray())
    {
        container = boost::dynamic_pointer_cast<const ModelItem>(GetContainer());
        B_ASSERT(container != NULL);
        
        SubItemArrayConstIter   ait;
        
        for (ait = container->mValueArray.begin(); 
             ait != container->mValueArray.end(); 
             ++ait, ++index)
        {
            if (ait->get() == this)
                break;
        }

        B_ASSERT(ait != container->mValueArray.end());
    }
    else
    {
        B_THROW(B::AEBadKeyFormException());
    }
    
    return (index);
}

// ------------------------------------------------------------------------------------------
B::String
ModelItem::GetName() const
{
    B::String           name;
    ConstModelItemPtr   container;
    
    if (IsParentDictionary())
    {
        container = boost::dynamic_pointer_cast<const ModelItem>(GetContainer());
        B_ASSERT(container != NULL);
        
        SubItemMapConstIter mit;
        
        mit = std::find_if(container->mValueDictionary.begin(), 
                           container->mValueDictionary.end(), 
                           ModelFinder(GetPtr()));
        
        B_ASSERT(mit != container->mValueDictionary.end());
        
        name = mit->first;
    }
    else if (IsParentArray())
    {
        name = B::Extract(B::StringFormatter("%1%") % (GetIndex()+1));
    }
    else
    {
        name = CFUSTR("root");
    }
    
    return (name);
}

// ------------------------------------------------------------------------------------------
SInt32
ModelItem::CountChildren() const
{
    SInt32  count   = 0;
    
    if (IsDictionary())
    {
        count = mValueDictionary.size();
    }
    else if (IsArray())
    {
        count = mValueArray.size();
    }
    
    return (count);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::GetChildren(
    std::vector<ModelItemPtr>&  items) const
{
    B_ASSERT(IsContainer());
    
    if (IsDictionary())
    {
        items.clear();
        items.reserve(mValueDictionary.size());
        
        std::copy(B::MakeValueAdapter(mValueDictionary.begin()), 
                  B::MakeValueAdapter(mValueDictionary.end()), 
                  std::back_inserter(items));
    }
    else if (IsArray())
    {
        items.clear();
        items.reserve(mValueArray.size());
        
        std::copy(mValueArray.begin(), mValueArray.end(), std::back_inserter(items));
    }
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::GetPtr()
{
    return boost::static_pointer_cast<ModelItem>(shared_from_this());
}

// ------------------------------------------------------------------------------------------
ConstModelItemPtr
ModelItem::GetPtr() const
{
    return boost::static_pointer_cast<ModelItem const>(shared_from_this());
}

// ------------------------------------------------------------------------------------------
B::String
ModelItem::MakeUniqueDictionaryKeyForNewItem() const
{
    int                 count   = 1;
    B::String           name(BLocalizedString(
                            CFSTR("New item"), 
                            "The default name of a new property list item"));
    B::StringFormatter  formatter(BLocalizedString(
                            CFSTR("New item %1%"), 
                            "The default name of a new property list item when there are already items with the default name.  %1% holds a number that makes the name unique."));
    
    while (mValueDictionary.find(name) != mValueDictionary.end())
    {
        name = B::Extract(formatter % ++count);
    }
    
    return (name);
}

// ------------------------------------------------------------------------------------------
B::String
ModelItem::MakeUniqueDictionaryKeyforCopy(
    const B::String&    inRootName) const
{
    int                 count   = 0;
    B::String           name(B::Extract(B::StringFormatter(BLocalizedString(
                            CFSTR("%1% copy"), 
                            "The default name of a duplicate item.  %1% holds the name of the original item (i.e., the source of the duplicate)."))
                            % inRootName));
    B::StringFormatter  formatter(BLocalizedString(
                            CFSTR("%1% copy %2%"), 
                            "The default name of a duplicate item when there are already items with the default name.  %1% holds the name of the original item (i.e., the source of the duplicate).  %2% holds a number that makes the name unique."));
    
    while (mValueDictionary.find(name) != mValueDictionary.end())
    {
        name = B::Extract(formatter % inRootName % ++count);
    }
    
    return (name);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::Select()
{
    mSelectSignal();
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::MakeValueAsPropertyList() const
{
    B::OSPtr<CFTypeRef> itemPtr;
    
    switch (mClass)
    {
    case class_String:
        itemPtr = ItemValue<class_String>::ValueToItem(mValueString);
        break;
        
    case class_Data:
        itemPtr = ItemValue<class_Data>::ValueToItem(mValueData);
        break;
        
    case class_Integer:
        itemPtr = ItemValue<class_Integer>::ValueToItem(mValueInteger);
        break;
        
    case class_Real:
        itemPtr = ItemValue<class_Real>::ValueToItem(mValueReal);
        break;
        
    case class_Boolean:
        itemPtr = ItemValue<class_Boolean>::ValueToItem(mValueBoolean);
        break;
        
    case class_Date:
        itemPtr = ItemValue<class_Date>::ValueToItem(mValueDate);
        break;
        
    case class_Array:
        itemPtr = ItemValue<class_Array>::ValueToItem(mValueArray);
        break;
        
    case class_Dictionary:
        itemPtr = ItemValue<class_Dictionary>::ValueToItem(mValueDictionary);
        break;
    }
    
    return (itemPtr);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::WriteItemPointerFlavor(std::ostream& ostr) const
{
    void* ptr = reinterpret_cast<void*>(const_cast<ModelItem*>(this));
    
    ostr << ptr << std::endl;
}

// ------------------------------------------------------------------------------------------
ModelItemPtr
ModelItem::ReadItemPointerFlavor(std::istream& istr)
{
    ModelItem*  modelItem   = NULL;
    void*       ptr;
    
    istr.clear();
    
    istr >> ptr;
    
    modelItem = istr.good() ? reinterpret_cast<ModelItem*>(ptr) : NULL;
    
    return ((modelItem != NULL) ? modelItem->GetPtr() : ModelItemPtr());
}

// ------------------------------------------------------------------------------------------
void
ModelItem::WriteInternalFlavor(std::ostream& ostr) const
{
    B::AEWriter         writer;
    std::vector<UInt8>  buffer;
    
    {
        B::AutoAEWriterRecord   autoRec(writer, 'MItm');
        
        if (IsParentDictionary())
            writer << B::AEKey(pName) << GetName();
        
        writer << B::AEKey(pModelItemValueProperty);
        WriteValue(writer);
    }
    
    writer.Close(buffer);
    
    ostr.write(reinterpret_cast<char*>(&buffer[0]), buffer.size());
}

// ------------------------------------------------------------------------------------------
void
ModelItem::WritePropertyListFlavor(std::ostream& ostr) const
{
    B::OSPtr<CFTypeRef>     valuePtr    = MakeValueAsPropertyList();
    B::OSPtr<CFStringRef>   namePtr;
    
    if (IsParentDictionary())
    {
        CFStringRef                 keys[1]     = { GetName().cf_ref() };
        CFTypeRef                   values[1]   = { valuePtr };
        B::OSPtr<CFDictionaryRef>   dictPtr(CFDictionaryCreate(
                                                NULL, 
                                                reinterpret_cast<const void **>(keys), 
                                                values, 1, 
                                                &kCFCopyStringDictionaryKeyCallBacks, 
                                                &kCFTypeDictionaryValueCallBacks), 
                                            B::from_copy);
        
        valuePtr.reset(dictPtr.get());
    }
    
    B::OSPtr<CFDataRef> xmlData(CFPropertyListCreateXMLData(NULL, valuePtr), B::from_copy);
    
    ostr.write(reinterpret_cast<const char*>(CFDataGetBytePtr(xmlData)), 
               CFDataGetLength(xmlData));
}


#if 0
// ------------------------------------------------------------------------------------------
void
ModelItem::WriteToStream(std::ostream& ostr)
{
    B::OSPtr<CFStringRef>   namePtr;
    B::OSPtr<CFTypeRef>     valuePtr;
    const void*             keys[2];
    const void*             values[2];
    
    if (IsParentDictionary())
        namePtr = mDictionaryKey;
    else
        namePtr.Adopt(CFSTR(""), false);
    
    valuePtr    = MakePropertyList();
    keys[0]     = CFSTR("__PLister_Name");
    keys[1]     = CFSTR("__PLister_Value");
    values[0]   = namePtr.Get();
    values[1]   = valuePtr.Get();
    
    B::OSPtr<CFDictionaryRef>   dict(CFDictionaryCreate(NULL, keys, values, 2, 
                                                    &kCFCopyStringDictionaryKeyCallBacks, 
                                                    &kCFTypeDictionaryValueCallBacks), 
                                     true);
    B::OSPtr<CFDataRef>         xmlData(CFPropertyListCreateXMLData(NULL, dict), true);
    
    ostr.write(reinterpret_cast<const char*>(CFDataGetBytePtr(xmlData)), 
               CFDataGetLength(xmlData));
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ReadFromStream(std::istream& istr, B::OSPtr<CFStringRef>& outName)
{
    std::ios_base::iostate  savedExceptions = istr.exceptions();
    
    istr.clear();
    istr.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
    
    std::istream::pos_type  startPos(0), endPos(0);
    
    startPos = istr.tellg();
    istr.seekg(0, std::ios_base::end);
    
    endPos = istr.tellg();
    istr.seekg(startPos);
    
    size_t  size    = endPos - startPos;
    
    CFUMutableDataPtr   dataPtr(CFDataCreateMutable(NULL, size), true);
    
    istr.read((char *) CFDataGetMutableBytePtr(dataPtr), size);
    
    B::OSPtr<CFTypeRef> typePtr(CFPropertyListCreateFromXMLData(NULL, 
                                                        dataPtr, 
                                                        kCFPropertyListImmutable, 
                                                        NULL), 
                                true);
    
    B::OSPtr<CFDictionaryRef>   dictPtr(CFUMakeTypedValue<CFDictionaryRef>(typePtr));
    B::OSPtr<CFTypeRef>         valuePtr;
    
    CFUGet(dictPtr, CFSTR("__PLister_Name"), outName);
    CFUGet(dictPtr, CFSTR("__PLister_Value"), valuePtr);
    
    istr.exceptions(savedExceptions);
    
    return (valuePtr);
}
#endif

// ------------------------------------------------------------------------------------------
size_t
ModelItem::CountElements(
    DescType        inElementType) const
{
    size_t  count;
    
    switch (inElementType)
    {
    case kModelItemKind:
        count = CountChildren();
        break;
        
    default:
        count = B::AEObject::CountElements(inElementType);
        break;
    }

    return (count);
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
ModelItem::GetElementByIndex(
    DescType        inElementType, 
    size_t          inIndex) const
{
    B::AEObjectPtr  element;
    
    switch (inElementType)
    {
    case kModelItemKind:
        if (IsArray())
        {
            if ((inIndex < 0) || (inIndex >= mValueArray.size()))
                B_THROW(B::AENoSuchObjectException());
            
            element = mValueArray[inIndex];
        }
        else if (IsDictionary())
        {
            if ((inIndex < 0) || (inIndex >= mValueDictionary.size()))
                B_THROW(B::AENoSuchObjectException());
            
            SubItemMapConstIter it  = mValueDictionary.begin();
            
            while (inIndex-- > 0)
                ++it;
            
            element = it->second;
        }
        else
        {
            B_THROW(B::AEBadKeyFormException());
        }
        break;
        
    default:
        element = B::AEObject::GetElementByIndex(inElementType, inIndex);
        break;
    }
    
    return (element);
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
ModelItem::GetElementByName(
    DescType            inElementType, 
    const B::String&    inName) const
{
    B::AEObjectPtr  element;
    
    switch (inElementType)
    {
    case kModelItemKind:
        if (IsArray())
        {
            SubItemArrayConstIter   ait(std::find_if(mValueArray.begin(), 
                                                     mValueArray.end(), 
                                                     ModelFinderByName(inName)));
            
            if (ait == mValueArray.end())
                B_THROW(B::AENoSuchObjectException());
            
            element = *ait;
        }
        else if (IsDictionary())
        {
            SubItemMapConstIter mit(mValueDictionary.find(inName));
            
            if (mit == mValueDictionary.end())
                B_THROW(B::AENoSuchObjectException());
            
            element = mit->second;
        }
        else
        {
            B_THROW(B::AEBadKeyFormException());
        }
        break;
        
    default:
        element = B::AEObject::GetElementByName(inElementType, inName);
        break;
    }
    
    return (element);
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
ModelItem::GetPropertyObject(
    DescType        inPropertyID)   //!< The property ID;  must match the application's AppleScript dictionary.
    const
{
    B::AEObjectPtr  propertyObj;
    
    switch (inPropertyID)
    {
    case pContainer:
        propertyObj = GetContainer();
        B_ASSERT(propertyObj != NULL);
        break;
        
    default:
        propertyObj = B::AEObject::GetPropertyObject(inPropertyID);
        break;
    }
    
    return (propertyObj);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::WriteProperty(
    DescType        inPropertyID, 
    B::AEWriter&    ioWriter) const
{
    switch (inPropertyID)
    {
    case pName:
        ioWriter.Write<typeUTF8Text>(GetName());
        break;
        
    case pModelItemValueProperty:
        WriteValue(ioWriter);
        break;

    default:
        B::AEObject::WriteProperty(inPropertyID, ioWriter);
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
ModelItem::ReadProperty(
    DescType        inPropertyID, 
    B::AEReader&    ioReader)
{
    B::AEDescriptor propertyDesc;
    
    switch (inPropertyID)
    {
    case pName:
        if (IsParentDictionary())
        {
            ModelItemPtr    parentItem  = boost::dynamic_pointer_cast<ModelItem>(GetContainer());
            B::String       newName;
            
            B_ASSERT(parentItem != NULL);
            
            if (ioReader.Read<typeUTF8Text>(newName) != GetName())
            {
                ModelItemPtr                thisPtr(GetPtr());  // prevent premature deletion.
                std::auto_ptr<UndoRename>   undoAction(new UndoRename(GetPtr(), newName));
                B::AutoUndo                 autoUndo(undoAction->GetActionName(), GetUndoTarget());

                if (parentItem->mValueDictionary.find(newName) != parentItem->mValueDictionary.end())
                    B_THROW(DuplicateItemException());
                
                parentItem->RemoveSubItem(GetPtr());
                parentItem->mValueDictionary.insert(SubItemMapValue(newName, thisPtr));
                SetContainer(parentItem);
                
                autoUndo.Add(undoAction);
                autoUndo.Commit();
                
                mNameChangedSignal();
            }
        }
        else
        {
            B_THROW(B::AENotModifiableException());
        }
        break;
        
    case pModelItemValueProperty:
        {
            std::auto_ptr<UndoValue>    undoAction(new UndoValue(GetPtr()));
            B::AutoUndo                 autoUndo(undoAction->GetActionName(), GetUndoTarget());
            B::OSPtr<CFTypeRef>         itemPtr;
            
            itemPtr = ReadItem(ioReader);
            
            SetModelItemValue(itemPtr);
            
            autoUndo.Add(undoAction);
            autoUndo.Commit();
        }
        break;

    default:
        B::AEObject::ReadProperty(inPropertyID, ioReader);
        break;
    }
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
ModelItem::CloneObject(
    DescType        inPosition,
    B::AEObjectPtr  inTarget, 
    const AERecord& /* inProperties */)
{
    if (IsRoot())
        B_THROW(B::AECantPutThatThereException());
    
    B_ASSERT(inTarget != NULL);
    
    ModelItemPtr    targetModel         = boost::dynamic_pointer_cast<ModelItem>(inTarget);
    ModelItemPtr    currentContainer    = boost::dynamic_pointer_cast<ModelItem>(GetContainer());
    ModelItemPtr    targetContainer;
    
    if (targetModel == NULL)
        B_THROW(B::AECantPutThatThereException());
    if (currentContainer == NULL)
        B_THROW(B::AECantPutThatThereException());
    
    // Some insertion positions make us siblings of the target, whereas 
    // others make us children of the target.
        
    switch (inPosition)
    {
    case kAEBefore:
    case kAEAfter:
    case kAEReplace:
        targetContainer = boost::dynamic_pointer_cast<ModelItem>(targetModel->GetContainer());
        if (targetContainer == NULL)
            B_THROW(B::AECantPutThatThereException());
        break;
        
    case kAEBeginning:
    case kAEEnd:
        targetContainer = targetModel;
        break;
    }
    
    B_ASSERT(targetContainer->IsContainer());
    
    ModelItemPtr    newModelPtr;
    
    if (targetContainer->IsArray())
    {
        newModelPtr = targetContainer->CopyArraySubItem(GetPtr(), inPosition, 
                                                        targetModel);
    }
    else    // dictionary
    {
        newModelPtr = targetContainer->CopyDictionarySubItem(GetPtr(), inPosition, 
                                                             targetModel);
    }
    
    ModelItemPtr                newModel    = newModelPtr;
    std::auto_ptr<UndoClone>    undoAction(new UndoClone(newModel));
    B::AutoUndo                 autoUndo(undoAction->GetActionName(), GetUndoTarget());
    
    autoUndo.Add(undoAction);
    autoUndo.Commit();
    
    targetContainer->mNewChildSignal(newModel->GetPtr());
    
    return (newModel);
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
ModelItem::CreateObject(
    DescType        inObjectClass,
    DescType        inPosition,
    B::AEObjectPtr  inTarget, 
    const AERecord& inProperties,
    const AEDesc&   inData)
{
    // Get some trivial cases out of the way.
    
    if (inObjectClass != kModelItemKind)
    {
        return (B::AEObject::CreateObject(inObjectClass, 
                                            inPosition, 
                                            inTarget, 
                                            inProperties, 
                                            inData));
    }
    
    if (!IsContainer())
        B_THROW(B::AEWrongDataTypeException());
    
    B_ASSERT(inTarget != NULL);
    
    ModelItemPtr    targetModel = boost::dynamic_pointer_cast<ModelItem>(inTarget);
    
    if (targetModel == NULL)
        B_THROW(B::AECantPutThatThereException());
    
    // Extract properties, if any.
    
    B::OSPtr<CFTypeRef> valuePtr(CFSTR(""));
    B::String           name;
    
    if (inProperties.descriptorType != typeNull)
    {
        B::AEReader reader(inProperties);
        
        if (reader.Contains(pModelItemValueProperty))
        {
            B::AutoAEReaderDescKey  autoKey(reader, pModelItemValueProperty);
            
            valuePtr = ReadItem(reader);
        }
        
        if (reader.Contains(pName) && IsDictionary())
        {
            reader.ReadKey<typeUTF8Text>(pName, name);
        }
    }
    
    ModelItemPtr    newModelPtr;
    
    if (IsArray())
    {
        newModelPtr = MakeNewArraySubItem(valuePtr, inPosition, targetModel);
    }
    else if (IsDictionary())
    {
        newModelPtr = MakeNewDictionarySubItem(valuePtr, name, inPosition, 
                                               targetModel);
    }
    
    ModelItemPtr                newModel    = newModelPtr;
    std::auto_ptr<UndoCreate>   undoAction(new UndoCreate(newModel));
    B::AutoUndo                 autoUndo(undoAction->GetActionName(), GetUndoTarget());
    
    autoUndo.Add(undoAction);
    autoUndo.Commit();
    
    mNewChildSignal(newModel->GetPtr());
    
    return (newModel);
}

// ------------------------------------------------------------------------------------------
void
ModelItem::DeleteObject()
{
    ModelItemPtr    parentItem  = boost::dynamic_pointer_cast<ModelItem>(GetContainer());
    
    B_ASSERT(parentItem != NULL);
    
    ModelItemPtr                itemPtr(GetPtr());  // prevent premature deletion.
    std::auto_ptr<UndoDelete>   undoAction(new UndoDelete(GetPtr()));
    B::AutoUndo                 autoUndo(undoAction->GetActionName(), GetUndoTarget());
    
    autoUndo.Add(undoAction);
    
    parentItem->RemoveSubItem(GetPtr());
    
    autoUndo.Commit();
    
    mDeletedSignal();
}

// ------------------------------------------------------------------------------------------
B::AEObjectPtr
ModelItem::MoveObject(
    DescType        inPosition,
    B::AEObjectPtr  inTarget)
{
    if (IsRoot())
        B_THROW(B::AECantPutThatThereException());
    
    B_ASSERT(inTarget != NULL);
    
    ModelItemPtr    targetModel         = boost::dynamic_pointer_cast<ModelItem>(inTarget);
    ModelItemPtr    currentContainer    = boost::dynamic_pointer_cast<ModelItem>(GetContainer());
    ModelItemPtr    targetContainer;
    
    if (targetModel == NULL)
        B_THROW(B::AECantPutThatThereException());
    if (currentContainer == NULL)
        B_THROW(B::AECantPutThatThereException());
    
    // Some insertion positions make us siblings of the target, whereas 
    // others make us children of the target.
    
    switch (inPosition)
    {
    case kAEBefore:
    case kAEAfter:
    case kAEReplace:
        targetContainer = boost::dynamic_pointer_cast<ModelItem>(targetModel->GetContainer());
        if (targetContainer == NULL)
            B_THROW(B::AECantPutThatThereException());
        break;
        
    case kAEBeginning:
    case kAEEnd:
        targetContainer = targetModel;
        break;
    }
    
    B_ASSERT(targetContainer->IsContainer());
    
    // An attempt to replace ourselves is treated as a no-op.
    
    if ((inPosition == kAEReplace) && (targetModel == GetPtr()))
        return (GetPtr());
    
    // If the move will cause us to lose our name, record our current 
    // name in the Undo stack.
    
//  B::String   nameAfterUndo;
//  
//  if (targetContainer->IsArray() && !currentContainer->IsArray())
//  {
//      nameAfterUndo = MakeUniqueDictionaryKeyForNewItem(GetName());
//      
//      mDocument->GetUndoManager()->Add(new UndoRename(this));
//  }
    
    ModelItemPtr            itemPtr(GetPtr());  // prevent premature deletion.
    std::auto_ptr<UndoMove> undoAction(new UndoMove(GetPtr()));
    B::AutoUndo             autoUndo(undoAction->GetActionName(), GetUndoTarget());
    B::String               name(GetName());
    
    currentContainer->RemoveSubItem(GetPtr());
    
    if (targetContainer->IsArray())
    {
        targetContainer->InsertArraySubItem(GetPtr(), inPosition, targetModel);
    }
    else    // dictionary
    {
        targetContainer->InsertDictionarySubItem(GetPtr(), inPosition, targetModel, 
                                                 name);
    }
    
    undoAction->Complete(GetPtr());
    
    autoUndo.Add(undoAction);
    autoUndo.Commit();
    
    mDeletedSignal();
    targetContainer->mNewChildSignal(GetPtr());
    
    return (GetPtr());
}

// ------------------------------------------------------------------------------------------
void
ModelItem::MakeSpecifier(
    B::AEWriter&    ioWriter) const
{
    if (IsParentDictionary())
    {
        // item "name" of ...
        BuildNameSpecifier(GetContainer(), GetClassID(), GetName(), ioWriter);
    }
    else if (IsParentArray())
    {
        // item <N> of ...
        BuildIndexSpecifier(GetContainer(), GetClassID(), GetIndex(), ioWriter);
    }
    else    // Root
    {
        // root item property of its enclosing document
        BuildPropertySpecifier(GetContainer(), PListerDoc::kRootItemProperty, ioWriter);
    }
}

// ------------------------------------------------------------------------------------------
void
ModelItem::SendChangeClassAppleEvent(
    EClass              inNewClass)
{
    OSStatus    err;
    
    if (inNewClass == mClass)
        return;
    
    switch (inNewClass)
    {
    case class_String:
        SendSetPropertyAppleEvent<typeUTF8Text>(
                    ModelItem::pModelItemValueProperty, 
                    B::String());
        break;
        
    case class_Data:
        {
            B::AEDescriptor dataDesc(typeData, NULL, 0);
            
            SendSetPropertyAppleEvent(
                        ModelItem::pModelItemValueProperty, 
                        dataDesc);
        }
        break;
        
    case class_Integer:
        SendSetPropertyAppleEvent<typeSInt32>(
                    ModelItem::pModelItemValueProperty, 
                    0);
        break;
        
    case class_Real:
        SendSetPropertyAppleEvent<typeIEEE64BitFloatingPoint>(
                    ModelItem::pModelItemValueProperty, 
                    0.0);
        break;
        
    case class_Boolean:
        SendSetPropertyAppleEvent<typeBoolean>(
                    ModelItem::pModelItemValueProperty, 
                    true);
        break;
        
    case class_Date:
        SendSetPropertyAppleEvent<typeLongDateTime>(
                    ModelItem::pModelItemValueProperty, 
                    ConvertCFDateToLongDateTime(GetDateTimeNow()));
        break;
        
    case class_Array:
        {
            B::AEDescriptor listDesc;
            
            err = AECreateList(NULL, 0, false, listDesc);
            B_THROW_IF_STATUS(err);
            
            SendSetPropertyAppleEvent(
                        ModelItem::pModelItemValueProperty, 
                        listDesc);
        }
        break;
        
    case class_Dictionary:
        {
            B::AEDescriptor recordDesc;
            
            err = AECreateList(NULL, 0, true, recordDesc);
            B_THROW_IF_STATUS(err);
            
            SendSetPropertyAppleEvent(
                        ModelItem::pModelItemValueProperty, 
                        recordDesc);
        }
        break;
    }
}

// ------------------------------------------------------------------------------------------
void
ModelItem::WriteValue(
    B::AEWriter&    ioWriter) const
{
    switch (mClass)
    {
    case class_String:
        ioWriter << mValueString;
        break;
        
    case class_Data:
        {
            B::AutoAEWriterDesc autoDesc(ioWriter, typeData);
            
            ioWriter.Append(CFDataGetBytePtr(mValueData), 
                            CFDataGetLength(mValueData));
        }
        break;
        
    case class_Integer:
        if ((mValueInteger >= INT_MIN) && (mValueInteger <= INT_MAX))
        {
            SInt32  intval  = mValueInteger;
            
            ioWriter.Write<typeSInt32>(intval);
        }
        else
        {
            ioWriter.Write<typeSInt64>(mValueInteger);
        }
        break;
        
    case class_Real:
        ioWriter.Write<typeIEEE64BitFloatingPoint>(mValueReal);
        break;
        
    case class_Boolean:
        ioWriter.Write<typeBoolean>(mValueBoolean);
        break;
        
    case class_Date:
        {
            LongDateTime    ldt = ConvertCFDateToLongDateTime(mValueDate);
            
            ioWriter.Write<typeLongDateTime>(ldt);
        }
        break;
        
    case class_Array:
        {
            B::AutoAEWriterList autoList(ioWriter);
            
            std::for_each(mValueArray.begin(), mValueArray.end(), 
                          ModelValueWriter(ioWriter));
        }
        break;
        
    case class_Dictionary:
        {
            B::AutoAEWriterRecord   autoRecord(ioWriter, typeAERecord);
            B::AutoAEWriterList     autoList(ioWriter, keyASUserRecordFields);
            
            std::for_each(mValueDictionary.begin(), mValueDictionary.end(), 
                          ModelValueWriter(ioWriter));
        }
        break;
    }
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ReadItem(
    B::AEReader&    ioReader)
{
    B::OSPtr<CFTypeRef> itemPtr;
    
    if (ioReader.IsRecord())
    {
        itemPtr = ItemValue<class_Dictionary>::ReadItem(ioReader);
    }
    else if (ioReader.IsList())
    {
        itemPtr = ItemValue<class_Array>::ReadItem(ioReader);
    }
    else
    {
        switch (ioReader.GetType())
        {
        case typeChar:
        case typeIntlText:
//      case typeCString:   // deprecated
//      case typePString:   // deprecated
        case typeUnicodeText:
        case typeUTF16ExternalRepresentation:
        case typeUTF8Text:
            itemPtr = ItemValue<class_String>::ReadItem(ioReader);
            break;
            
        case typeData:
            itemPtr = ItemValue<class_Data>::ReadItem(ioReader);
            break;
            
        case typeSInt16:
        case typeSInt32:
        case typeUInt32:
        case typeSInt64:
            itemPtr = ItemValue<class_Integer>::ReadItem(ioReader);
            break;
            
        case typeIEEE32BitFloatingPoint:
        case typeIEEE64BitFloatingPoint:
        case type128BitFloatingPoint:
            itemPtr = ItemValue<class_Real>::ReadItem(ioReader);
            break;
            
        case typeTrue:
        case typeFalse:
        case typeBoolean:
            itemPtr = ItemValue<class_Boolean>::ReadItem(ioReader);
            break;
            
        case typeLongDateTime:
            itemPtr = ItemValue<class_Date>::ReadItem(ioReader);
            break;
            
        case 'plst':
            itemPtr = ReadPropertyList(ioReader);
            break;
            
        default:
            B_THROW(B::AECoercionFailException());
            break;
        }
    }
    
    return (itemPtr);
}

// ------------------------------------------------------------------------------------------
B::OSPtr<CFTypeRef>
ModelItem::ReadPropertyList(
    B::AEReader&    ioReader)
{
    B::OSPtr<CFDataRef> dataPtr(ReadIntoBlob(ioReader));
    CFTypeRef           typeRef;
    CFStringRef         errorRef;
    
    typeRef = CFPropertyListCreateFromXMLData(NULL, dataPtr, 
                                              kCFPropertyListImmutable, 
                                              &errorRef);
    
    if (typeRef == NULL)
    {
        B::OSPtr<CFStringRef>   errorStr(errorRef, B::from_copy);
        
        B_THROW(B::PropertyListCreateException(errorStr));
    }
    
    return (B::OSPtr<CFTypeRef>(typeRef, B::from_copy));
}

// ------------------------------------------------------------------------------------------
pascal OSErr
ModelItem::CoerceToPropertyList(
    const AEDesc*   fromDesc, 
    DescType        /* toType */, 
    long            /* handlerRefcon */, 
    AEDesc*         result)
{
    OSStatus    err;
    
    try
    {
        if (fromDesc->descriptorType == 'plst')
            B_THROW(B::AECoercionFailException());
        
        B::AEReader         reader(*fromDesc);
        B::OSPtr<CFTypeRef> typePtr(ReadItem(reader));
        B::OSPtr<CFDataRef> dataPtr(CFPropertyListCreateXMLData(NULL, typePtr), B::from_copy);
        
        err = ::AECreateDesc('plst', 
                             CFDataGetBytePtr(dataPtr), 
                             CFDataGetLength(dataPtr), 
                             result);
        B_THROW_IF_STATUS(err);
    }
    catch (std::exception& ex)
    {
        err = B::ErrorHandler::GetStatus(ex, errAECoercionFail);
    }
    catch (...)
    {
        err = errAECoercionFail;
    }
    
    return (err);
}


// Messages for ModelItem exception classes

//  CFCopyLocalizedString("DuplicateItemException",
//                        "Error message for a duplicate item.")
//  CFCopyLocalizedString("MoveIntoOffspringException",
//                        "Error message when an attempt is made to move a property list item into one of its children.")
