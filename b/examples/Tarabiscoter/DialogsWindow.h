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

#ifndef DialogsWindow_H_
#define DialogsWindow_H_

#pragma once

// B headers
#include "BAlert.h"
#include "BDialog.h"
#include "BToolboxViews.h"


class DialogsWindow : public B::Window
{
public:

	// constructor
	DialogsWindow(
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
	
	void	ShowDialog(
				WindowModality		inModality);
	void	ShowAlert(
				WindowModality		inModality);
	void	DialogCompleted(
				const B::Dialog&	inDialog,
				UInt32				inCommand,
				const std::string&	inString);
	void	AlertCompleted(
				const B::Alert&		inAlert,
				B::Alert::Button	inButton,
				const std::string&	inString);
	void	SetStatus(
				const std::string&	inType,
				UInt32				inCommand,
				const std::string&	inString);
	std::string
			GetModalityString(
				WindowModality		inModality);
			
	// member variables
	B::StaticTextView*	mStatus;
};


#endif // DialogsWindow
