package FC_Solve::ShaAndLen;

use strict;
use warnings;

use Digest::SHA ();

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _len
            _hasher
            )
    ]
);

sub new
{
    my $class = shift;
    my $self  = {};
    bless $self, $class;
    $self->_init(@_);
    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->_len(0);
    $self->_hasher( Digest::SHA->new(256) );

    return;
}

sub add
{
    my ( $self, $str ) = @_;

    $self->_len( $self->_len() + length($str) );
    $self->_hasher->add($str);

    return;
}

sub len
{
    return shift->_len();
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
    return $self->add(
        scalar $cb->(
            do { local $/; scalar <$fh>; }
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
