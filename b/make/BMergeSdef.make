# ==========================================================================================
#	
#	Copyright (C) 2004-2005 Paul Lalonde enrg.
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
#	This makefile copies & transforms a .sdef file containing an XML description of an 
#	application's Apple Event terminology.
#	
#	It transforms the .sdef into an 'aete' resource, which is merged into the 
#	application's non-localised resource file.  This resource is currently required by 
#	AppleScript.
#	
#	Requires the following variables, either in the environment 
#	or passed on the command line:
#		
#		SDEF		- The path to the source (.sdef) file
#		APP_DIR		- The destination bundle directory (need not exist)

SDEF_NAME	:= $(basename $(notdir $(SDEF)))
APP_NAME	:= $(basename $(notdir $(APP_DIR)))
AETE		:= $(APP_DIR)/../$(SDEF_NAME)Aete.r
RSRC_DIR	:= $(APP_DIR)/Contents/Resources
NOLOC_RSRC	:= $(RSRC_DIR)/$(APP_NAME).rsrc

SDP			:= /usr/bin/sdp
REZ			:= /Developer/Tools/Rez


.PHONY		: all noloc_rsrc sdef aete clean

all			: noloc_rsrc aete

noloc_rsrc	:: $(AETE)
	$(REZ) "$<" -a -o "$(NOLOC_RSRC)" -ov -useDF

aete		: $(AETE)

$(AETE)		: $(SDEF)
	$(SDP) -f a -o "$@" "$<"

clean		: 
	rm -rf "$(AETE)"
