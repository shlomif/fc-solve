#!/usr/bin/perl

use strict;
use warnings;

# Load the plugin.
use Git::Repository 'Blame';

my $repository = Git::Repository->new();

# Get the git blame information.
my $blame_lines = $repository->blame( 'fc-solve/benchmarks/shlomif-core-i3-desktop-machine.txt' );

print "Epoch time\tBenchmark duration\tDeals Per Second\n";
foreach my $line (@$blame_lines)
{
    if (my ($timing) = $line->get_line =~ /\A([0-9]+\.[0-9]+)s\z/)
    {
        printf "%d\t%s\t%f\n", $line->get_commit_attributes->{'author-time'}, $timing, (32_000/$timing);
    }
}

=head1 COPYRIGHT & LICENSE

Copyright 2016 by Shlomi Fish

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
