package Games::Solitaire::Verify::Card;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Card - a class wrapper for an individual 
Solitaire card.

=head1 VERSION

Version 0.0101

=cut

our $VERSION = '0.09';

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;

__PACKAGE__->mk_acc_ref([qw(
    rank
    suit
    _game
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Card;

    # Initialise a Queen-of-Hearts
    my $queen_of_hearts = Games::Solitaire::Verify::Card->new(
        {
            string => "QH",
        },
    );

=head1 FUNCTIONS

=cut

sub _card_num_normalize
{
    my $arg = shift;

    if (ref($arg) eq "")
    {
        return +{ map { $_ => $arg } (qw(t non_t)) };
    }
    else
    {
        return $arg
    }
}

my @card_nums =  (map { _card_num_normalize($_) } 
    ("A", (2 .. 9), 
    {
        't' => "T",
        'non_t' => "10",
    },
    , "J", "Q", "K"));

my %ranks_map = (map { $card_nums[$_]->{t} => ($_+1) } (0 .. $#card_nums));

my @suits_map_proto =
(
    ["H" => { name => "hearts",   color => "red", },],
    ["C" => { name => "clubs",    color => "black", },],
    ["D" => { name => "diamonds", color => "red", },],
    ["S" => { name => "spades",   color => "black", },],
);

my %suits_map = (map {@$_} @suits_map_proto);

=head2 $class->get_suits_seq()

Returns the expected sequence of the suits - "H", "S", "C", "D".

=cut

sub get_suits_seq
{
    my $class = shift;

    return [map { $_->[0] } @suits_map_proto];
}

=head2 $class->calc_rank($rank_string)

Calculates the numerical rank of the string passed as argument.

Example:

    my $ten = Games::Solitaire::Verify::Card->calc_rank("T")
    # Prints 10.
    print "$ten\n";

=cut

sub calc_rank
{
    my ($self, $s) = @_;

    return $ranks_map{$s};
}

=head2 $class->calc_rank_with_0($rank_string)

Same as calc_rank only supporting "0" as the zero'th card.

=cut

sub calc_rank_with_0
{
    my ($self, $str) = @_;

    if ($str eq "0")
    {
        return 0;
    }
    else
    {
        return $self->calc_rank($str);
    }
}

sub _from_string
{
    my ($self, $str) = @_;

    if (length($str) != 2)
    {
        Games::Solitaire::Verify::Exception::Parse::Card->throw(
            error => "string length is too long",
        );
    }

    my ($rank, $suit) = split(//, $str);

    if (! defined($self->rank($self->calc_rank($rank))))
    {
        Games::Solitaire::Verify::Exception::Parse::Card::UnknownRank->throw(
            error => "unknown rank",
        );
    }

    if (exists($suits_map{$suit}))
    {
        $self->suit($suit);
    }
    else
    {
        Games::Solitaire::Verify::Exception::Parse::Card::UnknownSuit->throw(
            error => "unknown suit",
        );
    }
}

sub _init
{
    my ($self, $args) = @_;

    if (exists($args->{string}))
    {
        return $self->_from_string($args->{string});
    }
}


=head2 $card->rank()

Returns the rank of the card as an integer. Ace is 1, 2-10 are 2-20;
J is 11, Q is 12 and K is 13.

=head2 $card->suit()

Returns "H", "C", "D" or "S" depending on the suit.

=head2 $card->color()

Returns "red" or "black" depending on the rank of the card.

=cut

sub color
{
    my ($self) = @_;

    return $suits_map{$self->suit()}->{'color'};
}

=head2 my $copy = $card->clone();

Clones the card into a new copy.

=cut

sub clone
{
    my $self = shift;

    my $new_card = Games::Solitaire::Verify::Card->new();

    $new_card->suit($self->suit());
    $new_card->rank($self->rank());

    return $new_card;
}

=head2 $card->to_string()

Converts the card to a string representation.

=cut

sub to_string
{
    my $self = shift;

    return $self->rank_to_string($self->rank()) . $self->suit();
}

=head2 $class->rank_to_string($rank_idx)

Converts the rank to a string.

=cut

sub rank_to_string
{
    my ($class, $rank) = @_;

    if ($rank == 0)
    {
        return '0';
    }
    else
    {
        return $card_nums[$rank-1]->{t};
    }
}

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify::Card


You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-Verify>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-Verify>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-Verify>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-Verify>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify::Move
