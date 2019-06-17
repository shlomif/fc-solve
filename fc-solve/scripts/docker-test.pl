#! /usr/bin/env perl
#
# Short description for docker-test.pl
#
# Author Shlomi Fish <shlomif@cpan.org>
# Version 0.0.1
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
use strict;
use warnings;
use 5.014;
use autodie;

use Path::Tiny qw/ path tempdir tempfile cwd /;

sub do_system
{
    my ($args) = @_;

    my $cmd = $args->{cmd};
    print "Running [@$cmd]";
    if ( system(@$cmd) )
    {
        die "Running [@$cmd] failed!";
    }
}

my @deps = map { /^BuildRequires:\s*(\S+)/ ? ("'$1'") : () }
    path("freecell-solver.spec.in")->lines_utf8;
my $SYS = "fedora:31";
do_system( { cmd => [ 'docker', 'pull', $SYS ] } );
my $script = <<"EOSCRIPTTTTTTT";
set -e -x
sudo dnf -y install cmake gcc gcc-c++ git glibc-devel libcmocka-devel make perl-autodie perl-Path-Tiny @deps
git clone https://github.com/shlomif/fc-solve.git
mkdir b
cd b
perl ../fc-solve/fc-solve/scripts/Tatzer
make
FCS_TEST_BUILD=1 perl ../fc-solve/fc-solve/source/run-tests.pl --glob='build-process.t'
EOSCRIPTTTTTTT
do_system( { cmd => [ 'docker', 'run', $SYS, 'bash', '-c', $script, ] } );
