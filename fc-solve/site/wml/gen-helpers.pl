#!/usr/bin/env perl

use strict;
use warnings;

use HTML::Latemp::GenMakeHelpers ();
use File::Find::Object::Rule     ();

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
require IO::All;

IO::All->import('io');

my $generator = HTML::Latemp::GenMakeHelpers->new(
    'hosts' => [
        {
            'id'         => "common",
            'source_dir' => "common",
            'dest_dir'   => "\$(TARGET)",
        },
        {
            'id'         => "src",
            'source_dir' => "src",
            'dest_dir'   => q/$(D)/,
        },
    ],
);

eval { $generator->process_all(); };

my $E = $@;

if ($E)
{
    print "$E\n";
}

my $text = io("include.mak")->slurp();
$text =~
s!^((?:T2_DOCS|T2_DIRS) = )([^\n]*)!my ($prefix, $files) = ($1,$2); $prefix . ($files =~ s# +ipp\.\S*##gr)!ems;
io("include.mak")->print($text);

io()->file('Makefile')->print("include lib/make/_Main.mak\n");

1;
