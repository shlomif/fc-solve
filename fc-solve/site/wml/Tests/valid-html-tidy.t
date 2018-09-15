#!/usr/bin/perl

use strict;
use warnings;

use Test::HTML::Tidy::Recursive ();

Test::HTML::Tidy::Recursive->new(
    {
        targets         => [ './dest', './dest-prod' ],
        filename_filter => sub {
            return shift !~ m{/docs/distro/};
        },
    }
)->run;
