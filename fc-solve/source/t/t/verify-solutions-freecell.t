#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 4;
use Data::Dumper (qw/Dumper/);
use FC_Solve::Paths qw( is_freecell_only samp_board );

use FC_Solve::GetOutput ();

use Games::Solitaire::Verify::Solution ();

sub verify_solution_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    my ( $args, $msg ) = @_;

    if ( exists( $args->{variant} ) and is_freecell_only() )
    {
        return ok( 1,
q#Test skipped because it's a non-Freecell variant on a Freecell-only build.#
        );
    }

    my $cmd_line        = FC_Solve::GetOutput->new($args);
    my $fc_solve_output = $cmd_line->open_cmd_line->{fh};

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant  => $cmd_line->variant,
            (
                $cmd_line->is_custom
                ? (
                    variant_params =>
                        Games::Solitaire::Verify::VariantParams->new(
                        $args->{variant_params}
                        )
                    )
                : ()
            ),
        },
    );

    my $verdict = $solution->verify();
    my $test_verdict = ok( !$verdict, $msg );

    if ( !$test_verdict )
    {
        diag( "Verdict == " . Dumper($verdict) );
    }

    close($fc_solve_output);

    return $test_verdict;
}

# TEST
verify_solution_test(
    {
        board => samp_board("24-mid-with-colons.board"),
    },
"Accepting a board with leading colons as directly input from the -p -t solution",
);

# TEST
verify_solution_test(
    {
        board =>
            samp_board( "larrysan-kings-only-0-freecells-unlimited-move.board",
            ),
        theme => [
            qw(--freecells-num 0 --empty-stacks-filled-by kings --sequence-move unlimited)
        ],
        variant        => "custom",
        variant_params => {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 0,
            'sequence_move'          => "unlimited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "kings",
        },
    },
    "sequence move unlimited is indeed unlimited (even if not esf-by-any)."
);

# This command line theme yields an especially short solution to the
# previously intractable deal #12 .
# TEST
verify_solution_test(
    {
        deal  => 12,
        theme => [
            qw(--freecells-num 2 -to '[012][347]' --method random-dfs -seed 33)
        ],
        variant        => "custom",
        msdeals        => 1,
        variant_params => {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 2,
            'sequence_move'          => "limited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "any",
        },
    },
    "Checking the 2-freecells '-seed 33' preset."
);

# This command line theme yields an ever shorter solution to the
# previously intractable deal #12 .
# TEST
verify_solution_test(
    {
        deal  => 12,
        theme => [
            qw(--freecells-num 2 -to '[012][347]' --method random-dfs -seed 236)
        ],
        variant        => "custom",
        msdeals        => 1,
        variant_params => {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 2,
            'sequence_move'          => "limited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "any",
        },
    },
    "Checking the 2-freecells '-seed 236' preset."
);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut

