package FC_Solve::ShaAndLen;

use strict;
use warnings;

use Digest::SHA ();

sub _hasher
{
    my $self = shift;

    if (@_)
    {
        $self->{_hasher} = shift;
    }

    return $self->{_hasher};
}

sub new
{
    my $class = shift;
    my $self  = bless {}, $class;
    $self->_init(@_);
    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->{_len} = 0;
    $self->_hasher( Digest::SHA->new( $args->{name} // 256 ) );

    return;
}

sub ref_add
{
    my ( $self, $str ) = @_;

    $self->{_len} += length($$str);
    $self->_hasher->add($$str);

    return;
}

sub add
{
    my ( $self, $str ) = @_;

    return $self->ref_add( \$str );

    return;
}

sub len
{
    return shift->{_len};
}

sub hexdigest
{
    return shift->_hasher->clone()->hexdigest();
}

sub _unity
{
    return shift;
}

sub add_file
{
    my ( $self, $fh ) = @_;
    return $self->add_processed_slurp( $fh, \&_unity );
}

sub add_processed_slurp
{
    my ( $self, $fh, $cb ) = @_;
    return $self->ref_add(
        scalar $cb->(
            \(
                do { local $/; scalar <$fh>; }
            ),
        )
    );
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
