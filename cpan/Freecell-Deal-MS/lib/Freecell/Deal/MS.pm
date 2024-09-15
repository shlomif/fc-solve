package Freecell::Deal::MS;

use strict;
use warnings;

use Math::RNG::Microsoft::FCPro ();

use Class::XSAccessor {
    constructor => 'new',
    accessors   => [qw(deal)],
};

my @INITIAL_CARDS = (
    map {
        my $s = $_;
        map { $s . $_ } qw/C D H S/;
    } ( 'A', ( 2 .. 9 ), 'T', 'J', 'Q', 'K' )
);

sub as_columns_array
{
    my ($self) = @_;

    my @cards = @INITIAL_CARDS;
    Math::RNG::Microsoft::FCPro->new( seed => scalar( $self->deal ) )
        ->shuffle( \@cards );
    my @lines = ( map { [] } 0 .. 7 );
    my $i     = -1;
    while (@cards)
    {
        push @{ $lines[ ( ( ++$i ) & 7 ) ] }, pop(@cards);
    }
    my $rec = { array_of_arrays_of_strings => \@lines, };
    return $rec;
}

sub as_str
{
    my ($self) = @_;

    my @cards = @INITIAL_CARDS;
    Math::RNG::Microsoft::FCPro->new( seed => scalar( $self->deal ) )
        ->shuffle( \@cards );
    my @lines = ( map { [ ':', ] } 0 .. 7 );
    my $i     = -1;
    while (@cards)
    {
        push @{ $lines[ ( ( ++$i ) & 7 ) ] }, pop(@cards);
    }
    my $str = join "", map { "@$_\n" } @lines;
    return $str;
}

1;

__END__

=encoding utf8

=head1 NAME

Freecell::Deal::MS - deal Windows FreeCell / FC Pro layouts

=head1 SYNOPSIS

    use Test::More tests => 2;

    use Freecell::Deal::MS ();

    {

        my $obj = Freecell::Deal::MS->new( deal => 240 );

        # TEST
        ok( $obj, "object" );

        # TEST
        is( $obj->as_str(), <<'EOF', 'as_str 240' );
    : JH 9C 5S KC 6S 2H AS
    : 5D 3D 9S 2S 3C AD 8C
    : 8S 5C KD QC 3H 4D 3S
    : 7S AC 9H 6C QH KS 4H
    : KH JD 7D 4C 8H 6H
    : TS TC 4S 5H QD JS
    : 9D JC 2C QS TH 2D
    : AH 7C 6D 8D TD 7H
    EOF

    }

=head1 DESCRIPTION

Deals FC Pro deals: L<http://rosettacode.org/wiki/Deal_cards_for_FreeCell>
and L<https://fc-solve.shlomifish.org/faq.html#what_are_ms_deals> .

=head1 METHODS

=head2 Freecell::Deal::MS->new(deal => 11982)

Constructor.

=head2 $obj->as_str()

Returns the deal layout as a string.

=head2 $obj->as_columns_array()

Returns the deal layout as hash reference with a key 'array_of_arrays_of_strings'
which points to an array of 8 columns.

( Available since version 0.6.0 .  )

=head2 $obj->deal()

B<For internal use!>

=head1 SEE ALSO

L<https://github.com/shlomif/fc-solve/blob/master/fc-solve/source/board_gen/make_multi_boards.c>

L<https://pypi.org/project/pysol-cards/>

L<https://fc-solve.shlomifish.org/faq.html#what_are_ms_deals>

=cut
