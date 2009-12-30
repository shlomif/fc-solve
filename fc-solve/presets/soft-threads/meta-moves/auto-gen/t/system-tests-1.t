#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 11;
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

package FCS::SimuTest;

use IO::All;

sub new
{
    my $class = shift;

    my $filename = shift;

    my $self = {};

    bless $self, $class;

    $self->{text_ref} = \(scalar(io->file($filename)->slurp()));

    return $self;
}

sub text_ref
{
    my $self = shift;

    return $self->{text_ref};
}

sub _extract_line
{
    my ($self, $board_id) = @_;

    if (my ($line) = ${$self->text_ref()} =~ m{^(\Q$board_id\E:[^\n]*?)\n}ms)
    {
        return $line;
    }
    else
    {
        Carp::confess "Could not match board ID '$board_id';";
    }
}

sub is_board_line
{
    my ($self, $board_id, $expected, $blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    Test::More::is ($self->_extract_line($board_id), $expected, $blurb);
}

package main;

{
    my $simu_fn = "300-quota-simulation.txt";

    trap {    
        # TEST
        ok (!system("perl process.pl --quotas-expr='(300)x100' --simulate-to=$simu_fn > 300-quota-output.got"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    my $simu = FCS::SimuTest->new($simu_fn);

    # TEST
    $simu->is_board_line(
        1, 
        q{1:Solved:300@2,300@5,244@9,:844}, 
        "1 was simulated correctly."
    );

    # TEST
    $simu->is_board_line(
        24,
        q{24:Solved:300@2,300@5,300@9,119@2,:1019},
        "24 was simulated correctly."
    );
}

{
    my $simu_fn = "100-200-quota-simulation.txt";

    trap {    
        # TEST
        ok (!system("perl process.pl --quotas-expr='((100,200)x100)' --simulate-to=$simu_fn > 100-200-quota-output.got"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    my $simu = FCS::SimuTest->new($simu_fn);

    # TEST
    $simu->is_board_line(
        1000, 
        q{1000:Solved:125@2,:125}, 
        "1,000 was simulated correctly."
    );

    # TEST
    $simu->is_board_line(
        15_001,
        q{15001:Solved:400@2,300@5,300@9,200@5,100@9,200@16,300@2,100@16,200@9,88@5,:2188},
        "15,001 was simulated correctly."
    );
}
