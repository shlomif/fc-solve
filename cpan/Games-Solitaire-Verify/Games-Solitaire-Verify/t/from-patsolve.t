#!/usr/bin/env perl

use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

use Test::Differences (qw( eq_or_diff ));

require 5.008;

use Test::More tests => 6;

use Games::Solitaire::Verify::App::CmdLine::From_Patsolve ();

my $sol_fn =
    path('t/data/sample-solutions/patsolve-338741497-win-solution.txt');

my $board_fn = path('t/data/boards/338741497.board');

{
    my $obj = Games::Solitaire::Verify::App::CmdLine::From_Patsolve->new(
        {
            argv => [ qw(-g freecell), $board_fn, $sol_fn, ],
        }
    );

    $obj->_read_initial_state();

    # TEST
    ok( 1, "_read_initial_state is successful." );

    $obj->_perform_move("2S to temp");

    # TEST
    eq_or_diff(
        scalar( $obj->_get_buffer ),
        <<"EOF",
-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C 2S
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 0

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

EOF
        "After 2S to temp move.",
    );

    $obj->_perform_move('6C to temp');
    $obj->_perform_move('9C to temp');
    $obj->_perform_move('5C to 6D');

    # TEST
    eq_or_diff(
        scalar( $obj->_get_buffer ),
        <<"EOF",
-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C 2S
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 0

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 1

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 6 to freecell 2

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 5 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

EOF
        "After 2S to temp move.",
    );

    $obj->_perform_move('KS to temp');
    $obj->_perform_move('7D to 8C');
    $obj->_perform_move('6C to 7D');
    $obj->_perform_move('2H to temp');
    $obj->_perform_move('AC out');

    # TEST
    eq_or_diff(
        scalar( $obj->_get_buffer ),
        <<"EOF",
-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C 2S
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 0

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 1

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 6 to freecell 2

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 5 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 1 to freecell 3

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 1 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from freecell 1 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S      9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 2 to freecell 1

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S AC
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 2 to the foundations

Foundations: H-0 C-A D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

EOF
        "After 2S to temp move.",
    );

    $obj->_perform_move('2C out');
    $obj->_perform_move('5H to 6C');
    $obj->_perform_move('4S to 5H');
    $obj->_perform_move('3D to 4S');
    $obj->_perform_move('2S to 3D');
    $obj->_perform_move('QS to temp');
    $obj->_perform_move('TH to JS');
    $obj->_perform_move('QC to KH');
    $obj->_perform_move('QH to KC');
    $obj->_perform_move('KS to empty pile');

    # TEST
    eq_or_diff(
        scalar( $obj->_get_buffer ),
        <<"EOF",
-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0
Freecells:
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C 2S
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 0

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C 6C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 5 to freecell 1

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C
: 8D 8H 8S 6S 6D 9C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 6 to freecell 2

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 5C
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 5 to stack 6

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D KS
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 1 to freecell 3

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C 7D
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 1 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  6C  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from freecell 1 to stack 5

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S      9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S AC 2H
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 2 to freecell 1

Foundations: H-0 C-0 D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S AC
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 2 to the foundations

Foundations: H-0 C-A D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H 2C
: 9D 3H JH JS 4S
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 1 to the foundations

Foundations: H-0 C-2 D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D 5H
: 9D 3H JH JS 4S
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 1 to stack 5

Foundations: H-0 C-2 D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D
: 9D 3H JH JS 4S
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C 5H
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 2 to stack 5

Foundations: H-0 C-2 D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC 3D
: 9D 3H JH JS
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C 5H 4S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 1 to stack 5

Foundations: H-0 C-2 D-0 S-0
Freecells:  2S  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC
: 9D 3H JH JS
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C 5H 4S 3D
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from freecell 0 to stack 5

Foundations: H-0 C-2 D-0 S-0
Freecells:      2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC
: 9D 3H JH JS
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH QS
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from stack 4 to freecell 0

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC
: 9D 3H JH JS
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH TH
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 4 to stack 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH QC
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 1 to stack 4

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
: QH
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 1 to stack 3

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KS
: QD 5S JC 3S 9H 3C 9S
:
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D KC QH
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move a card from freecell 3 to stack 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C
: QD 5S JC 3S 9H 3C 9S
: KS
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D KC QH
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

EOF
        "After KS to empty pile move.",
    );
}

{
    my $obj = Games::Solitaire::Verify::App::CmdLine::From_Patsolve->new(
        {
            argv => [ qw(-g freecell), $board_fn, $sol_fn, ],
        }
    );

    $obj->_process_main();

    # TEST
    eq_or_diff(
        scalar( $obj->_get_buffer ),
        path("./t/data/sample-solutions/from-patsolve--want.txt")->slurp_utf8,
        "After KS to empty pile move.",
    );
}
