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

#ifndef BNullPrintPolicy_H_
#define BNullPrintPolicy_H_

#pragma once


namespace B {

// forward declarations
class   Bundle;
class   DialogModality;
class   PageFormat;
class   Printer;
class   PrintSession;
class   PrintSettings;


/*!
    @brief  Do-nothing print policy.
    
    This class is a stub implementation of PRINT_POLICY.  Most of its member functions 
    do nothing.
    
    This class can be used as a template parameter to those classes that take an 
    PRINT_POLICY template parameter.
    
    Currently, two classes take undo policies as template parameters:  Application and 
    Document.
*/
class NullPrintPolicy : public boost::noncopyable
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
    
    //! @name Constructor & Destructor
    //@{
    //! Constructor.
            NullPrintPolicy(EventTargetRef, const Bundle&)  {}
    //! Destructor.
    virtual ~NullPrintPolicy();
    //@}
    
    //! @name Setup
    //@{
    //! Sets the callback that is invoked before the Page Setup dialog is displayed.
    void    SetPageSetupDialogInitCallback(DialogInitCallback)      {}
    //! Sets the callback that is invoked after the Page Setup dialog has been displayed.
    void    SetPageSetupDialogResultCallback(DialogResultCallback)  {}
    //! Sets the callback that is invoked before the Print dialog is displayed.
    void    SetPrintDialogInitCallback(DialogInitCallback)          {}
    //! Sets the callback that is invoked after the Print dialog has been displayed.
    void    SetPrintDialogResultCallback(DialogResultCallback)      {}
    //! Returns a signal that is triggered whenever the page format is changed.
    PageFormatChangedSignal&    GetPageFormatChangedSignal();
    //! Returns a signal that is triggered whenever the print settings are changed.
    PrintSettingsChangedSignal& GetPrintSettingsChangedSignal();
    //! Enables the Print menu command.
    void    EnablePrintCommand(bool)    {}
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
    void    ValidatePageFormat()    {}
    //! Validates the embedded print settings object.
    void    ValidatePrintSettings() {}

    //@}
    
    //! @name Printing
    //@{
    
    //! Initiates the printing process.
    void    Print(PrintSettings*, Printer*, bool)   {}
    //@}
};


}   // namespace B


#endif  // BNullPrintPolicy_H_
