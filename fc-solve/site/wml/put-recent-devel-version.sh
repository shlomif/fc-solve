#!/bin/sh

mypath="$1"
cd t/fc-solve/source
arc_name="freecell-solver-`cat ver.txt`.tar.gz"
if [ ! -e $arc_name ] ; then
    ./prepare_package.sh
fi
cp -f $arc_name $mypath

