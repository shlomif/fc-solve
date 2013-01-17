package AI::Pathfinding::OptimizeMultiple::PostProcessor;

use strict;
use warnings;

use parent 'AI::Pathfinding::OptimizeMultiple::Base';

__PACKAGE__->mk_acc_ref([qw(
    _should_do_rle
    _offset_quotas
)]);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->_should_do_rle($args->{do_rle});
    $self->_offset_quotas($args->{offset_quotas});

    return $self;
}




sub scans_rle
{
    my $self = shift;

    my @scans_list = @{shift()};

    my $scan = shift(@scans_list);

    my (@a);
    while (my $next_scan = shift(@scans_list))
    {
        if ($next_scan->scan_idx() == $scan->scan_idx())
        {
            $scan->iters( $scan->iters() + $next_scan->iters() );
        }
        else
        {
            push @a, $scan;
            $scan = $next_scan;
        }
    }
    push @a, $scan;
    return \@a;
}


sub process
{
    my $self = shift;

    my $scans_orig = shift;

    # clone the scans.
    my $scans = [ map { $_->clone(); } @{$scans_orig}];

    if ($self->_offset_quotas)
    {
        $scans =
        [
            map { my $ret = $_->clone(); $ret->iters($ret->iters()+1); $ret; }
            @$scans
        ];
    }

    if ($self->_should_do_rle)
    {
        $scans = $self->scans_rle($scans);
    }

    return $scans;
}


1;

__END__

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple::PostProcessor

=head1 NAME

AI::Pathfinding::OptimizeMultiple::PostProcessor

=head1 NAME

AI::Pathfinding::OptimizeMultiple::PostProcessor - post-processor.

=head1 VERSION

Version 0.0.1

=head1 SUBROUTINES/METHODS

=head2 $self->scans_rle()

For internal use.

=head2 $self->process($scans_orig)

For internal use.

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
