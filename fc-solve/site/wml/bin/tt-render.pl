#!/usr/bin/env perl

use strict;
use warnings;
use 5.014;

use lib './lib';

use FreecellSolver::Site::TTRender ();

my $printable;
my $stdout;
my @filenames;

my $obj = FreecellSolver::Site::TTRender->new( { stdout => $stdout, } );

if ( !@filenames )
{
    @filenames = ("js-fc-solve/text/gui-tests.xhtml");
}
$obj->proc( shift @filenames );
