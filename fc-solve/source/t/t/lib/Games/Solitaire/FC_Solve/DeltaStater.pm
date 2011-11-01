#!/usr/bin/perl

use strict;
use warnings;

use Games::Solitaire::Verify::Solution;

package BitWriter;

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(_bits_ref _bit_idx)]);

sub _init
{
    my $self = shift;

    $self->_bits_ref(\(my $s = ''));

    $self->_bit_idx(0);

    return;
}

sub _next_idx
{
    my $self = shift;

    my $ret = $self->_bit_idx;

    $self->_bit_idx($ret+1);

    return $ret;
}

sub write
{
    my ($self, $len, $data) = @_;

    while ($len)
    {
        vec(${$self->_bits_ref()}, $self->_next_idx(), 1) = ($data & 0b1);
    }
    continue
    {
        $len--;
        $data >>= 1;
    }

    return;
}

sub get_bits
{
    my $self = shift;

    return ${$self->_bits_ref()};
}

package BitReader;

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(bits _bit_idx)]);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->bits($args->{bits});

    $self->_bit_idx(0);

    return;
}

sub _next_idx
{
    my $self = shift;

    my $ret = $self->_bit_idx;

    $self->_bit_idx($ret+1);

    return $ret;
}

sub read 
{
    my ($self, $len) = @_;

    my $idx = 0;
    my $ret = 0;
    while ($idx < $len)
    {
        $ret |= (vec($self->bits(), $self->_next_idx(), 1) << $idx);
    }
    continue
    {
        $idx++;
    }

    return $ret;
}

package Games::Solitaire::FC_Solve::DeltaStater;

use base 'Games::Solitaire::Verify::Base';

my $two_fc_variant = Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

$two_fc_variant->num_freecells(2);

__PACKAGE__->mk_acc_ref([qw(_derived_state _init_state _columns_initial_lens)]);

sub _get_column_orig_num_cards
{
    my ($self, $col) = @_;

    my $num_cards = $col->len();

    CALC_NUM_CARDS:
    while ($num_cards >= 2)
    {
        my $child_card = $col->pos($num_cards - 1);
        my $parent_card = $col->pos($num_cards - 2);
        if (!
            (($child_card->rank()+1 == $parent_card->rank())
                && ($child_card->color() ne $parent_card->color()))
        )
        {
            last CALC_NUM_CARDS;
        }
    }
    continue
    {
        $num_cards--;
    }

    if ($num_cards == 1)
    {
        $num_cards = 0;
    }

    return $num_cards;
}

sub _init
{
    my ($self, $args) = @_;

    $self->_init_state(
        Games::Solitaire::Verify::State->new(
            {
                string => $args->{init_state_str},
                variant => 'custom',
                variant_params => $two_fc_variant,
            },
        )
    );

    my $init_state = $self->_init_state;

    my @columns_initial_bit_lens;

    foreach my $col_idx (0 .. $init_state->num_columns() - 1)
    {
        my $num_cards = $self->_get_column_orig_num_cards(
            $init_state->get_column($col_idx)
        );

        my $bitmask = 1;
        my $num_bits = 0;

        while ($bitmask <= $num_cards)
        {
            $num_bits++;
            $bitmask <<= 1;
        }

        push @columns_initial_bit_lens, $num_bits;
    }

    $self->_columns_initial_lens(\@columns_initial_bit_lens);

    return;
}

sub set_derived
{
    my ($self, $args) = @_;

    $self->_derived_state(
        Games::Solitaire::Verify::State->new(
            {
                string => $args->{state_str},
                variant => 'custom',
                variant_params => $two_fc_variant,
            }
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

    return [map { [4 => $self->_derived_state->get_foundation_value($_, 0)] } @suits];
}

sub _get_suit_bit
{
    my ($self, $card) = @_;

    my $suit = $card->suit();

    return (($suit eq 'H' || $suit eq 'C') ? 0 : 1);
}

my %suit_to_idx = do { 
    my $s = Games::Solitaire::Verify::Card->get_suits_seq();
    (map { $s->[$_] => $_ } (0 .. $#$s)) ; 
};

sub _get_card_bitmask
{
    my ($self, $card) = @_;

    return ($card->rank() | ($suit_to_idx{$card->suit()} << 4));
}

my $COL_TYPE_EMPTY = 0;
my $COL_TYPE_ENTIRELY_NON_ORIG = 1;
my $COL_TYPE_HAS_ORIG = 2;

sub _get_column_encoding_composite
{
    my ($self, $col_idx) = @_;

    my $derived = $self->_derived_state();

    my $col = $derived->get_column($col_idx);

    my $num_orig_cards = $self->_get_column_orig_num_cards($col);

    my $col_len = $col->len();
    my $num_derived_cards = $col_len - $num_orig_cards;

    my $num_cards_in_seq = $num_derived_cards;
    my @init_card;
    if (($num_orig_cards == 0) && $num_derived_cards)
    {
        @init_card = ([6 => $self->_get_card_bitmask($col->pos(0))]);
        $num_cards_in_seq--;
    }

    return
    {
        type => (
              ($col_len == 0) ? $COL_TYPE_EMPTY
            : $num_orig_cards ? $COL_TYPE_HAS_ORIG
            :                   $COL_TYPE_ENTIRELY_NON_ORIG
        ),
        enc =>
        [
            [$self->_columns_initial_lens->[$col_idx] => $num_orig_cards], 
            [4 => $num_derived_cards ],
            @init_card,
            (
                map { [1 => $self->_get_suit_bit($col->pos($_))] }
                ($col_len - $num_cards_in_seq .. $col_len - 1)
            ),
        ],
    };
}

sub get_column_encoding
{
    my ($self, $col_idx) = @_;

    return $self->_get_column_encoding_composite($col_idx)->{enc};
}

sub get_freecells_encoding
{
    my ($self) = @_;

    my $derived = $self->_derived_state();

    return [
        map {
            my $card = $derived->get_freecell($_);
            [ 6 => (defined($card) ? $self->_get_card_bitmask($card) : 0) ]
        } (0 .. $derived->num_freecells()-1)
    ];
}

sub encode
{
    my ($self) = @_;

    my $bit_writer = BitWriter->new;

    foreach my $bit_spec (
        @{$self->get_freecells_encoding()},
        (map { @{$self->get_column_encoding($_)} } (0 .. $self->_derived_state->num_columns - 1)),
    )
    {
        $bit_writer->write( $bit_spec->[0] => $bit_spec->[1] );
    }

    return $bit_writer->get_bits();
}

sub decode
{
    my ($self, $bits) = @_;

    my $bit_reader = BitReader->new({ bits => $bits });

    my %foundations = (map { $_ => 14 } @suits);

    my $process_card = sub {
        my $card = shift;

        if ($card->rank() < $foundations{$card->suit()})
        {
            $foundations{$card->suit()} = $card->rank();
        }

        return $card;
    };

    my $process_card_bits = sub {
        my $card_bits = shift;

        my $card = Games::Solitaire::Verify::Card->new;
        $card->rank($card_bits & ((1 << 4)-1));
        $card->suit($suits[$card_bits >> 4]);

        return $process_card->($card);
    };

    my $num_freecells = $self->_init_state->num_freecells();
    # Read the Freecells.
    my $freecells = Games::Solitaire::Verify::Freecells->new({count => $num_freecells});

    foreach my $freecell_idx (0 .. $num_freecells - 1)
    {
        my $card_bits = $bit_reader->read(6);

        if ($card_bits != 0)
        {
            $freecells->assign($freecell_idx, $process_card_bits->($card_bits));
        }
    }

    my @columns;

    foreach my $col_idx (0 .. $self->_init_state->num_columns - 1)
    {
        my $col = Games::Solitaire::Verify::Column->new({cards => []});
        
        my $num_orig_cards = $bit_reader->read($self->_columns_initial_lens->[$col_idx]);
        
        my $orig_col = $self->_init_state->get_column($col_idx);
        foreach my $i (0 .. $num_orig_cards-1)
        {
            $col->push(
                $process_card->(
                    $orig_col->pos($i)->clone()
                ),
            );
        }

        my $num_derived_cards = $bit_reader->read(4);
        my $num_cards_in_seq = $num_derived_cards;

        if (($num_orig_cards == 0) && $num_derived_cards)
        {
            my $card_bits = $bit_reader->read(6);
            $col->push($process_card_bits->($card_bits));
            $num_cards_in_seq--;
        }

        if ($num_cards_in_seq)
        {
            my $last_card = $col->pos(-1);
            for my $i (0 .. $num_cards_in_seq - 1)
            {
                my $new_card = Games::Solitaire::Verify::Card->new;
                my $suit_bit = $bit_reader->read(1);

                $new_card->rank($last_card->rank()-1);
                $new_card->suit(
                    $suits[
                    (($suit_bit << 1) | ($last_card->color eq "red" ? 1 : 0))
                    ]
                );

                $col->push(
                    $process_card->($last_card = $new_card)
                );
            }
        }
        
        push @columns, $col;
    }

    my $foundations_obj = Games::Solitaire::Verify::Foundations->new(
        {
            num_decks => 1,
        },
    );

    foreach my $found (keys(%foundations))
    {
        $foundations_obj->assign($found, 0, $foundations{$found}-1);
    }

    my $state =
        Games::Solitaire::Verify::State->new(
            {
                variant => 'custom',
                variant_params => $two_fc_variant,
            }
        );

    foreach my $col (@columns)
    {
        $state->add_column($col);
    }

    $state->set_freecells($freecells);

    $state->set_foundations($foundations_obj);

    return $state;
}

1;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2011 Shlomi Fish

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

