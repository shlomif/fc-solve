#!/usr/bin/perl

use strict;
use warnings;

use Path::Tiny qw/ path /;

my ($version) =
    ( map { m{\Aversion * = *(\S+)} ? ($1) : () }
        path("./dist.ini")->lines_utf8() );

if ( !defined($version) )
{
    die "Version is undefined!";
}

my $PROJECT = "Task-FreecellSolver-Testing-MultiConfig";
my @cmd     = (
    "git", "tag", "-m", "Tagging the $PROJECT release as $version",
    "$PROJECT-$version",
);

print join( " ", map { /\s/ ? qq{"$_"} : $_ } @cmd ), "\n";
exec(@cmd);
