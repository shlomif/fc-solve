#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 96;
use Games::Solitaire::Verify::State;
use Games::Solitaire::Verify::Move;
use Games::Solitaire::Verify::Exception;

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

    my $err = $board->verify_and_perform_move($move3_bad);

    # TEST
    isa_ok ($err,
        "Games::Solitaire::Verify::Exception::Move::Dest::Freecell",
        "\$move3_bad cannot be performed due to an occupied freecell"
    );

    # TEST
    is ($err->move()->source_type(),
        $move3_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move3_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move3_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move3_bad->dest(),
        "dest() is identical in \$err->move() and original move",
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


    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok($err,
        "Games::Solitaire::Verify::Exception::Move::Src::Col::NoCards",
        "\$move1_bad cannot be performed due to no cards in stack",
        );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
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

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok ($err, 
        "Games::Solitaire::Verify::Exception::Move::Dest::Foundation",
        "Bad Move of the correct dest - no suitable foundation",
    );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-0 C-0 D-A S-A 
Freecells:  JD  8H  2H  4S
: 4C 2C 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
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
            fcs_string => "Move a card from freecell 0 to stack 0",
            game => "freecell",
        }
    );

    # TEST
    ok ($move1, "Freecell->Stack move was initialised.");

    # TEST
    ok (!$board->verify_and_perform_move($move1),
        "Performing Freecell->Stack move"
    );

    # TEST
    ok (!defined($board->get_freecell(0)), "Freecell after Freecell->Stack move is empty");

    # TEST
    is ($board->get_column(0)->to_string(), ": 4C 2C 9C 8C QS JD",
        "Stack is ok after Freecell->Stack move"
    );
}

{
    my $string = <<"EOF";
Foundations: H-0 C-0 D-A S-A 
Freecells:  JD  8H  2H  4S
: 4C 2C 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 0 to stack 1",
            game => "freecell",
        }
    );

    # TEST
    ok ($move1_bad, "Freecell->Stack move was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok ($err,
        "Games::Solitaire::Verify::Exception::Move::Dest::Col::NonMatchSuits",
        "Due to non-matching suits",
    );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->seq_build_by(), "alt_color", 
        "Error contains sequence built by alternate color"
    );
}

{
    my $string = <<"EOF";
Foundations: H-5 C-A D-A S-A 
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
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
            fcs_string => "Move 2 cards from stack 3 to stack 4",
            game => "freecell",
        }
    );

    # TEST
    ok ($move1, "Stack->Stack move was initialised.");

    # TEST
    ok (!$board->verify_and_perform_move($move1),
        "Was able to perform a stack->stack move."
    );

    # TEST
    is ($board->get_column(3)->to_string(),
        ": 5D 2S JC 5C JH",
        "Testing that the source stack is OK."
    );

    # TEST
    is ($board->get_column(4)->to_string(),
        ": 2D KD TH TC TD 8D 7C 6D 5S",
        "Testing that the dest stack is OK."
    );
}

{
    my $string = <<"EOF";
Foundations: H-5 C-A D-A S-A 
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move 2 cards from stack 0 to stack 1",
            game => "freecell",
        }
    );

    # TEST
    ok ($move1_bad, "Bad Stack->Stack move was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok ($err,
        "Games::Solitaire::Verify::Exception::Move::NotEnoughEmpties",
        "Cannot perform a bad stack->stack move with not enough columns.",
    );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );

}

{
    # Initial MS Freecell Position No. 24
    my $string = <<"EOF";
Foundations: H-0 C-0 D-0 S-0 
Freecells:                
: 4C 2C 9C 8C QS 4S 2H
: 8D QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 5H
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

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move 2 cards from stack 0 to stack 4",
            game => "freecell",
        },
    );

    # TEST
    ok($move1_bad, "Bad Move 1 was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok ($err,
        "Games::Solitaire::Verify::Exception::Move::Src::Col::NonSequence",
        "Cannot move non-sequence"
    );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-2 C-0 D-A S-A 
Freecells:  8D  8H  6S  5S
: 4C 2C 9C 8C QS 4S
: 5H QH 3C AC 3H 4H QD JD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD
: 7H JS KH TS KC 7C
: 
: 7S 6C 7D 4D 8S 9D
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "bakers_game",
        }
    );

    my $move1 = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move 1 cards from stack 4 to stack 1",
            game => "bakers_game",
        }
    );

    # TEST
    ok ($move1, "Stack->Stack move was initialised.");

    # TEST
    ok (!$board->verify_and_perform_move($move1),
        "Was able to perform a stack->stack move."
    );

    # TEST
    is ($board->to_string(), <<"EOF", "New board is OK.");
Foundations: H-2 C-0 D-A S-A 
Freecells:  8D  8H  6S  5S
: 4C 2C 9C 8C QS 4S
: 5H QH 3C AC 3H 4H QD JD TD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC
: 7H JS KH TS KC 7C
: 
: 7S 6C 7D 4D 8S 9D
EOF
}

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

    my $bad_move1 = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 2 to the foundations",
            game => "freecell",
        },
    );

    # TEST
    ok($bad_move1, "Move 1 was initialised");

    my $err = $board->verify_and_perform_move($bad_move1);

    # TEST
    isa_ok ($err, 
        "Games::Solitaire::Verify::Exception::Move::Dest::Foundation",
        "Bad Move of the correct dest - no suitable foundation",
    );

    # TEST
    is ($err->move()->source_type(),
        $bad_move1->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $bad_move1->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $bad_move1->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $bad_move1->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-0 C-0 D-A S-A 
Freecells:  JD      2H  4S
: 4C 2C 9C 8C QS 8H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S
: 7S 6C 7D 4D 8S 9D
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 1 to stack 5",
            game => "freecell",
        }
    );

    # TEST
    ok ($move1_bad, "Freecell->Stack move was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok($err,
        "Games::Solitaire::Verify::Exception::Move::Src::Freecell::Empty",
        "\$move1_bad cannot be performed due to empty freecell",
        );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-6 C-6 D-A S-8 
Freecells:  3D  4D      5D
: 9C 8H
: KS QH JC 9S
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
            fcs_string => "Move a card from freecell 2 to the foundations",
            game => "freecell",
        },
    );

    # TEST
    ok ($move1_bad, "Move-1 was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok ($err,
        "Games::Solitaire::Verify::Exception::Move::Src::Freecell::Empty",
        "\$move1_bad cannot be performed because it's an empty freecell"
    );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-5 C-A D-A S-A 
Freecells:  6S  8H  3C  4S
: 4C 2C 9C 8C QS JD
: 
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D 5S
: 2D KD TH TC TD 8D 7C
: 7H JS KH TS KC QD
: QH
: 7S 6C 7D 4D 8S 9D
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move 2 cards from stack 6 to stack 1",
            game => "freecell",
        }
    );

    # TEST
    ok ($move1_bad, "Stack->Stack move was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok($err,
        "Games::Solitaire::Verify::Exception::Move::Src::Col::NotEnoughCards",
        "\$move1_bad cannot be performed due to not enough cards in column",
        );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-6 C-4 D-A S-7 
Freecells:  3D  QS  9C  JD
: 
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
: 9D 8S 7D 6C 5D
: 9S 8H
EOF
    
    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "forecell",
        }
    );

    my $move1_bad = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from freecell 2 to stack 0",
            game => "freecell",
        }
    );
    
    # TEST
    ok ($move1_bad, "Stack->Stack move was initialised.");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok($err,
        "Games::Solitaire::Verify::Exception::Move::Dest::Col::OnlyKingsCanFillEmpty",
        "\$move1_bad cannot be performed because only kings can fill empty stack",
        );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}

{
    my $string = <<"EOF";
Foundations: H-6 C-4 D-A S-7 
Freecells:  3D  QS  9C  JD
: 
: KS QH JC
: QC JH
: KC QD
: 2D KD TH TC TD 8D 7C 6D 5C 4D
: 7H JS KH TS 9H 8C
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
            fcs_string => "Move a card from freecell 3 to stack 1",
            game => "freecell",
        }
    );
    
    # TEST
    ok ($move1_bad, "Freecell->Stack move was initialised");

    my $err = $board->verify_and_perform_move($move1_bad);

    # TEST
    isa_ok($err,
        "Games::Solitaire::Verify::Exception::Move::Dest::Col::RankMismatch",
        '$move1_bad',
        );

    # TEST
    is ($err->move()->source_type(),
        $move1_bad->source_type(),
        "source_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest_type(),
        $move1_bad->dest_type(),
        "dest_type is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->source(),
        $move1_bad->source(),
        "source() is identical in \$err->move() and original move",
    );

    # TEST
    is ($err->move()->dest(),
        $move1_bad->dest(),
        "dest() is identical in \$err->move() and original move",
    );
}
