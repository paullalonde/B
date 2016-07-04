// ==========================================================================================
//	
//	Copyright (C) 2006 Paul Lalonde enrg.
//	
//	This program is free software;  you can redistribute it and/or modify it under the 
//	terms of the GNU General Public License as published by the Free Software Foundation;  
//	either version 2 of the License, or (at your option) any later version.
//	
//	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with this 
//	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
//	Suite 330, Boston, MA  02111-1307  USA
//	
// ==========================================================================================

#ifndef BTextFilters_H_
#define BTextFilters_H_

#pragma once

// B headers
#include "BOSPtr.h"
#include "BTextValidator.h"


namespace B {

// forward declarations
class	TextSource;

namespace TextFilters {


// ==========================================================================================
//	CharacterSet

class CharacterSet
{
public:
	
	class Exception : public TextValidator::Exception
	{
	public:
		
		virtual	~Exception() throw();

		virtual const char*	what() const throw();
	};

	//! @name Constructor / Destructor.
	//@{
	//! Constructor.
	CharacterSet(OSPtr<CFCharacterSetRef> inCharSet);
	//! Constructor.
	CharacterSet(CFCharacterSetPredefinedSet inPredefinedSet);
	//@}
	
	TextValidator::Action
	operator () (
		bool				inFiltering,
		const TextSource&	inSource,
		const String&		inCandidateText,
		const CFRange&		inSelection,
		String&				outReplacementText) const;
	
private:
	
	const OSPtr<CFCharacterSetRef>	mCharSet;
};


// ==========================================================================================
//	Length

class Length
{
public:
	
	class Exception : public TextValidator::Exception
	{
	public:
		
		explicit	Exception(size_t inLength, size_t inMinLength, size_t inMaxLength);
		virtual		~Exception() throw();
		
		virtual const char*	what() const throw();
		size_t				GetLength() const		{ return (mLength); }
		size_t				GetMinLength() const	{ return (mMinLength); }
		size_t				GetMaxLength() const	{ return (mMaxLength); }
		
	private:
		
		const size_t	mLength;
		const size_t	mMinLength;
		const size_t	mMaxLength;
	};

	//! @name Constructor / Destructor.
	//@{
	//! Constructor.
	Length(size_t inMaxLength);
	//! Constructor.
	Length(size_t inMinLength, size_t inMaxLength);
	//@}
	
	TextValidator::Action
	operator () (
		bool				inFiltering,
		const TextSource&	inSource,
		const String&		inCandidateText,
		const CFRange&		inSelection,
		String&				outReplacementText) const;
	
private:
	
	const size_t	mMinLength;
	const size_t	mMaxLength;
};


// ==========================================================================================
//	NumberFilter

class NumberFilter
{
public:
	
	class Exception : public TextValidator::Exception
	{
	public:
		
		explicit	Exception(
						OSPtr<CFNumberFormatterRef>		inFormatter,
						CFNumberFormatterOptionFlags	inFlags);
		virtual		~Exception() throw();
		
		virtual const char*				what() const throw();
		OSPtr<CFNumberFormatterRef>		GetFormatter() const	{ return (mFormatter); }
		CFNumberFormatterOptionFlags	GetFlags() const		{ return (mFlags); }
		
	private:
		
		OSPtr<CFNumberFormatterRef>		mFormatter;
		CFNumberFormatterOptionFlags	mFlags;
	};
	
	OSPtr<CFNumberRef>
			Parse(const String& inText) const;
	
	String	Format(const OSPtr<CFNumberRef>& inNumber) const;
	String	Format(int inNumber) const;
	String	Format(double inNumber) const;
	
	TextValidator::Action
			operator () (
				bool				inFiltering,
				const TextSource&	inSource,
				const String&		inCandidateText,
				const CFRange&		inSelection,
				String&				outReplacementText) const;
	
protected:
	
	typedef boost::function1<void, CFNumberRef>	ValidationFunction;
	
	//! @name Constructor / Destructor.
	//@{
				//! Constructor.
	explicit	NumberFilter(
					OSPtr<CFNumberFormatterRef>		inFormatter,
					ValidationFunction				inValidator,
					bool							inValidateOnFilter,
					bool							inIntegerOnly);
	//@}
	
	OSPtr<CFNumberFormatterRef>		GetFormatter() const	{ return (mFormatter); }
	CFNumberFormatterOptionFlags	GetFlags() const		{ return (mFlags); }
	
private:
	
	static CFNumberFormatterOptionFlags	MakeFlags(bool inIntegerOnly);
	
	static CFRange	FindNonWhitespaceRange(const String& str);
	
	// member variables
	const OSPtr<CFNumberFormatterRef>	mFormatter;
	const CFNumberFormatterOptionFlags	mFlags;
	const ValidationFunction			mValidator;
	const bool							mValidateOnFilter;
	
	// static member variables
	static const OSPtr<CFCharacterSetRef>	sNonWSCharSet;
};


// ==========================================================================================
//	Number

class Number : public NumberFilter
{
public:
	
	//! @name Constructor / Destructor.
	//@{
				//! Constructor.
	explicit	Number(
					OSPtr<CFNumberFormatterRef>	inFormatter,
					bool						inIntegerOnly = false,
					bool						inValidateOnFilter = false);
	//@}

private:
	
	void	Validate(CFNumberRef number) const;
};


// ==========================================================================================
//	NumberRange

class NumberRange : public NumberFilter
{
public:
	
	class Exception : public NumberFilter::Exception
	{
	public:
		
		explicit	Exception(
						OSPtr<CFNumberFormatterRef>		inFormatter,
						CFNumberFormatterOptionFlags	inFlags,
						OSPtr<CFNumberRef>				inNumber,
						OSPtr<CFNumberRef>				inMinNumber,
						OSPtr<CFNumberRef>				inMaxNumber);
		virtual		~Exception() throw();
		
		virtual const char*	what() const throw();
		OSPtr<CFNumberRef>	GetNumber() const		{ return (mNumber); }
		OSPtr<CFNumberRef>	GetMinNumber() const	{ return (mMinNumber); }
		OSPtr<CFNumberRef>	GetMaxNumber() const	{ return (mMaxNumber); }
		
	private:
		
		OSPtr<CFNumberRef>	mNumber;
		OSPtr<CFNumberRef>	mMinNumber;
		OSPtr<CFNumberRef>	mMaxNumber;
	};
	
	//! @name Constructor / Destructor.
	//@{
	//! Constructor.
	template <typename T>
	explicit	NumberRange(
					OSPtr<CFNumberFormatterRef>	inFormatter,
					T							inMinNumber,
					T							inMaxNumber,
					bool						inIntegerOnly = false,
					bool						inValidateOnFilter = false);
	//@}
	
private:
	
	template <typename T>
	static OSPtr<CFNumberRef>	MakeNumber(T value);
	
	void	Validate(CFNumberRef number) const;
	
	// member variables
	const OSPtr<CFNumberRef>	mMinNumber;
	const OSPtr<CFNumberRef>	mMaxNumber;
};

// ------------------------------------------------------------------------------------------
template <typename T>
NumberRange::NumberRange(
	OSPtr<CFNumberFormatterRef>	inFormatter,
	T							inMinNumber,
	T							inMaxNumber,
	bool						inIntegerOnly /* = false */,
	bool						inValidateOnFilter /* = false */)
		: NumberFilter(inFormatter, boost::bind(&NumberRange::Validate, this, _1), 
					   inValidateOnFilter, inIntegerOnly),
		  mMinNumber(MakeNumber<T>(inMinNumber)), 
		  mMaxNumber(MakeNumber<T>(inMaxNumber))
{
}


// ==========================================================================================
//	DateFilter

class DateFilter
{
public:
	
	class Exception : public TextValidator::Exception
	{
	public:
		
		explicit	Exception(
						OSPtr<CFDateFormatterRef>	inFormatter);
		virtual		~Exception() throw();
		
		virtual const char*			what() const throw();
		OSPtr<CFDateFormatterRef>	GetFormatter() const	{ return (mFormatter); }
		
	private:
		
		const OSPtr<CFDateFormatterRef>	mFormatter;
	};
	
	OSPtr<CFDateRef>
			Parse(const String& inText) const;
	String	Format(const OSPtr<CFDateRef>& inDate) const;
	
	TextValidator::Action
			operator () (
				bool				inFiltering,
				const TextSource&	inSource,
				const String&		inCandidateText,
				const CFRange&		inSelection,
				String&				outReplacementText) const;
	
protected:
	
	typedef boost::function1<void, CFDateRef>	ValidationFunction;
	
	//! @name Constructor / Destructor.
	//@{
	//! Constructor.
	explicit	DateFilter(
					OSPtr<CFDateFormatterRef>	inFormatter,
					ValidationFunction			inValidator,
					bool						inValidateOnFilter);
	//@}
	
	OSPtr<CFDateRef>	Parse(const String& inText, const std::nothrow_t&) const;
	
	OSPtr<CFDateFormatterRef>	GetFormatter() const	{ return (mFormatter); }
	
private:
	
	// member variables
	const OSPtr<CFDateFormatterRef>	mFormatter;
	const ValidationFunction		mValidator;
	const bool						mValidateOnFilter;
};


// ==========================================================================================
//	Date

class Date : public DateFilter
{
public:
	
	//! @name Constructor / Destructor.
	//@{
	//! Constructor.
	explicit	Date(
					OSPtr<CFDateFormatterRef>	inFormatter,
					bool						inValidateOnFilter = false);
	//@}

private:
	
	void	Validate(CFDateRef date) const;
};


// ==========================================================================================
//	DateRange

class DateRange : public DateFilter
{
public:
	
	class Exception : public DateFilter::Exception
	{
	public:
		
		explicit	Exception(
						OSPtr<CFDateFormatterRef>	inFormatter,
						OSPtr<CFDateRef>			inDate,
						OSPtr<CFDateRef>			inMinDate,
						OSPtr<CFDateRef>			inMaxDate);
		virtual		~Exception() throw();
		
		virtual const char*	what() const throw();
		OSPtr<CFDateRef>	GetDate() const		{ return (mDate); }
		OSPtr<CFDateRef>	GetMinDate() const	{ return (mMinDate); }
		OSPtr<CFDateRef>	GetMaxDate() const	{ return (mMaxDate); }
		
	private:
		
		const OSPtr<CFDateRef>	mDate;
		const OSPtr<CFDateRef>	mMinDate;
		const OSPtr<CFDateRef>	mMaxDate;
	};
	
	//! @name Constructor / Destructor.
	//@{
	//! Constructor.
	explicit	DateRange(
					OSPtr<CFDateFormatterRef>	inFormatter,
					CFDateRef					inMinDate,
					CFDateRef					inMaxDate,
					bool						inValidateOnFilter = false);
	//@}
	
private:
	
	void	Validate(CFDateRef date) const;
	
	// member variables
	const OSPtr<CFDateRef>	mMinDate;
	const OSPtr<CFDateRef>	mMaxDate;
};

}	// namespace TextFilters
}	// namespace B


#endif	// BTextFilters_H_
