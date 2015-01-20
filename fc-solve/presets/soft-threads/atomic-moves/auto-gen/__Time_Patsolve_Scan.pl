#!/usr/bin/perl

use strict;
use warnings;

my @x_params = (qw(4 1 8 -1 7 11 4 2 2 1 2));

my @y_params = (0.0032, 0.32, -3.0);

exec($^X, "time-scan.pl", 
    "--method", "patsolve",
    (map { ("--patsolve-x-param", "$_,$x_params[$_]") } keys @x_params),
    (map { ("--patsolve-y-param", "$_,$y_params[$_]") } keys @y_params),
);
