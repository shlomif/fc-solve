#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use Test::Differences;

my $data_dir = "./t/data/range-verifier/";

my $summary_file = "$data_dir/summary.txt";

sub delete_summary
{
    unlink ($summary_file);

    return;
}

delete_summary();

my $ranger_verifier = $ENV{'FCS_PATH'} . '/scripts/verify-range-in-dir-and-collect-stats.pl';

# TEST
ok (!system(
        $^X, $ranger_verifier,
    '--summary-lock', "$data_dir/summary.lock",
    '--summary-file', $summary_file,
    '-g', 'bakers_game',
    '--min-idx', '1', '--max-idx', '10',
    "$data_dir/bakers-game-solutions-dir",
    ),
    "Script was run successfully.",
);

sub _slurp
{
    my $filename = shift;

    open my $in, "<", $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

sub _mysplit
{
    my $string = shift;

    return [split(/;\s*/, $string, -1)];
}

# TEST
eq_or_diff(
    _mysplit(_slurp($summary_file)),
    _mysplit(<<'EOF'),
Range[1->10]: {{{ Moments['solved'/'iters'] = [9,4811,4100221,4058458523,4268134909957,4648965759709931,5185500695222621461,5882844148227533651479]; 
Min['solved'/'iters'] = [73];
Max['solved'/'iters'] = [1213];
Moments['solved'/'gen_states'] = [9,5188,4515602,4564551784,4903565758274,5451942758060008,6206095384941163202,7188433984991667343288]; 
Min['solved'/'gen_states'] = [106];
Max['solved'/'gen_states'] = [1246];
Moments['unsolved'/'iters'] = [1,3436,11806096,40565745856,139383902761216,478923089887538176,1645579736853580000000,5654211975828900000000000]; 
Min['unsolved'/'iters'] = [3436];
Max['unsolved'/'iters'] = [3436];
Moments['unsolved'/'gen_states'] = [1,3436,11806096,40565745856,139383902761216,478923089887538176,1645579736853580000000,5654211975828900000000000];
Min['unsolved'/'gen_states'] = [3436];
Max['unsolved'/'gen_states'] = [3436];
}}}
EOF
    "Summary file for Baker's Game Range 1-10 is Proper.",
);

# Clean up after everything.
delete_summary();

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

