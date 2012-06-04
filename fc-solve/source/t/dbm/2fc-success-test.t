#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use File::Spec;
use File::Temp qw(tempdir);
use Test::Differences;

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

{
    my $path = File::Spec->catdir(File::Spec->curdir(), 't', 't', 'data');
    my $expected_text = _slurp(File::Spec->catfile($path, 'sample-solutions', 'dbm-24-mid.sol'));

    my $tempdir = tempdir(CLEANUP => 1);
    my $got_text = `./dbm_fc_solver --offload-dir-path $tempdir --num-threads 1 @{[File::Spec->catfile($path, 'sample-boards', '24-mid.board')]}`;

    foreach my $text ($got_text, $expected_text)
    {
        $text =~ s/Time: \d+\.\d+/Time: 24/g;
    }

    # TEST
    eq_or_diff ($got_text, $expected_text, "Texts are the same")
}

