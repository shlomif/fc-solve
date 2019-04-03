#!/usr/bin/perl

# This program converts the solution output of dbm_fc_solver
# (and depth_dbm_fc_solver) to something that can be used as input for
# verify-solitaire-solution. It accepts the same command line arguments
# as verify-solitaire-solution so you should use it something like that:
#
# perl scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl \
#  --freecells-num 2 -
#
use strict;
use warnings;

package Games::Solitaire::Verify::App::From_DBM_FC_Solver;

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::VariantsMap ();
use Games::Solitaire::Verify::Solution    ();
use Games::Solitaire::Verify::State       ();
use Games::Solitaire::Verify::Move        ();
use FC_Solve::HorneAutomovePrune          ();

use Getopt::Long qw(GetOptionsFromArray);

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _filename
            _variant_params
            )
    ]
);

sub _init
{
    my ( $self, $args ) = @_;

    my $argv = $args->{'argv'};

    my $variant_map = Games::Solitaire::Verify::VariantsMap->new();

    my $variant_params = $variant_map->get_variant_by_id("freecell");

    GetOptionsFromArray(
        $argv,
        'g|game|variant=s' => sub {
            my ( undef, $game ) = @_;

            $variant_params = $variant_map->get_variant_by_id($game);

            if ( !defined($variant_params) )
            {
                die "Unknown variant '$game'!\n";
            }
        },
        'freecells-num=i' => sub {
            my ( undef, $n ) = @_;
            $variant_params->num_freecells($n);
        },
        'stacks-num=i' => sub {
            my ( undef, $n ) = @_;
            $variant_params->num_columns($n);
        },
        'decks-num=i' => sub {
            my ( undef, $n ) = @_;

            if ( !( ( $n == 1 ) || ( $n == 2 ) ) )
            {
                die "Decks should be 1 or 2.";
            }

            $variant_params->num_decks($n);
        },
        'sequences-are-built-by=s' => sub {
            my ( undef, $val ) = @_;

            my %seqs_build_by = (
                ( map { $_ => $_ } (qw(alt_color suit rank)) ),
                "alternate_color" => "alt_color",
            );

            my $proc_val = $seqs_build_by{$val};

            if ( !defined($proc_val) )
            {
                die "Unknown sequences-are-built-by '$val'!";
            }

            $variant_params->seqs_build_by($proc_val);
        },
        'empty-stacks-filled-by=s' => sub {
            my ( undef, $val ) = @_;

            my %empty_stacks_filled_by_map =
                ( map { $_ => 1 } (qw(kings any none)) );

            if ( !exists( $empty_stacks_filled_by_map{$val} ) )
            {
                die "Unknown empty stacks filled by '$val'!";
            }

            $variant_params->empty_stacks_filled_by($val);
        },
        'sequence-move=s' => sub {
            my ( undef, $val ) = @_;

            my %seq_moves = ( map { $_ => 1 } (qw(limited unlimited)) );

            if ( !exists( $seq_moves{$val} ) )
            {
                die "Unknown sequence move '$val'!";
            }

            $variant_params->sequence_move($val);
        },
    ) or die "Cannot process command line arguments";

    my $filename = shift(@$argv);

    if ( !defined($filename) )
    {
        $filename = "-";
    }

    $self->_variant_params($variant_params);
    $self->_filename($filename);

    return;
}

sub _out_running_state
{
    my $running_state = shift;
    print $running_state->to_string();
    return;
}

sub run
{
    my $self = shift;

    my $filename       = $self->_filename();
    my $variant_params = $self->_variant_params();

    my $fh;

    if ( $filename eq "-" )
    {
        $fh = *STDIN;
    }
    else
    {
        open $fh, "<", $filename
            or die "Cannot open '$filename' - $!";
    }

    my $initial_state = Games::Solitaire::Verify::State->new(
        {
            variant        => "custom",
            variant_params => $self->_variant_params(),
            string         => do
            {
                local $/;
                my $s = <$fh>;
                $s =~ s/^(\w)/: $1/gms;
                $s = "Freecells:\n$s" if ( $s !~ /^Freecells/ms );
                $s = "Foundations: H-0 C-0 D-0 S-0\n$s" if ( $s !~ /^Found/ms );
                $s;
            },
        },
    );

    my $running_state = $initial_state->clone();

    FC_Solve::HorneAutomovePrune::_prune_all( $running_state,
        sub { }, sub { }, );
    _out_running_state($running_state);

    close($fh);
    return;
}

package main;

Games::Solitaire::Verify::App::From_DBM_FC_Solver->new( { argv => [@ARGV] } )
    ->run();

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
