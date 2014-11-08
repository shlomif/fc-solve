package Games::Solitaire::Verify::KlondikeTalon;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::KlondikeTalon - a class for representing the
talon of Klondike-like games.

=cut

our $VERSION = '0.1400';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

use List::Util qw(first);

__PACKAGE__->mk_acc_ref([qw(
    _max_num_redeals
    _num_redeals_so_far
    _undealt_cards
    _waste
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::KlondikeTalon;

    # For internal use.

=head1 METHODS

=cut

sub _input_from_string
{
    my $self = shift;
    my $str = shift;

    if (my ($cards_str) = ($str =~ /\ATalon: (.*)\z/ms))
    {
        $self->_undealt_cards(
            [
                map { Games::Solitaire::Verify::Card->new(
                    {
                        string => $_,
                    }
                )
                }
                split /\s+/, $cards_str
            ]
        );
    }
    else
    {
        die "Wrong format - does not start with Talon.";
    }
}

sub _init
{
    my ($self, $args) = @_;

    $self->_max_num_redeals(
        $args->{max_num_redeals}
    );

    $self->_num_redeals_so_far(0);

    $self->_undealt_cards([]);
    $self->_waste([]);

    if (exists($args->{string}))
    {
        $self->_input_from_string($args->{string});
    }

    return;
}

=head2 $self->draw()

Draw a card from the undealt cards to the waste.

=cut

sub draw
{
    my $self = shift;

    if (! @{ $self->_undealt_cards() })
    {
        die "Cannot draw.";
    }

    push @{$self->_waste()}, shift( @{$self->_undealt_cards()} );

    return;
}

=head2 my $card = $self->extract_top()

Extract the top card and return it.

=cut

sub extract_top
{
    my $self = shift;

    if (! @{ $self->_waste() })
    {
        die "Cannot extract_top.";
    }

    return pop(@{$self->_waste()});
}

=head2 $self->redeal()

Redeal the talon after there are no undealt cards.

=cut

sub redeal
{
    my $self = shift;

    if (@{$self->_undealt_cards()})
    {
        die "Cannot redeal while there are remaining cards.";
    }

    if ($self->_num_redeals_so_far() == $self->_max_num_redeals())
    {
        die "Cannot redeal because maximal number exceeded.";
    }

    $self->_num_redeals_so_far($self->_num_redeals_so_far() + 1);

    push @{$self->_undealt_cards()}, @{$self->_waste()};

    $self->_waste([]);

    return;
}

=head2 my $string = $self->to_string()

Return a string representation of the talon.

=cut

sub to_string
{
    my $self = shift;

    return join(" ", "Talon:",
        (map { $_->fast_s() } reverse @{$self->_waste()}),
        '==>',
        (map { $_->fast_s() } @{$self->_undealt_cards()}),
        '<==',
    );
}

1; # End of Games::Solitaire::Verify::KlondikeTalon
