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

// file header
#include "BNibUtils.h"

// B headers
#include "BCustomView.h"
#include "BWindow.h"


namespace B {
namespace NibUtils {


// ------------------------------------------------------------------------------------------
OSPtr<HIViewRef>
CreateViewFromNib(
    Nib&            inNib,          //!< The nib file from which to read the window.
    const char*     inWindowName,   //!< The window's name in the nib file.
    const HIViewID& inID)           //!< The ID of the view to return.
{
    boost::shared_ptr<Window>   window  = Window::CreateFromNib<Window>(inNib, inWindowName, AEObjectPtr());
    HIViewRef                   content = window->GetContentView();
    OSPtr<HIViewRef>            view(ViewUtils::FindSubview(content, inID), from_copy);
    OSStatus                    err;
    
    err = HIViewRemoveFromSuperview(view.get());
    B_THROW_IF_STATUS(err);

    return view;
}

// ------------------------------------------------------------------------------------------
void
CreateViewsIntoSuperview(
    Nib&            inNib,          //!< The nib file from which to read the window.
    const char*     inWindowName,   //!< The window's name in the nib file.
    const HIViewID& inParentID,
    HIViewRef       inSuperview,
    bool            inResizeSuperview)
{
    boost::shared_ptr<Window>   window  = Window::CreateFromNib<Window>(inNib, inWindowName, AEObjectPtr());
    HIViewRef                   content = window->GetContentView();
    HIViewRef                   parent, child;
    HIRect                      parentFrame, savedSuperviewFrame;
    OSStatus                    err;
    
    // Locate the parent view in the window we just created. If the caller didn't 
    // specify any, use the content view instead.
    
    if ((inParentID.signature != 0) || (inParentID.id != 0))
    {
        parent = ViewUtils::FindSubview(content, inParentID);
    }
    else
    {
        parent = content;
    }
    
    // Save the new superview's frame & change to match the current parent's frame.
    // After the subviews are added, we will restore the superview's frame.  This allows
    // HILayout stuff to happen to the subviews, so that if the parent view and superview
    // don't have the same frame, the subviews' layout doesn't get messed up.
    
    if (!inResizeSuperview)
    {
        err = HIViewGetFrame(inSuperview, &savedSuperviewFrame);
        B_THROW_IF_STATUS(err);
    }
    
    err = HIViewGetFrame(parent, &parentFrame);
    B_THROW_IF_STATUS(err);
    
    parentFrame.origin = CGPointZero;
    
    err = HIViewSetFrame(inSuperview, &parentFrame);
    B_THROW_IF_STATUS(err);
    
    while ((child = HIViewGetLastSubview(parent)) != NULL)
    {
        err = HIViewAddSubview(inSuperview, child);
        B_THROW_IF_STATUS(err);
    }
    
    if (!inResizeSuperview)
    {
        err = HIViewSetFrame(inSuperview, &savedSuperviewFrame);
        B_THROW_IF_STATUS(err);
    }
}

// ------------------------------------------------------------------------------------------
void
ReplaceViewInSuperview(
    Nib&            inNib,          //!< The nib file from which to read the window.
    const char*     inWindowName,   //!< The window's name in the nib file.
    const HIViewID& inParentID,
    HIViewRef       inSuperview,
    const HIViewID& inViewID,
    bool            inResizeSuperview)
{
    HIViewRef   view    = B::ViewUtils::FindSubview(inSuperview, inViewID);
    
    if (view != NULL)
    {
        OSStatus    err;
        
        err = HIViewRemoveFromSuperview(view);
        B_THROW_IF_STATUS(err);
        
        CFRelease(view);
    }
    
    CreateViewsIntoSuperview(inNib, inWindowName, inParentID, inSuperview, 
                             inResizeSuperview);
}

// ------------------------------------------------------------------------------------------
OSPtr<HIViewRef>
CreateViewsIntoNewSuperview(
    Nib&            inNib,          //!< The nib file from which to read the window.
    const char*     inWindowName,   //!< The window's name in the nib file.
    const HIViewID& inParentID,
    CFStringRef     inSuperviewClassID,
    const HIViewID& inSuperviewID,
    HIViewRef       inSuperviewSuperview,
    EventRef        inEvent,
    bool            inResizeSuperview)
{
    OSPtr<HIViewRef>    superview;
    
    superview = CustomView::Create(inSuperviewClassID, inSuperviewID, inSuperviewSuperview, 
                                   NULL, &inNib, inEvent);
    
    CreateViewsIntoSuperview(inNib, inWindowName, inParentID, superview, inResizeSuperview);
    
    return superview;
}

}   // namespace NibUtils
}   // namespace B
