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

#ifndef BViewFactory_H_
#define BViewFactory_H_

#pragma once

// standard headers
#include <map>

// system headers
#include <Carbon/Carbon.h>

// B headers
#include "BErrorHandler.h"
#include "BViewUtils.h"


namespace B {

// forward declarations
class   View;


/*! @brief  Instantiates subclasses of View that match a given <tt>HIViewRef</tt>.
    
    ViewFactory manages a set of triples <i>\<ControlKind, HIViewID, 
    FactoryProc\></i>.  @c ControlKind identifies the type of view, eg push button, 
    scroll bar, etc.  @c HIViewID identifies a particular view within a window.  
    Finally, FactoryProc is a function that instantiates a C/C++ object (derived from 
    View) appropriate for the @c ControlKind / @c HIViewID pair.
    
    ViewFactory maintains a singleton instance of itself, called the default 
    view factory.  It contains application-wide triples.  This allows triples for a 
    specific context (such as a particular window) to be isolated from more general-
    purpose triples.
    
    A ViewFactory is initialised by calling its Register() member functions.  They 
    construct the collection of triples.  Once initialised, View objects are 
    instantiated by calling the Instantiate() member function.
    
    Instantiate() works by attempting to find a match in its collection of triples for 
    the @c HIViewRef it's given as an argument.  It starts by looking for a complete match 
    on the @c HIViewRef's @c ControlKind and @c HIViewID.  If no match is found, it looks 
    for a partial match on @c ControlKind and @c HIViewID.id.  If no match is found, it 
    looks for a partial match on @c ControlKind only.  If there is still no match, the 
    process is repeated with the default view factory.  At the end of all this, if there 
    was a match, then the matched triple's FactoryProc is called to instantiate the view 
    object;  else, a default function is called instead.
    
    @ingroup    ViewGroup
*/
class ViewFactory : public boost::noncopyable
{
public:
    
    //! @name Types
    //@{
    /*! @brief  The factory function prototype.
        
        The function takes an existing HIToolbox view (aka @c HIViewRef) and instantiates 
        a subclass of View tied to it.
        
        @param  inViewRef   The @c HIViewRef.
        @return             The newly instantiated View.  Never returns @c NULL (an exception is thrown if something goes wrong).
    */
    typedef View*   (*FactoryProc)(HIViewRef inViewRef);
    //@}
    
    //! @name Default Factory
    //@{
    //! Returns the singleton default view factory.
    static ViewFactory& Default();
    //@}
    
    //! @name Constructor / Destructor
    //@{
    //! Default constructor.
    ViewFactory();
    //@}
    
    //! @name Inquiries
    //@{
    //! Is this the default view factory?
    bool    IsDefault() const   { return (mDefault); }
    //@}
    
    //! @name Registration
    //@{
    //! Registers the class @a VIEW for @c HIViewRefs with the given class ID and @a inID.
    template <class VIEW> void  Register(
                CFStringRef     inClassID, 
                SInt32          inID);
    //! Registers the class @a VIEW for @c HIViewRefs with the given class ID and any id.
    template <class VIEW> void  Register(
                CFStringRef     inClassID);
    //! Registers @a inProc for @c HIViewRefs identified by @a inClassID and @a inID.
    void    Register(
                CFStringRef     inClassID, 
                const HIViewID& inID,
                FactoryProc     inProc);
    //! Unregisters @c HIViewRefs identified by @a inClassID and @a inID.
    void    Unregister(
                CFStringRef     inClassID, 
                const HIViewID& inID);
    //@}
    
    //! @name Instantiation
    //@{
    //! Creates a View object tied to @a inViewRef.
    View*   Instantiate(
                HIViewRef   inViewRef) const;
    //@}
    
    //! @name Defaults
    //@{
    //! Returns the default signature.
    static OSType       GetDefaultSignature();
    //! Sets the default signature.
    static void         SetDefaultSignature(OSType inSig);
    //! Returns the default factory function.
    static FactoryProc  GetDefaultFactoryProc();
    //! Sets the default factory function.
    static void         SetDefaultFactoryProc(FactoryProc inProc);
    //@}
    
private:
    
    // default factory constructor
    ViewFactory(bool);
    
    FactoryProc FindMatch(
                    CFStringRef         inClassID, 
                    const HIViewID&     inID) const;
    static View*    DefaultFactoryProc(HIViewRef inViewRef);
    
    struct Key
    {
        Key(CFStringRef inClassId, const HIViewID& inViewId)
            : classId(inClassId), viewId(inViewId) {}

        Key(const Key& inKey)
            : classId(inKey.classId), viewId(inKey.viewId) {}
        
        const CFStringRef   classId;
        const HIViewID      viewId;
    };
    
    struct KeyCompare
    {
        bool    operator()(const Key& x, const Key& y) const;
    };
    
    typedef std::map<Key, FactoryProc, KeyCompare>  FactoryMap;
    
    // member variables
    FactoryMap  mMap;
    const bool  mDefault;
    
    // static member variables
    static FactoryProc  sDefaultProc;
    static OSType       sDefaultIDSig;
};

// ------------------------------------------------------------------------------------------
/*! @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    
    Use this function when registering a View subclass for a specific view (which is 
    usually, but not necessarily, in a specific window).
    
    Because this function uses the default signature, it needs to have been set prior to 
    calling this function.
    
    Class @a VIEW needs the following members:
    
    @code
        // The factory function.
        static View*    Instantiate(HIViewRef inViewRef);
    @endcode
*/
template <class VIEW> inline void
ViewFactory::Register(
    CFStringRef inClassId,  //!< The view's class ID.
    SInt32      inViewId)   //!< The view's ID.
{
    B_ASSERT(sDefaultIDSig != 0);
    
    HIViewID    viewId  = { sDefaultIDSig, inViewId };
    
    Register(inClassId, viewId, &VIEW::Instantiate);
}

// ------------------------------------------------------------------------------------------
/*! @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    
    Use this function when registering a View subclass for all @c HIViewRefs matching 
    @a VIEW's class ID.
    
    Class @a VIEW needs the following members:
    
    @code
        // The factory function.
        static View*    Instantiate(HIViewRef inViewRef);
    @endcode
*/
template <class VIEW> inline void
ViewFactory::Register(
    CFStringRef inClassId)  //!< The view's class ID.
{
    HIViewID    viewId  = { 0, 0 };
    
    Register(inClassId, viewId, &VIEW::Instantiate);
}

}   // namespace B


#endif  // BViewFactory_H_
