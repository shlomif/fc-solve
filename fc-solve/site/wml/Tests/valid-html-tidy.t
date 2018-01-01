#!/usr/bin/perl

use strict;
use warnings;

use Test::HTML::Tidy::Recursive ();

Test::HTML::Tidy::Recursive->new(
    {
        targets => [ './dest', './dest-prod' ],
    }
)->run;
