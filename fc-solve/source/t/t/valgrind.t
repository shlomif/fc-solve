#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
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
        "--track-origins=yes",
        "--leak-check=yes",
        "--log-file=$log_fn",
        $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve",
        @$cmd_line_args,
    );
    
    open my $read_from_valgrind, "<", $log_fn
        or die "Cannot open valgrind.log for reading";
    my $found_error_summary = 0;
    my $found_malloc_free = 0;
    LINES_LOOP:
    while (my $l = <$read_from_valgrind>)
    {
        if (index($l, q{ERROR SUMMARY: 0 errors from 0 contexts}) >= 0)
        {
            $found_error_summary = 1;
        }
        elsif (index($l, q{malloc/free: in use at exit: 0 bytes}) >= 0)
        {
            $found_malloc_free = 1;
        }
    }
    close ($read_from_valgrind);

    if (ok (($found_error_summary && $found_malloc_free), $blurb))
    {
        unlink($log_fn);
    }
    else
    {
        die "Valgrind failed";
    }
}

# TEST
test_using_valgrind(
    ["1", "2", "1", "-l", "gi"],
    qq{"range-parallel-solve 1 2 1 -l gi" returned no errors}
);

# TEST
test_using_valgrind(
    ["1", "2", "1", "-opt"],
    qq{"range-parallel-solve 1 2 1 -opt" returned no errors}
);

# TEST
test_using_valgrind(
    [qw(11981 11983 1 -opt)],
    qq{"range-parallel-solve 11981 11983 1 -opt" returned no errors}
)
