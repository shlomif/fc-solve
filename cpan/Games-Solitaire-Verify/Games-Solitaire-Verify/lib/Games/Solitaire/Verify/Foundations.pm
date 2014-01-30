package Games::Solitaire::Verify::Foundations;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Foundations - a class for representing the
foundations (or home-cells) in a Solitaire game.

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.1203';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

use List::Util qw(first);

__PACKAGE__->mk_acc_ref([qw(
    _num_decks
    _founds
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Foundations;

    # For internal use.

=head1 FUNCTIONS

=cut

sub _input_from_string
{
    my $self = shift;
    my $str = shift;

    my $rank_re = '[0A1-9TJQK]';

    if($str !~ m{\AFoundations: H-($rank_re) C-($rank_re) D-($rank_re) S-($rank_re) *\z}ms)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Foundations->throw(
            error => "Wrong Foundations",
        );
    }
    {
        my @founds_strings = ($1, $2, $3, $4);

        foreach my $suit (@{$self->_get_suits_seq()})
        {
            $self->assign($suit, 0,
                Games::Solitaire::Verify::Card->calc_rank_with_0(
                        shift(@founds_strings)
                    )
                );
        }
    }
}

sub _init
{
    my ($self, $args) = @_;

    if (! exists($args->{num_decks}))
    {
        die "No number of decks were specified";
    }

    $self->_num_decks($args->{num_decks});

    $self->_founds(
        +{
            map
            { $_ => [(0) x $self->_num_decks()], }
            @{$self->_get_suits_seq()}
        }
    );

    if (exists($args->{string}))
    {
        $self->_input_from_string($args->{string});
    }

    return;
}

sub _get_suits_seq
{
    my $class = shift;

    return Games::Solitaire::Verify::Card->get_suits_seq();
}

=head2 $self->value($suit, $index)

Returns the card in the foundation $suit with the index $index .

=cut

sub value
{
    my ($self, $suit, $idx) = @_;

    return $self->_founds()->{$suit}->[$idx];
}

=head2 $self->assign($suit, $index, $rank)

Sets the value of the foundation with the suit $suit and the
index $index to $rank .

=cut

sub assign
{
    my ($self, $suit, $idx, $rank) = @_;

    $self->_founds()->{$suit}->[$idx] = $rank;

    return;
}

=head2 $self->increment($suit, $index)

Increments the value of the foundation with the suit $suit and the
index $index to $rank .

=cut

sub increment
{
    my ($self, $suit, $idx) = @_;

    $self->_founds()->{$suit}->[$idx]++;

    return;
}

=head2 $self->to_string()

Stringifies the freecells into the Freecell Solver solution display notation.

=cut

sub _foundations_strings
{
    my $self = shift;

    return [
        map {
            sprintf(qq{ %s-%s},
                $_,
                Games::Solitaire::Verify::Card->rank_to_string(
                    $self->value($_, 0)
                ),
            )
        }
        (@{$self->_get_suits_seq()})
    ];
}

sub to_string
{
    my $self = shift;

    return   "Foundations:"
           . join("", @{$self->_foundations_strings()})
           . " " # We need the trailing space for compatibility with
                 # Freecell Solver.
           ;
}


=head2 $board->clone()

Returns a clone of the freecells, with all of their cards duplicated.

=cut

sub clone
{
    my $self = shift;

    my $copy =
        __PACKAGE__->new(
            {
                num_decks => $self->_num_decks(),
            }
        );

    foreach my $suit (@{$self->_get_suits_seq()})
    {
        foreach my $deck_idx (0 .. ($self->_num_decks()-1))
        {
            $copy->assign(
                $suit, $deck_idx,
                $self->value($suit, $deck_idx),
            );
        }
    }

    return $copy;
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
