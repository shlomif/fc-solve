use 5.014;
use strict;
use warnings;
use Path::Tiny qw/ path /;
use Getopt::Long qw/ GetOptions /;

my $output;
GetOptions( '-o=s' => \$output, )
    or die "Failed - $!";

my $t = '';
foreach my $fn (@ARGV)
{
    $t .= path($fn)->slurp_utf8;
}

path($output)->spew_utf8( $t =~ s{[\n\r]+\z}{}r );
