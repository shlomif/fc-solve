#! /bin/sh
#
# r.sh
# Copyright (C) 2024 Shlomi Fish < https://www.shlomifish.org/ >
#
# Distributed under the terms of the MIT license.
#


(
    -t fcs
    shlomif_ux
    set -e -x
    test -n "$trunk"
    fcsdir="$trunk/fc-solve"
    test -d "$fcsdir"
    cd "$fcsdir"
    rm -fr "B"
    mkdir "B"
    cd "B"
    ../scripts/Tatzer -l n2b
    gmake -j 12 install
    cd "$trunk/cpan/Games-Solitaire-Verify/benchmark"
    git clean -dfqx . || true
    export LD_LIBRARY_PATH="$FCS_PATH"
    perl sanity-test.pl
    # git clean -dfqx ./Results || true
    # rm -f ./Results/*.res || true
    perl gen-par-mak.pl
    s="`date +'%s.%N'`"
    time gmake -j 12 -f par2.mak
    e="`date +'%s.%N'`"
    perl -E 'my ($s, $e) = @ARGV; printf(qq{%f -> %f [ %f ]\n}, $s, $e, ($e - $s));' -- "$s" "$e"
)
