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
#include "BOpenGLView.h"

// B headers
#include "BEvent.h"
#include "BViewUtils.h"
#include "BWindowUtils.h"
#include "CFUtils.h"


#define VIEW_FLAGS  (kViewContainer)


namespace B {


static int  which   = 0;


const CFStringRef   OpenGLView::kHIObjectClassID        = 
                        CFSTR("ca.paullalonde.b.view.openglview");

// ------------------------------------------------------------------------------------------
EventTarget*
OpenGLView::Instantiate(HIObjectRef inObjectRef)
{
    return (new OpenGLView(inObjectRef, VIEW_FLAGS));
}

// ------------------------------------------------------------------------------------------
OpenGLView::OpenGLView(
    HIObjectRef     inObjectRef, 
    EViewFlags      inViewFlags)
        : CustomView(inObjectRef, inViewFlags), 
          mEventHandler(inObjectRef), 
          mDMNotification(DisplayMgrNotificationProc, reinterpret_cast<void*>(this))
{
    imgCode = which++;
    
    InitEventHandler();
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::InitEventHandler()
{
    mEventHandler.Add(this, &OpenGLView::BOpenGLViewDraw);
    
    mEventHandler.Init();
}

// ------------------------------------------------------------------------------------------
const CustomView::ViewPartVector&
OpenGLView::GetViewParts() const
{
    static ViewPartVector   sViewParts(1, ViewPart(kControlPicturePart, kFocusNever));
    
    return (sViewParts);
}

// ------------------------------------------------------------------------------------------
ControlKind
OpenGLView::GetKind() const
{
    ControlKind kind    = { kControlKindSig, kControlKind };
    
    return (kind);
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::Draw(
    ControlPartCode /* inPart */, 
    CGContextRef    /* inContext */, 
    const Shape*    /* inShape */)
{
    if (mContext.get() != NULL)
    {
        AutoSetAglContext                       savedContext(mContext);
        Event<kEventClassB, kBOpenGLViewDraw>   event;
        
        event.Send(GetViewRef());
        
        glFlush();
        //aglSwapBuffers(mContext);
    }
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::BuildGL(
    AGLContext      /* inContext */)
{
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::DisposeGL(
    AGLContext      /* inContext */)
{
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::DrawGL()
{
//printf("DrawGL\n"); fflush(stdout);
    
    if ((imgCode & 1) == 0)
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_POLYGON);
            glVertex3f(0.5, 0.1, 0.0);
            glVertex3f(0.5, 0.9, 0.0);
            glVertex3f(0.9, 0.1, 0.0);
        glEnd();
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_POLYGON);
            glVertex3f(0.5, 0.1, 0.0);
            glVertex3f(0.1, 0.1, 0.0);
            glVertex3f(0.1, 0.7, 0.0);
            glVertex3f(0.5, 0.7, 0.0);
        glEnd();
    //  glFlush();
    }
    else
    {
        glShadeModel (GL_SMOOTH);

        glEnable(GL_DEPTH_TEST);    // Hidden surface removal
        glEnable(GL_CULL_FACE); // Do not draw inside of cube
        glFrontFace(GL_CCW);    // Counter clock-wise polygons face out
        
        glClearColor (0.2f, 0.2f, 0.4f, 1.0f);  

        static GLfloat fRot [3] = { 0.0, 0.0, 0.0 };
//      static GLfloat fVel [3] = { 0.3, 0.1, 0.2 };
//      static GLfloat fAccel [3] = { 0.003, -0.005, 0.004 };
//      GLfloat fVMax = 2.0;
//      short i;
        // do velocities
//      if (gAnimate == true) {
//          for (i = 0; i < 3; i++)
//          {
//              fVel[i] += fAccel[i];
//              if (fVel[i] > fVMax)
//              {
//                  fAccel[i] *= -1.0;
//                  fVel[i] = fVMax;
//              }
//              else if (fVel[i] < -fVMax)
//              {
//                  fAccel[i] *= -1.0;
//                  fVel[i] = -fVMax;
//              }
//              fRot[i] += fVel[i];
//              while (fRot[i] > 360.0)
//                  fRot[i] -= 360.0;
//              while (fRot[i] < -360.0)
//                  fRot[i] += 360.0;
//          }
//      }
//      aglSetCurrentContext (aglContext);

        // Viewing transformation
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0, 0.0, -8.0);

        glRotatef (fRot[0], 1.0, 0.0, 0.0);
        glRotatef (fRot[1], 0.0, 1.0, 0.0);
        glRotatef (fRot[2], 0.0, 0.0, 1.0);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static GLint num_faces = 6;
        static GLfloat cube_vertices [8][3] = {
                            {1.0, 1.0, 1.0}, {1.0, -1.0, 1.0}, {-1.0, -1.0, 1.0}, {-1.0, 1.0, 1.0},
                            {1.0, 1.0, -1.0}, {1.0, -1.0, -1.0}, {-1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0} };
        static short cube_faces [6][4] = {
                            {3, 2, 1, 0}, {2, 3, 7, 6}, {0, 1, 5, 4}, {3, 0, 4, 7}, {1, 2, 6, 5}, {4, 5, 6, 7} };
//      static short cube_texCoords [2][4] = { {0, 0, 1, 1}, {0, 1, 1, 0} };
        
        GLfloat fSize   = 2.0;
        
        long f, i;
        glBegin (GL_QUADS);
        glColor3f (0.8f, 0.2f, 0.6f);
        for (f = 0; f < num_faces; f++)
            for (i = 0; i < 4; i++)
                glVertex3f(cube_vertices[cube_faces[f][i]][0] * fSize, cube_vertices[cube_faces[f][i]][1] * fSize, cube_vertices[cube_faces[f][i]][2] * fSize);
        glEnd ();
        glColor3f (0.0, 1.0, 1.0);
        for (f = 0; f < num_faces; f++)
        {
            glBegin (GL_LINE_LOOP);
                for (i = 0; i < 4; i++)
                    glVertex3f(cube_vertices[cube_faces[f][i]][0] * fSize, cube_vertices[cube_faces[f][i]][1] * fSize, cube_vertices[cube_faces[f][i]][2] * fSize);
            glEnd ();
        }
    }
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::DisplayChanged()
{
    if (mContext.get() != NULL)
    {
        AutoSetAglContext   autoSetContext(mContext);
        
        ThrowIfAgl(aglUpdateContext(mContext));
        
        Invalidate();
    }
}

// ------------------------------------------------------------------------------------------
bool
OpenGLView::SetData(
    ControlPartCode inPart, 
    ResType         inDataTag, 
    const void*     inDataPtr, 
    size_t          inDataSize)
{
    bool    handled = true;
    
    switch (inDataTag)
    {
    case kContextTag:
        {
            AutoAglContext  autoCtxt(Data<kContextTag>::Get(inDataPtr, inDataSize));
            
            if (mContext.get() != NULL)
                DisposeGL(mContext);
            
            mContext = autoCtxt;
            
            SetDrawable();
            
            if (mContext.get() != NULL)
                BuildGL(mContext);
            
            Invalidate();
        }
        break;
        
//  case kHelpButtonUrlTag:
//      SetUrl(Data<kHelpButtonUrlTag>::Get(inDataPtr, inDataSize));
//      break;
        
    default:
        handled = CustomView::SetData(inPart, inDataTag, inDataPtr, inDataSize);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
OpenGLView::GetData(
    ControlPartCode inPart, 
    ResType         inDataTag, 
    void*           inDataPtr, 
    size_t&         ioDataSize) const
{
    bool    handled = true;
    
    switch (inDataTag)
    {
    case kContextTag:
        Data<kContextTag>::Set(inDataPtr, ioDataSize, mContext);
        break;
        
//  case kHelpButtonUrlTag:
//      {
//          String  urlStr;
//          
//          Data<kHelpButtonUrlTag>::Set(inDataPtr, ioDataSize, urlStr);
//          SetUrl(urlStr);
//      }
//      break;
        
    default:
        handled = CustomView::GetData(inPart, inDataTag, inDataPtr, ioDataSize);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
OpenGLView::BoundsChanged(
    UInt32          inAttributes, 
    const Rect&     inPreviousBounds, 
    const Rect&     inCurrentBounds)
{
    CustomView::BoundsChanged(inAttributes, inPreviousBounds, inCurrentBounds);
    
    SetDrawable();
    
//  if (mContext.get() != NULL)
//  {
//      AutoSetAglContext   savedContext(mContext);
//      Rect                bounds  = inCurrentBounds;
//      
//      HIViewConvertRect(&bounds, HIViewGetSuperview(GetViewRef()), GetViewRef());
//      ResizeDrawable(bounds);
//      DrawGL(mContext);
//      glFlush();
//  }
    
    return (true);
}

//// ------------------------------------------------------------------------------------------
//void
//OpenGLView::VisibilityChanged()
//{
//  CustomView::VisibilityChanged();
//  
//  SetDrawable();
//}

// ------------------------------------------------------------------------------------------
void
OpenGLView::OwningWindowChanged(
    WindowRef       inOriginalWindow, 
    WindowRef       inCurrentWindow)
{
    CustomView::OwningWindowChanged(inOriginalWindow, inCurrentWindow);
    
    SetDrawable();
    
//  if (inCurrentWindow != NULL)
//  {
//      std::auto_ptr<EventHandler> handler(new EventHandler(inCurrentWindow));
//      
//      handler->Add(this, &OpenGLView::WindowShown);
//      handler->Add(this, &OpenGLView::WindowBoundsChanged);
//      
//      mWindowHandler = handler;
//  }
//  else
//  {
//      mWindowHandler.reset();
//  }
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::SetDrawable()
{
    if (mContext != NULL)
    {
        AutoSetAglContext   savedContext(mContext);
        WindowRef           window  = GetOwner();
        
        if (window != NULL)
        {
            Rect        windowBounds;
            ::Rect      qdBounds;
            
            GetWindowPortBounds(window, &qdBounds);
            windowBounds = qdBounds;
            
            Rect    bounds  = WindowUtils::ConvertFromWindowToLocal(
                                ViewUtils::Convert(GetBounds(), GetViewRef(), window),
                                window);
            
            ThrowIfAgl(aglSetDrawable(mContext, GetWindowPort(window)));
            
            if (bounds != windowBounds)
            {
                GLint   bufferRect[4];
                
                bufferRect[0] = static_cast<int>(bounds.minx());
                bufferRect[1] = static_cast<int>(windowBounds.height() - bounds.maxy());
                bufferRect[2] = static_cast<int>(bounds.width());
                bufferRect[3] = static_cast<int>(bounds.height());
                
                ThrowIfAgl(aglSetInteger(mContext, AGL_BUFFER_RECT, bufferRect));
                ThrowIfAgl(aglEnable(mContext, AGL_BUFFER_RECT));
            }
            else
            {
printf("no buffer rect\n"); fflush(stdout);

                ThrowIfAgl(aglDisable(mContext, AGL_BUFFER_RECT));
            }
            
            //ThrowIfAgl(aglUpdateContext(mContext));
            glViewport(0, 0, static_cast<int>(bounds.width()), static_cast<int>(bounds.height()));
        }
        else
        {
            ThrowIfAgl(aglSetDrawable(mContext, NULL));
        }
    }
}

//// ------------------------------------------------------------------------------------------
//void
//OpenGLView::ResizeDrawable(const Rect& inBounds)
//{
//  if (mContext != NULL)
//  {
//      AutoSetAglContext   savedContext(mContext);
//      WindowRef           window  = GetOwner();
//      
//      if (window != NULL)
//      {
//          gNeedBufferRect = true;
//          
//#if 0
//          HIViewRef   contentView = Window::GetContentView(window);
//          Rect        bounds      = inBounds;
//          Rect        windowBounds;
//          ::Rect      qdBounds;
//          OSStatus    err;
//          
//          GetWindowPortBounds(window, &qdBounds);
//          windowBounds = qdBounds;
//          
////            err = HIViewGetBounds(contentView, &contentBounds);
////            B_THROW_IF_STATUS(err);
//          
//          err = HIViewConvertRect(&bounds, GetViewRef(), contentView);
//          B_THROW_IF_STATUS(err);
//          
//          if (bounds != windowBounds)
//          {
//              GLint   bufferRect[4];
//              
//              bufferRect[0] = bounds.minx();
//              bufferRect[1] = windowBounds.height() - bounds.maxy();
//              bufferRect[2] = bounds.width();
//              bufferRect[3] = bounds.height();
//              
//              ThrowIfAgl(aglSetInteger(mContext, AGL_BUFFER_RECT, bufferRect));
//              ThrowIfAgl(aglEnable(mContext, AGL_BUFFER_RECT));
//          }
//          else
//          {
//              ThrowIfAgl(aglDisable(mContext, AGL_BUFFER_RECT));
//          }
//          
//          ThrowIfAgl(aglUpdateContext(mContext));
//          glViewport(0, 0, bounds.width(), bounds.height());
//#endif
//          
//          Invalidate();
//      }
//  }
//}

// ------------------------------------------------------------------------------------------
void
OpenGLView::PrintDebugInfo()
{
    printf("OpenGLView\n");
    
    CustomView::PrintDebugInfo();
}

// ------------------------------------------------------------------------------------------
void
OpenGLView::AddedSubview(HIViewRef /* inSubview */)
{
}

// ------------------------------------------------------------------------------------------
bool
OpenGLView::BOpenGLViewDraw(
    Event<kEventClassB, kBOpenGLViewDraw>&  /* event */)
{
    DrawGL();
    
    return (true);
}

//// ------------------------------------------------------------------------------------------
//bool
//OpenGLView::WindowShown(
//  Event<kEventClassWindow, kEventWindowShown>&    event)
//{
//  if (event.mWindowRef == GetOwner())
//  {
//      if (mContext.get() != NULL)
//      {
//          AutoSetAglContext   savedContext(mContext);
//          Rect                bounds; //= inCurrentBounds;
//          
//          //HIViewConvertRect(&bounds, HIViewGetSuperview(GetViewRef()), GetViewRef());
//          
//          GetBounds(bounds);
//          ResizeDrawable(bounds);
//          DrawGL(mContext);
//          glFlush();
//      }
//  }
//  
//  return (false);
//}
//
//// ------------------------------------------------------------------------------------------
//bool
//OpenGLView::WindowBoundsChanged(
//  Event<kEventClassWindow, kEventWindowBoundsChanged>&    event)
//{
//  if (event.mWindowRef == GetOwner())
//  {
//      if (mContext.get() != NULL)
//      {
//          AutoSetAglContext   savedContext(mContext);
//          Rect                bounds; //= inCurrentBounds;
//          
//          //HIViewConvertRect(&bounds, HIViewGetSuperview(GetViewRef()), GetViewRef());
//          
//          GetBounds(bounds);
//          ResizeDrawable(bounds);
//          DrawGL(mContext);
//          glFlush();
//      }
//  }
//  
//  return (false);
//}

// ------------------------------------------------------------------------------------------
pascal void
OpenGLView::DisplayMgrNotificationProc(
    void*           userData, 
    short           message, 
    void*           /* notifyData */)
{
    OpenGLView* view    = reinterpret_cast<OpenGLView*>(userData);
    
    try
    {
        if (message == kDMNotifyEvent)
        {
            view->DisplayChanged();
        }
    }
    catch (...)
    {
        // Just prevent the exception from propagating.
    }
}


#ifndef DOXYGEN_SKIP
B_VAR_EXPORT EventTargetRegistrar<OpenGLView>   gRegisterOpenGLView;
#endif


}   // namespace B
