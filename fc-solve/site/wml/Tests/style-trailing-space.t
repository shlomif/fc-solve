#!/usr/bin/env perl

use strict;
use warnings;

use Test::TrailingSpace 0.03 ();
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
        (?:lib/jquery/qunit/)
            |
        (?:lib/data/mbox/)
            |
        (?: lib/repos/Solitairey )
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
            lib/transcrypt_module/src
                |
            dest(?:-prod)?/(?:
                (?: js/yui-unpack/? .*? )
                    |
                (?:.*\.(?:epub|rtf|pdf)\z)
                    |
                (?:js/(?:(?:$lib)|$bigint))
                    |
                (?:js-fc-solve/(?:text|automated-tests)/$lib\z)
                    |
                (?:mail-lists/)
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
    (?:\.(?:diff|gif|jar|jpg|patch|png|ttf|wasm|webp|woff|xcf|xz|zip)\z)
        |
    (?:/\.[^/]+\.swp\z)
    #x,
    }
);

# TEST
$finder->no_trailing_space("No trailing whitespace was found.")
