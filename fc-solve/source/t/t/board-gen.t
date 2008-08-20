#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 10;
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

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py 100 seahaven_towers`;
    
    my $expected = <<"EOF";
FC: - 9S 5S
AD 5C 9C 9H 2C
JS 9D 6H 7H 2H
4S 4H AC 3C KH
QC JD AS KS 8C
7C KD 6S JC 7D
6D AH 5H 3S 8D
JH QS 6C 10C QD
10D 10H 3D 4C 2D
QH 7S 4D 5D 8S
10S 8H 2S 3H KC
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Seahaven 100 Output",
    );
}

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py 250 simple_simon`;
    
    my $expected = <<"EOF";
6S 6C 10C JC 9H 8D 3C 5S
JH 2S KH 10S 7D 4D 8S 5D
9D 8H 4S KC 7C 4C QS 5C
4H 10D 2C 3S 8C 10H 7S
3H 6D QH 7H 9S AD
2D 2H 6H JS QD
AC 5H QC AS
KD JD KS
9C 3D
AH
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Simple Simon 250",
    );
}


{
    my $got = `python ../board_gen/make_pysol_freecell_board.py -t 4200 fan`;
    
    my $expected = <<"EOF";
7H 9H 3S
4C 8H 8C
TH TS 3H
9D QD 5H
5S AS JH
QC 3D 6D
5D KD 9C
TC 4D KC
JD 8S 9S
7S JC 2S
4S KS KH
2H 5C AH
2C QS 3C
4H 2D 6S
TD 6H 7D
7C AC JS
6C 8D QH
AD
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Fan 4200",
    );
}

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py -t 314 eight_off`;

    my $expected = <<"EOF";
FC: TD - 8D - 5D - 6S -
7C QD 8H AS AD 2S
5C 5H 2C QH 8C TS
3H JD 4C 3S AH 9C
5S 6C 4S 8S 3C KD
QS JS 9H QC 6H 2H
TH TC 7H 9D 3D 4H
4D JC KC 7S JH 9S
7D 6D 2D KH KS AC
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "Eight Off 314",
    );
}

{
    my $got = `python ../board_gen/make_pysol_freecell_board.py -t 103 der_katzenschwantz`;

    my $expected = <<"EOF";
JC JS AH 2H 3H 2S 2D 9D 9C 4C QD TH 6H 6S 9C 3C 5H JD 4H JH 9S 7D 4D 4C JH QH 9D 2D
KC
KH 6D 9S 2C 3H
KS QS
KD TS AS 5S TD 5C 5H 3S 7C TH 6S 7H 5D 6C 4D 5S AC QS 8S 4S AH 2H JD TC 8H 3D 8S 5D 9H 6H AS 2S 8D QC AD AC 8D
KD 6D
KS 8C 7S QC QH 5C JS 7C 2C TS 7S 4H 9H 7D 4S 6C QD AD TD 8C 3C
KC 7H TC 3S 3D 8H JC
KH
EOF

    # TEST
    eq_or_diff (
        $got,
        $expected,
        "der_katzenschwantz 103",
    );
}
