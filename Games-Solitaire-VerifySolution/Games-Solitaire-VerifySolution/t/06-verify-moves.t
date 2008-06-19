#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 18;
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

    my $move2 = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 6 to freecell 0",
            game => "freecell",
        },
    );

    # TEST
    ok($move2, "Move 2 was initialised.");

    # TEST
    ok (!$board->verify_and_perform_move($move2),
        "Testing for right movement"
    );

    # TEST
    is ($board->get_freecell(0)->to_string(), "JD",
        "Card has moved to the freecell");

    # TEST
    is ($board->get_column(6)->to_string(), ": AH 5S 6S AD 8H");

    my $move3_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 2 to freecell 0",
            game => "freecell",
        },
    );

    # TEST
    ok($move3_bad, "Move 3 (bad) was initialised.");

    # TEST
    ok ($board->verify_and_perform_move($move3_bad),
        "\$move3_bad cannot be performed"
    );
}

{
    my $string = <<"EOF";
Foundations: H-6 C-A D-A S-7 
Freecells:  3D          9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC JH
: 
: 2D KD TH TC TD 8D 7C 6D 5C 4D 3C
: 7H JS KH TS KC QD JC
: 9D 8S 7D 6C 5D
: 9S 8H
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 3 to freecell 1",
            game => "freecell",
        },
    );

    # TEST
    ok ($move1_bad, "Move-1-Bad was initialised.");

    # TEST
    ok ($board->verify_and_perform_move($move1_bad),
        "\$move1_bad cannot be performed"
    );
}

{
    my $string = <<"EOF";
Foundations: H-6 C-6 D-A S-8 
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1 = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 2 to the foundations",
            game => "freecell",
        },
    );

    # TEST
    ok ($move1, "Move-1 was initialised.");

    # TEST
    ok (!$board->verify_and_perform_move($move1),
        "Testing for right movement"
    );

    # TEST
    ok (!defined($board->get_freecell(2)), "Freecell 2 is empty after the move");

    # TEST
    is ($board->get_foundation_value("S", 0), 9,
        "9S is now in founds"
    );
}

{
    my $string = <<"EOF";
Foundations: H-6 C-6 D-A S-8 
Freecells:  3D  4D  9S  5D
: 9C 8H
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D
: 7H JS KH TS 9H 8C 7D
: 9D
: QS JD
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 0 to the foundations",
            game => "freecell",
        },
    );

    # TEST
    ok ($move1_bad, "Move-1-Bad was initialised.");

    # TEST
    ok ($board->verify_and_perform_move($move1_bad),
        "Testing that the move is rejected"
    );
}

