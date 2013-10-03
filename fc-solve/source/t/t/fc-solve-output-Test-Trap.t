#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use File::Temp qw( tempdir );
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

my $fc_solve_exe = File::Spec->catfile($ENV{FCS_PATH}, 'fc-solve');

{
    trap
    {
        system(
            $fc_solve_exe, "--reset_junk_at_end",
            File::Spec->catfile(
                $ENV{FCS_SRC_PATH}, 't', 't', 'data',
                'sample-boards', '24-mid.board'
            ),
        );
    };

    my $needle = qq#Unknown option "--reset_junk_at_end". Type "$fc_solve_exe --help" for usage information.#;

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
        system(
            $fc_solve_exe, '--read-from-file4,amateur-star.sh',
            '--stacks-num', '7',
            File::Spec->catfile(
                $ENV{FCS_SRC_PATH}, 't', 't', 'data',
                'sample-boards', '24-mid.board'
            ),
        );
    };

    my $needle = qq#Unknown option "--read-from-file4,amateur-star.sh". Type "$fc_solve_exe --help" for usage information.#;

    # TEST
    like (
        $trap->stderr(),
        qr/^\Q$needle\E$/ms,
        "Option without space is not accepted."
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

