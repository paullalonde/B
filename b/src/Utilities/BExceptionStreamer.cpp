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
#include "BExceptionStreamer.h"

// standard headers
#include <stdexcept>

// library headers
#include <boost/any.hpp>
#include <boost/format.hpp>
//#include <boost/numeric/ublas/exception.hpp>
//#include <boost/regex.hpp>
//#include <boost/spirit.hpp>
//#include <boost/test/test_tools.hpp>
#include <boost/thread.hpp>

// B headers
#include "BErrorHandler.h"
#include "BString.h"
#include "CFUtils.h"


namespace B {

// ======================================================================================

class BoostIoBadFormatStringRethrower : 
    public ExceptionStreamer::CustomRethrower<boost::io::bad_format_string>
{
    virtual void    Write(
        const boost::io::bad_format_string& ex, 
        std::ostream&                       ostr) const
    {
        ostr << ex.get_pos() << " " << ex.get_next();
    }
    
    virtual void    Rethrow(
        std::istream&   istr) const
    {
        std::size_t pos, next;
        
        istr >> pos >> next;
        
        B_THROW(boost::io::bad_format_string(pos, next));
    }
};

// ======================================================================================

class BoostIoTooFewArgsRethrower : 
    public ExceptionStreamer::CustomRethrower<boost::io::too_few_args>
{
    virtual void    Write(
        const boost::io::too_few_args&  ex, 
        std::ostream&                   ostr) const
    {
        ostr << ex.get_cur() << " " << ex.get_expected();
    }
    
    virtual void    Rethrow(
        std::istream&   istr) const
    {
        std::size_t cur, expected;
        
        istr >> cur >> expected;
        
        B_THROW(boost::io::too_few_args(cur, expected));
    }
};

// ======================================================================================

class BoostIoTooManyArgsRethrower : 
    public ExceptionStreamer::CustomRethrower<boost::io::too_many_args>
{
    virtual void    Write(
        const boost::io::too_many_args& ex, 
        std::ostream&                   ostr) const
    {
        ostr << ex.get_cur() << " " << ex.get_expected();
    }
    
    virtual void    Rethrow(
        std::istream&   istr) const
    {
        std::size_t cur, expected;
        
        istr >> cur >> expected;
        
        B_THROW(boost::io::too_many_args(cur, expected));
    }
};

// ======================================================================================

class BoostIoOutOfRangeRethrower: 
    public ExceptionStreamer::CustomRethrower<boost::io::out_of_range>
{
    virtual void    Write(
        const boost::io::out_of_range&  ex, 
        std::ostream&                   ostr) const
    {
        ostr << ex.get_index() << " " << ex.get_beg() << " " << ex.get_end();
    }
    
    virtual void    Rethrow(
        std::istream&   istr) const
    {
        int index, beg, end;
        
        istr >> index >> beg >> end;
        
        B_THROW(boost::io::out_of_range(index, beg, end));
    }
};

template <> struct ExceptionTraits<std::ios_base::failure>  { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::logic_error>        { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::domain_error>       { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::invalid_argument>   { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::length_error>       { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::out_of_range>       { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::runtime_error>      { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::overflow_error>     { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::range_error>        { typedef StringExceptionTag    Category; };
template <> struct ExceptionTraits<std::underflow_error>    { typedef StringExceptionTag    Category; };

//template <> struct ExceptionTraits<boost::bad_expression>                 { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::bad_pattern>                    { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::divide_by_zero> { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::internal_logic> { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::external_logic> { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::bad_argument>       { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::bad_size>           { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::bad_index>      { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::singular>           { typedef StringExceptionTag    Category; };
//template <> struct ExceptionTraits<boost::numeric::ublas::non_real>           { typedef StringExceptionTag    Category; };
//template <> class ExceptionTraits<boost::spirit::parser_error<ErrorDescrT, IteratorT> >   { typedef StreamExceptionTag    Category; };

boost::once_flag                            ExceptionStreamer::sInitOnce    = BOOST_ONCE_INIT;
bool                                        ExceptionStreamer::sInitFailure = false;
boost::intrusive_ptr<ExceptionStreamer>*    ExceptionStreamer::sInstancePtr = NULL;

// ------------------------------------------------------------------------------------------
void
ExceptionStreamer::InitSingleton()
{
    B_ASSERT(sInstancePtr == NULL);
    
    ExceptionStreamer*  streamer    = NULL;
    
    sInitFailure = false;
    
    try
    {
        streamer        = new ExceptionStreamer;
        sInstancePtr    = new boost::intrusive_ptr<ExceptionStreamer>(streamer);
        streamer        = NULL;
        
        atexit(CleanupSingleton);
    }
    catch (...)
    {
        // Prevent exceptions from propagating.
        
        delete sInstancePtr;
        sInstancePtr = NULL;
        
        delete streamer;
        
        sInitFailure = true;
    }
}

// ------------------------------------------------------------------------------------------
void
ExceptionStreamer::CleanupSingleton()
{
    delete sInstancePtr;
    sInstancePtr = NULL;
}

// ------------------------------------------------------------------------------------------
boost::intrusive_ptr<ExceptionStreamer>
ExceptionStreamer::Get()
{
    // call_once ensures that InitSingleton is only called once, even if multiple 
    // threads enter here concurrently.  Furthermore, any threads entering call_once 
    // will be blocked until the first one exits.
    
    boost::call_once(InitSingleton, sInitOnce);
    B_THROW_IF(sInitFailure, std::runtime_error("can't init ExceptionStreamer"));
    
    // If this assertion fails, it's probably because we are being called during 
    // application shutdown, and CleanupSingleton() has already been called.
    B_ASSERT(sInstancePtr != NULL);
    
    return (*sInstancePtr);
}

// ------------------------------------------------------------------------------------------
ExceptionStreamer::ExceptionStreamer()
    : mRefCount(0)
{
    // Fill in the encoding map, which is used to encode reserved characters.
    
    mEncodingMap.insert(EncodingMap::value_type('&',  "amp"));
    mEncodingMap.insert(EncodingMap::value_type('\'', "apos"));
    mEncodingMap.insert(EncodingMap::value_type('>',  "gt"));
    mEncodingMap.insert(EncodingMap::value_type('<',  "lt"));
    mEncodingMap.insert(EncodingMap::value_type('"',  "quot"));
    
    for (EncodingMap::const_iterator it = mEncodingMap.begin(); 
         it != mEncodingMap.end(); 
         ++it)
    {
        mEncodedChars += it->first;
    }
    
    // Fill in the decoding map, which is the inverse of the encoding map.
    
    for (EncodingMap::const_iterator it = mEncodingMap.begin(); 
         it != mEncodingMap.end(); 
         ++it)
    {
        mDecodingMap.insert(DecodingMap::value_type(it->second, it->first));
    }
    
    // Standard C++ exceptions.
    
    PrivateRegisterType<std::ios_base::failure>();
    PrivateRegisterType<std::logic_error>();
    PrivateRegisterType<std::domain_error>();
    PrivateRegisterType<std::invalid_argument>();
    PrivateRegisterType<std::length_error>();
    PrivateRegisterType<std::out_of_range>();
    PrivateRegisterType<std::runtime_error>();
    PrivateRegisterType<std::overflow_error>();
    PrivateRegisterType<std::range_error>();
    PrivateRegisterType<std::underflow_error>();
    PrivateRegisterType<std::exception>();
    PrivateRegisterType<std::bad_alloc>();
    PrivateRegisterType<std::bad_cast>();
    PrivateRegisterType<std::bad_exception>();
    PrivateRegisterType<std::bad_typeid>();
    
    // Boost exceptions.
    
    PrivateRegisterType<boost::bad_any_cast>();
    PrivateRegisterType<boost::bad_function_call>();
    PrivateRegisterType<boost::bad_weak_ptr>();
    // Thread
    PrivateRegisterType<boost::invalid_thread_argument>();
    PrivateRegisterType<boost::lock_error>();
    PrivateRegisterType<boost::thread_permission_error>();
    PrivateRegisterType<boost::thread_resource_error>();
    PrivateRegisterType<boost::unsupported_thread_option>();
    // Format
    PrivateRegisterType<boost::io::format_error>();
    PrivateRegisterRethrower<BoostIoBadFormatStringRethrower>();
    PrivateRegisterRethrower<BoostIoTooFewArgsRethrower>();
    PrivateRegisterRethrower<BoostIoTooManyArgsRethrower>();
    PrivateRegisterRethrower<BoostIoOutOfRangeRethrower>();
    // Test
//  PrivateRegisterType<boost::test_toolbox::tt_detail::test_tool_failed>();
    // Regex
//  PrivateRegisterType<boost::bad_expression>();
//  PrivateRegisterType<boost::bad_pattern>();
    // uBLAS
//  PrivateRegisterType<boost::numeric::ublas::divide_by_zero>();
//  PrivateRegisterType<boost::numeric::ublas::internal_logic>();
//  PrivateRegisterType<boost::numeric::ublas::external_logic>();
//  PrivateRegisterType<boost::numeric::ublas::bad_argument>();
//  PrivateRegisterType<boost::numeric::ublas::bad_size>();
//  PrivateRegisterType<boost::numeric::ublas::bad_index>();
//  PrivateRegisterType<boost::numeric::ublas::singular>();
//  PrivateRegisterType<boost::numeric::ublas::non_real>();
    // Spirit
//  PrivateRegisterType<boost::spirit::parser_error_base>();
//  PrivateRegisterType<boost::spirit::parser_error<ErrorDescrT, IteratorT> >();    // templated exception -- yikes!
//  PrivateRegisterType<boost::spirit::illegal_backtracking>();
}

// ------------------------------------------------------------------------------------------
ExceptionStreamer::~ExceptionStreamer()
{
    while (!mRethrowerMap.empty())
    {
        RethrowerMap::iterator          it(mRethrowerMap.begin());
        std::auto_ptr<RethrowerBase>    ptr(it->second);
        
        mRethrowerMap.erase(it);
    }
}

// ------------------------------------------------------------------------------------------
bool
ExceptionStreamer::PrivateRegister(
    const char*                     exceptionClass, 
    std::auto_ptr<RethrowerBase>    rethrower)
{
    B_ASSERT(exceptionClass != NULL);
    
    RethrowerMap::value_type    value(exceptionClass, rethrower.get());
    bool                        good;
    
    good = mRethrowerMap.insert(value).second;
    
    rethrower.release();
    
    return (good);
}

// ------------------------------------------------------------------------------------------
bool
ExceptionStreamer::PrivateUnregister(
    const char* exceptionClass)
{
    RethrowerMap::iterator  it      = mRethrowerMap.find(exceptionClass);
    bool                    good    = (it != mRethrowerMap.end());
    
    if (good)
    {
        std::auto_ptr<RethrowerBase>    ptr(it->second);
        
        mRethrowerMap.erase(it);
    }
    
    return (good);
}

// ------------------------------------------------------------------------------------------
void
ExceptionStreamer::Externalize(
    const std::exception&   ex, 
    std::ostream&           ostr) const
{
    const std::type_info&           exType(typeid(ex));
    RethrowerMap::const_iterator    it  = mRethrowerMap.find(exType.name());
    
    // Give up if we don't know anything about this exception.
    
    B_ASSERT(it != mRethrowerMap.end());
    
    if (it == mRethrowerMap.end())
        return;
    
    // Special std::bad_alloc.  Since this function allocates quite a bit of memory, 
    // and bad_alloc doesn't contain any state of interest, we'll let normal OS result 
    // codes deal with communicating out-of-memory conditions.
    
    if (dynamic_cast<const std::bad_alloc*>(&ex) != NULL)
        return;
    
    // Write out XML header, exception tag and class attribute.
    
    ostr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" 
         << "<exception class=\"" << Encode(exType.name()) << "\"";
    
    // If the exception rethrower generates a value, add it as an attribute of the 
    // exception element.
    
    std::ostringstream  sstr;
    
    it->second->Externalize(ex, sstr);
    
    std::string value(sstr.str());
    
    if (!value.empty())
    {
        ostr << " value=\"" << value << "\"";
    }
    
    // Close out the tag.
    
    ostr << "/>";
}

// ------------------------------------------------------------------------------------------
void
ExceptionStreamer::Rethrow(
    std::istream&   istr) const
{
    // Read the XML data.
    
    std::vector<UInt8>  buffer(256);
    size_t              nused = 0, nread;
    
    do
    {
        if (nused >= buffer.size())
            buffer.resize(buffer.size() * 2);
        
        nread = istr.readsome(reinterpret_cast<char*>(&buffer[nused]), 
                              buffer.size() - nused);
        
        nused += nread;
        
    } while (nread > 0);
    
    if (nused == 0)
        return;
    
    OSPtr<CFDataRef>    xmlData(CFDataCreateWithBytesNoCopy(NULL, &buffer[0], nused, 
                                                            kCFAllocatorNull), 
                                from_copy);
    OSPtr<CFXMLTreeRef> docTree(CFXMLTreeCreateFromData(NULL, xmlData, NULL, 
                                                        kCFXMLParserSkipWhitespace, 
                                                        kCFXMLNodeCurrentVersion),
                                from_copy, std::nothrow);
    
    if (docTree == NULL)
        return;
    
    // Document node
    
    CFXMLNodeRef    docNode = CFXMLTreeGetNode(docTree);
    
    if ((docNode == NULL) || (CFXMLNodeGetTypeCode(docNode) != kCFXMLNodeTypeDocument))
        return;
    
    // Processing instruction node

    CFXMLTreeRef    elemTree = CFTreeGetFirstChild(docTree);
    
    if (elemTree == NULL)
        return;
    
    CFXMLNodeRef    elemNode = CFXMLTreeGetNode(elemTree);
    
    if ((elemNode == NULL) || (CFXMLNodeGetTypeCode(elemNode) != kCFXMLNodeTypeProcessingInstruction))
        return;
    
    // Element node
    
    elemTree = CFTreeGetNextSibling(elemTree);
    
    if (elemTree == NULL)
        return;
    
    elemNode = CFXMLTreeGetNode(elemTree);
    
    if ((elemNode == NULL) || (CFXMLNodeGetTypeCode(elemNode) != kCFXMLNodeTypeElement))
        return;
    
    // Exception element
    
    CFStringRef elemStr = CFXMLNodeGetString(elemNode);
    
    if ((elemStr == NULL) || !CFEqual(elemStr, CFSTR("exception")))
        return;
    
    const CFXMLElementInfo* elemInfo = reinterpret_cast<const CFXMLElementInfo*>(CFXMLNodeGetInfoPtr(elemNode));
    
    if ((elemInfo == NULL) || (elemInfo->attributes == NULL) || !elemInfo->isEmpty)
        return;
    
    // Locate the rethrower for this exception class.
    
    OSPtr<CFStringRef>  encodedClass;
    String              encodedClassStr;
    std::string         exClass;
    
    if (!CFUGet(elemInfo->attributes, CFSTR("class"), encodedClass))
        return;
    
    encodedClassStr = encodedClass;
    encodedClassStr.copy(exClass, kCFStringEncodingASCII);
    exClass = Decode(exClass);
    
    RethrowerMap::const_iterator    it  = mRethrowerMap.find(exClass);
    
    if (it == mRethrowerMap.end())
        return;
    
    // Retrieve the exception's value, if any.
    
    OSPtr<CFStringRef>  encodedValue;
    std::string         value;
    
    if (CFUGet(elemInfo->attributes, CFSTR("value"), encodedValue))
    {
        String  encodedValueStr;
        
        encodedValueStr = encodedValue;
        encodedValueStr.copy(value, kCFStringEncodingUTF8);
        value = Decode(value);
    }
    
    // Invoke the rethrower.
    
    std::istringstream  sstr(value);
    
    it->second->Rethrow(sstr);
}

// ------------------------------------------------------------------------------------------
std::string
ExceptionStreamer::Encode(const std::string& inString) const
{
    std::string             str(inString);
    std::string::size_type  pos = 0;
    
    while ((pos = str.find_first_of(mEncodedChars, pos)) != std::string::npos)
    {
        EncodingMap::const_iterator it  = mEncodingMap.find(str[pos]);
        
        B_ASSERT(it != mEncodingMap.end());
        
        str.replace(pos, 1, it->second);
        
        pos += it->second.size();
    }
    
    return (str);
}

// ------------------------------------------------------------------------------------------
std::string
ExceptionStreamer::Decode(const std::string& inString) const
{
    std::string             str(inString);
    std::string::size_type  pos = 0;
    
    while ((pos = str.find('&', pos)) != std::string::npos)
    {
        std::string::size_type  pos2    = str.find(';', pos+1);
        
        if (pos2 == std::string::npos)
            break;
        
        std::string                 entity(str.substr(pos+1, pos2-pos-1));
        DecodingMap::const_iterator it  = mDecodingMap.find(entity);
        
        if (it != mDecodingMap.end())
        {
            str.replace(pos, pos2-pos+1, 1, it->second);
            ++pos;
        }
        else
        {
            pos = pos2 + 1;
        }
    }
    
    return (str);
}


// ==========================================================================================
//  ExceptionStreamer::RethrowerBase

#pragma mark -

//// ------------------------------------------------------------------------------------------
//void
//ExceptionStreamer::RethrowerBase::WriteExString(
//  const std::exception&   ex, 
//  std::ostream&           ostr) const
//{
//  std::string str(ex.what());
//  
//  // Make sure we don't have any embedded end-of-line characters, as they will cause 
//  // problems during serialisation.
//  
//  std::string::size_type  pos = 0;
//  
//  while ((pos = str.find_first_of("\r\n", pos)) != std::string::npos)
//  {
//      str.replace(pos++, 1, " ");
//  }
//  
//  ostr << str << "\n";
//}

// ------------------------------------------------------------------------------------------
std::string
ExceptionStreamer::RethrowerBase::ReadExString(
    std::istream&           istr) const
{
    char        buff[256];
    size_t      nread;
    std::string str;
    
    while ((nread = istr.readsome(buff, sizeof(buff))) > 0)
    {
        str.append(buff, nread);
    }
    
    return (str);
}


// ==========================================================================================
//  Global Functions

#pragma mark -

// ------------------------------------------------------------------------------------------
void    intrusive_ptr_add_ref(ExceptionStreamer* s)
{
    B_ASSERT(s != NULL);
    
    IncrementAtomic(&s->mRefCount);
}

// ------------------------------------------------------------------------------------------
void    intrusive_ptr_release(ExceptionStreamer* s)
{
    B_ASSERT(s != NULL);
    B_ASSERT(s->mRefCount > 0);
    
    if (DecrementAtomic(&s->mRefCount) == 1)
    {
        // The ref count is now zero
        delete s;
    }
}

}   // namespace B
