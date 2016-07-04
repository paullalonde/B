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

#ifndef BAETextObject_H_
#define BAETextObject_H_

#pragma once

// B headers
#include "BAEObject.h"


namespace B {

// forward declarations
class	String;
namespace Graphics {
	class	Color;
}


class ITextBackingStore
{
public:
	
	virtual	~ITextBackingStore();
	
	//! @name Characters
	//@{
	virtual size_t			CountCharacters() const = 0;
	virtual AEObjectPtr		GetCharacterByIndex(size_t inIndex) const = 0;
	//@}
	
	//! @name Word
	//@{
	virtual size_t			CountWords() const = 0;
	virtual AEObjectPtr		GetWordByIndex(size_t inIndex) const = 0;
	//@}
	
	//! @name Paragraph
	//@{
	virtual void	GetParagraphLengths(std::vector<size_t>& outLengths) const = 0;
	//@}
	
	//! @name Attribute Runs
	//@{
	virtual size_t			CountAttributeRuns() const = 0;
	virtual AEObjectPtr		GetAttributeRunByIndex(size_t inIndex) const = 0;
	//@}
	
	//! @name Properties
	//@{
	virtual B::String			GetFontName(size_t inOffset) const = 0;
	virtual Fixed				GetFontSize(size_t inOffset) const = 0;
	virtual B::Graphics::Color	GetTextColor(size_t inOffset) const = 0;
	
	virtual void	SetFontName(const String& inFontName) = 0;
	virtual void	SetFontSize(Fixed inFontSize) = 0;
	virtual void	SetTextColor(const Graphics::Color& inFontName) = 0;
	//@}
	
protected:
	
	ITextBackingStore();
};


class AETextObject : public AEObject
{
public:
	
	//! @name Elements.
	//@{
	//! Returns the number of elements of the given class.
	virtual size_t		CountElements(
							DescType		inElementType) const;
	
	//! Returns the element of the given class with the given index.
	virtual AEObjectPtr	GetElementByIndex(
							DescType		inElementType, 
							size_t			inIndex) const;
	
	//! Returns all of the elements of the given class.
	virtual void		GetAllElements(
							DescType		inElementType, 
							std::list<AEObjectPtr>&	outElements) const;
	//@}
	
	//! @name Properties.
	//@{
	//! Returns the AEObject matching a given property, if any.
	virtual AEObjectPtr	GetPropertyObject(
							DescType		inPropertyID) const;
	//! Writes the value of the property identified by @a inPropertyID into @a ioWriter.
	virtual void		WriteProperty(
							DescType		inPropertyID, 
							AEWriter&		ioWriter) const;
	//! Reads the value of the property identified by @a inPropertyID from @a ioReader.
	virtual void		ReadProperty(
							DescType		inPropertyID, 
							AEReader&		ioReader);
	//@}

	//! @name Object Resolution.
	//@{
	//! Access elements according to @c formRange and @a inKeyData.
	virtual void	AccessElementsByRange(
						const AEInfo::ClassInfo&	inClassInfo,
						const AEInfo::ElementInfo&	inElementInfo,
						const AEDesc&				inKeyData,
						AEDesc&						outTokenDesc) const;
	//@}
	
protected:
	
	//! @name Constructors / Destructor.
	//@{
	//!	Constructor.
	explicit	AETextObject(
					AEObjectPtr			inContainer,
					DescType			inClassID,
					ITextBackingStore*	inBackingStore);
	//@}
	
	//! @name Attribute Runs
	//@{
	virtual size_t			CountAttributeRuns() const = 0;
	virtual AEObjectPtr		GetAttributeRunByIndex(size_t inIndex) const = 0;
	//@}
	
	virtual String			GetFontName() const = 0;
	virtual Fixed			GetFontSize() const = 0;
	virtual Graphics::Color	GetTextColor() const = 0;
	
	virtual void	SetFontName(const String& inFontName) = 0;
	virtual void	SetFontSize(Fixed inFontSize) = 0;
	virtual void	SetTextColor(const Graphics::Color& inFontName) = 0;

private:
	
	typedef AEObject	inherited;
	
	// member variables
	ITextBackingStore* const	mBackingStore;
};

}	// namespace B


#endif	// BAETextObject_H_
