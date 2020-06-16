#!/usr/bin/env perl

use strict;
use warnings;

sub _exec
{
    my ( $cmd, $err ) = @_;

    if ( system(@$cmd) )
    {
        die $err;
    }
    return;
}

my $src_dn =
"$ENV{HOME}/.cookiecutters/cookiecutter--shlomif-latemp-sites/\{\{cookiecutter.project_slug\}\}";
if ( -d $src_dn )
{
    _exec( [ "rsync", "-ra", "$src_dn/", "./" ], "rsync failed", );
    require Path::Tiny;
    Path::Tiny::path("./bin/batch-inplace-html-minifier")
        ->edit_raw( sub { s/\n\{% (?:end)?raw %\}\n/\n/g; } );
}
else
{
    _exec(
        [
            'cookiecutter', '-f', '--no-input',
            'gh:shlomif/cookiecutter--shlomif-latemp-sites',
            'project_slug=.',
        ],
        'cookiecutter failed.'
    );
}
