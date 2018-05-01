#!/usr/bin/perl

use strict;
use warnings;

use Test::Differences (qw( eq_or_diff ));

require 5.008;

use File::Spec;

use autodie;

use Test::More tests => 6;

use Games::Solitaire::Verify::App::CmdLine::From_Patsolve;

my $sol_fn = File::Spec->catfile( File::Spec->curdir(),
    qw(t data sample-solutions patsolve-338741497-win-solution.txt) );

my $board_fn = File::Spec->catfile( File::Spec->curdir(),
    qw(t data boards 338741497.board) );

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $num_classesontents = <$in>;

    close($in);

    return $num_classesontents;
}

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

Move 1 cards from stack 3 to stack 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C
: QD 5S JC 3S 9H 3C 9S
: KS QH
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 4D


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C
: QD 5S JC 3S 9H 3C 9S
: KS QH
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D KC
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H


====================

Move a card from stack 3 to freecell 3

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KC
: QD 5S JC 3S 9H 3C 9S
: KS QH
: 9D 3H JH JS TH
: AS 5D 4C TC TD 2D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H


====================

Move 1 cards from stack 0 to stack 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KC
: QD 5S JC 3S 9H 3C
: KS QH
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD 2D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H


====================

Move 1 cards from stack 0 to stack 7

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  2H  9C  KC
: QD 5S JC 3S 9H
: KS QH
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD 2D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C


====================

Move a card from freecell 1 to stack 7

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS      9C  KC
: QD 5S JC 3S 9H
: KS QH
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD 2D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 0 to freecell 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  9H  9C  KC
: QD 5S JC 3S
: KS QH
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD 2D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 0 to stack 6

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  9H  9C  KC
: QD 5S JC
: KS QH
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD 2D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 3 to stack 6

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  9H  9C  KC
: QD 5S JC
: KS QH
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 0 to stack 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  9H  9C  KC
: QD 5S
: KS QH JC
: 9D 3H JH JS TH 9S
: AS 5D 4C TC TD
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 3 to stack 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  9H  9C  KC
: QD 5S
: KS QH JC TD
: 9D 3H JH JS TH 9S
: AS 5D 4C TC
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 1 to stack 3

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS      9C  KC
: QD 5S
: KS QH JC TD
: 9D 3H JH JS TH 9S
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 2 to stack 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS          KC
: QD 5S
: KS QH JC TD 9C
: 9D 3H JH JS TH 9S
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 0 to freecell 1

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S      KC
: QD
: KS QH JC TD 9C
: 9D 3H JH JS TH 9S
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 0 to freecell 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  QD  KC
:
: KS QH JC TD 9C
: 9D 3H JH JS TH 9S
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 3 to stack 0

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  QD
: KC
: KS QH JC TD 9C
: 9D 3H JH JS TH 9S
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 2 to stack 0

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S
: KC QD
: KS QH JC TD 9C
: 9D 3H JH JS TH 9S
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 2 to freecell 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  9S
: KC QD
: KS QH JC TD 9C
: 9D 3H JH JS TH
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 2 to freecell 3

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  9S  TH
: KC QD
: KS QH JC TD 9C
: 9D 3H JH JS
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 2 to stack 0

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  9S  TH
: KC QD JS
: KS QH JC TD 9C
: 9D 3H JH
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 3 to stack 0

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  9S
: KC QD JS TH
: KS QH JC TD 9C
: 9D 3H JH
: AS 5D 4C TC 9H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 3 to freecell 3

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  9S  9H
: KC QD JS TH
: KS QH JC TD 9C
: 9D 3H JH
: AS 5D 4C TC
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 3 to stack 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  9S  9H
: KC QD JS TH
: KS QH JC TD 9C
: 9D 3H JH TC
: AS 5D 4C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 2 to stack 0

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S      9H
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC
: AS 5D 4C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 3 to freecell 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  4C  9H
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC
: AS 5D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 3 to stack 2

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  4C
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: AS 5D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 3 to freecell 3

Foundations: H-0 C-2 D-0 S-0
Freecells:  QS  5S  4C  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: AS
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 3 to the foundations

Foundations: H-0 C-2 D-0 S-A
Freecells:  QS  5S  4C  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D 2S
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-2 D-0 S-2
Freecells:  QS  5S  4C  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 2 to stack 3

Foundations: H-0 C-2 D-0 S-2
Freecells:  QS  5S      5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D
: 8D 8H 8S 6S 6D 5C 4D 3S 2D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 6 to freecell 2

Foundations: H-0 C-2 D-0 S-2
Freecells:  QS  5S  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D
: 8D 8H 8S 6S 6D 5C 4D 3S
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 6 to the foundations

Foundations: H-0 C-2 D-0 S-3
Freecells:  QS  5S  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S 3D
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 5 to stack 3

Foundations: H-0 C-2 D-0 S-3
Freecells:  QS  5S  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C 3D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4S
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-2 D-0 S-4
Freecells:  QS  5S  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C 3D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 1 to the foundations

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS      2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C 3D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 3 to freecell 1

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  3D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 4C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 3 to stack 5

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  3D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 1 to stack 5

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS      2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D 5C 4D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 6 to freecell 1

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  4D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D 5C
: AD 7H 7S 7C 4H 3C 2H


====================

Move 1 cards from stack 6 to stack 3

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  4D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from freecell 1 to stack 3

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS      2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C 4D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H 3C 2H


====================

Move a card from stack 7 to freecell 1

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  2H  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C 4D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H 3C


====================

Move 1 cards from stack 7 to stack 3

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  2H  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C 4D 3C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H


====================

Move a card from freecell 1 to stack 3

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS      2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S 6D
: AD 7H 7S 7C 4H


====================

Move a card from stack 6 to freecell 1

Foundations: H-0 C-2 D-0 S-5
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S 6S
: AD 7H 7S 7C 4H


====================

Move a card from stack 6 to the foundations

Foundations: H-0 C-2 D-0 S-6
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H 8S
: AD 7H 7S 7C 4H


====================

Move 1 cards from stack 6 to stack 2

Foundations: H-0 C-2 D-0 S-6
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S
: KS QH JC TD 9C
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D 8H
: AD 7H 7S 7C 4H


====================

Move 1 cards from stack 6 to stack 0

Foundations: H-0 C-2 D-0 S-6
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 8D
: AD 7H 7S 7C 4H


====================

Move 1 cards from stack 6 to stack 1

Foundations: H-0 C-2 D-0 S-6
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
:
: AD 7H 7S 7C 4H


====================

Move 1 cards from stack 7 to stack 6

Foundations: H-0 C-2 D-0 S-6
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 4H
: AD 7H 7S 7C


====================

Move 1 cards from stack 7 to stack 1

Foundations: H-0 C-2 D-0 S-6
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 4H
: AD 7H 7S


====================

Move a card from stack 7 to the foundations

Foundations: H-0 C-2 D-0 S-7
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 4H
: AD 7H


====================

Move 1 cards from stack 7 to stack 2

Foundations: H-0 C-2 D-0 S-7
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S 7H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 4H
: AD


====================

Move a card from stack 7 to the foundations

Foundations: H-0 C-2 D-A S-7
Freecells:  QS  6D  2D  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S 7H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 4H
:


====================

Move a card from freecell 2 to the foundations

Foundations: H-0 C-2 D-2 S-7
Freecells:  QS  6D      5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S 7H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C 3D
: 4H
:


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-2 D-3 S-7
Freecells:  QS  6D      5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S 7H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 4H
:


====================

Move 1 cards from stack 2 to stack 7

Foundations: H-0 C-2 D-3 S-7
Freecells:  QS  6D      5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H 8S
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 4H
: 7H


====================

Move a card from stack 2 to the foundations

Foundations: H-0 C-2 D-3 S-8
Freecells:  QS  6D      5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
: 5C 4D 3C 2H
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 4H
: 7H


====================

Move a card from stack 3 to freecell 2

Foundations: H-0 C-2 D-3 S-8
Freecells:  QS  6D  2H  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
: 5C 4D 3C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 4H
: 7H


====================

Move a card from stack 3 to the foundations

Foundations: H-0 C-3 D-3 S-8
Freecells:  QS  6D  2H  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
: 5C 4D
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 4H
: 7H


====================

Move a card from stack 3 to the foundations

Foundations: H-0 C-3 D-4 S-8
Freecells:  QS  6D  2H  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
: 5C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H 4C
: 4H
: 7H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-4 D-4 S-8
Freecells:  QS  6D  2H  5D
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
: 5C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 4H
: 7H


====================

Move a card from freecell 3 to the foundations

Foundations: H-0 C-4 D-5 S-8
Freecells:  QS  6D  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
: 5C
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 4H
: 7H


====================

Move a card from stack 3 to the foundations

Foundations: H-0 C-5 D-5 S-8
Freecells:  QS  6D  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 4H
: 7H


====================

Move a card from freecell 1 to the foundations

Foundations: H-0 C-5 D-6 S-8
Freecells:  QS      2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C 5H
: 4H
: 7H


====================

Move a card from stack 5 to freecell 1

Foundations: H-0 C-5 D-6 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D 6C
: 4H
: 7H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-6 D-6 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C 7D
: 4H
: 7H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-6 D-7 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D 7C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C
: 4H
: 7H


====================

Move a card from stack 1 to the foundations

Foundations: H-0 C-7 D-7 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C 8D
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C
: 4H
: 7H


====================

Move a card from stack 1 to the foundations

Foundations: H-0 C-7 D-8 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS 8C
: 4H
: 7H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-8 D-8 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD 9C
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS
: 4H
: 7H


====================

Move a card from stack 1 to the foundations

Foundations: H-0 C-9 D-8 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD
: 9D 3H JH TC 9H
:
: AH 6H KD KH QC
: JD TS
: 4H
: 7H


====================

Move 1 cards from stack 2 to stack 5

Foundations: H-0 C-9 D-8 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD
: 9D 3H JH TC
:
: AH 6H KD KH QC
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 2 to the foundations

Foundations: H-0 C-T D-8 S-8
Freecells:  QS  5H  2H
: KC QD JS TH 9S 8H
: KS QH JC TD
: 9D 3H JH
:
: AH 6H KD KH QC
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 1 to freecell 3

Foundations: H-0 C-T D-8 S-8
Freecells:  QS  5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH JC
: 9D 3H JH
:
: AH 6H KD KH QC
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 1 to the foundations

Foundations: H-0 C-J D-8 S-8
Freecells:  QS  5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
: 9D 3H JH
:
: AH 6H KD KH QC
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 4 to the foundations

Foundations: H-0 C-Q D-8 S-8
Freecells:  QS  5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
: 9D 3H JH
:
: AH 6H KD KH
: JD TS 9H
: 4H
: 7H


====================

Move 1 cards from stack 4 to stack 3

Foundations: H-0 C-Q D-8 S-8
Freecells:  QS  5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
: 9D 3H JH
: KH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move a card from freecell 0 to stack 3

Foundations: H-0 C-Q D-8 S-8
Freecells:      5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
: 9D 3H JH
: KH QS
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move 1 cards from stack 2 to stack 3

Foundations: H-0 C-Q D-8 S-8
Freecells:      5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
: 9D 3H
: KH QS JH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 2 to freecell 0

Foundations: H-0 C-Q D-8 S-8
Freecells:  3H  5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
: 9D
: KH QS JH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 2 to the foundations

Foundations: H-0 C-Q D-9 S-8
Freecells:  3H  5H  2H  TD
: KC QD JS TH 9S 8H
: KS QH
:
: KH QS JH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move a card from freecell 3 to the foundations

Foundations: H-0 C-Q D-T S-8
Freecells:  3H  5H  2H
: KC QD JS TH 9S 8H
: KS QH
:
: KH QS JH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move 1 cards from stack 0 to stack 2

Foundations: H-0 C-Q D-T S-8
Freecells:  3H  5H  2H
: KC QD JS TH 9S
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 0 to the foundations

Foundations: H-0 C-Q D-T S-9
Freecells:  3H  5H  2H
: KC QD JS TH
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: JD TS 9H
: 4H
: 7H


====================

Move a card from stack 5 to freecell 3

Foundations: H-0 C-Q D-T S-9
Freecells:  3H  5H  2H  9H
: KC QD JS TH
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: JD TS
: 4H
: 7H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-Q D-T S-T
Freecells:  3H  5H  2H  9H
: KC QD JS TH
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: JD
: 4H
: 7H


====================

Move a card from stack 5 to the foundations

Foundations: H-0 C-Q D-J S-T
Freecells:  3H  5H  2H  9H
: KC QD JS TH
: KS QH
: 8H
: KH QS JH
: AH 6H KD
:
: 4H
: 7H


====================

Move 1 cards from stack 0 to stack 5

Foundations: H-0 C-Q D-J S-T
Freecells:  3H  5H  2H  9H
: KC QD JS
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: TH
: 4H
: 7H


====================

Move a card from stack 0 to the foundations

Foundations: H-0 C-Q D-J S-J
Freecells:  3H  5H  2H  9H
: KC QD
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: TH
: 4H
: 7H


====================

Move a card from stack 0 to the foundations

Foundations: H-0 C-Q D-Q S-J
Freecells:  3H  5H  2H  9H
: KC
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: TH
: 4H
: 7H


====================

Move a card from stack 0 to the foundations

Foundations: H-0 C-K D-Q S-J
Freecells:  3H  5H  2H  9H
:
: KS QH
: 8H
: KH QS JH
: AH 6H KD
: TH
: 4H
: 7H


====================

Move a card from stack 4 to the foundations

Foundations: H-0 C-K D-K S-J
Freecells:  3H  5H  2H  9H
:
: KS QH
: 8H
: KH QS JH
: AH 6H
: TH
: 4H
: 7H


====================

Move 1 cards from stack 4 to stack 0

Foundations: H-0 C-K D-K S-J
Freecells:  3H  5H  2H  9H
: 6H
: KS QH
: 8H
: KH QS JH
: AH
: TH
: 4H
: 7H


====================

Move a card from stack 4 to the foundations

Foundations: H-A C-K D-K S-J
Freecells:  3H  5H  2H  9H
: 6H
: KS QH
: 8H
: KH QS JH
:
: TH
: 4H
: 7H


====================

Move a card from freecell 2 to the foundations

Foundations: H-2 C-K D-K S-J
Freecells:  3H  5H      9H
: 6H
: KS QH
: 8H
: KH QS JH
:
: TH
: 4H
: 7H


====================

Move a card from freecell 0 to the foundations

Foundations: H-3 C-K D-K S-J
Freecells:      5H      9H
: 6H
: KS QH
: 8H
: KH QS JH
:
: TH
: 4H
: 7H


====================

Move a card from stack 6 to the foundations

Foundations: H-4 C-K D-K S-J
Freecells:      5H      9H
: 6H
: KS QH
: 8H
: KH QS JH
:
: TH
:
: 7H


====================

Move a card from freecell 1 to the foundations

Foundations: H-5 C-K D-K S-J
Freecells:              9H
: 6H
: KS QH
: 8H
: KH QS JH
:
: TH
:
: 7H


====================

Move a card from stack 0 to the foundations

Foundations: H-6 C-K D-K S-J
Freecells:              9H
:
: KS QH
: 8H
: KH QS JH
:
: TH
:
: 7H


====================

Move a card from stack 7 to the foundations

Foundations: H-7 C-K D-K S-J
Freecells:              9H
:
: KS QH
: 8H
: KH QS JH
:
: TH
:
:


====================

Move a card from stack 2 to the foundations

Foundations: H-8 C-K D-K S-J
Freecells:              9H
:
: KS QH
:
: KH QS JH
:
: TH
:
:


====================

Move a card from freecell 3 to the foundations

Foundations: H-9 C-K D-K S-J
Freecells:
:
: KS QH
:
: KH QS JH
:
: TH
:
:


====================

Move a card from stack 5 to the foundations

Foundations: H-T C-K D-K S-J
Freecells:
:
: KS QH
:
: KH QS JH
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-J C-K D-K S-J
Freecells:
:
: KS QH
:
: KH QS
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-J C-K D-K S-Q
Freecells:
:
: KS QH
:
: KH
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-Q C-K D-K S-Q
Freecells:
:
: KS
:
: KH
:
:
:
:


====================

Move a card from stack 3 to the foundations

Foundations: H-K C-K D-K S-Q
Freecells:
:
: KS
:
:
:
:
:
:


====================

Move a card from stack 1 to the foundations

Foundations: H-K C-K D-K S-K
Freecells:
:
:
:
:
:
:
:
:


====================

This game is solveable.
EOF
        "After KS to empty pile move.",
    );
}
