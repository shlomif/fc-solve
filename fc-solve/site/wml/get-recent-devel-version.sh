#!/bin/sh

export SVNROOT="svn+ssh://svn.berlios.de/svnroot/repos/fc-solve"
TARGET_DIR="Source-Devel"
if [ ! -e "$TARGET_DIR" ] ; then
    (mkdir "$TARGET_DIR"
    cd "$TARGET_DIR"
    rm -fr *
    mkdir fc-solve
    cd fc-solve
    svn checkout "$SVNROOT"/tags/latest-devel/fc-solve/source > /dev/null 2>&1 
    )
else
    svn update "$TARGET_DIR"/fc-solve/source > /dev/null 2>&1
fi
cat "$TARGET_DIR"/fc-solve/source/ver.txt
