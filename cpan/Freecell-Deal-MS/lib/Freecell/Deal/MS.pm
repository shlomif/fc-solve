package Freecell::Deal::MS;

use strict;
use warnings;

use Math::RNG::Microsoft::FCPro ();

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
    Math::RNG::Microsoft::FCPro->new( seed => scalar( $self->deal ) )
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
=head1 DESCRIPTION

=head1 METHODS

=head2 Freecell::Deal::MS->new(deal => 11982)

Constructor.

=head2 $obj->as_str()

Returns the deal layout as a string.

=head2 $obj->deal()

B<For internal use!>

=head1 SEE ALSO

L<https://github.com/shlomif/fc-solve/blob/master/fc-solve/source/board_gen/make_multi_boards.c>

L<https://pypi.org/project/pysol-cards/>

=cut
