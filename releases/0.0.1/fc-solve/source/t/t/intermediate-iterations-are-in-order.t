#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 8;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;


#
# This test verifies that when doing ./fc-solve -s -i , the iterations
# number are precisely ascending and don't repeat themselves.
sub assert_directly_ascending_iters
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;
    my $msg = shift;

    my $board = $args->{board};
    my $deal = $args->{deal};

    if (! defined($board))
    {
        if (!defined($deal))
        {
            confess "Neither Deal nor board are specified";
        }
        if ($deal !~ m{\A[1-9][0-9]*\z})
        {
            confess "Invalid deal $deal";
        }
    }

    my $theme = $args->{theme} || ["-l", "gi"];

    my $variant = $args->{variant}  || "freecell";
    my $is_custom = ($variant eq "custom");
    my $variant_s = $is_custom ? "" : "-g $variant";

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    open my $fc_solve_output,
        ($board ? "" : "make_pysol_freecell_board.py $deal $variant | ") .
        "$fc_solve_exe $variant_s " . shell_quote(@$theme) . " -s -i -p -t -sam " .
        ($board ? shell_quote($board) : "") .
        " |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    my $verdict = 1;
    my $diag = "";
    {
        my $last_iter;
        LINE_LOOP:
        while (my $line = <$fc_solve_output>)
        {
            chomp($line);
            if ($line =~ m{\AIteration: (\d+)\z})
            {
                my $new_iter = $1;
                if (!defined($last_iter))
                {
                    if ($new_iter ne "0")
                    {
                        $verdict = 0;
                        $diag .= "The iterations do not start at 0.";
                    }
                }
                else
                {
                    if ($new_iter != $last_iter+1)
                    {
                        $verdict = 0;
                        $diag .= "Iteration $new_iter does not directly follow $last_iter";
                    }
                }
                $last_iter = $new_iter;
                if (!$verdict)
                {
                    last LINE_LOOP;
                }
            }
        }
    }

    if (! close($fc_solve_output))
    {
        $verdict = 0;
        $diag .= "Process failed";
    }

    my $test_verdict = ok($verdict, $msg);
    if (length($diag))
    {
        diag($diag);
    }

    return $test_verdict;
}

# TEST
assert_directly_ascending_iters({deal => 24, theme => [],}
    , "Verifying the trace of deal #24");

# TEST
assert_directly_ascending_iters({deal => 1941, theme => [],},
    "Verifying 1941 (The Hardest Deal) with the default algorithm");

# TEST
assert_directly_ascending_iters({deal => 1941},
    "Verifying 1941 (The Hardest Deal) with '-l gi'");

# TEST
assert_directly_ascending_iters({deal => 24},
    "Verifying deal No. 24 with '-l gi'");

# TEST
assert_directly_ascending_iters(
    {deal => 100, theme => [qw(-l john-galt-line)],},
    "Verifying deal No. 100 with '-l john-galt-line' to check a rotating theme"
);

# TEST
assert_directly_ascending_iters(
    {deal => 11982, },
    "Verifying deal No. 11982 (unsolvable) with -l gi"
);

# TEST
assert_directly_ascending_iters(
    {deal => 2400, theme => [qw(-l fools-gold)],},
    "Verifying deal No. 2400 with an atomic moves preset (fools-gold)"
);

# TEST
assert_directly_ascending_iters(
    {deal => 11982, theme => [qw(-l fools-gold)],},
    "Verifying unsolvable deal No. 11982 with an atomic moves preset (fools-gold)"
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

