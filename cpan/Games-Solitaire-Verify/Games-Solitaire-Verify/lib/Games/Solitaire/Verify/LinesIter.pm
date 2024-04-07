package Games::Solitaire::Verify::LinesIter;

use strict;
use warnings;
use autodie;

sub new
{
    my $class = shift;

    my $self = bless {}, $class;

    $self->_init(@_);

    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->{_get}      = $args->{_get};
    $self->{_line_num} = 0;

    return;
}

sub _get_line
{
    my ( $self, ) = @_;

    my $ret = $self->{_get}->();
    return ( $ret, ++$self->{_line_num} );
}

sub _assert_empty_line
{
    my ( $self, ) = @_;

    my ( $s, $line_idx ) = $self->_get_line;

    if ( $s ne '' )
    {
        Carp::confess("Line '$line_idx' is not empty, but '$s'");
    }

    return;
}

sub _compare_line
{
    my ( $self, $wanted_line, $title, ) = @_;

    my ( $line, $line_idx ) = $self->_get_line;
    if ( $line ne $wanted_line )
    {
        Carp::confess(
            "$title string is '$line' vs. '$wanted_line' at line no. $line_idx"
        );
    }

    return;
}

1;

__END__

=head1 NAME

Games::Solitaire::Verify::LinesIter - a lines' iterator
to keep track of line numbers.

=head1 METHODS

=head2 Games::Solitaire::Verify::LinesIter->new()

Construct a new lines iterator ( B<FOR INTERNAL USE!!> )

=cut
