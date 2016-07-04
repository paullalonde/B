// ==========================================================================================
//  
//  Copyright (C) 2005-2006 Paul Lalonde enrg.
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

#ifndef BPasteboard_H_
#define BPasteboard_H_

#pragma once

// B headers
#include "BOSPtr.h"


namespace B {

// forward declarations
class   PasteboardFlavor;
class   PasteboardItem;
class   String;
class   Url;


// ==========================================================================================
//  Pasteboard

class Pasteboard : public boost::noncopyable
{
public:
    
    //! @name Types
    //@{
    typedef boost::function2<void, PasteboardItem&, CFStringRef>    PromiseKeeper;
    //@}
    
    //! @name Constructors
    //@{
    //! Default constructor.  Constructs a uniquely-named pasteboard.
            Pasteboard();
    //! Constructor.  Constructs a pasteboard object with the given name.
            Pasteboard(CFStringRef inName);
    //! Constructor.  Constructs a pasteboard object with the given pasteboard reference.
            Pasteboard(PasteboardRef inPasteboardRef);
    //@}
    
    //! @name Inquiries
    //@{
    bool        HasBeenModified() const     { return ((mSyncFlags & kPasteboardModified) != 0); }
    bool        IsOwner() const             { return ((mSyncFlags & kPasteboardClientIsOwner) != 0); }
    UInt32      GetChangeCount() const      { return (mChangeCount); }
    //! Returns the pasteboard's name.
    String      GetName() const;
    //! Returns the pasteboard's paste location.
    Url         GetPasteLocation() const;
    //@}
    
    //! @name Modifiers
    //@{
    void        Synchronize();
    //! Empties the pasteboard & acquires its ownership.
    void        Clear();
    void        Clear(PromiseKeeper inKeeper);
    void        SetPromiseKeeper(PromiseKeeper inKeeper);
    //! Sets the pasteboard's paste location.
    void        SetPasteLocation(const Url& inUrl);
    //! Resolves all promised flavor data on this pasteboard.
    void        ResolvePromises();
    //! Resolves all promised flavor data on all pasteboards.
    static void ResolveAllPromises();
    //@}
    
    //! @name Pasteboard Items
    //@{
    //! Returns the number of pasteboard items.
    size_t          size() const;
    //! Returns the nth pasteboard item.
    PasteboardItem  operator [] (size_t index) const;
    PasteboardItem  NewItem(PasteboardItemID inItemID);
    template <typename T>
    PasteboardItem  NewItem(T inItemID);
    //@}
    
private:
    
    typedef PasteboardItem          IteratorValueType;
    typedef ptrdiff_t               IteratorDifferenceType;
    typedef const PasteboardItem*   IteratorConstPointerType;
    typedef const PasteboardItem&   IteratorConstReferenceType;

public:

    // PasteboardItem iterator
    /*!
        @todo   %Document this class!
    */
    class const_iterator : public std::iterator<std::random_access_iterator_tag, 
                                                IteratorValueType, 
                                                IteratorDifferenceType, 
                                                IteratorConstPointerType, 
                                                IteratorConstReferenceType>
    {
    private:
        
        typedef std::iterator<std::random_access_iterator_tag, 
                              IteratorValueType, 
                              IteratorDifferenceType, 
                              IteratorConstPointerType, 
                              IteratorConstReferenceType> base;

    public:
        
        // types
        typedef base::value_type        value_type;
        typedef base::difference_type   difference_type;
        typedef base::pointer           pointer;
        typedef base::reference         reference;
        
        // constructor
        const_iterator()                                        : mPasteboard(NULL), mIndex(0)  {}
        const_iterator(const const_iterator& it)                : mPasteboard(it.mPasteboard), mIndex(it.mIndex)    {}
        
        const_iterator& operator = (const const_iterator& it)   { mPasteboard = it.mPasteboard; mIndex = it.mIndex; return (*this); }
        
        value_type  operator * () const;
        value_type  operator [] (difference_type i) const;
        
        const_iterator& operator ++ ()                          { ++mIndex; return (*this); }
        const_iterator& operator -- ()                          { --mIndex; return (*this); }
        const_iterator& operator += (difference_type n)         { mIndex += n; return (*this); }
        const_iterator& operator -= (difference_type n)         { mIndex -= n; return (*this); }
        const_iterator  operator ++ (int)                       { return (const_iterator(mPasteboard, mIndex++)); }
        const_iterator  operator -- (int)                       { return (const_iterator(mPasteboard, mIndex--)); }
        const_iterator  operator +  (difference_type n) const   { return (const_iterator(mPasteboard, mIndex + n)); }
        const_iterator  operator -  (difference_type n) const   { return (const_iterator(mPasteboard, mIndex - n)); }
        
        // friends
        friend const_iterator   operator +  (difference_type n, const const_iterator& it)           { return (const_iterator(it.mPasteboard, n + it.mIndex)); }
        friend const_iterator   operator -  (difference_type n, const const_iterator& it)           { return (const_iterator(it.mPasteboard, n - it.mIndex)); }
        friend difference_type  operator -  (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex -  it2.mIndex); }
        friend bool             operator == (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex == it2.mIndex); }
        friend bool             operator != (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex != it2.mIndex); }
        friend bool             operator >  (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex >  it2.mIndex); }
        friend bool             operator >= (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex >= it2.mIndex); }
        friend bool             operator <  (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex <  it2.mIndex); }
        friend bool             operator <= (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex <= it2.mIndex); }
        
    private:
        
        const_iterator(const Pasteboard* inPasteboard, size_t inIndex)
            : mPasteboard(inPasteboard), mIndex(inIndex)    {}
        
        // member variables
        const Pasteboard*   mPasteboard;
        size_t              mIndex;
        
        // friends
        friend class    Pasteboard;
    };

    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator; //!< The type of constant reverse iterators.
    
    //! @name Iterators
    //@{
    //! Returns an iterator for the beginning of the list of pasteboard items.
    const_iterator          begin() const;
    //! Returns an iterator for the end of the list of pasteboard items.
    const_iterator          end() const;
    //! Returns an iterator for the beginning of a reverse iteration of the list of pasteboard items.
    const_reverse_iterator  rbegin() const;
    //! Returns an iterator for the end of a reverse iteration of the list of pasteboard items.
    const_reverse_iterator  rend() const;
    //@}
    
private:
    
    void    InvokePromiseKeeper(
                PasteboardRef       pasteboard, 
                PasteboardItemID    itemID, 
                CFStringRef         flavorType);
    
    // callbacks
    static OSStatus PromiseKeeperProc(
                        PasteboardRef       pasteboard, 
                        PasteboardItemID    itemID, 
                        CFStringRef         flavorType, 
                        void*               context);
    
    // member variables
    OSPtr<PasteboardRef>    mPasteboard;
    PasteboardSyncFlags     mSyncFlags;
    UInt32                  mChangeCount;
    PromiseKeeper           mKeeper;
};


// ==========================================================================================
//  PasteboardItem

class PasteboardItem
{
public:
    
    //! @name Constructors / Destructor
    //@{
    //! Constructor.
            PasteboardItem(
                const OSPtr<PasteboardRef>& inPasteboard, 
                PasteboardItemID            inItemID);
    //! Constructor.
            template <typename T> PasteboardItem(
                const OSPtr<PasteboardRef>& inPasteboard, 
                T                           inItemID);
    //! Copy constructor.
            PasteboardItem(const PasteboardItem& src);
    //@}
    
    PasteboardItem& operator = (const PasteboardItem& src);
    
    //! @name Inquiries
    //@{
    PasteboardItemID    GetItemID() const       { return (mItemID); }
    //@}
    
    //! @name Flavors
    //@{
    //! Does the item contain the given flavor?
    bool                Contains(CFStringRef inFlavorType) const;
    //! Returns the number of flavors for this pasteboard item.
    size_t              size() const;
    //! Returns the nth flavor of this pasteboard item.
    PasteboardFlavor    operator [] (size_t inIndex) const;
    PasteboardFlavor    NewFlavor(CFStringRef inFlavorType) const;
    //@}
    
private:
    
    typedef PasteboardFlavor        IteratorValueType;
    typedef ptrdiff_t               IteratorDifferenceType;
    typedef const PasteboardFlavor* IteratorConstPointerType;
    typedef const PasteboardFlavor& IteratorConstReferenceType;

public:

    // PasteboardFlavor iterator
    /*!
        @todo   %Document this class!
    */
    class const_iterator : public std::iterator<std::random_access_iterator_tag, 
                                                IteratorValueType, 
                                                IteratorDifferenceType, 
                                                IteratorConstPointerType, 
                                                IteratorConstReferenceType>
    {
    private:
        
        typedef std::iterator<std::random_access_iterator_tag, 
                              IteratorValueType, 
                              IteratorDifferenceType, 
                              IteratorConstPointerType, 
                              IteratorConstReferenceType> base;

    public:
        
        // types
        typedef base::value_type        value_type;
        typedef base::difference_type   difference_type;
        typedef base::pointer           pointer;
        typedef base::reference         reference;
        
        // constructors
        const_iterator()                                        : mPasteboardItem(NULL), mIndex(0)  {}
        const_iterator(const const_iterator& it)                : mPasteboardItem(it.mPasteboardItem), mIndex(it.mIndex)    {}
        
        const_iterator& operator = (const const_iterator& it)   { mPasteboardItem = it.mPasteboardItem; mIndex = it.mIndex; return (*this); }
        
        value_type  operator * () const;
        value_type  operator [] (difference_type i) const;
        
        const_iterator& operator ++ ()                          { ++mIndex; return (*this); }
        const_iterator& operator -- ()                          { --mIndex; return (*this); }
        const_iterator& operator += (difference_type n)         { mIndex += n; return (*this); }
        const_iterator& operator -= (difference_type n)         { mIndex -= n; return (*this); }
        const_iterator  operator ++ (int)                       { return (const_iterator(mPasteboardItem, mIndex++)); }
        const_iterator  operator -- (int)                       { return (const_iterator(mPasteboardItem, mIndex--)); }
        const_iterator  operator +  (difference_type n) const   { return (const_iterator(mPasteboardItem, mIndex + n)); }
        const_iterator  operator -  (difference_type n) const   { return (const_iterator(mPasteboardItem, mIndex - n)); }
        
        // friends
        friend const_iterator   operator +  (difference_type n, const const_iterator& it)           { return (const_iterator(it.mPasteboardItem, n + it.mIndex)); }
        friend const_iterator   operator -  (difference_type n, const const_iterator& it)           { return (const_iterator(it.mPasteboardItem, n - it.mIndex)); }
        friend difference_type  operator -  (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex -  it2.mIndex); }
        friend bool             operator == (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex == it2.mIndex); }
        friend bool             operator != (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex != it2.mIndex); }
        friend bool             operator >  (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex >  it2.mIndex); }
        friend bool             operator >= (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex >= it2.mIndex); }
        friend bool             operator <  (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex <  it2.mIndex); }
        friend bool             operator <= (const const_iterator& it1, const const_iterator& it2)  { return (it1.mIndex <= it2.mIndex); }
        
    private:
        
        const_iterator(const PasteboardItem* inPasteboardItem, size_t inIndex)
            : mPasteboardItem(inPasteboardItem), mIndex(inIndex)    {}
        
        // member variables
        const PasteboardItem*   mPasteboardItem;
        size_t                  mIndex;
        
        // friends
        friend class    PasteboardItem;
    };

    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator; //!< The type of constant reverse iterators.
    
    //! @name Iterators
    //@{
    //! Returns an iterator for the beginning of the list of pasteboard item flavors.
    const_iterator          begin() const;
    //! Returns an iterator for the end of the list of pasteboard item flavors.
    const_iterator          end() const;
    //! Returns an iterator for the beginning of a reverse iteration of the list of pasteboard item flavors.
    const_reverse_iterator  rbegin() const;
    //! Returns an iterator for the end of a reverse iteration of the list of pasteboard item flavors.
    const_reverse_iterator  rend() const;
    //@}
    
private:
    
    CFArrayRef  GetFlavorTypes() const;
    
    // member variables
    OSPtr<PasteboardRef>        mPasteboard;
    PasteboardItemID            mItemID;
    mutable OSPtr<CFArrayRef>   mFlavorTypes;
};


// ==========================================================================================
//  PasteboardFlavor

class PasteboardFlavor
{
public:
    
    //! @name Constructors / Destructor
    //@{
    //! Default constructor.
                PasteboardFlavor();
    //! Constructor.
                PasteboardFlavor(
                    const OSPtr<PasteboardRef>& inPasteboard, 
                    PasteboardItemID            inItemID,
                    CFStringRef                 inFlavorType);
    //! Copy constructor.
                PasteboardFlavor(const PasteboardFlavor& src);
    //@}
    
    PasteboardFlavor&   operator = (const PasteboardFlavor& src);
    
    //! @name Inquiries
    //@{
    CFStringRef GetFlavorType() const       { return (mFlavorType); }
    bool        IsSaved() const             { return !GetFlagBit(kPasteboardFlavorNotSaved); }
    bool        IsPromised() const          { return GetFlagBit(kPasteboardFlavorPromised); }
    bool        IsSenderOnly() const        { return GetFlagBit(kPasteboardFlavorSenderOnly); }
    bool        IsRequestOnly() const       { return GetFlagBit(kPasteboardFlavorRequestOnly); }
    bool        IsSenderTranslated() const  { return GetFlagBit(kPasteboardFlavorSenderTranslated); }
    bool        IsSystemTranslated() const  { return GetFlagBit(kPasteboardFlavorSystemTranslated); }
    //! Returns the flavor's flags.
    PasteboardFlavorFlags   GetFlags() const;
    //! Returns the flavor's data.
    OSPtr<CFDataRef>        GetData() const;
    bool                    GetData(OSPtr<CFDataRef>& outData) const;
    //@}
    
    //! @name Modifiers
    //@{
    void    SetSaved(bool inSaved)                      { SetFlagBit(kPasteboardFlavorNotSaved, !inSaved); }
    void    SetSenderOnly(bool inSenderOnly)            { SetFlagBit(kPasteboardFlavorSenderOnly, inSenderOnly); }
    void    SetRequestOnly(bool inRequestOnly)          { SetFlagBit(kPasteboardFlavorRequestOnly, inRequestOnly); }
//  void    SetSenderTranslated(bool inTranslated);
    void    SetFlags(PasteboardFlavorFlags inFlags);
    void    SetData(const OSPtr<CFDataRef>& inData);
    bool    SetData(const OSPtr<CFDataRef>& inData, const std::nothrow_t&);
    void    SetData(const OSPtr<CFMutableDataRef>& inData);
    bool    SetData(const OSPtr<CFMutableDataRef>& inData, const std::nothrow_t&);
    void    SetData();
    bool    SetData(const std::nothrow_t&);
    //@}
    
private:
    
    bool    GetFlagBit(PasteboardFlavorFlags inBit) const;
    void    SetFlagBit(PasteboardFlavorFlags inBit, bool inSet);
    
    // member variables
    OSPtr<PasteboardRef>            mPasteboard;
    PasteboardItemID                mItemID;
    CFStringRef                     mFlavorType;
    mutable PasteboardFlavorFlags   mFlags;
    mutable bool                    mGotFlags;
};


// ==========================================================================================
//  Pasteboard

// ------------------------------------------------------------------------------------------
template <typename T> PasteboardItem
Pasteboard::NewItem(T inItemID)
{
    return PasteboardItem(mPasteboard, inItemID);
}

// ------------------------------------------------------------------------------------------
inline Pasteboard::const_iterator::value_type
Pasteboard::const_iterator::operator * () const
{
    B_ASSERT(mPasteboard != NULL);
    
    return (mPasteboard->operator [] (mIndex));
}

// ------------------------------------------------------------------------------------------
inline Pasteboard::const_iterator::value_type
Pasteboard::const_iterator::operator [] (difference_type i) const
{
    B_ASSERT(mPasteboard != NULL);
    
    return (mPasteboard->operator [] (mIndex + i));
}


// ==========================================================================================
//  PasteboardItem

// ------------------------------------------------------------------------------------------
template <typename T>
PasteboardItem::PasteboardItem(
    const OSPtr<PasteboardRef>& inPasteboard, 
    T                           inItemID)
        : mPasteboard(inPasteboard), mItemID(reinterpret_cast<PasteboardItemID>(inItemID))
{
}

// ------------------------------------------------------------------------------------------
inline PasteboardItem::const_iterator::value_type
PasteboardItem::const_iterator::operator * () const
{
    B_ASSERT(mPasteboardItem != NULL);
    
    return (mPasteboardItem->operator [] (mIndex));
}

// ------------------------------------------------------------------------------------------
inline PasteboardItem::const_iterator::value_type
PasteboardItem::const_iterator::operator [] (difference_type i) const
{
    B_ASSERT(mPasteboardItem != NULL);
    
    return (mPasteboardItem->operator [] (mIndex + i));
}


// ==========================================================================================
//  PasteboardFlavor

// ------------------------------------------------------------------------------------------
inline bool
PasteboardFlavor::GetFlagBit(PasteboardFlavorFlags inBit) const
{
    return ((GetFlags() & inBit) != 0);
}


}   // namespace B


#endif  // BPasteboard_H_
