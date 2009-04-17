#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;

use Games::Solitaire::Verify::Solution;

sub test_using_valgrind
{
    my $cmd_line_args = shift;
    my $blurb = shift;

    my $log_fn = "valgrind.log";

    system(
        "valgrind",
        "--log-file=$log_fn",
        $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve",
        @$cmd_line_args,
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

    if (ok ($found, $blurb))
    {
        unlink($log_fn);
    }
}

# TEST
test_using_valgrind(
    ["1", "2", "1", "-l", "gi"],
    "\"range-parallel-solve 1 2 1 -l gi\" returned no errors"
);

# TEST
test_using_valgrind(
    ["1", "2", "1", "-opt"],
    "\"range-parallel-solve 1 2 1 -opt\" returned no errors"
);

