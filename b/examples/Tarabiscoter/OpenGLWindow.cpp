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

// file header
#include "OpenGLWindow.h"

// B headers
#include "BOpenGLView.h"


typedef B::ViewData<
    B::OpenGLView::kControlKindSig, 
    B::OpenGLView::kControlKind, 
    B::OpenGLView::kContextTag>         ContextData;


// ------------------------------------------------------------------------------------------
OpenGLWindow::OpenGLWindow(
    WindowRef       inWindowRef,
    B::AEObjectPtr  inContainer)
        : inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
OpenGLWindow::Awaken(B::Nib* /* inFromNib */)
{
//  FindView<B::View>(100)->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
//                                      kHILayoutBindRight, kHILayoutBindNone);
//  FindView<B::View>(101)->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
//                                      kHILayoutBindRight, kHILayoutBindNone);
    
    GLint           attrib[]    = { AGL_RGBA, /* AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 16, */ AGL_NONE };
    AGLPixelFormat  pixelFormat;
    AGLContext      context;
    B::View*        glView;
    
    pixelFormat = aglChoosePixelFormat(NULL, 0, attrib);
    
    glView = FindView<B::View>(1);
//  glView->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
//                      kHILayoutBindNone, kHILayoutBindBottom);
    
    context = aglCreateContext(pixelFormat, NULL);
    ContextData::Set(*glView, context);
    
    glView = FindView<B::View>(2);
//  glView->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
//                      kHILayoutBindRight, kHILayoutBindBottom);
    
    context = aglCreateContext(pixelFormat, NULL);
    ContextData::Set(*glView, context);
}
