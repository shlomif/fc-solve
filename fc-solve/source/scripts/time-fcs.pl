#!/usr/bin/perl

# This script takes an ./mptest dump (such as:
#
#    ./mptest 1 32000 500 -l gi > dump.txt
#
# And calculates the time elapsing from start to finish.

use strict;
use warnings;

sub get_time
{
    my $l = shift;
    if ($l =~ m{ at (\d+\.\d+)})
    {
        return $1;
    }
    else
    {
        die "Cannot find a number at line '$l'";
    }
}

foreach my $file (@ARGV)
{
    open my $in, "<", $file
        or die "Cannot open $file";
    my @lines = <$in>;
    close($in);
    if (@ARGV > 1)
    {
        print "${file}:";
    }
    print +(get_time($lines[-1]) - get_time($lines[0])), "\n";
}



=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

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

