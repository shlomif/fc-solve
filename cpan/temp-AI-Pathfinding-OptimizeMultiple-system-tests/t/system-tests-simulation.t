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

use base 'Shlomif::FCS::CalcMetaScan::Base';

use IO::All;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

__PACKAGE__->mk_acc_ref([qw(text_ref fn_base)]);

sub _init
{
    my $self = shift;

    my ($fn_base, $cmd_line) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    $self->fn_base($fn_base);

    trap {
        Test::More::ok (!system(
                join(
                    " ", $^X, "process.pl", @$cmd_line,
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
    my $simu = FCS::SimuTest->new("300-quota", ["--quotas-expr='((300)x100)'"]);

    # TEST
    $simu->is_board_line(
        1,
        q{1:Solved:300@2,245@9,:545},
        "1 was simulated correctly."
    );

    # TEST
    $simu->is_board_line(
        24,
        q{24:Solved:300@2,300@9,300@5,120@2,:1020},
        "24 was simulated correctly."
    );
}

{
    # TEST*2
    my $simu = FCS::SimuTest->new("100-200-quota", ["--quotas-expr='((100,200)x100)'"]);

    # TEST
    $simu->is_board_line(
        1000,
        q{1000:Solved:126@2,:126},
        "1,000 was simulated correctly."
    );

    # TEST
    $simu->is_board_line(
        15_001,
        q{15001:Solved:400@2,300@5,500@9,100@5,200@16,100@2,189@5,:1789},
        "15,001 was simulated correctly."
    );
}
