#!/bin/bash

TARGET_DIR="Source-Devel"

mypath="$1"

( cd "$TARGET_DIR"/fc-solve/source
arc_name="freecell-solver-`cat ver.txt`.tar.gz"
if [ ! -e "$arc_name" ] ; then
    ./prepare_package.sh
fi 
cat "$arc_name"
) > "$mypath"

