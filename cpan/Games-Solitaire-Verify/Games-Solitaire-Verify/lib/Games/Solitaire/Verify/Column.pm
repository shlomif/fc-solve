package Games::Solitaire::Verify::Column;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Column - a class wrapper for Solitaire
columns that are composed of a sequence of cards.

=head1 VERSION

Version 0.0101

=cut

our $VERSION = '0.1201';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

__PACKAGE__->mk_acc_ref([qw(
    _cards
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Column;

    # Initialise a column
    my $column = Games::Solitaire::Verify::Column->new(
        {
            string => ": KH QS 5C",
        },
    );

    # Prints 3
    print $column->len();

    my $queen_card = $column->pos(1);

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

sub _from_string
{
    my ($self, $str) = @_;

    if ($str !~ s{\A:(?: )?}{})
    {
        Games::Solitaire::Verify::Exception::Parse::Column::Prefix->throw(
            error => "String does not start with \": \"",
        );
    }

    # Ignore trailing whitespace, so we don't have -1.
    my @cards = split(/ +/, $str);

    $self->_cards(
        [
            map
            {
                Games::Solitaire::Verify::Card->new(
                    {string => $_ }
                )
            }
            @cards
        ]
    );

    return;
}

sub _init
{
    my ($self, $args) = @_;

    if (exists($args->{string}))
    {
        return $self->_from_string($args->{string});
    }
    elsif (exists($args->{cards}))
    {
        $self->_cards($args->{cards});
        return;
    }
    else
    {
        die "Cannot init - no 'string' or 'cards' specified."
    }
}

=head2 $column->len()

Returns an integer representing the number of cards in the column.

=cut

sub len
{
    my $self = shift;

    return scalar(@{$self->_cards()});
}

=head2 $column->pos($idx)

Returns the card (a L<Games::Solitaire::Verify::Card> object)
at position $idx in Column. $idx starts at 0.

=cut

sub pos
{
    my $self = shift;
    my $idx = shift;

    return $self->_cards->[$idx];
}

=head2 $column->top()

Returns the top card.

=cut

sub top
{
    my $self = shift;

    return $self->pos(-1);
}

=head2 $column->clone()

Returns a clone of the column.

=cut

sub clone
{
    my $self = shift;

    my $new_col = Games::Solitaire::Verify::Column->new(
        {
            cards => [ map { $_->clone() } @{$self->_cards()} ],
        }
    );

    return $new_col;
}

=head2 $base_column->append($column_with_more_cards)

Appends the column $column_with_more_cards to $base_column .

=cut

sub append
{
    my ($self, $more_cards) = @_;

    my $more_copy = $more_cards->clone();

    push @{$self->_cards()}, @{$more_copy->_cards()};
}

=head2 $column->push($card)

Appends a single card to the top of the column.

=cut

sub push
{
    my ($self, $card) = @_;

    push @{$self->_cards()}, $card;
}

=head2 my $card_at_top = $column->pop()

Pops a card from the top of the column and returns it.

=cut

sub pop
{
    my $self = shift;

    return pop(@{$self->_cards()});
}

=head2 $column->to_string()

Converts to a string.

=cut

sub to_string
{
    my $self = shift;

    return ":" .
        ($self->len()
            ? $self->_non_zero_cards_string()
            : " " # We need the single trailing space for
                  # Freecell Solver compatibility
        );
}

sub _non_zero_cards_string
{
    my $self = shift;

    return join("",
            (map
                { " " . $self->pos($_)->to_string() }
                (0 .. ($self->len()-1))
            )
        );
}

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>.

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify::Column


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
