package Games::Solitaire::FC_Solve::DeltaStater::DeBondt;

use strict;
use warnings;

use base 'Games::Solitaire::FC_Solve::DeltaStater';

use Carp;

use Games::Solitaire::Verify::Solution;
use Games::Solitaire::FC_Solve::DeltaStater::OptionsStruct;
use FC_Solve::VarBaseDigitsReader;
use FC_Solve::VarBaseDigitsWriter;

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

__PACKAGE__->mk_acc_ref([qw(_card_states _bakers_dozen_topmost_cards_lookup)]);

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

my $OPT__BAKERS_DOZEN__ORIG_POS = 0;
my $OPT__BAKERS_DOZEN__FIRST_PARENT = 1;
my $NUM__BAKERS_DOZEN__OPTS = $OPT__BAKERS_DOZEN__FIRST_PARENT + 4;
my $OPT__BAKERS_DOZEN__IN_FOUNDATION = $NUM__BAKERS_DOZEN__OPTS;
my $NUM__BAKERS_DOZEN__OPTS_FOR_READ = $OPT__BAKERS_DOZEN__IN_FOUNDATION + 1;

my @suits = @{Games::Solitaire::Verify::Card->get_suits_seq()};

sub _init
{
    my ($self, $args) = @_;

    $self->SUPER::_init($args);

    if ($self->_is_bakers_dozen())
    {
        my $_bakers_dozen_topmost_cards_lookup = '';
        foreach my $col_idx (0 .. $self->_init_state->num_columns - 1)
        {
            my $col = $self->_init_state->get_column($col_idx);
            my $col_len = $col->len();

            if ($col_len > 0)
            {
                my $top_card = $col->pos(0);
                vec($_bakers_dozen_topmost_cards_lookup, $self->_get_card_bitmask($top_card), 1) = 1;
            }
            $self->_bakers_dozen_topmost_cards_lookup($_bakers_dozen_topmost_cards_lookup);
        }
    }

    return;
}

sub _initialize_card_states
{
    my ($self,$num_opts) = @_;

    $self->_card_states(
        [
            map
            {
                Games::Solitaire::FC_Solve::DeltaStater::OptionsStruct->new(
                    {count => $num_opts, }
                );
            }
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

sub _mark_suit_rank_as_true
{
    my ($self, $suit, $rank, $opt) = @_;

    $self->_opt_by_suit_rank($suit, $rank)->mark_as_true($opt);

    return;
}

sub _get_suit_rank_verdict
{
    my ($self, $suit, $rank) = @_;

    return $self->_opt_by_suit_rank($suit, $rank)->get_verdict;
}

sub _opt_by_card
{
    my ($self, $card) = @_;

    return $self->_opt_by_suit_rank(
        $self->_get_suit_idx($card),
        $card->rank(),
    );
}

sub _mark_opt_as_true
{
    my ($self, $card, $opt) = @_;

    $self->_opt_by_card($card)->mark_as_true($opt);

    return;
}

sub _get_card_verdict
{
    my ($self, $card) = @_;

    return $self->_opt_by_card($card)->get_verdict;
}

sub _wanted_suit_bit_opt
{
    my ($self, $parent_card) = @_;

    return
    $self->_get_suit_bit($parent_card)
    ? $OPT_PARENT_SUIT_MOD_IS_1
    : $OPT_PARENT_SUIT_MOD_IS_0
    ;
}

sub _wanted_suit_idx_opt
{
    my ($self, $parent_card) = @_;
    return $OPT__BAKERS_DOZEN__FIRST_PARENT
    + $self->_get_suit_idx($parent_card)
    ;
}

sub _calc_child_card_option
{
    my ($self, $parent_card, $child_card) = @_;

    if ($self->_is_bakers_dozen())
    {
        if (
            ($child_card->rank() != 1)
                and
            ($child_card->rank()+1 == $parent_card->rank())
        )
        {
            return $self->_wanted_suit_idx_opt($parent_card);
        }
        else
        {
            return $OPT__BAKERS_DOZEN__ORIG_POS;
        }
    }

    if (
        ($child_card->rank() != 1)
            and
        (($child_card->rank()+1 == $parent_card->rank())
            && ($child_card->color() ne $parent_card->color()))
    )
    {
        return $self->_wanted_suit_bit_opt($parent_card);
    }
    else
    {
        return $OPT_ORIG_POS;
    }
}

sub _get_top_rank_for_iter
{
    my ($self) = @_;

    return ($self->_is_bakers_dozen() ? ($RANK_KING-1) : $RANK_KING);
}

sub encode_composite
{
    my ($self) = @_;

    my $derived = $self->_derived_state;

    $self->_initialize_card_states(
        $self->_is_bakers_dozen()
        ? $NUM__BAKERS_DOZEN__OPTS
        : $NUM_OPTS);

    my $writer = FC_Solve::VarBaseDigitsWriter->new;

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
            $self->_mark_suit_rank_as_true($suit_idx, $rank, $OPT_DONT_CARE);
        }

=begin foo
        # Removing because it is probably not needed.

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
=end foo

=cut

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

    if ($self->_is_bakers_dozen())
    {
        foreach my $col_idx (@cols_indexes)
        {
            my $col = $derived->get_column($col_idx);
            my $col_len = $col->len();

            if ($col_len > 0)
            {
                my $top_card = $col->pos(0);

                # Skip Aces which were already set.
                if ($top_card->rank() != 1)
                {
                    $self->_mark_opt_as_true($top_card, $OPT__BAKERS_DOZEN__ORIG_POS);
                }

                foreach my $pos (1 .. $col_len - 1)
                {
                    my $parent_card = $col->pos($pos-1);
                    my $this_card = $col->pos($pos);

                    # Skip Aces which were already set.
                    if ($this_card->rank() != 1)
                    {
                        if ($this_card->rank()+1 == $parent_card->rank())
                        {
                            $self->_mark_opt_as_true(
                                $this_card,
                                $self->_wanted_suit_idx_opt($parent_card),
                            );
                        }
                        else
                        {
                            $self->_mark_opt_as_true(
                                $this_card,
                                $OPT__BAKERS_DOZEN__ORIG_POS
                            );
                        }
                    }
                }
            }
        }
    }
    else
    {
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
    }

    # All cards should be determined now - let's encode.
    #
    # The foundations have already been encoded.
    #
    # Skip encoding the aces, and the kings are encoded with less bits.

    foreach my $rank (2 .. $self->_get_top_rank_for_iter())
    {
        SUIT_IDX:
        foreach my $suit_idx (0 .. $#suits)
        {
            my $opt = $self->_get_suit_rank_verdict($suit_idx, $rank);
            my $base;
            if ($self->_is_bakers_dozen())
            {
                my $card = Games::Solitaire::Verify::Card->new;
                $card->rank($rank);
                $card->suit($suits[$suit_idx]);
                if (vec($self->_bakers_dozen_topmost_cards_lookup(), $self->_get_card_bitmask($card), 1))
                {
                    next SUIT_IDX;
                }
                $base = $NUM__BAKERS_DOZEN__OPTS;
            }
            else
            {
                $base = (($rank == $RANK_KING) ? $NUM_KING_OPTS : $NUM_OPTS);
            }

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

sub _fill_column_with_descendant_cards
{
    my ($self, $col) = @_;

    my $parent_card = $col->top;

    while (defined($parent_card))
    {
        my $child_card;

        my $wanted_opt = $self->_is_bakers_dozen
            ? $self->_wanted_suit_idx_opt($parent_card)
            : $self->_wanted_suit_bit_opt($parent_card);

        SEEK_CHILD_CARD:
        foreach my $suit (
            $self->_is_bakers_dozen()
            ? qw(H C D S)
            : $parent_card->color() eq "red"
            ? (qw(C S))
            : qw(H D)
        )
        {
            my $candidate_card = Games::Solitaire::Verify::Card->new;
            $candidate_card->rank($parent_card->rank() - 1);
            $candidate_card->suit($suit);

            my $opt = $self->_get_card_verdict($candidate_card);

            if ($opt == $wanted_opt)
            {
                $child_card = $candidate_card;
                last SEEK_CHILD_CARD;
            }
        }

        if (defined($child_card))
        {
            $col->push($child_card);
        }
        $parent_card = $child_card;
    }

    return;
}

sub decode
{
    my ($self, $bits) = @_;

    my $reader = FC_Solve::VarBaseDigitsReader->new({data => $bits});

    $self->_initialize_card_states(
        $self->_is_bakers_dozen
        ? $NUM__BAKERS_DOZEN__OPTS_FOR_READ
        : $NUM_OPTS_FOR_READ
    );

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
            $self->_mark_suit_rank_as_true(
                $suit_idx, $rank,
                ($self->_is_bakers_dozen
                    ? $OPT__BAKERS_DOZEN__IN_FOUNDATION
                    : $OPT_IN_FOUNDATION
                )
            );
        }

        $foundations_obj->assign($suits[$suit_idx], 0, $foundation_rank);
    }

    my $is_in_foundations = sub {
        my ($card) = @_;

        return ($card->rank() <= $foundations_obj->value($card->suit(), 0));
    };

    my $init_state = $self->_init_state;

    my $orig_pos_opt =
    ($self->_is_bakers_dozen
        ? $OPT__BAKERS_DOZEN__ORIG_POS
        : $OPT_ORIG_POS
    );

    my $num_freecells = $init_state->num_freecells();
    my @freecell_cards;

    my @new_top_most_cards;

    my %orig_top_most_cards = (
        map {
            my $card = $init_state->get_column($_)->pos(0);

            (defined($card)) ? ( $card->to_string() => 1 ) : ()
       } (0 .. $init_state->num_columns() - 1)
    );

    # Process the kings:
    if ($self->_is_bakers_dozen())
    {
        foreach my $suit_idx (0 .. $#suits)
        {
            my $card = Games::Solitaire::Verify::Card->new;
            $card->rank($RANK_KING);
            $card->suit($suits[$suit_idx]);

            if (! $is_in_foundations->($card))
            {
                $self->_mark_opt_as_true($card, $OPT__BAKERS_DOZEN__ORIG_POS);
            }
        }
    }

    foreach my $rank (1 .. $self->_get_top_rank_for_iter())
    {
        READ_SUITS:
        foreach my $suit_idx (0 .. $#suits)
        {
            my $card = Games::Solitaire::Verify::Card->new;
            $card->rank($rank);
            $card->suit($suits[$suit_idx]);

            if ($self->_is_bakers_dozen())
            {
                if (vec(
                        $self->_bakers_dozen_topmost_cards_lookup(),
                        $self->_get_card_bitmask($card),
                        1
                    )
                )
                {
                    if (! $is_in_foundations->($card))
                    {
                        $self->_mark_opt_as_true($card, $OPT__BAKERS_DOZEN__ORIG_POS);
                    }
                    next READ_SUITS;
                }
            }

            my $existing_opt = $self->_get_suit_rank_verdict($suit_idx, $rank);

            if ($rank == 1)
            {
                if ($existing_opt < 0)
                {
                    $self->_mark_suit_rank_as_true(
                        $suit_idx, $rank, $orig_pos_opt
                    );
                }
                next READ_SUITS;
            }

            my $base = ($self->_is_bakers_dozen ? $NUM__BAKERS_DOZEN__OPTS :
                ($rank == $RANK_KING) ? $NUM_KING_OPTS : $NUM_OPTS);
            my $item_opt = $reader->read($base);

            if ($existing_opt < 0)
            {
                $self->_mark_suit_rank_as_true(
                    $suit_idx, $rank, $item_opt
                );

                if (not $self->_is_bakers_dozen())
                {
                    if ($item_opt == $OPT_FREECELL)
                    {
                        push @freecell_cards, $card;
                    }
                    elsif ($item_opt == $OPT_TOPMOST)
                    {
                        if (!exists($orig_top_most_cards{$card->to_string}))
                        {
                            push @new_top_most_cards, $card;
                        }
                    }
                }
            }
        }
    }

    my $freecells = Games::Solitaire::Verify::Freecells->new({count => $num_freecells});

    my $get_bitmask = sub {
        my $card = shift;
        if (!defined($card))
        {
            return 0;
        }
        else
        {
            return $self->_get_card_bitmask($card);
        }
    };

    push @freecell_cards, ((undef) x ($num_freecells - @freecell_cards));

    @freecell_cards =
        sort { $get_bitmask->($a) <=> $get_bitmask->($b) }
        @freecell_cards;

    @new_top_most_cards =
        reverse
        sort { $get_bitmask->($a) <=> $get_bitmask->($b) }
        @new_top_most_cards;

    foreach my $fc_idx (0 .. $#freecell_cards)
    {
        if (defined($freecell_cards[$fc_idx]))
        {
            $freecells->assign($fc_idx, $freecell_cards[$fc_idx]);
        }
    }

    my @columns;
    foreach my $col_idx (0 .. $self->_init_state->num_columns - 1)
    {
        my $col = Games::Solitaire::Verify::Column->new({cards => []});

        my $orig_col = $init_state->get_column($col_idx);

        my $top_card = $orig_col->pos(0);

        my $top_opt = $self->_get_card_verdict($top_card);

        if (!defined($top_card) or
            (! (($top_opt == $OPT_TOPMOST) || ($top_opt == $orig_pos_opt)))
        )
        {
            if (@new_top_most_cards)
            {
                my $new_top_card = shift(@new_top_most_cards);
                $col->push($new_top_card);

                $self->_fill_column_with_descendant_cards(
                    $col,
                );
            }
        }
        else
        {
            $col->push($top_card);

            my $parent_card = $top_card;
            GO_OVER_EXISTING_CARDS:
            foreach my $pos (1 .. $orig_col->len() - 1)
            {
                my $child_card = $orig_col->pos($pos);

                if (
                    (!$is_in_foundations->($child_card))
                        and
                    (
                        $self->_get_card_verdict($child_card)
                            ==
                        $self->_calc_child_card_option(
                            $parent_card, $child_card
                        )
                    )
                )
                {
                    $col->push($child_card);
                    $parent_card = $child_card;
                }
                else
                {
                    last GO_OVER_EXISTING_CARDS;
                }
            }

            $self->_fill_column_with_descendant_cards($col);
        }

        push @columns, $col;
    }

    my $state = $self->_calc_new_empty_state_obj;

    foreach my $col (@columns)
    {
        $state->add_column($col);
    }

    $state->set_freecells($freecells);

    $state->set_foundations($foundations_obj);

    return $state;
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
