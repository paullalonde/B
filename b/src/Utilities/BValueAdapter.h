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

#ifndef BValueAdapter_H_
#define BValueAdapter_H_

#pragma once

// B headers
#include "BFwd.h"


namespace B {

template <typename ITERATOR>
class ValueAdapter : public std::iterator<
                        typename std::iterator_traits<ITERATOR>::iterator_category, 
                        typename ITERATOR::value_type::second_type>
{
public:
    
    typedef ITERATOR                                                    iterator_type;
    typedef typename ITERATOR::value_type::second_type                  value_type;
    typedef typename std::iterator_traits<ITERATOR>::difference_type    difference_type;
    typedef typename std::iterator_traits<ITERATOR>::reference          reference;
    typedef typename std::iterator_traits<ITERATOR>::pointer            pointer;
    
    ValueAdapter() {}
    ValueAdapter(const iterator_type& it) : mIterator(it) {}
    ValueAdapter(const ValueAdapter& va) : mIterator(va.mIterator) {}
    
    ValueAdapter&   operator = (const ValueAdapter& va) { mIterator = va.mIterator; return (*this); }
    ValueAdapter&   operator ++ ()                      { ++mIterator; return (*this); }
    ValueAdapter    operator ++ (int)                   { return ValueAdapter(mIterator++); }
    ValueAdapter&   operator -- ()                      { --mIterator; return (*this); }
    ValueAdapter    operator -- (int)                   { return ValueAdapter(mIterator--); }
    ValueAdapter&   operator += (difference_type n)     { mIterator += n; return *this; }
    ValueAdapter&   operator -= (difference_type n)     { mIterator -= n; return *this; }
    value_type      operator * () const                 { return mIterator->second; }
    pointer         operator -> () const                { return &mIterator->second; }
    iterator_type   base() const                        { return mIterator; }
    
private:
    
    iterator_type   mIterator;
};

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator == (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() == it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator != (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() != it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator < (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() < it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator <= (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() <= it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator > (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() > it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline bool operator >= (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() >= it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline ValueAdapter<ITERATOR>
operator + (const ValueAdapter<ITERATOR>& it, typename ValueAdapter<ITERATOR>::difference_type n)
{
    return ValueAdapter<ITERATOR>(it.base() + n);
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline ValueAdapter<ITERATOR>
operator + (typename ValueAdapter<ITERATOR>::difference_type n, const ValueAdapter<ITERATOR>& it)
{
    return ValueAdapter<ITERATOR>(n + it.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline ValueAdapter<ITERATOR>
operator - (const ValueAdapter<ITERATOR>& it, typename ValueAdapter<ITERATOR>::difference_type n)
{
    return ValueAdapter<ITERATOR>(it.base() - n);
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline ValueAdapter<ITERATOR>
operator - (typename ValueAdapter<ITERATOR>::difference_type n, const ValueAdapter<ITERATOR>& it)
{
    return ValueAdapter<ITERATOR>(n - it.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
inline typename ValueAdapter<ITERATOR>::difference_type 
operator - (const ValueAdapter<ITERATOR>& it1, const ValueAdapter<ITERATOR>& it2)
{
    return (it1.base() - it2.base());
}

// ------------------------------------------------------------------------------------------
template <typename ITERATOR>
ValueAdapter<ITERATOR>
MakeValueAdapter(const ITERATOR& it)
{
    return ValueAdapter<ITERATOR>(it);
}

}   // namespace B


#endif  // BValueAdapter_H_
