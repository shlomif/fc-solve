#!/usr/bin/perl

use strict;
use warnings;
use autodie;
use Carp       ();
use FindBin    qw/ $Bin /;
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
        Carp::confess("Running [@$cmd] failed!");
    }
}
do_system( { cmd => [ $^X, "gen-par-mak.pl" ] } );

sub _cleanup
{
    my ($start) = @_;

    if ($start)
    {
        rmtree( ["_Inline"] );
    }
    unlink( grep { -f $_ } glob("Results/*.res") );
    return;
}

_cleanup(1);

do_system( { cmd => [ "gmake", "-f", "par2.mak", "-j1", "Results/1.res", ] } );
do_system( { cmd => [ "gmake", "-f", "par2.mak", "-j1", "Results/2.res", ] } );

use Path::Tiny qw/ path tempdir tempfile cwd /;

my @l = path("Results/1.res")->lines_utf8;
chomp @l;
my $i = 1;
while (@l)
{
    my $l = shift(@l);
    if ( $l ne "== $i ==" )
    {
        Carp::confess($l);
    }
    $l = shift(@l);
    if ( $l !~ /\AVerdict: Solved ; Iters: [0-9]+ ; Length: [0-9]+\z/ )
    {
        Carp::confess($l);
    }
}
continue { ++$i; }
Carp::confess() if $i ne 401;
_cleanup('');
unlink( "build.ninja", "par2.mak" );
