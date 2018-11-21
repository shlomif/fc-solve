#!/usr/bin/env perl

use strict;
use warnings;

use Test::TrailingSpace 0.03;
use Test::More tests => 1;

my $lib    = qr#libfreecell-solver(?:-asm)?\.js(?:\.mem)?#;
my $bigint = qr#(?:big-integer\.js|qunit\.js)#;
my $finder = Test::TrailingSpace->new(
    {
        root              => '.',
        filename_regex    => qr/./,
        abs_path_prune_re => qr#
     (?:\A (?:\./)?
        (?:node_modules)
            |
        (?:lib/out-babel/js/$bigint)
            |
        (?:
            (?:(?:src|dest(?:-prod)?).*?typings.*\.ts\z)
                |
            lib/docbook/5
                |
            lib/fc-solve-for-javascript
                |
            dest(?:-prod)?/(?:
                (?:.*\.(?:epub|rtf|pdf)\z)
                    |
                (?:js/(?:(?:$lib)|$bigint))
                    |
                (?:js-fc-solve/(?:text|automated-tests)/$lib\z)
            )
       )
    )
        |
    (?:lib/for-node/$lib\z)
        |
    (?:\.sass-cache)
        |
    (?:$lib\z)
        |
    (?:\.(?:diff|jpg|patch|png|wasm|woff|xcf|xz|zip)\z)
    #x,
    }
);

# TEST
$finder->no_trailing_space("No trailing whitespace was found.")
