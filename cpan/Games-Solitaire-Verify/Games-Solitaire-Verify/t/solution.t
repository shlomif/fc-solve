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

sub _solution_is_ok
{
    my ( $solution, $blurb ) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $verdict = $solution->verify();

    ok( !$verdict, $blurb )
        or diag( "Verdict == " . Dumper($verdict) );

    return;
}

{
    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-freecell-24.txt"),
            variant  => "freecell",
        },
    );

    # TEST
    _solution_is_ok( $solution, "freecell 24" );
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

    # TEST
    _solution_is_ok( $solution, 'bakers game 24' );
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

    # TEST
    _solution_is_ok( $solution, 'forecell 24' );
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

    # TEST
    _solution_is_ok( $solution, "Seahaven Towers Solution is OK." );
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

    # TEST
    _solution_is_ok( $solution, "Relaxed Freecell Solution is OK." );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-eight-off-200.txt"),
            variant  => "eight_off",
        },
    );

    # TEST
    _solution_is_ok( $solution, "Eight Off Solution is OK." );
}

{

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-eight-off-200-l-gi.txt"),
            variant  => "eight_off",
        },
    );

    # TEST
    _solution_is_ok( $solution, "Eight Off -l gi Solution is OK." );
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

    # TEST
    _solution_is_ok( $solution, "Custom Relaxed Freecell Solution is OK." )
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

    # TEST
    _solution_is_ok( $solution,
        "Solution of Zero-freecell, unlimited move, kings-only freecell is OK."
    );
}

{

    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => _samp_sol("fcs-simple-simon-24.txt"),
            variant  => "simple_simon",
        },
    );

    # TEST
    _solution_is_ok( $solution,
        "Everything is OK with Simple Simon Deal No. 24." );
}
