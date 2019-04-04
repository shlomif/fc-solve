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

use parent 'Games::Solitaire::Verify::App::CmdLine';

use Games::Solitaire::Verify::State              ();
use Games::Solitaire::Verify::HorneAutomovePrune ();

sub _read_next_state
{
    my ( $self, $fh ) = @_;
    my $line = <$fh>;
    chomp($line);
    if ( $line eq "END" )
    {
        return;
    }
    elsif ( $line ne "--------" )
    {
        die "Incorrect format.";
    }

    my $s = <$fh>;
LINES:
    while ( $line = <$fh> )
    {
        if ( $line !~ /\S/ )
        {
            last LINES;
        }
        $s .= $line;
    }
    $line = <$fh>;
    chomp($line);
    if ( $line ne "==" )
    {
        die "Cannot find '==' terminator";
    }

    return Games::Solitaire::Verify::State->new(
        {
            variant        => "custom",
            variant_params => $self->_variant_params(),
            string         => $s,
        },
    );
}

sub _out_running_state
{
    my $running_state = shift;
    print $running_state->to_string();
    print "\n\n====================\n\n";
    return;
}

sub _out_move
{
    my $move_s = shift;
    print "$move_s\n\n";
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

    my $found = 0;

LINES_PREFIX:
    while ( my $line = <$fh> )
    {
        chomp($line);
        if ( $line eq "Success!" )
        {
            $found = 1;
            last LINES_PREFIX;
        }
    }

    if ( !$found )
    {
        close($fh);
        die "State was not solved successfully.";
    }

    my $initial_state = $self->_read_next_state($fh);

    my $running_state = $initial_state->clone();

    my @cols_iter = ( 0 .. ( $running_state->num_columns() - 1 ) );
    my @fc_iter   = ( 0 .. ( $running_state->num_freecells() - 1 ) );
    my @cols_indexes = @cols_iter;
    my @fc_indexes   = @fc_iter;

    print "-=-=-=-=-=-=-=-=-=-=-=-\n\n";

    _out_running_state($running_state);
MOVES:
    while ( my $move_line = <$fh> )
    {
        chomp($move_line);

        if ( $move_line eq "END" )
        {
            last MOVES;
        }

        # I thought I needed them, but I did not eventually.
        #
        # my @rev_cols_indexes;
        # @rev_cols_indexes[@cols_indexes] = (0 .. $#cols_indexes);
        # my @rev_fc_indexes;
        # @rev_fc_indexes[@fc_indexes] = (0 .. $#fc_indexes);

        my ( $src, $dest );
        my $dest_move;

        my @tentative_fc_indexes   = @fc_indexes;
        my @tentative_cols_indexes = @cols_indexes;
        if ( ( $src, $dest ) =
            $move_line =~ m{\AColumn (\d+) -> Freecell (\d+)\z} )
        {
            $dest_move =
"Move a card from stack $tentative_cols_indexes[$src] to freecell $tentative_fc_indexes[$dest]";
        }
        elsif ( ( $src, $dest ) =
            $move_line =~ m{\AColumn (\d+) -> Column (\d+)\z} )
        {
            $dest_move =
"Move 1 cards from stack $tentative_cols_indexes[$src] to stack $tentative_cols_indexes[$dest]";
        }
        elsif ( ( $src, $dest ) =
            $move_line =~ m{\AFreecell (\d+) -> Column (\d+)\z} )
        {
            $dest_move =
"Move a card from freecell $tentative_fc_indexes[$src] to stack $tentative_cols_indexes[$dest]";
        }
        elsif ( ($src) = $move_line =~ m{\AColumn (\d+) -> Foundation \d+\z} )
        {
            $dest_move =
"Move a card from stack $tentative_cols_indexes[$src] to the foundations";
        }
        elsif ( ($src) = $move_line =~ m{\AFreecell (\d+) -> Foundation \d+\z} )
        {
            $dest_move =
"Move a card from freecell $tentative_fc_indexes[$src] to the foundations";
        }
        else
        {
            die "Unrecognized Move line '$move_line'.";
        }

        Games::Solitaire::Verify::HorneAutomovePrune::perform_and_output_move(
            {
                state        => $running_state,
                move_string  => $dest_move,
                output_state => \&_out_running_state,
                output_move  => \&_out_move,
            }
        );
        Games::Solitaire::Verify::HorneAutomovePrune::do_prune(
            {
                state        => $running_state,
                output_state => \&_out_running_state,
                output_move  => \&_out_move,
            }
        );

        my $new_state = $self->_read_next_state($fh);

        my $populate_new_resource_indexes = sub {
            my ( $iter, $get_pivot_cb ) = @_;

            my @new_resources_indexes;

            my %non_assigned_resources = ( map { $_ => 1 } @$iter );
            my %old_resources_map;

            foreach my $idx (@$iter)
            {
                my $card = $get_pivot_cb->( $running_state, $idx );

                push @{ $old_resources_map{$card} }, $idx;
            }

            foreach my $idx (@$iter)
            {
                my $card = $get_pivot_cb->( $new_state, $idx );
                my $aref = $old_resources_map{$card};

                if ( ( !defined($aref) ) or ( !@$aref ) )
                {
                    $aref = $old_resources_map{''};
                }
                my $i = shift(@$aref);

                $new_resources_indexes[$idx] = $i;
                if ( defined($i) )
                {
                    delete( $non_assigned_resources{$i} );
                }
            }

            my @non_assigned_resources_list =
                sort { $a <=> $b } keys(%non_assigned_resources);

            foreach my $resource_idx (@new_resources_indexes)
            {
                if ( !defined($resource_idx) )
                {
                    $resource_idx = shift(@non_assigned_resources_list);
                }
            }

            return \@new_resources_indexes;
        };

        my $new_cols_indexes = $populate_new_resource_indexes->(
            \@cols_iter,
            sub {
                my ( $state, $idx ) = @_;
                my $col = $state->get_column($idx);
                return ( $col->len ? $col->pos(0)->to_string() : '' );
            },
        );

        my $new_fc_indexes = $populate_new_resource_indexes->(
            \@fc_iter,
            sub {
                my ( $state, $idx ) = @_;
                my $card_obj = $state->get_freecell($idx);
                return ( defined($card_obj) ? $card_obj->to_string() : '' );
            },
        );

        my $verify_state = Games::Solitaire::Verify::State->new(
            {
                variant        => 'custom',
                variant_params => $self->_variant_params(),
            }
        );

        foreach my $idx (@cols_iter)
        {
            $verify_state->add_column(
                $running_state->get_column( $new_cols_indexes->[$idx] )->clone()
            );
        }

        $verify_state->set_freecells(
            Games::Solitaire::Verify::Freecells->new(
                {
                    count => $running_state->num_freecells(),
                }
            )
        );

        foreach my $idx (@fc_iter)
        {
            my $card_obj =
                $running_state->get_freecell( $new_fc_indexes->[$idx] );

            if ( defined($card_obj) )
            {
                $verify_state->set_freecell( $idx, $card_obj->clone() );
            }
        }

        $verify_state->set_foundations( $running_state->_foundations->clone() );

        my $v_s = $verify_state->to_string();
        my $n_s = $new_state->to_string();
        if ( $v_s ne $n_s )
        {
            die "States mismatch:\n<<\n$v_s\n>>\n vs:\n<<\n$n_s\n>>\n.";
        }

        @cols_indexes = @$new_cols_indexes;
        @fc_indexes   = @$new_fc_indexes;
    }

    print "This game is solveable.\n";

    close($fh);
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
