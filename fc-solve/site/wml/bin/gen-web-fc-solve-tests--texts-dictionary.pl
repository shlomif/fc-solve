#! /usr/bin/env perl
#
# Short description for gen-web-fc-solve-tests--texts-dictionary.pl
#
# Author Shlomi Fish <shlomif@cpan.org>
use strict;
use warnings;
use Path::Tiny qw/ path /;
use Dir::Manifest ();
use JSON::MaybeXS ();

my $dir   = "lib/web-fcs-tests-strings/";
my $TEXTS = Dir::Manifest->new(
    {
        manifest_fn => "$dir/texts-lists.txt",
        dir         => "$dir/texts"
    }
)->texts_dictionary( { slurp_opts => {} } );

path("src/js/web-fcs-tests-strings.ts")->spew_utf8( "export const dict = ",
    JSON::MaybeXS->new->ascii->canonical(1)->encode($TEXTS), ";" );
