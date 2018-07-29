#!/bin/bash
#
# fuzz-build.bash
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#

if test "$1" = 'g'
then
    export FCS_GCC=1
    # seed=160
    seed=17
else
    export CC=/usr/bin/clang CXX=/usr/bin/clang++ FCS_CLANG=1
    seed=19
fi
export CFLAGS="-Werror"
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
