package Games::Solitaire::VerifySolution::Card;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::VerifySolution::Card - a class wrapper for an individual 
Solitaire card.

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.01';

use base 'Games::Solitaire::VerifySolution::Base';

use Games::Solitaire::VerifySolution::Exception;

__PACKAGE__->mk_accessors(qw(
    rank
    suit
    _game
    ));

=head1 SYNOPSIS

    use Games::Solitaire::VerifySolution::Card;

    # Initialise a Queen-of-Hearts
    my $queen_of_hearts = Games::Solitaire::VerifySolution::Card->new(
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

my %suits_map =
(
    "H" => { name => "hearts",   color => "red", },
    "S" => { name => "spades",   color => "black", },
    "C" => { name => "clubs",    color => "black", },
    "D" => { name => "diamonds", color => "red", },
);

=head2 $class->calc_rank($rank_string)

Calculates the numerical rank of the string passed as argument.

Example:

    my $ten = Games::Solitaire::VerifySolution::Card->calc_rank("T")
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
        Games::Solitaire::VerifySolution::Exception::Parse::Card->throw(
            error => "string length is too long",
        );
    }

    my ($rank, $suit) = split(//, $str);

    if (! defined($self->rank($self->calc_rank($rank))))
    {
        Games::Solitaire::VerifySolution::Exception::Parse::Card::UnknownRank->throw(
            error => "unknown rank",
        );
    }

    if (exists($suits_map{$suit}))
    {
        $self->suit($suit);
    }
    else
    {
        Games::Solitaire::VerifySolution::Exception::Parse::Card::UnknownSuit->throw(
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

    my $new_card = Games::Solitaire::VerifySolution::Card->new();

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

    return $card_nums[$self->rank()-1]->{t} . $self->suit();
}

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-VerifySolution>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::VerifySolution::Card


You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-VerifySolution>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-VerifySolution>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-VerifySolution>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-VerifySolution>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish, all rights reserved.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::VerifySolution::Move
