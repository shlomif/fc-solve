package Games::Solitaire::Verify::Column;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Column - a class wrapper for Solitaire
columns that are composed of a sequence of cards.

=cut

our $VERSION = '0.1601';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

__PACKAGE__->mk_acc_ref([qw(
    _cards
    _s
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

=head1 METHODS

=cut

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

    $self->_recalc;

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

        $self->_recalc;
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

    $self->_recalc;

    return;
}

=head2 $column->push($card)

Appends a single card to the top of the column.

=cut

sub push
{
    my ($self, $card) = @_;

    push @{$self->_cards()}, $card;

    $self->_recalc;

    return;
}

=head2 my $card_at_top = $column->pop()

Pops a card from the top of the column and returns it.

=cut

sub pop
{
    my $self = shift;

    my $card = pop(@{$self->_cards()});

    $self->_recalc;

    return $card;
}

=head2 $column->to_string()

Converts to a string.

=cut

sub _recalc
{
    my $self = shift;

    $self->_s(
        join(' ',
            ':',
            (map
                { $_->fast_s() }
                @{$self->_cards()}
            )
        )
    );

    return;
}

sub to_string
{
    return shift->_s;
}

1; # End of Games::Solitaire::Verify::Column
