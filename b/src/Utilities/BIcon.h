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

#ifndef BIcon_H_
#define BIcon_H_

#pragma once

// system headers
#include <ApplicationServices/ApplicationServices.h>

// B headers
#include "BAutoRegion.h"
#include "BErrorHandler.h"
#include "BFwd.h"


namespace B {

// forward declarations
class   Point;
class   Rect;
class   Url;

/*!
    @brief  Wrapper class around IconRefs.
    
    The class properly maintains reference counts on the underlying @c IconRef.
    
    @todo   Complete class description!
*/
class Icon
{
public:
    
    //! @name Constructors & Destructor
    //@{
    //! Default constructor.  The icon is set to @c NULL.
                Icon();
    //! Copy constructor.
                Icon(const Icon& inIcon);
    //! Initialises the object with an @c IconRef.
    explicit    Icon(IconRef inIconRef, bool inFromCreateOrCopy);
    //! Initialises the object with the icon of a file system entity.
    explicit    Icon(
                    const Url&  inFileUrl);
    //! Initialises the object with the icon of a file system entity.
    explicit    Icon(
                    const FSRef& inFileRef);
    //! Initialises the object with a system icon.
    explicit    Icon(
                    OSType      inIconType);
    //! Initialises the object with a creator & file type.
    explicit    Icon(
                    OSType      inCreator, 
                    OSType      inIconType, 
                    SInt16      inVRefNum = kOnSystemDisk);
    //! Initialises the object one or more of @a inCreator, @a inFileType, @a inExtension and @a inMIMEType.
    explicit    Icon(
                    OSType      inCreator, 
                    OSType      inFileType, 
                    CFStringRef inExtension,
                    CFStringRef inMIMEType);
    //! Destructor.  Releases the underlying IconRef.
                ~Icon();
    //@}
    
    //! @name Assignment
    //@{
    //! Assignment operator.
    Icon&   operator = (const Icon& inIcon);
    /*! @overload
    */
    Icon&   operator = (const Url& inFileUrl);
    /*! @overload
    */
    Icon&   operator = (OSType inIconType);
    //! Assignment.  The new bundle is retained, and the old one is released.
    Icon&   Assign(const Icon& inIcon);
    /*! @overload
    */
    Icon&   Assign(IconRef inIconRef, bool inFromCreateOrCopy);
    /*! @overload
    */
    Icon&   Assign(const Url& inFileUrl);
    /*! @overload
    */
    Icon&   Assign(const FSRef& inFileRef);
    /*! @overload
    */
    Icon&   Assign(OSType inIconType);
    /*! @overload
    */
    Icon&   Assign(
                OSType  inCreator, 
                OSType  inIconType, 
                SInt16  inVRefNum = kOnSystemDisk);
    /*! @overload
    */
    Icon&   Assign(
                OSType      inCreator, 
                OSType      inFileType, 
                CFStringRef inExtension,
                CFStringRef inMIMEType);
    //@}
    
    //! @name Inquiries
    //@{
    //! Returns @c true if the underlying @c IconRef is @c NULL.
    bool    Empty() const       { return (mRef == NULL); }
    //! Returns the icon's label.
    SInt16  GetLabel() const;
    //@}
    
    //! @name Conversions
    //@{
    //! Returns the object's underlying @c IconRef.
                operator IconRef () const;
    //! Converts the icon into a region.
    AutoRegion  GetRegion(
                    const Rect&         inRect, 
                    IconAlignmentType   inAlignment = kAlignNone) const;
    //! Converts the icon into an @c IconFamilyHandle.
    IconFamilyHandle
                GetIconFamily(
                    IconSelectorValue   inWhichIcons) const;
    //@}
    
    //! @name Drawing
    //@{
    //! Draws the icon in the current @c GrafPort.
    void    Draw(
                const Rect&         inRect, 
                SInt16              inLabel = 0, 
                IconAlignmentType   inAlignment = kAlignNone, 
                IconTransformType   inTransform = kTransformNone) const;
    //! Draws the icon in the given CoreGraphics context.
    void    Draw(
                CGContextRef        inContext, 
                const Rect&         inRect, 
                SInt16              inLabel = 0, 
                IconAlignmentType   inAlignment = kAlignNone, 
                IconTransformType   inTransform = kTransformNone) const;
    //! Draws the icon upside down in the given CoreGraphics context.
    void    DrawFlipped(
                CGContextRef        inContext, 
                const Rect&         inRect, 
                SInt16              inLabel = 0, 
                IconAlignmentType   inAlignment = kAlignNone, 
                IconTransformType   inTransform = kTransformNone) const;
    //@}
    
    //! @name Hit-Testing
    //@{
    //! Determines if @a inTestPoint is within the icon.
    bool    Contains(
                const Point&        inTestPoint,
                const Rect&         inRect, 
                IconAlignmentType   inAlignment = kAlignNone) const;
    //! Determines if @a inTestRect intersects the icon.
    bool    Intersects(
                const Rect&         inTestRect,
                const Rect&         inRect, 
                IconAlignmentType   inAlignment = kAlignNone) const;
    //@}
    
    //! @name Registration
    //@{
    //! Associates a .icns file with the (@a inCreator, @a inIconType) pair.
    static Icon Register(
                    OSType              inCreator, 
                    OSType              inIconType, 
                    const Url&          inIcnsFileUrl);
    //! Associates icon family data with the (@a inCreator, @a inIconType) pair.
    static Icon Register(
                    OSType              inCreator, 
                    OSType              inIconType, 
                    IconFamilyHandle    inIconFamily);
    //! Associates a 'icns' resource with the (@a inCreator, @a inIconType) pair.
//  static Icon Register(
//                  OSType              inCreator, 
//                  OSType              inIconType, 
//                  const Url&          inResourceFileUrl, 
//                  SInt16              inResourceID);
    //! Removes the association between the (@a inCreator, @a inIconType) pair and icon data.
    static void Unregister(
                    OSType              inCreator, 
                    OSType              inIconType);
    //@}
    
private:
    
    // member variables
    IconRef mRef;
    SInt16  mLabel;
};

// ------------------------------------------------------------------------------------------
inline Icon&
Icon::operator = (const Icon& inIcon)
{
    return (Assign(inIcon));
}

// ------------------------------------------------------------------------------------------
inline Icon&
Icon::operator = (const Url& inFileUrl)
{
    return (Assign(inFileUrl));
}

// ------------------------------------------------------------------------------------------
inline Icon&
Icon::operator = (OSType inIconType)
{
    return (Assign(inIconType));
}

// ------------------------------------------------------------------------------------------
inline
Icon::operator IconRef () const
{
    B_ASSERT(mRef != NULL);
    return (mRef);
}

// ------------------------------------------------------------------------------------------
/*! The label contains a useful value only if the icon was obtained from a 
    file-system entity.
*/
inline SInt16
Icon::GetLabel() const
{
    return (mLabel);
}


}   // namespace B


#endif  // BIcon_H_
