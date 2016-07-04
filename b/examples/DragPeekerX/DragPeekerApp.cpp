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

// file header
#include "DragPeekerApp.h"

#if !defined(NDEBUG) && defined(__MWERKS__)
#	include <DataViewer.h>
#endif

// library headers
#include <boost/tuple/tuple.hpp>

// B headers
#include "BAboutBox.h"
#include "BStringFormatter.h"
#include "BToolboxViews.h"
#include "BWindowUtils.h"

// project headers
#include "DragPeekerView.h"

// for template instantiation
#include "BApplication.tpl.h"


// ==========================================================================================
//	main

int main()
{
#if !defined(NDEBUG) && defined(__MWERKS__)
	DataViewLibInit();
#endif
	
	Application	app;
	
	app.Run();
	
	return 0;
}


// ==========================================================================================
//	Application

#pragma mark -

// ------------------------------------------------------------------------------------------
Application::Application()
	: mNextWindowNumber(0)
{
	// There's nothing to do.
}

// ------------------------------------------------------------------------------------------
Application::~Application()
{
	// There's nothing to do.
}

// ------------------------------------------------------------------------------------------
void
Application::RegisterViews(B::ViewFactory& ioFactory)
{
	// Call base class to perform view registration.
	inherited::RegisterViews(ioFactory);
	
	// Register our own custom view(s).
	B::EventTarget::Register<DragPeekerView>();
}

// ------------------------------------------------------------------------------------------
/*!	We've gotten an 'Open application' Apple %Event.  Open a new drag peeker window.
*/
void
Application::OpenApplication()
{
	CreateDragPeekerWindow();
}

// ------------------------------------------------------------------------------------------
/*!	We've gotten an 'Reopen application' Apple %Event.  Open a new drag peeker window, if 
	there isn't any currently open window.
*/
void
Application::ReopenApplication()
{
	if (CountElements(cWindow) == 0)
	{
		CreateDragPeekerWindow();
	}
}

// ------------------------------------------------------------------------------------------
/*!	Handle menu commands.  We just handle the New command (to display a new drag peeker 
	window).
*/
bool
Application::HandleCommand(
	const HICommandExtended&	inHICommand)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandNew:
		CreateDragPeekerWindow();
		break;
		
	default:
		handled = inherited::HandleCommand(inHICommand);
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
/*!	Update the status of menu commands.  We just enable the New command.
*/
bool
Application::HandleUpdateStatus(
	const HICommandExtended&	inHICommand, 
	B::CommandData&				ioCmdData)
{
	bool	handled	= true;
	
	switch (inHICommand.commandID)
	{
	case kHICommandNew:
		ioCmdData.SetEnabled(true);
		break;
	
	default:
		handled = inherited::HandleUpdateStatus(inHICommand, ioCmdData);
		break;
	}
	
	return (handled);
}

// ------------------------------------------------------------------------------------------
void
Application::CreateDragPeekerWindow()
{
	SendCreateElementAppleEvent(cWindow, kAEBeginning, GetAEObjectPtr());
}

// ------------------------------------------------------------------------------------------
/*!	Constructs the drag peeker window.
	
	This sample was originally written for Jaguar, where IB doesn't support the scroll 
	view.  This is why we basically retrieve an empty window from the nib file, then 
	add a scroll view then our custom view to it.
	
	If we were to redo this sample as Panther-only, we could put everything in the nib 
	file directly (I think).
*/
B::AEObjectPtr
Application::CreateObject(
	DescType		inObjectClass,
	DescType		inPosition,
	B::AEObjectPtr	inTarget, 
	const AERecord&	inProperties,
	const AEDesc&	inData)
{
	if (inObjectClass != cWindow)
	{
		return (inherited::CreateObject(inObjectClass, inPosition, inTarget, 
										inProperties, inData));
	}
	
	const OptionBits	kScrollerOptions	= kHIScrollViewOptionsVertScroll 
											| kHIScrollViewOptionsHorizScroll 
											| kHIScrollViewOptionsAllowGrow;
	boost::shared_ptr<B::Window>	window;
	B::Rect			bounds;
	B::ScrollView*	scroller;
	
	window = B::Window::CreateFromNib<B::Window>(GetNib(), "MainWindow", GetAEObjectPtr());
	
	bounds = B::WindowUtils::GetContentViewBounds(window->GetWindowRef());
	
	// Create the scroll view.
	
	scroller = B::ScrollView::Create(B::ViewUtils::MakeViewID(), window->GetContentView(), 
									 &bounds, &GetNib(), NULL, kScrollerOptions);
	scroller->SetBindings(kHILayoutBindLeft, kHILayoutBindTop, 
						  kHILayoutBindRight, kHILayoutBindBottom);
	scroller->Show(true);
	
	// Create the drag peeker view.
	
	B::OSPtr<HIViewRef>	viewPtr;
	DragPeekerView*		peeker;
	B::String			title;
	
	boost::tie(viewPtr, peeker) = DragPeekerView::Create(0, *scroller);
	peeker->Show(true);
	
	// Give the window a unique name.
	
	if (++mNextWindowNumber < 2)
	{
		title = "DragPeekerX";
	}
	else
	{
		B::StringFormatter	fmt("DragPeekerX %u");
		
		title = (fmt % mNextWindowNumber).extract();
	}
	
	window->SetTitle(title);
	window->AddToWindowList();
	window->Show(true);
	
//	windowPtr.release();
	
	return (window);
}
