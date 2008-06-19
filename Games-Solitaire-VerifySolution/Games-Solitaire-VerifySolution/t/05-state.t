#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 18;
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
