#!/usr/bin/perl

use strict;
use warnings;

use Test::Differences (qw( eq_or_diff ));

require 5.008;

use File::Spec;

use autodie;

use Test::More tests => 3;

use Games::Solitaire::Verify::App::CmdLine::From_Patsolve;

my $WS = ' ';
my $sol_fn = File::Spec->catfile(File::Spec->curdir(),
    qw(t data sample-solutions patsolve-338741497-win-solution.txt)
);

my $board_fn = File::Spec->catfile(File::Spec->curdir(),
    qw(t data boards 338741497.board)
);

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
            argv => [qw(-g freecell), $board_fn, $sol_fn,],
        }
    );

    $obj->_read_initial_state();

    # TEST
    ok (1, "_read_initial_state is successful.");

    $obj->_perform_move("2S to temp");

    # TEST
    eq_or_diff (
        scalar ( $obj->_get_buffer ),
        <<"EOF",
-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS
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

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:  2S$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS
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
    eq_or_diff (
        scalar ( $obj->_get_buffer ),
        <<"EOF",
-=-=-=-=-=-=-=-=-=-=-=-

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS
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

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:  2S$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS$WS
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

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:  2S  6C$WS$WS$WS$WS$WS$WS$WS$WS
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

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:  2S  6C  9C$WS$WS$WS$WS
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

Foundations: H-0 C-0 D-0 S-0$WS
Freecells:  2S  6C  9C$WS$WS$WS$WS
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
}

