#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 19;
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

# TEST
vtest({id => "forecell1099default", deal => 1099, variant => "forecell", theme => [],},
    "Forecell Deal #1099"
);

# TEST
vtest({id => "relaxed_freecell11982",deal => 11982, variant => "relaxed_freecell", },
    "Relaxed Freecell Deal #11982"
);


# TEST
vtest(
    {
        id => "seahaven_towers1977fools-gold",
        deal => 1977,
        variant => "seahaven_towers",
        theme => ["-l", "fools-gold",],
    },
    "Seahaven Towers #1977"
);

# TEST
vtest({
        id => "eight_off200", deal => 200, variant => "eight_off",
    },
    "Eight Off #200 with -l gi"
);

# TEST
vtest(
    {id =>"eight_off200default", deal => 200, variant => "eight_off",
        theme => [],
    },
    "Eight Off #200 with default heuristic"
);

# TEST
vtest(
    {id => "simple_simon24default", deal => 24, variant => "simple_simon",
        theme => [],
    },
    "Simple Simon #24 with default theme",
);

# TEST
vtest(
    {id => "simple_simon19806default", deal => 19806, variant => "simple_simon",
        theme => [],
    },
    "Simple Simon #19806 with default theme",
);

# TEST
vtest(
    {id => "simple_simon1with_i", deal => 1, variant => "simple_simon",
        theme => ["-to", "abcdefghi"],
    },
    "Simple Simon #1 with seq-to-false-parent",
);

# TEST
vtest(
    {
        id => "simple_simon1with_next_instance",
        deal => 1,
        variant => "simple_simon",
        theme => ["-to", "abcdefgh", "--next-instance", "-to", "abcdefghi",],
    },
    "Simple Simon #1 using an --next-instance",
);

# TEST
vtest(
    {
        id => "freecell254076_l_by",
        deal => 254076,
        msdeals => 1,
        theme => ["-l", "by", "--scans-synergy", "dead-end-marks"],
    },
    "Freecell MS 254,076 while using -l by with dead-end-marks"
);

# TEST
vtest({id => "freecell24", deal => 24,
        output_file => "24.solution.txt",},
    "Verifying the solution of deal No. 24 with -o");

# TEST
vtest({id => "freecell24_children_playing_ball", deal => 24,
        theme => ["-l", "children-playing-ball"], with_flares => 1},
    "Verifying the solution of deal No. 24 with -l cpb");

# TEST
vtest({id => "freecell24_sentient_pearls", deal => 24,
        theme => ["-l", "sentient-pearls"], with_flares => 1},
    "Verifying the solution of deal No. 24 with -l sp");


# TEST
vtest({id => "freecell24with_empty_soft_thread_name", deal => 1,
        theme => ["-to", "013[2456789]", "-nst", "-l" ,"by",],
    },
    "Solving Deal #24 with an empty soft thread name"
);

# TEST
vtest(
    {
        id => "freecell1941",
        deal => 1941,
        theme => ["-l", "gi", "--cache-limit", "2000"],
    },
    "Verifying 1941 with --cache-limit set to a different value",
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
vtest({
    id => "qualified_seed_for_6240",
    deal => 6240,
    theme => ["-l", "qs", "-fif", "5", "--flares-choice", "fcpro",],
    with_flares => 1,
    },
    "Qualified seed test with -fif and --flares-choice fcpro",
);

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

