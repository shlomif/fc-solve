#!/bin/sh
cat fcs_user.h fcs_cl.h | 
    grep extern | 
    grep -v '"C"' | 
    sed 's/^[^f]*//' | 
    sed 's/(.*$//' | 
    (echo "LIBRARY freecell-solver"
     echo "EXPORTS"
     A=1
     while read T ; do 
        echo "    $T @$A" ; 
        A=`expr $A + 1` ; 
     done
    )

