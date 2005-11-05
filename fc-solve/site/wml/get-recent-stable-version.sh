#!/bin/bash
(if [ -e Source ] ; then
    svn update Source 
else
    svn checkout 'svn://svn.berlios.de/fc-solve/branches/Version-2_8_x/fc-solve/source/' Source
fi) > /dev/null 2>&1
(cd Source && cat ver.txt)
