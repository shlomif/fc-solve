#! /bin/bash
#
# benchmark--gen-multiple-layouts.bash
# Copyright (C) 2020 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#


outdir='../bench-gen-multi'
minidx=1
# maxidx=32000
maxidx=100000

shell_loop()
{
    local i="$minidx"
    while test "$i" -le "$maxidx"
    do
        ./board_gen/pi-make-microsoft-freecell-board -t "$i" > "$outdir/$i.board"
        (( ++i ))
    done
}

c_gen()
{
    ./board_gen/gen-multiple-solitaire-layouts--c --dir "$outdir" --suffix .board seq "$minidx" "$maxidx"
}

rust_gen()
{
    ~/progs/rust/tutorial-progs/git/rust-progs/multi_deal_freecell --dir "$outdir" --suffix .board seq "$minidx" "$maxidx"
}

pypy_gen()
{
    pypy3 board_gen/gen-multiple-pysol-layouts --dir "$outdir" --ms --prefix '' --suffix .board --game freecell seq "$minidx" "$maxidx"
}

python_gen()
{
    python3 board_gen/gen-multiple-pysol-layouts --dir "$outdir" --ms --prefix '' --suffix .board --game freecell seq "$minidx" "$maxidx"
}

perl_gen()
{
    perl ../scripts/perl-msdeal-generator.pl --dir "$outdir" --suffix .board seq "$minidx" "$maxidx"
}

wrapper()
{
    local func="$1"
    shift
    echo "== Benchmarking '$func' =="
    mkdir -p "$outdir"
    time "$func"
    mv "$outdir" "$outdir-$func"
    echo
}

rm -fr "$outdir-"{c_gen,perl_gen,pypy_gen,python_gen,rust_gen,shell_loop}

if false
then
    wrapper c_gen
    wrapper perl_gen
    exit
fi
# wrapper shell_loop
wrapper python_gen
wrapper pypy_gen
wrapper c_gen
wrapper rust_gen
wrapper perl_gen
