#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;
use IO::All;
use POSIX;

my @results;

foreach my $file (qw(index old-news))
{
    my @lines = io("src/".$file.".html.wml")->chomp()->getlines();

    LINES_LOOP:
    while (defined(my $l = shift(@lines)))
    {
        if (my ($y, $m, $d, $formatted_date) =
            ($l =~ m[\A<h3 id="news-(\d{4})-(\d{1,2})-(\d{1,2})>([^<]+)</h3> *\z]
            ))
        {
            my $time = mktime(0, 30, 12, $d, $m-1, $y-1900);
            my $exp_format = strftime("%d-%b-%Y", localtime($time));
            foreach my $f ($exp_format, $formatted_date)
            {
                $f =~ s{\A0}{};
            }
            if ($exp_format ne $formatted_date)
            {
                die "Expected format '$exp_format' differs in line '$l'!";
            }

            my $html = "";
            my $html_l;

            GET_HTML:
            while (defined($html_l = shift(@lines)))
            {
                if ($html_l =~ m{\A<h3>})
                {
                    last GET_HTML;
                }
                $html .= $html_l;
            }
            $l = $html_l;
            push @results, { year => $y, mon => $m, day_of_month => $d, html => $html };
            redo LINES_LOOP;
        }
    }
}

print Data::Dumper->new([\@results])->Dump();
