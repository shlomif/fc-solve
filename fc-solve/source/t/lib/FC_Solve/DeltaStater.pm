package FC_Solve::DeltaStater;

use strict;
use warnings;

use Games::Solitaire::Verify::Card        ();
use Games::Solitaire::Verify::Column      ();
use Games::Solitaire::Verify::State       ();
use Games::Solitaire::Verify::VariantsMap ();

use FC_Solve::DeltaStater::BitWriter;
use FC_Solve::DeltaStater::BitReader;

use parent 'Games::Solitaire::Verify::Base';

my $two_fc_variant =
    Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

$two_fc_variant->num_freecells(2);

sub _get_two_fc_variant
{
    return $two_fc_variant;
}

my $bakers_dozen_variant =
    Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id(
    'bakers_dozen');

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _derived_state _init_state _columns_initial_lens _orig_args _variant
        )
    ]
);

sub _get_column_orig_num_cards
{
    my ( $self, $col ) = @_;

    my $num_cards = $col->len();

CALC_NUM_CARDS:
    while ( $num_cards >= 2 )
    {
        my $child_card  = $col->pos( $num_cards - 1 );
        my $parent_card = $col->pos( $num_cards - 2 );
        if (
            !(
                   ( $child_card->rank() + 1 == $parent_card->rank() )
                && ( $child_card->color() ne $parent_card->color() )
            )
            )
        {
            last CALC_NUM_CARDS;
        }
    }
    continue
    {
        $num_cards--;
    }

    if ( $num_cards == 1 )
    {
        $num_cards = 0;
    }

    return $num_cards;
}

sub _is_bakers_dozen
{
    my ($self) = @_;

    return ( $self->_variant() eq "bakers_dozen" );
}

sub _calc_state_obj_generic
{
    my ( $self, $args ) = @_;
    return $self->_is_bakers_dozen()
        ? Games::Solitaire::Verify::State->new(
        {
            variant => $self->_variant(),
            %{ $self->_orig_args() },
            %{$args},
        }
        )
        : Games::Solitaire::Verify::State->new(
        {
            variant        => 'custom',
            variant_params => $two_fc_variant,
            %{ $self->_orig_args() },
            %{$args},
        },
        );
}

sub _calc_state_obj_from_string
{
    my ( $self, $args ) = @_;

    return $self->_calc_state_obj_generic(
        { %{ $self->_orig_args() }, string => $args->{str}, %$args, } );
}

sub _calc_new_empty_state_obj
{
    my ($self) = @_;

    return $self->_calc_state_obj_generic( { %{ $self->_orig_args() }, } );
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->_orig_args($args);

    $self->_variant( $args->{variant} || "two_fc_freecell" );

    $self->_init_state(
        $self->_calc_state_obj_from_string(
            { str => $args->{init_state_str}, %$args, }
        )
    );

    my $init_state = $self->_init_state;

    my @columns_initial_bit_lens;

    foreach my $col_idx ( 0 .. $init_state->num_columns() - 1 )
    {
        my $num_cards =
            $self->_get_column_orig_num_cards(
            $init_state->get_column($col_idx) );

        my $bitmask  = 1;
        my $num_bits = 0;

        while ( $bitmask <= $num_cards )
        {
            ++$num_bits;
            $bitmask <<= 1;
        }

        push @columns_initial_bit_lens, $num_bits;
    }

    $self->_columns_initial_lens( \@columns_initial_bit_lens );

    return;
}

sub set_derived
{
    my ( $self, $args ) = @_;

    $self->_derived_state(
        $self->_calc_state_obj_from_string(
            { str => $args->{state_str}, %$args, }
        )
    );

    return;
}

my @suits = (qw(H C D S));

# NOTE : Not used because it can be calculated from the freecells and the
# columns.
sub get_foundations_bits
{
    my ($self) = @_;

    return [
        map { [ 4 => $self->_derived_state->get_foundation_value( $_, 0 ) ] }
            @suits ];
}

sub _get_suit_bit
{
    my ( $self, $card ) = @_;

    my $suit = $card->suit();

    return ( ( $suit eq 'H' || $suit eq 'C' ) ? 0 : 1 );
}

my %suit_to_idx = do
{
    my $s = Games::Solitaire::Verify::Card->get_suits_seq();
    ( map { $s->[$_] => $_ } ( 0 .. $#$s ) );
};

sub _suit_get_suit_idx
{
    my ( $self, $suit ) = @_;

    return $suit_to_idx{$suit};
}

sub _get_suit_idx
{
    my ( $self, $card ) = @_;

    return $self->_suit_get_suit_idx( $card->suit );
}

sub _get_card_bitmask
{
    my ( $self, $card ) = @_;

    return ( $self->_get_suit_idx($card) | ( $card->rank() << 2 ) );
}

sub _calc_card
{
    my ( $self, $rank, $suit_idx ) = @_;

    return Games::Solitaire::Verify::Card->new(
        {
            string => (
                Games::Solitaire::Verify::Card->rank_to_string($rank)
                    . $suits[$suit_idx]
            )
        }
    );
}

my $COL_TYPE_EMPTY             = 0;
my $COL_TYPE_ENTIRELY_NON_ORIG = 1;
my $COL_TYPE_HAS_ORIG          = 2;

sub _get_column_encoding_composite
{
    my ( $self, $col_idx ) = @_;

    my $derived = $self->_derived_state();

    my $col = $derived->get_column($col_idx);

    my $num_orig_cards = $self->_get_column_orig_num_cards($col);

    my $col_len           = $col->len();
    my $num_derived_cards = $col_len - $num_orig_cards;

    my $num_cards_in_seq = $num_derived_cards;
    my @init_card;
    if ( ( $num_orig_cards == 0 ) && $num_derived_cards )
    {
        @init_card = ( [ 6 => $self->_get_card_bitmask( $col->pos(0) ) ] );
        $num_cards_in_seq--;
    }

    return {
        type => (
              ( $col_len == 0 ) ? $COL_TYPE_EMPTY
            : $num_orig_cards   ? $COL_TYPE_HAS_ORIG
            :                     $COL_TYPE_ENTIRELY_NON_ORIG
        ),
        enc => [
            [ $self->_columns_initial_lens->[$col_idx] => $num_orig_cards ],
            [ 4                                        => $num_derived_cards ],
            @init_card,
            (
                map { [ 1 => $self->_get_suit_bit( $col->pos($_) ) ] }
                    ( $col_len - $num_cards_in_seq .. $col_len - 1 )
            ),
        ],
    };
}

sub get_column_encoding
{
    my ( $self, $col_idx ) = @_;

    return $self->_get_column_encoding_composite($col_idx)->{enc};
}

sub get_freecells_encoding
{
    my ($self) = @_;

    my $derived = $self->_derived_state();

    return [
        map {
            my $card = $derived->get_freecell($_);
            [ 6 => ( defined($card) ? $self->_get_card_bitmask($card) : 0 ) ]
        } ( 0 .. $derived->num_freecells() - 1 )
    ];
}

sub _composite_get_cols_and_indexes
{
    my ($self) = @_;

    my @cols_indexes = ( 0 .. $self->_derived_state->num_columns - 1 );
    my @cols =
        ( map { $self->_get_column_encoding_composite($_) } @cols_indexes );

    {
        my $non_orig_idx = 0;
        my $empty_idx    = $#cols;

        # Move the empty columns to the front, but only within the
        # entirely_non_orig
        # That's because the orig columns should be preserved in their own
        # place.
    MOVE_EMPTIES_LOOP:
        while (1)
        {
        NON_ORIG_IDX_LOOP:
            while ( $non_orig_idx < @cols )
            {
                if ( $cols[$non_orig_idx]->{type} eq
                    $COL_TYPE_ENTIRELY_NON_ORIG )
                {
                    last NON_ORIG_IDX_LOOP;
                }
            }
            continue
            {
                ++$non_orig_idx;
            }

            if ( $non_orig_idx == @cols )
            {
                last MOVE_EMPTIES_LOOP;
            }

        EMPTY_IDX_LOOP:
            while ( $empty_idx >= 0 )
            {
                if ( $cols[$empty_idx]->{type} eq $COL_TYPE_EMPTY )
                {
                    last EMPTY_IDX_LOOP;
                }
            }
            continue
            {
                --$empty_idx;
            }

            if ( ( $empty_idx < 0 ) || ( $empty_idx < $non_orig_idx ) )
            {
                last MOVE_EMPTIES_LOOP;
            }

            @cols_indexes[ $non_orig_idx, $empty_idx ] =
                @cols_indexes[ $empty_idx, $non_orig_idx ];
            ++$non_orig_idx;
            --$empty_idx;
        }
    }

    {
        my @new_non_orig_cols_indexes =
            ( grep { $cols[$_]->{type} eq $COL_TYPE_ENTIRELY_NON_ORIG }
                @cols_indexes );

        my $get_sort_val = sub {
            my ($i) = @_;
            return $self->_get_card_bitmask(
                $self->_derived_state()->get_column($i)->pos(0) );
        };
        my @sorted = ( sort { $get_sort_val->($a) <=> $get_sort_val->($b) }
                @new_non_orig_cols_indexes );

        foreach my $idx_idx ( 0 .. $#cols_indexes )
        {
            if ( $cols[ $cols_indexes[$idx_idx] ]->{type} eq
                $COL_TYPE_ENTIRELY_NON_ORIG )
            {
                $cols_indexes[$idx_idx] = shift(@sorted);
            }
        }
    }

    return { cols => \@cols, cols_indexes => \@cols_indexes };
}

sub encode_composite
{
    my ($self) = @_;

    my $cols_struct = $self->_composite_get_cols_and_indexes;

    my $cols         = $cols_struct->{cols};
    my $cols_indexes = $cols_struct->{cols_indexes};

    my $bit_writer = FC_Solve::DeltaStater::BitWriter->new;
    foreach my $bit_spec (
        @{ $self->get_freecells_encoding() },
        ( map { @{ $_->{enc} } } @{$cols}[ @{$cols_indexes} ] ),
        )
    {
        $bit_writer->write( $bit_spec->[0] => $bit_spec->[1] );
    }

    return $bit_writer->get_bits();
}

sub encode
{
    my ($self) = @_;

    my $bit_writer = FC_Solve::DeltaStater::BitWriter->new;

    foreach my $bit_spec (
        @{ $self->get_freecells_encoding() },
        (
            map { @{ $self->get_column_encoding($_) } }
                ( 0 .. $self->_derived_state->num_columns - 1 )
        ),
        )
    {
        $bit_writer->write( $bit_spec->[0] => $bit_spec->[1] );
    }

    return $bit_writer->get_bits();
}

sub decode
{
    my ( $self, $bits ) = @_;

    my $bit_reader = FC_Solve::DeltaStater::BitReader->new( { bits => $bits } );

    my %foundations = ( map { $_ => 14 } @suits );

    my $process_card = sub {
        my $card = shift;

        if ( $card->rank() < $foundations{ $card->suit() } )
        {
            $foundations{ $card->suit() } = $card->rank();
        }

        return $card;
    };

    my $process_card_bits = sub {
        my $card_bits = shift;

        my $card = $self->_calc_card(
            ( $card_bits >> 2 ),
            ( $card_bits & ( ( 1 << 2 ) - 1 ) ),
        );

        return $process_card->($card);
    };

    my $num_freecells = $self->_init_state->num_freecells();

    # Read the Freecells.
    my $freecells =
        Games::Solitaire::Verify::Freecells->new( { count => $num_freecells } );

    foreach my $freecell_idx ( 0 .. $num_freecells - 1 )
    {
        my $card_bits = $bit_reader->read(6);

        if ( $card_bits != 0 )
        {
            $freecells->assign( $freecell_idx,
                $process_card_bits->($card_bits) );
        }
    }

    my @columns;

    foreach my $col_idx ( 0 .. $self->_init_state->num_columns - 1 )
    {
        my $col = Games::Solitaire::Verify::Column->new( { cards => [] } );

        my $num_orig_cards =
            $bit_reader->read( $self->_columns_initial_lens->[$col_idx] );

        my $orig_col = $self->_init_state->get_column($col_idx);
        foreach my $i ( 0 .. $num_orig_cards - 1 )
        {
            $col->push( $process_card->( $orig_col->pos($i)->clone() ), );
        }

        my $num_derived_cards = $bit_reader->read(4);
        my $num_cards_in_seq  = $num_derived_cards;

        if ( ( $num_orig_cards == 0 ) && $num_derived_cards )
        {
            my $card_bits = $bit_reader->read(6);
            $col->push( $process_card_bits->($card_bits) );
            $num_cards_in_seq--;
        }

        if ($num_cards_in_seq)
        {
            my $last_card = $col->pos(-1);
            for my $i ( 0 .. $num_cards_in_seq - 1 )
            {
                my $suit_bit = $bit_reader->read(1);

                my $new_card = $self->_calc_card(
                    ( $last_card->rank() - 1 ),
                    (
                        ( $suit_bit << 1 ) |
                            ( $last_card->color eq "red" ? 1 : 0 )
                    ),
                );

                $col->push( $process_card->( $last_card = $new_card ) );
            }
        }

        push @columns, $col;
    }

    my $foundations_obj = Games::Solitaire::Verify::Foundations->new(
        {
            num_decks => 1,
        },
    );

    foreach my $found ( keys(%foundations) )
    {
        $foundations_obj->assign( $found, 0, $foundations{$found} - 1 );
    }

    my $state = $self->_calc_new_empty_state_obj();
    foreach my $col (@columns)
    {
        $state->add_column($col);
    }
    $state->set_freecells($freecells);
    $state->set_foundations($foundations_obj);

    return $state;
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
