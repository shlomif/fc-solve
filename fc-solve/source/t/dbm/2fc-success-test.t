#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use File::Spec;
use File::Temp qw(tempdir);
use Test::Differences;
use Path::Tiny;
use FC_Solve::Paths qw( samp_board );

{
    my $expected_text = path(File::Spec->catfile($ENV{FCS_SRC_PATH}, 't', 't', 'data', 'sample-solutions', 'dbm-24-mid.sol'))->slurp_utf8;

    my $tempdir = tempdir(CLEANUP => 1);
    my $got_text = `./dbm_fc_solver --offload-dir-path $tempdir --num-threads 1 @{[samp_board('24-mid.board')]}`;

    foreach my $text ($got_text, $expected_text)
    {
        $text =~ s/Time: \d+\.\d+/Time: 24/g;
    }

    # TEST
    eq_or_diff ($got_text, $expected_text, "Texts are the same")
}

