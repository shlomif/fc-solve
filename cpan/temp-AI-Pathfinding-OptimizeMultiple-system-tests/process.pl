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

use AI::Pathfinding::OptimizeMultiple::App::CmdLine;

my $iface = AI::Pathfinding::OptimizeMultiple::App::CmdLine->new({ argv => [@ARGV], }, );
$iface->run();

