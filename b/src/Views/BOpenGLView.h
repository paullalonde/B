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

#ifndef BOpenGLView_H_
#define BOpenGLView_H_

#pragma once

// system headers
#include <AGL/agl.h>

// B headers
#include "BAutoUPP.h"
#include "BCustomView.h"
#include "BOpenGLUtilities.h"
#include "BQuickdrawUtilities.h"
#include "BViewData.h"

/*
    kEventClassB / kBOpenGLViewDraw
    
    The contents of the OpenGL view need to be drawn.  The correct AGL context 
    is already current and may be obtained via @c aglGetCurrentContext().
*/
enum {
  kBOpenGLViewDraw  = 100
};

namespace B {


/*! @brief  A custom view implementing a OpenGl view.
    
    @todo       %Document this class!
    @ingroup    ViewGroup
*/
class OpenGLView : public CustomView
{
public:
    
    // constants
    enum    { kControlKindSig       = kEventClassB, 
              kControlKind          = FOUR_CHAR_CODE('OpGL') };
    enum    {
//            kPixelFormatTag       = FOUR_CHAR_CODE('PixF'),
              kContextTag           = FOUR_CHAR_CODE('Ctxt')
            };
    
    // instantiation
    static EventTarget* Instantiate(HIObjectRef inObjectRef);
    
    static const CFStringRef    kHIObjectClassID;
    
protected:
    
    // constructor
                    OpenGLView(
                        HIObjectRef     inObjectRef, 
                        EViewFlags      inViewFlags);
    
    virtual const ViewPartVector&
                    GetViewParts() const;
    virtual ControlKind
                    GetKind() const;
    virtual void    BuildGL(
                        AGLContext      inContext);
    virtual void    DisposeGL(
                        AGLContext      inContext);
    virtual void    DrawGL();
    virtual void    DisplayChanged();
                    
    // CarbonEvent handlers
    virtual bool    SetData(
                        ControlPartCode inPart, 
                        ResType         inDataTag, 
                        const void*     inDataPtr, 
                        size_t          inDataSize);
    virtual bool    GetData(
                        ControlPartCode inPart, 
                        ResType         inDataTag, 
                        void*           inDataPtr, 
                        size_t&         ioDataSize) const;
    virtual bool    BoundsChanged(
                        UInt32          inAttributes, 
                        const Rect&     inPreviousBounds, 
                        const Rect&     inCurrentBounds);
//  virtual void    VisibilityChanged();
    virtual void    OwningWindowChanged(
                        WindowRef       inOriginalWindow, 
                        WindowRef       inCurrentWindow);
//  virtual void    GetMinimumSize(
//                      Size&       outMinimumSize) const;
//  virtual void    GetMaximumSize(
//                      Size&       outMaximumSize) const;
    virtual void    AddedSubview(
                        HIViewRef       inSubview);
    
    // overrides from EventTarget
    virtual void    PrintDebugInfo();
    
private:
    
    // types
    
    template <ResType TAG> struct Data : public ViewData<kControlKindSig, kControlKind, TAG> {};
    
    virtual void    Draw(
                        HIViewPartCode  inPart, 
                        CGContextRef    inContext, 
                        const Shape*    inDrawShape);
    
    void    InitEventHandler();
    void    SetDrawable();
//  void    ResizeDrawable(const Rect& inBounds);
    
    bool    BOpenGLViewDraw(
                Event<kEventClassB, kBOpenGLViewDraw>&          event);
//  bool    WindowShown(
//              Event<kEventClassWindow, kEventWindowShown>&            event);
//  bool    WindowBoundsChanged(
//              Event<kEventClassWindow, kEventWindowBoundsChanged>&    event);

    // callbacks
    static pascal void
                    DisplayMgrNotificationProc(
                        void*           userData, 
                        short           message, 
                        void*           notifyData);
    
    // member variables
    EventHandler            mEventHandler;
    AutoDMExtendedNotify    mDMNotification;
    AutoAglContext          mContext;
    
    int imgCode;
};


#ifndef DOXYGEN_SKIP

//template <> struct ViewDataDef<
//  OpenGLView::kControlKindSig, 
//  OpenGLView::kControlKind, 
//  OpenGLView::kPixelFormatTag>    { typedef ViewDataTrait<AGLPixelFormat> Trait;  };

template <> struct ViewDataDef<
    OpenGLView::kControlKindSig, 
    OpenGLView::kControlKind, 
    OpenGLView::kContextTag>        { typedef ViewDataTrait<AGLContext>     Trait;  };

#endif  // DOXYGEN_SKIP

}   // namespace B


#endif  // BOpenGLView_H_
