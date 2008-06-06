#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Games::Solitaire::VerifySolution::Card;

{
    my $card = Games::Solitaire::VerifySolution::Card->new(
        {
            string => "AH",
        },
    );

    # TEST
    is ($card->rank(), 1, "Rank of AH is 1");

    # TEST
    is ($card->suit(), "H", "Suit of AH is Hearts");

    # TEST
    is ($card->color(), "red", "Color of AH is red");
}
