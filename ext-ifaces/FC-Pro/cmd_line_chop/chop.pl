#!/usr/bin/perl -w

use strict;

my $string = join("", <>);

my @args;

my $last_arg = "";

my @chars = split(//, $string);

my $next_char;

push @chars, "\0";
my $i;

for($i=0;$i<scalar(@chars);)
{
    while ($chars[$i] =~ /^[ \t\n]$/) 
    {
        $i++;
    }
    if ($chars[$i] eq "\0")
    {
        last;
    }
    AFTER_WS:
    while ($chars[$i] =~ /^[^ \t\n"\\\0]$/)
    {
        $last_arg .= $chars[$i];
        $i++;
    }
    
    if ($chars[$i] =~ /^[ \t\n\0]$/)
    {
        NEXT_ARG:
        push @args, $last_arg;
        $last_arg = "";
        if ($chars[$i] eq "\0")
        {
            last;
        }
    }
    elsif ($chars[$i] eq "\\")
    {
        my $next_char = $chars[++$i];
        $i++;
        if ($next_char eq "\0")
        {
            $i--;
            goto NEXT_ARG;
        }
        elsif (($next_char eq "\n") || ($next_char eq "\r"))
        {
            goto NEXT_ARG;
        }
        else
        {
            $last_arg .= $next_char;
        }
    }
    elsif ($chars[$i] eq "\"")
    {
        $i++;
        while(($chars[$i] ne "\"") && ($chars[$i] ne "\0"))
        {
            if ($chars[$i] eq "\\")
            {
                $next_char = $chars[++$i];
                if ($next_char eq "\0")
                {
                    push @args, $last_arg;
                    goto END_OF_LOOP;
                }
                elsif (($next_char eq "\n") || ($next_char eq "\r"))
                {
                    # Do nothing
                }
                elsif (($next_char eq "\\") || ($next_char eq "\""))
                {
                    $last_arg .= $next_char;
                }
                else
                {
                    $last_arg .= "\\";
                    $last_arg .= $next_char;
                }
            }
            else
            {
                $last_arg .= $chars[$i];
            }
            $i++;
        }
        $i++;
        goto AFTER_WS;
    }
}

END_OF_LOOP:
for($i=0;$i<scalar(@args);$i++)
{
    print "$i: $args[$i]\n";
}
