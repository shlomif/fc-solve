#!/usr/bin/perl

use strict;
use warnings;

use Cwd;

my $filename = shift(@ARGV);

exec(
    "./dbm_fc_solver",
    "-o", "$filename-dump",
    "--dbm-store-path", (Cwd::getcwd() . "/" . "dbm_store.leveldb"),
    # "--dbm-store-path", (Cwd::getcwd() . "/" . "dbm_store.bdb"),
    "--pre-cache-max-count", 1_000_000,
    "--caches-delta", 1_000_000,
    "--num-threads", 2,
    $filename
);
