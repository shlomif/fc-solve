#!/bin/bash
if test "$1" = 'g'
then
    export FCS_GCC=1 CFLAGS="-Werror"
    seed=160
else
    export CC=/usr/bin/clang CXX=/usr/bin/clang++ FCS_CLANG=1 CFLAGS="-Werror"
    seed=19
fi
while true
do
    echo "Checking seed=$seed"
    export FCS_THEME_RAND="$seed"
    if (../source/Tatzer && make)
    then
        let seed++
    else
        echo "seed=$seed failed"
        exit 1
    fi
done
