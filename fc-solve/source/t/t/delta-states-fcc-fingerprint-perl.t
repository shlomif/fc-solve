#!/usr/bin/perl

use strict;
use warnings;

use Path::Tiny qw/ path /;

use Test::More tests => 3;
use Test::Differences qw/ eq_or_diff /;
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

    my $maxlen   = 0;
    my $board_fn = bin_board("$DEAL_IDX.board");
    my $fc_variant =
        Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id(
        'freecell');

    $fc_variant->num_freecells($num_freecells);
    my $delta = FC_Solve::DeltaStater::FccFingerPrint->new(
        {
            variant        => 'custom',
            variant_params => $fc_variant,
            init_state_str => normalize_lf(
                      "Foundations: H-0 C-0 D-0 S-0\n"
                    . "Freecells:\n"
                    . ( path($board_fn)->slurp_raw() =~ s/^/: /gmrs )
            ),
        }
    );

    my $was_printed = '';
    my $count       = 0;
    open my $exe_fh,
qq#$FC_SOLVE_EXE -l tfts --freecells-num $num_freecells -sam -sel -c -p -t -mi 3000000 ${board_fn} |#;
    while ( my $l = <$exe_fh> )
    {
        if ( $l =~ /\AFoundations:/ )
        {
            while ( ( my $m = <$exe_fh> ) =~ /\S/ )
            {
                $l .= $m;
            }
            $delta->set_derived(
                {
                    state_str => normalize_lf($l),
                }
            );
            my $state_str = $delta->_derived_state->to_string;
            my $encoded   = $delta->encode_composite();
            die if @$encoded != 2;

            # say "gotencoded=<@$encoded>";
            $was_printed = 1;
            my $this_len = length $encoded->[1];
            if ( $this_len > 7 )
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
    return cmp_ok( $maxlen, '<=', 7, "maxlen for deal = $DEAL_IDX" );
}

sub unit_test1
{
    my ($args)        = @_;
    my $DEAL_IDX      = $args->{DEAL_IDX};
    my $num_freecells = $args->{num_freecells};

    my $fc_variant =
        Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id(
        'freecell');

    $fc_variant->num_freecells($num_freecells);
    my $init_state_str = q<Foundations: H-0 C-0 D-0 S-0
Freecells:
: JD KD 2S 4C 3S 6D 6S
: 2D KC KS 5C TD 8S 9C
: 9H 9S 9D TS 4S 8D 2H
: JC 5S QD QH TH QS 6H
: 5D AD JS 4H 8H 6C
: 7H QC AS AC 2C 3D
: 7C KH AH 4D JH 8C
: 5H 3H 3C 7S 7D TC
>;
    my $delta = FC_Solve::DeltaStater::FccFingerPrint->new(
        {
            variant        => 'custom',
            variant_params => $fc_variant,
            init_state_str => $init_state_str,
        }
    );
    my $maxlen = 0;

    for my $l (
        q<Foundations: H-0 C-2 D-0 S-A
Freecells:          3D
: 2D KC KS 5C TD 8S 9C
: 5H 3H 3C 7S 7D TC
: 5D AD JS 4H 8H 6C
: 7H QC
: 7C KH AH 4D JH 8C
: 9H 9S 9D TS 4S 8D 2H
: JC 5S QD QH TH QS 6H
: JD KD 2S 4C 3S 6D 6S
>
        )
    {
        $delta->set_derived(
            {
                state_str => normalize_lf($l),
            }
        );
        my $state_str = $delta->_derived_state->to_string;
        my $encoded   = $delta->encode_composite();
        die if @$encoded != 2;
        my $this_len = length $encoded->[1];

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
    return cmp_ok( $maxlen, '<=', 7, "maxlen for deal = $DEAL_IDX" );
}

{
SKIP:
    {
        if ( is_freecell_only() )
        {
            Test::More::skip(
                "freecells hard coded to 4 - we need zero freecells", 3 );
        }

        # TEST
        mytest( +{ DEAL_IDX => 164, num_freecells => 0, } );

        # TEST
        mytest( +{ DEAL_IDX => 164, num_freecells => 2, } );

        # TEST
        unit_test1( +{ DEAL_IDX => 1, num_freecells => 3, } );
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
