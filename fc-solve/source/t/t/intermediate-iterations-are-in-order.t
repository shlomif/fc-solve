#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;


# 
# This test verifies that when doing ./fc-solve -s -i , the iterations
# number are precisely ascending and don't repeat themselves.
sub verify_solution_test
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
    my $diag;
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
                        $diag = "The iterations do not start at 0.";
                    }
                }
                else
                {
                    if ($new_iter != $last_iter+1)
                    {
                        $verdict = 0;
                        $diag = "Iteration $new_iter does not directly follow $last_iter";
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

    my $test_verdict = ok($verdict, $msg);
    if (defined($diag))
    {
        diag($diag);
    }

    close($fc_solve_output);

    return $test_verdict;
}

# TEST
verify_solution_test({deal => 24, theme => [],}
    , "Verifying the trace of deal #24");

# TEST
verify_solution_test({deal => 1941, theme => [],}, 
    "Verifying 1941 (The Hardest Deal) with the default algorithm");

# TEST
verify_solution_test({deal => 1941}, 
    "Verifying 1941 (The Hardest Deal) with '-l gi'");
