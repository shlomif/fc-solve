package AI::Pathfinding::OptimizeMultiple::PostProcessor;

use strict;
use warnings;

use 5.012;

use MooX qw/late/;

has _should_do_rle => (isa => 'Bool', is => 'ro', init_arg => 'do_rle', required => 1);
has _offset_quotas => (isa => 'Bool', is => 'ro', init_arg => 'offset_quotas', required => 1);

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

AI::Pathfinding::OptimizeMultiple::PostProcessor - post-processor.

=head1 SUBROUTINES/METHODS

=head2 $self->scans_rle()

For internal use.

=head2 $self->process($scans_orig)

For internal use.

=cut
