#!/usr/bin/perl

use strict;
use warnings;

use IO::File;
use List::Util qw(first);

my $log_fn = "iters-by-quotas.txt";

my $start_quota = 100;
my $end_quota = 1_000;

if (-e $log_fn)
{
    my $last_line = `tail -1 $log_fn`;
    if (($start_quota) = $last_line =~ m{\A(\d+)})
    {
        # Everything is OK - do nothing.
    }
    else
    {
        die "Incorrect line '$last_line' at the end of $log_fn";
    }
}

open my $data_out, ">>", $log_fn
    or die "Could not open '$log_fn' for append";
$data_out->autoflush();

foreach my $quota ($start_quota .. $end_quota)
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

