#!/usr/bin/perl

use strict;
use warnings;

use File::Find::Object;
use IO::All;

my $new_ver = shift(@ARGV);

my $tree = File::Find::Object->new( {}, 'lib/Games/Solitaire/' );

while ( my $r = $tree->next() )
{
    if ( $r =~ m{/\.svn\z} )
    {
        $tree->prune();
    }
    elsif ( $r =~ m{\.pm\z} )
    {
        my @lines = io->file($r)->getlines();
    LINES_LOOP:
        foreach (@lines)
        {
            if (s#(\$VERSION = ')[0-9]+\.[0-9]+(')#$1 . $new_ver . $2#e)
            {
                last LINES_LOOP;
            }
        }
        io->file($r)->print(@lines);
    }
}
