#!/usr/bin/perl

use strict;
use warnings;

use DateTime                   ();
use DateTime::Format::Strptime ();
use Time::JulianDay qw/ gm_julian_day /;

use IO::All qw/ io /;

my @releases;

my $strptime = DateTime::Format::Strptime->new(
    pattern   => '%d-%b-%Y',
    locale    => 'en_GB',
    time_zone => 'UTC',
    on_error  => 'croak',
);

foreach my $l ( io->file("./NEWS.txt")->chomp->getlines() )
{
    if ( my ( $ver, $date_s ) =
        $l =~ m#\AVersion ([0-9\.]+):? \(([0-9]+-[A-Z][a-z]+-[0-9]+)\)\z# )
    {
        if ( $ver eq '0.4' or $ver eq '0.6' or $ver =~ /\.0\z/ )
        {
            push @releases,
                +{
                ver  => $ver,
                date => gm_julian_day(
                    $strptime->parse_datetime($date_s)->epoch()
                ),
                };
        }
    }
}

unshift @releases, +{ ver => "Future", date => gm_julian_day( time() ), };

@releases = reverse @releases;
foreach my $i ( 1 .. $#releases )
{
    my ( $p, $n ) = @releases[ $i - 1, $i ];
    printf "%s→%s\t%d\n", $p->{ver}, $n->{ver}, $n->{date} - $p->{date};
}
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2016 Shlomi Fish

=cut
