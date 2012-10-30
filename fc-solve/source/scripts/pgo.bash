#!/bin/bash

# This is a script for PGO - Profile Guided Optimisations in gcc.

compiler="$1"
shift
mode="$1"
shift

pgo_flags=""
make_vars=()

src="$(perl -e 'use File::Basename qw(dirname); use File::Spec; print File::Spec->rel2abs(dirname(shift(@ARGV))."/..");' "$0")"
# echo "src_dir=$src"

# theme="-l te"
# theme="--read-from-file 4,$(pwd)/Presets/testing-presets/all-star-4.sh"
theme="-l as"

run_make()
{
    make -r -j4 -f "$src"/Makefile.gnu SRC_DIR="$src" "$@"
}

m_clean()
{
    run_make clean
}

run_self()
{
    local cmd="$1"
    shift

    bash "$src"/scripts/pgo.bash "$compiler" "$cmd"
}

run_timing()
{
    if [ -x ~/bin/sudo_time_fcs ] ; then
        sudo ~/bin/sudo_time_fcs "$@"
    else
        sudo_renice ./freecell-solver-range-parallel-solve "$@"
    fi
}

if test "$mode" = "total" ; then
    m_clean && \
    run_self "gen" && \
    rm -f *.gcda && \
    run_timing 1 32000 4000 $theme && \
    m_clean && \
    run_self "use"
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

run_make FREECELL_ONLY=1 \
    EXTRA_CFLAGS="$pgo_flags" \
    COMPILER="$compiler" \
    $make_vars
