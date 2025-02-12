#! /usr/bin/env perl
#
# Short description for ci-build-gperf-mswin.pl
#
# Version 0.0.1
# Copyright (C) 2022 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the NOASSERTION license.

use strict;
use warnings;
use 5.014;
use autodie;

use Path::Tiny qw/ path tempdir tempfile cwd /;

use Docker::CLI::Wrapper::Container v0.0.4 ();

if ( -e qq#C:\\fcsdep\\bin\\gperf.exe# ) { exit(0); }

# $ENV{PATH} = qq#C:\\msys64\\mingw64\\bin;$ENV{PATH}#;
my $container = "foo";
my $sys       = "fedora33";
my $obj       = Docker::CLI::Wrapper::Container->new(
    { container => $container, sys => $sys, }, );
$obj->do_system( { cmd => ['sh configure --prefix=C:/fcsdep'] } );
$obj->do_system( { cmd => ['gmake'] } );
$obj->do_system( { cmd => ['gmake install'] } );
