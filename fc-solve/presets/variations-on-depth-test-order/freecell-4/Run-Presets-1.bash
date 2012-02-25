#!/bin/bash

max_board="500"
max_iters="10000"

function _preset()
{
    local name="$1"
    shift

    d="data/$name++--$max_board--$max_iters.dump"
    if test -z "$PARSE" ; then
        echo "Preset $name == $@"
    fi
    if ! test -e "$d" ; then
        freecell-solver-range-parallel-solve 1 "$max_board" 1 "$@" --max-iters "$max_iters" \
        > "$d"
        # | tee "$d"
    fi
    unsolved_boards_count="$(grep -cP '^Unsolved' "$d")"
    iters_count="$(tail -1 "$d" | perl -ne 'print $1 if m{total_num_iters=(\d+)}')"
    wallclock_time="$(perl -e 'use IO::All; sub get_time { my ($s) = @_; $s =~ m/at (\d+\.\d+)/; return $1 } my @lines= io->file(shift(@ARGV))->getlines(); print get_time($lines[-1])-get_time($lines[0]);' "$d")"
    if test -z "$PARSE" ; then
        echo "   Unsolved = $unsolved_boards_count  ; Iters = $iters_count ; Wallclock Time = $wallclock_time "
    else
        echo -e "$name\t$wallclock_time\t$unsolved_boards_count\t$iters_count"
    fi
}

_preset "default"

_preset "eightl" -to 01847296 -dto 12,014765 -dto 20,0167854 -dto 40,01236874

_preset "10step" \
    -to 01234567 \
    -dto 10,0187465 \
    -dto 20,0134587

_preset "5step" \
    -to 01234567 \
    -dto 5,0187465 \
    -dto 10,0134587

_preset "15step" \
    -to 01234567 \
    -dto 15,0187465 \
    -dto 30,0134587 \

_preset "11step" \
    -to 01234567 \
    -dto 11,0187465 \
    -dto 22,0134587

_preset "12step" \
    -to 01234567 \
    -dto 12,0187465 \
    -dto 24,0134587

_preset "13step" \
    -to 01234567 \
    -dto 13,0187465 \
    -dto 26,0134587

_preset "14step" \
    -to 01234567 \
    -dto 14,0187465 \
    -dto 28,0134587

_preset "14_24step" -to 01234567  -dto 14,0187465  -dto 24,0134587

_preset "scan23" --method soft-dfs -to 0123456789 -dto 19,0126394875

_preset "01234567" -to 01234567

_preset "0123456" -to 0123456

_preset "flamingo" -to 01234567 -dto 30,0123876594
for threshold in 40 50 55 65 70 75 80 85 90 95 100 105 110 115 120 130 140 150 ; do
    _preset "flamingo${threshold}" -to 01234567 -dto "${threshold},0123876594"
done    

_preset "qlad"  -to 01234567  -dto 13,0187465  -dto 26,0134587 -dto 35,819435
_preset "encad"  -to 01234567  -dto 13,0187465  -dto 26,0134587 -dto 35,0123456789

