#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 8;
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

    open my $fc_solve_output, 
        "make_pysol_freecell_board.py $deal $variant | " . 
        "fc-solve -g $variant " . shell_quote(@$theme) . " -p -t -sam |"
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

# 24 is my lucky number. (Shlomif)
# TEST
verify_solution_test({deal => 24}, "Verifying the solution of deal #24");

# TEST
verify_solution_test({deal => 1941}, "Verifying 1941 (The Hardest Deal)");

# TEST
verify_solution_test({deal => 24, theme => [],}, 
    "Solving Deal #24 with the default heuristic"
);

# TEST
verify_solution_test({deal => 617, theme => ["-l", "john-galt-line"],}, 
    "Solving Deal #617 with the john-galt-line"
);

# TEST
verify_solution_test({deal => 24, variant => "bakers_game", theme => [],}, 
    "Baker's Game Deal #24"
);

# TEST
verify_solution_test({deal => 1099, variant => "forecell", theme => [],}, 
    "Forecell Deal #1099"
);


# TEST
verify_solution_test({deal => 11982, variant => "relaxed_freecell", },
    "Relaxed Freecell Deal #11982"
);


# TEST
verify_solution_test(
    {
        deal => 1977,
        variant => "seahaven_towers",
        theme => ["-l", "fools-gold",],
    }, 
    "Seahaven Towers #1977"
);

