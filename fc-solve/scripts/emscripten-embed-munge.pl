#!/usr/bin/perl

use strict;
use warnings;
use 5.014;

use File::Find::Object ();
my $dir        = shift(@ARGV);
my $PRESET_DIR = "/fc-solve/share/freecell-solver/";
my $tree       = File::Find::Object->new( {}, "$dir$PRESET_DIR" );
my @f;
while ( my $fn = $tree->next() )
{
    if ( -f $fn )
    {
        push @f, $fn;
    }
}

my @output;
foreach my $fn (@f)
{
    push @output, $fn =~ s#\A(\Q$dir\E(/.*))\z#$1\@$2#r;
}
print join( " ", map { "--embed-file $_" } @output ), "\n";
