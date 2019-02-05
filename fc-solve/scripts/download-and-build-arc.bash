#! /usr/bin/env bash
#
# download-and-build-arc.bash
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#


set -e -x

(
    base='freecell-solver'
    ver='5.0.0'
    basever="$base-$ver"
    export TMPDIR="${TMPDIR:-/tmp}"
    arc="$basever.tar.xz"
    cd "$TMPDIR"
    tarc="$TMPDIR/$arc"
    if ! test -e "$arc"
    then
        wget -O "$arc" "https://fc-solve.shlomifish.org/downloads/fc-solve/$arc"
    fi
    rm -fr "$basever" || true
    tar -xvf "$arc"
    build="fcs-build"
    rm -fr "$build" || true
    mkdir -p "$build"
    pushd "$build"
    cmake -DFCS_WITH_TEST_SUITE=OFF "$TMPDIR/$basever"
    make VERBOSE=1
    popd
) 2>&1 | tee -a ~/fc-solve-build-log.txt
