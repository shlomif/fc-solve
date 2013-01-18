package AI::Pathfinding::OptimizeMultiple::Scan;

use strict;
use warnings;

use MooX qw/late/;

our $VERSION = '0.0.3';

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

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2010 Shlomi Fish

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
