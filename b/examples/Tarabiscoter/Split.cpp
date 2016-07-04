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
#include "Split.h"

// B headers
#include "BBundle.h"
#include "BIcon.h"
#include "BImageUtilities.h"
#include "BSplitView.h"


typedef B::ViewData<
    B::SplitView::kControlKindSig, 
    B::SplitView::kControlKind, 
    B::SplitView::kOrientationTag>      OrientationData;

typedef B::ViewData<
    B::SplitView::kControlKindSig, 
    B::SplitView::kControlKind, 
    B::SplitView::kSubviewTag>          SubviewData;

typedef B::ViewData<
    B::SplitView::kControlKindSig, 
    B::SplitView::kControlKind, 
    B::SplitView::kContentTag>          ContentData;

static const OSType kSplitterIconType   = 'Spli';


// ------------------------------------------------------------------------------------------
Split::Split(
    WindowRef       inWindowRef,
    B::AEObjectPtr  inContainer)
        : inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
Split::Awaken(B::Nib* /* inFromNib */)
{
//  static bool sRegisteredSplitterIcon = false;
    
//  B_ASSERT(inFromNib != NULL);
    
//  const B::Bundle&    bundle(inFromNib->GetBundle());
//  B::Icon             icon;
    
//  if (!sRegisteredSplitterIcon)
//  {
//      B::Url  iconUrl(bundle.Resource(B::String("knob"), B::String("icns")));
//      
//      icon = B::Icon::Register(bundle.PackageCreator(), kSplitterIconType, iconUrl);
//      
//      sRegisteredSplitterIcon = true;
//  }
//  else
//  {   
//      icon.Assign(bundle.PackageCreator(), kSplitterIconType);
//  }
    
//  HIViewImageContentInfo  content;
    B::View*                splitView;
    B::View*                subview;
    
//  content.contentType = kControlContentIconRef;
//  content.u.iconRef   = icon;
    
    splitView = FindView<B::View>(1);
                           
//  ContentData::Set(*splitView, content);
    
    if (SubviewData::Get(*splitView, B::SplitView::kViewPartViewA) == NULL)
    {
        splitView->SetMaximum(10000);
        splitView->SetValue(5000);
        
        OrientationData::Set(*splitView, false);
                
        subview = FindView<B::View>(2);
        SubviewData::Set(*splitView, *subview, B::SplitView::kViewPartViewA);
        
        subview = FindView<B::View>(5);
        SubviewData::Set(*splitView, *subview, B::SplitView::kViewPartViewB);
    }
    
    splitView = FindView<B::View>(2);

//  ContentData::Set(*splitView, content);
    
    if (SubviewData::Get(*splitView, B::SplitView::kViewPartViewA) == NULL)
    {
        splitView->SetMaximum(10000);
        splitView->SetValue(3333);
        splitView->SetSize(kControlSizeSmall);

        subview = FindView<B::View>(3);
        SubviewData::Set(*splitView, *subview, B::SplitView::kViewPartViewA);
        
        subview = FindView<B::View>(4);
        SubviewData::Set(*splitView, *subview, B::SplitView::kViewPartViewB);
    }
    
    splitView = FindView<B::View>(5);

//  ContentData::Set(*splitView, content);
    
    if (SubviewData::Get(*splitView, B::SplitView::kViewPartViewA) == NULL)
    {
        splitView->SetMaximum(10000);
        splitView->SetValue(6666);
        splitView->SetSize(kControlSizeSmall);

        subview = FindView<B::View>(6);
        SubviewData::Set(*splitView, *subview, B::SplitView::kViewPartViewA);
        
        subview = FindView<B::View>(7);
        SubviewData::Set(*splitView, *subview, B::SplitView::kViewPartViewB);
    }
}
