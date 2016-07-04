// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

#ifndef NavigationWindow_H_
#define NavigationWindow_H_

#pragma once

// B headers
#include "BNavDialogs.h"
#include "BToolboxViews.h"
#include "BWindow.h"


class NavigationWindow : public B::Window
{
public:

	// constructor
	NavigationWindow(
		WindowRef		inWindowRef,
		B::AEObjectPtr	inContainer);
	
protected:
	
	// overrides from B::Window
	virtual void	Awaken(B::Nib* inFromNib);
	
	// CarbonEvent handlers
	virtual bool	HandleCommand(
						const HICommandExtended&	inHICommand);

private:
	
	// types
	typedef B::Window   inherited;
	
	void	ShowAskReviewDocumentsDialog(
				WindowModality						inModality);
	void	AskReviewDocumentsCompleted(
				const B::AskReviewDocumentsDialog&	inDialog,
				B::AskReviewDocumentsDialog::Action	inAction,
				const std::string&					inModality);
	
	void	ShowAskSaveChangesDialog(
				WindowModality						inModality);
	void	AskSaveChangesCompleted(
				const B::AskSaveChangesDialog&		inDialog,
				B::AskSaveChangesDialog::Action		inAction,
				const std::string&					inModality);
	
	void	ShowAskDiscardChangesDialog(
				WindowModality						inModality);
	void	AskDiscardChangesCompleted(
				const B::AskDiscardChangesDialog&	inDialog,
				bool								inDiscard,
				const std::string&					inModality);
	
	void	ShowGetFileDialog(
				WindowModality						inModality);
	void	GetFileCompleted(
				const B::GetFileDialog&				inDialog,
				bool								inGood,
				const NavReplyRecord&				inNavReply,
				const std::string&					inModality);
	
	void	ShowPutFileDialog(
				WindowModality						inModality);
	void	PutFileCompleted(
				const B::PutFileDialog&				inDialog,
				bool								inGood,
				NavReplyRecord&						ioNavReply,
				const B::SaveInfo&					inSaveInfo,
				const std::string&					inModality);

//	void	ShowAlert(
//				WindowModality		inModality,
//				const char*			inStatus);
//	void	AlertCompleted(
//				B::Alert&			inAlert,
//				B::Alert::Button	inButton,
//				const std::string&	inString);

	void	SetStatus(
				const std::string&	inDialog,
				const std::string&	inModality,
				const std::string&	inResult,
				CFStringRef			inFileName = NULL);
	std::string
			GetModalityString(
				WindowModality		inModality);
	
	// member variables
	B::StaticTextView*	mStatus;
};


#endif // NavigationWindow_H_
