#!/usr/bin/perl

use strict;
use warnings;

use IO::File;
use List::Util qw(first);

open my $data_out, ">", "iters-by-quotas.txt";
$data_out->autoflush();

foreach my $quota (100 .. 1_000)
{
    open my $process, "-|", 
        $^X, 
        "process.pl",
        '--quotas-are-cb',
        "--quotas-expr=sub { return (shift() < 5000 ? $quota : undef()); }"
        or die "Could not open process for quota '$quota'";

    my @lines = <$process>;
    close($process);

    my $total_iters = first { m{^total_iters = (\d+)} } @lines;

    $total_iters =~ m{(\d+)};

    $data_out->print("$quota\t$1\n");
}

$data_out->close();

