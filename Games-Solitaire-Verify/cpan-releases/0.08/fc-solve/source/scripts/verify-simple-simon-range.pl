#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use Data::Dumper;

use Games::Solitaire::Verify::Solution;

my $dump = io()->file("total_dump.txt");

LINES_LOOP:
while (!$dump->eof())
{
    my @l = (map { $dump->chomp()->getline() } (1 .. 4)))
    my ($deal) = ($l[0] =~ m{\A(\d+):});
    print "Testing Deal No. $deal\n";

    if ($l[1] !~ m{This game is solveable})
    {
        next LINES_LOOP;
    }

    my $variant = "simple_simon";
    open my $fc_solve_output,
        +("make_pysol_freecell_board.py $deal $variant | " . 
        "./fc-solve -g $variant -p -t -sam - |")
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $fc_solve_output,
            variant => $variant,
        },
    );

    my $verdict = $solution->verify();

    if ($verdict)
    {
        print("Verdict == " . Dumper($verdict));
        Carp::confess("Deal No. $deal failed verification");
    }

    close($fc_solve_output);
}

