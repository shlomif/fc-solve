#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Test::Differences qw/ eq_or_diff /;
use Path::Tiny        qw/ path /;
use FC_Solve::Paths   qw/ offload_arg samp_board samp_sol /;

{
    my $expected_text = path( samp_sol('dbm-24-mid.sol') )->slurp_utf8;

    my $got_text =
`./dbm-fc-solver @{[offload_arg()]} --num-threads 1 @{[samp_board('24-mid.board')]}`
        =~ s/[ \t]+\n/\n/gr;

    foreach my $text ( $got_text, $expected_text )
    {
        $text =~ s/Time: [0-9]+\.[0-9]+/Time: 24/g;
    }

    # TEST
    eq_or_diff( $got_text, $expected_text, "Texts are the same" )
}
