#!/usr/bin/perl

use strict;
use warnings;

use Test::TrailingSpace;
use Test::More tests => 1;

{
    my $finder = Test::TrailingSpace->new(
        {
            root => $ENV{FCS_SRC_PATH},
            filename_regex => qr/./,
            abs_path_prune_re => qr#CMakeFiles|_Inline|(?:\.(?:xcf|patch)\z)|#,
        }
    );

    # TEST
    $finder->no_trailing_space("No trailing whitespace was found.")
}
