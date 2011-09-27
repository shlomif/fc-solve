#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use Test::Differences;

use Games::Solitaire::Verify::Solution;

package FCS::DeltaStater;

use base 'Games::Solitaire::Verify::Base';

my $two_fc_variant = Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

$two_fc_variant->num_freecells(2);

__PACKAGE__->mk_acc_ref([qw(_derived_state _init_state)]);

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

sub get_foundations_bits
{
    my ($self) = @_;

    return [map { [4 => $self->_derived_state->get_foundation_value($_, 0)] } @suits];
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
}

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2008 Shlomi Fish

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

