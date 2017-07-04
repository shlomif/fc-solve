package FC_Solve::DeltaStater::OptionsStruct;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

my $UNKNOWN    = 0;
my $IMPOSSIBLE = 1;
my $TRUE       = 2;

use List::MoreUtils qw(firstidx);

__PACKAGE__->mk_acc_ref( [qw(_options _which_option _num_unknowns)] );

sub _init
{
    my ( $self, $args ) = @_;

    my $count = $args->{count};

    $self->_options( [ ($UNKNOWN) x $count ] );

    $self->_which_option(-1);

    $self->_num_unknowns($count);

    return;
}

sub get_option
{
    my ( $self, $idx ) = @_;

    return $self->_options->[$idx];
}

sub _set_option
{
    my ( $self, $idx, $val ) = @_;

    if ( ( $val != $TRUE ) && ( $val != $IMPOSSIBLE ) )
    {
        Carp::confess "Must be true or impossible.";
    }

    if ( $self->get_option($idx) != $UNKNOWN )
    {
        Carp::confess "Option was already set.";
    }

    $self->_options->[$idx] = $val;
}

sub get_verdict
{
    my ($self) = @_;

    return $self->_which_option;
}

sub mark_as_impossible
{
    my $self = shift;
    my $idx  = shift;

    if ( $self->_which_option() >= 0 )
    {
        Carp::confess "Already decided!";
    }

    if ( $self->get_option($idx) != $UNKNOWN )
    {
        Carp::confess "Already set.";
    }

    $self->_set_option( $idx, $IMPOSSIBLE );
    $self->_num_unknowns( $self->_num_unknowns() - 1 );

    if ( $self->_num_unknowns() == 1 )
    {
        $self->_which_option(
            firstidx { $self->get_option($_) == $UNKNOWN }
            ( 0 .. $#{ $self->_options() } )
        );
        $self->_set_option( $self->_which_option(), $TRUE );
        $self->_num_unknowns(0);
    }

    return;
}

sub mark_as_true
{
    my $self       = shift;
    my $option_idx = shift;

    if ( $self->_which_option() >= 0 )
    {
        Carp::confess "Already decided!";
    }

    if ( $self->get_option($option_idx) != $UNKNOWN )
    {
        Carp::confess "Already set.";
    }

    $self->_set_option( $option_idx, $TRUE );
    $self->_which_option($option_idx);
    $self->_num_unknowns(0);

    foreach my $idx ( 0 .. $#{ $self->_options } )
    {
        if ( $self->get_option($idx) == $UNKNOWN )
        {
            $self->_set_option( $idx, $IMPOSSIBLE );
        }
    }

    return;
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
