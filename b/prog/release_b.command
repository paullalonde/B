#!/bin/sh
# release_b.command -- Prepare a release of B
# 
# This involves checking out the source at a given tag, changing the Doxygen 
# config file to generate graphs, generating the docs, then removing all 
# unwanted files (Subversion directories, etc) and finally tar'ing everything.
# 
# usage:  release_b release-number
#
# The release-number should be of the form X.Y.Z, eg 0.3.2.


# usage: ReplaceTokenInFile() token replacement-text 
ReplaceTokenInFiles() {
    # We match either {{$1}} or %7B%7B$1%7D%7D, which is the URI-encoded equivalent.
    echo "Replacing {{$1}} with '$2' ..."
    grep -Zlr "{{$1}}"         . | xargs -0 -n1 inplace_token_replace "{{$1}}"         "$2"
    grep -Zlr "%7B%7B$1%7D%7D" . | xargs -0 -n1 inplace_token_replace "%7B%7B$1%7D%7D" "$2"
}

# important vars for later
CO_DIR=~/src/b_rel
REL_NAME="$1"
REL_DIR="b-$REL_NAME"
TAG_NAME="B_RELEASE_`echo $REL_NAME | tr . _`"
REL_DATE="`date '+%B %e %Y'`"
REL_TARBALL="$REL_DIR.tar.gz"
IDISK_ROOT="/Volumes/paullalonde"
DISTRO_ROOT="$IDISK_ROOT/Sites/b"

# Sanity checks
if [ ! -d "$IDISK_ROOT" ]
then
    echo "My iDisk isn't mounted !!"
    exit 1
fi

# To make sure we start with a clean slate, remove output directory, then 
# re-create it.
rm -rf $CO_DIR
mkdir -p $CO_DIR
cd $CO_DIR

# Copy the template files from the iDisk so we can manipulate them locally.
cp -R "$DISTRO_ROOT/templates/" templates/

# Check out the source with the given tag.
svn export svn://127.0.0.1/B/tags/$TAG_NAME all

# Replace tokens in all source tree files, and all template files.
ReplaceTokenInFiles B_RELEASE_VERSION "$REL_NAME"
ReplaceTokenInFiles B_RELEASE_DATE    "$REL_DATE"
ReplaceTokenInFiles B_RELEASE_TARBALL "$REL_TARBALL"

# touch the .a files, 'cause if we don't ld will complain
ranlib all/3rd/apple/MoreFilesX/libMoreFilesX.a
ranlib all/3rd/apple/MoreIsBetter/MIB-Libraries/libMoreIsBetterLib.a

# massage the permissions on selected files.
chmod a+x all/b/prog/*.command

# Generate the documentation.
cd all/b/doc
sed '/^HAVE_DOT[ ]*=.*/{ s/^HAVE_DOT[ ]*=.*/HAVE_DOT = YES/; }' Doxyfile >Doxyfile.rel
doxygen Doxyfile.rel
rm Doxyfile.rel
cd -

# Remove all files & folders that we don't want in the distribution.
find all -type d -name CVS            -print0 | xargs -0 -n1 rm -rf
find all -type d -name .svn           -print0 | xargs -0 -n1 rm -rf
find all -type f -name .DS_Store      -print0 | xargs -0 -n1 rm 
find all -type f -name .FBCIndex      -print0 | xargs -0 -n1 rm
find all -type d -name .FBCLockFolder -print0 | xargs -0 -n1 rm -rf

# Tar and gzip the whole tree.
mv all "$REL_DIR"
tar cvfz "$REL_TARBALL" "$REL_DIR"

# Copy files to the iDisk
rm -rf "$DISTRO_ROOT/test/"
mkdir -p "$DISTRO_ROOT/test/"
echo "Copying tarball ..."
cp "$REL_TARBALL" "$DISTRO_ROOT/test/"
echo "Copying change log ..."
cp $REL_DIR/b/Changes.html "$DISTRO_ROOT/test/"
echo "Copying site index ..."
cp templates/index.html "$DISTRO_ROOT/test/"

# We're done
echo "Done!"
