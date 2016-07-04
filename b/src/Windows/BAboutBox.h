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

#ifndef BAboutBox_H_
#define BAboutBox_H_

#pragma once

// B headers
#include "BWindow.h"


namespace B {


// forward declarations
class   Bundle;
class   StaticTextView;


/*! @brief  A simple, self-contained about box.
    
    @todo   %Document this class!
*/
class AboutBox : public Window
{
public:
    
    //! Displays the About Box.
    static void     Display(
                        AEObjectPtr     inContainer, 
                        const Bundle&   inBundle, 
                        const String&   inDescription = String());
    
    //! Constructor.
                    AboutBox(
                        WindowRef       inWindowRef,
                        AEObjectPtr     inContainer); 

protected:
    
    //! Destructor.
    virtual         ~AboutBox();
    
    virtual void    Awaken(Nib* inFromNib);
    
    // overrides from AEObject
    virtual void    MakeSpecifier(
                        AEWriter&       ioWriter) const;
    
private:
    
    // member variables
    const Bundle&       mBundle;
    StaticTextView*     mName;
    StaticTextView*     mVersion;
    StaticTextView*     mDescription;
    StaticTextView*     mCopyright;
    String              mDescriptionStr;
    
    // static member variables
    static const Bundle*    sBundle;
    static String           sDescriptionStr;
    static boost::shared_ptr<AboutBox>  sAboutBox;
};

}   // namespace B


#endif // BAboutBox_H_
