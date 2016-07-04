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
#include "BAboutBox.h"

// B headers
#include "BBundle.h"
#include "BEventCustomParams.h"
#include "BIcon.h"
#include "BRect.h"
#include "BToolboxViews.h"


namespace {
    
    const int       kNameID         = 1;
    const int       kVersionID      = 2;
    const int       kDescriptionID  = 3;
    const int       kCopyrightID    = 4;
    const int       kIconID         = 5;

}   // anonymous namespace

namespace B {


// static member variables
const Bundle*               AboutBox::sBundle   = NULL;
String                      AboutBox::sDescriptionStr;
boost::shared_ptr<AboutBox> AboutBox::sAboutBox;

// ------------------------------------------------------------------------------------------
void
AboutBox::Display(
    AEObjectPtr     inContainer, 
    const Bundle&   inBundle, 
    const String&   inDescription /* = String() */)
{
    try
    {
        boost::shared_ptr<AboutBox> aboutBox    = sAboutBox;
        Nib                         nib(inBundle, "B");
        OSPtr<WindowRef>            windowPtr;
        
        sBundle         = &inBundle;
        sDescriptionStr = inDescription;
        
        if (aboutBox.get() == NULL)
        {
            aboutBox = Window::CreateFromNib<AboutBox>(nib, "About", inContainer);
        }
        
        aboutBox->Show(true);
        aboutBox->Select();
        
        sAboutBox = aboutBox;
        windowPtr.release();
    }
    catch (...)
    {
        sAboutBox.reset();
        sBundle     = NULL;
        
        sDescriptionStr.clear();
        
        throw;
    }
}

// ------------------------------------------------------------------------------------------
AboutBox::AboutBox(
    WindowRef   inWindowRef,
    AEObjectPtr inContainer)
        : Window(inWindowRef, inContainer), 
          mBundle(*sBundle), mName(NULL), mVersion(NULL), mDescription(NULL), 
          mCopyright(NULL), mDescriptionStr(sDescriptionStr)
{
    B_ASSERT(sBundle != NULL);
    
    // Make sure the About Box doesn't end up in the Window menu.  IB doesn't allow that 
    // to be specified, so we have to do it programatically.
    
    OSStatus    err;
    
    err = ChangeWindowAttributes(inWindowRef, 0, kWindowInWindowMenuAttribute);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
AboutBox::~AboutBox()
{
    B_ASSERT(sAboutBox.get() == this);
    
    sAboutBox.reset();
}

// ------------------------------------------------------------------------------------------
void
AboutBox::Awaken(B::Nib* /* inFromNib */)
{
    Icon        appIcon(mBundle.PackageCreator(), mBundle.PackageType());
    IconView*   appIconView;
    ControlButtonContentInfo    iconInfo;
    
    iconInfo.contentType    = kControlContentIconRef;
    iconInfo.u.iconRef      = appIcon;
    
    appIconView = FindView<IconView>(kEventClassB, kIconID);
    appIconView->SetContent(iconInfo);
    
    mName = FindView<StaticTextView>(kEventClassB, kNameID);
    Rect nameFrame = mName->GetFrame();
    
    mVersion = FindView<StaticTextView>(kEventClassB, kVersionID);
    Rect versionFrame = mVersion->GetFrame();
    
    mCopyright = FindView<StaticTextView>(kEventClassB, kCopyrightID);
    Rect copyrightFrame = mCopyright->GetFrame();
    
    mDescription = FindView<StaticTextView>(kEventClassB, kDescriptionID);
    Rect descriptionFrame = mDescription->GetFrame();
    
    ControlFontStyleRec fontRec;
    int                 textHeight, textDelta, totalDelta = 0;
    
    // The name string uses 14 pt Lucida Grande Bold.
    
    fontRec.flags   = kControlUseThemeFontIDMask | kControlUseJustMask | kControlUseSizeMask | kControlAddToMetaFontMask;
    fontRec.font    = kThemeEmphasizedSystemFont;
    fontRec.size    = 14;
    fontRec.just    = teCenter;
    
    String temp = mName->GetText();
    
    mName->SetFontStyle(fontRec);
    mName->SetText(mBundle.Name());
    
    textHeight  = mName->GetTextHeight();
    textDelta   = static_cast<int>(textHeight - nameFrame.height());
    totalDelta  += textDelta;
    nameFrame.height(textHeight);
    
    versionFrame.moveby(0.0f, textDelta);
    descriptionFrame.moveby(0.0f, textDelta);
    copyrightFrame.moveby(0.0f, textDelta);
    
    // The version string uses the label font.
    
    fontRec.flags   = kControlUseThemeFontIDMask | kControlUseJustMask;
    fontRec.font    = kThemeLabelFont;
    fontRec.just    = teCenter;
    
    mVersion->SetFontStyle(fontRec);
    mVersion->SetText(mBundle.ShortVersionString());
    
    textHeight  = mVersion->GetTextHeight();
    textDelta   = static_cast<int>(textHeight - versionFrame.height());
    totalDelta  += textDelta;
    versionFrame.height(textHeight);
    
    descriptionFrame.moveby(0.0f, textDelta);
    copyrightFrame.moveby(0.0f, textDelta);
    
    // The description string uses the small system font.
    
    if (!mDescriptionStr.empty())
    {
        fontRec.flags   = kControlUseThemeFontIDMask | kControlUseJustMask;
        fontRec.font    = kThemeSmallSystemFont;
        fontRec.just    = teCenter;
        
        mDescription->SetFontStyle(fontRec);
        mDescription->SetText(mDescriptionStr);
        
        textHeight  = mDescription->GetTextHeight();
        textDelta   = static_cast<int>(textHeight - descriptionFrame.height());
        descriptionFrame.height(textHeight);
    }
    else
    {
        textDelta = static_cast<int>(descriptionFrame.miny() - copyrightFrame.miny());
        
        mDescription->Show(false);
    }
    
    totalDelta  += textDelta;
    
    copyrightFrame.moveby(0.0f, textDelta);
    
    // The copyright string uses the label font.
    
    fontRec.flags   = kControlUseThemeFontIDMask | kControlUseJustMask;
    fontRec.font    = kThemeLabelFont;
    fontRec.just    = teCenter;
    
    mCopyright->SetFontStyle(fontRec);
    mCopyright->SetText(mBundle.Copyright());
    
    textHeight  = mCopyright->GetTextHeight();
    textDelta   = static_cast<int>(textHeight - copyrightFrame.height());
    totalDelta  += textDelta;
    copyrightFrame.height(textHeight);
    
    ::Rect      windowBounds;
    OSStatus    err;
    
    err = GetWindowBounds(*this, kWindowStructureRgn, &windowBounds);
    B_THROW_IF_STATUS(err);
    
    windowBounds.bottom += totalDelta;
    
    err = SetWindowBounds(*this, kWindowStructureRgn, &windowBounds);
    B_THROW_IF_STATUS(err);
    
    mName->SetFrame(nameFrame);
    mVersion->SetFrame(versionFrame);
    mDescription->SetFrame(descriptionFrame);
    mCopyright->SetFrame(copyrightFrame);
}

// ------------------------------------------------------------------------------------------
void
AboutBox::MakeSpecifier(
    AEWriter&       ioWriter) const
{
    BuildIndexSpecifier(GetContainer(), GetClassID(), GetIndex(), ioWriter);
}


}   // namespace B
