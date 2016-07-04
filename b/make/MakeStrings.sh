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
#	
#	Generates string tables.
#	
#	usage:  MakeStrings.sh [-o resources-directory] source-file...

RSRC_DIR=.

# Parse arguments.

while [ $# -gt 0 ]
do
	case "$1" in
		-o) RSRC_DIR="$2"
			shift;;
		
		*)	break;;
	esac
	shift
done

# Generate the strings files from the source files.

mkdir -p $RSRC_DIR/ascii $RSRC_DIR/tmp
genstrings -s BLocalizedString -o $RSRC_DIR/tmp/ "$@"

# For every strings file in the temp directory, perform 3-way merge.

cd $RSRC_DIR/tmp/

for arg in *.strings
do
	if [ ! -f ../English.lproj/$arg ]
	then
		# The english file wasn't found
		cp -p $arg ../ascii/$arg
		mv $arg ../English.lproj/$arg
	else
		if [ ! -f ../ascii/$arg ] 
		then
			# The ascii file wasn't found
			mv $arg ../ascii/$arg
			MergeStrings ../ascii/$arg ../English.lproj/$arg
		else
			# We have the ascii file
			cmp --quiet ../ascii/$arg $arg || {
				# The ascii and temp files are different, so merge 
				# the changes into the english file.
				MergeStrings ../ascii/$arg $arg ../English.lproj/$arg
				mv $arg ../ascii/$arg
			}
		fi
	fi
done

cd -

# Clean up.

rm -rf $RSRC_DIR/tmp
