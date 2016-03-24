#!/usr/bin/perl

use strict;
use warnings;

use DateTime;
use DateTime::Format::Strptime;
use Time::JulianDay;

use IO::All qw/io/;

my @releases;

my $strptime = DateTime::Format::Strptime->new(
    pattern => '%d-%b-%Y',
    locale => 'en_GB',
    time_zone => 'UTC',
    on_error => 'croak',
);

foreach my $l (io->file("./NEWS.txt")->chomp->getlines())
{
    if (my ($ver, $date_s) = $l =~ m#\AVersion ([0-9\.]+):? \(([0-9]+-[A-Z][a-z]+-[0-9]+)\)\z#)
    {
        if ($ver eq '0.4' or $ver eq '0.6' or $ver =~ /\.0\z/)
        {
            push @releases, +{
                ver => $ver,
                date => gm_julian_day($strptime->parse_datetime($date_s)->epoch()),
            };
        }
    }
}

unshift @releases, +{ ver => "Future", date => gm_julian_day(time()), };

@releases = reverse@releases;
foreach my $i (1 .. $#releases)
{
    my ($p, $n) = @releases[$i-1,$i];
    printf "%s→%s\t%d\n", $p->{ver}, $n->{ver}, $n->{date}-$p->{date};
}

=head1 COPYRIGHT & LICENSE

Copyright 2016 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
