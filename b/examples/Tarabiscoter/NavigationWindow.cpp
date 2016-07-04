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
#include "NavigationWindow.h"

// B headers
#include "BView.h"
#include "BWindowUtils.h"

// project headers
#include "TarabiscoterApp.h"


// Make non-zero to compile some code that will exercise template instantiation.
#define COMPILATION_TESTS   0


// ==========================================================================================
//  NavigationWindow

// ------------------------------------------------------------------------------------------
NavigationWindow::NavigationWindow(
    WindowRef       inWindowRef,
    B::AEObjectPtr  inContainer)
        : inherited(inWindowRef, inContainer)
{
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::Awaken(B::Nib* /* inFromNib */)
{
    mStatus = FindView<B::StaticTextView>(1);
}

// ------------------------------------------------------------------------------------------
bool
NavigationWindow::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case 'ARDN':
        ShowAskReviewDocumentsDialog(kWindowModalityNone);
        break;
        
    case 'ARDA':
        ShowAskReviewDocumentsDialog(kWindowModalityAppModal);
        break;
        
    case 'ARDW':
        ShowAskReviewDocumentsDialog(kWindowModalityWindowModal);
        break;
        
    case 'ASCN':
        ShowAskSaveChangesDialog(kWindowModalityNone);
        break;
        
    case 'ASCA':
        ShowAskSaveChangesDialog(kWindowModalityAppModal);
        break;
        
    case 'ASCW':
        ShowAskSaveChangesDialog(kWindowModalityWindowModal);
        break;
        
    case 'ADCN':
        ShowAskDiscardChangesDialog(kWindowModalityNone);
        break;
        
    case 'ADCA':
        ShowAskDiscardChangesDialog(kWindowModalityAppModal);
        break;
        
    case 'ADCW':
        ShowAskDiscardChangesDialog(kWindowModalityWindowModal);
        break;
        
    case 'GETN':
        ShowGetFileDialog(kWindowModalityNone);
        break;
        
    case 'GETA':
        ShowGetFileDialog(kWindowModalityAppModal);
        break;
        
    case 'GETW':
        ShowGetFileDialog(kWindowModalityWindowModal);
        break;
        
    case 'PUTN':
        ShowPutFileDialog(kWindowModalityNone);
        break;
        
    case 'PUTA':
        ShowPutFileDialog(kWindowModalityAppModal);
        break;
        
    case 'PUTW':
        ShowPutFileDialog(kWindowModalityWindowModal);
        break;
        
    default:
        handled = inherited::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::ShowAskReviewDocumentsDialog(
    WindowModality  inModality)
{
    B::DialogModality                           modality(inModality, *this);
    std::auto_ptr<B::AskReviewDocumentsDialog>  dialogPtr;
    
    dialogPtr = B::AskReviewDocumentsDialog::Make<B::AskReviewDocumentsDialog>(
                        modality, 3, 
                        boost::bind(
                            &NavigationWindow::AskReviewDocumentsCompleted, 
                            this, _1, _2, GetModalityString(inModality)));
    
    dialogPtr->SetPrefsKey(1);
    B::NavDialog::Enter(dialogPtr);

#if COMPILATION_TESTS
    
    B::AskReviewDocumentsDialog::Action action;
    
    B::AskReviewDocumentsDialog::Start<B::AskReviewDocumentsDialog>(modality, 3);
    
    action = B::AskReviewDocumentsDialog::Run<B::AskReviewDocumentsDialog>(3);
    
#endif
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::AskReviewDocumentsCompleted(
    const B::AskReviewDocumentsDialog&  /* inDialog */,
    B::AskReviewDocumentsDialog::Action inAction,
    const std::string&                  inModality)
{
    std::string result;
    
    switch (inAction)
    {
    case B::AskReviewDocumentsDialog::kReview:  result = "review";  break;
    case B::AskReviewDocumentsDialog::kDiscard: result = "discard"; break;
    case B::AskReviewDocumentsDialog::kCancel:  result = "cancel";  break;
    }
    
    SetStatus("Ask Review Documents", inModality, result);
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::ShowAskSaveChangesDialog(
    WindowModality  inModality)
{
    B::DialogModality                       modality(inModality, *this);
    B::String                               filename("My Document");
    B::AskSaveChangesDialog::Context        context = B::AskSaveChangesDialog::kQuitting;
    std::auto_ptr<B::AskSaveChangesDialog>  dialogPtr;
    
    dialogPtr = B::AskSaveChangesDialog::Make<B::AskSaveChangesDialog>(
                        modality, filename, context, 
                        boost::bind(
                            &NavigationWindow::AskSaveChangesCompleted, 
                            this, _1, _2, GetModalityString(inModality)));
    
    dialogPtr->SetPrefsKey(2);
    B::NavDialog::Enter(dialogPtr);

#if COMPILATION_TESTS
    
    B::AskSaveChangesDialog::Action action;
    
    B::AskSaveChangesDialog::Start<B::AskSaveChangesDialog>(modality, filename, context);
    
    action = B::AskSaveChangesDialog::Run<B::AskSaveChangesDialog>(filename, context);
    
#endif
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::AskSaveChangesCompleted(
    const B::AskSaveChangesDialog&      /* inDialog */,
    B::AskSaveChangesDialog::Action     inAction,
    const std::string&                  inModality)
{
    std::string result;
    
    switch (inAction)
    {
    case B::AskSaveChangesDialog::kSave:        result = "save";        break;
    case B::AskSaveChangesDialog::kDontSave:    result = "don't save";  break;
    case B::AskSaveChangesDialog::kCancel:      result = "cancel";      break;
    }
    
    SetStatus("Ask Save Changes", inModality, result);
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::ShowAskDiscardChangesDialog(
    WindowModality  inModality)
{
    B::DialogModality                           modality(inModality, *this);
    B::String                                   filename("My Document");
    std::auto_ptr<B::AskDiscardChangesDialog>   dialogPtr;
    
    dialogPtr = B::AskDiscardChangesDialog::Make<B::AskDiscardChangesDialog>(
                        modality, filename, 
                        boost::bind(
                            &NavigationWindow::AskDiscardChangesCompleted, 
                            this, _1, _2, GetModalityString(inModality)));
    
    dialogPtr->SetPrefsKey(3);
    B::NavDialog::Enter(dialogPtr);

#if COMPILATION_TESTS
    
    B::AskDiscardChangesDialog::Action  action;
    
    B::AskDiscardChangesDialog::Start<B::AskDiscardChangesDialog>(modality, filename);
    
    action = B::AskDiscardChangesDialog::Run<B::AskDiscardChangesDialog>(filename);
    
#endif
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::AskDiscardChangesCompleted(
    const B::AskDiscardChangesDialog&   /* inDialog */,
    bool                                inDiscard,
    const std::string&                  inModality)
{
    SetStatus("Ask Discard Changes", inModality, inDiscard ? "discard" : "cancel");
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::ShowGetFileDialog(
    WindowModality  inModality)
{
    B::DialogModality               modality(inModality, *this);
    std::auto_ptr<B::GetFileDialog> dialogPtr;
    
    dialogPtr = B::GetFileDialog::Make<B::GetFileDialog>(
                        modality, 
                        boost::bind(
                            &NavigationWindow::GetFileCompleted, 
                            this, _1, _2, _3, GetModalityString(inModality)));
    
    dialogPtr->SetPrefsKey(4);
    B::NavDialog::Enter(dialogPtr);

#if COMPILATION_TESTS
    
    B::AutoNavReply autoReply;
    bool            goodReply;
    
    B::GetFileDialog::Start<B::GetFileDialog>(modality);
    
    goodReply = B::GetFileDialog::Run<B::GetFileDialog>(autoReply);
    
#endif
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::GetFileCompleted(
    const B::GetFileDialog&             /* inDialog */,
    bool                                inGood,
    const NavReplyRecord&               /* inNavReply */,
    const std::string&                  inModality)
{
    SetStatus("Get File", inModality, inGood ? "open" : "cancel");
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::ShowPutFileDialog(
    WindowModality  inModality)
{
    B::DialogModality               modality(inModality, *this);
    B::String                       filename("My Document.txt");
    std::auto_ptr<B::PutFileDialog> dialogPtr;
    
    dialogPtr = B::PutFileDialog::Make<B::PutFileDialog>(
                        modality, filename, 
                        boost::bind(
                            &NavigationWindow::PutFileCompleted, 
                            this, _1, _2, _3, _4, GetModalityString(inModality)));
    
    dialogPtr->SetPrefsKey(5);
    B::NavDialog::Enter(dialogPtr);

#if COMPILATION_TESTS
    
    B::AutoNavReply autoReply;
    bool            goodReply;
    
    B::PutFileDialog::Start<B::PutFileDialog>(modality, filename);
    
    goodReply = B::PutFileDialog::Run<B::PutFileDialog>(filename, autoReply);
    
#endif
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::PutFileCompleted(
    const B::PutFileDialog&             /* inDialog */,
    bool                                inGood,
    NavReplyRecord&                     ioNavReply,
    const B::SaveInfo&                  /* inSaveInfo */,
    const std::string&                  inModality)
{
    SetStatus("Get File", inModality, inGood ? "open" : "cancel", ioNavReply.saveFileName);
}

// ------------------------------------------------------------------------------------------
void
NavigationWindow::SetStatus(
    const std::string&  inDialog,
    const std::string&  inModality,
    const std::string&  inResult,
    CFStringRef         inFileName /* = NULL */)
{
    std::ostringstream  ostr;
    
    ostr << "Nav Dialog \"" << inDialog << "\" of modality \"" << inModality 
         << "\" completed with command \"" << inResult << "\"";
    
    if (inFileName != NULL)
    {
        B::String   filename(inFileName);
        
        ostr << " and file \"";
        filename.Write(ostr, kCFStringEncodingUTF8);
        ostr << "\"";
    }
    
    ostr << ".";
    
    mStatus->SetText(B::String(ostr.str(), kCFStringEncodingUTF8));
}

// ------------------------------------------------------------------------------------------
std::string
NavigationWindow::GetModalityString(
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
