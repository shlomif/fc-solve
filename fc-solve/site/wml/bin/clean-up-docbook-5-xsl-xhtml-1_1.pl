#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use lib './lib';

use XML::LibXML;
use XML::LibXML::XPathContext;
use Getopt::Long;
use Path::Tiny qw/ path /;

use Shlomif::DocBookClean;

my $out_fn;

GetOptions( "output|o=s" => \$out_fn, );

# Input the filename
my $filename = shift(@ARGV)
    or die "Give me a filename as a command argument: myscript FILENAME";

{
    my $s = path($filename)->slurp_utf8;

    $s =~ s{\A.*?<body[^>]*>}{}sm;
    $s =~ s{</body>.*\z}{}ms;

    Shlomif::DocBookClean::cleanup_docbook( \$s );

    # Fixed in Perl 6...
    $s =~ s{<(/?)h(\d)}{"<".$1."h".($2+2)}ge;

    $s =~ s/[ \t]+$//gms;
    path($out_fn)->spew_utf8($s);
}
