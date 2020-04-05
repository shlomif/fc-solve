package FC_Solve::DeltaStater::BitWriter;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref( [qw(_bits_ref _bit_idx)] );

sub _init
{
    my $self = shift;

    $self->_bits_ref( \( my $s = '' ) );

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

sub write
{
    my ( $self, $len, $data ) = @_;

    while ($len)
    {
        vec( ${ $self->_bits_ref() }, $self->_next_idx(), 1 ) = ( $data & 0b1 );
    }
    continue
    {
        $len--;
        $data >>= 1;
    }

    return;
}

sub get_bits
{
    my $self = shift;

    return ${ $self->_bits_ref() };
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
