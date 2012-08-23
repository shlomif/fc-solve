#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

my ($version) =
    (map { m{\$VERSION *= *'([^']+)'} ? ($1) : () }
    io->file('lib/Games/Solitaire/Verify.pm')->getlines()
    )
    ;

if (!defined ($version))
{
    die "Version is undefined!";
}

my $mini_repos_base = 'https://fc-solve.googlecode.com/svn/fc-solve';

my $mod_name = "Games-Solitaire-Verify";
my @cmd = (
    "svn", "copy", "-m",
    "Tagging the $mod_name release as $version",
    "$mini_repos_base/trunk",
    "$mini_repos_base/tags/$mod_name/$mod_name/cpan-releases/$version",
);

print join(" ", map { /\s/ ? qq{"$_"} : $_ } @cmd), "\n";
exec(@cmd);

