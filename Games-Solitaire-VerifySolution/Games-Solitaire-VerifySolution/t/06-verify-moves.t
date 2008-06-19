#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 4;
use Games::Solitaire::Verify::State;
use Games::Solitaire::Verify::Move;

{
    # Initial MS Freecell Position No. 24
    my $string = <<"EOF";
Foundations: H-0 C-0 D-0 S-0 
Freecells:                
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF
    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1 = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 3 to the foundations",
            game => "freecell",
        },
    );

    # TEST
    ok($move1, "Move 1 was initialised.");

    # TEST
    ok (!$board->verify_and_perform_move($move1),
        "Testing for right movement"
    );

    # TEST
    is ($board->get_column(3)->to_string(), ": 5D 2S JC 5C JH 6D",
        "Checking that the card was moved."
    );

    # TEST
    is ($board->get_foundation_value("S", 0), 1,
        "AS is now in founds",
    );
}

