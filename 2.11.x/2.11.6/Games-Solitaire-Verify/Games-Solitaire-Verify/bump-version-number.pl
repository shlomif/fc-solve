#!/usr/bin/perl

use strict;
use warnings;

use File::Find::Object;
use IO::All;

my $tree = File::Find::Object->new({}, 'lib/Games/Solitaire/');

while (my $r = $tree->next()) {
    if ($r =~ m{/\.svn\z})
    {
        $tree->prune();
    }
    elsif ($r =~ m{\.pm\z})
    {
        my @lines = io->file($r)->getlines();
        LINES_LOOP:
        foreach (@lines)
        {
            if (s#(\$VERSION = ')\d+\.\d+(')#$1 . "0.03" . $2#e)
            {
                last LINES_LOOP;
            }
        }
        io->file($r)->print(
            @lines
        );
    }
}

