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

#ifndef BWindow_H_
#define BWindow_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/concept_check.hpp>

// B headers
#include "BAEObject.h"
#include "BEventCustomParams.h"
#include "BEventHandler.h"
#include "BNib.h"
#include "BRect.h"
#include "BView.h"
#include "BViewFactory.h"


namespace B {


// forward declarations
class   CommandData;
class   EventBase;
class   Icon;
class   Nib;
class   Point;
class   Rect;
class   Size;
class   View;

// Window AE properties that aren't (yet) in AERegistry.h
enum {
    pIsMinimizable  = 'ismn',   // Can the window be minimized ?
    pIsMinimized    = 'pmnd'    // Is the window currently minimized ?
};


// ==========================================================================================
//  Window

#pragma mark Window

/*! @brief  A %Window Manager window
    
    @todo   %Document this class!
    
    @sa     @ref using_windows
*/
class Window : public AEObject
{
public:
    
    //! @name Constructors
    //@{
    //! Constructor.  In B, Window objects are created @e after their @c WindowRef.
protected:
                    Window(
                        WindowRef       inWindowRef, 
                        AEObjectPtr     inContainer, 
                        DescType        inClassID = cWindow);
    //@}
public:
    
    /*! @name Instantiating Windows and Views
        
        All of the functions in this group instantiate a @c WindowRef from a nib file, 
        and construct a new Window object (whose exact class is @a WINDOW) with it.  
        They only differ in the number of arguments passed to @a WINDOW's constructor.
    */
    //@{
    //! Create a window and its views from a nib.
    template <class WINDOW>
    static boost::shared_ptr<WINDOW>
                    CreateFromNib(
                        Nib&            inNib,
                        const char*     inWindowName, 
                        AEObjectPtr     inContainer);
    //! Create a window and its views from a nib.
    template <class WINDOW, typename STATE>
    static boost::shared_ptr<WINDOW>
                    CreateFromNibWithState(
                        Nib&            inNib,
                        const char*     inWindowName, 
                        AEObjectPtr     inContainer,
                        STATE           inState);
    /*! @overload
        @note   The window's AEOM container is assumed to be implicit in @a inState.
    */
    template <class WINDOW, typename STATE>
    static boost::shared_ptr<WINDOW>
                    CreateFromNibWithState(
                        Nib&            inNib,
                        const char*     inWindowName, 
                        STATE           inState);
    //@}
    
    // enumerating windows
    static Window*  FromWindowRef(
                        WindowRef       inWindowRef);
    static SInt32   CountWindowsOfClass(
                        DescType        inClassID);
    static Window*  GetWindowOfClassByIndex(
                        DescType        inClassID, 
                        unsigned        inIndex);
    static Window*  GetWindowOfClassByName(
                        DescType        inClassID, 
                        const String&   inName);
    static Window*  GetWindowOfClassByUniqueID(
                        DescType        inClassID, 
                        SInt32          inUniqueID);
                    
    void            Select();
    void            Show(bool showIt);
    void            Transition(
                        WindowTransitionEffect  inEffect, 
                        WindowTransitionAction  inAction, 
                        Rect*                   inRect = NULL, 
                        Window*                 inParent = NULL);
    void            Reposition(
                        WindowPositionMethod    inMethod, 
                        Window*                 inParent = NULL);
    virtual void    Close();
    
    //! @name Registering Views
    //@{
    //! Registers the class @a VIEW for @c HIViewRefs with the given class ID and @a inID.
    template <class VIEW> void  Register(
                CFStringRef     inClassId, 
                SInt32          inID);
    //! Registers the class @a VIEW for @c HIViewRefs with the given class ID and any id.
    template <class VIEW> void  Register(
                CFStringRef     inClassId);
    //@}
    
    //! @name Finding Views
    //@{
    //! Returns the view matching the main bundle's signature and @a inID.
    template <class VIEW>
    VIEW*       FindView(
                    int     inID) const;
    /*! @overload
    */
    template <class VIEW>
    VIEW*       FindView(
                    OSType  inSignature, 
                    int     inID) const;
    //! Returns the view matching the main bundle's signature and @a inID, or @c NULL.
    template <class VIEW>
    VIEW*       FindView(
                    int     inID, 
                    const std::nothrow_t&) const;
    /*! @overload
    */
    template <class VIEW>
    VIEW*       FindView(
                    OSType  inSignature, 
                    int     inID, 
                    const std::nothrow_t&) const;
    //@}
    
    // UI level
    virtual void    CloseWindow(
                        OSType      inSaveOption = kAEAsk, 
                        const Url&  inUrl = Url());
    
    // inquiries
    bool            IsComposited() const    { return (mComposited); }
    bool            IsVisible() const       { return (IsWindowVisible(mWindowRef)); }
    bool            IsZoomable() const;
    bool            IsZoomed() const;
    bool            IsCollapseable() const;
    bool            IsCollapsed() const;
    bool            IsCloseable() const;
    bool            IsResizeable() const;
    bool            IsFloating() const;
    bool            IsModal() const;
    String          GetName() const;
    HIViewRef       GetContentView() const  { return (mContentView); }
    
    // conversions
    //! Returns the object's underlying @c WindowRef.
    operator        WindowRef () const      { return (mWindowRef); }
    //! Returns the object's underlying @c WindowRef.
    WindowRef       GetWindowRef() const    { return (mWindowRef); }
    //! Returns the object's underlying @c HIObjectRef.
    HIObjectRef     GetObjectRef() const    { return (reinterpret_cast<HIObjectRef>(mWindowRef)); }
    //! Retrieves the underlying @c EventTargetRef.
    EventTargetRef  GetEventTarget() const  { return (HIObjectGetEventTarget(GetObjectRef())); }
    
    // modifiers
    void            SetMinimumSize(
                        const Size&     inMinimumSize);
    void            SetMaximumSize(
                        const Size&     inMaximumSize);
    void            SetIdealSize(
                        const Size&     inIdealSize);
    void            SetTitle(
                        const String&   inTitle);
    void            SetAlternateTitle(
                        const String&   inTitle);
    void            SetProxy(
                        const Icon&     inIcon);
    void            SetProxy(
                        const Url&      inUrl);
    void            SetProxy(
                        OSType          inCreator, 
                        OSType          inFileType);
    void            ClearProxy();
    void            SetModified(
                        bool            inModified);
    void            Zoom(
                        bool            inZoomItOut);
    void            Collapse(
                        bool            inCollapse);
    
    // size & location
    Rect            GetContentViewBounds() const;
    Rect            GetStructureBounds() const;
    Point           GetStructureOrigin() const;
    Size            GetStructureSize() const;
    Rect            GetContentBounds() const;
    Point           GetContentOrigin() const;
    Size            GetContentSize() const;
    void            SetStructureBounds(
                        const Rect&     inBounds);
    void            MoveStructureTo(
                        const Point&    inOrigin);
    void            MoveStructureBy(
                        float           inDeltaX, 
                        float           inDeltaY);
    void            ResizeStructureTo(
                        const Size&     inSize);
    void            ResizeStructureBy(
                        float           inDeltaWidth, 
                        float           inDeltaHeight);
    void            SetContentBounds(
                        const Rect&     inBounds);
    void            ResizeContentTo(
                        const Size&     inSize);
    void            ResizeContentBy(
                        float           inDeltaWidth, 
                        float           inDeltaHeight);
    void            MoveContentTo(
                        const Point&    inOrigin);
    void            MoveContentBy(
                        float           inDeltaX, 
                        float           inDeltaY);
    
    // toolbars
    void            SetToolbar(OSPtr<HIToolbarRef> inToolbar);
    void            ShowToolbar(bool inShowIt, bool inAnimate);
    bool            IsToolbarVisible() const;
    
    // window list
    void            AddToWindowList();
    void            RemoveFromWindowList();
    bool            IsInWindowList() const;
    
    // overrides from AEObject
    virtual void    WriteProperty(
                        DescType        inPropertyID, 
                        AEWriter&       ioWriter) const;
    virtual void    ReadProperty(
                        DescType        inPropertyID, 
                        AEReader&       ioReader);
    virtual void    CloseObject(
                        OSType          inSaveOption = kAEAsk, 
                        const Url&      inFileUrl = Url());
    virtual void    ActivateObject();
    
    // debugging
    virtual void    DebugPrint() const;
    
protected:
    
    /*! @name Object Destruction
        
        Window's destructor and operator delete are protected in order to minimise 
        the risk of a client trying to (1) create a Window on the stack, or (2) trying to 
        explicitly delete a Window instead of calling Close() (or equivalently, 
        @a DisposeWindow on its @c WindowRef).
        
        Derived classes should also make their destructors protected.
    */
    //@{
    //! Destructor.
    virtual         ~Window();
    //! Operator delete.
    void            operator delete (void* p);
    //@}
    
    void            PostCreateWindow(Nib* inFromNib);
    virtual void    RegisterViews();
    View*           Instantiate(HIViewRef inViewRef) const;
    virtual bool    ShouldWindowClose();
    virtual Size    GetIdealSize() const;
    
    // Carbon %Event handlers
    virtual void    HandleShowing();
    virtual void    HandleHiding();
    virtual void    HandleShown();
    virtual void    HandleHidden();
    virtual void    HandleActivated();
    virtual void    HandleDeactivated();
    virtual bool    HandleFocusAcquired();
    virtual bool    HandleFocusRelinquish();
    virtual bool    HandleZoomed();
    virtual bool    HandleResizeCompleted();
    virtual bool    HandleDragCompleted();
    virtual bool    HandleTransitionCompleted(void* inUserData);
    virtual bool    HandleCollapsed();
    virtual bool    HandleExpanded();
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        CommandData&                ioCmdData);
    virtual void    HandleSheetOpened(
                        WindowRef   inSheetWindowRef);
    virtual void    HandleSheetClosed(
                        WindowRef   inSheetWindowRef);
    
    // overrides from AEObject
    virtual void    MakeSpecifier(
                        AEWriter&       ioWriter) const;
    
    struct WindowDeleter
    {
        void operator () (Window* window) const;
    };
    
private:
    
    //! @name Private Overridables
    //@{
    //! The window has just been loaded.
    virtual void    Awaken(Nib* inFromNib);
    //@}
    
//  void    operator delete (void* p);
    
    void    InitEventHandler();
    void    InstantiateViews();
    void    AwakenViews(Nib* inFromNib);
    void    RemoveFromWindowList(const std::nothrow_t&);
    
    // Carbon %Event handlers
    bool    WindowShowing(
                Event<kEventClassWindow, kEventWindowShowing>&          event);
    bool    WindowHiding(
                Event<kEventClassWindow, kEventWindowHiding>&           event);
    bool    WindowShown(
                Event<kEventClassWindow, kEventWindowShown>&            event);
    bool    WindowHidden(
                Event<kEventClassWindow, kEventWindowHidden>&           event);
    bool    WindowActivated(
                Event<kEventClassWindow, kEventWindowActivated>&        event);
    bool    WindowDeactivated(
                Event<kEventClassWindow, kEventWindowDeactivated>&      event);
    bool    WindowZoomed(
                Event<kEventClassWindow, kEventWindowZoomed>&           event);
    bool    WindowResizeCompleted(
                Event<kEventClassWindow, kEventWindowResizeCompleted>&  event);
    bool    WindowDragCompleted(
                Event<kEventClassWindow, kEventWindowDragCompleted>&    event);
    bool    WindowTransitionCompleted(
                Event<kEventClassWindow, kEventWindowTransitionCompleted>&  event);
    bool    WindowCollapsed(
                Event<kEventClassWindow, kEventWindowCollapsed>&        event);
    bool    WindowExpanded(
                Event<kEventClassWindow, kEventWindowExpanded>&         event);
    bool    WindowClose(    
                Event<kEventClassWindow, kEventWindowClose>&            event);
    bool    WindowClosed(
                Event<kEventClassWindow, kEventWindowClosed>&           event);
    bool    WindowGetIdealSize(
                Event<kEventClassWindow, kEventWindowGetIdealSize>&     event);
    bool    WindowFocusAcquired(
                Event<kEventClassWindow, kEventWindowFocusAcquired>&    event);
    bool    WindowFocusRelinquish(
                Event<kEventClassWindow, kEventWindowFocusRelinquish>&  event);
    bool    WindowSheetOpened(
                Event<kEventClassWindow, kEventWindowSheetOpened>&      event);
    bool    WindowSheetClosed(
                Event<kEventClassWindow, kEventWindowSheetClosed>&      event);
    bool    CommandProcess(
                Event<kEventClassCommand, kEventCommandProcess>&        event);
    bool    CommandUpdateStatus(
                Event<kEventClassCommand, kEventCommandUpdateStatus>&   event);
    
    // member variables
    WindowRef       mWindowRef;
    EventHandler    mEventHandler;
    ViewFactory     mViewFactory;
    HIViewRef       mContentView;
    Size            mIdealSize;
    const bool      mComposited;
    bool            mHandlingAECollapse;
    bool            mHandlingAEZoom;
    
    static std::list< boost::shared_ptr<Window> >   sWindows;
};

// ------------------------------------------------------------------------------------------
/*! Instantiates a window of class @a WINDOW, from the window definition @a inWindowName in 
    nib file @a inNib.
    
    The function result type needs some explaining.  It's an @c std::pair, with the 
    first element holding a smart pointer to a @c WindowRef, and the second holding a 
    pointer to the @a WINDOW object.  The reason for this is that the window's lifetime is 
    controlled by the smart pointer;  one can't destroy a window object by calling 
    @c operator @c delete on it.  So if an exception occurs further up the call 
    chain, the window will be disposed properly when the smart pointer is destructed.
    Obviously, the second part of the pair (the @a WINDOW pointer) is there for convenient 
    type-safe access to the window object.
    
    @param  WINDOW  Template parameter.  The C/C++ class of the instantiated window.  Must 
                    be Window or a class derived from Window.
*/
template <class WINDOW>
boost::shared_ptr<WINDOW>
Window::CreateFromNib(
    Nib&        inNib,          //!< The nib file from which to read the window.
    const char* inWindowName,   //!< The window's name in the nib file.
    AEObjectPtr inContainer)    //!< Some state that is passed to @a WINDOW's constructor.
{
    boost::function_requires< boost::ConvertibleConcept<WINDOW*, B::Window*> >();
    
    // Read in the window from the nib file.
    OSPtr<WindowRef>    windowRef(inNib.CreateWindow(inWindowName));
    
    // Instantiate the window object.
    boost::shared_ptr<WINDOW>   windowPtr(new WINDOW(windowRef, inContainer), 
                                          WindowDeleter());
    
    // Finish initialising the window object.
    windowPtr->PostCreateWindow(&inNib);
    
    return (windowPtr);
}

// ------------------------------------------------------------------------------------------
/*! This function is identical to CreateFromNib(), except that the @a WINDOW constructor 
    will be passed an object of type @a STATE.  This can be used to communicate information 
    between the window and the code that instantiates it.
    
    @param  WINDOW  Template parameter.  The C/C++ class of the instantiated window.  Must 
                    be Window or a class derived from Window.
    @param  STATE   Template parameter.  Any C/C++ type.  It is passed to @a WINDOW's 
                    constructor but is otherwise uninterpreted.
    
    @sa CreateFromNib()
*/
template <class WINDOW, typename STATE>
boost::shared_ptr<WINDOW>
Window::CreateFromNibWithState(
    Nib&        inNib,          //!< The nib file from which to read the window.
    const char* inWindowName,   //!< The window's name in the nib file.
    AEObjectPtr inContainer,    //!< The AEOM container of the window.  This will usually be the application or a document.
    STATE       inState)        //!< Some state that is passed to @a WINDOW's constructor.
{
    boost::function_requires< boost::ConvertibleConcept<WINDOW*, B::Window*> >();
    boost::function_requires< boost::CopyConstructibleConcept<STATE> >();
    
    // Read in the window from the nib file.
    OSPtr<WindowRef>    windowRef(inNib.CreateWindow(inWindowName));
    
    // Instantiate the window object.
    boost::shared_ptr<WINDOW>   windowPtr(new WINDOW(windowRef, inContainer, inState),
                                          WindowDeleter());
    
    // Finish initialising the window object.
    windowPtr->PostCreateWindow(&inNib);
    
    return (windowPtr);
}

// ------------------------------------------------------------------------------------------
/*! This function is identical to CreateFromNib(), except that the @a WINDOW constructor 
    will be passed an object of type @a STATE instead of an AEObject.  This can be used to 
    communicate information between the window and the code that instantiates it.
    
    @param  WINDOW  Template parameter.  The C/C++ class of the instantiated window.  Must 
                    be Window or a class derived from Window.
    @param  STATE   Template parameter.  Any C/C++ type.  It is passed to @a WINDOW's 
                    constructor but is otherwise uninterpreted.
    
    @sa CreateFromNib()
*/
template <class WINDOW, typename STATE>
boost::shared_ptr<WINDOW>
Window::CreateFromNibWithState(
    Nib&        inNib,          //!< The nib file from which to read the window.
    const char* inWindowName,   //!< The window's name in the nib file.
    STATE       inState)        //!< Some state that is passed to @a WINDOW's constructor.
{
    boost::function_requires< boost::ConvertibleConcept<WINDOW*, B::Window*> >();
    boost::function_requires< boost::CopyConstructibleConcept<STATE> >();
    
    // Read in the window from the nib file.
    OSPtr<WindowRef>    windowRef(inNib.CreateWindow(inWindowName));
    
    // Instantiate the window object.
    boost::shared_ptr<WINDOW>   windowPtr(new WINDOW(windowRef, inState), 
                                          WindowDeleter());
    
    // Finish initialising the window object.
    windowPtr->PostCreateWindow(&inNib);
    
    return (windowPtr);
}

// ------------------------------------------------------------------------------------------
/*! @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    
    Use this function when registering a View subclass for a specific view in the window.
    
    @sa ViewFactory::Register(CFStringRef, SInt32)
*/
template <class VIEW> inline void
Window::Register(
    CFStringRef inClassId,  //!< The view's class ID.
    SInt32      inViewId)   //!< The view's ID.
{
    mViewFactory.Register<VIEW>(inClassId, inViewId);
}

// ------------------------------------------------------------------------------------------
/*! @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    
    Use this function when registering a View subclass for all @c HIViewRefs in this window 
    that match the given class ID.
    
    @sa ViewFactory::Register(CFStringRef)
*/
template <class VIEW> inline void
Window::Register(
    CFStringRef inClassId)  //!< The view's class ID.
{
    mViewFactory.Register<VIEW>(inClassId);
}

// ------------------------------------------------------------------------------------------
/*! If the view is found, it is downcast to type @a VIEW.
    
    @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    @return         The requested view.  Never returns @c NULL (an exception is thrown if something goes wrong).
    
    @note   If the caller doesn't care about the specific type of the returned view, View 
            should be used as the template parameter, eg <tt>FindView<B::View>(...)</tt>.
*/
template <class VIEW> VIEW*
Window::FindView(
    int     inID)           //!< The view's ID.
    const
{
    return (View::FindTypedSubview<VIEW>(HIViewGetRoot(mWindowRef), inID));
}

// ------------------------------------------------------------------------------------------
/*! If the view is found, it is downcast to type @a VIEW.
    
    @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    @return         The requested view.  Never returns @c NULL (an exception is thrown if something goes wrong).
    
    @note   If the caller doesn't care about the specific type of the returned view, View 
            should be used as the template parameter, eg <tt>FindView<B::View>(...)</tt>.
*/
template <class VIEW> VIEW*
Window::FindView(
    OSType  inSignature,    //!< The view's signature.
    int     inID)           //!< The view's ID.
    const
{
    HIViewID    viewID  = { inSignature, inID };
    
    return (View::FindTypedSubview<VIEW>(HIViewGetRoot(mWindowRef), viewID));
}

// ------------------------------------------------------------------------------------------
/*! If the view is found, it is downcast to type @a VIEW.
    
    @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    @return         The requested view, or @c NULL if the view wasn't found.
    
    @note   If the caller doesn't care about the specific type of the returned view, View 
            should be used as the template parameter, eg <tt>FindView<B::View>(...)</tt>.
    
    @note   Even though the function takes an @c std::nothrow_t argument, that only applies 
            to the case where a view matching @a inID isn't found.  If 
            the view @e is found but the downcast fails, an exception will be thrown.
*/
template <class VIEW> VIEW*
Window::FindView(
    int                     inID,   //!< The view's ID.
    const std::nothrow_t&   nt)     //!< An indication that the caller doesn't want the function to throw.
    const
{
    HIViewRef   viewRef     = View::FindSubview(inID, nt);
    VIEW*       typedView   = NULL;
    
    if (viewRef != NULL)
        typedView = View::GetTypedViewFromRef<VIEW>(viewRef);
    
    return (typedView);
}

// ------------------------------------------------------------------------------------------
/*! If the view is found, it is downcast to type @a VIEW.
    
    @param  VIEW    Template parameter.  The C/C++ class of the view object.  Must be View or a class derived from View.
    @return         The requested view, or @c NULL if the view wasn't found.
    
    @note   If the caller doesn't care about the specific type of the returned view, View 
            should be used as the template parameter, eg <tt>FindView<B::View>(...)</tt>.
    
    @note   Even though the function takes an @c std::nothrow_t argument, that only applies 
            to the case where a view matching @a inSignature and @a inID isn't found.  If 
            the view @e is found but the downcast fails, an exception will be thrown.
*/
template <class VIEW> VIEW*
Window::FindView(
    OSType                  inSignature,    //!< The view's signature.
    int                     inID,           //!< The view's ID.
    const std::nothrow_t&   nt)             //!< An indication that the caller doesn't want the function to throw.
    const
{
    HIViewID    viewID      = { inSignature, inID };
    HIViewRef   viewRef     = View::FindSubview(viewID, nt);
    VIEW*       typedView   = NULL;
    
    if (viewRef != NULL)
        typedView = View::GetTypedViewFromRef<VIEW>(viewRef);
    
    return (typedView);
}


// ==========================================================================================
//  AutoWindow

#pragma mark -
#pragma mark AutoWindow

/*!
    @brief  Manages a "stack-based" window.
    
    AutoWindow's constructor instantiates a window, and its destructor 
    disposes of it.  It is most useful for displaying modal dialogs. 
    For example:
    
    @code
    void foo(Nib& nib, AEObject* container)
    {
        AutoWindow<B::Window> autoWin(nib, "my window", container);
        
        // Do stuff here.  The window gets disposed when we leave 
        // this scope.
    }
    @endcode
*/
template <class WINDOW> class AutoWindow : boost::noncopyable
{
public:
    
#ifndef DOXYGEN_SKIP
    // Concept checks.
    typedef WINDOW* TemplatePtrType;
    typedef Window* WindowPtrType;
    BOOST_CLASS_REQUIRE2(TemplatePtrType, WindowPtrType, boost, ConvertibleConcept);
#endif
    
#if DOXYGEN_SCAN
    //! @name Template Parameters
    //@{
    typedef typename    WINDOW; //!< Template parameter.  A C/C++ class that derives from B::Window.
    //@}
#endif

    //! @name Constructors / Destructor
    //@{
    //! @c WINDOW* constructor.
            AutoWindow(
                WINDOW*     inWindow);
    //! The read-from-nib constructor.
            AutoWindow(
                Nib&        inNib,
                const char* inWindowName,
                AEObject*   inContainer);
    //! The read-from-nib-and-give-the-window-some-state constructor.
            template <typename STATE>
            AutoWindow(
                Nib&        inNib,
                const char* inWindowName,
                AEObject*   inContainer,
                STATE       inState);
    //! A slightly simpler read-from-nib-and-give-the-window-some-state constructor.
            template <typename STATE>
            AutoWindow(
                Nib&        inNib,
                const char* inWindowName,
                STATE       inState);
    //! Destructor.
            ~AutoWindow();
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns the embedded Window object.
    WINDOW& get()       { return (*mWindow); }
    //@}
    
private:
    
    // member variables
    WINDOW* mWindow;
};

// ------------------------------------------------------------------------------------------
/*! Use this constructor when the Window object has been instantiated beforehand.
*/
template <class WINDOW>
AutoWindow<WINDOW>::AutoWindow(
    WINDOW*     inWindow)       //!< The window to manage.  May be @c NULL.
        : mWindow(inWindow)
{
}

// ------------------------------------------------------------------------------------------
/*! Use this constructor to instantiate a window from a nib and hand over its ownership 
    to this object, all in one go.  The @a WINDOW constructor will be passed an object of 
    type @a STATE.  This can be used to communicate information between the window and 
    the code that instantiates it.
    
    @param  STATE   Any C/C++ type.  It is passed to @a WINDOW's constructor but is otherwise uninterpreted.
*/
template <class WINDOW> template <typename STATE> 
AutoWindow<WINDOW>::AutoWindow(
    Nib&        inNib,          //!< The nib file from which to read the window.
    const char* inWindowName,   //!< The window's name in the nib file.
    AEObject*   inContainer,    //!< The AEOM container of the window.  This will usually be the application or a document.
    STATE       inState)        //!< Some state that is passed to @a WINDOW's constructor.
        : mWindow(Window::CreateFromNibWithState<WINDOW, STATE>(inNib, inWindowName, inContainer, inState))
{
}

// ------------------------------------------------------------------------------------------
/*! Use this constructor to instantiate a window from a nib and hand over its ownership 
    to this object, all in one go.  The @a WINDOW constructor will be passed an object of 
    type @a STATE.  This can be used to communicate information between the window and 
    the code that instantiates it.
    
    @param  STATE       Any C/C++ type.  It is passed to @a WINDOW's constructor but is otherwise uninterpreted.
*/
template <class WINDOW> template <typename STATE> 
AutoWindow<WINDOW>::AutoWindow(
    Nib&        inNib,          //!< The nib file from which to read the window.
    const char* inWindowName,   //!< The window's name in the nib file.
    STATE       inState)        //!< Some state that is passed to @a WINDOW's constructor.
        : mWindow(Window::CreateFromNibWithState<WINDOW, STATE>(inNib, inWindowName, inState))
{
}

// ------------------------------------------------------------------------------------------
/*! Use this constructor to instantiate a window from a nib and hand over its ownership 
    to this object, all in one go.
*/
template <class WINDOW>
AutoWindow<WINDOW>::AutoWindow(
    Nib&        inNib,          //!< The nib file from which to read the window.
    const char* inWindowName,   //!< The window's name in the nib file.
    AEObject*   inContainer)    //!< The AEOM container of the window.  This will usually be the application or a document.
        : mWindow(Window::CreateFromNib<WINDOW>(inNib, inWindowName, inContainer))
{
}

// ------------------------------------------------------------------------------------------
template <class WINDOW>
AutoWindow<WINDOW>::~AutoWindow()
{
    if (mWindow != NULL)
        mWindow->Close();
}

}   // namespace B


#endif  // BWindow_H_
