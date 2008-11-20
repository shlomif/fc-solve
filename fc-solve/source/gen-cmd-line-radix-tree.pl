#!/usr/bin/perl

use strict;
use warnings;
use List::Util qw(first);
use Data::Dumper;

my $text_out;
my $find_prefix;
my $process_opts = "";
my $enum = "enum FCS_COMMAND_LINE_OPTIONS\n{\n";
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
            if ((length($remaining) == 0) || (!%$reached))
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
                if (length($remaining) == 1)
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

    print Dumper($start);
}

$enum .= "FCS_OPT_UNRECOGNIZED,\n";

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

open my $out, ">", $module_filename.".new";
print {$out} $text_out;
close($out);

open my $enum_fh, ">", "cmd_line_enum.h";
print {$enum_fh} $enum, "\n};\n";
close($enum_fh);

