#!/bin/sh
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
#	This script adds a few symbolic links under /usr/local, in order for Xcode/PB/gcc 
#	to be able to find B's headers and libraries.
#	
#	The script is assumed to be run from its original location in the B installation, 
#	namely b/prog.

BASE_NAME="`basename $0`"
DIR_NAME="`dirname $0`"

# Get the absolute path of the directory containing this script.  The exact way of 
# determining it depends on how we were invoked.
if (echo "$DIR_NAME" | grep '^\/')
then
	# Absolute path
	PROG_DIR="$DIR_NAME"
else
	# Relative path
	if [ "$DIR_NAME" == "." ]
	then
		# No directory was given, just the file name.
		PROG_DIR="$PWD"
	else
		# We have a relative path.
		PROG_DIR="$PWD/$DIR_NAME"
	fi
fi

# Move up the directory tree until we get the B installation directory.
B_DIR="`dirname $PROG_DIR`"
B_ROOT="`dirname $B_DIR`"

# We want to create a symbolic link called /usr/local/b, so abort if there is already 
# something (other than a symlink) with the same name.
if [ -e /usr/local/b -a ! -s /usr/local/b ]
then
	echo "$BASE_NAME:  There is already something called /usr/local/b!  Please delete or rename it." >&2
	exit 2
fi

# We want to create a directory called /usr/local/lib/b, so abort if there is already 
# something (other than a directory) with the same name.
if [ -e /usr/local/lib/b -a ! -d /usr/local/lib/b ]
then
        echo "$BASE_NAME:  There is already something called /usr/local/lib/b!  Please delete or rename it." >&2
        exit 2
fi

# Create the /usr/local directory if necessary (unlikey, but just in case...)
if [ ! -d /usr/local ]
then
	echo "Creating directory /usr/local/ ..."
	sudo mkdir /usr/local || {
			echo "$BASE_NAME:  sudo mkdir failed" >&2
			exit 1
	}
fi

# Remove any previous link to B. 
sudo rm -f /usr/local/b || {
	echo "$BASE_NAME:  sudo rm failed" >&2
	exit 1
}

# Create the symlink.
echo "Creating symlink /usr/local/b ..."
sudo ln -s "$B_ROOT" /usr/local/b || {
	echo "$BASE_NAME:  sudo ln failed" >&2
	exit 1
}

# Create the /usr/local/lib directory if necessary
if [ ! -d /usr/local/lib ]
then
	echo "Creating directory /usr/local/lib/ ..."
	sudo mkdir /usr/local/lib || {
			echo "$BASE_NAME:  sudo mkdir failed" >&2
			exit 1
	}
fi

# Remove any previous lib directory
sudo rm -rf /usr/local/lib/b || {
        echo "$BASE_NAME:  sudo rm failed" >&2
        exit 1
}

# Create the lib directory
echo "Creating directory /usr/local/lib/b ..."
sudo mkdir /usr/local/lib/b || {
        echo "$BASE_NAME:  sudo mkdir failed" >&2
        exit 1
}

# Create the symlinks inside the lib directory
echo "Creating symlink /usr/local/lib/b/libMoreFilesX.a ..."
sudo ln -s /usr/local/b/3rd/apple/MoreFilesX/libMoreFilesX.a /usr/local/lib/b/libMoreFilesX.a || {
        echo "$BASE_NAME:  sudo ln failed" >&2
        exit 1
}
echo "Creating symlink /usr/local/lib/b/libMoreIsBetterLib.a ..."
sudo ln -s /usr/local/b/3rd/apple/MoreIsBetter/MIB-Libraries/libMoreIsBetterLib.a /usr/local/lib/b/libMoreIsBetterLib.a || {
        echo "$BASE_NAME:  sudo ln failed" >&2
        exit 1
}

echo "Done!"

