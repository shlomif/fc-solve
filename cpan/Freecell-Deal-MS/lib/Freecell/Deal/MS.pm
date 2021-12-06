package Freecell::Deal::MS;

use strict;
use warnings;

use Math::RNG::Microsoft ();

use Class::XSAccessor {
    constructor => 'new',
    accessors   => [qw(deal)],
};

sub as_str
{
    my ($self) = @_;

    my @cards = (
        map {
            my $s = $_;
            map { $s . $_ } qw/C D H S/;
        } ( 'A', ( 2 .. 9 ), 'T', 'J', 'Q', 'K' )
    );
    Math::RNG::Microsoft->new( seed => scalar( $self->deal ) )
        ->shuffle( \@cards );
    my @lines = ( map { [ ':', ] } 0 .. 7 );
    my $i     = 0;
    while (@cards)
    {
        push @{ $lines[$i] }, pop(@cards);
        $i = ( ( $i + 1 ) & 7 );
    }
    my $str = join "", map { "@$_\n" } @lines;
    return $str;
}

1;

=encoding utf8

=head1 NAME

Freecell::Deal::MS - deal Windows FreeCell / FC Pro layouts

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 METHODS

=head2 Freecell::Deal::MS->new(deal => 11982)

Constructor.

=head2 $obj->as_str()

Returns the deal layout as a string.

=head2 $obj->deal()

B<For internal use!>
=cut
