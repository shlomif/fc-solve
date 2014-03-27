#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::CmdLine::Expand;
use FC_Solve::CmdLine::Simulate;
use Test::More tests => 4;

use Test::Differences;

sub check
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $input_argv = shift;
    my $want_argv = shift;
    my $msg = shift;

    my $obj = FC_Solve::CmdLine::Expand->new(
        {
            input_argv => $input_argv,
        }
    );

    eq_or_diff(
        $obj->argv(),
        $want_argv,
        $msg,
    );
}

# TEST
check(
    ['-s', '-i'],
    ['-s', '-i'],
    "Basic test",
);

# TEST
check(
    ['-l', 'cpb'],
    [
        split(/\s+/, <<'EOF'),
--flare-name 1 --method soft-dfs -to 0123456789 -opt
-nf --flare-name 2 --method soft-dfs -to 0123467 -opt
-nf --flare-name 3 --method random-dfs -seed 2 -to 0[01][23456789] -opt
-nf --flare-name 4 --method random-dfs -seed 1 -to 0[0123456789] -opt
-nf --flare-name 5 --method random-dfs -seed 3 -to 0[01][23467] -opt
-nf --flare-name 9 --method random-dfs -seed 4 -to 0[0123467] -opt
-nf --flare-name 10 --method random-dfs -to [01][23456789] -seed 8 -opt
-nf --flare-name 11 --method a-star -asw 0.2,0.8,0,0,0 -opt
-nf --flare-name 12 --method random-dfs -to [01][23456789] -seed 268 -opt
-nf --flare-name 15 --method random-dfs -to [0123456789] -seed 142 -opt
-nf --flare-name 16 --method a-star -asw 0.2,0.3,0.5,0,0 -opt
-nf --flare-name 17 --method random-dfs -to [01][23456789] -seed 5 -opt
-nf --flare-name 18 --method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -opt
-nf --flare-name 19 --method soft-dfs -to 0126394875 -opt
-nf --flare-name 20 --method random-dfs -seed 105 -opt
EOF
    '--flares-plan',
    'Run:6246@1,Run:2663@2,Run:6799@3,Run:7161@4,Run:3466@5,Run:3594@9,Run:6896@10,Run:7269@11,Run:7194@12,Run:6462@15,Run:7742@16,Run:7029@17,Run:3769@18,Run:5244@19,Run:7149@20',
    ],
    'With -l',
);

{
    my $obj = FC_Solve::CmdLine::Simulate->new(
        {
            input_argv => ['-l', 'cpb',],
        },
    );

    # TEST
    ok ($obj, "FC_Solve::CmdLine::Simulate was initialized.");

    # TEST
    is ($obj->get_flares_num(), 15, "There are 15 flares.");
}

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

