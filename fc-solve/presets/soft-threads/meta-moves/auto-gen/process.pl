#!/usr/bin/perl

use Shlomif::FCS::CalcMetaScan::CmdLine;

use strict;
use warnings;

my $iface = Shlomif::FCS::CalcMetaScan::CmdLine->new();
$iface->run();

