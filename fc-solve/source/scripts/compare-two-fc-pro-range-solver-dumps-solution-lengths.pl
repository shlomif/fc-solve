#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use Getopt::Long;
use File::Basename qw(basename);

my $cache_dir = "./cache";

my $from_path;
my $to_path;
my $iters_threshold;
my $should_histogram;

GetOptions(
    'f|from=s' => \$from_path,
    't|to=s' => \$to_path,
    'iters=i' => \$iters_threshold,
    'histogram!' => \$should_histogram,
) or die "Cannot read get opts.";

if (!defined $from_path)
{
    die "--from not specified.";
}

if (!defined $to_path)
{
    die "--to not specified.";
}

if (!defined $iters_threshold)
{
    die "--iters not specified.";
}

if (! -d $cache_dir)
{
    io->dir($cache_dir)->mkpath();
}

sub l
{
    my $fn = shift;

    my $cache_fn = "$cache_dir/" . basename($fn) . ".rel-info";
    if (! -f $cache_fn)
    {
        io->file($cache_fn)->print(
            grep { /\A\[\[Num/ } io->file($fn)->getlines()
        );
    }

    return [io->file($cache_fn)->chomp->getlines];
}

sub get_fcs
{
    return [map { m{\A\[\[Num FCS Moves\]\]=(-?\d+)\z} ? ($1) : () }
        @{shift()}];
}

sub get_fcpro
{
    return [map { m{\A\[\[Num FCPro Moves\]\]=(-?\d+)\z} ? ($1) : () }
        @{shift()}]
}

sub get_iters
{
    return [map { m{\A\[\[Num Iters\]\]=(-?\d+)\z} ? ($1) : () }
        @{shift()}]
}

# my $mfi = l("./micro-finance-improved.fc-pro-dump.txt");
# my $mfi = l("./micro-finance-improved--flares-choice-fcpro.fc-pro-dump.txt");
my $mfi = l($from_path);
my $mfi_fcs = get_fcs($mfi);
my $mfi_fcpro = get_fcpro($mfi);

# my $new = l("./new_scan.dump.txt");
# my $new = l("./micro-finance-improved--flares-choice-fcpro.fc-pro-dump.txt");
my $new = l($to_path);
my $new_fcs = get_fcs($new);
my $new_fcpro = get_fcpro($new);
my $new_iters = get_iters($new);

foreach my $aref ($mfi_fcs, $mfi_fcpro, $new_fcs, $new_fcpro, $new_iters)
{
    if (@$aref != 32_000)
    {
        die "$aref is wrong!";
    }
}

my %histogram;

for my $i (keys @$new_iters)
{
    if ($new_iters->[$i] >= 0 and $new_iters->[$i] < $iters_threshold)
    {
        my $bef = $mfi_fcpro->[$i];
        my $aft = $new_fcpro->[$i];
        if ($aft < $bef)
        {
            my $delta = $bef - $aft;
            if ($should_histogram)
            {
                $histogram{$delta}++;
            }
            else
            {
                printf("I=%d %d -> %d Delta=%d%s\n", $i+1, $bef, $aft, $delta,
                    ($aft > $bef ? " (Worse)" : ""),
                );
            }
        }
    }
}

if ($should_histogram)
{
    foreach my $delta (sort { $a <=> $b } keys %histogram)
    {
        print "$histogram{$delta}\t$delta\n";
    }
}
