#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Games::Solitaire::Verify::Solution;
use String::ShellQuote;
use Carp;

# 24 is my lucky number.
# -- Shlomi Fish
{
    open my $fc_solve_output, "pi-make-microsoft-freecell-board 24 | fc-solve -l gi -p -t -sam |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant => "freecell",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Everyting is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($fc_solve_output);
}
