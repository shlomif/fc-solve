#!/usr/bin/perl

use strict;
use warnings;
use autodie;
use FindBin qw/ $Bin /;
use File::Path qw/ rmtree /;
use Env::Path;

Env::Path->LD_LIBRARY_PATH->Prepend( $ENV{FCS_PATH} );

chdir($Bin);

sub do_system
{
    my ($args) = @_;

    my $cmd = $args->{cmd};
    print "Running [@$cmd]\n";
    if ( system(@$cmd) )
    {
        die "Running [@$cmd] failed!";
    }
}
do_system( { cmd => [ $^X, "gen-par-mak.pl" ] } );
rmtree( ["_Inline"] );
unlink( grep { -f $_ } glob("Results/*.res") );
do_system( { cmd => [ "gmake", "-f", "par2.mak", "-j1", "Results/1.res", ] } );
do_system( { cmd => [ "gmake", "-f", "par2.mak", "-j1", "Results/2.res", ] } );

