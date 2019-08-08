#! /usr/bin/env perl
#
# Short description for change-license-to-slashslash.pl
#
use strict;
use warnings;
use 5.014;
use autodie;

s#\A/\*\n((?: \*[^\n]*\n)+?) \*\/\n#$1 =~ s%^ \*%//%gmrs#ems;
