#!/bin/bash

# What this script does is generate a relatively short solution for 
# 34152-mid.board derived from the two-freecell #341,52 board which the 
# solver had difficulty solving.

function myfunc()
{
    ./fc-solve -to 01ABCDE '--set-pruning' r:tf --freecells-num 2 -opt -opt-to 0123456789ABCDE -p -t -sam -sel -
}

cat <<'EOF' | myfunc
Foundations: H-3 C-0 D-0 S-0
Freecells: - 7D
AC AD KC 6C KH 3S 7H
4D 4S JH 5C QH JS TH
8H 9S 7S 4H KD 4C TC 9D
QD AS 8C 8S 6D JD 2S
7C
9H 3D 8D 6H 5S
5H QC QS 2C KS 6S 5D
3C TS 2D JC TD 9C
EOF

