#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 6;

my $range_solver = $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve";

# TEST
ok (!system($range_solver, "1", "20", "10", "-l", "gi"), "Range solver was successful");

# TEST
ok (!system($range_solver, "1", "2", "1", "-opt"), "Range solver with -opt was successful");

# Apparently, the excessive creation of new soft_threads for the optimization
# thread caused next_soft_thread_id to overflow.
# This checks against it.

# TEST
ok (!system($range_solver, "1", "64", "1", "-l", "cool-jives", "-opt"), 
    "Range solver with -opt and a large number of boards was succesful."
);

{
    my $output = `$range_solver 1 3 1 -mi 1`;

    foreach my $deal (1 .. 3)
    {
        # TEST*3
        like($output, qr{^Intractable Board No. \Q$deal\E}ms,
            "Boards are marked as intractable given --max-iters"
        );
    }
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

