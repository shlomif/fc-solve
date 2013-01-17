package AI::Pathfinding::OptimizeMultiple::SimulationResults;

use MooX qw/late/;

has status => (isa => 'Str', is => 'ro', required => 1,);
has total_iters => (isa => 'Int', is => 'ro', required => 1,);
has scan_runs => (isa => 'ArrayRef[AI::Pathfinding::OptimizeMultiple::ScanRun]', is => 'ro', required => 1,);

sub get_total_iters
{
    return shift->total_iters();
}

sub get_status
{
    return shift->status();
}

1;

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple::SimulationResults - the simulation results.

=head1 VERSION

version 0.0.1

=head1 SYNOPSIS

=head1 DESCRIPTION

A class for simulation results.

=head1 SLOTS

=head2 $scan_run->status()

The status.

=head2 $scan_run->total_iters()

The total iterations count so far.

=head2 my $aref = $self->scan_runs()

An array reference of L<AI::Pathfinding::OptimizeMultiple::ScanRun> .

=head1 METHODS

=head2 $self->get_total_iters()

Returns the total iterations.

=head2 $self->get_status()

Returns the status.

=head1 VERSION

version 0.0.1

=head1 SEE ALSO

L<AI::Pathfinding::OptimizeMultiple> , L<AI::Pathfinding::OptimizeMultiple::ScanRun> .

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/> .

=cut
