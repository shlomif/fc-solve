package AI::Pathfinding::OptimizeMultiple::ScanRun;

use strict;
use warnings;

use 5.012;

use MooX qw/late/;

our $VERSION = '0.0.7';

has iters => (isa => 'Int', is => 'rw', required => 1);
has scan_idx => (isa => 'Int', is => 'ro', required => 1);

sub clone
{
    my $self = shift;

    return ref($self)->new(
        {
            iters => $self->iters(),
            scan_idx => $self->scan_idx(),
        }
    );
}

1; # End of AI::Pathfinding::OptimizeMultiple::ScanRun;

__END__

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple::ScanRun - running scan_idx for certain
iterations.

=head1 SYNOPSIS

    printf "Run %s for %d iterations.\n",
        $scans[$scan_alloc->scan_idx], $scan_alloc->iters;

=head1 DESCRIPTION

A class for scan iterations.

=head1 SLOTS

=head2 $scan_run->scan_idx()

The index of the scan (not the ID/name).

=head2 $scan_run->iters()

Number of iterations it ran.

=head1 METHODS

=head2 my $copy = $scan_run->clone()

Clones the scan.

=head1 SEE ALSO

L<AI::Pathfinding::OptimizeMultiple> .

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/> .

=head1 ACKNOWLEDGEMENTS

B<popl> from Freenode's #perl for trying to dig some references to an existing
algorithm in the scientific literature.

=cut
