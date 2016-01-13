package Games::Solitaire::Verify::Freecells;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Freecells - a class for representing the
Freecells in games such as Freecell, Baker's Game, or Seahaven Towers

=cut

our $VERSION = '0.1601';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

use List::Util qw(first);

__PACKAGE__->mk_acc_ref([qw(
    _count
    _cells
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Freecells;

    # For internal use.

=head1 METHODS

=cut

sub _input_from_string
{
    my $self = shift;
    my $str = shift;

    if ($str !~ m{\AFreecells:}g)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Freecells->throw(
            error => "Wrong Freecell String",
        );
    }

    POS:
    for my $pos (0 .. ($self->count()-1))
    {
        if ($str =~ m{\G\z}cg)
        {
            last POS;
        }
        elsif ($str =~ m{\G  (..)}g)
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

    my $ret = "Freecells:" . (($self->count() == 0) ? "" :
        join("",
            map { "  " . (defined($_) ? $_->fast_s() : "  ") }
            map { $self->cell($_) }
            (0 .. ($self->count()-1))
        ));
    $ret =~ s# +\z##;

    return $ret;
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

=head2 $self->num_empty()

Returns the number of empty freecells.

=cut

sub num_empty
{
    my $self = shift;

    my $count = 0;

    foreach my $fc_idx (0 .. ($self->count()-1) )
    {
        if (!defined($self->cell($fc_idx)))
        {
            $count++;
        }
    }
    return $count;
}

1; # End of Games::Solitaire::Verify::Freecells
