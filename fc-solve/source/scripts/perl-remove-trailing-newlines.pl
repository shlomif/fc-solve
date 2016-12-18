#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use Getopt::Long qw/ GetOptions /;

my $output;
GetOptions( '-o=s' => \$output, )
    or die "Failed - $!";

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

my $t = '';
foreach my $fn (@ARGV)
{
    $t .= _slurp($fn);
}

$t =~ s{[\n\r]+\z}{};

open my $o, '>', $output;
print {$o} $t;
close $o;
