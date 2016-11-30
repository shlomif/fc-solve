#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use File::Spec ();
use FC_Solve::Paths qw( bin_board data_file samp_board samp_sol );
use FC_Solve::CheckResults ();

my $v = FC_Solve::CheckResults->new(
    {
        data_filename => data_file(['digests-and-lens-storage.yml']),
    }
);

sub vtest
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return $v->vtest(@_);
}

# TEST
vtest(
    {
        id => "375783-dbm-sol",
        deal => 375_783,
        complete_command =>
        "$^X $ENV{FCS_SRC_PATH}/scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl --freecells-num=2 @{[samp_sol('375783.dbm-sol')]}"
    },
    "Verifying the output of scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl",
);

#
# We discovered that Freecell Solver did not properly handle the foundations
# with 0's like "H-0" , "S-0", etc. This is as opposed to what exists in the
# README.txt file.
#
# This test aims to fix it.

# TEST
vtest({id => "freecell24_board_with_founds_0", deal => 24,
        board => samp_board( 'ms24-with-founds-0.board'),
    }, "Properly handle foundations like H-0 S-0 etc.");

# TEST
vtest(
    {
        id => "freecell24",
        deal => 24,
        board => bin_board('24.no-newline.board'),
    },
    "Handle initial layouts without a trailing newline."
);

# Store the changes at the end so they won't get lost.
$v->end();


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

