#!/bin/sh

OPT="$1"

automake
autoconf
(cd board_gen && automake && autoconf)
./configure && make dist

if [ "$OPT" == "--rpm" ] ; then
    rpm -tb freecell-solver-`cat ver.txt`.tar.gz
fi
    
