#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use lib './lib';

use Shlomif::Spelling::Whitelist ();

# TEST
ok (
    Shlomif::Spelling::Whitelist->new->is_sorted(),
    "Whitelist file is sorted."
) or diag("Whitelist file is not sorted! Please run ./scripts/sort-check-spelling-file");
