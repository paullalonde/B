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

#ifndef BKeyAdapter_H_
#define BKeyAdapter_H_

#pragma once

// B headers
#include "BFwd.h"


namespace B {

template <typename ITERATOR>
class KeyAdapter : public std::iterator<
                        typename std::iterator_traits<ITERATOR>::iterator_category, 
                        typename ITERATOR::value_type::first_type>
{
public:
    
    typedef ITERATOR                                                    iterator_type;
    typedef typename ITERATOR::value_type::first_type                   value_type;
    typedef typename std::iterator_traits<ITERATOR>::difference_type    difference_type;
    typedef typename std::iterator_traits<ITERATOR>::reference          reference;
    typedef typename std::iterator_traits<ITERATOR>::pointer            pointer;
    
    KeyAdapter() {}
    KeyAdapter(const iterator_type& it) : mIterator(it) {}
    KeyAdapter(const KeyAdapter& ka) : mIterator(ka.mIterator) {}
    
    KeyAdapter&     operator = (const KeyAdapter& ka)   { mIterator = ka.mIterator; return *this; }
    KeyAdapter&     operator ++ ()                      { ++mIterator; return *this; }
    KeyAdapter      operator ++ (int)                   { return KeyAdapter(mIterator++); }
    KeyAdapter&     operator -- ()                      { --mIterator; return *this; }
    KeyAdapter      operator -- (int)                   { return KeyAdapter(mIterator--); }
    KeyAdapter&     operator += (difference_type n)     { mIterator += n; return *this; }
    KeyAdapter&     operator -= (difference_type n)     { mIterator -= n; return *this; }
    value_type      operator * () const                 { return mIterator->first; }
    pointer         operator -> () const                { return &mIterator->first; }
    iterator_type   base() const                        { return mIterator; }
    
private:
    
    iterator_type   mIterator;
};

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator == (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() == it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator != (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() != it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator < (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() < it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator <= (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() <= it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator > (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() > it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator >= (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() >= it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline KeyAdapter<ITERATOR>
operator + (const KeyAdapter<ITERATOR>& it, typename KeyAdapter<ITERATOR>::difference_type n)
{
    return KeyAdapter<ITERATOR>(it.base() + n);
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline KeyAdapter<ITERATOR>
operator + (typename KeyAdapter<ITERATOR>::difference_type n, const KeyAdapter<ITERATOR>& it)
{
    return KeyAdapter<ITERATOR>(n + it.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline KeyAdapter<ITERATOR>
operator - (const KeyAdapter<ITERATOR>& it, typename KeyAdapter<ITERATOR>::difference_type n)
{
    return KeyAdapter<ITERATOR>(it.base() - n);
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline KeyAdapter<ITERATOR>
operator - (typename KeyAdapter<ITERATOR>::difference_type n, const KeyAdapter<ITERATOR>& it)
{
    return KeyAdapter<ITERATOR>(n - it.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline typename KeyAdapter<ITERATOR>::difference_type 
operator - (const KeyAdapter<ITERATOR>& it1, const KeyAdapter<ITERATOR>& it2)
{
    return (it1.base() - it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
KeyAdapter<ITERATOR>
MakeKeyAdapter(const ITERATOR& it)
{
    return KeyAdapter<ITERATOR>(it);
}

}   // namespace B


#endif  // BKeyAdapter_H_
