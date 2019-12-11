#! /usr/bin/env perl
#
# Short description for expect.pl
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
use Expect ();
my $exp = Expect->new;

# $exp->raw_pty(1);
$exp->debug(3);
$exp->exp_internal(1);
$exp->log_stdout(1);
$exp->max_accum(880_000);

$ENV{NODE_PATH} = cwd->child(qw# lib for-node js#);
$exp->spawn( "node", "inspect", "./qunit.js",
    "lib/for-node/test-code-emcc.js" );
$exp->expect( 10, 'debug>' );
$exp->send("breakOnUncaught\n");
$exp->expect( 30, 'debug>' );
$exp->send("c\n");
$exp->expect( 10, 'debug>' );
exit;
$exp->send("q\n");
$exp->expect( 10, 'debug>' );
