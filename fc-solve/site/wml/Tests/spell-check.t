#!/usr/bin/env perl

use strict;
use warnings;

use lib './lib';

use Test::More;

if ( $ENV{SKIP_SPELL_CHECK} )
{
    plan skip_all => 'Skipping spell check due to environment variable';
}
else
{
    plan tests => 1;
}

use Shlomif::Spelling::Iface_Local ();

# TEST
Shlomif::Spelling::Iface_Local->new->test_spelling("No spelling errors.");
