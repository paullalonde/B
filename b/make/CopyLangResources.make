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
#	Utility makefile that applies a makefile to all language directories 
#	under a given root directory.
#	
#	Requires the following variables, either in the environment 
#	or passed on the command line:
#		
#		SRC_DIR			- the source Resources directory
#		DST_DIR			- the destination Resources directory (need not exist)
#		LANG_MAKEFILE	- the makefile to invoke
#
#	The sub-makefile (LANG_MAKEFILE) is invoked with the following definitions:
#
#		MAKE_LANG	- the language
#		SRC_DIR		- the source language directory
#		DST_DIR		- the destination language directory


ALL_LANGS	:=	$(basename $(notdir $(wildcard $(SRC_DIR)/*.lproj)))


.PHONY		: all clean $(ALL_LANGS)

all			: $(ALL_LANGS)

$(ALL_LANGS)	:
	$(MAKE) -f "$(LANG_MAKEFILE)" MAKE_LANG="$@" \
		SRC_DIR="$(SRC_DIR)/$@.lproj" \
		DST_DIR="$(DST_DIR)/$@.lproj"

clean:
	rm -rf $(wildcard $(DST_DIR)/*.lproj)
