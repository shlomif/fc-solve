#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 8;
use Games::Solitaire::VerifySolution::State;

{
    my $string = <<"EOF";
Foundations: H-5 C-0 D-3 S-Q
Freecells:  3D      JH  9H
EOF
    my $board = Games::Solitaire::VerifySolution::State->new(
        {
            string => $string,
            variant => "freecell",
        }
    );

    # TEST
    is ($board->get_foundation_value("H", 0), 5, "Foundation H Value");

    # TEST
    is ($board->get_foundation_value("C", 0), 0, "Foundation C Value");

    # TEST
    is ($board->get_foundation_value("D", 0), 3, "Foundation D Value");

    # TEST
    is ($board->get_foundation_value("S", 0), 12, "Foundation S Value");

    # TEST
    is ($board->get_freecell(0)->to_string(), "3D", "Freecell 0");

    # TEST
    ok (!defined($board->get_freecell(1)), "Freecell 1");

    # TEST
    is ($board->get_freecell(2)->to_string(), "JH", "Freecell 2");
    
    # TEST
    is ($board->get_freecell(3)->to_string(), "9H", "Freecell 9");
}
