#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 12;
use File::Spec;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );
use FC_Solve::Paths qw( bin_board bin_exe_raw samp_board $FC_SOLVE__RAW );

my $MID24_BOARD = samp_board('24-mid.board');

{
    trap
    {
        system( $FC_SOLVE__RAW, "--reset_junk_at_end", $MID24_BOARD );
    };

    my $needle = qq#Unknown option "--reset_junk_at_end". Type "$FC_SOLVE__RAW --help" for usage information.#;

    # TEST
    like (
        $trap->stderr(),
        qr/^\Q$needle\E$/ms,
        "Unknown option was recognized, for one that has a valid prefix.",
    );

    # TEST
    unlike (
        $trap->stdout(),
        qr/\S/,
        "Empty standard output due to unrecognized option.",
    );
}

{
    trap
    {
        system( $FC_SOLVE__RAW, '--read-from-file4,amateur-star.sh',
            '--stacks-num', '7', $MID24_BOARD );
    };

    my $needle = qq#Unknown option "--read-from-file4,amateur-star.sh". Type "$FC_SOLVE__RAW --help" for usage information.#;

    # TEST
    like (
        $trap->stderr(),
        qr/^\Q$needle\E$/ms,
        "Option without space is not accepted."
    );
}

{
    trap
    {
        system(
            bin_exe_raw(['freecell-solver-fc-pro-range-solve']), '1', '3', '1',
            '-l', 'as',
        );
    };

    my @lines = split(/\n/, $trap->stdout());

    # TEST
    is_deeply(
        [grep
            { /\A\[\[Num FCS Moves\]\]=([0-9]+)\z/ ? (not ($1 > 0)) :
                (die "Incorrect $_");
            }
            grep { /\A\[\[Num FCS Moves\]\]/ }
            @lines
        ],
        [],
        "All FCS Move counts are valid",
    );

    # TEST
    is_deeply(
        [grep
            { /\A\[\[Num FCPro Moves\]\]=([0-9]+)\z/ ? (not ($1 > 0)) :
                (die "Incorrect $_");
            }
            grep { /\A\[\[Num FCPro Moves\]\]/ }
            @lines
        ],
        [],
        "All FCPro Move counts are valid.",
    );
}

{
    trap
    {
        system(
            bin_exe_raw(['summary-fc-solve']) , 1591, 1592, 1593,
            qw(-- --method random-dfs -to [0123456789] -sp r:tf -opt -opt-to 0123456789ABCDE -seed 24 -mi 10000)
        );
    };

    my $out = $trap->stdout();
    # TEST
    like (
        $out,
        qr/^1591 = Verdict: Intractable.*?^1592 = Verdict: Solved.*?^1593 = Verdict: Solved/ms,
        "All deals in summary-fc-solve are either intractable or solved.",
    );

    # TEST
    unlike (
        $out,
        qr/Verdict: Unsolved/,
        'No deal is unsolved, because that makes no sense.',
    );
}

{
    my $status;

    trap
    {
        $status = system(
            $FC_SOLVE__RAW,
qw#
--flare-name prefix_of_a_long_name --method soft-dfs -to 0123456789 -sp r:tf -opt -opt-to 0123456789ABCDE
-nf --flare-name another_long_name --method soft-dfs -to 0123467 -sp r:tf -opt -opt-to 0123456789ABCDE
--flares-plan
# , 'Run:100@prefix,Run:200@another_long_name',
            bin_board('24.board'),
        );
    };

    my $needle = q#Flares Plan: Unknown flare name.#;

    # TEST
    like (
        $trap->stderr(),
        qr/^\Q$needle\E$/ms,
        q#Cannot use a prefix of a flare's name as the name in the flares plan.#,
    );

    # TEST
    ok (
        scalar ($status != 0),
        'Exited with a non-zero exit code.',
    );

    # TEST
    is (
        $trap->stdout(),
        '',
        'Empty standard output on flares plan error.'
    );
}

{
    trap
    {
        system( $FC_SOLVE__RAW, "-mi", 1000, samp_board('24-with-stray-d-char.board') );
    };

    my $needle = q#Not enough input.#;

    # TEST
    like (
        $trap->stderr(),
        qr/^\Q$needle\E$/ms,
        "Invalid card format.",
    );

    # TEST
    unlike (
        $trap->stdout(),
        qr/\S/,
        "Empty standard output due to invalid card format.",
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

