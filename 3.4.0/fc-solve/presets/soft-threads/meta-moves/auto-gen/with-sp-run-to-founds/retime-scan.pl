#!/usr/bin/perl

use strict;
use warnings;

exec($^X, "-I../", "../retime-scan.pl", @ARGV);
