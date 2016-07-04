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

#ifndef BPasteboardStream_H_
#define BPasteboardStream_H_

#pragma once

// B headers
#include "BFwd.h"
#include "BPasteboard.h"


namespace B {

// ==========================================================================================
//  BasicPasteboardInputStreambuf

/*! @brief  Stream buffer class for input pasteboard data.
    
    @todo   %Document this class!
*/
template <typename charT, class traits>
class BasicPasteboardStreambuf : public std::basic_streambuf<charT, traits>
{
private:
    
    // types
    typedef std::ios_base::seekdir  seekdir;
    typedef std::streamsize         streamsize;
    typedef BasicPasteboardStreambuf<charT, traits> StreambufType;
    
public:
    
    // types
    typedef charT                       char_type;
    typedef typename traits::int_type   int_type;
    typedef typename traits::pos_type   pos_type;
    typedef typename traits::off_type   off_type;
    typedef traits                      traits_type;
    
    BasicPasteboardStreambuf();
    
    bool            is_open() const;
    StreambufType*  open(
                        const PasteboardFlavor& inFlavor, 
                        std::ios::openmode      inOpenMode);
    StreambufType*  close();
    
    const PasteboardFlavor& flavor() const  { return (mFlavor); }
    PasteboardFlavor&       flavor()        { return (mFlavor); }
    
protected:
    
    // overrides from std::basic_streambuf
    virtual int_type    overflow(int_type c = traits::eof());
    virtual streamsize  xsgetn(char_type* s, streamsize n);
#if 0
    virtual streamsize  xsputn(const char_type* s, streamsize n);
    virtual pos_type    seekoff(off_type off, seekdir way, openmode which);
    virtual pos_type    seekpos(pos_type pos, openmode which);
#endif
    
private:
    
    // types
    typedef std::basic_streambuf<charT, traits> inherited;
    
    size_t  resize(size_t new_size);
    
    // member variables
    PasteboardFlavor        mFlavor;
    OSPtr<CFDataRef>        mReadBuffer;
    OSPtr<CFMutableDataRef> mWriteBuffer;
    std::ios::openmode      mOpenMode;
};


// ==========================================================================================
//  BasicPasteboardInputStream

/*! @brief  Input stream class for pasteboard data.
    
    @todo   %Document this class!
*/
template <typename charT, class traits>
class BasicPasteboardInputStream : public std::basic_istream<charT, traits>
{
private:
    
    // types
    typedef std::basic_istream<charT, traits>       inherited;
    typedef BasicPasteboardStreambuf<charT, traits> BufferType;
    
public:
    
    // types
    typedef charT                       char_type;
    typedef typename traits::int_type   int_type;
    typedef typename traits::pos_type   pos_type;
    typedef typename traits::off_type   off_type;
    typedef traits                      traits_type;
    
    BasicPasteboardInputStream();
    BasicPasteboardInputStream(
        const PasteboardFlavor& inFlavor);
    BasicPasteboardInputStream(
        const PasteboardItem&   inItem, 
        CFStringRef             inFlavorType);
    
    bool    is_open() const;
    void    open(
                const PasteboardFlavor& inFlavor);
    void    open(
                const PasteboardItem&   inItem, 
                CFStringRef             inFlavorType);
    void    close();
    
    BufferType* rdbuf() const   { return const_cast<BufferType*>(&mStreambuf); }
    
    const PasteboardFlavor& flavor() const  { return (mStreambuf.flavor()); }
    
private:
    
    // member variables
    BufferType  mStreambuf;
};


// ==========================================================================================
//  BasicPasteboardOutputStream

/*! @brief  Output stream class for pasteboard data.
    
    @todo   %Document this class!
*/
template <typename charT, class traits>
class BasicPasteboardOutputStream : public std::basic_ostream<charT, traits>
{
private:
    
    // types
    typedef std::basic_ostream<charT, traits>       inherited;
    typedef BasicPasteboardStreambuf<charT, traits> BufferType;
    
public:
    
    // types
    typedef charT                       char_type;
    typedef typename traits::int_type   int_type;
    typedef typename traits::pos_type   pos_type;
    typedef typename traits::off_type   off_type;
    typedef traits                      traits_type;
    
    BasicPasteboardOutputStream();
    BasicPasteboardOutputStream(
        PasteboardFlavor&   ioFlavor);
    BasicPasteboardOutputStream(
        PasteboardItem&     ioItem, 
        CFStringRef         inFlavorType);
    
    bool    is_open() const;
    void    open(
                PasteboardFlavor&   ioFlavor);
    void    open(
                PasteboardItem&     ioItem, 
                CFStringRef         inFlavorType);
    void    close();
    
    BufferType* rdbuf() const   { return const_cast<BufferType*>(&mStreambuf); }
    
    PasteboardFlavor&   flavor()    { return (mStreambuf.flavor()); }
    
private:
    
    // member variables
    BufferType  mStreambuf;
};


}   // namespace B


#endif  // BPasteboardStream_H_
