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
#include "PListerApp.h"

#if defined(__MWERKS__) && !defined(NDEBUG)
#   include <DataViewer.h>
#endif

// B headers
#include "BAEEvent.h"
#include "BAlert.h"
#include "BViewFactory.h"
#include "BDataBrowser.h"
#include "BToolboxViews.h"

// project headers
#include "Browser.h"
#include "ModelItem.h"
#include "PListerDoc.h"
#include "PListerWin.h"

// for template instantiation
#include "BApplication.tpl.h"
#include "BMultipleDocumentPolicy.tpl.h"
#include "BDocumentWindow.tpl.h"


// ==========================================================================================
//  main

int main()
{
#if defined(__MWERKS__) && !defined(NDEBUG)
    DataViewLibInit();
#endif

    PListerApp  app;
    
    app.Run();
    
    return 0;
}


// ==========================================================================================
//  PListerApp

#pragma mark -

// ------------------------------------------------------------------------------------------
PListerApp::PListerApp()
{
}

// ------------------------------------------------------------------------------------------
void
PListerApp::RegisterDocuments()
{
    GetDocumentPolicy().GetFactory().RegisterDocumentClass<PListerDoc, cDocument>();
}

// ------------------------------------------------------------------------------------------
void
PListerApp::RegisterAppleEvents(B::AEObjectSupport& ioObjectSupport)
{
    inherited::RegisterAppleEvents(ioObjectSupport);
    
    // PLister doesn't define any custom Apple Events (it only uses the standard ones 
    // that B::AEObjectSupport already knows about), so this function isn't really 
    // needed.
    // 
    // If it did have custom events, they would be registered like this:
    // 
    // ioObjectSupport.SetDefaultEventHandler(this, &PListerApp::MyAEHandler);
}


// ------------------------------------------------------------------------------------------
bool
PListerApp::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
//  case 'Tst1':
//      {
//          B::Window*  window;
//          
//          window = B::Window::CreateFromNib<B::Window>(GetNib(), "text", this);
//          window->Show(true);
//      }
//      break;
//      
//  case 'Tst2':
//      {
//          B::Window*  window;
//          
//          window = B::Window::CreateFromNib<B::Window, AEObject>(
//                              this, GetNib(), "opengl");
//          
//          GLint           attrib[]    = { AGL_RGBA, /* AGL_DOUBLEBUFFER, AGL_DEPTH_SIZE, 16, */ AGL_NONE };
//          AGLPixelFormat  pixFormat   = aglChoosePixelFormat(NULL, 0, attrib);
//          HIViewID        ctl1Id      = { 'PLst', 1 };
//          HIViewID        ctl2Id      = { 'PLst', 2 };
//          AGLContext      ctxt1, ctxt2;
//          HIViewRef       view1, view2;
//          OSStatus        err;
//          
//          ctxt1 = aglCreateContext(pixFormat, NULL);
//          err = GetControlByID(window->GetWindowRef(), &ctl1Id, &view1);
//          err = SetControlData(view1, 0, 'Ctxt', sizeof(ctxt1), &ctxt1);
//          
//          ctxt2 = aglCreateContext(pixFormat, NULL);
//          err = GetControlByID(window->GetWindowRef(), &ctl2Id, &view2);
//          err = SetControlData(view2, 0, 'Ctxt', sizeof(ctxt2), &ctxt2);
//          
//          window->Show(true);
//      }
//      break;
//      
//  case 'Tst3':
//      {
//          B::Window*  window;
//          
//          window = B::Window::CreateFromNib<B::Window, AEObject>(
//                              this, GetNib(), "splitter");
//          
//          HIViewID        ctl1Id      = { 'PLst', 1 };
//          HIViewID        ctl2Id      = { 'PLst', 2 };
//          HIViewRef       contentview, subview1, subview2;
//          B::OSPtr<HIViewRef> splitview;
//          B::Rect         bounds;
//          OSStatus        err;
//          
//          contentview = B::Window::GetContentView(window->GetWindowRef());
//          HIViewGetBounds(contentview, &bounds);
//          
//          bounds.inset(20, 20);
//          
//          splitview = B::SplitView::Create(window->GetWindowRef(), bounds, 
//                                           kControlSizeNormal, true, 
//                                           5000, 0, 10000);
//          
//#if B_BUILDING_ON_10_3_OR_LATER
//
//          HILayoutInfo    layout;
//          
//          layout.version                  = kHILayoutInfoVersionZero;
//          layout.binding.top.toView       = NULL;
//          layout.binding.top.kind         = kHILayoutBindMin;
//          layout.binding.top.offset       = 0.0f;
//          layout.binding.left.toView      = NULL;
//          layout.binding.left.kind        = kHILayoutBindMin;
//          layout.binding.left.offset      = 0.0f;
//          layout.binding.bottom.toView    = NULL;
//          layout.binding.bottom.kind      = kHILayoutBindMax;
//          layout.binding.bottom.offset    = 0.0f;
//          layout.binding.right.toView     = NULL;
//          layout.binding.right.kind       = kHILayoutBindMax;
//          layout.binding.right.offset     = 0.0f;
//          layout.scale.x.toView           = NULL;
//          layout.scale.x.kind             = kHILayoutScaleAbsolute;
//          layout.scale.x.ratio            = 0.0f;
//          layout.scale.y.toView           = NULL;
//          layout.scale.y.kind             = kHILayoutScaleAbsolute;
//          layout.scale.y.ratio            = 0.0f;
//          layout.position.x.toView        = NULL;
//          layout.position.x.kind          = kHILayoutPositionNone;
//          layout.position.x.offset        = 0.0f;
//          layout.position.y.toView        = NULL;
//          layout.position.y.kind          = kHILayoutPositionNone;
//          layout.position.y.offset        = 0.0f;
//          
//          err = HIViewSetLayoutInfo(splitview, &layout);
//#endif
//
//          err = HIViewSetVisible(splitview, true);
//          
//          err = GetControlByID(window->GetWindowRef(), &ctl1Id, &subview1);
//          B::ViewData<B::SplitView::kControlKindSig, B::SplitView::kControlKind, B::SplitView::kSubviewATag>::Set(splitview, subview1);
//          
//          err = GetControlByID(window->GetWindowRef(), &ctl2Id, &subview2);
//          B::ViewData<B::SplitView::kControlKindSig, B::SplitView::kControlKind, B::SplitView::kSubviewBTag>::Set(splitview, subview2);
//          
//          window->Show(true);
//      }
//      break;
        
    default:
        handled = inherited::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
bool
PListerApp::HandleUpdateStatus(
    const HICommandExtended&    inHICommand, 
    B::CommandData&             ioCmdData)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case 'Tst1':
    case 'Tst2':
    case 'Tst3':
    case 'Tst4':
        ioCmdData.SetEnabled(false);
        break;
        
    default:
        handled = inherited::HandleUpdateStatus(inHICommand, ioCmdData);
        break;
    }
    
    return (handled);
}


// ==========================================================================================
//  PListerAppDocPolicy

#pragma mark -

// ------------------------------------------------------------------------------------------
PListerAppDocPolicy::PListerAppDocPolicy(
    B::AEObjectPtr      inApplication, 
    const B::Bundle&    inBundle, 
    B::Nib*             inAppNib)
        : inherited(inApplication, inBundle, inAppNib)
{
}

// ------------------------------------------------------------------------------------------
std::auto_ptr<B::GetFileDialog>
PListerAppDocPolicy::CreateGetFileDialog(
    UInt32                      inCommandID,
    const B::DialogModality&    inModality)
{
    std::auto_ptr<B::GetFileDialog> getFileDlg(inherited::CreateGetFileDialog(inCommandID, 
                                                                              inModality));

    getFileDlg->SetFlags(kNavAllowMultipleFiles  | 
                         kNavAllFilesInPopup     | 
                         kNavSupportPackages     | 
                         kNavAllowOpenPackages);
    
    return (getFileDlg);
}
