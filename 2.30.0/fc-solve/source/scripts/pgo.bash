#!/bin/bash

# This is a script for PGO - Profile Guided Optimisations in gcc.

mode="$1"
shift

pgo_flags=""

if test "$mode" = "use" ; then
    pgo_flags="-fprofile-use"
elif test "$mode" = "gen" ; then
    pgo_flags="-fprofile-generate"
else
    echo "Unknown mode '$mode'!" 1>&2
    exit -1
fi
    
make -f Makefile.gnu FREECELL_ONLY=1 \
    CFLAGS="-DFCS_FREECELL_ONLY=1 -O3 -Wall -Werror=implicit-function-declaration -march=pentium4 -fomit-frame-pointer $pgo_flags -fPIC"
