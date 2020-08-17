package FC_Solve::VarBaseDigitsReader;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

use Math::BigInt try => 'GMP';

__PACKAGE__->mk_acc_ref( [qw(_data)] );

sub _init
{
    my ( $self, $args ) = @_;
    my $buf    = $args->{data};
    my $n      = Math::BigInt->new(0);
    my $factor = 0;
    foreach my $byte ( split //, $buf, -1 )
    {
        $n |=
            ( Math::BigInt->new( ord($byte) ) << $factor );
        $factor += 8;
    }
    $self->_data($n);
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
