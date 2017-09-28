#!/usr/bin/perl

use strict;
use warnings;

use Test::TrailingSpace 0.03;
use Test::More tests => 1;

my $lib = qr#libfreecell-solver\.js(?:\.mem)?#;
my $finder = Test::TrailingSpace->new(
    {
        root => '.',
        filename_regex => qr/./,
        abs_path_prune_re => qr#(?:\A(?:\./)?(?:(?:(?:src|dest(?:-prod)?).*?typings.*\.ts\z)|lib/fc-solve-for-javascript|dest(?:-prod)?/(?:(?:js/$lib)|(?:js-fc-solve/(?:text|automated-tests)/$lib\z))))|(?:lib/for-node/$lib\z)|(?:$lib\z)|(?:\.(?:diff|jpg|patch|png|woff|xcf|xz|zip)\z)#,
    }
);

# TEST
$finder->no_trailing_space("No trailing whitespace was found.")
