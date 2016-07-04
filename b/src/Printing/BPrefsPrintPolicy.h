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

#ifndef BPrefsPrintPolicy_H_
#define BPrefsPrintPolicy_H_

#pragma once

// B headers
#include "BAbstractPrintPolicy.h"
#include "BPreferences.h"
#include "BString.h"


namespace B {

// forward declarations
class   Preferences;


/*! @brief  Test print policy
    
    @todo   %Document this class!
*/
class PrefsPrintPolicy : public AbstractPrintPolicy
{
public:
    
    // constructors / destructor
            PrefsPrintPolicy(
                EventTargetRef  inTarget, 
                const Bundle&   inBundle);
    virtual ~PrefsPrintPolicy();
    
    void    SetPreferences(const Preferences& inPreferences);
    void    SetPrefsKey(const String& inPrefsKey);
    
protected:
    
    virtual void    FillPageFormat(PageFormat& ioPageFormat);
    virtual void    FillPrintSettings(PrintSettings& ioPrintSettings);
    
private:
    
    Preferences mPreferences;
    B::String   mPrefsKey;
};


}   // namespace B


#endif  // BPrefsPrintPolicy_H_
