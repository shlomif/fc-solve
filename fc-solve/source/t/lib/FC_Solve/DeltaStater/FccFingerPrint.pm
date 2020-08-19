package FC_Solve::DeltaStater::FccFingerPrint;

use strict;
use warnings;
use bytes;
use 5.014;

use parent 'FC_Solve::DeltaStater::DeBondt';

use Carp ();
use List::Util qw/ first max /;

use Games::Solitaire::Verify::Card        ();
use Games::Solitaire::Verify::Foundations ();
use Games::Solitaire::Verify::State       ();

use FC_Solve::VarBaseDigitsReader::XS ();
use FC_Solve::VarBaseDigitsWriter::XS ();

use Math::BigInt try => 'GMP';

=head1 ALGORITHM

See git/fc-solve/fc-solve/docs/debondt-compact-freecell-positions--document.asciidoc

=cut

__PACKAGE__->mk_acc_ref( [qw(_proxied_worker)] );

use FC_Solve::DeltaStater::Constants qw/
    $NUM_OPTS
    $OPT_DONT_CARE
    $OPT_FREECELL
    $OPT_ORIG_POS
    $OPT_PARENT_SUIT_MOD_IS_0
    $OPT_PARENT_SUIT_MOD_IS_1
    $OPT_TOPMOST
    $RANK_KING
    /;

my $ORIG_POS                         = 0;
my $ABOVE_PARENT_CARD_OR_EMPTY_SPACE = 1;
my $IN_FOUNDATIONS                   = 2;
my $INFERRED_SUB_STATE               = -1;

package FC_Solve::DeltaStater::FccFingerPrint::StatesRecord;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref(
    [
        qw(
            CARD_PAIR_STATES_MAP
            CARD_PAIR_STATE_BASE
            REVERSE_CARD_PAIR_STATES_MAP
            does_have_zero_freecells
            num_single_card_states
            rev_single_card_states
            should_skip_is_king
            single_card_states
            state_opt_next
            )
    ]
);

sub _init
{
    my ( $self, $args ) = @_;
    $self->does_have_zero_freecells( $args->{does_have_zero_freecells} );

    $self->REVERSE_CARD_PAIR_STATES_MAP( [] );
    $self->single_card_states( {} );
    $self->rev_single_card_states( [] );
    $self->num_single_card_states(0);
    $self->state_opt_next(0);
    $self->should_skip_is_king( $self->does_have_zero_freecells() );
    return;
}

sub _add_state_pair
{
    my ( $rec, $arg ) = @_;
    my @input = @$arg;
    die if @input != 2;
    my @s = ( grep { !exists $rec->single_card_states()->{$_} } @input );
    die if grep { $_ ne 'ABOVE_FREECELL' } @s;
    if ( not @s )
    {
        my ( $y, $x ) = @{ $rec->single_card_states() }{@input};

        die if $y < 0 || $y >= @{ $rec->CARD_PAIR_STATES_MAP() };
        die if $x < 0 || $x >= @{ $rec->CARD_PAIR_STATES_MAP()->[$y] };
        die if defined $rec->CARD_PAIR_STATES_MAP()->[$y]->[$x];
        my $val = $rec->{state_opt_next}++;
        $rec->CARD_PAIR_STATES_MAP()->[$y]->[$x] = $val;
        $rec->REVERSE_CARD_PAIR_STATES_MAP()->[$val] = [
            map {
                FC_Solve::DeltaStater::FccFingerPrint::StatesRecord::_string_to_int(
                    $_)
            } @input
        ];
    }
    return;

}

sub finalize
{
    my $rec = shift;
    $rec->CARD_PAIR_STATES_MAP(
        [
            map {
                [ map { undef() } 0 .. $rec->num_single_card_states() - 1 ]
            } 0 .. $rec->num_single_card_states() - 1
        ]
    );
    my $_add = sub {
        $rec->_add_state_pair( [@_] );
        return;
    };
    $_add->( 'LOWEST_CARD',    'LOWEST_CARD' );
    $_add->( 'LOWEST_CARD',    'ABOVE_FREECELL' );
    $_add->( 'ABOVE_FREECELL', 'LOWEST_CARD' );
    $_add->( 'ABOVE_FREECELL', 'ABOVE_FREECELL' );
    $_add->( 'LOWEST_CARD',    'PARENT_0' );
    $_add->( 'LOWEST_CARD',    'PARENT_1' );
    $_add->( 'ABOVE_FREECELL', 'PARENT_0' );
    $_add->( 'ABOVE_FREECELL', 'PARENT_1' );
    $_add->( 'PARENT_0',       'LOWEST_CARD' );
    $_add->( 'PARENT_1',       'LOWEST_CARD' );
    $_add->( 'PARENT_0',       'ABOVE_FREECELL' );
    $_add->( 'PARENT_1',       'ABOVE_FREECELL' );
    $_add->( 'PARENT_0',       'PARENT_1' );
    $_add->( 'PARENT_1',       'PARENT_0' );
    $rec->CARD_PAIR_STATE_BASE( $rec->state_opt_next() );
    return;
}

use FC_Solve::DeltaStater::Constants qw/
    $NUM_OPTS
    $OPT_DONT_CARE
    $OPT_FREECELL
    $OPT_ORIG_POS
    $OPT_PARENT_SUIT_MOD_IS_0
    $OPT_PARENT_SUIT_MOD_IS_1
    $OPT_TOPMOST
    $RANK_KING
    /;

sub _string_to_int
{
    local $_ = shift;
    return
          $_ eq 'ABOVE_FREECELL' ? $OPT_FREECELL
        : $_ eq 'PARENT_0'       ? $OPT_PARENT_SUIT_MOD_IS_0
        : $_ eq 'PARENT_1'       ? $OPT_PARENT_SUIT_MOD_IS_1
        : $_ eq 'LOWEST_CARD'    ? $OPT_TOPMOST
        :                          do { die $_ };
}

sub _my_add_state
{
    my ( $rec, $state ) = @_;
    my $hash     = $rec->single_card_states();
    my $rev_hash = $rec->rev_single_card_states();
    my $val      = ( ( $rec->{num_single_card_states} )++ );
    $rev_hash->[$val] = _string_to_int($state);
    $hash->{$state} = $val;
    return;
}

package FC_Solve::DeltaStater::FccFingerPrint;

my $positive_rec = FC_Solve::DeltaStater::FccFingerPrint::StatesRecord->new(
    { does_have_zero_freecells => '' } );
my $zero_rec = FC_Solve::DeltaStater::FccFingerPrint::StatesRecord->new(
    { does_have_zero_freecells => 1 } );
foreach my $rec ( $positive_rec, $zero_rec )
{
    foreach my $state (qw/ LOWEST_CARD ABOVE_FREECELL PARENT_0 PARENT_1 /)
    {
        if (
            not(    $rec->does_have_zero_freecells()
                and $state eq 'ABOVE_FREECELL' )
            )
        {
            $rec->_my_add_state($state);
        }
    }
    $rec->finalize();
}

die if $zero_rec->CARD_PAIR_STATE_BASE() ne 7;

my @suits = @{ Games::Solitaire::Verify::Card->get_suits_seq() };

sub _init
{
    my ( $self, $args ) = @_;

    $self->SUPER::_init($args);
    $self->_proxied_worker(
        FC_Solve::DeltaStater::FccFingerPrint::ProxiedWorker->new( $args, ) );

    die
        if $self->_proxied_worker->_init_state()->num_freecells() ne
        $self->_init_state->num_freecells();
    if ( $self->_is_bakers_dozen() )
    {
        die "unimplemented";
    }

    return;
}

my $zero_fc_variant =
    Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

$zero_fc_variant->num_freecells(0);

sub _calc_state_obj_generic
{
    my ( $self, $args ) = @_;
    return $self->_is_bakers_dozen()
        ? ( die "unimpl" )
        : Games::Solitaire::Verify::State->new(
        {
            variant        => 'custom',
            variant_params => $zero_fc_variant,
            %{$args},
        },
        );
}

sub _initialize_card_states
{
    my ( $self, $num_opts ) = @_;

    $self->_card_states( [ map { [] } ( 0 .. $RANK_KING * 4 - 1 ) ] );

    return;
}

package FC_Solve::DeltaStater::FccFingerPrint::OptRecord;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref(
    [
        qw/
            fingerprint_state
            sub_state
            /
    ]
);

sub _init
{
    my ( $self, $args ) = @_;
    $self->fingerprint_state( $args->{fingerprint_state} );
    $self->sub_state( $args->{sub_state} );

    return;
}

package FC_Solve::DeltaStater::FccFingerPrint;

sub _mark_suit_rank_as_true
{
    my ( $self, $suit, $rank, $opt ) = @_;

    die if not $opt->isa('FC_Solve::DeltaStater::FccFingerPrint::OptRecord');

    @{ $self->_opt_by_suit_rank( $suit, $rank ) } = ($opt);

    return;
}

sub _mark_opt_as_true
{
    my ( $self, $card, $opt ) = @_;
    die if not $opt->isa('FC_Solve::DeltaStater::FccFingerPrint::OptRecord');

    @{ $self->_opt_by_card($card) } = (@$opt);

    return;
}

sub _encode_single_uknown_info_card
{
    my ( $self, $state_writer, $opt, $is_king, $variant_states ) = @_;
    my $state_o = $opt->sub_state;
    if ($is_king)
    {
        $state_writer->write( { base => 2, item => $state_o } );
    }
    else
    {
        $state_writer->write(
            {
                base => $variant_states->num_single_card_states(),
                item => $state_o
            }
        );
    }
    return;
}

sub _encode_a_pair_of_uknown_info_cards
{
    my ( $self, $state_writer, $opt1, $opt2, $is_king, $variant_states ) = @_;
    my @states = ( $opt1->sub_state, $opt2->sub_state );
    if ($is_king)
    {
        foreach my $state_o (@states)
        {
            $state_writer->write( { base => 2, item => $state_o } );
        }
    }
    else
    {
        $state_writer->write(
            {
                base => (
                    $variant_states->CARD_PAIR_STATE_BASE()
                        // do { die }
                ),
                item => (
                    $variant_states->CARD_PAIR_STATES_MAP()->[ $states[0] ]
                        ->[ $states[1] ] // do
                    {
                        die "unknown key @states";
                    }
                ),
            }
        );
    }

    return;
}

sub encode_composite
{
    my ($self) = @_;

    my $derived = $self->_derived_state;

    my $variant_states      = $self->_calc_variant_states();
    my $should_skip_is_king = $variant_states->should_skip_is_king();

    # die if $derived->num_freecells > 0;

    $self->_initialize_card_states_for_encode();

    my $state_writer       = FC_Solve::VarBaseDigitsWriter::XS->new;
    my $fingerprint_writer = FC_Solve::VarBaseDigitsWriter::XS->new;

    my @cols_indexes = ( 0 .. $derived->num_columns - 1 );
    foreach my $col_idx (@cols_indexes)
    {
        my $col     = $derived->get_column($col_idx);
        my $col_len = $col->len();

        foreach my $height ( 0 .. $col_len - 1 )
        {
            my $card = $col->pos($height);
            my $o;
            if ( $height == 0 )
            {
                $o = FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                    {
                        fingerprint_state => $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                        sub_state         => (
                            $variant_states->single_card_states()
                                ->{'LOWEST_CARD'} // ( die "no LOWEST_CARD" )
                        ),
                    }
                );
            }
            else
            {
                my $parent_card = $col->pos( $height - 1 );
                if ( $self->_is_parent_card( $parent_card, $card ) )
                {
                    $o = FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                        {
                            fingerprint_state =>
                                $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                            sub_state => (
                                (
                                    $self->_suit_get_suit_idx(
                                        $parent_card->suit
                                    ) & 2
                                )
                                ? ( $variant_states->single_card_states()
                                        ->{'PARENT_1'}
                                        // ( die "no PARENT_1" ) )
                                : ( $variant_states->single_card_states()
                                        ->{'PARENT_0'}
                                        // ( die "no PARENT_0" ) )
                            ),
                        }
                    );
                }
                else
                {
                    $o = FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                        {
                            fingerprint_state => $ORIG_POS,
                            sub_state         => $INFERRED_SUB_STATE,
                        }
                    );
                }

            }
            $self->_mark_suit_rank_as_true(
                $self->_suit_get_suit_idx( $card->suit ),
                $card->rank, $o );
        }
    }
    foreach my $fc_idx ( 0 .. $derived->num_freecells - 1 )
    {
        my $card = $derived->get_freecell($fc_idx);

        if ( defined($card) )
        {
            $self->_mark_opt_as_true(
                $card,
                FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                    {
                        fingerprint_state => $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                        sub_state         => (
                            $variant_states->single_card_states()
                                ->{'ABOVE_FREECELL'}
                                // ( die "no ABOVE_FREECELL" )
                        ),
                    }
                ),
            );
        }
    }

    # We encode the foundations separately so set the card value as don't care.
    my $cnt = 0;
    foreach my $rank ( 1 .. $RANK_KING )
    {
        foreach my $color ( 0 .. 1 )
        {
            my $suit1 = $color;
            my $suit2 = ( $color | 2 );
            my $rank1 = $derived->get_foundation_value( $suits[$suit1], 0 );
            my $rank2 = $derived->get_foundation_value( $suits[$suit2], 0 );
            my $is_founds1 = ( $rank1 >= $rank );
            my $is_founds2 = ( $rank2 >= $rank );
            my $opt1 =
                $is_founds1
                ? FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                {
                    fingerprint_state => $IN_FOUNDATIONS,
                    sub_state         => $INFERRED_SUB_STATE,
                }
                )
                : $self->_opt_by_suit_rank( $suit1, $rank )->[0];
            my $opt2 =
                $is_founds2
                ? FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                {
                    fingerprint_state => $IN_FOUNDATIONS,
                    sub_state         => $INFERRED_SUB_STATE,
                }
                )
                : $self->_opt_by_suit_rank( $suit2, $rank )->[0];
            die if !( defined $opt1->fingerprint_state );

            if ( !( defined $opt2->fingerprint_state ) )
            {
                $DB::single = 1;
                Carp::confess("opt2");
            }
            my $fingerprint1 = $opt1->fingerprint_state;
            my $fingerprint2 = $opt2->fingerprint_state;
            my $is_king      = ( $rank == $RANK_KING );

            if ( $rank > 1 && ( not( $is_king && $should_skip_is_king ) ) )
            {
                if ( $fingerprint1 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                {
                    if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                    {
                    }
                    else
                    {
                        $self->_encode_single_uknown_info_card( $state_writer,
                            $opt2, $is_king, $variant_states );
                    }
                }
                else
                {
                    if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                    {
                        $self->_encode_single_uknown_info_card( $state_writer,
                            $opt1, $is_king, $variant_states );
                    }
                    else
                    {
                        $self->_encode_a_pair_of_uknown_info_cards(
                            $state_writer,
                            $opt1, $opt2, $is_king, $variant_states );
                    }
                }
            }

            # print("$rank $color $fingerprint1 $fingerprint2\n");

            $fingerprint_writer->write( { base => 3, item => $fingerprint1 } );
            $fingerprint_writer->write( { base => 3, item => $fingerprint2 } );
            $cnt += 2;
        }
    }

    if ( $cnt != 52 )
    {
        ...;
    }
    $self->_free_card_states;

    my $_data = sub {
        my $n = shift()->get_data;
        return $n;

=begin foo

        my $ret = '';
        while ( $n > 0 )
        {
            $ret .= chr( $n & 255 );
            $n >>= 8;
        }
        return $ret;
=end foo

=cut

    };

    my $ret = $_data->($fingerprint_writer);
    return [ $ret, $_data->($state_writer) ];
}

sub _calc_variant_states
{
    return shift()->_init_state->num_freecells ? $positive_rec : $zero_rec;
}

my @SUIT_INDEXES =
    ( map { my $color = $_; [ $color, ( $color | 2 ) ] } 0 .. 1 );

my $CARD_STATES_SKIP = -1;

sub decode
{
    my ( $self, $encoded ) = @_;

    my $variant_states      = $self->_calc_variant_states();
    my $should_skip_is_king = $variant_states->should_skip_is_king();
    die "@$encoded" unless @$encoded == 2;
    my $_reader = sub {
        my $idx = shift;
        my $buf = $encoded->[$idx];

=begin foo
        my $n      = Math::BigInt->new(0);
        my $factor = 0;
        foreach my $byte ( split //,$buf,  -1 )
        {
            $n |=
                ( Math::BigInt->new( ord($byte) ) << $factor );
            $factor += 8;
        }
=end foo

=cut

        return FC_Solve::VarBaseDigitsReader::XS->new( { data => $buf, } );
    };
    my $fingerprint_reader = $_reader->(0);
    my $state_reader       = $_reader->(1);

    my @queue;
    my @fingerprints;
    my @card_states;

    my $foundations_obj = Games::Solitaire::Verify::Foundations->new(
        {
            num_decks => 1,
        },
    );
    foreach my $rank ( 1 .. $RANK_KING )
    {
        my $is_king = ( $rank == $RANK_KING );
        foreach my $indexes (@SUIT_INDEXES)
        {
            my @are_not_set;
            foreach my $suit_idx (@$indexes)
            {
                my $f   = $fingerprint_reader->read(3);
                my $val = $CARD_STATES_SKIP;
                my $newval;
                if ( $f == $IN_FOUNDATIONS )
                {
                    # say "ff=$f $suit_idx";
                    $foundations_obj->assign( $suits[$suit_idx], 0, $rank );
                    $newval = $OPT_DONT_CARE;
                }
                elsif ( $f == $ORIG_POS )
                {
                    $newval = $OPT_ORIG_POS;
                }
                else
                {
                    $newval = (
                        ( $is_king && $should_skip_is_king )
                        ? $OPT_TOPMOST
                        : (-2)
                    );
                }
                if ( $rank > 1 )
                {
                    $val = $newval;
                }

                $card_states[$suit_idx][$rank]  = $val;
                $fingerprints[$suit_idx][$rank] = $f;

                push @are_not_set, scalar( $val == -2 );
            }
            if ( $are_not_set[0] or $are_not_set[1] )
            {
                if ( $are_not_set[0] and $are_not_set[1] )
                {
                    my $s = $state_reader->read(
                        $variant_states->CARD_PAIR_STATE_BASE()
                            // do { die }
                    );
                    my @pos = @{
                        $variant_states->REVERSE_CARD_PAIR_STATES_MAP()->[$s]
                            // do { die }
                    };
                    for my $i ( keys @pos )
                    {
                        $card_states[ $indexes->[$i] ][$rank] = $pos[$i];
                    }
                }
                else
                {
                    my $s = $state_reader->read(
                        $variant_states->num_single_card_states()
                            // do { die }
                    );
                    my $pos =
                        $variant_states->rev_single_card_states()->[$s]
                        // do { die };
                    $card_states[
                        $indexes->[
                        first { $are_not_set[$_] }
                    0 .. 1
                        ]
                    ][$rank] = $pos;
                }
            }
        }
    }

    foreach my $suit_idx ( 0 .. $#suits )
    {
        push @queue, $foundations_obj->value( $suits[$suit_idx], 0 );
    }

    # say "@queue";
    foreach my $rank ( 1 .. $self->_get_top_rank_for_iter() )
    {
        foreach my $suit_idx ( 0 .. $#suits )
        {
            my $val = $card_states[$suit_idx][$rank];
            if ( $val >= 0 )
            {
                push @queue, $val;
            }
            else
            {
                die if $val != $CARD_STATES_SKIP;
            }
        }
    }
    my $ret = $self->_proxied_worker()->decode( \@queue, );

    die if @queue;

    return $ret;
}

package FC_Solve::DeltaStater::FccFingerPrint::ProxiedWorker;

use parent 'FC_Solve::DeltaStater::DeBondt';

sub _calc_reader_from_data
{
    my ( $self, $queue ) = @_;
    return FC_Solve::DeltaStater::FccFingerPrint::ProxiedWorker::Reader->new(
        {
            _q => $queue,
        },
    );
}

package FC_Solve::DeltaStater::FccFingerPrint::ProxiedWorker::Reader;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _q
            )
    ]
);

sub _init
{
    my ( $self, $args ) = @_;

    $self->_q(
        $args->{_q}
            || do { die "false"; }
    );

    return;
}

sub read
{
    my ( $self, $base ) = @_;

    my $ret = shift( @{ $self->{_q} } );

    die if $ret ne int($ret);
    die if $ret < 0;
    die if $ret >= $base;

    return $ret;
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2020 Shlomi Fish

=cut
