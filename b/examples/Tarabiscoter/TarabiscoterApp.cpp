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
#include "TarabiscoterApp.h"


#if !defined(NDEBUG) && defined(__MWERKS__)
#   include <DataViewer.h>
#endif

// B headers
#include "BDialog.h"

// project headers
#include "DialogsWindow.h"
#include "DrawerHost.h"
#include "Layout.h"
#include "OpenGLWindow.h"
#include "NavigationWindow.h"
#include "Split.h"
#include "TextWindow.h"

// for template instantiation
#include "BApplication.tpl.h"

extern "C" { int NSApplicationLoad(void); }

// ==========================================================================================
//  main

int main()
{
#if !defined(NDEBUG) && defined(__MWERKS__)
    DataViewLibInit();
#endif
    
    NSApplicationLoad();
    
    TarabiscoterApp app;
    
    app.Run();
    
    return 0;
}


// ==========================================================================================
//  TarabiscoterApp

#pragma mark -

// ------------------------------------------------------------------------------------------
TarabiscoterApp::TarabiscoterApp()
{
    // There's nothing to do.
}

// ------------------------------------------------------------------------------------------
TarabiscoterApp::~TarabiscoterApp()
{
    // There's nothing to do.
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::RegisterViews(B::ViewFactory& ioFactory)
{
    // Call base class to perform view registration.
    inherited::RegisterViews(ioFactory);
}

// ------------------------------------------------------------------------------------------
/*! Handle menu commands.  We just handle the New command (to display a new drag peeker 
    window).
*/
bool
TarabiscoterApp::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case 'Drwr':
        CreateDrawerWindow();
        break;
        
    case 'LayC':
        CreateLayoutWindow();
        break;
        
    case 'GLwC':
        CreateOpenGLWindow();
        break;
        
    case 'SplC':
        CreateSplitWindow();
        break;
        
    case 'Dlgs':
        CreateDialogsWindow();
        break;
        
    case 'NavS':
        CreateNavigationWindow();
        break;
        
    case 'Text':
        CreateTextWindow();
        break;
        
    case 'Btds':
        BluetoothDeviceSelector();
        break;
        
    case 'Btsb':
        BluetoothServiceBrowser();
        break;
        
    default:
        handled = inherited::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! Update the status of menu commands.  We just enable the New command.
*/
bool
TarabiscoterApp::HandleUpdateStatus(
    const HICommandExtended&    inHICommand, 
    B::CommandData&             ioCmdData)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case 'Drwr':
    case 'LayC':
    case 'GLwC':
    case 'SplC':
    case 'Dlgs':
    case 'NavS':
    case 'Text':
        ioCmdData.SetEnabled(true);
        break;
        
    case 'Btds':
    case 'Btsb':
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
TarabiscoterApp::CreateDrawerWindow()
{
    boost::shared_ptr<DrawerHost>   drawerHost(B::Window::CreateFromNib<DrawerHost>(
                                                    GetNib(), "DrawerHost", GetAEObjectPtr()));
    
    drawerHost->AddToWindowList();
    drawerHost->Show(true);
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::CreateLayoutWindow()
{
    boost::shared_ptr<Layout>   layout(B::Window::CreateFromNib<Layout>(
                                                GetNib(), "layout", GetAEObjectPtr()));
    
    layout->AddToWindowList();
    layout->Show(true);
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::CreateOpenGLWindow()
{
    boost::shared_ptr<OpenGLWindow> opengl(B::Window::CreateFromNib<OpenGLWindow>(
                                                GetNib(), "opengl", GetAEObjectPtr()));
    
    opengl->AddToWindowList();
    opengl->Show(true);
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::CreateSplitWindow()
{
    boost::shared_ptr<Split>    split(B::Window::CreateFromNib<Split>(
                                                GetNib(), "split", GetAEObjectPtr()));
    
    split->AddToWindowList();
    split->Show(true);
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::CreateDialogsWindow()
{
    boost::shared_ptr<DialogsWindow>    dialogs(B::Window::CreateFromNib<DialogsWindow>(
                                                GetNib(), "dialog-host", GetAEObjectPtr()));
    
    dialogs->AddToWindowList();
    dialogs->Show(true);
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::CreateNavigationWindow()
{
    boost::shared_ptr<NavigationWindow> navigation(B::Window::CreateFromNib<NavigationWindow>(
                                                GetNib(), "nav", GetAEObjectPtr()));
    
    navigation->AddToWindowList();
    navigation->Show(true);
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::CreateTextWindow()
{
    boost::shared_ptr<TextWindow>   text(B::Window::CreateFromNib<TextWindow>(
                                                GetNib(), "textw", GetAEObjectPtr()));
    
    text->AddToWindowList();
    text->Show(true);
}

#include <IOBluetooth/Bluetooth.h>
#include <IOBluetooth/BluetoothAssignedNumbers.h>
#include <IOBluetoothUI/IOBluetoothUIUserLib.h>

#include "BAutoreleasePool.h"

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::BluetoothDeviceSelector()
{
    IOBluetoothDeviceSearchAttributes   attrs;
    IOBluetoothDeviceSearchDeviceAttributes search1;
    
    memset(&search1, 0, sizeof(search1));
    
    search1.serviceClassMajor = kBluetoothServiceClassMajorAny;
    search1.deviceClassMajor = kBluetoothDeviceClassMajorPhone;
    search1.deviceClassMinor = kBluetoothDeviceClassMinorAny;
    
    attrs.options = kSearchOptionsNone;
    attrs.maxResults = 10;
    attrs.deviceAttributeCount = 0;
    attrs.attributeList = NULL;
    
    B::AutoreleasePool  pool;
    
    IOBluetoothDeviceSelectorControllerRef  ref = IOBluetoothGetDeviceSelectorController();
    B::OSPtr<CFArrayRef> selectedDevices(IOBluetoothDeviceSelectorRunPanelWithAttributes(
                                            ref, &attrs), B::from_copy);
                                            
    CFShow(selectedDevices.get());
}

// ------------------------------------------------------------------------------------------
void
TarabiscoterApp::BluetoothServiceBrowser()
{
    IOBluetoothServiceBrowserControllerOptions  opts;
    
    opts = kIOBluetoothServiceBrowserControllerOptionsDisconnectWhenDone;
    
    IOBluetoothSDPServiceRecordRef sdpRef;
    IOReturn result;
    
    result = IOBluetoothServiceBrowserControllerBrowseDevices(&sdpRef, opts);
    
#if 0
    IOBluetoothServiceBrowserControllerRef ref;
    
    ref = IOBluetoothServiceBrowserControllerCreate(opts);
    
    
    result = IOBluetoothServiceBrowserControllerDiscover(ref, 
    IOBluetoothDeviceSearchDeviceAttributes search1;
    
    memset(&search1, 0, sizeof(search1));
    
    search1.serviceClassMajor = kBluetoothServiceClassMajorAny;
    search1.deviceClassMajor = kBluetoothDeviceClassMajorPhone;
    search1.deviceClassMinor = kBluetoothDeviceClassMinorAny;
    
    attrs.options = kSearchOptionsNone;
    attrs.maxResults = 10;
    attrs.deviceAttributeCount = 0;
    attrs.attributeList = NULL;
    
    B::AutoreleasePool  pool;
    
    IOBluetoothDeviceSelectorControllerRef  ref = IOBluetoothGetDeviceSelectorController();
    B::OSPtr<CFArrayRef> selectedDevices(IOBluetoothDeviceSelectorRunPanelWithAttributes(
                                            ref, &attrs), B::from_copy);
#endif
                                            
    printf("allo\n");
}
