#!/usr/bin/env perl

use strict;
use warnings;
use 5.014;

use lib './lib';

use Getopt::Long                   qw/ GetOptions /;
use Path::Tiny                     qw/ path /;
use FreecellSolver::Site::TTRender ();

my $printable;
my $stdout;
my @filenames;

GetOptions(
    'fn=s'       => \@filenames,
    'printable!' => \$printable,
    'stdout!'    => \$stdout,
) or die $!;

my $obj = FreecellSolver::Site::TTRender->new(
    { printable => $printable, stdout => $stdout, } );

if ( !@filenames )
{
    @filenames = ("js-fc-solve/text/gui-tests.xhtml");
}
$obj->proc( shift @filenames );
