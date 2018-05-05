#!/usr/bin/perl

use strict;
use warnings;

my @x_params = (qw(2 4 7 -2 -2 1 0 -5 4 -1 1 ));

my @y_params = ( -0.0024, 0.22, 0.0 );

exec( $^X,
    "time-scan.pl",
    "--method",
    "patsolve",
    ( map { ( "--patsolve-x-param", "$_,$x_params[$_]" ) } keys @x_params ),
    ( map { ( "--patsolve-y-param", "$_,$y_params[$_]" ) } keys @y_params ),
);
