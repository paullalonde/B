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
#include "BImageUtilities.h"

// system headers
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

// B headers
#include "BMemoryUtilities.h"
#include "BQuickdrawUtilities.h"
#include "BQuickTimeUtilities.h"
#include "BRect.h"
#include "BString.h"
#include "BUrl.h"


namespace B {

#define RECT_HEIGHT(R)  ((R).bottom-(R).top)
#define RECT_WIDTH(R)   ((R).right-(R).left)


static OSPtr<CGImageRef>    CreateCGImageWithQTFromDataRef(
    OSType      inDataRefType,
    Handle      inDataRef);

static void BufRelease(
    void*       inInfo,
    const void* inData,
    size_t      inSize);


// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>   CreateImageFromURL(const Url& inUrl)
{
    String              ext;
    OSPtr<CGImageRef>   image;
    
    if (inUrl.GetPathExtension(ext))
    {
        OSPtr<CGDataProviderRef>    provider;
        
        if ((CFStringCompare(ext.cf_ref(), CFSTR("jpg"),  kCFCompareCaseInsensitive) == 0) || 
            (CFStringCompare(ext.cf_ref(), CFSTR("jpeg"), kCFCompareCaseInsensitive) == 0) || 
            (CFStringCompare(ext.cf_ref(), CFSTR("jpe"),  kCFCompareCaseInsensitive) == 0))
        {
            image = CreateImageFromJpegURL(inUrl);
        }
        else if ((CFStringCompare(ext.cf_ref(), CFSTR("png"), kCFCompareCaseInsensitive) == 0))
        {
            image = CreateImageFromPngURL(inUrl);
        }
        else if ((CFStringCompare(ext.cf_ref(), CFSTR("icns"), kCFCompareCaseInsensitive) == 0))
        {
            image = CreateImageFromIcnsURL(inUrl);
        }
    }
    
    if (image == NULL)
    {
        AutoMacHandle   dataRef;
        OSType          dataRefType;
        OSStatus        err;
        
        if (inUrl.IsFileUrl())
        {
            AliasHandle aliasH;
            FSRef       ref;
            
            inUrl.Copy(ref);
            
            err = FSNewAliasMinimal(&ref, &aliasH);
            B_THROW_IF_STATUS(err);
            
            dataRef.reset(reinterpret_cast<Handle>(aliasH));
            dataRefType = AliasDataHandlerSubType;
        }
        else
        {
            std::string urlStr;
            
            inUrl.GetString().copy(urlStr, kCFStringEncodingUTF8);
            
            SetHandleSize(dataRef.get_handle(), urlStr.size() + 1);
            B_THROW_IF_STATUS(MemError());
            
            AutoHandleLock  lock(dataRef);
            
            strcpy(*dataRef.get_handle(), urlStr.c_str());
            dataRefType = URLDataHandlerSubType;
        }
        
        image = CreateCGImageWithQTFromDataRef(dataRefType, dataRef.get_handle());
    }
    
    return (image);
}

// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>   CreateImageFromJpegURL(const Url& inUrl)
{
    OSPtr<CGDataProviderRef>    provider(CGDataProviderCreateWithURL(inUrl.cf_ref()), from_copy);
    OSPtr<CGImageRef>           image;
    
    image.reset(CGImageCreateWithJPEGDataProvider(provider, NULL, false, 
                                                  kCGRenderingIntentDefault), 
                from_copy);
    
    return (image);
}

// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>   CreateImageFromPngURL(const Url& inUrl)
{
    OSPtr<CGDataProviderRef>    provider(CGDataProviderCreateWithURL(inUrl.cf_ref()), from_copy);
    OSPtr<CGImageRef>           image;
    
    image.reset(CGImageCreateWithPNGDataProvider(provider, NULL, false, 
                                                 kCGRenderingIntentDefault), 
                from_copy);
    
    return (image);
}

// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>   CreateImageFromIcnsURL(const Url& inUrl)
{
    CFDataRef   dataRef;
    SInt32      errorCode;
    
    if (!CFURLCreateDataAndPropertiesFromResource(NULL, inUrl.cf_ref(), &dataRef, NULL, 
                                                  NULL, &errorCode))
    {
        B_THROW_STATUS(errorCode);
    }
    
    OSPtr<CFDataRef>                        dataPtr(dataRef, from_copy);
    size_t                                  dataSize    = CFDataGetLength(dataRef);
    AutoTypedHandle<IconFamilyResource>     iconFamilyH;
    
    iconFamilyH.resize(dataSize);
    
    BlockMoveData(CFDataGetBytePtr(dataRef), *iconFamilyH.get(), dataSize);
    
    return (CreateImageFromIconFamily(iconFamilyH.get(), INT_MAX));
}

// ------------------------------------------------------------------------------------------
OSPtr<CGImageRef>   CreateImageFromIconFamily(IconFamilyHandle inFamily, int /* inSizeHint */)
{
    struct IconInfo {
        int     size;
        OSType  imageType;
        int     imageDepth;
        OSType  maskType;
        int     maskDepth;
    };
    
    static const IconInfo   kIconInfos[] = {
        { 128,  kThumbnail32BitData,    32, kThumbnail8BitMask, 8 },
        
        { 48,   kHuge32BitData,         32, kHuge8BitMask,      8 },
        { 48,   kHuge8BitData,          8,  kHuge1BitMask,      1 },
        { 48,   kHuge4BitData,          4,  kHuge1BitMask,      1 },

        { 32,   kLarge32BitData,        32, kLarge8BitMask,     8 },
        { 32,   kLarge8BitData,         8,  kLarge1BitMask,     1 },
        { 32,   kLarge4BitData,         4,  kLarge1BitMask,     1 },

        { 16,   kSmall32BitData,        32, kSmall8BitMask,     8 },
        { 16,   kSmall8BitData,         8,  kSmall1BitMask,     1 },
        { 16,   kSmall4BitData,         4,  kSmall1BitMask,     1 },
    };
    static const size_t kNumIconInfos   = sizeof(kIconInfos) / sizeof(kIconInfos[0]);
    
    AutoMacHandle   imageHandle, maskHandle;
    const IconInfo* iconInfo;
    OSStatus        err = noErr;
    
    for (iconInfo = kIconInfos; iconInfo < kIconInfos + kNumIconInfos; iconInfo++)
    {
        imageHandle.resize(0);
        maskHandle.resize(0);
        
        err = GetIconFamilyData(inFamily, iconInfo->imageType, imageHandle.get_handle());
        if (err != noErr)
            continue;
        
        err = GetIconFamilyData(inFamily, iconInfo->maskType, maskHandle.get_handle());
        if (err != noErr)
            continue;
        
        break;
    }
    
    B_THROW_IF_STATUS(err);
    
    AutoHandleLock  imageLock(imageHandle), maskLock(maskHandle);
    Rect            bounds(0, 0, iconInfo->size, iconInfo->size);
    long            imageRowBytes, maskRowBytes;
    CTabHandle      imageCTab, maskCTab;
    
    imageRowBytes   = (iconInfo->size * iconInfo->imageDepth) / 8;
    maskRowBytes    = (iconInfo->size * iconInfo->maskDepth) / 8;
    imageCTab       = (iconInfo->imageDepth > 8) ? NULL : GetCTable(64 + iconInfo->imageDepth);
    maskCTab        = (iconInfo->maskDepth  > 8) ? NULL : GetCTable(32 + iconInfo->maskDepth);
    
    AutoGWorld  imageWorld(iconInfo->imageDepth, bounds, *imageHandle.get_handle(), 
                           imageRowBytes, imageCTab);
    AutoGWorld  maskWorld(iconInfo->maskDepth, bounds, *maskHandle.get_handle(), 
                          maskRowBytes, maskCTab);
    CGImageRef  imageRef;
    
    err = CreateCGImageFromPixMaps(GetGWorldPixMap(imageWorld), 
                                   GetGWorldPixMap(maskWorld), 
                                   &imageRef);
    B_THROW_IF_STATUS(err);
    
    return (OSPtr<CGImageRef>(imageRef, from_copy));
}

// ------------------------------------------------------------------------------------------
static OSPtr<CGImageRef>
CreateCGImageWithQTFromDataRef(
    OSType      inDataRefType,
    Handle      inDataRef)
{
    const int   kBitsPerComponent   = 8;
    const int   kComponentsPerPixel = 4;
    const int   kBitsPerPixel       = kBitsPerComponent * kComponentsPerPixel;
    
    GraphicsImportComponent importer;
    OSStatus                err;
    
    err = GetGraphicsImporterForDataRefWithFlags(inDataRef, inDataRefType, &importer, 
                                                 kDontUseValidateToFindGraphicsImporter);
    B_THROW_IF_STATUS(err);
    
    AutoComponentInstance   autoImporter(importer);
    ::Rect                  bounds;
    long                    width, height, rowBytes;
    size_t                  buffSize;
    
    err = GraphicsImportGetNaturalBounds(importer, &bounds);
    B_THROW_IF_STATUS(err);
    
    width       = bounds.right - bounds.left;
    height      = bounds.bottom - bounds.top;
    rowBytes    = ((width * kBitsPerComponent * kComponentsPerPixel) + 7) / 8;
    buffSize    = height * rowBytes;
    
    AutoMacPtr  autoPtr(buffSize);
    AutoGWorld  autoGWorld(kBitsPerPixel, Rect(bounds), autoPtr.get_ptr(), rowBytes);
    
    err = GraphicsImportSetGWorld(importer, autoGWorld, GetGWorldDevice(autoGWorld));
    B_THROW_IF_STATUS(err);
    
    err = GraphicsImportDraw(importer);
    B_THROW_IF_STATUS(err);
    
    OSPtr<CGDataProviderRef>    provider(CGDataProviderCreateWithData(NULL, autoPtr.get_ptr(), 
                                                                      buffSize,
                                                                      BufRelease), 
                                         from_copy);
    
    autoPtr.release();
    
    OSPtr<CGColorSpaceRef>  colorSpace(CGColorSpaceCreateDeviceRGB(), from_copy);
    OSPtr<CGImageRef>       image(CGImageCreate(width, height, kBitsPerComponent, 
                                                kBitsPerPixel, rowBytes, colorSpace,
                                                kCGImageAlphaPremultipliedFirst, 
                                                provider, NULL, false, 
                                                kCGRenderingIntentDefault), 
                                  from_copy);
    
    return (image);
    
//  // Allocate the buffer
//  width = RECT_WIDTH( bounds );
//  height = RECT_HEIGHT( bounds );
//  rowbytes = width * 4;
//  dataPtr = NewPtr( height * rowbytes );
//  require_action( dataPtr != NULL, CantAllocBuffer, err = memFullErr );
    
//  err = NewGWorldFromPtr( &gWorld, 32, &bounds, NULL, NULL, NULL,  
//dataPtr, rowbytes );
//  require_noerr( err, CantCreateGWorld );
    
//  err = GraphicsImportSetGWorld( importer, gWorld, GetGWorldDevice(  
//gWorld) );
//  require_noerr( err, CantSetGWorld );

//  RGBColor    blackRGB    = { 0, 0, 0 };
    
//  RGBBackColor(&blackRGB);
//  EraseRect(&bounds);
    
//  err = GraphicsImportSetGraphicsMode( importer,      // graphics importer instance
//                                       graphicsModeStraightAlpha, // graphics transfer mode to use for drawing
//                                       NULL );
//  require_noerr( err, CantSetGraphicsMode );
                                        
//  err = GraphicsImportDraw( importer );
//  require_noerr( err, CantDraw );
    
//  provider = CGDataProviderCreateWithData( NULL, dataPtr, height *  
//rowbytes,
//          GWorldImageBufferRelease );
//  require_action( provider != NULL, CantCreateProvider, err = memFullErr  
//);

//  colorspace = CGColorSpaceCreateDeviceRGB();
//  require_action( colorspace != NULL, CantCreateColorSpace, err =  
//memFullErr );
    
//  *outImage = CGImageCreate( width, height, 8, 32, rowbytes, colorspace,
//          kCGImageAlphaPremultipliedFirst, provider, NULL, false,  
//kCGRenderingIntentDefault );
//  require_action( *outImage != NULL, CantCreateImage, err = memFullErr );
    
//CantCreateImage:
//  CGColorSpaceRelease( colorspace );
//
//CantCreateColorSpace:
//  CGDataProviderRelease( provider );
//
//CantCreateProvider:
//CantDraw:
//CantSetGraphicsMode:
//CantSetGWorld:
//  if ( gWorld != NULL )
//      DisposeGWorld( gWorld );
//
//CantCreateGWorld:
//CantAllocBuffer:
//CantGetBounds:
//CantGetImporter:
//CantMakeFSSpec:
//  return err;
}

// ------------------------------------------------------------------------------------------
static void BufRelease(void*, const void* inData, size_t)
{
    DisposePtr(reinterpret_cast<Ptr>(const_cast<void*>(inData)));
}

}   // namespace B
