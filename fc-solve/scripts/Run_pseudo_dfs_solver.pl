#!/usr/bin/perl

use strict;
use warnings;

# Board 384243.

exec(
    "./pseudo_dfs_fc_solver",
    <<'EOF'
Freecells: 2H
3H 9C 5H TS 9H QD JS
TC 8H 9S AC 6S 3D 4D
5S AS 7H 9D 2C KD KC
2S 5C 7S JC 8C 7D AH
AD QC 8S 6H 7C 3S
6D JH JD 2D 4H TD
KH 4C TH 6C 3C QH
QS 2H 5D 8D 4S KS
EOF
);
