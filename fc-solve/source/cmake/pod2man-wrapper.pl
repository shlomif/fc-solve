#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use File::Temp qw/tempdir/;
use File::Copy; 

my ($src, $dest, $sect, $center, $release);

GetOptions(
    'src=s' => \$src,
    'dest=s' => \$dest,
    'section=s' => \$sect,
    'center=s' => \$center,
    'release=s' => \$release,
)
    or die "Wrong options.";

if (!defined($src))
{
    die "src not specified.";
}

if (!defined($dest))
{
    die "dest not specified.";
}

if (!defined($sect))
{
    die "section not specified.";
}

if (!defined($center))
{
    die "center not specified.";
}

if (!defined($release))
{
    die "release not specified.";
}

my $dir = tempdir( CLEANUP => 1);

my $pod = "$dir/Hoola.pod";

if (! -e $src)
{
    die "Cannot find '$src'";
}
copy($src, $pod);

if(
system(
    'pod2man',
    "--section=$sect",
    "--center=$center",
    "--release=$release",
    $pod,
    $dest
)
)
{
    die "pod2man failed! $!";
}
