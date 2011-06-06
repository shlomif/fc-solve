#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Test::Differences;

use Storable qw(retrieve);

my $data_dir = "./t/data/range-verifier/";

my $summary_file = "$data_dir/summary.txt";
my $stats_file = "$data_dir/summary.stats.perl-storable";

sub delete_summary
{
    unlink ($summary_file);
    unlink ($stats_file);

    return;
}

delete_summary();

my $ranger_verifier = $ENV{'FCS_PATH'} . '/scripts/verify-range-in-dir-and-collect-stats.pl';

# TEST
ok (!system(
        $^X, $ranger_verifier,
    '--summary-lock', "$data_dir/summary.lock",
    '--summary-stats-file', $stats_file,
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

    return [split(/\n/, $string, -1)];
}

# TEST
eq_or_diff(
    _mysplit(_slurp($summary_file)),
    _mysplit(<<'EOF'),
Solved Range: Start=1 ; End=10
EOF
    "Summary file for Baker's Game Range 1-10 is Proper.",
);

# TEST
eq_or_diff(
    retrieve($stats_file),
    {
        counts =>
        { 
            solved => 
            { 
                iters =>
                {map { $_ => 1 } (73,145,146,164,453,726,815,1076,1213)}, 
                gen_states => {map { $_ => 1 } (106, 184, 187, 205, 500, 790, 870, 1100, 1246)},
                sol_lens => {(map { $_ => 1 } (97,98,100,109,112,122,124)), 119 => 2}, 
            }, 
            unsolved =>
            {
                iters => {3436 => 1,},
                gen_states => {3436 => 1,} 
            },
        },
    },
    "Statistics are OK.",
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

