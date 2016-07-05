#!/bin/sh
# ==========================================================================================
#       
#       Copyright (C) 2004-2005 Paul Lalonde enrg.
#       
#       This program is free software;  you can redistribute it and/or modify it under the 
#       terms of the GNU General Public License as published by the Free Software Foundation;  
#       either version 2 of the License, or (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful, but WITHOUT ANY 
#       WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
#       PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public License along with this 
#       program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
#       Suite 330, Boston, MA  02111-1307  USA
#       
# ==========================================================================================
#
#   Simple shell script that builds all sample projects.
#   
#   This script should be run from the main B directory.

if [ -d /usr/local/b/b ]
then
    cd /usr/local/b/b
else
    echo "Can't find installation location of B !!"
    exit 1
fi

asfile="/tmp/build_all_$$.applescript"
style="Development"
build="build"
clean=
xc=1
cw=0

for arg
do
    case "$arg" in
    --help)
        echo "Builds the Xcode and CodeWarrior sample projects that come with B."
        echo "usage:  `basename $0` [options]"
        echo "options:"
        echo "    --help         This message."
        echo "    --clean        Removes build products and intermediate files."
        echo "    --cleanbuild   Clean before building."
        echo "    --development  Build Development configuration."
        echo "    --deployment   Build Deployment configuration (default)."
        echo "    --cw           Build CodeWarrior projects."
        echo "    --no-cw        Do not build CodeWarrior projects (default)."
        echo "    --xc           Build Xcode projects (default)."
        echo "    --no-xc        Do not build Xcode projects."
        exit
        ;;
    --clean)
        clean="clean"
        build=
        ;;
    --cleanbuild)
        clean="clean"
        build="build"
        ;;
    --development)
        style="Development"
        ;;
    --deployment)
        style="Deployment"
        ;;
    --cw)
        cw=1
        ;;
    --no-cw)
        cw=0
        ;;
    --xc)
        xc=1
        ;;
    --no-xc)
        xc=0
        ;;
    esac
done

# Map Xcode build style to CodeWarrior target.

if [ "$style" = "Development" ]
then
    targ="Debug"
else
    targ="Release"
fi

for sample in DragPeekerX Framework PLister Tarabiscoter
do
    echo
    echo Building Sample Project $sample ...
    echo

    if [ $xc != 0 ]
    then
        # Build XCode project
            
        cd examples/$sample
        xcodebuild -project *.xcodeproj -target "$sample" -configuration "$style" $clean $build
        cd -
    fi
    
    if [ $cw != 0 ]
    then
        # Build an AppleScript file that will open, build & close the CodeWarrior project.

        rm -f "$asfile"
    
        echo >>"$asfile"        "tell application \"Finder\"" 
        echo >>"$asfile"        "    set bpath to (startup disk as string) & \"usr:local:b:\""
        echo >>"$asfile"        "end tell" 
        echo >>"$asfile"        "set spath to bpath & \"b:examples:$sample:$sample.mcp\"" 
        echo >>"$asfile"        "tell application \"CodeWarrior IDE\"" 
        echo >>"$asfile"        "    open file spath" 
        echo >>"$asfile"        "    Set Current Target \"$targ\"" 
        if [ "$clean" != "" ]
        then
            echo >>"$asfile"    "    Remove Binaries" 
        fi
        if [ "$build" != "" ]
        then
            echo >>"$asfile"    "end tell" 
            echo >>"$asfile"    "with timeout of 20 * 60 seconds" 
            echo >>"$asfile"    "    tell application \"CodeWarrior IDE\"" 
            echo >>"$asfile"    "        Make Project" 
            echo >>"$asfile"    "    end tell" 
            echo >>"$asfile"    "end timeout" 
            echo >>"$asfile"    "tell application \"CodeWarrior IDE\"" 
        fi
        echo >>"$asfile"        "    Close Project" 
        echo >>"$asfile"        "end tell" 
        
        osascript "$asfile"
    fi
done

# Clean up

rm -f "$asfile"
