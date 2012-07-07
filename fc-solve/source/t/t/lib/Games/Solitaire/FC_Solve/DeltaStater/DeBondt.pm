package Games::Solitaire::FC_Solve::DeltaStater::DeBondt;

use strict;
use warnings;

use Games::Solitaire::Verify::Solution;

use base 'Games::Solitaire::FC_Solve::DeltaStater';

use Carp;

=head1 ALGORITHM

Given the original position with all the information and the derived position
that we wish to encode, we encode it as follows:

=head2 States of Each card

If we take, say the Jack of Diamonds, then the options are:

1. C is the topmost card of a stack (does not have a parent).

2. C lies on the queen of spades.

3. C lies on queen of clubs.

4. C is already in the foundations.

5. C is on a freecell.

6. C lies on the card on which it was originally present.

=head2 Optimisations.

The only valid positions for Aces (given
Horne's prune) are in their original position (#6 in your case), or in the
foundation (#4 in your case), so we can have:

    log2(6) * 48 + log2(2) * 4 = 128.078200034615 bits (bytes: 17)

In addition, cases #2 and #3 are not possible for kings, so they only have
4 possible cases, so we get:

    log2(6) * 44 + log2(2) * 4 + log2(4) * 4 = 125.738350031731 bits
    (16 bytes)

If we first encode the value of each foundation
(from 0 to King - 14 values in total), we can remove one option (#4 in your
case) from each of the bases and get:

    log2(14) * 4 + log2(5) * 44 + log2(1) * 4 + log2(3) * 4
        = 123.73410586615 bits (bytes: 16)

=cut

package OptionsStruct;

use base 'Games::Solitaire::Verify::Base';

my $UNKNOWN = 0;
my $IMPOSSIBLE = 1;
my $TRUE = 2;

use List::MoreUtils qw(firstidx);

__PACKAGE__->mk_acc_ref([qw(_options _which_option _num_unknowns)]);

sub _init
{
    my ($self, $args) = @_;

    my $count = $args->{count};

    $self->_options([($UNKNOWN) x $count]);

    $self->_which_option(-1);

    $self->_num_unknowns($count);

    return;
}

sub get_option
{
    my ($self,$idx) = @_;

    return $self->_options->[$idx];
}

sub _set_option
{
    my ($self,$idx, $val) = @_;

    if (($val != $TRUE) && ($val != $IMPOSSIBLE))
    {
        Carp::confess "Must be true or impossible.";
    }

    if ($self->get_option($idx) != $UNKNOWN)
    {
        Carp::confess "Option was already set.";
    }

    $self->_options->[$idx] = $val;
}

sub get_verdict
{
    my ($self) = @_;

    return $self->_which_option;
}

sub mark_as_impossible
{
    my $self = shift;
    my $idx = shift;

    if ($self->_which_option() >= 0)
    {
        Carp::confess "Already decided!";
    }

    if ($self->get_option($idx) != $UNKNOWN)
    {
        Carp::confess "Already set.";
    }

    $self->_set_option($idx, $IMPOSSIBLE);
    $self->_num_unknowns($self->_num_unknowns() -1);

    if ($self->_num_unknowns() == 1)
    {
        $self->_which_option(
            firstidx { $self->get_option($_) == $UNKNOWN }
            (0 .. $#{$self->_options()})
        );
        $self->_set_option($self->_which_option(), $TRUE);
        $self->_num_unknowns(0);
    }

    return;
}

sub mark_as_true
{
    my $self = shift;
    my $option_idx = shift;

    if ($self->_which_option() >= 0)
    {
        Carp::confess "Already decided!";
    }

    if ($self->get_option($option_idx) != $UNKNOWN)
    {
        Carp::confess "Already set.";
    }

    $self->_set_option($option_idx, $TRUE);
    $self->_which_option($option_idx);
    $self->_num_unknowns(0);

    foreach my $idx (0 .. $#{$self->_options})
    {
        if ($self->get_option($idx) == $UNKNOWN)
        {
            $self->_set_option($idx, $IMPOSSIBLE);
        }
    }

    return;
}

package Games::Solitaire::FC_Solve::DeltaStater::DeBondt;

__PACKAGE__->mk_acc_ref([qw(_card_states)]);

my $RANK_KING = 13;
my $FOUNDATION_BASE = $RANK_KING+1;

my $OPT_TOPMOST = 0;
my $OPT_DONT_CARE = $OPT_TOPMOST;
my $OPT_FREECELL = 1;
my $OPT_ORIG_POS = 2;
my $NUM_KING_OPTS = 3;
my $OPT_PARENT_SUIT_MOD_IS_0 = 3;
my $OPT_PARENT_SUIT_MOD_IS_1 = 4;
my $NUM_OPTS = 5;
my $OPT_IN_FOUNDATION = 5;
my $NUM_OPTS_FOR_READ = 6;

my (@suits, %suit_to_idx);

{
    my $s = Games::Solitaire::Verify::Card->get_suits_seq();
    @suits = @{$s};

    %suit_to_idx = (map { $s->[$_] => $_ } (0 .. $#$s));
}

sub _initialize_card_states
{
    my ($self,$num_opts) = @_;

    $self->_card_states(
        [
            map
            { OptionsStruct->new({count => $num_opts, }); }
            (0 .. $RANK_KING * 4)
        ]
    );

    return;
}

sub _free_card_states
{
    my $self = shift;

    $self->_card_states(undef());

    return;
}

sub _opt_by_suit_rank
{
    my ($self, $suit, $rank) = @_;

    # Carp::cluck("Suit == $suit ; Rank == $rank");
    return $self->_card_states->[$suit * $RANK_KING + $rank-1];
}

sub _opt_by_card
{
    my ($self, $card) = @_;

    return $self->_opt_by_suit_rank(
        $suit_to_idx{$card->suit()},
        $card->rank(),
    );
}

sub _mark_opt_as_true
{
    my ($self, $card, $opt) = @_;

    $self->_opt_by_card($card)->mark_as_true($opt);

    return;
}

sub _calc_child_card_option
{
    my ($self, $parent_card, $child_card) = @_;

    if (
        (($child_card->rank()+1 == $parent_card->rank())
            && ($child_card->color() ne $parent_card->color()))
    )
    {
        return $self->_get_suit_bit($parent_card)
            ? $OPT_PARENT_SUIT_MOD_IS_1
            : $OPT_PARENT_SUIT_MOD_IS_0
            ;
    }
    else
    {
        return $OPT_ORIG_POS;
    }
}

sub encode_composite
{
    my ($self) = @_;

    my $derived = $self->_derived_state;

    $self->_initialize_card_states($NUM_OPTS);

    my $writer = VariableBaseDigitsWriter->new;

    # We encode the foundations separately so set the card value as don't care.
    foreach my $suit_idx (0 .. $#suits)
    {
        my $rank = $derived->get_foundation_value($suits[$suit_idx], 0);

        $writer->write({base => $FOUNDATION_BASE, item => $rank });

        # The Aces are always hard-wired, so always mark them.
        #
        my $max_rank = (($rank < 1) ? 1 : $rank);

        foreach my $rank (1 .. $max_rank)
        {
            $self->_opt_by_suit_rank($suit_idx, $rank)->mark_as_true($OPT_DONT_CARE);
        }

        if ($max_rank != $RANK_KING)
        {
            foreach my $opt (
                $OPT_PARENT_SUIT_MOD_IS_0,
                $OPT_PARENT_SUIT_MOD_IS_1,
            )
            {
                $self->_opt_by_suit_rank($suit_idx, $RANK_KING)->mark_as_impossible(
                    $opt
                );
            }
        }
    }

    foreach my $fc_idx (0 .. $derived->num_freecells - 1)
    {
        my $card = $derived->get_freecell($fc_idx);

        if (defined($card))
        {
            $self->_mark_opt_as_true($card, $OPT_FREECELL);
        }
    }

    my @cols_indexes = (0 .. $derived->num_columns - 1);
    foreach my $col_idx (@cols_indexes)
    {
        my $col = $derived->get_column($col_idx);

        my $col_len = $col->len();

        if ($col_len > 0)
        {
            my $top_card = $col->pos(0);

            if ($top_card->rank() != 1)
            {
                $self->_mark_opt_as_true($top_card, $OPT_TOPMOST);
            }

            my $parent_card = $top_card;

            foreach my $child_idx (1 .. $col_len-1)
            {
                my $child_card = $col->pos($child_idx);

                if ($child_card->rank() != 1)
                {
                    $self->_mark_opt_as_true(
                        $child_card,
                        $self->_calc_child_card_option(
                            $parent_card, $child_card
                        )
                    );
                }

                $parent_card = $child_card;
            }
        }
    }

    # All cards should be determined now - let's encode.
    #
    # The foundations have already been encoded.
    #
    # Skip encoding the aces, and the kings are encoded with less bits.

    foreach my $rank (2 .. $RANK_KING)
    {
        foreach my $suit_idx (0 .. $#suits)
        {
            my $opt = $self->_opt_by_suit_rank($suit_idx, $rank)->get_verdict();

            my $base = (($rank == $RANK_KING) ? $NUM_KING_OPTS : $NUM_OPTS);
            if ($opt < 0)
            {
                Carp::confess ( "Opt is not set." );
            }

            if ($opt >= $base)
            {
                Carp::confess ( "Opt overflow." );
            }

            $writer->write(
                {
                    base => $base,
                    item => $opt,
                }
            );
        }
    }

    $self->_free_card_states;

    return $writer->get_data();
}

sub decode
{
    my ($self, $bits) = @_;

    my $reader = VariableBaseDigitsReader->new({data => $bits});

    $self->_initialize_card_states($NUM_OPTS_FOR_READ);

    my $foundations_obj = Games::Solitaire::Verify::Foundations->new(
        {
            num_decks => 1,
        },
    );

    foreach my $suit_idx (0 .. $#suits)
    {
        my $foundation_rank = $reader->read($FOUNDATION_BASE);

        foreach my $rank (1 .. $foundation_rank)
        {
            $self->_opt_by_suit_rank($suit_idx, $rank)->mark_as_true($OPT_IN_FOUNDATION);
        }

        $foundations_obj->assign($suits[$suit_idx], 0, $foundation_rank);
    }

    return;
}
=head1 COPYRIGHT & LICENSE

Copyright 2012 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
