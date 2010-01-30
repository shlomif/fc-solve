#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 19;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;

use Games::Solitaire::Verify::Solution;

sub verify_solution_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;
    my $msg = shift;

    my $board = $args->{board};
    my $deal = $args->{deal};

    if (! defined($board))
    {
        if (!defined($deal))
        {
            confess "Neither Deal nor board are specified";
        }
        if ($deal !~ m{\A[1-9][0-9]*\z})
        {
            confess "Invalid deal $deal";
        }
    }

    my $theme = $args->{theme} || ["-l", "gi"];

    my $variant = $args->{variant}  || "freecell";
    my $is_custom = ($variant eq "custom");
    my $variant_s = $is_custom ? "" : "-g $variant";

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    open my $fc_solve_output,
        ($board ? "" : "make_pysol_freecell_board.py $deal $variant | ") .
        "$fc_solve_exe $variant_s " . shell_quote(@$theme) . " -p -t -sam " .
        ($board ? shell_quote($board) : "") .
        " |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant => $variant,
            ($is_custom ? (variant_params => $args->{variant_params}) : ()),
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

# TEST
verify_solution_test({deal => 200, variant => "eight_off", },
    "Eight Off #200 with -l gi"
);

# TEST
verify_solution_test({deal => 200, variant => "eight_off", theme => [],},
    "Eight Off #200 with default heuristic"
);

# TEST
verify_solution_test({deal => 24, theme => ["-opt"],},
    "-opt should work."
);

# TEST
verify_solution_test(
    {
        board =>
        File::Spec->catfile(
            File::Spec->curdir(),
            "t",
            "data",
            "sample-boards",
            "larrysan-kings-only-0-freecells-unlimited-move.txt",
        ),
        theme => [qw(--freecells-num 0 --empty-stacks-filled-by kings --sequence-move unlimited)],
        variant => "custom",
        variant_params =>
            Games::Solitaire::Verify::VariantParams->new(
                {
                    'num_decks' => 1,
                    'num_columns' => 8,
                    'num_freecells' => 0,
                    'sequence_move' => "unlimited",
                    'seq_build_by' => "alt_color",
                    'empty_stacks_filled_by' => "kings",
                }
            ),

    },
    "sequence move unlimited is indeed unlimited (even if not esf-by-any)."
);

# TEST
verify_solution_test(
    {deal => 24, variant => "simple_simon",
        theme => [],
    },
    "Simple Simon #24 with default theme",
);

# TEST
verify_solution_test(
    {deal => 19806, variant => "simple_simon",
        theme => [],
    },
    "Simple Simon #19806 with default theme",
);

# TEST
verify_solution_test(
    {deal => 1, variant => "simple_simon",
        theme => ["-to", "abcdefghi",],
    },
    "Simple Simon #1 with abcdefghi (move seq to false parent)",
);

# TEST
verify_solution_test(
    {deal => 1, variant => "simple_simon",
        theme => ["-to", "abcdefgh", "-ni", "-to", "abcdefghi",],
    },
    "Simple Simon #1 using an -ni",
);

# TEST
verify_solution_test(
    {deal => 1, variant => "simple_simon",
        theme => ["-to", "abcdefgh", "--next-instance", "-to", "abcdefghi",],
    },
    "Simple Simon #1 using an -ni",
);


# TEST
verify_solution_test(
    {deal => 1, variant => "simple_simon",
        theme => ["-l", "tlm",],
    },
    "Simple Simon #1 using the 'the-last-mohican' theme",
);


# TEST
verify_solution_test(
    {deal => 24, theme => ["-nht",],
    },
    "Testing a solution with the -nht flag.",
);

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2008 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

