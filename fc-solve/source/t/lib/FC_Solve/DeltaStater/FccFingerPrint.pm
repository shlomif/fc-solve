package FC_Solve::DeltaStater::FccFingerPrint;

use strict;
use warnings;

use parent 'FC_Solve::DeltaStater';

use Carp ();

use Games::Solitaire::Verify::Card        ();
use Games::Solitaire::Verify::Column      ();
use Games::Solitaire::Verify::Foundations ();
use Games::Solitaire::Verify::State       ();

use FC_Solve::DeltaStater::OptionsStruct ();
use FC_Solve::VarBaseDigitsReader        ();
use FC_Solve::VarBaseDigitsWriter        ();

=head1 ALGORITHM

See git/fc-solve/fc-solve/docs/debondt-compact-freecell-positions--document.asciidoc

=cut

__PACKAGE__->mk_acc_ref(
    [qw(_card_states _bakers_dozen_topmost_cards_lookup)] );

my $RANK_KING       = 13;
my $FOUNDATION_BASE = $RANK_KING + 1;

my $ORIG_POS                         = 0;
my $ABOVE_PARENT_CARD_OR_EMPTY_SPACE = 1;
my $IN_FOUNDATIONS                   = 2;
my ( $LOWEST_CARD, $ABOVE_FREECELL, $PARENT_0, $PARENT_1 ) = ( 0 .. 3 );
my $state_opt_next = 0;
my %STATES_OPTS;
{

    sub _add
    {
        $STATES_OPTS{"@_"} = $state_opt_next++;
    }
    _add( $LOWEST_CARD,    $LOWEST_CARD );
    _add( $LOWEST_CARD,    $ABOVE_FREECELL );
    _add( $ABOVE_FREECELL, $LOWEST_CARD );
    _add( $ABOVE_FREECELL, $ABOVE_FREECELL );
    _add( $LOWEST_CARD,    $PARENT_0 );
    _add( $LOWEST_CARD,    $PARENT_1 );
    _add( $ABOVE_FREECELL, $PARENT_0 );
    _add( $ABOVE_FREECELL, $PARENT_1 );
    _add( $PARENT_0,       $LOWEST_CARD );
    _add( $PARENT_1,       $LOWEST_CARD );
    _add( $PARENT_0,       $ABOVE_FREECELL );
    _add( $PARENT_1,       $ABOVE_FREECELL );
    _add( $PARENT_0,       $PARENT_1 );
    _add( $PARENT_1,       $PARENT_0 );
}

my $OPT_TOPMOST              = 0;
my $OPT_DONT_CARE            = $OPT_TOPMOST;
my $OPT_FREECELL             = 1;
my $OPT_ORIG_POS             = 2;
my $NUM_KING_OPTS            = 3;
my $OPT_PARENT_SUIT_MOD_IS_0 = 3;
my $OPT_PARENT_SUIT_MOD_IS_1 = 4;
my $NUM_OPTS                 = 5;
my $OPT_IN_FOUNDATION        = 5;
my $NUM_OPTS_FOR_READ        = 6;

my $OPT__BAKERS_DOZEN__ORIG_POS      = 0;
my $OPT__BAKERS_DOZEN__FIRST_PARENT  = 1;
my $NUM__BAKERS_DOZEN__OPTS          = $OPT__BAKERS_DOZEN__FIRST_PARENT + 4;
my $OPT__BAKERS_DOZEN__IN_FOUNDATION = $NUM__BAKERS_DOZEN__OPTS;
my $NUM__BAKERS_DOZEN__OPTS_FOR_READ = $OPT__BAKERS_DOZEN__IN_FOUNDATION + 1;

my @suits = @{ Games::Solitaire::Verify::Card->get_suits_seq() };

sub _init
{
    my ( $self, $args ) = @_;

    $self->SUPER::_init($args);

    if ( $self->_is_bakers_dozen() )
    {
        die "unimplemented";
    }

    return;
}

sub _calc_state_obj_generic
{
    my ( $self, $args ) = @_;
    return $self->_is_bakers_dozen()
        ? ( die "unimpl" )
        : Games::Solitaire::Verify::State->new(
        {
            %{$args}, variant => 'freecell'
        },
        );
}

sub _initialize_card_states
{
    my ( $self, $num_opts ) = @_;

    $self->_card_states( [ map { [] } ( 0 .. $RANK_KING * 4 - 1 ) ] );

    return;
}

sub _free_card_states
{
    my $self = shift;

    $self->_card_states( undef() );

    return;
}

sub _opt_by_suit_rank
{
    my ( $self, $suit, $rank ) = @_;

    # Carp::cluck("Suit == $suit ; Rank == $rank");
    if ( $rank < 1 or $rank > $RANK_KING )
    {
        $DB::single = 1;
    }
    return $self->_card_states->[ $suit * $RANK_KING + $rank - 1 ];
}

sub _mark_suit_rank_as_true
{
    my ( $self, $suit, $rank, $opt ) = @_;

    @{ $self->_opt_by_suit_rank( $suit, $rank ) } = @$opt;

    return;
}

sub _get_suit_rank_verdict
{
    my ( $self, $suit, $rank ) = @_;

    return $self->_opt_by_suit_rank( $suit, $rank )->get_verdict;
}

sub _opt_by_card
{
    my ( $self, $card ) = @_;

    return $self->_opt_by_suit_rank( $self->_get_suit_idx($card),
        $card->rank(), );
}

sub _mark_opt_as_true
{
    my ( $self, $card, $opt ) = @_;

    @{ $self->_opt_by_card($card) } = (@$opt);

    return;
}

sub _get_card_verdict
{
    my ( $self, $card ) = @_;

    return $self->_opt_by_card($card)->get_verdict;
}

sub _wanted_suit_bit_opt
{
    my ( $self, $parent_card ) = @_;

    return $self->_get_suit_bit($parent_card)
        ? $OPT_PARENT_SUIT_MOD_IS_1
        : $OPT_PARENT_SUIT_MOD_IS_0;
}

sub _wanted_suit_idx_opt
{
    my ( $self, $parent_card ) = @_;
    return $OPT__BAKERS_DOZEN__FIRST_PARENT +
        $self->_get_suit_idx($parent_card);
}

sub _is_parent_card
{
    my ( $self, $parent_card, $child_card ) = @_;
    return (   ( $child_card->rank() + 1 == $parent_card->rank() )
            && ( $child_card->color() ne $parent_card->color() ) );
}

sub _calc_child_card_option
{
    my ( $self, $parent_card, $child_card ) = @_;

    if ( $self->_is_bakers_dozen() )
    {
        if (    ( $child_card->rank() != 1 )
            and ( $child_card->rank() + 1 == $parent_card->rank() ) )
        {
            return $self->_wanted_suit_idx_opt($parent_card);
        }
        else
        {
            return $OPT__BAKERS_DOZEN__ORIG_POS;
        }
    }

    if ( ( $child_card->rank() != 1 )
        and $self->_is_parent_card( $parent_card, $child_card ) )
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

    return ( $self->_is_bakers_dozen() ? ( $RANK_KING - 1 ) : $RANK_KING );
}

sub encode_composite
{
    my ($self) = @_;

    my $derived = $self->_derived_state;

    $self->_initialize_card_states(
          $self->_is_bakers_dozen()
        ? $NUM__BAKERS_DOZEN__OPTS
        : $NUM_OPTS
    );

    my $writer_state       = FC_Solve::VarBaseDigitsWriter->new;
    my $writer_fingerprint = FC_Solve::VarBaseDigitsWriter->new;

    my @cols_indexes = ( 0 .. $derived->num_columns - 1 );
    foreach my $col_idx (@cols_indexes)
    {
        my $col     = $derived->get_column($col_idx);
        my $col_len = $col->len();

        foreach my $height ( 0 .. $col_len - 1 )
        {
            my $card = $col->pos($height);
            my $o;
            if ( $height == 0 )
            {
                $o = [ $ABOVE_PARENT_CARD_OR_EMPTY_SPACE, $LOWEST_CARD ];
            }
            else
            {
                my $parent_card = $col->pos( $height - 1 );
                if ( $self->_is_parent_card( $parent_card, $card ) )
                {
                    $o = [
                        $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                        ( $self->_suit_get_suit_idx( $parent_card->suit ) & 2 )
                        ? $PARENT_1
                        : $PARENT_0
                    ];
                }
                else
                {
                    $o = [$ORIG_POS];
                }

            }
            $self->_mark_suit_rank_as_true(
                $self->_suit_get_suit_idx( $card->suit ),
                $card->rank, $o );
        }
    }
    foreach my $fc_idx ( 0 .. $derived->num_freecells - 1 )
    {
        my $card = $derived->get_freecell($fc_idx);

        if ( defined($card) )
        {
            $self->_mark_opt_as_true( $card,
                [ $ABOVE_PARENT_CARD_OR_EMPTY_SPACE, $ABOVE_FREECELL ] );
        }
    }

    # We encode the foundations separately so set the card value as don't care.
    my $cnt = 0;
    foreach my $rank ( 1 .. $RANK_KING )
    {
        foreach my $color ( 0 .. 1 )
        {
            my $suit1 = $color;
            my $suit2 = ( $color | 2 );
            my $rank1 = $derived->get_foundation_value( $suits[$suit1], 0 );
            my $rank2 = $derived->get_foundation_value( $suits[$suit2], 0 );
            my $fingerprint1;
            my $fingerprint2;
            my $is_founds1 = ( $rank1 >= $rank );
            my $is_founds2 = ( $rank2 >= $rank );
            my $opt1       = $self->_opt_by_suit_rank( $suit1, $rank );
            my $opt2       = $self->_opt_by_suit_rank( $suit2, $rank );
            $opt1->[0] = $IN_FOUNDATIONS if $is_founds1;
            die if !defined $opt1->[0];
            $opt2->[0] = $IN_FOUNDATIONS if $is_founds2;

            if ( !defined $opt2->[0] )
            {
                $DB::single = 1;
                Carp::confess "opt2";
            }
            $fingerprint1 = $opt1->[0];
            $fingerprint2 = $opt2->[0];
            my $is_king = ( $rank == $RANK_KING );

            if ( $rank > 1 )
            {
                if ( $fingerprint1 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                {
                    if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                    {
                    }
                    else
                    {
                        my $state_o = $opt2->[1];
                        if ($is_king)
                        {
                            $writer_state->write(
                                { base => 2, item => $state_o } );
                        }
                        else
                        {
                            $writer_state->write(
                                { base => 4, item => $state_o } );
                        }
                    }
                }
                else
                {
                    if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                    {
                        my $state_o = $opt1->[1];
                        if ($is_king)
                        {
                            $writer_state->write(
                                { base => 2, item => $state_o } );
                        }
                        else
                        {
                            $writer_state->write(
                                { base => 4, item => $state_o } );
                        }
                    }
                    else
                    {
                        my @states = ( $opt1->[1], $opt2->[1] );
                        if ($is_king)
                        {
                            foreach my $state_o (@states)
                            {
                                $writer_state->write(
                                    { base => 2, item => $state_o } );
                            }
                        }
                        else
                        {
                            $writer_state->write(
                                {
                                    base => $state_opt_next,
                                    item => (
                                        $STATES_OPTS{"@states"}
                                            // do { die "unknown key @states"; }
                                    ),
                                }
                            );
                        }
                    }
                }
            }

            # print("$rank $color $fingerprint1 $fingerprint2\n");

            $writer_fingerprint->write( { base => 3, item => $fingerprint1 } );
            $writer_fingerprint->write( { base => 3, item => $fingerprint2 } );
            $cnt += 2;
        }
    }

    if ( $cnt != 52 )
    {
        ...;
    }
    $self->_free_card_states;

    my $_data = sub {
        my $n   = shift()->get_data;
        my $ret = '';
        while ( $n > 0 )
        {
            $ret .= chr( $n & 255 );
            $n >>= 8;
        }
        return $ret;
    };

    my $ret = $_data->($writer_fingerprint);
    return [ $ret, $_data->($writer_state) ];
}

sub _fill_column_with_descendant_cards
{
    my ( $self, $col ) = @_;

    my $parent_card = $col->top;

    while ( defined($parent_card) )
    {
        my $child_card;

        my $wanted_opt =
              $self->_is_bakers_dozen
            ? $self->_wanted_suit_idx_opt($parent_card)
            : $self->_wanted_suit_bit_opt($parent_card);

    SEEK_CHILD_CARD:
        foreach my $suit (
              $self->_is_bakers_dozen()      ? qw(H C D S)
            : $parent_card->color() eq "red" ? (qw(C S))
            :                                  qw(H D)
            )
        {
            my $candidate_card = $self->_calc_card(
                $parent_card->rank() - 1,
                $self->_suit_get_suit_idx($suit),
            );

            my $opt = $self->_get_card_verdict($candidate_card);

            if ( $opt == $wanted_opt )
            {
                $child_card = $candidate_card;
                last SEEK_CHILD_CARD;
            }
        }

        if ( defined($child_card) )
        {
            $col->push($child_card);
        }
        $parent_card = $child_card;
    }

    return;
}

sub decode
{
    my ( $self, $bits ) = @_;

    my $reader = FC_Solve::VarBaseDigitsReader->new( { data => $bits } );

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

    foreach my $suit_idx ( 0 .. $#suits )
    {
        my $foundation_rank = $reader->read($FOUNDATION_BASE);

        foreach my $rank ( 1 .. $foundation_rank )
        {
            $self->_mark_suit_rank_as_true(
                $suit_idx,
                $rank,
                (
                      $self->_is_bakers_dozen
                    ? $OPT__BAKERS_DOZEN__IN_FOUNDATION
                    : $OPT_IN_FOUNDATION
                )
            );
        }

        $foundations_obj->assign( $suits[$suit_idx], 0, $foundation_rank );
    }

    my $is_in_foundations = sub {
        my ($card) = @_;

        return ( $card->rank() <= $foundations_obj->value( $card->suit(), 0 ) );
    };

    my $init_state = $self->_init_state;

    my $orig_pos_opt = (
          $self->_is_bakers_dozen
        ? $OPT__BAKERS_DOZEN__ORIG_POS
        : $OPT_ORIG_POS
    );

    my $num_freecells = $init_state->num_freecells();
    my @freecell_cards;

    my @new_top_most_cards;

    my %orig_top_most_cards = (
        map {
            my $card = $init_state->get_column($_)->pos(0);

            ( defined($card) ) ? ( $card->to_string() => 1 ) : ()
        } ( 0 .. $init_state->num_columns() - 1 )
    );

    # Process the kings:
    if ( $self->_is_bakers_dozen() )
    {
        foreach my $suit_idx ( 0 .. $#suits )
        {
            my $card = $self->_calc_card( $RANK_KING, $suit_idx, );

            if ( !$is_in_foundations->($card) )
            {
                $self->_mark_opt_as_true( $card, $OPT__BAKERS_DOZEN__ORIG_POS );
            }
        }
    }

    foreach my $rank ( 1 .. $self->_get_top_rank_for_iter() )
    {
    READ_SUITS:
        foreach my $suit_idx ( 0 .. $#suits )
        {
            my $card = $self->_calc_card( $rank, $suit_idx, );

            if ( $self->_is_bakers_dozen() )
            {
                if (
                    vec(
                        $self->_bakers_dozen_topmost_cards_lookup(),
                        $self->_get_card_bitmask($card),
                        1
                    )
                    )
                {
                    if ( !$is_in_foundations->($card) )
                    {
                        $self->_mark_opt_as_true( $card,
                            $OPT__BAKERS_DOZEN__ORIG_POS );
                    }
                    next READ_SUITS;
                }
            }

            my $existing_opt =
                $self->_get_suit_rank_verdict( $suit_idx, $rank );

            if ( $rank == 1 )
            {
                if ( $existing_opt < 0 )
                {
                    $self->_mark_suit_rank_as_true( $suit_idx, $rank,
                        $orig_pos_opt );
                }
                next READ_SUITS;
            }

            my $base = (
                  $self->_is_bakers_dozen ? $NUM__BAKERS_DOZEN__OPTS
                : ( $rank == $RANK_KING ) ? $NUM_KING_OPTS
                :                           $NUM_OPTS
            );
            my $item_opt = $reader->read($base);

            if ( $existing_opt < 0 )
            {
                $self->_mark_suit_rank_as_true( $suit_idx, $rank, $item_opt );

                if ( not $self->_is_bakers_dozen() )
                {
                    if ( $item_opt == $OPT_FREECELL )
                    {
                        push @freecell_cards, $card;
                    }
                    elsif ( $item_opt == $OPT_TOPMOST )
                    {
                        if (
                            !exists( $orig_top_most_cards{ $card->to_string } )
                            )
                        {
                            push @new_top_most_cards, $card;
                        }
                    }
                }
            }
        }
    }

    my $freecells =
        Games::Solitaire::Verify::Freecells->new( { count => $num_freecells } );

    my $get_bitmask = sub {
        my $card = shift;
        if ( !defined($card) )
        {
            return 0;
        }
        else
        {
            return $self->_get_card_bitmask($card);
        }
    };

    push @freecell_cards, ( (undef) x ( $num_freecells - @freecell_cards ) );

    @freecell_cards =
        sort { $get_bitmask->($a) <=> $get_bitmask->($b) } @freecell_cards;

    @new_top_most_cards =
        reverse
        sort { $get_bitmask->($a) <=> $get_bitmask->($b) } @new_top_most_cards;

    foreach my $fc_idx ( 0 .. $#freecell_cards )
    {
        if ( defined( $freecell_cards[$fc_idx] ) )
        {
            $freecells->assign( $fc_idx, $freecell_cards[$fc_idx] );
        }
    }

    my @columns;
    foreach my $col_idx ( 0 .. $self->_init_state->num_columns - 1 )
    {
        my $col = Games::Solitaire::Verify::Column->new( { cards => [] } );

        my $orig_col = $init_state->get_column($col_idx);

        my $top_card = $orig_col->pos(0);

        my $top_opt = defined($top_card)
            ? $self->_get_card_verdict($top_card)

            # Don't care.
            : (-1);

        if (
            !defined($top_card)
            or (
                !(
                       ( $top_opt == $OPT_TOPMOST )
                    || ( $top_opt == $orig_pos_opt )
                )
            )
            )
        {
            if (@new_top_most_cards)
            {
                my $new_top_card = shift(@new_top_most_cards);
                $col->push($new_top_card);

                $self->_fill_column_with_descendant_cards( $col, );
            }
        }
        else
        {
            $col->push($top_card);

            my $parent_card = $top_card;
        GO_OVER_EXISTING_CARDS:
            foreach my $pos ( 1 .. $orig_col->len() - 1 )
            {
                my $child_card = $orig_col->pos($pos);

                if (
                    ( !$is_in_foundations->($child_card) )
                    and (
                        $self->_get_card_verdict($child_card) ==
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
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
