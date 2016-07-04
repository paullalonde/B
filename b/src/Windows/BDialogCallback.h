// ==========================================================================================
//  
//  Copyright (C) 2004-2006 Paul Lalonde enrg.
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

#ifndef BDialogCallback_H_
#define BDialogCallback_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/function.hpp>
#include <boost/utility.hpp>


namespace B {

#if DOXYGEN_SCAN

/*!
    @brief  Helper class for implementing dialog callbacks.
    
    This is actually a number of separate template classes which vary only in 
    the number of template arguments.  They are named @c DialogCallbackBase0 to 
    @c DialogCallbackBase5.  They aren't documented separately as it would be highly 
    redundant.
    
    Developers won't normally need to use this class directly.
*/
template <class BASE, typename T1, typename ... TN>
class DialogCallbackBaseN : public boost::noncopyable
{
public:
    
    //! Non-throwing destructor to prevent gcc from complaining.
    virtual         ~DialogCallbackBaseN();
    
    //! The callback is invoked via this operator.
    virtual void    operator () (const BASE&, T1, ... TN) const;
};

/*!
    @brief  Helper class for implementing dialog callbacks.
    
    This is actually a number of separate template classes which vary only in 
    the number of template arguments.  They are named @c DialogCallback0 to 
    @c DialogCallback5.  They aren't documented separately as it would be highly 
    redundant.
    
    Developers won't normally need to use this class directly.
    
    @note   @a DERIVED needs to be a class derived from @a BASE.
*/
template <class DERIVED, class BASE, typename T1, ... typename TN>
class DialogCallbackN : public DialogCallbackBaseN<BASE, T1, ... TN>
{
public:
    
    //! The type of the @c boost::function stored internally.
    typedef boost::function6<void, DERIVED&, T1, ... TN>    FunctorType;
    
    //! Constructor, taking a functor.
                    DialogCallbackN(FunctorType inFunctor);
                    
    //! Non-throwing destructor to prevent gcc from complaining.
    virtual         ~DialogCallbackN();
    
    //! The callback is invoked via this operator.
    virtual void    operator () (const BASE&, T1, ... TN) const;

private:
    
    FunctorType mFunctor;
};

#endif  // DOXYGEN_SCAN


#ifndef DOXYGEN_SKIP

// ==========================================================================================
//  DialogCallbackBase0

template <class BASE>
class DialogCallbackBase0 : public boost::noncopyable
{
public:
    virtual         ~DialogCallbackBase0();
    virtual void    operator () (const BASE&) const;
};

// ------------------------------------------------------------------------------------------
template <class BASE>
DialogCallbackBase0<BASE>::~DialogCallbackBase0()
{
}

// ------------------------------------------------------------------------------------------
template <class BASE> void
DialogCallbackBase0<BASE>::operator () (const BASE&) const
{
}


// ==========================================================================================
//  DialogCallbackBase1

template <class BASE, typename T1>
class DialogCallbackBase1 : public boost::noncopyable
{
public:
    virtual         ~DialogCallbackBase1();
    virtual void    operator () (const BASE&, T1) const;
};

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1>
DialogCallbackBase1<BASE, T1>::~DialogCallbackBase1()
{
}

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1> void
DialogCallbackBase1<BASE, T1>::operator () (const BASE&, T1) const
{
}


// ==========================================================================================
//  DialogCallbackBase2

template <class BASE, typename T1, typename T2>
class DialogCallbackBase2 : public boost::noncopyable
{
public:
    virtual         ~DialogCallbackBase2();
    virtual void    operator () (const BASE&, T1, T2) const;
};

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2>
DialogCallbackBase2<BASE, T1, T2>::~DialogCallbackBase2()
{
}

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2> void
DialogCallbackBase2<BASE, T1, T2>::operator () (const BASE&, T1, T2) const
{
}


// ==========================================================================================
//  DialogCallbackBase3

template <class BASE, typename T1, typename T2, typename T3>
class DialogCallbackBase3 : public boost::noncopyable
{
public:
    virtual         ~DialogCallbackBase3();
    virtual void    operator () (const BASE&, T1, T2, T3) const;
};

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2, typename T3>
DialogCallbackBase3<BASE, T1, T2, T3>::~DialogCallbackBase3()
{
}

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2, typename T3> void
DialogCallbackBase3<BASE, T1, T2, T3>::operator () (const BASE&, T1, T2, T3) const
{
}


// ==========================================================================================
//  DialogCallbackBase4

template <class BASE, typename T1, typename T2, typename T3, typename T4>
class DialogCallbackBase4 : public boost::noncopyable
{
public:
    virtual         ~DialogCallbackBase4();
    virtual void    operator () (const BASE&, T1, T2, T3, T4) const;
};

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2, typename T3, typename T4>
DialogCallbackBase4<BASE, T1, T2, T3, T4>::~DialogCallbackBase4()
{
}

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2, typename T3, typename T4> void
DialogCallbackBase4<BASE, T1, T2, T3, T4>::operator () (const BASE&, T1, T2, T3, T4) const
{
}


// ==========================================================================================
//  DialogCallbackBase5

template <class BASE, typename T1, typename T2, typename T3, typename T4, typename T5>
class DialogCallbackBase5 : public boost::noncopyable
{
public:
    virtual         ~DialogCallbackBase5();
    virtual void    operator () (const BASE&, T1, T2, T3, T4, T5) const;
};

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2, typename T3, typename T4, typename T5>
DialogCallbackBase5<BASE, T1, T2, T3, T4, T5>::~DialogCallbackBase5()
{
}

// ------------------------------------------------------------------------------------------
template <class BASE, typename T1, typename T2, typename T3, typename T4, typename T5> void
DialogCallbackBase5<BASE, T1, T2, T3, T4, T5>::operator () (const BASE&, T1, T2, T3, T4, T5) const
{
}


// ==========================================================================================
//  DialogCallback0

template <class DERIVED, class BASE>
class DialogCallback0 : public DialogCallbackBase0<BASE>
{
public:
    
    typedef boost::function1<void, const DERIVED&>  FunctorType;
    
                    DialogCallback0(FunctorType inFunctor);
    virtual         ~DialogCallback0();
    virtual void    operator () (const BASE&) const;

private:
    
    FunctorType mFunctor;
};

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE>
DialogCallback0<DERIVED, BASE>::DialogCallback0(FunctorType inFunctor)
    : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE>
DialogCallback0<DERIVED, BASE>::~DialogCallback0()
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE> void
DialogCallback0<DERIVED, BASE>::operator () (const BASE& inBase) const
{
    mFunctor(dynamic_cast<const DERIVED&>(inBase));
}


// ==========================================================================================
//  DialogCallback1

template <class DERIVED, class BASE, typename T1>
class DialogCallback1 : public DialogCallbackBase1<BASE, T1>
{
public:
    
    typedef boost::function2<void, const DERIVED&, T1>  FunctorType;
    
                    DialogCallback1(FunctorType inFunctor);
    virtual         ~DialogCallback1();
    virtual void    operator () (const BASE&, T1) const;

private:
    
    FunctorType mFunctor;
};

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1>
DialogCallback1<DERIVED, BASE, T1>::DialogCallback1(FunctorType inFunctor)
    : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1>
DialogCallback1<DERIVED, BASE, T1>::~DialogCallback1()
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1> void
DialogCallback1<DERIVED, BASE, T1>::operator () (const BASE& inBase, T1 a1) const
{
    mFunctor(dynamic_cast<const DERIVED&>(inBase), a1);
}


// ==========================================================================================
//  DialogCallback2

template <class DERIVED, class BASE, typename T1, typename T2>
class DialogCallback2 : public DialogCallbackBase2<BASE, T1, T2>
{
public:
    
    typedef boost::function3<void, const DERIVED&, T1, T2>  FunctorType;
    
                    DialogCallback2(FunctorType inFunctor);
    virtual         ~DialogCallback2();
    virtual void    operator () (const BASE&, T1, T2) const;

private:
    
    FunctorType mFunctor;
};

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2>
DialogCallback2<DERIVED, BASE, T1, T2>::DialogCallback2(FunctorType inFunctor)
    : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2>
DialogCallback2<DERIVED, BASE, T1, T2>::~DialogCallback2()
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2> void
DialogCallback2<DERIVED, BASE, T1, T2>::operator () (const BASE& inBase, T1 a1, T2 a2) const
{
    mFunctor(dynamic_cast<const DERIVED&>(inBase), a1, a2);
}


// ==========================================================================================
//  DialogCallback3

template <class DERIVED, class BASE, typename T1, typename T2, typename T3>
class DialogCallback3 : public DialogCallbackBase3<BASE, T1, T2, T3>
{
public:
    
    typedef boost::function4<void, const DERIVED&, T1, T2, T3>  FunctorType;
    
                    DialogCallback3(FunctorType inFunctor);
    virtual         ~DialogCallback3();
    virtual void    operator () (const BASE&, T1, T2, T3) const;

private:
    
    FunctorType mFunctor;
};

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3>
DialogCallback3<DERIVED, BASE, T1, T2, T3>::DialogCallback3(FunctorType inFunctor)
    : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3>
DialogCallback3<DERIVED, BASE, T1, T2, T3>::~DialogCallback3()
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3> void
DialogCallback3<DERIVED, BASE, T1, T2, T3>::operator () (const BASE& inBase, T1 a1, T2 a2, T3 a3) const
{
    mFunctor(dynamic_cast<const DERIVED&>(inBase), a1, a2, a3);
}


// ==========================================================================================
//  DialogCallback4

template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4>
class DialogCallback4 : public DialogCallbackBase4<BASE, T1, T2, T3, T4>
{
public:
    
    typedef boost::function5<void, const DERIVED&, T1, T2, T3, T4>  FunctorType;
    
                    DialogCallback4(FunctorType inFunctor);
    virtual         ~DialogCallback4();
    virtual void    operator () (const BASE&, T1, T2, T3, T4) const;

private:
    
    FunctorType mFunctor;
};

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4>
DialogCallback4<DERIVED, BASE, T1, T2, T3, T4>::DialogCallback4(FunctorType inFunctor)
    : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4>
DialogCallback4<DERIVED, BASE, T1, T2, T3, T4>::~DialogCallback4()
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4> void
DialogCallback4<DERIVED, BASE, T1, T2, T3, T4>::operator () (const BASE& inBase, T1 a1, T2 a2, T3 a3, T4 a4) const
{
    mFunctor(dynamic_cast<const DERIVED&>(inBase), a1, a2, a3, a4);
}


// ==========================================================================================
//  DialogCallback5

template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4, typename T5>
class DialogCallback5 : public DialogCallbackBase5<BASE, T1, T2, T3, T4, T5>
{
public:
    
    typedef boost::function6<void, const DERIVED&, T1, T2, T3, T4, T5>  FunctorType;
    
                    DialogCallback5(FunctorType inFunctor);
    virtual         ~DialogCallback5();
    virtual void    operator () (const BASE&, T1, T2, T3, T4, T5) const;

private:
    
    FunctorType mFunctor;
};

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4, typename T5>
DialogCallback5<DERIVED, BASE, T1, T2, T3, T4, T5>::DialogCallback5(FunctorType inFunctor)
    : mFunctor(inFunctor)
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4, typename T5>
DialogCallback5<DERIVED, BASE, T1, T2, T3, T4, T5>::~DialogCallback5()
{
}

// ------------------------------------------------------------------------------------------
template <class DERIVED, class BASE, typename T1, typename T2, typename T3, typename T4, typename T5> void
DialogCallback5<DERIVED, BASE, T1, T2, T3, T4, T5>::operator () (const BASE& inBase, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) const
{
    mFunctor(dynamic_cast<const DERIVED&>(inBase), a1, a2, a3, a4, a5);
}

#endif  // !DOXYGEN_SKIP


}   // namespace B


#endif  // BDialogCallback_H_
