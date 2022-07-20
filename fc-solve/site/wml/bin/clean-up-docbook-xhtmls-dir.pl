#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use lib './lib';

use Getopt::Long qw/ GetOptions /;
use Path::Tiny   qw/ path /;

use Shlomif::RenderDocBookIndiv ();

my @dirs;

GetOptions( "dir=s" => \@dirs, ) or die $!;

foreach my $dir (@dirs)
{
    Shlomif::RenderDocBookIndiv::render_docbook( { htmls_dir => $dir } );
}
