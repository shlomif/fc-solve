#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 8;

use Carp;

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

use MooX qw/late/;

use IO::All;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

has cmd_line => (is => 'ro', isa => 'ArrayRef[Str]', required => 1);
has text_ref => (is => 'rw', isa => 'ScalarRef[Str]',);
has fn_base => (is => 'ro', isa => 'Str', required => 1,);

sub run
{
    my $self = shift;

    trap {
        Test::More::ok (!system(
                join( " ",
                    $^X, "process.pl", @{$self->cmd_line()},
                    "--simulate-to=".$self->_simulate_to(),
                    ">",
                    $self->_output_got()
                )
            ),
            "Running to " . $self->_simulate_to(),
        );
    };

    my $stderr = $trap->stderr();

    Test::More::is ($stderr, "",
        "process.pl of " . $self->_simulate_to()
        . " did not return any errors on stderr");

    $self->text_ref(\(scalar(io->file($self->_simulate_to)->slurp())));

    return $self;
}

sub _simulate_to
{
    return shift->fn_base() . "-simulation.txt";
}

sub _output_got
{
    return shift->fn_base() . "-output.got";
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

sub DESTROY
{
    my $self = shift;

    unlink($self->_simulate_to(), $self->_output_got());

    return;
}

package main;

{
    # TEST*2
    my $simu = FCS::SimuTest->new(
        {
            fn_base => "300-quota",
            cmd_line => ["--quotas-expr='((300)x100)'",],
        }
    );

    $simu->run();

    # TEST
    $simu->is_board_line(
        1,
        q{1:Solved:300@24,300@5,245@9,:845},
        "1 was simulated correctly."
    );

    # TEST
    $simu->is_board_line(
        24,
        q{24:Solved:118@24,:118},
        "24 was simulated correctly."
    );
}

{
    # TEST*2
    my $simu = FCS::SimuTest->new(
        {
            fn_base =>"100-200-quota",
            cmd_line => ["--quotas-expr='((100,200)x100)'"]
        },
    );

    $simu->run();

    # TEST
    $simu->is_board_line(
        1000,
        q{1000:Solved:100@22,300@24,120@5,:520},
        "1,000 was simulated correctly."
    );

    # TEST
    $simu->is_board_line(
        15_001,
        q{15001:Solved:100@22,300@24,200@5,100@22,200@5,100@24,300@9,200@24,100@9,200@16,100@24,189@5,:2089},
        "15,001 was simulated correctly."
    );
}
