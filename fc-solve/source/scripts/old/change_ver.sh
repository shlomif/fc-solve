#!/bin/bash

VER="`cat ver.txt`"
find . -type f -name '*.pre.*' -o -name '*.pre' | 
    grep -v '~$' |    # Filter out the temporary files
    (while read T ; do
        echo "Changing $T"
        NOPRE_FN=$(echo $T | sed 's/\.pre//')
        cat $T | sed 's/\[{#FCS_VERSION#}\]/'"$VER"'/g' > $NOPRE_FN
    done)
    
