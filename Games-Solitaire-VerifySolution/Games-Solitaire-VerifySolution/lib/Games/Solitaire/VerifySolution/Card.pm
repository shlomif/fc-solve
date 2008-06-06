package Games::Solitaire::VerifySolution::Card;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::VerifySolution::Card - a class wrapper for an individual 
Solitaire cards.

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

    use Games::Solitaire::VerifySolution::Move;

    my $move1 = Games::Solitaire::VerifySolution::Move->new(
        {
            fcs_string => "Move a card from stack 0 to the foundations",
            game => "freecell",
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

sub _from_string
{
    my ($self, $str) = @_;

    if (length($str) != 2)
    {
        die "Card string is longer than 2";
    }

    my ($rank, $suit) = split(//, $str);

    if (exists($ranks_map{$rank}))
    {
        $self->rank($ranks_map{$rank});
    }
    else
    {
        die "Unknown rank";
    }

    if (exists($suits_map{$suit}))
    {
        $self->suit($suit);
    }
    else
    {
        die "Unknown suit";
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

=head2 $card->color()

Returns "red" or "black" depending on the rank of the card.

=cut

sub color
{
    my ($self) = @_;

    return $suits_map{$self->suit()}->{'color'};
}

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-VerifySolution>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::VerifySolution


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
