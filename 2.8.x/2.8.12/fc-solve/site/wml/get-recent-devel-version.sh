#!/bin/sh

export SVNROOT="svn+ssh://svn.berlios.de/svnroot/repos/fc-solve"

if [ ! -e t ] ; then
    mkdir t
    cd t
    rm -fr *
    svn checkout "$SVNROOT"/tags/latest-devel/fc-solve/source > /dev/null 2>&1 
    mkdir fc-solve
    mv source fc-solve/
    cat fc-solve/source/ver.txt
else
    cd t/fc-solve/source
    svn update > /dev/null 2>&1
    cat ver.txt
fi

