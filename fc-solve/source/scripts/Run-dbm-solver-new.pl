#!/usr/bin/perl

use strict;
use warnings;

use Cwd;
use File::Path qw(mkpath);

my $filename = shift(@ARGV);

my $dump_file = "$filename-dump";

if (-e $dump_file)
{
    die "Dump file $dump_file exists.";
}

my $queue_dir = "$ENV{HOME}/tmp/queue-offload/";
mkpath($queue_dir);

exec(
    "./dbm_fc_solver",
    "--offload-dir-path", $queue_dir,
    "-o", $dump_file,
    "--num-threads", 1,
    $filename
);
