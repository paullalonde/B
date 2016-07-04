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
#	Utility makefile that copies a nib from one place to another.
#	
#	Requires the following variables, either in the environment 
#	or passed on the command line:
#		
#		SRC_DIR	- the directory containing the source nib
#		DST_DIR	- the directory containing the destination nib (need not exist)
#		NIB		- the name of the nib

SRC_NIB		:= $(SRC_DIR)/$(NIB)
DST_NIB		:= $(DST_DIR)/$(NIB)

NIB_FILES	:= $(DST_NIB)/classes.nib $(DST_NIB)/info.nib
XIB_FILES	:= $(DST_NIB)/objects.xib


.PHONY		: all clean

all			: $(NIB_FILES) $(XIB_FILES)

$(NIB_FILES)	: $(DST_NIB)/%.nib : $(SRC_NIB)/%.nib
	mkdir -p "$(@D)"
	cp -p "$<" "$@"

$(XIB_FILES)	: $(DST_NIB)/%.xib : $(SRC_NIB)/%.xib
	mkdir -p "$(@D)"
	cp -p "$<" "$@"

clean:
	rm -rf $(DST_NIB)
