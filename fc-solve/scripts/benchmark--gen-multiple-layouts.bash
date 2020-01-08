#! /bin/bash
#
# benchmark--gen-multiple-layouts.bash
# Copyright (C) 2020 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#


outdir='../foo'
minidx=1
maxidx=32000

shell_loop()
{
    local i="$minidx"
    while test "$i" -le "$maxidx"
    do
        ./board_gen/pi-make-microsoft-freecell-board -t "$i" > "$outdir/$i.board"
        let ++i
    done
}

wrapper()
{
    local fn="$1"
    shift
    mkdir -p "$outdir"
    time "$fn"
    mv "$outdir" "$outdir-$fn"
}

wrapper shell_loop
