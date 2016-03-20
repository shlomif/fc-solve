#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;
use IO::All;
use POSIX;

my @results;
my $last_time;

foreach my $file (qw(index old-news))
{
    my @lines = io("src/".$file.".html.wml")->chomp()->getlines();

    LINES_LOOP:
    while (defined(my $l = shift(@lines)))
    {
        if (my ($y, $m, $d, $formatted_date) =
            ($l =~ m[\A<h3 id="news-(\d{4})-(\d{1,2})-(\d{1,2})">([^<]+)</h3> *\z]
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
            if (defined($last_time) && ($last_time <= $time))
            {
                die "Last time on line '$l' is not descending.";
            }
            $last_time = $time;

            my $html = "";
            my $html_l;

            GET_HTML:
            while (defined($html_l = shift(@lines)))
            {
                if ($html_l =~ m{\A<h3})
                {
                    last GET_HTML;
                }
                $html .= $html_l . "\n";
            }
            unshift (@lines, $html_l);
            push @results,
                {
                    year => $y,
                    mon => int($m),
                    day_of_month => int($d),
                    html => $html,
                };
        }
    }
}

io("MyOldNews.pm")->print(Data::Dumper->new([[reverse(@results)]])->Dump());
