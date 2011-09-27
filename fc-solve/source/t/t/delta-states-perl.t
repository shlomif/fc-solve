#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;

use Games::Solitaire::Verify::Solution;

package FCS::DeltaStater;

use base 'Games::Solitaire::Verify::Base';

sub _init
{
    my ($self, $args) = @_;

    return;
}

package main;

{
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
    ok ($delta);
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

