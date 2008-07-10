package Games::Solitaire::Verify::Freecells;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Freecells - a class for representing the
Freecells in games such as Freecell, Baker's Game, or Seahaven Towers

=head1 VERSION

Version 0.0101

=cut

our $VERSION = '0.0101';

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

use List::Util qw(first);

__PACKAGE__->mk_accessors(qw(
    _count
    _cells
    ));

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Freecells;

    # For internal use.

=head1 FUNCTIONS

=cut

sub _input_from_string
{
    my $self = shift;
    my $str = shift;

    if ($str !~ m{\AFreecells:}gms)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Freecells->throw(
            error => "Wrong Freecell String",
        );
    }

    for my $pos (0 .. ($self->count()-1))
    {
        if ($str =~ m{\G  (..)}gms)
        {
            my $card_s = $1;
            $self->assign($pos, $self->_parse_freecell_card($card_s))
        }
        else
        {
            Games::Solitaire::Verify::Exception::Parse::State::Freecells->throw(
                error => "Wrong Freecell String",
            );
        }
    }
}

sub _init
{
    my ($self, $args) = @_;

    if (! exists($args->{count}))
    {
        die "The count was not specified for the freecells";
    }

    $self->_count($args->{count});

    $self->_cells([(undef) x $self->_count()]);

    if (exists($args->{string}))
    {
        return $self->_input_from_string($args->{string});
    }

    return;
}

sub _parse_freecell_card
{
    my ($self, $s) = @_;

    return
    (
        ($s eq q{  })
            ? undef()
            : Games::Solitaire::Verify::Card->new(
                {
                    string => $s,
                }
            )
    );
}

=head2 $self->count()

Returns the number of cells.

=cut

sub count
{
    my $self = shift;

    return $self->_count();
}

=head2 $self->cell($index)

Returns the card in the freecell with the index $index .

=cut

sub cell
{
    my ($self, $idx) = @_;

    return $self->_cells()->[$idx];
}

=head2 $self->assign($index, $card)

Sets the card in the freecell with the index $index to $card, which
should be a L<Games::Solitaire::Verify::Card> object.

=cut

sub assign
{
    my ($self, $idx, $card) = @_;

    $self->_cells()->[$idx] = $card;

    return;
}

=head2 $self->to_string()

Stringifies the freecells into the Freecell Solver solution display notation.

=cut

sub to_string
{
    my $self = shift;

    return "Freecells:" . join("",
        map { "  " . (defined($_) ? $_->to_string() : "  ") }
        map { $self->cell($_) } 
        (0 .. ($self->count()-1))
    );
}

=head2 $self->cell_clone($pos)

Returns a B<clone> of the card in the position $pos .

=cut

sub cell_clone
{
    my ($self, $pos) = @_;

    my $card = $self->cell($pos);

    return defined($card) ? $card->clone() : undef();
}

=head2 $self->clear($pos)

Clears/empties the freecell at position $pos .

=cut

sub clear
{
    my ($self, $pos) = @_;

    undef($self->_cells()->[$pos]);

    return;
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
                count => $self->count(),
            }
        );

    foreach my $pos (0 .. ($self->count()-1))
    {
        $copy->assign($pos, $self->cell_clone($pos));
    }

    return $copy;
}

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

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

Copyright 2008 Shlomi Fish, all rights reserved.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify::Move
