#!/bin/sh

mypath="$1"
cd t/fc-solve/source
./prepare_package.sh
cp -f freecell-solver-`cat ver.txt`.tar.gz $mypath

