#!/bin/bash

# This is a script for PGO - Profile Guided Optimisations in gcc.

compiler="$1"
shift
mode="$1"
shift

pgo_flags=""
make_vars=()

if test "$mode" = "total" ; then
    make -f Makefile.gnu clean && \
    bash scripts/pgo.bash "$compiler" "gen" &&\
    rm -f *.gcda && \
    sudo_renice ./freecell-solver-range-parallel-solve 1 32000 4000 -l eo && \
    make -f Makefile.gnu clean && \
    bash scripts/pgo.bash "$compiler" "use"
    exit 0
elif test "$mode" = "use" ; then
    if test "$compiler" = "gcc" ; then
        pgo_flags="-fprofile-use"
    elif test "$compiler" = "icc" ; then
        pgo_flags="-prof-use"
    else
        echo "Unknown compiler '$compiler'!" 1>&2
        exit -1
    fi
    make_vars=(OPT_AND_DEBUG=0 DEBUG=0)
elif test "$mode" = "gen" ; then
    if test "$compiler" = "gcc" ; then
        pgo_flags="-fprofile-generate"
    elif test "$compiler" = "icc" ; then
        pgo_flags="-prof-gen"
    else
        echo "Unknown compiler '$compiler'!" 1>&2
        exit -1
    fi
else
    echo "Unknown mode '$mode'!" 1>&2
    exit -1
fi
    
make -f Makefile.gnu FREECELL_ONLY=1 \
    EXTRA_CFLAGS="$pgo_flags" \
    COMPILER="$compiler" \
    $make_vars
