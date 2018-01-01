#!/usr/bin/perl

use strict;
use warnings;

use Test::HTML::Recursive::DeprecatedTags;

Test::HTML::Recursive::DeprecatedTags->new(
    {
        targets => ['./dest'],
    }
)->run;
