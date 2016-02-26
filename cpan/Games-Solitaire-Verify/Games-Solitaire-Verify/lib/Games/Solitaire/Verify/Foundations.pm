package Games::Solitaire::Verify::Foundations;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Foundations - a class for representing the
foundations (or home-cells) in a Solitaire game.

=cut

our $VERSION = '0.1700';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;

use List::Util qw(first);

# _s is the string.
__PACKAGE__->mk_acc_ref([qw(
    _num_decks
    _founds
    _s
    )]);

# Suits sequence:
my @SS = (@{Games::Solitaire::Verify::Card->get_suits_seq()});
# Reverse.
my %RS = (map { $SS[$_] => $_ } (0 .. $#SS));
# Ranks
my @R = (@{Games::Solitaire::Verify::Card->get_ranks_strings()});

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Foundations;

    # For internal use.

=head1 METHODS

=cut

sub _input_from_string
{
    my $self = shift;
    my $str = shift;

    my $rank_re = '[0A1-9TJQK]';

    if($str !~ m{\AFoundations: H-($rank_re) C-($rank_re) D-($rank_re) S-($rank_re) *\z}ms)
    {
        die "str=<$str>";
        Games::Solitaire::Verify::Exception::Parse::State::Foundations->throw(
            error => "Wrong Foundations",
        );
    }
    {
        my @founds_strings = ($1, $2, $3, $4);

        foreach my $suit (@SS)
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
            @SS
        }
    );

    $self->_s($self->_init_s);

    if (exists($args->{string}))
    {
        $self->_input_from_string($args->{string});
    }


    return;
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

    # Replace the rank in place.
    substr($self->{_s}, (length('Foundations:') + 3) + ($RS{$suit} << 2), 1, $R[$rank]);
    return;
}

=head2 $self->increment($suit, $index)

Increments the value of the foundation with the suit $suit and the
index $index to $rank .

=cut

sub increment
{
    my ($self, $suit, $idx) = @_;

    substr($self->{_s}, (length('Foundations:') + 3) + ($RS{$suit} << 2), 1, $R[++($self->_founds()->{$suit}->[$idx])]);

    return;
}

=head2 $self->to_string()

Stringifies the freecells into the Freecell Solver solution display notation.

=cut

sub _foundations_strings
{
    my $self = shift;

    return [
    ];
}

sub to_string
{
    return $_[0]->_s;
}

sub _init_s
{
    my $S = shift;

    return   "Foundations:"
    . join("",
        map {
            " $_-".  $R[ $S->value($_, 0) ]
        }
        @SS
    );
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

    foreach my $suit (@SS)
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

1; # End of Games::Solitaire::Verify::Move
