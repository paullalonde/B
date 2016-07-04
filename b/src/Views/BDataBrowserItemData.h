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

#ifndef BDataBrowserItemData_H_
#define BDataBrowserItemData_H_

#pragma once

// standard headers
#include <string>

// system headers
#include <Carbon/Carbon.h>


namespace B {


// forward declarations
class	String;


/*!	@brief	Handle class to get/set properties of a DataBrowserItem.
	
	@todo		%Document this class!
	@ingroup	ViewGroup
*/
class DataBrowserItemData
{
public:
	
	// constructor / destructor
	DataBrowserItemData(
		DataBrowserItemDataRef	inItemData);
	
	// inquiries -- call these from DataBrowserItem::SetData()
	String					GetText();
	SInt32					GetValue();
	SInt32					GetMinimum();
	SInt32					GetMaximum();
	Boolean					GetBoolean();
	MenuRef					GetMenu();
	void					GetColor(
								RGBColor&		outColor);
	ThemeDrawState			GetDrawState();
	ThemeButtonValue		GetButtonValue();
	IconTransformType		GetIconTransform();
	long					GetDateTime();
	void					GetLongDateTime(
								LongDateTime&	outLongDateTime);
	DataBrowserPropertyID	GetProperty();
	
	// modifiers -- call these from DataBrowserItem::GetData()
	void	SetIcon(
				IconRef				inIconRef);
	void	SetText(
				const String&		inText);
	void	SetText(
				CFStringRef			inText);
	void	SetValue(
				SInt32				inValue);
	void	SetMinimum(
				SInt32				inMinimum);
	void	SetMaximum(
				SInt32				inMaximum);
	void	SetBoolean(
				Boolean				inBoolean);
	void	SetMenu(
				MenuRef				inMenuRef);
	void	SetColor(
				const RGBColor&		inColor);
	void	SetDrawState(
				ThemeDrawState		inState);
	void	SetButtonValue(
				ThemeButtonValue	inValue);
	void	SetIconTransform(
				IconTransformType	inTransform);
	void	SetDateTime(
				long				inDateTime);
	void	SetLongDateTime(
				const LongDateTime&	inLongDateTime);
	void	SetItemID(
				DataBrowserItemID	inItemID);
	
private:
	
	// member variables
	DataBrowserItemDataRef	mItemData;
};


}	// namespace B


#endif	// BDataBrowserItemData_H_
