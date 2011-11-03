#!/bin/bash
set -x
VER="$(cat ver.txt)"
svn copy -m "Tagging the Freecell Solver $VER release" \
    http://fc-solve.googlecode.com/svn/fc-solve/trunk/ \
    http://fc-solve.googlecode.com/svn/fc-solve/tags/releases/"$VER"
