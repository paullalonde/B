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

#ifndef PListerWin_H_
#define PListerWin_H_

#pragma once

// system headers
#include <CoreFoundation/CFPropertyList.h>

// project headers
#include "BDocumentWindow.h"
#include "BPreferences.h"

// project headers
#include "PListerDoc.h"


// forward declarations
class   Browser;


class PListerWin : public B::DocumentWindow<PListerDoc>
{
public:

    // constructor
                    PListerWin(
                        WindowRef       inWindowRef,
                        PListerDoc*     inDocument);

protected:
    
    virtual void    RegisterViews();
    virtual void    Awaken(B::Nib* inFromNib);
    virtual void    LoadContent();
    virtual B::Size GetIdealSize() const;
    
    // notifications
    void        BrowserSelectionChanged();
    void        BrowserUserStateChanged();
    
    // Carbon %Event handlers
    virtual bool    HandleFocusAcquired();
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        B::CommandData&             ioCommandData);
    
private:
    
    typedef B::DocumentWindow<PListerDoc>   inherited;
    
    void    UpdateButtons();
    
    // member variables
    PListerDoc*     mPListerDoc;
    Browser*        mBrowser;
    B::View*        mNewChildButton;
    B::View*        mNewSiblingButton;
    B::View*        mDeleteButton;
    B::View*        mMoveDownButton;
    B::View*        mMoveUpButton;
    B::Preferences  mPreferences;
};

#endif // PListerWin_H_
