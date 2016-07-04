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

// file header
#include "BTextUtilities.h"

// B headers
#include "BErrorHandler.h"


namespace B {

template <typename T> static void   AddAttribute(
    ATSUAttributeTag                    inTag,
    T*                                  inValue,
    std::vector<ATSUAttributeTag>&      ioTags,
    std::vector<ByteCount>&             ioSizes,
    std::vector<ATSUAttributeValuePtr>& ioValues)
{
    B_ASSERT(inValue != NULL);
    
    ioTags.push_back(inTag);
    ioSizes.push_back(sizeof(*inValue));
    ioValues.push_back(inValue);
}

static void AddQDStyleAttribute(
    ATSUAttributeTag                    inTag,
    Style                               inQDStyleMask,
    Style                               inQDStyle,
    std::vector<ATSUAttributeTag>&      ioTags,
    std::vector<ByteCount>&             ioSizes,
    std::vector<ATSUAttributeValuePtr>& ioValues)
{
    static Boolean  sTrueBool   = true;
    
    if (inQDStyle & inQDStyleMask)
    {
        AddAttribute(inTag, &sTrueBool, ioTags, ioSizes, ioValues);
    }
}

// ------------------------------------------------------------------------------------------
AtsuStylePtr
MakeAtsuStylePtr()
{
    ATSUStyle   style;
    OSStatus    err;
    
    err = ATSUCreateStyle(&style);
    B_THROW_IF_STATUS(err);
    
    return AtsuStylePtr(style);
}

// ------------------------------------------------------------------------------------------
AtsuStylePtr
MakeAtsuStylePtr(ATSUStyle style)
{
    B_ASSERT(style != NULL);
    
    return AtsuStylePtr(style);
}

// ------------------------------------------------------------------------------------------
AtsuStylePtr
MakeAtsuStylePtr(ThemeFontID inThemeFontID, ScriptCode inScriptCode)
{
    Str255      qdFontName;
    short       qdFontSize;
    Style       qdFontStyle;
    OSStatus    err;
    
    err = GetThemeFont(inThemeFontID, inScriptCode, qdFontName, &qdFontSize, &qdFontStyle);
    B_THROW_IF_STATUS(err);
    
    ATSUFontID  atsuFontID;
    
    err = ATSUFindFontFromName(qdFontName+1, qdFontName[0], kFontFullName, 
                               kFontMacintoshPlatform, kFontRomanScript, 
                               static_cast<FontLanguageCode>(kFontNoLanguage), 
                               &atsuFontID);
    B_THROW_IF_STATUS(err);
    
    AtsuStylePtr                        style(MakeAtsuStylePtr());
    Fixed                               atsuFontSize    = FixRatio(qdFontSize, 1);
    short                               atsuOrientation = kATSUStronglyHorizontal;
    std::vector<ATSUAttributeTag>       tags;
    std::vector<ByteCount>              sizes;
    std::vector<ATSUAttributeValuePtr>  values;
    
    // font ID, font size, and orientation
    
    AddAttribute(kATSUFontTag,              &atsuFontID,        tags, sizes, values);
    AddAttribute(kATSUSizeTag,              &atsuFontSize,      tags, sizes, values);
    AddAttribute(kATSUVerticalCharacterTag, &atsuOrientation,   tags, sizes, values);
    
    // Quickdraw styles
    
    AddQDStyleAttribute(kATSUQDBoldfaceTag,  bold,      qdFontStyle, tags, sizes, values);
    AddQDStyleAttribute(kATSUQDItalicTag,    italic,    qdFontStyle, tags, sizes, values);
    AddQDStyleAttribute(kATSUQDUnderlineTag, underline, qdFontStyle, tags, sizes, values);
    AddQDStyleAttribute(kATSUQDCondensedTag, condense,  qdFontStyle, tags, sizes, values);
    AddQDStyleAttribute(kATSUQDExtendedTag,  extend,    qdFontStyle, tags, sizes, values);
    
    err = ATSUSetAttributes(style.get(), tags.size(), &tags[0], &sizes[0], &values[0]);
    B_THROW_IF_STATUS(err);
    
    return style;
    
#if 0
/* MakeThemeATSUIStyle creates a simple ATSUI style record
    that based on the current theme font ID that can be used in
    calls to the ATSUI text drawing routines. */
OSStatus MakeThemeATSUIStyle(ThemeFontID themeFontID,
        ATSUStyle *theStyle) {
    OSStatus err;
    ATSUStyle localStyle;
    ATSUFontID atsuFont;
    Fixed atsuSize;
    short atsuOrientation, fontFamily, fontSize;
    Style fontStyle;
    Boolean trueVar = true, falseVar = false;

        /* Three parrallel arrays for setting up attributes. */
    ATSUAttributeTag theTags[] = {
            kATSUFontTag, kATSUSizeTag, kATSUVerticalCharacterTag,
            kATSUQDBoldfaceTag, kATSUQDItalicTag, kATSUQDUnderlineTag,
            kATSUQDCondensedTag, kATSUQDExtendedTag
        };
    ByteCount theSizes[] = {
            sizeof(ATSUFontID), sizeof(Fixed), sizeof(UInt16),
            sizeof(Boolean), sizeof(Boolean), sizeof(Boolean),
            sizeof(Boolean), sizeof(Boolean)
        };
    ATSUAttributeValuePtr theValues[] = {
            NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL
        };

        /* set up locals */
    localStyle = NULL;
    atsuFont = 0;
    atsuSize = 0x00080000;
    atsuOrientation = kATSUStronglyHorizontal;
    /* or atsuOrientation = kATSUStronglyVertical */

        /* calculate the theme font parameters */
    err = GetThemeFont( themeFontID, smSystemScript,
            fontName,  &fontSize, &fontStyle);
    if (err != noErr) goto bail;
    atsuSize = FixRatio(fontSize, 1);

        /* set the values array to point to our locals */
    theValues[0] = &atsuFont;
    theValues[1] = &atsuSize;
    theValues[2] = &atsuOrientation;
    theValues[3] = (((fontStyle & bold)      != 0) ? &trueV : &falseV);
    theValues[4] = (((fontStyle & italic)    != 0) ? &trueV : &falseV);
    theValues[5] = (((fontStyle & underline) != 0) ? &trueV : &falseV);
    theValues[6] = (((fontStyle & condense)  != 0) ? &trueV : &falseV);
    theValues[7] = (((fontStyle & extend)    != 0) ? &trueV : &falseV);

        /* calculate the font ID */
    GetFNum( fontName, &fontFamily);
    err = ATSUFONDtoFontID( fontFamily, fontStyle, &atsuFont);
    if (err != noErr) goto bail;

        /* find the font ID */
    err = ATSUFindFontFromName((Ptr)fontName+1, (long)fontName[0],
        kFontFullName, kFontMacintoshPlatform,
        kFontRomanScript, kFontNoLanguage, &atsuFont);
    if (err != noErr) goto bail;

        /* create a style */
    err = ATSUCreateStyle(&localStyle);
    if (err != noErr) goto bail;

        /* set the style attributes */
    err = ATSUSetAttributes( localStyle,
        sizeof(theTags)/sizeof(theTags[0]),
        theTags, theSizes, theValues );
    if (err != noErr) goto bail;

        /* store the new style for the caller */
    *theStyle = localStyle;
    return noErr;
bail:
    if (localStyle != NULL) ATSUDisposeStyle(localStyle);
    return err;
}
#endif
}

// ------------------------------------------------------------------------------------------
AtsuStylePtr
CopyAtsuStylePtr(const AtsuStylePtr& src)
{
    ATSUStyle   style;
    OSStatus    err;
    
    err = ATSUCreateAndCopyStyle(src.get(), &style);
    B_THROW_IF_STATUS(err);
    
    return AtsuStylePtr(style);
}

// ------------------------------------------------------------------------------------------
AtsuLayoutPtr
MakeAtsuLayoutPtr()
{
    ATSUTextLayout  layout;
    OSStatus        err;
    
    err = ATSUCreateTextLayout(&layout);
    B_THROW_IF_STATUS(err);
    
    return AtsuLayoutPtr(layout);
}

// ------------------------------------------------------------------------------------------
AtsuLayoutPtr
MakeAtsuLayoutPtr(ATSUTextLayout layout)
{
    B_ASSERT(layout != NULL);
    
    return AtsuLayoutPtr(layout);
}

// ------------------------------------------------------------------------------------------
AtsuLayoutPtr
CopyAtsuLayoutPtr(const AtsuLayoutPtr& src)
{
    ATSUTextLayout  layout;
    OSStatus        err;
    
    err = ATSUCreateAndCopyTextLayout(src.get(), &layout);
    B_THROW_IF_STATUS(err);
    
    return AtsuLayoutPtr(layout);
}

}   // namespace B

// ------------------------------------------------------------------------------------------
void
intrusive_ptr_add_ref(ATSUStyle style)
{
    B_ASSERT(style != NULL);
    
    UInt32      refCount;
    OSStatus    err;
    
    err = ATSUGetStyleRefCon(style, &refCount);
    B_THROW_IF_STATUS(err);
    
    err = ATSUSetStyleRefCon(style, ++refCount);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
intrusive_ptr_release(ATSUStyle style)
{
    B_ASSERT(style != NULL);
    
    UInt32      refCount;
    OSStatus    err;
    
    err = ATSUGetStyleRefCon(style, &refCount);
    B_THROW_IF_STATUS(err);
    
    B_ASSERT(refCount > 0);
    
    if (--refCount == 0)
    {
        ATSUDisposeStyle(style);
    }
    else
    {
        err = ATSUSetStyleRefCon(style, refCount);
        B_THROW_IF_STATUS(err);
    }
}

// ------------------------------------------------------------------------------------------
void
intrusive_ptr_add_ref(ATSUTextLayout layout)
{
    B_ASSERT(layout != NULL);
    
    UInt32      refCount;
    OSStatus    err;
    
    err = ATSUGetTextLayoutRefCon(layout, &refCount);
    B_THROW_IF_STATUS(err);
    
    err = ATSUSetTextLayoutRefCon(layout, ++refCount);
    B_THROW_IF_STATUS(err);
}

// ------------------------------------------------------------------------------------------
void
intrusive_ptr_release(ATSUTextLayout layout)
{
    B_ASSERT(layout != NULL);
    
    UInt32      refCount;
    OSStatus    err;
    
    err = ATSUGetTextLayoutRefCon(layout, &refCount);
    B_THROW_IF_STATUS(err);
    
    B_ASSERT(refCount > 0);
    
    if (--refCount == 0)
    {
        ATSUDisposeTextLayout(layout);
    }
    else
    {
        err = ATSUSetTextLayoutRefCon(layout, refCount);
        B_THROW_IF_STATUS(err);
    }
}
