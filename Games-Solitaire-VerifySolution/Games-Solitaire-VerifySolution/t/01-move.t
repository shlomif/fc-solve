#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 8;
use Games::Solitaire::VerifySolution::Move;

{
    my $move = Games::Solitaire::VerifySolution::Move->new(
        {
            fcs_string => "Move a card from stack 3 to the foundations",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "Checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "source_type() is stack");

    # TEST
    is ($move->dest_type(), "foundation", "dest_type() is foundation");

    # TEST
    is ($move->source(), 3, "source() is 3");
}

{
    my $move = Games::Solitaire::VerifySolution::Move->new(
        {
            fcs_string => "Move a card from stack 0 to the foundations",
            game => "freecell",
        }
    );

    # TEST
    ok ($move, "Checking that ::Move->new works");

    # TEST
    is ($move->source_type(), "stack", "source_type() is stack");

    # TEST
    is ($move->dest_type(), "foundation", "dest_type() is foundation");

    # TEST
    is ($move->source(), 0, "source() is 0");
}
