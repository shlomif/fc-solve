#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 7;
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

