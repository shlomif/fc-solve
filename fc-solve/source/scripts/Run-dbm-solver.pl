#!/usr/bin/perl

use strict;
use warnings;

use Cwd;

my $filename = shift(@ARGV);

my $dbm_store = (Cwd::getcwd() . "/" . "dbm_store.leveldb");
my $dump_file = "$filename-dump";

if (-e $dbm_store)
{
    die "DBM store '$dbm_store' exists.";
}

if (-e $dump_file)
{
    die "Dump file $dump_file exists.";
}

exec(
    "./dbm_fc_solver",
    "-o", $dump_file,
    "--dbm-store-path", $dbm_store,
    # "--dbm-store-path", (Cwd::getcwd() . "/" . "dbm_store.bdb"),
    "--pre-cache-max-count", 1_000_000,
    "--caches-delta", 1_000_000,
    "--num-threads", 2,
    $filename
);
