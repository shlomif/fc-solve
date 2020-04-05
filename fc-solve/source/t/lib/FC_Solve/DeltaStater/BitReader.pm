package FC_Solve::DeltaStater::BitReader;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref( [qw(bits _bit_idx)] );

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->bits( $args->{bits} );

    $self->_bit_idx(0);

    return;
}

sub _next_idx
{
    my $self = shift;

    my $ret = $self->_bit_idx;

    $self->_bit_idx( $ret + 1 );

    return $ret;
}

sub read
{
    my ( $self, $len ) = @_;

    my $ret = 0;
    for my $idx ( 0 .. $len - 1 )
    {
        $ret |= ( vec( $self->bits(), $self->_next_idx(), 1 ) << $idx );
    }

    return $ret;
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2011 Shlomi Fish

=cut
