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

        my $strcmp_regex = qr{\(!strcmp *\( *argv\[arg\] *, *"-[a-zA-Z\-]+" *\) *\)};
        if ($line =~ m{\A\Q$ws_prefix\Eelse if \(($strcmp_regex *\|\| *)*$strcmp_regex\) *\n?\z}ms)
        {
            my @strings = ($line =~ m{"([^"]+)"}g);
            my $first = first { m{\A--} } @strings;
            if (!defined($first))
            {
                $first = $strings[0];
            }

            $first =~ s{\A-*}{};
            $first =~ tr{-}{_};
            my $opt = "FCS_OPT_" . uc($first);

            $find_prefix .= "else if (" . join("||", map { qq{(!strcmp(argv[arg], "$_"))} } @strings) . ")\n";
            $find_prefix .= "{\n";
            $find_prefix .= "    opt = $opt;\n";
            $find_prefix .= "}\n";

            $process_opts .= "case ${opt}:\n";

            do
            {
                $line = <$module>;
                $process_opts .= $line
            } while ($line !~ m/\A\Q$ws_prefix\E}/);
            $process_opts .= "break;\n\n";

            $enum .= "    $opt,\n";
        }
    }
    elsif ($in)
    {
        $in = 0;      
        
        $text_out .= "$ws_prefix/* OPT-PARSE-START */\n";
        $text_out .= $find_prefix;
        $text_out .= "switch (opt)\n{\n$process_opts\n}\n";
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

