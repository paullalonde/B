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

#ifndef BAbstractPrintPolicy_H_
#define BAbstractPrintPolicy_H_

#pragma once

// B headers
#include "BEventHandler.h"


namespace B {

// forward declarations
class   Bundle;
class   CommandData;
class   PageFormat;
class   Printer;
class   PrintSession;
class   PrintSettings;
class   Window;


/*! @brief  Test print policy
    
    @todo   %Document this class!
*/
class AbstractPrintPolicy : public boost::noncopyable
{
public:
    
    //! @name Types
    //@{
    //! The type of the callback that is invoked before the Page Setup dialog is displayed.
    typedef boost::function1<void, PrintSession&>       DialogInitCallback;
    //! The type of the callback that is invoked after the Page Setup dialog has been displayed.
    typedef boost::function2<void, PrintSession&, bool> DialogResultCallback;
    //! The type of the signal that is triggered whenever the page format is changed.
    typedef boost::signal1<void, PageFormat&>           PageFormatChangedSignal;
    //! The type of the signal that is triggered whenever the print settings are changed.
    typedef boost::signal1<void, PrintSettings&>        PrintSettingsChangedSignal;
    //@}
    
    // constructors / destructor
    virtual ~AbstractPrintPolicy();
    
    //! @name Setup
    //@{
    //! Sets the callback that is invoked before the Page Setup dialog is displayed.
    void    SetPageSetupDialogInitCallback(DialogInitCallback inCallback);
    //! Sets the callback that is invoked after the Page Setup dialog has been displayed.
    void    SetPageSetupDialogResultCallback(DialogResultCallback inCallback);
    //! Sets the callback that is invoked before the Print dialog is displayed.
    void    SetPrintDialogInitCallback(DialogInitCallback inCallback);
    //! Sets the callback that is invoked after the Print dialog has been displayed.
    void    SetPrintDialogResultCallback(DialogResultCallback inCallback);
    //! Returns a signal that is triggered whenever the page format is changed.
    PageFormatChangedSignal&
            GetPageFormatChangedSignal();
    //! Returns a signal that is triggered whenever the print settings are changed.
    PrintSettingsChangedSignal&
            GetPrintSettingsChangedSignal();
    //! Enables the Print menu command.
    void    EnablePrintCommand(bool enable);
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns the policy's page format object.
    PageFormat&     GetPageFormat();
    //! Returns the policy's print settings object.
    PrintSettings&  GetPrintSettings();
    //@}
    
    //! @name Validation
    //@{
    //! Validates the embedded page format object.
    void    ValidatePageFormat();
    //! Validates the embedded print settings object.
    void    ValidatePrintSettings();
    //@}
    
    //! @name Printing
    //@{
    virtual void    Print(
                        PrintSettings*  inPrintSettings,
                        Printer*        inPrinter,
                        bool            inShowDialog);
    //@}
    
protected:
    
    // constructors / destructor
    AbstractPrintPolicy(
        EventTargetRef  inTarget, 
        const Bundle&   inBundle);
    
    virtual void    ShowPageSetupDialog();
    
    // Carbon %Event handlers
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        CommandData&                ioCmdData);
    
    virtual void    FillPageFormat(PageFormat& ioPageFormat) = 0;
    virtual void    FillPrintSettings(PrintSettings& ioPrintSettings) = 0;
    
private:
    
    void    InitEventHandler();

    void    EnsurePageFormat(PrintSession& ioPrintSession);
    void    EnsurePrintSettings(PrintSession& ioPrintSession);
    
    void    ForwardPageSetupResult(bool inAccepted);
    void    ForwardPrintResult(bool inAccepted);
    
    static void DefaultDialogInit(
                    PrintSession&       ioPrintSession);
    static void DefaultDialogResult(
                    PrintSession&       ioPrintSession, 
                    bool                inAccepted);
    
    // Carbon %Event handlers
    bool    CommandProcess(
                Event<kEventClassCommand, kEventCommandProcess>&        event);
    bool    CommandUpdateStatus(
                Event<kEventClassCommand, kEventCommandUpdateStatus>&   event);
    
    // member variables
    EventHandler                        mEventHandler;
    const Bundle&                       mBundle;
    bool                                mPrintEnabled;
    boost::scoped_ptr<PrintSession>     mPrintSession;
    boost::scoped_ptr<PageFormat>       mPageFormat;
    boost::scoped_ptr<PageFormat>       mSavedPageFormat;
    boost::scoped_ptr<PrintSettings>    mPrintSettings;
    boost::scoped_ptr<PrintSettings>    mSavedPrintSettings;
    DialogInitCallback                  mPageSetupInitCallback;
    DialogResultCallback                mPageSetupResultCallback;
    DialogInitCallback                  mPrintInitCallback;
    DialogResultCallback                mPrintResultCallback;
    PageFormatChangedSignal             mPageFormatChangedSignal;
    PrintSettingsChangedSignal          mPrintSettingsChangedSignal;
};

// ------------------------------------------------------------------------------------------
inline void
AbstractPrintPolicy::SetPageSetupDialogInitCallback(DialogInitCallback inCallback)
{
    mPageSetupInitCallback = inCallback;
}

// ------------------------------------------------------------------------------------------
inline void
AbstractPrintPolicy::SetPageSetupDialogResultCallback(DialogResultCallback inCallback)
{
    mPageSetupResultCallback = inCallback;
}

// ------------------------------------------------------------------------------------------
inline void
AbstractPrintPolicy::SetPrintDialogInitCallback(DialogInitCallback inCallback)
{
    mPrintInitCallback = inCallback;
}

// ------------------------------------------------------------------------------------------
inline void
AbstractPrintPolicy::SetPrintDialogResultCallback(DialogResultCallback inCallback)
{
    mPrintResultCallback = inCallback;
}

// ------------------------------------------------------------------------------------------
inline void
AbstractPrintPolicy::EnablePrintCommand(bool enable)
{
    mPrintEnabled = enable;
}

// ------------------------------------------------------------------------------------------
inline AbstractPrintPolicy::PageFormatChangedSignal&
AbstractPrintPolicy::GetPageFormatChangedSignal()
{
    return mPageFormatChangedSignal;
}

// ------------------------------------------------------------------------------------------
inline AbstractPrintPolicy::PrintSettingsChangedSignal&
AbstractPrintPolicy::GetPrintSettingsChangedSignal()
{
    return mPrintSettingsChangedSignal;
}


}   // namespace B


#endif  // BAbstractPrintPolicy_H_
