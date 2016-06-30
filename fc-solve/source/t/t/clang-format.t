#!/usr/bin/perl

use strict;
use warnings;

use File::Copy qw( copy );
use Test::More;
use File::Which qw( which );


if ($ENV{FCS_TEST_CLANG_FORMAT})
{
    my $fmt = which('clang-format');
    if ($fmt)
    {
        plan tests => 1;
        my $SRC_PATH = $ENV{FCS_SRC_PATH};
        my @filenames = grep { ! /\Qcmd_line_inc.h\E|\Qrate_state.c\E/ } grep { m#\A\Q$SRC_PATH\E/(?i:[a-z])# } sort {$a cmp $b} map { glob "$SRC_PATH/$_" } qw/*.c *.h *.cpp *.hpp/;
        foreach my $fn (@filenames)
        {
            copy($fn, "$fn.orig");
        }
        system($fmt, '-style=file', '-i', @filenames);
        my $all_ok = 1;
        foreach my $fn (@filenames)
        {
            if (system('cmp', '-s', $fn, "$fn.orig"))
            {
                diag("$fn is improperly formatted.");
                $all_ok = 0;
            }
            else
            {
                unlink("$fn.orig");
            }
        }
        ok ($all_ok, "Success - all files are properly formatted.");
    }
    else
    {
        plan skip_all => "No clang-format found.";
    }
}
else
{
    plan skip_all => "Environment variable to check was set.";
}

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2016 Shlomi Fish

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

