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

_exec(
    [
        'cookiecutter', '-f', '--no-input',
        'gh:shlomif/cookiecutter--shlomif-latemp-sites',
        'project_slug=.',
    ],
    'cookiecutter failed.'
);
