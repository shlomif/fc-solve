#!/usr/bin/perl

use strict;
use warnings;

local $ENV{FC_VARIANT} = "bakers_dozen";
exec($^X, "-I../", "../time-scan.pl", @ARGV);
