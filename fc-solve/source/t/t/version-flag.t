#!/usr/bin/perl

use strict;
use warnings;

use autodie;

use Test::More tests => 2;
use String::ShellQuote;

use File::Spec;
use File::Basename qw( dirname );

{
    open my $good_ver_fh, "<", File::Spec->catdir( dirname( __FILE__),
        (( File::Spec->updir() ) x 2), "ver.txt");
    my $good_ver = <$good_ver_fh>;
    close($good_ver_fh);
    chomp($good_ver);

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    my $got_output = `$fc_solve_exe --version`;

    my $got_status = $?;
    # TEST
    ok (!$got_status, "fc-solve --version ran fine");

    # TEST
    like (
        $got_output,
        qr{^libfreecell-solver version \Q$good_ver\E}ms,
        "Version is contained in the --version display",
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

