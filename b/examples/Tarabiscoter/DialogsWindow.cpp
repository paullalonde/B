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
#include "DialogsWindow.h"

// B headers
#include "BApplication.h"
#include "BView.h"
#include "BWindowUtils.h"

// project headers
#include "TarabiscoterApp.h"


// Make non-zero to compile some code that will exercise template instantiation.
#define COMPILATION_TESTS   0


namespace {

// ==========================================================================================
//  TestDialog

class TestDialog : public B::Dialog
{
public:

    TestDialog(
        WindowRef       inWindowRef,
        B::AEObjectPtr  inContainer);
    
    // for compilation tests only.
    TestDialog(
        WindowRef       inWindowRef,
        B::AEObjectPtr  inContainer, 
        int             x);
    
protected:
    
    // overrides from B::Window
    virtual void    Awaken(B::Nib* inFromNib);

private:
    
    // types
    typedef B::Dialog   inherited;
};

// ------------------------------------------------------------------------------------------
TestDialog::TestDialog(
    WindowRef       inWindowRef,
    B::AEObjectPtr  inContainer)
        : inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
TestDialog::TestDialog(
    WindowRef       inWindowRef,
    B::AEObjectPtr  inContainer,
    int             /* x */)
        : inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
TestDialog::Awaken(B::Nib* /* inFromNib */)
{
    // Set up the dialog's min & max size.
    
    B::Size size    = GetContentSize();
    
    SetMinimumSize(size);
    
    size.width  *= 1.5;
    size.height *= 1.2;
    
    SetMaximumSize(size);
}


// ==========================================================================================
//  TestAlert

class TestAlert : public B::Alert
{
public:

            TestAlert();
//  virtual ~TestDialog();
};

// ------------------------------------------------------------------------------------------
TestAlert::TestAlert()
{
    SetParamFlags(kStdAlertDoNotAnimateOnCancel);
//  SetHelpUrl(B::Url("http://www.apple.com"));
}


}   // anonymous namespace


// ==========================================================================================
//  DialogsWindow

// ------------------------------------------------------------------------------------------
DialogsWindow::DialogsWindow(
    WindowRef       inWindowRef,
    B::AEObjectPtr  inContainer)
        : inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
DialogsWindow::Awaken(B::Nib* /* inFromNib */)
{
    mStatus = FindView<B::StaticTextView>(1);
}

// ------------------------------------------------------------------------------------------
bool
DialogsWindow::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case 'DLGN':
        ShowDialog(kWindowModalityNone);
        break;
        
    case 'DLGA':
        ShowDialog(kWindowModalityAppModal);
        break;
        
    case 'DLGW':
        ShowDialog(kWindowModalityWindowModal);
        break;
        
    case 'ALRA':
        ShowAlert(kWindowModalityAppModal);
        break;
        
    case 'ALRW':
        ShowAlert(kWindowModalityWindowModal);
        break;
        
    default:
        handled = inherited::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
void
DialogsWindow::ShowDialog(
    WindowModality  inModality)
{
    B::Nib&             nib         = TarabiscoterApp::Get().GetNib();
    const char*         name        = "dialog-test";
    B::AEObjectPtr      container   = GetContainer();
    B::DialogModality   modality(inModality, *this);
    
    B::Dialog::Start<TestDialog>(nib, name, container, modality,
                                 boost::bind(
                                    &DialogsWindow::DialogCompleted, 
                                    this, _1, _2, GetModalityString(inModality)));
    
#if COMPILATION_TESTS
    
    std::pair<B::OSPtr<WindowRef>, TestDialog*> p;
    UInt32                                      cmd;
    
    p = B::Dialog::Make<TestDialog>(nib, name, container, modality);
    p = B::Dialog::MakeWithState<TestDialog, int>(nib, name, container, 3, modality);
    
    B::Dialog::Enter(p);
    
    B::Dialog::Start<TestDialog>(nib, name, container, modality);
    
    cmd = B::Dialog::Run<TestDialog>(nib, name, container);
    cmd = B::Dialog::RunWithState<TestDialog, int>(nib, name, container, 3);
    
#endif
}

// ------------------------------------------------------------------------------------------
void
DialogsWindow::ShowAlert(
    WindowModality  inModality)
{
    B::DialogModality   modality(inModality, *this);
    B::String           messageText("This is an alert.");
    B::String           informativeText("At runtime, its appearance is modified depending on how it is displayed (modal or sheet).");
    
    B::Alert::Start<TestAlert>(messageText, informativeText, modality, 
                               true, true, true, B::Alert::kOK, B::Alert::kCancel, 
                               B::String(), B::String(), B::String(), 
                               boost::bind(
                                    &DialogsWindow::AlertCompleted, 
                                    this, _1, _2, GetModalityString(inModality)));

#if COMPILATION_TESTS
    
    B::String   okText("ok text");
    B::String   cancelText("cancel text");
    B::String   otherText("other text");

    std::auto_ptr<B::Alert> p;
    B::Alert::Button        btn;
    
    p = B::Alert::Make<B::Alert>(messageText, informativeText, modality);
    p = B::Alert::Make<B::Alert>(messageText, informativeText, modality, 
                                 false, true);
    p = B::Alert::Make<B::Alert>(messageText, informativeText, modality, 
                                 false, true, false, B::Alert::kCancel, B::Alert::kOK, 
                                 okText, cancelText, otherText);
    
    B::Alert::Enter(p);
    
    B::Alert::Start<B::Alert>(messageText, informativeText, modality);
    B::Alert::Start<B::Alert>(messageText, informativeText, modality, 
                              false, true);
    B::Alert::Start<B::Alert>(messageText, informativeText, modality, 
                              false, true, false, B::Alert::kCancel, B::Alert::kOK, 
                              okText, cancelText, otherText);
    
    btn = B::Alert::Run<B::Alert>(messageText, informativeText);
    btn = B::Alert::Run<B::Alert>(messageText, informativeText, 
                                  false, true);
    btn = B::Alert::Run<B::Alert>(messageText, informativeText, 
                                  false, true, false, B::Alert::kCancel, B::Alert::kOK, 
                                  okText, cancelText, otherText);

#endif
}

// ------------------------------------------------------------------------------------------
void
DialogsWindow::DialogCompleted(
    const B::Dialog&    inDialog,
    UInt32              inCommand,
    const std::string&  inString)
{
    const_cast<B::Dialog&>(inDialog).SetCloseOnDismiss(true);
    const_cast<B::Dialog&>(inDialog).SetAnimateOnDismiss(inCommand != kHICommandCancel);
    
    SetStatus("Dialog", inCommand, inString);
}

// ------------------------------------------------------------------------------------------
void
DialogsWindow::AlertCompleted(
    const B::Alert&     /* inAlert */,
    B::Alert::Button    inButton,
    const std::string&  inString)
{
    SetStatus("Alert", inButton, inString);
}

// ------------------------------------------------------------------------------------------
void
DialogsWindow::SetStatus(
    const std::string&  inType,
    UInt32              inCommand,
    const std::string&  inString)
{
    B::String   str(UTCreateStringForOSType(inCommand), B::from_copy);
    std::string cstr;
    
    str.copy(cstr, kCFStringEncodingMacRoman);
    
    std::ostringstream  ostr;
    
    ostr << inType << " of type \"" << inString << "\" completed with command " 
         << "'" << cstr << "'.";
    
    mStatus->SetText(B::String(ostr.str()));
}

// ------------------------------------------------------------------------------------------
std::string
DialogsWindow::GetModalityString(
        WindowModality      inModality)
{
    std::string status;
    
    switch (inModality)
    {
    case kWindowModalityNone:           status = "modeless";            break;
    case kWindowModalityAppModal:       status = "application-modal";   break;
    case kWindowModalityWindowModal:    status = "sheet";               break;
    }
    
    return (status);
}
