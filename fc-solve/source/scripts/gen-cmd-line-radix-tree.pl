
use strict;
use warnings;
use autodie;

use List::Util qw(first);
use Data::Dumper;
use Path::Tiny qw/path/;

my $find_prefix;
my $process_opts = "";
my $text_in = "";
my $ws_prefix;
my $in = 0;

my %strings_to_opts_map;

my $gperf_fn = 'cmd_line.gperf';
sub gen_radix_tree
{
    open my $fh, '>', $gperf_fn;
    print {$fh} <<"EOF";
%define initializer-suffix ,FCS_OPT_UNRECOGNIZED
%{
#include "cmd_line_enum.h"
%}
struct CommandOption
  {
  const char * name;
  int OptionCode;
  };
EOF
    print {$fh} "%%\n", map { "$_, $strings_to_opts_map{$_}\n" } sort { $a cmp $b } keys %strings_to_opts_map;

    close($fh);

    return;
}

my $ws = " " x 4;
my @enum = ("FCS_OPT_UNRECOGNIZED");

my $module_filename = "cmd_line.c";
open my $module, "<", $module_filename;
while (my $line = <$module>)
{
    if ($line =~ m{\A(\s*)/\* OPT-PARSE-START \*/})
    {
        # Skip the lines.
        UP_TO_SWITCH:
        while ($line = <$module>)
        {
            if ($line =~ m{\A *switch \(opt\) *\n?\z}ms)
            {
                $process_opts .= $line;
                last UP_TO_SWITCH;
            }
        }

        IN_SWITCH:
        while ($line = <$module>)
        {
            $process_opts .= $line;
            if ($line =~ m{\A */\* OPT-PARSE-END \*/})
            {
                last IN_SWITCH;
            }
            if (my ($opt, $strings) = $line =~ m{\A *case (FCS_OPT_\w+): /\* STRINGS=([^;]+); \*/ *\n?\z})
            {
                my @s = split(/\|/, $strings);
                %strings_to_opts_map =
                (
                    %strings_to_opts_map,
                    (map { $_ => $opt } @s),
                );
                push @enum, $opt;
            }
        }

        gen_radix_tree();
    }
}
close($module);

open my $enum_fh, ">", "cmd_line_enum.h";
print {$enum_fh} <<'EOF';
/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * cmd_line_enum.h - the ANSI C enum (= enumeration) for the command line
 * arguments. Partially auto-generated.
 */

#pragma once
EOF
print {$enum_fh} "enum\n{\n",
    (map { $ws . $_ . ",\n" } @enum[0..$#enum-1]),
    $ws . $enum[-1] . "\n",
    "};\n";

close($enum_fh);

my $inc_h = 'cmd_line_inc.h';

sub del
{
    if (-e $inc_h)
    {
        unlink($inc_h);
    }
}

del();
if (system('gperf', '-L', 'ANSI-C', '-t', $gperf_fn, "--output-file=$inc_h"))
{
    del();
    die "Running gperf failed!";
}
path($inc_h)->edit_utf8(sub { s#^(struct CommandOption \*)$#static $1#gms;});

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.



=cut

