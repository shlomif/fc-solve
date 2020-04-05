package FC_Solve::VarBaseDigitsWriter;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

use Math::BigInt try => 'GMP';

use Carp ();

__PACKAGE__->mk_acc_ref( [qw(_data _multiplier)] );

sub _init
{
    my $self = shift;

    $self->_data( Math::BigInt->new(0) );
    $self->_multiplier( Math::BigInt->new(1) );

    return;
}

sub write
{
    my ( $self, $args ) = @_;

    my $base = $args->{base};
    my $item = $args->{item};

    if ( $item < 0 )
    {
        Carp::confess("Item '$item' cannot be less than 0.");
    }
    elsif ( $item >= $base )
    {
        Carp::confess("Base '$base' must be greater than item '$item'");
    }

    $self->_data( $self->_data() + $self->_multiplier() * $item );

    $self->_multiplier( $self->_multiplier() * $base );

    return;
}

sub get_data
{
    my $self = shift;

    return $self->_data()->copy();
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
