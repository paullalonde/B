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
#	Makefile that copies the B resources for one language.
#	
#	Requires the following variables, either in the environment 
#	or passed on the command line:
#		
#		MAKE_LANG	- the language
#		SRC_DIR		- the source directory
#		DST_DIR		- the destination directory (need not exist)


STRINGS		:= $(DST_DIR)/B.strings
NIBS		:= $(DST_DIR)/B.nib

.PHONY		: all clean

all			: $(STRINGS) $(NIBS)

$(STRINGS)	: $(DST_DIR)/%.strings : $(SRC_DIR)/%.strings
	mkdir -p "$(@D)"
	cp -p "$<" "$@"

$(NIBS)		: $(DST_DIR)/%.nib : $(SRC_DIR)/%.nib
	$(MAKE) -f "$(B_DIR)/make/CopyNib.make" SRC_DIR="$(SRC_DIR)" \
		DST_DIR="$(DST_DIR)" NIB="$*.nib"

clean:
	rm -rf $(STRINGS)
