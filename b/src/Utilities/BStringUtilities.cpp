// ==========================================================================================
//  
//  Copyright (C) 2005-2006 Paul Lalonde enrg.
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
#include "BStringUtilities.h"

// standard headers
#include <locale>

// library headers
#include <openssl/bio.h>
#include <openssl/evp.h>

// B headers
#include "BErrorHandler.h"
#include "BOSPtr.h"


namespace
{
    const size_t    MAKE_WSTRING_BUFF_SIZE  = 10000;
}

namespace B {

const from_copy_t   from_copy   = {};

std::wstring
make_wstring(
    CFStringRef     cfstr)
{
    B_ASSERT(cfstr != NULL);
    
    std::wstring    wstr;
    CFIndex         size    = CFStringGetLength(cfstr);
    
    if (size > 0)
    {
        CFIndex done    = 0;
        
        while (done < size)
        {
            wchar_t buffer[MAKE_WSTRING_BUFF_SIZE / sizeof(wchar_t)];
            CFIndex used;
            
            done += CFStringGetBytes(cfstr, 
                            CFRangeMake(done, size - done), 
                            kCFStringEncodingUTF32, 0, false, 
                            reinterpret_cast<UInt8*>(buffer), sizeof(buffer), &used);
            
            B_ASSERT(used % sizeof(wchar_t) == 0);
        
            wstr.append(buffer, used / sizeof(wchar_t));
        }
    }
    
    return (wstr);
}

OSStatus
make_wstring(
    CFStringRef     cfstr, 
    std::wstring&   wstr, 
    const std::nothrow_t&)
{
    B_ASSERT(cfstr != NULL);
    
    OSStatus    err = noErr;
    
    try
    {
        wstr.clear();
        
        CFIndex size    = CFStringGetLength(cfstr);
        
        if (size > 0)
        {
            CFIndex done    = 0;
            
            while (done < size)
            {
                wchar_t buffer[MAKE_WSTRING_BUFF_SIZE / sizeof(wchar_t)];
                CFIndex used;
                
                done += CFStringGetBytes(cfstr, 
                                CFRangeMake(done, size - done), 
                                kCFStringEncodingUTF32, 0, false, 
                                reinterpret_cast<UInt8*>(buffer), sizeof(buffer), &used);
                
                B_ASSERT(used % sizeof(wchar_t) == 0);
        
                wstr.append(buffer, used / sizeof(wchar_t));
            }
        }
    }
    catch (...)
    {
        err = memFullErr;
    }
    
    return (err);
}

std::wstring
make_wstring(
    const std::string&  str,
    CFStringEncoding    encoding)
{
    return (make_wstring(make_cfstring(str, encoding)));
}

std::string
make_string(
    const std::wstring& wstr,
    CFStringEncoding    encoding)
{
    return (make_string(make_cfstring(wstr), encoding));
}

std::string
make_string(
    OSType              type)
{
    return make_string(make_cfstring(type), kCFStringEncodingMacRoman);
}

std::string
make_string(
    CFStringRef         cfstr,
    CFStringEncoding    encoding)
{
    B_ASSERT(cfstr != NULL);
    
    std::string str;
    CFIndex     size    = CFStringGetLength(cfstr);
    
    if (size > 0)
    {
        CFIndex done    = 0;
        
        while (done < size)
        {
            char    buffer[MAKE_WSTRING_BUFF_SIZE ];
            CFIndex used;
            
            done += CFStringGetBytes(cfstr, 
                            CFRangeMake(done, size - done), 
                            encoding, 0, false, 
                            reinterpret_cast<UInt8*>(buffer), sizeof(buffer), &used);
        
            str.append(buffer, used);
        }
    }
    
    return (str);
}

std::string
make_utf8_string(
    const std::wstring& wstr)
{
    return (make_string(make_cfstring(wstr), kCFStringEncodingUTF8));
}

std::string
make_utf8_string(
    CFStringRef         cfstr)
{
    return (make_string(cfstr, kCFStringEncodingUTF8));
}

OSPtr<CFStringRef>
make_cfstring(
    const std::wstring& wstr)
{
    if (!wstr.empty())
    {
        return (OSPtr<CFStringRef>(CFStringCreateWithBytes(
                                        NULL, reinterpret_cast<const UInt8*>(wstr.c_str()), 
                                        wstr.size() * sizeof(wchar_t), kCFStringEncodingUTF32, false), 
                                    from_copy));
    }
    else
    {
        return (OSPtr<CFStringRef>(CFSTR("")));
    }
}

OSStatus
make_cfstring(
    const std::wstring& wstr, 
    OSPtr<CFStringRef>& cfstr, 
    const std::nothrow_t&)
{
    OSStatus    err = noErr;
    
    try
    {
        if (!wstr.empty())
        {
            cfstr.reset(CFStringCreateWithBytes(
                            NULL, reinterpret_cast<const UInt8*>(wstr.c_str()), 
                            wstr.size() * sizeof(wchar_t), kCFStringEncodingUTF32, false), 
                        from_copy);
        }
        else
        {
            cfstr.reset(CFSTR(""));
        }
    }
    catch (...)
    {
        err = memFullErr;
    }
    
    return (err);
}

OSPtr<CFStringRef>
make_cfstring(
    const std::string&  str,
    CFStringEncoding    encoding)
{
    if (!str.empty())
    {
        return (OSPtr<CFStringRef>(CFStringCreateWithBytes(
                                        NULL, reinterpret_cast<const UInt8*>(str.c_str()), 
                                        str.size() * sizeof(char), encoding, false), 
                                    from_copy));
    }
    else
    {
        return (OSPtr<CFStringRef>(CFSTR("")));
    }
}

OSPtr<CFStringRef>
make_cfstring(
    OSType  type)
{
    return (OSPtr<CFStringRef>(UTCreateStringForOSType(type), from_copy));
}

OSPtr<CFMutableStringRef>
make_mutablecfstring(
    const std::wstring& wstr)
{
    if (!wstr.empty())
    {
        OSPtr<CFStringRef>  cfstr(make_cfstring(wstr));
        
        return (OSPtr<CFMutableStringRef>(CFStringCreateMutableCopy(NULL, 0, cfstr), from_copy));
    }
    else
    {
        return (OSPtr<CFMutableStringRef>(CFStringCreateMutable(NULL, 0), from_copy));
    }
}

OSType
make_ostype(
    const std::wstring&     wstr)
{
    return (make_ostype(make_cfstring(wstr)));
}

OSType
make_ostype(
    CFStringRef         cfstr)
{
    B_ASSERT(cfstr != NULL);
    
    return UTGetOSTypeFromString(cfstr);
}

void
make_pstring(
    const std::wstring& wstr, 
    StringPtr           pstr,
    size_t              psize,
    CFStringEncoding    encoding)
{
    make_pstring(make_cfstring(wstr), pstr, psize, encoding);
}

void
make_pstring(
    CFStringRef         cfstr, 
    StringPtr           pstr,
    size_t              psize,
    CFStringEncoding    encoding)
{
    B_ASSERT(cfstr != NULL);
    
    if (!CFStringGetPascalString(cfstr, pstr, psize, encoding))
        B_THROW_STATUS(paramErr);
}

void
make_unichars(
    const std::wstring&     wstr, 
    std::vector<UniChar>&   vec)
{
    make_unichars(make_cfstring(wstr), vec);
}

void
make_unichars(
    CFStringRef             cfstr, 
    std::vector<UniChar>&   vec)
{
    B_ASSERT(cfstr != NULL);
    
    CFIndex         len         = CFStringGetLength(cfstr);
    
    if (len > 0)
    {
        vec.resize(len);
        CFStringGetCharacters(cfstr, CFRangeMake(0, len), &vec[0]);
    }
    else
    {
        vec.clear();
    }
}

void
binary_to_base64(
    const void*         data,
    size_t              size,
    std::string&        str)
{
    str.clear();
    
    if (size > 0)
    {
        BIO*    mem = NULL;
        BIO*    b64 = NULL;
        
        try
        {
            // Create a memory buffer which will contain the Base64 encoded string
            mem = BIO_new(BIO_s_mem());
            B_THROW_IF_NULL(mem);

            // Push on a Base64 filter so that writing to the buffer encodes the data
            b64 = BIO_new(BIO_f_base64());
            B_THROW_IF_NULL(b64);

            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
            mem = BIO_push(b64, mem);
            b64 = NULL;

            // Encode all the data
            BIO_write(mem, data, size);
            BIO_flush(mem);

            // Create a new string from the data in the memory buffer
            char*   b64Ptr;
            size_t  b64Len  = BIO_get_mem_data(mem, &b64Ptr);
            
            str.assign(b64Ptr, b64Len);

            // Clean up and go home
            BIO_free_all(mem);
        }
        catch (...)
        {
            if (b64 != NULL)
                BIO_free(b64);
            
            if (mem != NULL)
                BIO_free_all(mem);
            
            throw;
        }
    }
}

void
base64_to_binary(
    const char*         data, 
    size_t              size, 
    std::vector<UInt8>& vec)
{
    vec.clear();
    
    if (size > 0)
    {
        BIO*    mem = NULL;
        BIO*    b64 = NULL;
        
        try
        {
            // Create a memory buffer containing Base64 encoded string data
            mem = BIO_new_mem_buf(const_cast<char*>(data), size);
            B_THROW_IF_NULL(mem);
        
            // Push a Base64 filter so that reading from the buffer decodes it
            b64 = BIO_new(BIO_f_base64());
            B_THROW_IF_NULL(b64);
            
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
            mem = BIO_push(b64, mem);
            b64 = NULL;
            
            char    buff[4096];
            int     nread;
            
            while ((nread = BIO_read(mem, buff, sizeof(buff))) > 0)
            {
                vec.insert(vec.end(), buff, buff + nread);
            }

            // Clean up and go home
            BIO_free_all(mem);
        }
        catch (...)
        {
            if (b64 != NULL)
                BIO_free(b64);
            
            if (mem != NULL)
                BIO_free_all(mem);
            
            throw;
        }
    }
}

}   // namespace B
