#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Carp;
use Data::Dumper;

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

    open my $fc_solve_output, 
        "pi-make-microsoft-freecell-board $deal | " . 
        "fc-solve -l gi -p -t -sam |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant => "freecell",
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

# 24 is my lucky number. (Shlomif)
# TEST
verify_solution_test({deal => 24}, "Verifying the solution of deal #24");

