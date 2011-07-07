#!/bin/bash

max_board="500"
max_iters="10000"

function _preset()
{
    local name="$1"
    shift

    d="data/$name++--$max_board--$max_iters.dump"
    echo "Preset $name == $@"
    if ! test -e "$d" ; then
        freecell-solver-range-parallel-solve 1 "$max_board" 1 "$@" --max-iters "$max_iters" \
        | tee "$d"
    fi
    echo "   Unsolved = $(grep -cP '^Unsolved' "$d") ; Iters = $(tail -1 "$d" | perl -ne 'print $1 if m{total_num_iters=(\d+)}')"
}

_preset "default"

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
_preset "flamingo100" -to 01234567 -dto 100,0123876594

_preset "qlad"  -to 01234567  -dto 13,0187465  -dto 26,0134587 -dto 35,819435

