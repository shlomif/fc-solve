#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

{
    my $SCRIPT = './bin/spell-checker-iface';
    my $output = `$SCRIPT 2>&1`;
    chomp($output);

    my $status = $?;

    # TEST
    is( $output, '', "No spelling errors." );

    if ($status)
    {
        die "Failed to execute $SCRIPT!";
    }
}
