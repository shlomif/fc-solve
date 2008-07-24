#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 6;

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

{
    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    $sha->add("Hello");
    $sha->add("World");

    # TEST
    is ($sha->len(), 10, "Length is 10");

    # TEST
    is ($sha->hexdigest(),
        "872e4e50ce9990d8b041330c47c9ddd11bec6b503ae9386a99da8584e9bb12c4",
        "Hex digest is OK."
    );
}

{
    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    open my $in, "<", "./t/data/sample-solutions/fcs-freecell-24.txt";
    $sha->add_file($in);
    close($in);

    # TEST
    is ($sha->len(), 32280, "Length for add_file() is 32280");

    # TEST
    is ($sha->hexdigest(),
        "17abf343aac1dec42b924b968588f2fa4e07632aab6932f5d084a6c754d61956",
        "Hex digest is OK for add_file()"
    );
}
