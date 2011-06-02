#!/usr/bin/perl

use strict;
use warnings;

local $ENV{FC_NUM} = 2;
exec($^X, "-I../", "../time-scan.pl", @ARGV);
