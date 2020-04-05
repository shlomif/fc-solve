#!/usr/bin/env perl

use 5.014;
use strict;
use warnings;
use autodie;

use Getopt::Long qw/ GetOptions /;
use Path::Tiny qw/path/;

# require Math::Random::MT;
for my $deal ( 1 .. 1e9 )
{
    say "Reached deal = $deal";

    # my $gen  = Math::Random::MT->new($deal);
    # my $step = 1000;
    for my $base ( 0 .. 1_000 )
    {
        say "Reached deal = $deal ; base = $base" if $base % 100 == 0;
        my $mi = $base;
        my $out =
`./board_gen/pi-make-microsoft-freecell-board -t $deal | ./fc-solve -l ve -mi $mi -p -t -sam -sel`;
        my ($iters) = $out =~ /\nTotal number of states checked is ([0-9]+)\./
            or die "fail $deal $base";

        if ( $out =~ m%Iterations count exceeded% )
        {
            die "fail2 $deal $base $iters $mi" if $iters != $mi;
        }
        elsif ( $out =~ m%This game is solv% )
        {
            die "fail3 $deal $base $iters $mi" if $iters > $mi;
        }
        elsif ( $out =~ m%I could not solve this game% )
        {
            die "fail4 $deal $base" if $iters > $mi;
        }
        else
        {
            say $out;
            die "fail5 $deal $base";
        }
    }
}

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2019 by Shlomi Fish

This program is distributed under the MIT / Expat License:
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
