#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 10;
use Games::Solitaire::VerifySolution::Column;

{
    my $column = Games::Solitaire::VerifySolution::Column->new(
        {
            string => ": KH QS 5C",
        },
    );

    # TEST
    is ($column->len(), 3, "Column has three cards");

    # TEST
    is ($column->pos(0)->rank(), 13, "Card[0].rank");

    # TEST
    is ($column->pos(0)->suit(), "H", "Card[0].suit");

    # TEST
    is ($column->pos(1)->rank(), 12, "Card[1].rank");

    # TEST
    is ($column->pos(1)->suit(), "S", "Card[1].suit");

    # TEST
    is ($column->pos(2)->rank(), 5, "Card[2].rank");

    # TEST
    is ($column->pos(2)->suit(), "C", "Card[2].rank");
}

{
    my $column = Games::Solitaire::VerifySolution::Column->new(
        {
            string => ": 3D TS 4H 5C 6D",
        },
    );

    my $copy = $column->clone();

    # TEST
    is ($copy->len(), 5, "Length of copy");

    # TEST
    is ($copy->pos(1)->rank(), 10, "Rank of Copy Card");

    # TEST
    is ($copy->pos(1)->suit(), "S", "Suit of Copy Card");
}
