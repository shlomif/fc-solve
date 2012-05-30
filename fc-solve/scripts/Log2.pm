package Log2;

# Silly module I've written (and I think it would be better as a DSL):
#
# Sample invocation and result:
#
# $ perl -MLog2 -e 'plog2(4 => 14, 44 => 5, 4 => 1, 4 => 3)'
# log2(14) * 4 + log2(5) * 44 + log2(1) * 4 + log2(3) * 4 = 
#    123.734105866159 bits (bytes: 16)

use strict;
use warnings;

use POSIX qw(ceil);

our @EXPORT = (qw(plog2));

use base 'Exporter';

sub plog2
{
    my @specs = @_;

    my $total = 0;

    my @clauses;

    while (@specs)
    {
        my $count = shift(@specs);
        my $base = shift(@specs);

        push @clauses, "log2($base) * $count";
        $total += log($base) * $count;
    }

    my $num_bits = $total/log(2);
    my $num_bytes = ceil($num_bits / 8);
    print join(' + ', @clauses), " = $num_bits bits (bytes: $num_bytes)\n";
}

1;

__END__

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
