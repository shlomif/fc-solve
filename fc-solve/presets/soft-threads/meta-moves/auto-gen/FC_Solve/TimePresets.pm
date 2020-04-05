package FC_Solve::TimePresets;

use strict;
use warnings;

use MooX qw/ late /;

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

use PDL ();

has start_board => ( is => 'ro', isa => 'Int', default => 1 );
has num_boards  => ( is => 'ro', isa => 'Int', default => 32000 );
has input_obj   => (
    is      => 'ro',
    isa     => 'AI::Pathfinding::OptimizeMultiple::DataInputObj',
    lazy    => 1,
    default => sub {
        my ($self) = @_;

        return AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
            {
                start_board => $self->start_board,
                num_boards  => $self->num_boards,
            }
        );
    },
    handles => [
        qw(
            _get_scan_cmd_line
            get_next_id
            get_scans_lens_iters_pdls
            get_scans_iters_pdls
            get_scan_ids_aref
            lookup_scan_idx_based_on_id
            selected_scans
            time_scan
            )
    ]
);

sub calc_params_from_environment
{
    my ($self) = @_;

    my %params;
    if ( exists( $ENV{FC_NUM} ) )
    {
        $params{freecells_num} = $ENV{FC_NUM};
    }
    if ( exists( $ENV{FC_VARIANT} ) )
    {
        $params{variant} = $ENV{FC_VARIANT};
    }

    return \%params;
}

sub calc_scans_lens_data
{
    my $self = shift;

    my $data_hash_ref = $self->get_scans_lens_iters_pdls();

    my $scans_lens_data =
        PDL::cat( @{$data_hash_ref}{ @{ $self->get_scan_ids_aref } } )
        ->xchg( 1, 3 )->clump( 2 .. 3 );

    return $scans_lens_data;
}

sub calc_scans_data_wo_lens
{
    my $self = shift;

    my $data_hash_ref = $self->get_scans_iters_pdls();

    my $scans_data =
        PDL::cat( @{$data_hash_ref}{ @{ $self->get_scan_ids_aref } } );

    return $scans_data;
}

1;

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2014 Shlomi Fish

=cut
