#!/usr/bin/perl

use strict;
use warnings;

use Carp;

$SIG{__WARN__} = sub {
    confess($_[0]);
};

$SIG{__DIE__} = sub {
    confess($_[0]);
};

use AI::Pathfinding::OptimizeMultiple::CmdLine;

my $iface = AI::Pathfinding::OptimizeMultiple::CmdLine->new();
$iface->run();

