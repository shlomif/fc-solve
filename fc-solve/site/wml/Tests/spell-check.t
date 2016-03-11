#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

{
    my $output = `./scripts/spell-checker-iface 2>&1`;
    chomp($output);

    my $status = $?;

    # TEST
    is ($output, '', "No spelling errors.");

    if ($status)
    {
        die "Failed to execute ./scripts/spell-checker-iface";
    }
}
