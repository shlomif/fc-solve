package AI::Pathfinding::OptimizeMultiple::Structs::Scan;

use strict;
use warnings;

use parent 'AI::Pathfinding::OptimizeMultiple::Base';

use vars (qw(@fields %fields_map));
@fields = (qw(
    cmd_line
    id
    used
));

%fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_acc_ref(\@fields);

sub _init
{
    my $self = shift;
    my (%args) = @_;
    $self->used(0);
    foreach my $field (grep {exists($fields_map{$_})} keys(%args))
    {
        $self->$field($args{$field});
    }

    return 0;
}

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

=head2 $scan->data_file_path()

Returns the path to the data file of the scan, where the numbers of iterations
per board are stored.

=cut

sub data_file_path
{
    my $self = shift;

    return "./data/" . $self->id() .  ".data.bin";
}

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

1;

