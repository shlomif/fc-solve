#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use File::Find::Object;
use File::Temp qw( tempdir );
use List::Util qw( first );
use File::Spec;
use File::stat;
use Cwd qw ( getcwd );

my $version;
my $package_base;
my $source_dir;

GetOptions(
    'version=s' => \$version,
    'package-base=s' => \$package_base,
    'source-dir=s' => \$source_dir,
) or die "GetOptions() failed.";

if (!defined($version))
{
    die "--version not specified.";
}

if (!defined($package_base))
{
    die "--package-base not specified.";
}

if (!defined($source_dir))
{
    die "--source-dir not specified.";
}

sub get_dir_entries
{
    my $dirname = shift;

    opendir my $dh, $dirname
        or die "Cannot opendir '$dirname'! - $!";

    my @ret = File::Spec->no_upwards(readdir($dh));

    closedir($dh);

    return \@ret;
}

my @tarballs = grep { /\A\Q$package_base\E-\Q$version\E\.tar\.\w+\z/ }
    @{get_dir_entries($source_dir)};

if (! @tarballs)
{
    die "Cannot find source tarballs in '$source_dir'!";
}

my $preferred_tarball = ((first { /\.gz\z/ } @tarballs) || $tarballs[0]);

my $tempdir = tempdir ( DIR => $source_dir, CLEANUP => 1);

my $orig_dir = getcwd();

my $source_dir_abs = File::Spec->rel2abs($source_dir);

chdir($tempdir);

print "Unpacking the source tarball.\n";
system("tar", "-xf", File::Spec->catfile($source_dir_abs, $preferred_tarball));

print "Applying the original timestamps to the files.\n";

my $ffo = File::Find::Object->new({}, '.');

FILES:
while (my $r = $ffo->next_obj())
{
    if (not $r->is_file())
    {
        next FILES;
    }

    my $components = $r->full_components();
    shift(@$components);

    my $source_fn = File::Spec->catfile($source_dir_abs, @$components);
    if (my $st = stat($source_fn) )
    {
        utime ($st->atime(), $st->mtime(), $r->path());
    }
}

my $arc_dir = first { -d $_ } @{get_dir_entries(".")};

if (!defined($arc_dir))
{
    die "No archive directory (in which all files are in the extracted folder.";
}

print "Repackaging.\n";
foreach my $tarball (@tarballs)
{
    system("tar", "-caf", File::Spec->catfile($source_dir_abs, $tarball),
        $arc_dir);
}
print "Finished!\n";

chdir ($orig_dir);
