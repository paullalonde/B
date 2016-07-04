#!/bin/sh
# ==========================================================================================
#	
#	Copyright (C) 2003 Paul Lalonde enrg.
#	
#	This program is free software;  you can redistribute it and/or modify it under the 
#	terms of the GNU General Public License as published by the Free Software Foundation;  
#	either version 2 of the License, or (at your option) any later version.
#	
#	This program is distributed in the hope that it will be useful, but WITHOUT ANY 
#	WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
#	PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License along with this 
#	program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
#	Suite 330, Boston, MA  02111-1307  USA
#	
# ==========================================================================================
#	
#	Prepares the I18n directory for translation.
#
#	The current directory is assumed to be the PLister directory
#	
#	usage:  PreGlot.sh path-to-appliation-bundle


APP_NAME=`basename "$1"`


# Remove OldBase and OldLoc, if they exist.

rm -rf "I18n/_OldBase/$APP_NAME"
rm -rf "I18n/_OldLoc/$APP_NAME"

# Move NewBase to OldBase, if it exists.

if [ -d "I18n/_NewBase/$APP_NAME" ]
then
	mv -f "I18n/_NewBase/$APP_NAME" "I18n/_OldBase/$APP_NAME"
fi

# Move NewLoc to OldLoc, if it exists.

if [ -d "I18n/_NewLoc/$APP_NAME" ]
then
	mv -f "I18n/_NewLoc/$APP_NAME" "I18n/_OldLoc/$APP_NAME"
fi

# Copy the application bundle to NewLoc.

cp -p -R "$1" "I18n/_NewBase/$APP_NAME"

exit 0
