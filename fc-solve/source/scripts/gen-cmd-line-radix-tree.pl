#!/usr/bin/perl

use strict;
use warnings;
use List::Util qw(first);
use Data::Dumper;

my $text_out;
my $find_prefix;
my $process_opts = "";
my $text_in = "";
my $ws_prefix;
my $in = 0;

my %strings_to_opts_map;

sub gen_radix_tree
{
    my $start = {};

    while (my ($string, $value) = each(%strings_to_opts_map))
    {
        my $remaining = $string;
        my $reached = $start;

        PUT_STRING:
        while (defined($value))
        {
            if (!%$reached)
            {
                $reached->{$remaining} = $value;
                undef($value);
            }
            else
            {
                my @keys = keys(%$reached);
                for my $pos (reverse(1 .. length($remaining)))
                {
                    if (my $k =
                        first
                        {
                            substr($_, 0, $pos) eq
                            substr($remaining, 0, $pos)
                        } @keys
                    )
                    {
                        if ($pos == length($k))
                        {
                            $remaining = substr($remaining,$pos);

                            if (ref($reached->{$k}) ne "HASH")
                            {
                                $reached->{$k} =
                                {
                                    "" => $reached->{$k},
                                }
                            }

                            $reached = $reached->{$k};

                            next PUT_STRING;
                        }
                        else
                        {
                            # Split the node at the position.
                            $reached = $reached->{substr($k, 0, $pos)} =
                            {
                                substr($k, $pos) => delete($reached->{$k}),
                            };

                            $remaining = substr($remaining, $pos);

                            next PUT_STRING;
                        }
                    }
                }

                # Split at the first character
                foreach my $k (keys(%$reached))
                {
                    if (($k eq "") || (length($k) == 1))
                    {
                        next;
                    }
                    my $v = delete($reached->{$k});
                    $reached->{substr($k,0,1)} =
                    {
                        substr($k,1) => $v,
                    }
                }
                if (length($remaining) <= 1)
                {
                    $reached->{$remaining} = $value;
                    $remaining = "";
                    undef($value);
                }
                else
                {
                    $reached = $reached->{substr($remaining,0,1)} = {};
                    $remaining = substr($remaining,1);
                }
            }
        }
    }

    # print Dumper($start);

    # Now let's render $start into C-code.
    my $code = "";

    $code .= <<"EOF";
p = (*arg);
opt = FCS_OPT_UNRECOGNIZED;
EOF

    my $render;

    $render = sub {
        my $node = shift;

        my $ret = "";

        if (ref($node) ne "HASH")
        {
            return "\n{\nif (*p == '\\0')\n{\n\nopt = $node;\n}\n}\n";
        }

        my @k = (sort { $a cmp $b } keys(%$node));
        if (@k == 1)
        {
            my $key = $k[0];
            my $has_kids = (ref($node->{$key}) eq "HASH");
            if ((length($key) == 1) && $has_kids)
            {
                return "\n{\nif (*(p++) == '$key')\n{\n" . $render->($node->{$key}) . "\n}\n\n}\n";
            }
            else
            {
                my $func = $has_kids ? 'strncmp' : 'strcmp';
                my $func_len = $has_kids ? ', '.length($key) : '';
                return "{\nif (!$func(p, \"$key\"$func_len)) {\n"
                    . ($has_kids
                        ? ("p += " . length($key) . ";\n" . $render->($node->{$key}))
                        : "opt = $node->{$key};\n"
                    )
                    . "\n}\n}\n"
                    ;
            }
        }
        else
        {
            return "{ switch(*(p++)) {"
                . join("", (map { "\ncase '" . (length($_) ? $_ : q{\\0}) . "':\n"
                    . (length($_)
                        ? $render->($node->{$_})
                        : "{\nopt = $node->{$_};\n}\n"
                    )
                    . "\nbreak;\n"
                } @k))
                . "\n}\n}\n";
        }
    };

    return $code . $render->($start);
}

my $ws = " " x 4;
my @enum = "FCS_OPT_UNRECOGNIZED";

my $module_filename = "cmd_line.c";
open my $module, "<", $module_filename;
while (my $line = <$module>)
{
    if ($line =~ m{\A(\s*)/\* OPT-PARSE-START \*/})
    {
        $text_out .= $line;

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

        $text_out .= gen_radix_tree();
        $text_out .= $process_opts;
    }
    else
    {
        $text_out .= $line;
    }
}
close($module);

open my $out, ">", $module_filename;
print {$out} $text_out;
close($out);

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
EOF
print {$enum_fh} "enum\n{\n",
    (map { $ws . $_ . ",\n" } @enum[0..$#enum-1]),
    $ws . $enum[-1] . "\n",
    "};\n";
close($enum_fh);

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

