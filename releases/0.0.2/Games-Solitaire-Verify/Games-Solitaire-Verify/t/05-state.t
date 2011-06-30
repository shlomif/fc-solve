#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 27;
use Games::Solitaire::Verify::State;

{
    my $string = <<"EOF";
Foundations: H-6 C-A D-A S-4
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D
EOF
    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    # TEST
    is ($board->get_foundation_value("H", 0), 6, "Foundation H Value");

    # TEST
    is ($board->get_foundation_value("C", 0), 1, "Foundation C Value");

    # TEST
    is ($board->get_foundation_value("D", 0), 1, "Foundation D Value");

    # TEST
    is ($board->get_foundation_value("S", 0), 4, "Foundation S Value");

    # TEST
    is ($board->get_freecell(0)->to_string(), "3D", "Freecell 0");

    # TEST
    ok (!defined($board->get_freecell(1)), "Freecell 1");

    # TEST
    is ($board->get_freecell(2)->to_string(), "JH", "Freecell 2");
    
    # TEST
    is ($board->get_freecell(3)->to_string(), "9H", "Freecell 9");

    # TEST
    is ($board->num_freecells(), 4, "Num Freecells");

    # TEST
    is ($board->num_empty_freecells(), 1, "Num empty freecells");

    # TEST
    is ($board->num_columns(), 8, "Num columns");

    # TEST
    is ($board->get_column(0)->to_string(), ": 4C 2C 9C 8C QS JD", 
        "Column 0"
    );

    # TEST
    is ($board->get_column(5)->to_string(), ": 7H JS KH TS KC QD JC",
        "Column 5"
    );
}

{
    my $string = <<"EOF";
Foundations: H-T C-7 D-6 S-J 
Freecells:  KH      KD    
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: 
: 
: KC QD
: 
: QS JD
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    # TEST
    is ($board->num_empty_columns(), 3, "Num empty columns")
}

{
    my $string = <<"EOF";
Foundations: H-T C-7 D-6 S-J 
Freecells:  KH      KD    
: 9C 8D
: KS QH JC TD
: QC JH TC 9D 8C 7D
: 
: 
: KC QD
: 
: QS JD
EOF

    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    my $copy = $board->clone();

    # TEST
    ok ($copy, "Copied");

    # TEST
    is ($copy->get_column(1)->to_string(), ": KS QH JC TD", 
        "Column 1 of Copy",
    );

    # TEST
    is ($copy->get_column(5)->to_string(), ": KC QD",
        "Column 5 of Copy",
    );

    # TEST
    is ($copy->get_foundation_value("C", 0), 7, "Foundation C Value of Copy");
}

{
    my $string = <<"EOF";
Foundations: H-6 C-A D-A S-4
Freecells:  3D      JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D 3C
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D
EOF
    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    # Do a simple move.
    $board->set_freecell(1, $board->get_column(3)->pop());

    my $new_board = <<"EOF";
Foundations: H-6 C-A D-A S-4 
Freecells:  3D  3C  JH  9H
: 4C 2C 9C 8C QS JD
: KS QH
: QC 9S 8H
: 5C 4D
: 2D KD TH TC TD 8D 7C 6D 5S
: 7H JS KH TS KC QD JC
: 9D 8S
: 7S 6C 7D 6S 5D
EOF

    # TEST
    is ($board->to_string(), $new_board, "Testing to_string() on a board");
}

{
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

    # TEST
    is ($board->to_string(), $string, "Testing board with H-0");
}

{
    my $string = <<"EOF";
Foundations: H-0 C-0 D-0 S-0 
Freecells:                
4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF

    my $board;

    eval {
        $board = Games::Solitaire::Verify::State->new(
            {
                string => $string,
                variant => "freecell",
        }
        );
    };

    my $err = $@;

    # TEST
    isa_ok(
        $err,
        "Games::Solitaire::Verify::Exception::Parse::State::Column",
        "Thrown a parse-column-prefix error",
    );
}

{
    my $string = <<"EOF";
Foundations: H-0 C-0 D-0 S-0 
Freecells:                
==<foo>== 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF

    my $board;

    eval {
        $board = Games::Solitaire::Verify::State->new(
            {
                string => $string,
                variant => "freecell",
        }
        );
    };

    my $err = $@;

    # TEST
    isa_ok(
        $err,
        "Games::Solitaire::Verify::Exception::Parse::State::Column",
        "Thrown a parse-column-prefix error",
    );
}

{
    my $board = Games::Solitaire::Verify::State->new(
        {
            variant => "freecell",
        }
    );

    my $column = Games::Solitaire::Verify::Column->new(
        {
            cards =>
            [
                Games::Solitaire::Verify::Card->new(
                    {
                        string => "KH",
                        id => 1,
                        data => { key => 'Foo', },
                    },
                ),
                Games::Solitaire::Verify::Card->new(
                    {
                        string => "QS",
                        id => 2,
                        data => { key => 'Bar', },
                    }
                ),
            ],
        },
    );

    $board->add_column($column);

    # TEST
    is ($board->get_column(0)->pos(0)->id(), 1, "First card has ID '1'");

    # TEST
    is_deeply(
        $board->get_column(0)->pos(0)->data(),
        { key => 'Foo', },
        "First card has right data()",
    );

    # TEST
    is ($board->get_column(0)->pos(1)->id(), 2, "Second card has ID '2'");

    foreach my $idx (1 .. (8-1))
    {
        $board->add_column(
            Games::Solitaire::Verify::Column->new(
                {
                    cards => [],
                }
            )
        );
    }

    $board->set_foundations(
        Games::Solitaire::Verify::Foundations->new(
            {
                num_decks => $board->num_decks(),
                string => 'Foundations: H-A C-A D-0 S-0',
            },
        ),
    );

    my $freecells = Games::Solitaire::Verify::Freecells->new(
        {
            count => 4,
        }
    );

    $board->set_freecells($freecells);

    my $copy = $board->clone();

    # TEST
    is ($copy->get_column(0)->pos(0)->id(), 1, "First card in copy has ID '1'");

    # TEST
    is_deeply (
        $copy->get_column(0)->pos(0)->data(),
        { key => 'Foo' }, 
        "First card in copy has right data."
    );
}
