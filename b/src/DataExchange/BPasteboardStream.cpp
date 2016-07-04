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
#include "BPasteboardStream.h"

// B headers
#include "BPasteboard.h"


namespace B {

// ==========================================================================================
//  BasicPasteboardStreambuf

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardStreambuf<charT, traits>::BasicPasteboardStreambuf()
    : mOpenMode(std::ios_base::openmode(0))
{
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> bool
BasicPasteboardStreambuf<charT, traits>::is_open() const
{
    return (mOpenMode != 0);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardStreambuf<charT, traits>*
BasicPasteboardStreambuf<charT, traits>::open(
    const PasteboardFlavor& inFlavor, 
    std::ios::openmode      inOpenMode)
{
    bool    good    = false;

    if (!is_open())
    {
        switch (inOpenMode)
        {
        case std::ios::in:
            // Opening for input
            {
                mFlavor = inFlavor;
                
                if (mFlavor.GetData(mReadBuffer))
                {
                    size_t      size    = CFDataGetLength(mReadBuffer);
                    char_type*  ptr     = const_cast<char_type*>(reinterpret_cast<const char_type*>(CFDataGetBytePtr(mReadBuffer)));
                    
                    inherited::setg(ptr, ptr, ptr+size);
                    
                    mOpenMode   = inOpenMode;
                    good        = true;
                }
            }
            break;
            
        case std::ios::out:
            // Opening for output
            {
                mWriteBuffer.reset(CFDataCreateMutable(NULL, 0), from_copy, std::nothrow);
                
                if (mWriteBuffer.get() != NULL)
                {
                    size_t  size    = resize(0);
                    
                    if (size > 0)
                    {
                        mFlavor     = inFlavor;
                        mOpenMode   = inOpenMode;
                        good        = true;
                    }
                }
            }
            break;
            
        default:
            break;
        }
    }
    
    return (good ? this : NULL);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardStreambuf<charT, traits>*
BasicPasteboardStreambuf<charT, traits>::close()
{
    bool    good    = false;
    
    if (is_open())
    {
        switch (mOpenMode)
        {
        case std::ios::in:
            // Open for input -- there's nothing to do.
            {
                inherited::setg(0, 0, 0);
                mFlavor = PasteboardFlavor();
                mReadBuffer.reset();
                mOpenMode = std::ios_base::openmode(0);
                good = true;
            }
            break;
            
        case std::ios::out:
            // Open for output -- flush the data
            {
                size_t  size = inherited::pptr() - inherited::pbase();
                
                CFDataSetLength(mWriteBuffer, size);
                
                if (static_cast<size_t>(CFDataGetLength(mWriteBuffer)) == size)
                {
                    // The resize succeeded
                    if (mFlavor.SetData(mWriteBuffer, std::nothrow))
                    {
                        // The write to the pasteboard succeeded
                        inherited::setp(0, 0);
                        mFlavor = PasteboardFlavor();
                        mWriteBuffer.reset();
                        mOpenMode = std::ios_base::openmode(0);
                        good = true;
                    }
                }
            }
            break;
            
        default:
            break;
        }
    }
    
    return (good ? this : NULL);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
typename BasicPasteboardStreambuf<charT, traits>::int_type
BasicPasteboardStreambuf<charT, traits>::overflow(int_type c /* = traits::eof() */)
{
    bool    good    = false;
    
    if (mOpenMode & std::ios::out)
    {
        if (c != traits::eq_int_type(c, traits::eof()))
        {
            *inherited::pptr() = c;
            inherited::pbump(1);
            
            good = (resize(0) > 0);
        }
    }
    
    return (good ? c : traits::eof());
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> size_t
BasicPasteboardStreambuf<charT, traits>::resize(size_t new_size)
{
    size_t  old_size    = CFDataGetLength(mWriteBuffer);
    bool    good        = false;
    
    if (new_size > 0)
    {
        if (old_size > 0)
        {
            while (old_size < new_size)
                old_size = static_cast<size_t>(old_size * 1.5);
            
            new_size = old_size;
        }
    }
    else if (old_size > 0)
    {
        new_size = static_cast<size_t>(old_size * 1.5);
    }
    else
    {
        new_size = BUFSIZ;
    }
    
    CFDataSetLength(mWriteBuffer, new_size);
    
    if (static_cast<size_t>(CFDataGetLength(mWriteBuffer)) == new_size)
    {
        char_type*  ptr = reinterpret_cast<char_type*>(CFDataGetMutableBytePtr(mWriteBuffer));
        size_t      off = inherited::pptr() - inherited::pbase();
        
        inherited::setp(ptr, ptr+new_size-1);
        inherited::pbump(off);
        
        good = true;
    }
    
    return (good ? new_size : 0);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
typename BasicPasteboardStreambuf<charT, traits>::streamsize
BasicPasteboardStreambuf<charT, traits>::xsgetn(char_type* s, streamsize n)
{
    streamsize  nleft   = inherited::egptr() - inherited::gptr();
    streamsize  nread   = std::min(n, nleft);
    
    memcpy(s, inherited::gptr(), nread * sizeof(char_type));
    inherited::gbump(nread);
    
    return (nread);
}

#if 0
// ------------------------------------------------------------------------------------------
template <class charT, class traits>
typename BasicPasteboardInputStreambuf<charT, traits>::pos_type
BasicPasteboardInputStreambuf<charT, traits>::seekoff(
    off_type    off, 
    seekdir     way,
    openmode    /* which */ /* = std::ios_base::in | std::ios_base::out */)
{
    pos_type            size    = CFDataGetLength(mBuffer);
    const char_type*    ptr     = reinterpret_cast<const char_type*>(CFDataGetBytePtr(mBuffer));
    pos_type            pos;
    
    switch (way)
    {
    case std::ios_base::beg:    pos = off;                              break;
    case std::ios_base::cur:    pos = inherited::gptr() - ptr + off;    break;
    case std::ios_base::end:    pos = size + off;                       break;

    default:
        return (pos_type(off_type(-1)));
    }
    
    if ((pos >= 0) && (pos <= size))
    {
        inherited::setg(ptr, ptr+pos, ptr+size);
    }
    else
    {
        pos = pos_type(off_type(-1));
    }
    
    return (pos);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
typename BasicPasteboardInputStreambuf<charT, traits>::pos_type
BasicPasteboardInputStreambuf<charT, traits>::seekpos(
    pos_type                pos, 
    std::ios_base::openmode /* which */ /* = std::ios_base::in | std::ios_base::out */)
{
    pos_type            size    = CFDataGetLength(mBuffer);
    const char_type*    ptr     = reinterpret_cast<const char_type*>(CFDataGetBytePtr(mBuffer));
    
    if ((pos >= 0) && (pos <= size))
    {
        inherited::setg(ptr, ptr+pos, ptr+size);
    }
    else
    {
        pos = pos_type(off_type(-1));
    }

    return (pos);
}
#endif


#if 0
// ==========================================================================================
//  BasicPasteboardOutputStreambuf

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardOutputStreambuf<charT, traits>::resize(size_t new_size)
{
    size_t  old_size    = CFDataGetLength(mBuffer);
    
    assert(new_size > old_size);
    
    CFDataSetLength(mBuffer, new_size);
    
    char_type*  ptr = reinterpret_cast<char_type*>(CFDataGetMutableBytePtr(mBuffer));
    
    inherited::setp(ptr, ptr+new_size-1);
    inherited::pbump(old_size);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> typename BasicPasteboardOutputStreambuf<charT, traits>::int_type
BasicPasteboardOutputStreambuf<charT, traits>::overflow(int_type c /* = traits::eof() */)
{
    if (c != traits::eq_int_type(c, traits::eof()))
    {
        *inherited::pptr() = c;
        inherited::pbump(1);
        
        resize(2 * (inherited::epptr() - inherited::pbase()));
    }
    
    return (c);
}

#if 0
// ------------------------------------------------------------------------------------------
template <class charT, class traits> std::streamsize
BasicPasteboardOutputStreambuf<charT, traits>::xsputn(const char_type* s, std::streamsize n)
{
    while (inherited::epptr() - inherited::pptr() < n)
    {
        resize(2 * (inherited::epptr() - inherited::pbase()));
    }
    
    memcpy(inherited::pptr(), s, n * sizeof(char_type));
    inherited::pbump(n);
    
    return (n);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
typename BasicPasteboardOutputStreambuf<charT, traits>::pos_type
BasicPasteboardOutputStreambuf<charT, traits>::seekoff(
    off_type    off, 
    seekdir     way,
    openmode    /* which */ /* = std::ios_base::in | std::ios_base::out */)
{
    pos_type            size    = CFDataGetLength(mBuffer);
    const char_type*    ptr     = reinterpret_cast<const char_type*>(CFDataGetBytePtr(mBuffer));
    pos_type            pos;
    
    switch (way)
    {
    case std::ios_base::beg:
        pos = off;
        break;
        
    case std::ios_base::cur:
        pos = inherited::pptr() - inherited::pbase() + off;
        break;
        
    case std::ios_base::end:
        pos = inherited::epptr() - inherited::pbase() + off;
        break;

    default:
        return (pos_type(off_type(-1)));
    }
    
    if ((pos >= 0) && (pos <= size))
    {
        inherited::setg(ptr, ptr+pos, ptr+size);
    }
    else
    {
        pos = pos_type(off_type(-1));
    }
    
    return (pos);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
typename BasicPasteboardOutputStreambuf<charT, traits>::pos_type
BasicPasteboardOutputStreambuf<charT, traits>::seekpos(
    pos_type                sp, 
    std::ios_base::openmode which /* = std::ios_base::in | std::ios_base::out */)
{
    if (!is_open())
        return (-1);
    
    if ((which & mOpenMode) == 0)
        return (-1);
    
    off_type    newPos  = sp;
    
    if ((newPos >= 0) && (newPos <= static_cast<off_type>(mBuffer.size())))
    {
        mOffset = newPos;
    }
    else
    {
        newPos = -1;
    }
    
    return (newPos);
}
#endif
#endif


// ==========================================================================================
//  BasicPasteboardInputStream

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardInputStream<charT, traits>::BasicPasteboardInputStream()
    : inherited(&mStreambuf)
{
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardInputStream<charT, traits>::BasicPasteboardInputStream(
    const PasteboardFlavor& inFlavor)
        : inherited(&mStreambuf)
{
    open(inFlavor);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardInputStream<charT, traits>::BasicPasteboardInputStream(
    const PasteboardItem&   inItem, 
    CFStringRef             inFlavorType)
        : inherited(&mStreambuf)
{
    open(inItem, inFlavorType);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> bool
BasicPasteboardInputStream<charT, traits>::is_open() const
{
    return (mStreambuf.is_open());
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardInputStream<charT, traits>::open(
    const PasteboardFlavor& inFlavor)
{
    if (!mStreambuf.open(inFlavor, std::ios::in))
        inherited::setstate(std::ios_base::failbit);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardInputStream<charT, traits>::open(
    const PasteboardItem&   inItem, 
    CFStringRef             inFlavorType)
{
    open(inItem.NewFlavor(inFlavorType));
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardInputStream<charT, traits>::close()
{
    if (!mStreambuf.close())
        inherited::setstate(std::ios_base::failbit);
}


// ==========================================================================================
//  BasicPasteboardOutputStream

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardOutputStream<charT, traits>::BasicPasteboardOutputStream()
    : inherited(&mStreambuf)
{
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardOutputStream<charT, traits>::BasicPasteboardOutputStream(
    PasteboardFlavor&   ioFlavor)
        : inherited(&mStreambuf)
{
    open(ioFlavor);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits>
BasicPasteboardOutputStream<charT, traits>::BasicPasteboardOutputStream(
    PasteboardItem& ioItem, 
    CFStringRef     inFlavorType)
        : inherited(&mStreambuf)
{
    open(ioItem, inFlavorType);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> bool
BasicPasteboardOutputStream<charT, traits>::is_open() const
{
    return (mStreambuf.is_open());
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardOutputStream<charT, traits>::open(
    PasteboardFlavor&   ioFlavor)
{
    if (!mStreambuf.open(ioFlavor, std::ios::out))
        inherited::setstate(std::ios_base::failbit);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardOutputStream<charT, traits>::open(
    PasteboardItem& ioItem, 
    CFStringRef     inFlavorType)
{
    if (!mStreambuf.open(ioItem.NewFlavor(inFlavorType), std::ios::out))
        inherited::setstate(std::ios_base::failbit);
}

// ------------------------------------------------------------------------------------------
template <class charT, class traits> void
BasicPasteboardOutputStream<charT, traits>::close()
{
    if (!mStreambuf.close())
        inherited::setstate(std::ios_base::failbit);
}


// ==========================================================================================
//  Template Instantiations

template class  BasicPasteboardStreambuf<char>;
template class  BasicPasteboardInputStream<char>;
template class  BasicPasteboardOutputStream<char>;


}   // namespace B
