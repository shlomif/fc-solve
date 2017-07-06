use strict;
use warnings;
use autodie;

use FindBin;
use Path::Tiny qw/ path /;

my %strings_to_opts_map;

my $enum_fn  = 'cmd_line_enum.h';
my $gperf_fn = 'cmd_line.gperf';
my $UNREC    = 'FCS_OPT_UNRECOGNIZED';

sub gen_radix_tree
{
    path($gperf_fn)->spew_utf8(
        <<"EOF",
%define initializer-suffix ,$UNREC
%{
#include "$enum_fn"
%}
struct CommandOption
  {
  const char * name;
  int OptionCode;
  };
%%
EOF
        map      { "$_, $strings_to_opts_map{$_}\n" }
            sort { $a cmp $b } keys %strings_to_opts_map,
    );
    return;
}

my $ws   = " " x 4;
my @enum = ($UNREC);

open my $module, "<", "$FindBin::Bin/../cmd_line.c";
SEARCH_FOR_SWITCH: while ( my $line = <$module> )
{
    if ( $line =~ m{\A(\s*)/\* OPT-PARSE-START \*/} )
    {
        while ( $line = <$module> )
        {
            if ( $line =~ m{\A */\* OPT-PARSE-END \*/} )
            {
                last SEARCH_FOR_SWITCH;
            }
            if ( my ( $opt, $strings ) =
                $line =~
                m{\A *case (FCS_OPT_\w+): /\* STRINGS=([^;]+);(?: \*/)? *\n?\z}
                )
            {
                my @s = split( /\|/, $strings );
                %strings_to_opts_map =
                    ( %strings_to_opts_map, ( map { $_ => $opt } @s ), );
                push @enum, $opt;
            }
        }
    }
}
close($module);
gen_radix_tree();

path($enum_fn)->spew_utf8(
    <<"EOF",
/*
 * $enum_fn - the ANSI C enum (= enumeration) for the command line
 * arguments. Partly auto-generated.
 */

#pragma once
EOF
    "enum\n{\n",
    ( map { $ws . $_ . ",\n" } @enum[ 0 .. $#enum - 1 ] ),
    $ws . $enum[-1] . "\n",
    "};\n"
);

my $inc_h      = 'cmd_line_inc.h';
my $tempdir    = Path::Tiny->tempdir;
my $temp_inc_h = $tempdir->child('temp__cmd_line_inc.h');

sub del
{
    if ( -e $inc_h )
    {
        unlink($inc_h);
    }
}

del();
if (
    system(
        'gperf', '-L',      'ANSI-C',
        '-t',    $gperf_fn, "--output-file=$temp_inc_h"
    )
    )
{
    del();
    die "Running gperf failed!";
}
path($inc_h)->spew_utf8(
    do
    {
        my $str = path($temp_inc_h)->slurp_utf8();
        $str =~
            s#(^|\n)(struct CommandOption \*(?:$|\n|\r\n))#${1}static $2#gms;
        $str;
        }
);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
