#!/usr/bin/perl

use strict;
use warnings;

use autodie;

use List::Util (qw/any all none notall/);

my $filename = shift(@ARGV);

open my $in, '<', $filename;

while (!eof($in))
{
    my @stacks = ();
    my ($iteration, $depth, $foundations, $freecells);
    while (defined($_ = <$in>) && (! m/-----/) && (! m/-=-=-=/))
    {
        if (/^Iter/)
        {
            ($iteration) = m/\d+/g;
        }
        elsif (/^Depth/)
        {
            ($depth) = m/\d+/g;
        }
        elsif (/^Found/)
        {
            $foundations = $_;
            $foundations =~ s/^Foundations: //;
        }
        elsif (/^Freecells/)
        {
            $freecells = $_;
            $freecells =~ s/^Freecells: //;
        }
        elsif (/^:/)
        {
            my $s = $_;
            $s =~ s/^: //;
            push @stacks, $s;
        }
    }
    last if (/-=-=-=/);

    my $check;

    $check = sub {
        return (
            (grep { m/^\s*5C\s*$/; } @stacks) &&
            1
        );
    };

    $check = sub {
        return (
            (grep { m/JH 6D 5C/; } @stacks) &&
            1
        );
    };

    $check = sub {
        return (
            1 && #($foundations =~ /D-7/) && 
            ($foundations =~ /C-A/) &&
            ($foundations =~ /H-2/) &&
            (grep { m/KH QH JH/ } @stacks) &&
            (grep { m/7H 6H 5H 4H/ } @stacks) && 
            (grep { m/JC 10C/ } @stacks) &&
            (grep { m/JH 10H 9H 8H 7H/ } @stacks) &&
            (grep { m/KS QS JS/ } @stacks) &&
            (grep { m/8S 7S/ } @stacks) &&
            (grep { m/5S 4S/ } @stacks) &&
            1
        );
    };

    $check = sub {
        return (
            ($foundations =~ /D-A/) &&
            ($foundations =~ /C-A/) &&
            (grep { m/7H 6H 5H/ } @stacks) &&
            (grep { m/9D 8D/ } @stacks) &&
            (grep { m/KH QH/ } @stacks) &&
            1
        );
    };

    $check = sub {
        return (
            ($freecells =~ /KC/) &&
            ($freecells =~ /JD/) &&
            ($freecells =~ /4S/) &&
            ($freecells =~ /KS/) &&
            1
        );
    };

    if ($check->())
    {
        print "I=$iteration D=$depth\n";
    }

}

close($in);
