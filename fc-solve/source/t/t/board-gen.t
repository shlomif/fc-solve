#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Test::Differences;

{
    my $bakers_dozen_24_got = `python ../board_gen/make_pysol_freecell_board.py -t 24 bakers_dozen`;
    
    my $bakers_dozen_24_expected = <<"EOF";
KD 8H AC AS
KC 3D 8C 2S
QD TD 7D 9S
2H JH 6S QH
KH 3S 9D 2C
JD 3H TH 7S
7C QS JC AH
8D 4D 6H 7H
6D AD 3C 2D
KS TS 9C 5D
4H TC 6C QC
4S 5C 5S 5H
8S 9H JS 4C
EOF

    # TEST
    eq_or_diff (
        $bakers_dozen_24_got,
        $bakers_dozen_24_expected,
        "Testing for good Baker's Dozen",
    );
}
