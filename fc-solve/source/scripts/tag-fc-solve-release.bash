#!/bin/bash
set -x
VER="$(cat ver.txt)"
svn copy -m "Tagging the Freecell Solver $VER release" \
    https://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk \
    https://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/tags/releases/"$VER"
