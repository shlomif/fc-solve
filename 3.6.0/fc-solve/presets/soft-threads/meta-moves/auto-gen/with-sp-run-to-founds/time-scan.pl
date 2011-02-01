#!/usr/bin/perl

use strict;
use warnings;

exec($^X, "-I../", "../time-scan.pl", @ARGV);
