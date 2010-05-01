#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 5;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;

use Games::Solitaire::Verify::Solution;

sub test_using_valgrind
{
    my $cmd_line_args = shift;
    my $blurb = shift;

    my $log_fn = "valgrind.log";

    system(
        "valgrind",
        "--track-origins=yes",
        "--leak-check=yes",
        "--log-file=$log_fn",
        $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve",
        @$cmd_line_args,
    );
    
    open my $read_from_valgrind, "<", $log_fn
        or die "Cannot open valgrind.log for reading";
    my $found_error_summary = 0;
    my $found_malloc_free = 0;
    LINES_LOOP:
    while (my $l = <$read_from_valgrind>)
    {
        if (index($l, q{ERROR SUMMARY: 0 errors from 0 contexts}) >= 0)
        {
            $found_error_summary = 1;
        }
        elsif (index($l, q{in use at exit: 0 bytes}) >= 0)
        {
            $found_malloc_free = 1;
        }
    }
    close ($read_from_valgrind);

    if (ok (($found_error_summary && $found_malloc_free), $blurb))
    {
        unlink($log_fn);
    }
    else
    {
        die "Valgrind failed";
    }
}

# TEST
test_using_valgrind(
    [qw(1 2 1 --method a-star)],
    qq{range-parallel-solve for board #2 using the BeFS method valgrind}
);

# TEST
test_using_valgrind(
    ["1", "2", "1", "-l", "gi"],
    qq{"range-parallel-solve 1 2 1 -l gi" returned no errors}
);

# TEST
test_using_valgrind(
    ["1", "2", "1", "-opt"],
    qq{"range-parallel-solve 1 2 1 -opt" returned no errors}
);

# TEST
test_using_valgrind(
    ["1", "2", "1", "--next-flare"],
    qq{"range-parallel-solve 1 2 1 --next-flare" returned no errors}
);

# TEST
test_using_valgrind(
    [qw(11981 11983 1 -opt)],
    qq{"range-parallel-solve 11981 11983 1 -opt" returned no errors}
);


=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

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

