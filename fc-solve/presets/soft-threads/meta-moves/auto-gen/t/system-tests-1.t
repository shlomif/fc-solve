#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

use Test::More tests => 7;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

use Carp;

{
    trap {    
        # TEST
        ok (!system("perl process.pl > output.got"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    # TEST
    ok (!system("cmp", "-s", "output.good", "output.got"),
        "results are identical"
    );
}

=head2 Format of Simulation Dump

    1:Solved:300@2,300@1,273@5,:873
    2:Solved:170@2,:170
    .
    .
    .
    11982:Unsolved:300@2,300@1,300@5.....,:300000

I.e: 

=over 4

=item * Board number.

=item * "Solved" or "Unsolved"

=item * iterations@board,iterations@board,iterations@board,

With a trailing comma.

=item * Total number of iterations.

=back 

=cut

sub extract_simu_line
{
    my ($simu_text_ref, $board_id) = @_;

    if (my ($line) = ${$simu_text_ref} =~ m{^(\Q$board_id\E:[^\n]*?)\n}ms)
    {
        return $line;
    }
    else
    {
        Carp::confess "Could not match board ID '$board_id';";
    }
}

{
    my $simu_fn = "300-quota-simulation.txt";

    trap {    
        # TEST
        ok (!system("perl process.pl --quotas-expr='(300)x100' --simulate-to=$simu_fn > 300-quota-output.got"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    my $simu_text = io->file($simu_fn)->slurp();

    {
        my $board1 = q{1:Solved:300@2,300@5,244@9,:844};

        my $line1 = extract_simu_line(\$simu_text, 1);

        # TEST
        is ($line1, $board1, "1 was simulated correctly.");
    }

    {
        my $board24 = q{24:Solved:300@2,300@5,300@9,119@2,:1019};

        my $line24 = extract_simu_line(\$simu_text, 24);

        # TEST
        is ($line24, $board24, "24 was simulated correctly.");
    }
}
