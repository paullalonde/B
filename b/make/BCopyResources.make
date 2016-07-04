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
#	Makefile that copies the B resources for all languages.
#	
#	Requires the following variables, either in the environment 
#	or passed on the command line:
#		
#		B_DIR	- the directory containing the B framework
#		DST_DIR	- the destination Resources directory (need not exist)


SRC_DIR			:= $(B_DIR)/src/Resources
SUB_MAKEFILE	:= $(B_DIR)/make/CopyLangResources.make
LANG_MAKEFILE	:= $(B_DIR)/make/BCopyLangResources.make


.PHONY		: all clean

all			: 
	$(MAKE) -f "$(SUB_MAKEFILE)" SRC_DIR="$(SRC_DIR)" DST_DIR="$(DST_DIR)" \
		LANG_MAKEFILE="$(LANG_MAKEFILE)"

clean		: 
	$(MAKE) -f "$(SUB_MAKEFILE)" SRC_DIR="$(SRC_DIR)" DST_DIR="$(DST_DIR)" \
		LANG_MAKEFILE="$(LANG_MAKEFILE)" $@
