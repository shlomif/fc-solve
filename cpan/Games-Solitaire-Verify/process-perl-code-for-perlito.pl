#!/usr/bin/perl

use strict;
use warnings;

my $text = do
{
    local $/;
    <ARGV>;
};

$text =~ s#^use parent '([^']+)';#use $1 (); use vars qw(\@ISA); \@ISA = (qw($1));#gms;

$text =~ s#^use Games::Solitaire::Verify::Exception;##gms;
$text =~ s#([\w:]+)->throw\s*\((.*?)\)\s*;#die +(bless { $2 }, '$1')#gms;
print $text;
