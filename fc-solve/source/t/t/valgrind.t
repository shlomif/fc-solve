#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;

use Games::Solitaire::Verify::Solution;

{
    my $log_fn = "valgrind.log";
    system(
        "valgrind",
        "--log-file=$log_fn",
        $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve",
        "1", "2", "1", "-l", "gi",
    );
    
    open my $read_from_valgrind, "<", $log_fn
        or die "Cannot open valgrind.log for reading";
    my $found = 0;
    LINES_LOOP:
    while (my $l = <$read_from_valgrind>)
    {
        if (index($l, q{ERROR SUMMARY: 0 errors from 0 contexts}) >= 0)
        {
            $found = 1;
            last LINES_LOOP;
        }
    }
    close ($read_from_valgrind);

    # TEST
    if (ok ($found, "\"range-parallel-solve 1 2 1 -l gi\" returned no errors"))
    {
        unlink($log_fn);
    }
}

