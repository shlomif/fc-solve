#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 4;
use Games::Solitaire::VerifySolution::State;

{
    my $board = Games::Solitaire::VerifySolution::State->new(
        {
            string => "Foundations: H-5 C-0 D-3 S-Q\n",
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
}
