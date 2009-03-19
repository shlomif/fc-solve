#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Carp;
use Data::Dumper;
use String::ShellQuote;

use Games::Solitaire::Verify::Solution;

sub verify_solution_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;
    my $msg = shift;

    my $deal = $args->{deal};

    if ($deal !~ m{\A[1-9][0-9]*\z})
    {
        confess "Invalid deal $deal";
    }

    my $theme = $args->{theme} || ["-l", "gi"];

    my $variant = $args->{variant}  || "freecell";

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    open my $fc_solve_output,
        "make_pysol_freecell_board.py $deal $variant | " .
        "$fc_solve_exe -g $variant " . shell_quote(@$theme) . " -p -t -sam |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant => $variant,
        },
    );

    my $verdict = $solution->verify();
    my $test_verdict = ok (!$verdict, $msg);

    if (!$test_verdict)
    {
        diag("Verdict == " . Dumper($verdict));
    }

    close($fc_solve_output);

    return $test_verdict;
}

my $range_solver = $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve";

# TEST
ok (!system($range_solver, "1", "20", "10", "-l", "gi"), "Range solver was successful");

