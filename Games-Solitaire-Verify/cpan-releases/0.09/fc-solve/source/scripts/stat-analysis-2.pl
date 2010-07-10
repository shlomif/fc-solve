#!/usr/bin/perl

use strict;
use warnings;

use Statistics::Descriptive;

my $filename = shift(@ARGV);

my @fields =
(
    {
        title => "Num FCS Moves"
    },
    {
        title => "Num FCPro Moves"
    },
);

foreach my $f (@fields)
{
    $f->{'s'} = Statistics::Descriptive::Full->new();
}

sub _slurp
{
    my $filename = shift;

    open my $in, "<", $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

my $dump = _slurp($filename);

foreach my $f (@fields)
{
    my $title = $f->{'title'};
    my @nums = ($dump =~ m{^\[\[\Q$title\E\]\]=(\d+)}msg);
    $f->{'s'}->add_data(@nums);
}

foreach my $f (@fields)
{
    my $s = $f->{'s'};
    print "$f->{title}\n";
    print "---------------------------\n";
    print "Min: " , $s->min(), "\n";
    print "Max: " , $s->max(), "\n";
    print "Average: " , $s->mean(), "\n";
    print "StdDev: " , $s->standard_deviation(), "\n";
    print "Median: " , $s->median(), "\n";
    print "\n";
}
