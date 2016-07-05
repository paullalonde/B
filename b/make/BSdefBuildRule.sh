#!/bin/sh
# ==========================================================================================
#   
#   Copyright (C) 2004-2005 Paul Lalonde enrg.
#   
#   This program is free software;  you can redistribute it and/or modify it under the 
#   terms of the GNU General Public License as published by the Free Software Foundation;  
#   either version 2 of the License, or (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful, but WITHOUT ANY 
#   WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
#   PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along with this 
#   program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
#   Suite 330, Boston, MA  02111-1307  USA
#   
# ==========================================================================================
#
#   This shell script is designed to be called from an Xcode build rule.  It basically 
#   just wraps the sdp tool, and returns a non-zero exit code if something goes wrong 
#   (which sdp doesn't do, it always returns zero).
#
#   The first argument is the input sdef file, and the second is the output Rez file.
#   

SDEF_FILE=$1
REZ_FILE=$2
ERR_FILE=/tmp/BSdefBuildRule.$$.txt

sdp -f a -o "$REZ_FILE" "$SDEF_FILE" >&$ERR_FILE

if [ -s $ERR_FILE ]
then
    cat $ERR_FILE
    rm $ERR_FILE
    exit 1
else
    rm $ERR_FILE
    exit 0
fi

