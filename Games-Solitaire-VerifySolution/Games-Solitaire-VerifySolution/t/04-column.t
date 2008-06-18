#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 20;
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

{
    my $column1 = Games::Solitaire::VerifySolution::Column->new(
        {
            string => ": TH 9C 8D",
        }
    );

    my $column2 = Games::Solitaire::VerifySolution::Column->new(
        {
            string => ": 7S 6D 5C",
        }
    );

    $column1->append($column2);

    # TEST
    is ($column1->len(), 6, "Column has 6 cards after appending");

    # TEST
    is ($column1->pos(3)->rank(), 7, "Rank of appended card");

    # TEST
    is ($column1->pos(3)->suit(), "S", "Suit of appended card");
}

{
    my $column = Games::Solitaire::VerifySolution::Column->new(
        {
            string => ": KH QS 5C 3S",
        },
    );

    my $card = $column->pop();

    # TEST
    is ($column->len(), 3, "Column has three cards");

    # TEST
    is ($card->rank(), 3, "Popped card rank");

    # TEST
    is ($card->suit(), "S", "Card[0].suit");
}

{
    # TEST:$num_columns=4
    my @column_strings = (
        ": KH QS 5C 3S",
        ": 7S 6D 5C",
        ": 3D TS 4H 5C 6D",
        ": KH QS 5C",
    );

    foreach my $string (@column_strings)
    {
        my $column = Games::Solitaire::VerifySolution::Column->new(
            {
                string => $string,
            },
        );
        # TEST*$num_columns
        is ($column->to_string(), $string, "Stringification of '$string'");
    }
}
