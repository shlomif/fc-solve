#!/bin/sh

export CVSROOT=":pserver:shlomif@cvs.fc-solve.berlios.de:/cvsroot/fc-solve"
export CVS_RSH=ssh

if [ ! -e t ] ; then
    mkdir t
fi
cd t
rm -fr *
cvs checkout -r LATEST_DEVEL fc-solve/source > /dev/null 2>&1 
cat fc-solve/source/ver.txt

