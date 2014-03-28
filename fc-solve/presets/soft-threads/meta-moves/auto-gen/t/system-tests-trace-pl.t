#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use Test::Differences qw( eq_or_diff );

{
    # Was OK in this revision:
    # commit fc5e442cc942abb1665d6eed350b3228f27b830b
    # Author: Shlomi Fish <shlomif@shlomifish.org>
    # Date:   Fri Mar 28 19:03:26 2014 +0300
    # Made the demo dependent only on local data.

    # TEST
    eq_or_diff(
        scalar(`perl trace.pl DEMOS-programs/data/amateur-star.sh | sha256sum -`),
        "bf715fa2d5bbfdc0a48c68472e0ac85273e17986504e291fc7dbb2a87fcf03b0  -\n",
        "perl trace.pl gives the same results for amateur-star.sh",
    );
}

