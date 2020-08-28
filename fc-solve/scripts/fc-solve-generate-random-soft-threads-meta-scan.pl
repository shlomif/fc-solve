#! /usr/bin/env perl
#
# Short description for fcs-0fc.pl
#
# Version 0.0.1
# Copyright (C) 2020 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

use strict;
use warnings;
use 5.014;
use autodie;

use Path::Tiny qw/ path tempdir tempfile cwd /;

srand(24011);
my @seeds;
foreach my $i ( 1 .. 30 )
{
    push @seeds, 1 + int( rand(100_000) );
}
exit(
    system(
        # "echo",
        "./summary-fc-solve",
        @ARGV, "--",
        (
            map {
                (
                    "--method", "random-dfs",
                    "-seed",    $seeds[$_],
                    "-to",      "[0AB]=rand()",
                    "-step",    2000,
                    ( ( $_ < $#seeds ) ? ("-nst") : () ),
                )
            } ( keys(@seeds) ),
        ),
        "--freecells-num",
        0,
        "-mi",
        "1000000",
    )
);
