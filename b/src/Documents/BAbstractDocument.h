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

#ifndef BAbstractDocument_H_
#define BAbstractDocument_H_

#pragma once

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/signal.hpp>

// B headers
#include "BAEObject.h"


namespace B {

// forward declarations
class   CommandData;
class   Window;
class   Nib;
class   Printer;
class   PrintSettings;
class   SaveInfo;


/*!
    @brief  The base class representing documents.
    
    @todo   Complete documentation.
*/
class AbstractDocument : public AEObject
{
public:
    
    //! @name Types
    //@{
    //!< Internal type.
    class                                           EventHelper;
    //! Shorthand for a type of signal used by this class.
    typedef boost::signal0<void>                    VoidSignal;
    //! Shorthand for a type of signal used by this class.
    typedef boost::signal1<void, AbstractDocument*> DocSignal;
    //@}
    
    //! @name Constructor & Destructor
    //@{
    //! Constructor.
                    AbstractDocument(
                        AEObjectPtr inContainer,
                        DescType    inClassID, 
                        SInt32      inUniqueID);
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns @c true is the document does @b not have a backing store.
    virtual bool        IsNew() const;
    
    //! Returns @c true if the document has any unsaved changes.
    virtual bool        IsModified() const;
    
    //! Registers a change to the document.
    virtual void        Modified();
    
    //! Unregisters a change to the document.
    virtual void        Unmodified();
    
    //! Returns the document's unique id.  This id is not persistent.
    virtual SInt32      GetUniqueID() const;
    
    //! Returns the document's name.
    virtual String      GetName() const;
    
    //! Returns the document's display name.
    virtual String      GetDisplayName() const;
    
    //! Returns the document's creator code.
    virtual OSType      GetCreator() const;
    
    //! Returns the document's event target.
    EventTargetRef      GetEventTarget() const;
    
    /*! @brief  Returns the document's type.
        
        The document type will normally be listed in the application's Info.plist file.
        
        This is a pure virtual function, which all derived classes must override.
    */
    virtual String      GetDocumentType() const = 0;

    /*! @brief  Returns the document's URL.
        
        This URL is used to identify the document within the B framework, so any 
        data set describable by a URL can potentially be put in a document.
        
        This is a pure virtual function, which all derived classes must override.
    */
    virtual Url         GetUrl() const = 0;
    
    /*! @brief  Returns @c true if @a inWindow belongs to the document.
        This is a pure virtual function, which all derived classes must override.
    */
    virtual bool        OwnsWindow(const Window* inWindow) const = 0;
    //@}
    
    //! @name Initialisation
    //@{
    //! Initialises a new document.
    virtual void    InitNewDocument(
                        const String&   inName,
                        const String&   inDocumentType, 
                        const AEDesc&   inProperties, 
                        const AEDesc&   inData,
                        Nib*            inNib);
    //! Initialises a document from the contents of a URL.
    virtual void    InitOpenDocument(
                        const Url&      inDocumentUrl, 
                        const String&   inDocumentType, 
                        Nib*            inNib, 
                        bool            inForPrinting);
    //@}
    
    //! @name Saving
    //@{
    //! Saves the document.
    virtual void    SaveDocument(
                        SaveInfo&       ioInfo);
    //! Reverts the document.
    virtual void    RevertDocument();
    //! Prints the document.
    virtual void    PrintDocument(
                        PrintSettings*  inPrintSettings,
                        Printer*        inPrinter,
                        bool            inShowDialog);
    //@}
    
    // document management
    virtual void    MakeCurrent();
    virtual bool    CloseDocument(
                        OSType          inSaveOption, 
                        const Url&      inUrl = Url(), 
                        Window*         inWindow = NULL);
    virtual void    Close();
    virtual void    Quitting(
                        OSType          inSaveOption);
    virtual void    CancelQuitting();
    bool            IsQuitting() const  { return (mQuitting); }
                    
    //! @name Overrides From AEObject
    //@{
    virtual void    WriteProperty(
                        DescType        inPropertyID, 
                        AEWriter&       ioWriter) const;
    virtual void    CloseObject(
                        OSType          inSaveOption = kAEAsk, 
                        const Url&      inUrl = Url());
    virtual void    SaveObject(
                        const Url&      inUrl = Url(),
                        const String&   inObjectType = String());
    virtual void    RevertObject();
    virtual void    PrintObject(
                        PrintSettings*  inSettings, 
                        Printer*        inPrinter, 
                        bool            inShowDialog);
    //@}
    
    //! @name Signals
    //@{
    //! Sent when the document becomes dirty (or clean).  Usually due to model changes, or Save or Save As.
    DocSignal&  GetDirtyStateChangedSignal()    { return (mDirtyStateChangedSignal); }
    //! Sent when the document's URL has changed.  Usually due to a Save or Save As.
    DocSignal&  GetUrlChangedSignal()           { return (mUrlChangedSignal); }
    //! Sent when the document's content changes completely.  Usually due to a Revert.
    DocSignal&  GetContentChangedSignal()       { return (mContentChangedSignal); }
    //! Sent when the document is about to be closed.
    DocSignal&  GetClosingSignal()              { return (mClosingSignal); }
    //! Sent when the user has cancelled an async action (i.e. Quit with multiple documents containing unsaved changes).
    VoidSignal& GetAsyncActionCancelledSignal() { return (mAsyncActionCancelledSignal); }
    //! Sent when the the document should be deleted (IMPORTANT: only the document policy should be listening to this !!).
    DocSignal&  GetDeleteSignal()               { return (mDeleteSignal); }
    //@}
    
protected:
    
    virtual void    MakeWindows(
                        Nib*            inNib);
    virtual void    ShowWindows();
    virtual void    CloseWindows();
    virtual void    InitDocument(
                        const String&   inName,
                        const String&   inDocumentType, 
                        const AEDesc&   inProperties, 
                        const AEDesc&   inData);
    virtual void    ReadDocument(
                        const Url&      inDocumentUrl, 
                        String&         ioDocumentType);
    virtual void    DoneReading(
                        const Url&      inDocumentUrl, 
                        const String&   inDocumentType);
    virtual void    CloseFiles();
    virtual bool    AskUserToClose(
                        bool            inFromUser, 
                        Window*         inWindow, 
                        NavAskSaveChangesAction inContext);
    virtual bool    AskUserToSave(
                        SaveInfo&       ioSaveInfo);
    virtual bool    AskUserToRevert(
                        Window*         inWindow);
    virtual void    DonePrinting();
    
    //! @name Saving
    //@{
    //! Saves the document to a new location or with a new type.
    virtual void    WriteDocumentToUrl(
                        SaveInfo&       ioSaveInfo);
    //! Saves the document to its current location and with its current type.
    virtual void    WriteDocument(
                        SaveInfo&       ioSaveInfo);
    //! Performs post-save tidying up.
    virtual void    WriteComplete(
                        const Url&      inOldUrl, 
                        const String&   inOldDocumentType);
    //@}
    
    //! @name Utility
    //@{
    //! Forces the change count to zero.
    virtual void    ClearModified();
    //! Broadcasts the "content changed" signal.
    virtual void    NotifyContentChanged();
    //! Broadcasts the "url changed" signal.
    virtual void    NotifyUrlChanged();
    //! Broadcasts the "async action cancelled" signal.
    virtual void    NotifyActionCancelled();
    //@}
    
    // overrides from AEObject
    //@{
    virtual void    MakeSpecifier(
                        AEWriter&       ioWriter) const;
    //@}
    
    ///! @name Carbon %Event handlers
    //@{
    virtual bool    HandleCommand(
                        const HICommandExtended&    inHICommand, 
                        Window*                     inWindow = NULL);
    virtual bool    HandleUpdateStatus(
                        const HICommandExtended&    inHICommand, 
                        CommandData&                ioCommandData, 
                        Window*                     inWindow = NULL);
    //@}
    
private:
    
    // member variables
    const SInt32        mUniqueID;
    OSPtr<HIObjectRef>  mObjectRef;
    DocSignal           mContentChangedSignal;
    DocSignal           mDirtyStateChangedSignal;
    DocSignal           mUrlChangedSignal;
    DocSignal           mClosingSignal;
    VoidSignal          mAsyncActionCancelledSignal;
    DocSignal           mDeleteSignal;
    unsigned            mModCount;
    bool                mOpenForPrinting;
    bool                mQuitting;
    
    // friend
    friend class    EventHelper;
};


}   // namespace B


#endif  // BAbstractDocument_H_
