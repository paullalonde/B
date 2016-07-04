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
#include "BTextFilters.h"

// B headers
#include "BStringFormatter.h"
#include "BTextSource.h"


namespace B {
namespace TextFilters {

// ==========================================================================================
//  CharacterSet

// ------------------------------------------------------------------------------------------
CharacterSet::CharacterSet(OSPtr<CFCharacterSetRef> inCharSet)
    : mCharSet(inCharSet)
{
}

// ------------------------------------------------------------------------------------------
CharacterSet::CharacterSet(CFCharacterSetPredefinedSet inPredefinedSet)
    : mCharSet(CFCharacterSetGetPredefined(inPredefinedSet))
{
}

// ------------------------------------------------------------------------------------------
TextValidator::Action
CharacterSet::operator () (
    bool                /* inFiltering */,
    const TextSource&   /* inSource */,
    const String&       inCandidateText,
    const CFRange&      /* inSelection */,
    String&             /* outReplacementText */) const
{
    size_t  size    = inCandidateText.size();
    
    if (size > 0)
    {
        CFRange search  = CFRangeMake(0, size);
        CFRange result;
        
        if (CFStringFindCharacterFromSet(inCandidateText.cf_ref(), mCharSet, search, 0, &result))
            return TextValidator::Accept;
        else
            throw Exception();
    }
    else
    {
        // The candidate text is empty, which probably means we are deleting
        // text. Accept it.
        
        return TextValidator::Accept;
    }
}


// ==========================================================================================
//  CharacterSet::Exception

// ------------------------------------------------------------------------------------------
CharacterSet::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
CharacterSet::Exception::what() const throw()
{
    return "B::TextFilters::CharacterSet::Exception";
}


// ==========================================================================================
//  Length

// ------------------------------------------------------------------------------------------
Length::Length(size_t inMaxLength)
    : mMinLength(0), mMaxLength(inMaxLength)
{
}

// ------------------------------------------------------------------------------------------
Length::Length(size_t inMinLength, size_t inMaxLength)
    : mMinLength(inMinLength), mMaxLength(inMaxLength)
{
}

// ------------------------------------------------------------------------------------------
TextValidator::Action
Length::operator () (
    bool                inFiltering,
    const TextSource&   inSource,
    const String&       inCandidateText,
    const CFRange&      inSelection,
    String&             /* outReplacementText */) const
{
    size_t  size;
    
    if (inFiltering)
        size = inSource.Combine(inSelection, inCandidateText).size();
    else
        size = inCandidateText.size();
    
    if ((size >= mMinLength) && (size <= mMaxLength))
        return TextValidator::Accept;
    else
        throw Exception(size, mMinLength, mMaxLength);
}


// ==========================================================================================
//  Length::Exception

// ------------------------------------------------------------------------------------------
Length::Exception::Exception(size_t inLength, size_t inMinLength, size_t inMaxLength)
    : mLength(inLength), mMinLength(inMinLength), mMaxLength(inMaxLength)
{
}

// ------------------------------------------------------------------------------------------
Length::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
Length::Exception::what() const throw()
{
    return "B::TextFilters::Length::Exception";
}


// ==========================================================================================
//  NumberFilter

const OSPtr<CFCharacterSetRef>  NumberFilter::sNonWSCharSet(
                                    CFCharacterSetCreateInvertedSet(NULL, 
                                        CFCharacterSetGetPredefined(
                                            kCFCharacterSetWhitespaceAndNewline)),
                                    from_copy);

// ------------------------------------------------------------------------------------------
NumberFilter::NumberFilter(
    OSPtr<CFNumberFormatterRef>     inFormatter,
    ValidationFunction              inValidator,
    bool                            inValidateOnFilter,
    bool                            inIntegerOnly)
        : mFormatter(inFormatter), mFlags(MakeFlags(inIntegerOnly)), 
          mValidator(inValidator), mValidateOnFilter(inValidateOnFilter)
{
}

// ------------------------------------------------------------------------------------------
CFNumberFormatterOptionFlags
NumberFilter::MakeFlags(bool inIntegerOnly)
{
    return (inIntegerOnly ? kCFNumberFormatterParseIntegersOnly 
                          : static_cast<CFNumberFormatterOptionFlags>(0));
}

// ------------------------------------------------------------------------------------------
TextValidator::Action
NumberFilter::operator () (
    bool                inFiltering,
    const TextSource&   inSource,
    const String&       inCandidateText,
    const CFRange&      inSelection,
    String&             /* outReplacementText */) const
{
    if (!inFiltering && !mValidateOnFilter)
    {
        // We don't perform any validation when filtering.
        
        return TextValidator::Accept;
    }
    
    String  text;
    
    if (!inFiltering)
        text = inCandidateText;
    else
        text = inSource.Combine(inSelection, inCandidateText);
    
    OSPtr<CFNumberRef>  number(Parse(text));

    mValidator(number.get());
    
    return TextValidator::Accept;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFNumberRef>
NumberFilter::Parse(
    const String&   inText) const
{
    CFRange             textRange   = FindNonWhitespaceRange(inText);
    CFRange             parsedRange = textRange;
    OSPtr<CFNumberRef>  numRef(CFNumberFormatterCreateNumberFromString(
                                    NULL, mFormatter, inText.cf_ref(), &parsedRange, mFlags),
                               B::from_copy, std::nothrow);
    
    if (numRef.get() == NULL)
    {
        throw NumberFilter::Exception(GetFormatter(), GetFlags());
    }
    
    if ((parsedRange.location != textRange.location) || 
        (parsedRange.length   != textRange.length))
    {
        throw NumberFilter::Exception(GetFormatter(), GetFlags());
    }
    
    return numRef;
}

// ------------------------------------------------------------------------------------------
String
NumberFilter::Format(const OSPtr<CFNumberRef>& inNumber) const
{
    return (String(CFNumberFormatterCreateStringWithNumber(
                        NULL, mFormatter, inNumber),
                   B::from_copy));
}

// ------------------------------------------------------------------------------------------
String
NumberFilter::Format(int inNumber) const
{
    return (String(CFNumberFormatterCreateStringWithValue(
                        NULL, mFormatter, kCFNumberIntType, &inNumber),
                   B::from_copy));
}

// ------------------------------------------------------------------------------------------
String
NumberFilter::Format(double inNumber) const
{
    return (String(CFNumberFormatterCreateStringWithValue(
                        NULL, mFormatter, kCFNumberDoubleType, &inNumber),
                   B::from_copy));
}

// ------------------------------------------------------------------------------------------
CFRange
NumberFilter::FindNonWhitespaceRange(const String& str)
{
    CFStringRef strRef  = str.cf_ref();
    CFIndex     len     = CFStringGetLength(strRef);
    CFRange     prefixRange, suffixRange;
    CFIndex     firstNonWS = 0, lastNonWS = len;
    
    if (CFStringFindCharacterFromSet(strRef, sNonWSCharSet, CFRangeMake(0, len), 
                                     0, &prefixRange))
    {
        firstNonWS = prefixRange.location;
    }
    
    if (CFStringFindCharacterFromSet(strRef, sNonWSCharSet, CFRangeMake(0, len), 
                                     kCFCompareBackwards, &suffixRange))
    {
        lastNonWS = suffixRange.location + suffixRange.length;
    }
    
    return CFRangeMake(firstNonWS, lastNonWS - firstNonWS);
}


// ==========================================================================================
//  NumberFilter::Exception

// ------------------------------------------------------------------------------------------
NumberFilter::Exception::Exception(
    OSPtr<CFNumberFormatterRef>     inFormatter,
    CFNumberFormatterOptionFlags    inFlags)
        : mFormatter(inFormatter), mFlags(inFlags)
{
}

// ------------------------------------------------------------------------------------------
NumberFilter::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
NumberFilter::Exception::what() const throw()
{
    return "B::TextFilters::NumberFilter::Exception";
}


// ==========================================================================================
//  Number

// ------------------------------------------------------------------------------------------
Number::Number(
    OSPtr<CFNumberFormatterRef> inFormatter,
    bool                        inIntegerOnly /* = false */,
    bool                        inValidateOnFilter /* = false */)
        : NumberFilter(inFormatter, boost::bind(&Number::Validate, this, _1), 
                       inValidateOnFilter, inIntegerOnly)
{
}

// ------------------------------------------------------------------------------------------
void
Number::Validate(CFNumberRef inNumber) const
{
    if (inNumber == NULL)
        throw NumberFilter::Exception(GetFormatter(), GetFlags());
}


// ==========================================================================================
//  NumberRange

// ------------------------------------------------------------------------------------------
template <>
NumberRange::NumberRange(
    OSPtr<CFNumberFormatterRef> inFormatter,
    CFNumberRef                 inMinNumber,
    CFNumberRef                 inMaxNumber,
    bool                        inIntegerOnly /* = false */,
    bool                        inValidateOnFilter /* = false */)
        : NumberFilter(inFormatter, boost::bind(&NumberRange::Validate, this, _1), 
                       inValidateOnFilter, inIntegerOnly),
          mMinNumber(inMinNumber), 
          mMaxNumber(inMaxNumber)
{
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(char value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberCharType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(short value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberShortType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(int value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberIntType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(long value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberLongType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(long long value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberLongLongType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(float value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberFloatType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
template <> OSPtr<CFNumberRef>
NumberRange::MakeNumber(double value)
{
    return OSPtr<CFNumberRef>(CFNumberCreate(NULL, kCFNumberDoubleType, &value), 
                              B::from_copy);
}

// ------------------------------------------------------------------------------------------
void
NumberRange::Validate(CFNumberRef number) const
{
    if (number == NULL)
        throw NumberFilter::Exception(GetFormatter(), GetFlags());
    
    if (mMinNumber.get() != NULL)
    {
        if (CFNumberCompare(number, mMinNumber, NULL) < 0)
            throw Exception(GetFormatter(), GetFlags(), OSPtr<CFNumberRef>(number), 
                            mMinNumber, mMaxNumber);
    }
    
    if (mMaxNumber.get() != NULL)
    {
        if (CFNumberCompare(number, mMaxNumber, NULL) > 0)
            throw Exception(GetFormatter(), GetFlags(), OSPtr<CFNumberRef>(number), 
                            mMinNumber, mMaxNumber);
    }
}


// ==========================================================================================
//  NumberRange::Exception

// ------------------------------------------------------------------------------------------
NumberRange::Exception::Exception(
    OSPtr<CFNumberFormatterRef>     inFormatter,
    CFNumberFormatterOptionFlags    inFlags,
    OSPtr<CFNumberRef>              inNumber,
    OSPtr<CFNumberRef>              inMinNumber,
    OSPtr<CFNumberRef>              inMaxNumber)
        : Number::Exception(inFormatter, inFlags),
          mNumber(inNumber), mMinNumber(inMinNumber), mMaxNumber(inMaxNumber)
{
}

// ------------------------------------------------------------------------------------------
NumberRange::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
NumberRange::Exception::what() const throw()
{
    return "B::TextFilters::NumberRange::Exception";
}


// ==========================================================================================
//  DateFilter

// ------------------------------------------------------------------------------------------
DateFilter::DateFilter(
    OSPtr<CFDateFormatterRef>   inFormatter,
    ValidationFunction          inValidator,
    bool                        inValidateOnFilter)
        : mFormatter(inFormatter), mValidator(inValidator), 
          mValidateOnFilter(inValidateOnFilter)
{
}

// ------------------------------------------------------------------------------------------
TextValidator::Action
DateFilter::operator () (
    bool                inFiltering,
    const TextSource&   inSource,
    const String&       inCandidateText,
    const CFRange&      inSelection,
    String&             /* outReplacementText */) const
{
    if (!inFiltering && !mValidateOnFilter)
    {
        // We don't perform any validation when filtering.
        
        return TextValidator::Accept;
    }
    
    String  text;
    
    if (!inFiltering)
        text = inCandidateText;
    else
        text = inSource.Combine(inSelection, inCandidateText);
    
    OSPtr<CFDateRef>    date(Parse(text, std::nothrow));

    mValidator(date.get());
    
    return TextValidator::Accept;
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDateRef>
DateFilter::Parse(
    const String&   inText) const
{
    return (OSPtr<CFDateRef>(CFDateFormatterCreateDateFromString(
                                    NULL, mFormatter, inText.cf_ref(), NULL),
                             B::from_copy));
}

// ------------------------------------------------------------------------------------------
OSPtr<CFDateRef>
DateFilter::Parse(
    const String&   inText,
    const std::nothrow_t&) const
{
    return (OSPtr<CFDateRef>(CFDateFormatterCreateDateFromString(
                                    NULL, mFormatter, inText.cf_ref(), NULL),
                             B::from_copy, std::nothrow));
}

// ------------------------------------------------------------------------------------------
String
DateFilter::Format(const OSPtr<CFDateRef>& inDate) const
{
    return (String(CFDateFormatterCreateStringWithDate(
                        NULL, mFormatter, inDate),
                   B::from_copy));
}


// ==========================================================================================
//  DateFilter::Exception

// ------------------------------------------------------------------------------------------
DateFilter::Exception::Exception(
    OSPtr<CFDateFormatterRef>   inFormatter)
        : mFormatter(inFormatter)
{
}

// ------------------------------------------------------------------------------------------
DateFilter::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
DateFilter::Exception::what() const throw()
{
    return "B::TextFilters::DateFilter::Exception";
}


// ==========================================================================================
//  Date

// ------------------------------------------------------------------------------------------
Date::Date(
    OSPtr<CFDateFormatterRef>   inFormatter,
    bool                        inValidateOnFilter /* = false */)
        : DateFilter(inFormatter, boost::bind(&Date::Validate, this, _1), 
                     inValidateOnFilter)
{
}

// ------------------------------------------------------------------------------------------
void
Date::Validate(CFDateRef date) const
{
    if (date == NULL)
        throw DateFilter::Exception(GetFormatter());
}


// ==========================================================================================
//  DateRange

// ------------------------------------------------------------------------------------------
DateRange::DateRange(
    OSPtr<CFDateFormatterRef>   inFormatter,
    CFDateRef                   inMinDate,
    CFDateRef                   inMaxDate,
    bool                        inValidateOnFilter /* = false */)
        : DateFilter(inFormatter, boost::bind(&DateRange::Validate, this, _1), 
                     inValidateOnFilter),
          mMinDate(inMinDate), mMaxDate(inMaxDate)
{
}

// ------------------------------------------------------------------------------------------
void
DateRange::Validate(CFDateRef date) const
{
    if (date == NULL)
        throw DateFilter::Exception(GetFormatter());
    
    if (mMinDate.get() != NULL)
    {
        if (CFDateCompare(date, mMinDate, NULL) < 0)
            throw Exception(GetFormatter(), OSPtr<CFDateRef>(date), mMinDate, mMaxDate);
    }
    
    if (mMaxDate.get() != NULL)
    {
        if (CFDateCompare(date, mMaxDate, NULL) > 0)
            throw Exception(GetFormatter(), OSPtr<CFDateRef>(date), mMinDate, mMaxDate);
    }
}


// ==========================================================================================
//  DateRange::Exception

// ------------------------------------------------------------------------------------------
DateRange::Exception::Exception(
    OSPtr<CFDateFormatterRef>   inFormatter,
    OSPtr<CFDateRef>            inDate,
    OSPtr<CFDateRef>            inMinDate,
    OSPtr<CFDateRef>            inMaxDate)
        : Date::Exception(inFormatter),
          mDate(inDate), mMinDate(inMinDate), mMaxDate(inMaxDate)
{
}

// ------------------------------------------------------------------------------------------
DateRange::Exception::~Exception() throw()
{
}

// ------------------------------------------------------------------------------------------
const char*
DateRange::Exception::what() const throw()
{
    return "B::TextFilters::DateRange::Exception";
}


}   // namespace TextFilters
}   // namespace B
