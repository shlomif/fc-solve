#! /usr/bin/env perl
#
# Short description for gen-web-fc-solve-tests--texts-dictionary.pl
#
# Author Shlomi Fish <shlomif@cpan.org>
use strict;
use warnings;
use Path::Tiny qw/ path /;
use JSON::MaybeXS ();

my $dir   = "lib/web-fcs-tests-strings/";
my %TEXTS = ( map { $_ => path("$dir/texts/$_")->slurp_utf8 }
        path("$dir/texts-lists.txt")->lines_utf8( { chomp => 1 } ) );

path("src/js/web-fcs-tests-strings.ts")->spew_utf8( "export const dict = ",
    JSON::MaybeXS->new->ascii->canonical(1)->encode( \%TEXTS ), ";" );
