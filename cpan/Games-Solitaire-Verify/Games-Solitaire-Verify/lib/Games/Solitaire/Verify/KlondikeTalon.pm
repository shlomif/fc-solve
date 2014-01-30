package Games::Solitaire::Verify::KlondikeTalon;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::KlondikeTalon - a class for representing the
talon of Klondike-like games.

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.1203';

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

=head1 FUNCTIONS

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
        (map { $_->to_string()} reverse @{$self->_waste()}),
        '==>',
        (map { $_->to_string()} @{$self->_undealt_cards()}),
        '<==',
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

1; # End of Games::Solitaire::Verify::KlondikeTalon
