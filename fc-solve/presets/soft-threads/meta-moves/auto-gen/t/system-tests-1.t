#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

use Test::More tests => 7;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

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

    my $board1 = q{1:Solved:300@2,300@5,244@9,:844};

    my ($line1) = $simu_text =~ m{^(1:[^\n]*?)\n}ms;

    # TEST
    is ($line1, $board1, "1 was simulated correctly.");

    my $board24 = q{24:Solved:300@2,300@5,300@9,119@2,:1019};

    my ($line24) = $simu_text =~ m{^(24:[^\n]*?)\n}ms;

    # TEST
    is ($line24, $board24, "24 was simulated correctly.");
}
