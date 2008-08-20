#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 5;
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

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py -t 10800 bakers_dozen`;
    
    my $expected = <<"EOF";
KC 3S AC AD
JD 9H 6H JS
KS 8D 8S QH
3D 6C TS 4C
7H 4D 2C 7D
KH TD 4S 3C
2S 3H JC 5C
AS 5S 6D 8C
7C 4H TC 9S
8H 9D QC 9C
7S 5D 5H 2H
2D QD QS TH
KD 6S AH JH
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Testing for good Baker's Dozen",
    );
}

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py 24 freecell`;
    
    my $expected = <<"EOF";
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD 10H 10C 10D 8D
7H JS KH 10S KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Testing for Freecell",
    );
}

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py 24 bakers_game`;
    
    my $expected = <<"EOF";
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD 10H 10C 10D 8D
7H JS KH 10S KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Testing for Bakers' Game",
    );
}

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py 1977 forecell`;
    
    my $expected = <<"EOF";
FC: 2D 8H 5S 6H
QH JC 7D 8S 4D 2C
JD JH KS 9C QD 2S
AD 10S 9D 3C 9S AC
10C 7H 4S KD 2H 3S
5C 6C 6D 4C 10D 8C
JS QC 8D 7C 7S 6S
3D AH 3H 10H 4H AS
KC QS 5D 5H 9H KH
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Testing for Forecell",
    );
}
