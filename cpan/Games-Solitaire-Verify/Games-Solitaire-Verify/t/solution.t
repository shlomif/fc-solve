#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 16;

use Data::Dumper;

use Games::Solitaire::Verify::Solution ();
use Path::Tiny qw/ path /;

sub _samp_sol
{
    return
        scalar Path::Tiny->cwd->child( qw/t data sample-solutions/, shift )
        ->openr;
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-freecell-24.txt"),
            variant  => "freecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Everything is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-freecell-24-wrong-1.txt"),
            variant  => "freecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( $verdict, "Solution is invalid" )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-freecell-24-wrong-2.txt"),
            variant  => "freecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( $verdict, "Solution is invalid" )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-bakers-game-24.txt"),
            variant  => "bakers_game",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Everything is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-freecell-24.txt"),
            variant  => "bakers_game",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( $verdict, "bakers_game cannot solve a Freecell solution" )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-forecell-24.txt"),
            variant  => "forecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Everything is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-freecell-24.txt"),
            variant  => "forecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( $verdict, "forecell cannot solve a Freecell solution" )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-seahaven-towers-1977.txt"),
            variant  => "seahaven_towers",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Seahaven Towers Solution is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-relaxed-freecell-11982.txt"),
            variant  => "freecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( $verdict, "Freecell cannot solve a Relaxed Freecell Game" )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-relaxed-freecell-11982.txt"),
            variant  => "relaxed_freecell",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Relaxed Freecell Solution is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-eight-off-200.txt"),
            variant  => "eight_off",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Eight Off Solution is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-eight-off-200-l-gi.txt"),
            variant  => "eight_off",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Eight Off -l gi Solution is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    my $relaxed_fc_variant_params =
        Games::Solitaire::Verify::VariantParams->new(
        {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 4,
            'sequence_move'          => "unlimited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "any",
        }
        );

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh       => _samp_sol("fcs-relaxed-freecell-11982.txt"),
            variant        => "custom",
            variant_params => $relaxed_fc_variant_params,
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Custom Relaxed Freecell Solution is OK." )
        or diag( "Verdict == " . Dumper($verdict) );
}

{

    my $fc_variant_params = Games::Solitaire::Verify::VariantParams->new(
        {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 4,
            'sequence_move'          => "limited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "any",
        }
    );

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh       => _samp_sol("fcs-relaxed-freecell-11982.txt"),
            variant        => "custom",
            variant_params => $fc_variant_params,
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( $verdict, "Custom Freecell cannot solve a Relaxed Freecell Game" )
        or diag( "Verdict == " . Dumper($verdict) );
}

{
    my $fc_variant_params = Games::Solitaire::Verify::VariantParams->new(
        {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 0,
            'sequence_move'          => "unlimited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "kings",
        }
    );

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol(
                "fcs-larrysan-kings-only-0-freecells-unlimited-move.txt"),
            variant        => "custom",
            variant_params => $fc_variant_params,
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict,
        "Solution of Zero-freecell, unlimited move, kings-only freecell is OK."
    ) or diag( "Verdict == " . Dumper($verdict) );
}

{

    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-simple-simon-24.txt"),
            variant  => "simple_simon",
        },
    );

    my $verdict = $solution->verify();

    # TEST
    ok( !$verdict, "Everything is OK with Simple Simon Deal No. 24." )
        or diag( "Verdict == " . Dumper($verdict) );
}
