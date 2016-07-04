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
#include "DragPeekerView.h"

// B headers
#include "BClipboard.h"
#include "BDrag.h"
#include "BEvent.h"
#include "BPasteboardStream.h"
#include "BStringUtilities.h"
#include "CFUtils.h"


#define VIEW_FLAGS  (kViewOpaque | kViewDragAndDrop | kViewScrollable | \
                     kViewFocus | kViewFocusOnClick | kViewCommands)

const CFStringRef   DragPeekerView::kHIObjectClassID        = 
                        CFSTR("ca.paullalonde.b.examples.DragPeekerX.dragpeekerview");

// ------------------------------------------------------------------------------------------
B::EventTarget*
DragPeekerView::Instantiate(HIObjectRef inObjectRef)
{
    return (new DragPeekerView(inObjectRef));
}

// ------------------------------------------------------------------------------------------
std::pair<B::OSPtr<HIViewRef>, DragPeekerView*>
DragPeekerView::Create(
    SInt32          inViewID /* = 0 */,         //!< The new view's ID; may be zero in which case the view doesn't get an @a HIViewID.
    HIViewRef       inSuperview /* = NULL */,   //!< The new view's superview;  may be @c NULL, in which case the view is "standalone".
    const B::Rect*  inFrame /* = NULL */,       //!< The new view's frame;  may be @c NULL, in which case the view doesn't get a frame with particular values.  If @a inSuperview is not @c NULL, the frame is assumed to be in @a inSuperview's coordinates.
    B::Nib*         inFromNib /* = NULL */)     //!< The nib from which to load other resources;  may be @c NULL.
{
    return (B::HybridView::Create<DragPeekerView>(B::ViewUtils::MakeViewID(inViewID), 
                                                  inSuperview, inFrame, inFromNib,
                                                  static_cast<EventRef>(NULL)));
}

// ------------------------------------------------------------------------------------------
DragPeekerView::DragPeekerView(HIObjectRef inObjectRef)
    : B::HybridView(inObjectRef, VIEW_FLAGS),
      mEmptyStr(CFUSTR("Drag into this window.")), 
      mCurrentDragRef(NULL), mFontSize(12.0f), mLineHeight(14.0f)
{
}

// ------------------------------------------------------------------------------------------
const B::CustomView::ViewPartVector&
DragPeekerView::GetViewParts() const
{
    static ViewPartVector   sViewParts(1, ViewPart(kControlPicturePart, kFocusNever));
    
    return (sViewParts);
}

// ------------------------------------------------------------------------------------------
ControlKind
DragPeekerView::GetKind() const
{
    ControlKind kind    = { kControlKindSig, kControlKind };
    
    return (kind);
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::Draw(
    ControlPartCode /* inPart */, 
    CGContextRef    inContext, 
    const B::Shape* /* inShape */)
{
    B::Rect     bounds  = GetBounds();
    OSStatus    err;
    
    CGContextSaveGState(inContext);
    
    CGContextSetRGBFillColor(inContext, 1.0, 1.0, 1.0, 1.0);
    CGContextFillRect(inContext, bounds);
    
    CGContextSetRGBFillColor(inContext, 0.0, 0.0, 0.0, 1.0);
    
    if (mDragItems.empty())
    {
        HIThemeTextInfo textInfo;
        B::Rect         textBounds;
        OSStatus        err;
        
        GetTextInfo(textInfo, textBounds);
        
        textBounds.center(textBounds.center(), mEmptyStrSize);

        err = HIThemeDrawTextBox(mEmptyStr, &textBounds, &textInfo, inContext, 
                                 kHIThemeOrientationNormal);
        B_THROW_IF_STATUS(err);
    }
    else
    {
        const char* kLabelFont      = "Helvetica";
        const char* kDataFont       = "Courier";
        const char* kDataBoldFont   = "Courier-Bold";
        const float kLeftMargin     = 4.0f;
        const float kItemMargin     = 30.0f;
        const float kFlavorMargin   = 60.0f;
        const float kDataMargin     = 150.0f;
        float       currLine        = mLineHeight;
        char        buff[256];
        
        CGAffineTransform transform = CGAffineTransformIdentity;
        // adjust the transform so the text doesn't draw upside down
        transform = CGAffineTransformScale(transform, 1, -1);
        CGContextSetTextMatrix(inContext, transform);
        
        CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kLeftMargin, currLine);
        ShowText(inContext, "Attributes:");
        
        CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kDataMargin, currLine);
        sprintf(buff, "%8.8lx", mDragAttributes);
        ShowText(inContext, buff);
        
        if (mDragAttributes != 0)
        {
            const char* comma   = "";
            
            strcpy(buff, " (");
            
            if (mDragAttributes & kDragHasLeftSenderWindow)
            {
                strcat(buff, comma);
                strcat(buff, "HasLeftSenderWindow");
                comma = ", ";
            }
            
            if (mDragAttributes & kDragInsideSenderApplication)
            {
                strcat(buff, comma);
                strcat(buff, "InsideSenderApplication");
                comma = ", ";
            }
            
            if (mDragAttributes & kDragInsideSenderWindow)
            {
                strcat(buff, comma);
                strcat(buff, "InsideSenderWindow");
                comma = ", ";
            }
            
            strcat(buff, ")");
            
            CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
            ShowText(inContext, buff);
        }
        currLine += mLineHeight;
        
        CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kLeftMargin, currLine);
        ShowText(inContext, "Modifiers:");
        
        CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kDataMargin, currLine);
        sprintf(buff, "%4.4hx %4.4hx %4.4hx", 
                mDragModifiers, mDragMouseDownModifiers, mDragMouseUpModifiers);
        ShowText(inContext, buff);
        currLine += mLineHeight;
        
        CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kLeftMargin, currLine);
        ShowText(inContext, "Allowable Actions:");
        
        CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kDataMargin, currLine);
        sprintf(buff, "%8.8lx", mDragAllowableActions);
        ShowText(inContext, buff);
        
        if (mDragAllowableActions != 0)
        {
            strcpy(buff, " (");
            
            if (mDragAllowableActions != static_cast<DragActions>(kDragActionAll))
            {
                const char* comma   = "";
                
                if (mDragAllowableActions & kDragActionAlias)
                {
                    strcat(buff, comma);
                    strcat(buff, "Alias");
                    comma = ", ";
                }
                
                if (mDragAllowableActions & kDragActionGeneric)
                {
                    strcat(buff, comma);
                    strcat(buff, "Generic");
                    comma = ", ";
                }
                
                if (mDragAllowableActions & kDragActionPrivate)
                {
                    strcat(buff, comma);
                    strcat(buff, "Private");
                    comma = ", ";
                }
                
                if (mDragAllowableActions & kDragActionMove)
                {
                    strcat(buff, comma);
                    strcat(buff, "Move");
                    comma = ", ";
                }
                
                if (mDragAllowableActions & kDragActionDelete)
                {
                    strcat(buff, comma);
                    strcat(buff, "Delete");
                    comma = ", ";
                }
            }
            else
            {
                strcat(buff, "All");
            }
            
            strcat(buff, ")");
            
            CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
            ShowText(inContext, buff);
        }
        currLine += mLineHeight;
        
        CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kLeftMargin, currLine);
        ShowText(inContext, "Drag Action:");
        
        CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kDataMargin, currLine);
        sprintf(buff, "%8.8lx", mDragAction);
        ShowText(inContext, buff);
        
        if (mDragAction != 0)
        {
            strcpy(buff, " (");
            
            if (mDragAction != static_cast<DragActions>(kDragActionAll))
            {
                const char* comma   = "";
                
                if (mDragAction & kDragActionAlias)
                {
                    strcat(buff, comma);
                    strcat(buff, "Alias");
                    comma = ", ";
                }
                
                if (mDragAction & kDragActionGeneric)
                {
                    strcat(buff, comma);
                    strcat(buff, "Generic");
                    comma = ", ";
                }
                
                if (mDragAction & kDragActionPrivate)
                {
                    strcat(buff, comma);
                    strcat(buff, "Private");
                    comma = ", ";
                }
                
                if (mDragAction & kDragActionMove)
                {
                    strcat(buff, comma);
                    strcat(buff, "Move");
                    comma = ", ";
                }
                
                if (mDragAction & kDragActionDelete)
                {
                    strcat(buff, comma);
                    strcat(buff, "Delete");
                    comma = ", ";
                }
            }
            else
            {
                strcat(buff, "All");
            }
            
            strcat(buff, ")");
            
            CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
            ShowText(inContext, buff);
        }
        currLine += mLineHeight;
        
        CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kLeftMargin, currLine);
        ShowText(inContext, "Number of items:");
        
        CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
        CGContextSetTextPosition(inContext, kDataMargin, currLine);
        sprintf(buff, "%lu", mDragItems.size());
        ShowText(inContext, buff);
        currLine += mLineHeight;
        
        for (size_t i = 0; i < mDragItems.size(); i++)
        {
            const ItemData& itemData    = mDragItems[i];
            
            currLine += mLineHeight;
            
            CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
            CGContextSetTextPosition(inContext, kLeftMargin, currLine);
            sprintf(buff, "%lu:", i+1);
            ShowText(inContext, buff);
            CGContextSetTextPosition(inContext, kItemMargin, currLine);
            ShowText(inContext, "Item reference:");
            
            CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
            CGContextSetTextPosition(inContext, kDataMargin, currLine);
            sprintf(buff, "%p", itemData.mItemID);
            ShowText(inContext, buff);
            currLine += mLineHeight;
            
            CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
            CGContextSetTextPosition(inContext, kItemMargin, currLine);
            ShowText(inContext, "Item bounds:");
            
            CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
            CGContextSetTextPosition(inContext, kDataMargin, currLine);
            sprintf(buff, "[%g, %g] [%g, %g]", 
                    itemData.mBounds.origin.x, itemData.mBounds.origin.y, 
                    itemData.mBounds.size.width, itemData.mBounds.size.height);
            ShowText(inContext, buff);
            currLine += mLineHeight;
            
            CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
            CGContextSetTextPosition(inContext, kItemMargin, currLine);
            ShowText(inContext, "Number of flavors:");
            
            CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
            CGContextSetTextPosition(inContext, kDataMargin, currLine);
            sprintf(buff, "%lu", itemData.mFlavors.size());
            ShowText(inContext, buff);
            currLine += mLineHeight;
            
            for (size_t j = 0; j < itemData.mFlavors.size(); j++)
            {
                const FlavorData&   flavorData  = itemData.mFlavors[j];
                std::string         flavorType  = B::make_utf8_string(flavorData.mFlavor.cf_ref());
                
                currLine += mLineHeight;
                
                CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kItemMargin, currLine);
                sprintf(buff, "%lu:", j+1);
                ShowText(inContext, buff);
                CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                ShowText(inContext, "Flavor type:");
                
                CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kDataMargin, currLine);
                CGContextShowText(inContext, flavorType.c_str(), flavorType.size());
                currLine += mLineHeight;
                
                if (!flavorData.mOSType.empty())
                {
                    std::string flavorOSType    = B::make_utf8_string(flavorData.mOSType.cf_ref());
                    
                    CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                    ShowText(inContext, "OSType:");
                    
                    CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kDataMargin, currLine);
                    CGContextShowText(inContext, flavorOSType.c_str(), flavorOSType.size());
                    currLine += mLineHeight;
                }
                
                if (!flavorData.mNSType.empty())
                {
                    std::string flavorNSType    = B::make_utf8_string(flavorData.mNSType.cf_ref());
                    
                    CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                    ShowText(inContext, "NSPasteboard:");
                    
                    CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kDataMargin, currLine);
                    CGContextShowText(inContext, flavorNSType.c_str(), flavorNSType.size());
                    currLine += mLineHeight;
                }
                
                if (!flavorData.mFileType.empty())
                {
                    std::string flavorFileType  = B::make_utf8_string(flavorData.mFileType.cf_ref());
                    
                    CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                    ShowText(inContext, "Extension:");
                    
                    CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kDataMargin, currLine);
                    CGContextShowText(inContext, flavorFileType.c_str(), flavorFileType.size());
                    currLine += mLineHeight;
                }
                
                if (!flavorData.mMimeType.empty())
                {
                    std::string flavorMimeType  = B::make_utf8_string(flavorData.mMimeType.cf_ref());
                    
                    CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                    ShowText(inContext, "MIME Type:");
                    
                    CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                    CGContextSetTextPosition(inContext, kDataMargin, currLine);
                    CGContextShowText(inContext, flavorMimeType.c_str(), flavorMimeType.size());
                    currLine += mLineHeight;
                }

                CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                ShowText(inContext, "Flavor flags:");
                
                CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kDataMargin, currLine);
                sprintf(buff, "%8.8lx", flavorData.mFlags);
                ShowText(inContext, buff);
                if (flavorData.mFlags != 0)
                {
                    const char* comma   = "";
                    
                    strcpy(buff, " (");
                    
                    if (flavorData.mFlags & flavorSenderOnly)
                    {
                        strcat(buff, comma);
                        strcat(buff, "SenderOnly");
                        comma = ", ";
                    }
                    
                    if (flavorData.mFlags & flavorSenderTranslated)
                    {
                        strcat(buff, comma);
                        strcat(buff, "SenderTranslated");
                        comma = ", ";
                    }
                    
                    if (flavorData.mFlags & flavorNotSaved)
                    {
                        strcat(buff, comma);
                        strcat(buff, "NotSaved");
                        comma = ", ";
                    }
                    
                    if (flavorData.mFlags & flavorSystemTranslated)
                    {
                        strcat(buff, comma);
                        strcat(buff, "SystemTranslated");
                        comma = ", ";
                    }
                    
                    if (flavorData.mFlags & flavorDataPromised)
                    {
                        strcat(buff, comma);
                        strcat(buff, "DataPromised");
                        comma = ", ";
                    }
                    
                    strcat(buff, ")");
                    
                    CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                    ShowText(inContext, buff);
                }
                currLine += mLineHeight;
                
                CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                ShowText(inContext, "Length:");
                
                CGContextSelectFont(inContext, kDataBoldFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kDataMargin, currLine);
                
                size_t  dataSize    = flavorData.mData.size();
                
                if (dataSize > 0)
                {
                    sprintf(buff, "%lu", dataSize);
                    ShowText(inContext, buff);
                }
                else
                {
                    ShowText(inContext, "-");
                }
                currLine += mLineHeight;
                
                CGContextSelectFont(inContext, kLabelFont, mFontSize, kCGEncodingMacRoman);
                CGContextSetTextPosition(inContext, kFlavorMargin, currLine);
                ShowText(inContext, "Data:");
                
                CGContextSelectFont(inContext, kDataFont, mFontSize, kCGEncodingMacRoman);
                
                if (dataSize > 0)
                {
                    const char* dataPtr = &flavorData.mData[0];
                    size_t      currOff = 0;
                    
                    while (currOff < dataSize)
                    {
                        size_t  linelen = std::min(16UL, dataSize - currOff);
                        char    linebuff[100];
                        
                        strcpy(linebuff, "");
                        
                        for (size_t k = 0; k < linelen; k += 2)
                        {
                            char    bytebuff[16];
                            
                            if (linelen == k + 1)
                            {
                                // odd line length
                                sprintf(bytebuff, "%2.2x ", 
                                        dataPtr[currOff+k] & 0x0FF);
                            }
                            else
                            {
                                sprintf(bytebuff, "%2.2x%2.2x ", 
                                        dataPtr[currOff+k] & 0x0FF, 
                                        dataPtr[currOff+k+1] & 0x0FF);
                            }
                            
                            strcat(linebuff, bytebuff);
                        }
                        
                        sprintf(buff, "%-41s'", linebuff);
                        
                        for (size_t k = 0; k < linelen; k++)
                        {
                            char    c   = dataPtr[currOff+k];
                            char    cbuff[16];
                            
                            if ((c < ' ') || (c > '~'))
                                c = '.';
                            
                            sprintf(cbuff, "%c", c);
                            strcat(buff, cbuff);
                        }
                        
                        strcat(buff, "'");
                        currOff += linelen;
                        
                        CGContextSetTextPosition(inContext, kDataMargin, currLine);
                        ShowText(inContext, buff);
                        currLine += mLineHeight;
                    }
                }
                else
                {
                    CGContextSetTextPosition(inContext, kDataMargin, currLine);
                    ShowText(inContext, "-");
                    currLine += mLineHeight;
                }
            }
        }
        
        if (mCurrentDragRef != NULL)
        {
            B::Shape    highlightShape(bounds);
            
            err = ShowDragHilite(mCurrentDragRef, highlightShape.GetRegion().get(), true);
            B_THROW_IF_STATUS(err);
        }
    }
    
    CGContextRestoreGState(inContext);
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::GetTextInfo(HIThemeTextInfo& outTextInfo, B::Rect& outTextBounds) const
{
    outTextInfo.version             = kHIThemeTextInfoVersionZero;
    outTextInfo.state               = IsActive() ? kThemeStateActive : kThemeStateInactive;
    outTextInfo.fontID              = kThemeEmphasizedSystemFont;
    outTextInfo.horizontalFlushness = kHIThemeTextHorizontalFlushCenter;
    outTextInfo.verticalFlushness   = kHIThemeTextVerticalFlushCenter;
    outTextInfo.options             = kHIThemeTextBoxOptionNone;
    outTextInfo.truncationPosition  = kHIThemeTextTruncationNone;
    outTextInfo.truncationMaxLines  = 0;
    outTextInfo.truncationHappened  = false;
    
    outTextBounds = GetBounds();
    outTextBounds.inset(20.0f);
}

// ------------------------------------------------------------------------------------------
B::Rect
DragPeekerView::GetOptimalBounds() const
{
    B::Rect bounds  = GetBounds();
    
    if (mDragItems.empty())
    {
        HIThemeTextInfo textInfo;
        B::Rect         textBounds;
        OSStatus        err;
        
        GetTextInfo(textInfo, textBounds);
        
        err = HIThemeGetTextDimensions(mEmptyStr, textBounds.size.width, &textInfo, 
                                       &mEmptyStrSize.width, &mEmptyStrSize.height, NULL);
        B_THROW_IF_STATUS(err);
        
        bounds.size = mEmptyStrSize;
    }
    else
    {
        unsigned    nlines  = 5 + 4 * mDragItems.size();
        
        for (size_t i = 0; i < mDragItems.size(); i++)
        {
            const ItemData& itemData    = mDragItems[i];
            
            nlines += 5 * itemData.mFlavors.size();
            
            for (size_t j = 0; j < itemData.mFlavors.size(); j++)
            {
                const FlavorData&   flavorData  = itemData.mFlavors[j];
                size_t              dataSize    = flavorData.mData.size();
                
                if (!flavorData.mOSType.empty())
                    ++nlines;
                
                if (!flavorData.mNSType.empty())
                    ++nlines;
                
                if (!flavorData.mFileType.empty())
                    ++nlines;
                
                if (!flavorData.mMimeType.empty())
                    ++nlines;
                
                if (dataSize > 16)
                {
                    nlines += ((dataSize - 16) + 15) / 16;
                }
            }
        }
        
        nlines++;
        
        bounds.size.width   = 650.0f;
        bounds.size.height  = nlines * mLineHeight;
    }
    
    return bounds;
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::GetMinimumSize(
    B::Size&        outMinimumSize) const
{
    outMinimumSize.width = outMinimumSize.height = 20.0;
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::GetMaximumSize(
    B::Size&        outMaximumSize) const
{
    outMaximumSize.width = outMaximumSize.height = 30000.0;
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::GetScrollInfo(
    B::Size&        outImageSize,
    B::Size&        outViewSize,
    B::Size&        outLineSize,
    B::Point&       outOrigin) const
{
    B::Rect bounds          = GetBounds();
    B::Rect optimalBounds   = GetOptimalBounds();
    
    outImageSize        = optimalBounds.size;
    outViewSize         = bounds.size;
    outLineSize.width   = mLineHeight;
    outLineSize.height  = 4 * mLineHeight;
    outOrigin           = bounds.origin;
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::ScrollTo(
    const B::Point& inOrigin)
{
    B::Rect     bounds  = GetBounds();
    B::Point    newOrigin;
    
    newOrigin.x = std::max(inOrigin.x, 0.0f);
    newOrigin.y = std::max(inOrigin.y, 0.0f);
    
    if (newOrigin != bounds.origin)
    {
        SetOrigin(newOrigin);
        Invalidate();
    }
}

// ------------------------------------------------------------------------------------------
bool
DragPeekerView::DragEnter(
    DragRef         inDragRef)
{
    B::Drag drag(inDragRef);
    
    mCurrentDragRef = inDragRef;
    
    FillItems(&drag, drag.GetPasteboard(), false);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::DragLeave(
    DragRef         /* inDragRef */)
{
    mCurrentDragRef = NULL;
    
    Invalidate();
}

// ------------------------------------------------------------------------------------------
bool
DragPeekerView::DragReceive(
    DragRef         inDragRef)
{
    B::Drag drag(inDragRef);
    
    FillItems(&drag, drag.GetPasteboard(), true);
    
    return (true);
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::PrintDebugInfo()
{
    printf("DragPeekerView\n");
    
    HybridView::PrintDebugInfo();
}

// ------------------------------------------------------------------------------------------
/*! Handle menu commands.  We just handle the New command (to display a new drag peeker 
    window).
*/
bool
DragPeekerView::HandleCommand(
    const HICommandExtended&    inHICommand)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case kHICommandPaste:
        FillWithPasteboard(B::Clipboard::Get());
        break;
        
    default:
        handled = B::HybridView::HandleCommand(inHICommand);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
/*! Update the status of menu commands.  We just enable the New command.
*/
bool
DragPeekerView::HandleUpdateStatus(
    const HICommandExtended&    inHICommand, 
    B::CommandData&             ioCmdData)
{
    bool    handled = true;
    
    switch (inHICommand.commandID)
    {
    case kHICommandPaste:
        ioCmdData.SetEnabled(true);
        break;
    
    default:
        handled = B::HybridView::HandleUpdateStatus(inHICommand, ioCmdData);
        break;
    }
    
    return (handled);
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::FillWithPasteboard(const B::Pasteboard& inPasteboard)
{
    FillItems(NULL, inPasteboard, true);
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::FillItems(const B::Drag* inDrag, const B::Pasteboard& inPasteboard, bool getData)
{
    mDragAttributes         = 0;
    mDragModifiers          = 0;
    mDragMouseDownModifiers = 0;
    mDragMouseUpModifiers   = 0;
    mDragAllowableActions   = 0;
    mDragAction             = 0;
    
    if (inDrag != NULL)
    {
        mDragAttributes         = inDrag->GetAttributes();
        mDragAllowableActions   = inDrag->GetAllowableActions();
        mDragAction             = inDrag->GetDropAction();
        mDragModifiers          = inDrag->GetModifiers();
        mDragMouseDownModifiers = inDrag->GetMouseDownModifiers();
        mDragMouseUpModifiers   = inDrag->GetMouseUpModifiers();
    }
    
    mDragItems.clear();
    std::for_each(inPasteboard.begin(), inPasteboard.end(), 
                  FillItemVector(inDrag, mDragItems, getData, GetViewRef()));
    
    SetOrigin(CGPointZero);
    Invalidate();
    ScrollInfoChanged();
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::ShowText(CGContextRef context, const char* str)
{
    CGContextShowText(context, str, strlen(str));
}

// ------------------------------------------------------------------------------------------
DragPeekerView::FillItemVector::FillItemVector(
    const B::Drag*          inDrag, 
    std::vector<ItemData>&  items, 
    bool                    getData, 
    HIViewRef               view)
        : mDrag(inDrag), mItems(items), mGetData(getData), mView(view)
{
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::FillItemVector::operator () (const B::PasteboardItem& inItem) const
{
    ItemData    itemData;
    
    if (mDrag != NULL)
    {
        itemData.mBounds = mDrag->GetItemBounds(inItem, mView);
    }
    else
    {
        itemData.mBounds = CGRectZero;
    }
    
    std::for_each(inItem.begin(), inItem.end(), 
                  FillFlavorVector(itemData.mFlavors, mGetData));
    
    mItems.push_back(itemData);
}

// ------------------------------------------------------------------------------------------
DragPeekerView::FillFlavorVector::FillFlavorVector(std::vector<FlavorData>& flavors, bool getData)
    : mFlavors(flavors), mGetData(getData)
{
}

// ------------------------------------------------------------------------------------------
void
DragPeekerView::FillFlavorVector::operator () (const B::PasteboardFlavor& inFlavor) const
{
    FlavorData  flavorData;
    
    flavorData.mFlavor  = inFlavor.GetFlavorType();
    flavorData.mFlags   = inFlavor.GetFlags();
    flavorData.mLength  = 0;
    
    CFStringRef             flavorStr   = flavorData.mFlavor.cf_ref();
    B::OSPtr<CFStringRef>   tempStr;
    
    tempStr.reset(UTTypeCopyPreferredTagWithClass(flavorStr, kUTTagClassOSType), std::nothrow);
    if (tempStr.get() != NULL)
        flavorData.mOSType = tempStr;
    
    tempStr.reset(UTTypeCopyPreferredTagWithClass(flavorStr, kUTTagClassNSPboardType), std::nothrow);
    if (tempStr.get() != NULL)
        flavorData.mNSType = tempStr;
    
    tempStr.reset(UTTypeCopyPreferredTagWithClass(flavorStr, kUTTagClassFilenameExtension), std::nothrow);
    if (tempStr.get() != NULL)
        flavorData.mFileType = tempStr;
    
    tempStr.reset(UTTypeCopyPreferredTagWithClass(flavorStr, kUTTagClassMIMEType), std::nothrow);
    if (tempStr.get() != NULL)
        flavorData.mMimeType = tempStr;
    
    if (mGetData)
    {
        B::PasteboardInputStream    istr(inFlavor);

        flavorData.mData.clear();
        
        while (istr.good())
        {
            char    buffer[10000];
            size_t  nread;
            
            nread = istr.readsome(buffer, sizeof(buffer));
            
            if (nread > 0)
                flavorData.mData.insert(flavorData.mData.end(), buffer, buffer+nread);
            else
                istr.setstate(std::ios::eofbit);
        }
        
        istr.close();
    }
    
    mFlavors.push_back(flavorData);
}
