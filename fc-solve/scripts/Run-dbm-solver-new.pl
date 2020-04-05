#!/usr/bin/perl

use strict;
use warnings;

use Path::Tiny qw/ path /;

my $filename = shift(@ARGV);

my $dump_file = "$filename-dump";

if ( -e $dump_file )
{
    die "Dump file $dump_file exists.";
}

my $queue_dir = path("$ENV{HOME}/tmp/queue-offload/");
$queue_dir->mkpath;

exec( "./dbm_fc_solver", "--offload-dir-path", $queue_dir, "-o", $dump_file,
    "--num-threads", 1, $filename );
