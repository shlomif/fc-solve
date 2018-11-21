package FC_Solve::VarBaseDigitsReader;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref( [qw(_data)] );

sub _init
{
    my ( $self, $args ) = @_;
    $self->_data( $args->{data} );
    return;
}

sub read
{
    my ( $self, $base ) = @_;

    my $data = $self->_data;

    my $ret = $data % $base;

    my $new_data = $data / $base;

    $self->_data($new_data);

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
