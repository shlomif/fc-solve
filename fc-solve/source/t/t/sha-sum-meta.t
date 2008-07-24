#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;

use lib './t/lib';

use Games::Solitaire::FC_Solve::ShaAndLen;

{
    my $string = "HelloWorld";

    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    $sha->add($string);

    # TEST
    is ($sha->len(), 10, "Length is 10");

    # TEST
    is ($sha->hexdigest(),
        "872e4e50ce9990d8b041330c47c9ddd11bec6b503ae9386a99da8584e9bb12c4",
        "Hex digest is OK."
    );
}
