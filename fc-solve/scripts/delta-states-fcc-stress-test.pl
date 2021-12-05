#!/usr/bin/perl

use strict;
use warnings;
use 5.014;
use bytes;

use Path::Tiny qw/ path /;

use FC_Solve::Paths qw/
    $FC_SOLVE_EXE
    bin_board
    is_freecell_only
    normalize_lf
    /;

use Games::Solitaire::Verify::VariantsMap ();
use FC_Solve::DeltaStater::FccFingerPrint ();

sub _canonicalize_board_string_columns
{
    my $s = shift;
    return $s =~
s#^(Freecells:)([^\n]+)#my($s,$e)=($1,$2);$s.join("",map{" $_"}sort split/\s+/,$e)#emrs
        =~ s#((?:^:[^\n]*\n)+)#join"",sort { $a cmp $b} split/^/ms, $1#emrs;
}

sub mytest
{
    my ($args)        = @_;
    my $DEAL_IDX      = $args->{DEAL_IDX};
    my $num_freecells = $args->{num_freecells};

    my $maxlen    = 0;
    my $LEN_LIMIT = [ 7, 7, 8, 8, 8 ]->[$num_freecells] or die;
    my $board_fn  = bin_board("$DEAL_IDX.board");
    my $fc_variant =
        Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id(
        'freecell');

    $fc_variant->num_freecells($num_freecells);
    my $init_state_str =
        normalize_lf( "Foundations: H-0 C-0 D-0 S-0\n"
            . "Freecells:\n"
            . ( path($board_fn)->slurp_raw() =~ s/^/: /gmrs ) );
    my $delta = FC_Solve::DeltaStater::FccFingerPrint->new(
        {
            variant        => 'custom',
            variant_params => $fc_variant,
            init_state_str => $init_state_str,
        }
    );

    my $was_printed = '';
    my $count       = 0;
    open my $exe_fh,
qq#$FC_SOLVE_EXE -l lg --freecells-num $num_freecells -sp r:tf -m -sel -c -p -t -mi 300 -s -i ${board_fn} |#;
READ_BOARD:
    while ( my $l = <$exe_fh> )
    {
        if ( $l =~ /\AFoundations:/ )
        {
            while ( ( my $m = <$exe_fh> ) =~ /\S/ )
            {
                $l .= $m;
            }
            if ( ( $l =~ /^Freecells:[^\n]*A/ms ) or ( $l =~ / A[CDHS]\n/ms ) )
            {
                next READ_BOARD;
            }
            my $state_str;
            my $encoded;
            eval {
                $delta->set_derived(
                    {
                        state_str => normalize_lf($l),
                    }
                );
                $state_str = $delta->_derived_state->to_string;
                $encoded   = $delta->encode_composite();
            };
            if ( my $err = $@ )
            {
                print "error <$err> when processing <$init_state_str> <$l> <>.";
                die $err;
            }

            die if @$encoded != 2;

            # say "gotencoded=<@$encoded>";
            $was_printed = 1;
            my $this_len = length $encoded->[1];
            if ( $this_len > $LEN_LIMIT )
            {
                $DB::single = 1;

                die "exceeded len = $this_len in deal $DEAL_IDX";
            }
            if ( $state_str !~ m#^:[^\n]*? A[CDHS]\n#ms )
            {
                my $round_trip_state;
                eval { $round_trip_state = $delta->decode($encoded); };
                if ( my $err = $@ )
                {
                    print "error <$err> when processing <$state_str>.";
                    die $err;
                }
                my $round_trip_str = $round_trip_state->to_string();
                die "mismatch $round_trip_str vs $state_str\n."
                    if _canonicalize_board_string_columns($round_trip_str) ne
                    _canonicalize_board_string_columns($state_str);
            }

            $maxlen = $this_len if $this_len > $maxlen;
        }
    }
    die "maxlen for deal = $DEAL_IDX" if ( $maxlen > $LEN_LIMIT );
    return;
}

{
SKIP:
    {
        if ( is_freecell_only() )
        {
            die "freecells hard coded to 4 - we need zero freecells";
        }

        foreach my $DEAL_IDX ( 1 .. 32_000 )
        {
            say "trying DEAL_IDX = $DEAL_IDX";
            foreach my $num_freecells ( 0 .. 4 )
            {
                say
"trying DEAL_IDX = $DEAL_IDX ; num_freecells = $num_freecells";
                mytest(
                    +{
                        DEAL_IDX      => $DEAL_IDX,
                        num_freecells => $num_freecells,
                    }
                );
            }
        }
    }
}
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2011 Shlomi Fish

=cut
