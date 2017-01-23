#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

my $flto = 0;

my $who = 'sub';

my $mem = 200;
my $num_threads = 4;
my $num_hours = 120;

my $march_flag = "-march=native";

GetOptions(
    'flto!' => \$flto,
    'who=s' => \$who,
    'mem=i' => \$mem,
) or die "No arguments";

if (!defined($who))
{
    die "Unknown who.";
}

my $sub = 1;
my $is_am = 0;

if ($who eq 'am')
{
    $is_am = 1;
    $sub = 0;
}

my $dest_dir_base;
if ($sub)
{
    $flto = 1;
    $num_threads = 24;
    $mem = 500;
    $num_hours = 700;
    $dest_dir_base = 'dbm_fcs_for_sub';
}
elsif ($is_am)
{
    $flto = 0;
    $num_threads = 16;
    $mem = 64;
    $num_hours = 700;
    $march_flag = '';
    $dest_dir_base = 'dbm_fcs_for_amadiro';
}

use FindBin ();
use lib "$FindBin::Bin";
use PrepareCommon;

my @deals =
(qw/
    982
    /);
my $obj = PrepareCommon->new({fcc_solver => 1, depth_dbm => 1, dest_dir_base => $dest_dir_base, flto => $flto, num_threads => $num_threads, mem => $mem, num_hours => $num_hours, march_flag => $march_flag, deals => [@deals],});
$obj->run;
