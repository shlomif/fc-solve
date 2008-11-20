#!/usr/bin/perl

use strict;
use warnings;
use List::Util qw(first);

my $text_out;
my $find_prefix;
my $process_opts = "";
my $enum = "enum FCS_COMMAND_LINE_OPTIONS\n{\n";
my $text_in = "";
my $ws_prefix;
my $in = 0;

open my $module, "<", "cmd_line.c";
while (my $line = <$module>)
{
    if (($line =~ m{\A(\s*)/\* OPT-PARSE-START \*/})
           .. 
        ($line =~ m{/\* OPT-PARSE-END \*/})
    )
    {
        $in = 1;
        if (!defined($ws_prefix))
        {
            $ws_prefix = $1;
        }

        my $inner_strcmp_regex = qr{!strcmp *\( *argv\[arg\] *, *"-[a-zA-Z\-]+" *\) *};
        my $strcmp_regex = qr{\($inner_strcmp_regex\)|$inner_strcmp_regex};

        my $complete_strcmp = ($line =~ m{\A\Q$ws_prefix\Eelse if \((?:$strcmp_regex *\|\| *)*$strcmp_regex\) *\n?\z}ms);

        my $is_start_line = ($line =~ m{\A\Q$ws_prefix\Eelse if \( *\n?\z}ms);
        if ($complete_strcmp || $is_start_line)
        {
            if ($is_start_line)
            {
                NEXT_LINE_LOOP:
                while (my $next_line = <$module>)
                {
                    $line .= $next_line;
                    if ($next_line =~ m{\A\Q$ws_prefix\E *\) *\n?\z})
                    {
                        last NEXT_LINE_LOOP;
                    }
                }
            }
            my @strings = ($line =~ m{"([^"]+)"}g);
            my $first = first { m{\A--} } @strings;
            if (!defined($first))
            {
                $first = $strings[0];
            }

            $first =~ s{\A-*}{};
            $first =~ tr{-}{_};
            my $opt = "FCS_OPT_" . uc($first);

            my $indent = "        ";
            $find_prefix .= join("",
                map { $indent. $_ . "\n" }
                (
                    ("else if (" . join("||", map { qq{(!strcmp(argv[arg], "$_"))} } @strings) . ")"), 
                    "{",
                    "    opt = $opt;",
                    "}"
                )
            );

            $process_opts .= "${ws_prefix}case ${opt}:\n";

            do
            {
                $line = <$module>;
                $process_opts .= $line
            } while ($line !~ m/\A\Q$ws_prefix\E\}/);
            $process_opts .= "${ws_prefix}break;\n\n";

            $enum .= "    $opt,\n";
        }
    }
    elsif ($in)
    {
        $in = 0;

        $text_out .= "$ws_prefix/* OPT-PARSE-START */\n";
        $text_out .= $find_prefix;
        $text_out .= "\n\n";
        $text_out .= "${ws_prefix}switch (opt)\n${ws_prefix}{\n$process_opts\n$ws_prefix}\n";
        $text_out .= "$ws_prefix/* OPT-PARSE-END */\n";
    }
    else
    {
        $text_out .= $line;
    }
}
close($module);

open my $out, ">", "cmd_line.c.out";
print {$out} $text_out;
close($out);

open my $enum_fh, ">", "cmd_line_enum.h";
print {$enum_fh} $enum, "\n}\n";
close($enum_fh);

