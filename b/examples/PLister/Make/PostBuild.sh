#!/bin/sh
# ==========================================================================================
#	
#	Copyright (C) 2003-2005 Paul Lalonde enrg.
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

case "$MW_CURRENT_TARGET" in
'Debug')		SUFFIX=.debug ;;
*)				SUFFIX=.app ;;
esac

B_DIR=/usr/local/b/b									; export B_DIR
PLISTER_DIR="$MW_PROJECT_DIRECTORY"						; export PLISTER_DIR
APP_DIR="$MW_OUTPUT_DIRECTORY/$MW_OUTPUT_NAME$SUFFIX"	; export APP_DIR
SDEF="$PLISTER_DIR/PLister.sdef"						; export SDEF

# Generate the 'aete' resource and merge it into the resource file.
make -s -f "$B_DIR/make/BMergeSdef.make"

# Remove Subversion admin directories
find "$APP_DIR" -type d -name '.svn' -print0 | xargs -0 -n 1 rm -rf
