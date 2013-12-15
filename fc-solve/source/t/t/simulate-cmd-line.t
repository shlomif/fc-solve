#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::CmdLineSimulation;
use Test::More tests => 1;

use Test::Differences;

sub check
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $input_argv = shift;
    my $want_argv = shift;
    my $msg = shift;

    my $obj = FC_Solve::CmdLineSimulation->new(
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

