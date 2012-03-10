#!/usr/bin/perl

use strict;
use warnings;

use lib './t/lib';

use Test::More tests => 19;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use Test::Differences;

use Games::Solitaire::FC_Solve::DeltaStater;

package main;

my $RANK_J = 11;
my $RANK_Q = 12;
my $RANK_K = 13;

{
my @suits = (qw(H C D S));
my %suit_to_idx = do { 
    my $s = Games::Solitaire::Verify::Card->get_suits_seq();
    (map { $s->[$_] => $_ } (0 .. $#$s)) ; 
};

sub _card_to_bin
{
    my ($args) = @_;

    my $suit = $args->{suit};
    my $rank = $args->{rank};

    if (!exists($suit_to_idx{$suit}))
    {
        die "Unknown suit '$suit'.";
    }
    return ($suit_to_idx{$suit} | ($rank << 2));
}

}


{
    # MS Freecell No. 982 Initial state.
    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
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
            [ 6 => _card_to_bin({suit => 'S', rank => 9,}) ], # 9S
        ],
        'get_column_lengths_bits() works',
    );

    # TEST
    eq_or_diff(
        $delta->get_freecells_encoding(),
        [
            [ 6 => _card_to_bin({suit => 'D', rank => 8}), ],  # 8D
            [ 6 => _card_to_bin({suit => 'D', rank => $RANK_Q}), ], # QD
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

{
    my $bit_reader = BitReader->new({ bits => chr(3 | (4 << 3))});

    # TEST
    ok ($bit_reader, 'Init bit_reader');
    
    # TEST
    is ($bit_reader->read(3), 3, 'bit_reader->read(3)');

    # TEST
    is ($bit_reader->read(4), 4, 'bit_reader->read(4)');
}

{
    # MS Freecell No. 982 Initial state.
    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
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
        scalar($delta->decode($delta->encode())->to_string()),
        <<'EOF',
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
        'decode() works.',
    );

    # TEST
    eq_or_diff(
        scalar($delta->decode($delta->encode_composite())->to_string()),
        <<'EOF',
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
        'encode_composite() test No. 1.',
    );
}

# More encode_composite tests - this time from the output of:
# pi-make-microsoft-freecell-board -t 24 | ./fc-solve -to 01ABCDE --freecells-num 2 -s -i -p -t

{
    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
        {
            init_state_str => <<'EOF'
Foundations: H-0 C-0 D-0 S-0 
Freecells:        
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF
        }
    );

    # TEST
    ok ($delta, 'Object was initialized correctly.');

    $delta->set_derived(
        {
            state_str => <<'EOF'
Foundations: H-0 C-0 D-0 S-4 
Freecells:  KS  TD
: 2C
: 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S
: QC 9S 6H 9H 8C 7D 6C 5D 4C 3D
: 
: 2D KD QS JH TC 9D 8S
: 7H JS KH TS KC 7C 6D 5C 4D
: AH 5S 6S AD 8H JD
: 2H

EOF
        }
    );

    # TEST
    eq_or_diff(
        scalar($delta->decode($delta->encode_composite())->to_string()),
        <<'EOF',
Foundations: H-0 C-0 D-0 S-4 
Freecells:  KS  TD
: 
: 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S
: QC 9S 6H 9H 8C 7D 6C 5D 4C 3D
: 2H
: 2D KD QS JH TC 9D 8S
: 7H JS KH TS KC 7C 6D 5C 4D
: AH 5S 6S AD 8H JD
: 2C
EOF
        'encode_composite() test No. 2.',
    );

    $delta->set_derived(
        {
            state_str => <<'EOF'
Foundations: H-0 C-0 D-0 S-2 
Freecells:  TD  4C
: 8S
: 5H QH 3C AC 3H 4H 3S 2H
: QC 9S 6H 9H 8C 7D 6C 5D 4S 3D 2C
: 5C 4D
: 2D KD QS JH TC 9D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: KS QD JC TH 9C 8D 7S 6D
EOF
        }
    );

    # TEST
    eq_or_diff(
        scalar($delta->decode($delta->encode_composite())->to_string()),
        <<'EOF',
Foundations: H-0 C-0 D-0 S-2 
Freecells:  TD  4C
: 5C 4D
: 5H QH 3C AC 3H 4H 3S 2H
: QC 9S 6H 9H 8C 7D 6C 5D 4S 3D 2C
: 8S
: 2D KD QS JH TC 9D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: KS QD JC TH 9C 8D 7S 6D
EOF
        'encode_composite() test No. 2.',
    );

}

# Make sure encode_composite avoids permutations of empty columns
# and completely-non-original states.
{
    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
        {
            init_state_str => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  KD    
: 
: 
: 
: KS QD
: 
: 
: 
: 
EOF
        },
    );

    $delta->set_derived(
        {
            state_str => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  QD  KD
: 
: 
: 
: 
: 
: KS
: 
: 
EOF
        },
    );

    my $calc_enc_state = sub {
        return join '', unpack("H*", $delta->encode_composite());
    };

    my $first_state = $calc_enc_state->();

    $delta->set_derived(
        {
            state_str => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  QD  KD
: 
: 
: 
: 
: 
: 
: 
: KS
EOF
        }
    );

    my $second_state = $calc_enc_state->();

    # TEST
    is (
        $first_state,
        $second_state,
        "Make sure encode_composite avoids permutations of empty columns and completely-non-original states.",
    );
}

# Make sure encode_composite avoids permutations of empty columns
# and completely-non-original states.
#
# Another edge-case.
{
    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
        {
            init_state_str => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  KD    
: 
: 
: 
: KS QD
: 
: 
: 
: 
EOF
        },
    );

    $delta->set_derived(
        {
            state_str => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  QD  KD
: KS
: 
: 
: 
: 
: 
: 
: 
EOF
        },
    );

    my $calc_enc_state = sub {
        return join '', unpack("H*", $delta->encode_composite());
    };

    my $first_state = $calc_enc_state->();

    $delta->set_derived(
        {
            state_str => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  QD  KD
: 
: 
: 
: 
: 
: 
: 
: KS
EOF
        }
    );

    my $second_state = $calc_enc_state->();

    # TEST
    is (
        $first_state,
        $second_state,
        "encode_composite avoids permutations - #2",
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

