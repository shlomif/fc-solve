package AI::Pathfinding::OptimizeMultiple::Scan;

use strict;
use warnings;

use 5.012;

use MooX qw/late/;

our $VERSION = '0.0.13';

has cmd_line => (isa => 'Str', is => 'ro', required => 1,);
has id => (isa => 'Str', is => 'ro', required => 1,);
has used => (isa => 'Bool', is => 'rw', default => sub { 0; });

sub mark_as_used
{
    my $self = shift;
    $self->used(1);
}

sub is_used
{
    my $self = shift;
    return $self->used();
}


sub data_file_path
{
    my $self = shift;

    return "./data/" . $self->id() .  ".data.bin";
}


1;

__END__

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple::Scan

=head1 SLOTS

=head2 $scan->cmd_line()

The command line string, which defines the scan's behaviour - required upon
initialization.

=head2 $scan->id()

The scan ID - a string.

=head2 $scan->used()

A boolean - whether the scan was used.

=head1 METHODS

=head2 $self->mark_as_used()

Mark the scan as used.

=head2 $self->is_used()

Returns whether the scan was used.

=head2 $scan->data_file_path()

Returns the path to the data file of the scan, where the numbers of iterations
per board are stored.

=cut
