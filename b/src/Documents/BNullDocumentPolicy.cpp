// ==========================================================================================
//	
//	Copyright (C) 2003-2006 Paul Lalonde enrg.
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

// file header
#include "BNullDocumentPolicy.h"

// B headers
#include "BErrorHandler.h"
#include "BException.h"
#include "BNullDocumentFactory.h"


namespace B {


NullDocumentFactory	NullDocumentPolicy::sFactory;

// ------------------------------------------------------------------------------------------
NullDocumentPolicy::FactoryType&
NullDocumentPolicy::GetFactory()
{
	return (sFactory);
}

// ------------------------------------------------------------------------------------------
void
NullDocumentPolicy::QuitApplication(OSType /* = kAEAsk */)
{
	QuitApplicationEventLoop();
}

// ------------------------------------------------------------------------------------------
/*!	Since this document policy doesn't instantiate documents, this implementation throws 
	an exception.
*/
AEObjectPtr
NullDocumentPolicy::OpenNewDocument(DescType, const AEDesc&, const AEDesc&)
{
	B_THROW(AECantHandleClassException());
	
	return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*!	Since this document policy doesn't instantiate documents, this implementation throws 
	an exception.
*/
SInt32
NullDocumentPolicy::CountDocuments(DescType) const
{
	B_THROW(AEClassHasNoElementsOfThisTypeException());
	
	return (0);
}

// ------------------------------------------------------------------------------------------
/*!	Since this document policy doesn't instantiate documents, this implementation throws 
	an exception.
*/
AEObjectPtr
NullDocumentPolicy::GetDocumentByIndex(DescType, unsigned) const
{
	B_THROW(AEClassHasNoElementsOfThisTypeException());
	
	return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*!	Since this document policy doesn't instantiate documents, this implementation throws 
	an exception.
*/
AEObjectPtr
NullDocumentPolicy::GetDocumentByName(DescType, const String&) const
{
	B_THROW(AEClassHasNoElementsOfThisTypeException());
	
	return AEObjectPtr();
}

// ------------------------------------------------------------------------------------------
/*!	Since this document policy doesn't instantiate documents, this implementation throws 
	an exception.
*/
AEObjectPtr
NullDocumentPolicy::GetDocumentByUniqueID(DescType, SInt32) const
{
	B_THROW(AEClassHasNoElementsOfThisTypeException());
	
	return AEObjectPtr();
}


}	// namespace B
