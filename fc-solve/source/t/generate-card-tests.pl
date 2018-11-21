#!/usr/bin/perl

use strict;
use warnings;

use Template ();
use Path::Tiny qw/ path /;
use FindBin;

sub rank_normalize
{
    my $arg = shift;

    if ( ref($arg) eq "" )
    {
        return +{ map { $_ => $arg } (qw(t non_t)) };
    }
    else
    {
        return $arg;
    }
}

my $implicit_t =
    ( path("../include/freecell-solver/fcs_back_compat.h")->slurp_utf8 =~
        /^#define FCS_BREAK_BACKWARD_COMPAT_1\r?$/ms ? 1 : 0 );

my $template = Template->new( { ABSOLUTE => 1, }, );

sub indexify
{
    my $offset = shift;
    my $array  = shift;

    return [ map { +{ 'idx' => ( $offset + $_ ), 'value' => $array->[$_] } }
            ( 0 .. $#$array ) ];
}

my $args = {
    'suits' => indexify( 0, [qw(H C D S)] ),
    'ranks' => indexify(
        1,
        [
            map { rank_normalize($_) } (
                "A",
                ( 2 .. 9 ),
                {
                    't'     => "T",
                    'non_t' => "10",
                },
                ,
                "J", "Q", "K"
            )
        ]
    ),
    'implicit_t' => $implicit_t,
};

$template->process( "$FindBin::Bin/card-test-render.c.tt",
    $args, "card-test-render.c", )
    || die $template->error();

$template->process( "$FindBin::Bin/card-test-parse.c.tt",
    $args, "card-test-parse.c", )
    || die $template->error();

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
