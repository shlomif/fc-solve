#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 9;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use Test::Differences;

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

package FCS::DeltaStater;

use base 'Games::Solitaire::Verify::Base';

my $two_fc_variant = Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

$two_fc_variant->num_freecells(2);

__PACKAGE__->mk_acc_ref([qw(_derived_state _init_state _columns_initial_lens)]);

sub _get_column_orig_num_cards
{
    my ($self, $state, $col_idx) = @_;

    my $col = $state->get_column($col_idx);
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
        my $num_cards = $self->_get_column_orig_num_cards($init_state, $col_idx);

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

sub get_column_encoding
{
    my ($self, $col_idx) = @_;

    my $derived = $self->_derived_state();

    my $col = $derived->get_column($col_idx);

    my $num_orig_cards = $self->_get_column_orig_num_cards($derived, $col_idx);

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
    [ 
        [$self->_columns_initial_lens->[$col_idx] => $num_orig_cards], 
        [4 => $num_derived_cards ],
        @init_card,
        (
            map { [1 => $self->_get_suit_bit($col->pos($_))] }
            ($col_len - $num_cards_in_seq .. $col_len - 1)
        ),
    ];
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

package main;

{
    # MS Freecell No. 982 Initial state.
    my $delta = FCS::DeltaStater->new(
        {
            init_state_str => <<'EOF'
Foundations: H-0 C-0 D-A S-0 
Freecells:        
: 6D 3C 3H KD 8C 5C
: TC 9C 9H 4S JC 6H 5H
: 2H 2D 3S 5D 9D QS KS
: 6S TD QC KH AS AH 7C
: KC 4H TH 7S 2C 9S
: AC QD 8D QH 3D 8S
: 7H 7D JD JH TS 6C
: 4C 4D 5S 2S JS 8H
EOF
        }
    );

    # TEST
    ok ($delta, 'Object was initialized correctly.');

    $delta->set_derived(
        {
            state_str => <<'EOF'
Foundations: H-0 C-2 D-A S-0 
Freecells:  8D  QD
: 6D 3C 3H KD 8C 5C
: TC 9C 9H 8S
: 2H 2D 3S 5D 9D QS KS QH JC
: 6S TD QC KH AS AH 7C 6H
: KC 4H TH 7S
: 9S
: 7H 7D JD JH TS 6C 5H 4S 3D
: 4C 4D 5S 2S JS 8H
EOF
        }
    );

    # TEST
    eq_or_diff(
        $delta->get_foundations_bits(),
        # Given as an array reference of $num_bits => $bits array refs.
        [
            [4 => 0,], # Hearts
            [4 => 2,], # Clubs
            [4 => 1,], # Diamonds
            [4 => 0,], # Spades
        ],
        'get_foundations_bits works',
    );

    # TEST
    eq_or_diff(
        $delta->get_column_encoding(0),
        [
            [ 3 => 6 ], # Orig len.
            [ 4 => 0 ], # Derived len. 
        ],
        'get_column_lengths_bits() works',
    );

    my $HC = [ 1 => 0, ];
    my $DS = [ 1 => 1, ];
    # TEST
    eq_or_diff(
        $delta->get_column_encoding(1),
        [
            [ 3 => 3 ], # Orig len.
            [ 4 => 1 ], # Derived len. 
            $DS, # 8S
        ],
        'get_column_lengths_bits() works',
    );
    # TEST
    eq_or_diff(
        $delta->get_column_encoding(5),
        [
            [ 3 => 0 ], # Orig len.
            [ 4 => 1 ], # Derived len. 
            [ 6 => (9 | (3 << 4)) ], # 9S
        ],
        'get_column_lengths_bits() works',
    );

    # TEST
    eq_or_diff(
        $delta->get_column_encoding(1),
        [
            [ 3 => 3 ], # Orig len.
            [ 4 => 1 ], # Derived len.
            $DS, # 8S
        ],
        'column No. 1',
    );

    # TEST
    eq_or_diff(
        $delta->get_freecells_encoding(),
        [
            [ 6 => (8 | (2 << 4)) ],  # 8D
            [ 6 => (12 | (2 << 4)) ], # QD
        ],
        'Freecells',
    );
}

{
    my $bit_writer = BitWriter->new;

    # TEST
    ok ($bit_writer, 'Init bit_writer');

    $bit_writer->write(4 => 5);
    $bit_writer->write(2 => 1);

    # TEST
    eq_or_diff(
        $bit_writer->get_bits(),
        chr(5 | (1 << 4)),
        "write() test",
    );
}

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

