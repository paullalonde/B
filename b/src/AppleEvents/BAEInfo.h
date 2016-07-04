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

#ifndef BAEInfo_H_
#define BAEInfo_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>

// library headers
#include <boost/utility.hpp>

// B headers
#include "BFwd.h"


namespace B {

// forward declarations
class	AEToken;
class	AEWriter;

namespace AEInfo {

enum EventDOBehavior
{
	kEventDOOptional,
	kEventDORequired,
	kEventDOIllegal,
	kEventDOBadOK,
};

enum EventResultAction
{
	kEventResultActionNone,
	kEventResultActionDefault,
	kEventResultActionCountList,
	kEventResultActionCollapseBooleanList,
};

enum Access
{
	kAccessRead			= 0x01,
	kAccessWrite		= 0x02,
	kAccessReadWrite	= kAccessRead | kAccessWrite
};

//! @name Types
//@{

//! The type of a default event handler.
typedef boost::function3<void, 
	const AEDesc&,
	const AppleEvent&,
	AEWriter&>		DefaultEventHandler;

//! The type of a default event handler.
typedef boost::function3<void, 
	const AEToken&,
	const AppleEvent&,
	AEWriter&>		ClassEventHandler;

//! The type of a comparison handler.
typedef boost::function3<bool, 
	DescType,
	const AEDesc&,
	const AEDesc&>	DescriptorComparer;

//! Uniquely identifies an Apple %Event.
typedef std::pair<AEEventClass, AEEventID>	EventKey;

struct EventKeyHash : public std::unary_function<EventKey, size_t>
{
	size_t	operator()(const EventKey& key) const
				{ return (key.first | key.second); }
};

//! Describes an Apple %Event.
struct EventInfo
{
	EventKey			mEventKey;
	EventDOBehavior		mDOBehavior;
	EventResultAction	mResultAction;
	DefaultEventHandler	mDefaultHandler;
};

//! The collection of supported Apple %Events.
#if defined(__MWERKS__)
typedef Metrowerks::hash_map<EventKey, EventInfo, EventKeyHash>	EventMap;
#else defined(__GNUC__)
typedef __gnu_cxx::hash_map<EventKey, EventInfo, EventKeyHash>	EventMap;
#endif

//! A set of supported key forms.
#if defined(__MWERKS__)
typedef Metrowerks::hash_set<DescType>	KeyFormSet;
#else defined(__GNUC__)
typedef __gnu_cxx::hash_set<DescType>	KeyFormSet;
#endif

//! Describes an element of a class.
struct ElementInfo
{
	//! The element's unique name.
	DescType	mName;
	//! The access rights on this element.
	Access		mAccess;
	//! The supported key forms for this element.
	KeyFormSet	mKeyForms;
};

//! A collection of the elements supported by a class.
#if defined(__MWERKS__)
typedef __gnu_cxx::hash_map<DescType, ElementInfo>	ElementMap;
#else defined(__GNUC__)
typedef __gnu_cxx::hash_map<DescType, ElementInfo>	ElementMap;
#endif

//! Describes a property of a class.
struct PropertyInfo
{
	//! The property's unique name.
	DescType	mName;
	//! The access rights on this property.
	Access		mAccess;
	//! Is the property in the 'properties' property?
	bool		mInProperties;
	//! Is the property list-valued?
	bool		mIsList;
};

//! A collection of the properties supported by a class.
#if defined(__MWERKS__)
typedef __gnu_cxx::hash_map<DescType, PropertyInfo>	PropertyMap;
#else defined(__GNUC__)
typedef __gnu_cxx::hash_map<DescType, PropertyInfo>	PropertyMap;
#endif

//! The collection of Apple %Events and their handlers supported by a class.
#if defined(__MWERKS__)
typedef __gnu_cxx::hash_map<EventKey, ClassEventHandler, EventKeyHash>	ClassEventMap;
#else defined(__GNUC__)
typedef __gnu_cxx::hash_map<EventKey, ClassEventHandler, EventKeyHash>	ClassEventMap;
#endif

//! Describes a class.
struct ClassInfo
{
	DescType			mCode;
	ElementMap			mElements;
	PropertyMap			mProperties;
	ClassEventMap		mEvents;
	KeyFormSet			mAllKeyForms;
	DescType			mParentCode;
	std::set<DescType>	mAncestors;
	std::set<DescType>	mDescendents;
};

//! A collection of the supported classes.
typedef std::map<DescType, ClassInfo>	ClassMap;

}	// namespace AEInfo
}	// namespace B


#endif	// BAEInfo_H_
