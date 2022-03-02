package FC_Solve::DeltaStater::FccFingerPrint;

use strict;
use warnings;
use bytes;
use 5.014;

use parent 'FC_Solve::DeltaStater::DeBondt';

use Carp ();
use List::Util qw/ any first max uniq /;

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
    $NUM_KING_OPTS
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

sub _assert_def
{
    my $val = shift;
    if ( $val < 0 )
    {
        Carp::confess("negative");
    }
    return $val // do { Carp::confess("undefined"); };
}

package FC_Solve::DeltaStater::FccFingerPrint::StatesRecord;

use parent 'Games::Solitaire::Verify::Base';
use List::Util qw/ any first max uniq /;

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
                1
                    ? $rec->single_card_states->{$_}
                    : (
                    FC_Solve::DeltaStater::FccFingerPrint::StatesRecord::_string_to_int(
                        $_)
                    )
            } @input
        ];
    }
    return;

}

sub _assert_def
{
    my $val = shift;
    if ( $val < 0 )
    {
        Carp::confess("negative");
    }
    return $val // do { Carp::confess("undefined"); };
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
    $_add->( 'LEFTMOST_CARD',  'LEFTMOST_CARD' );
    $_add->( 'LEFTMOST_CARD',  'ABOVE_FREECELL' );
    $_add->( 'ABOVE_FREECELL', 'LEFTMOST_CARD' );
    $_add->( 'ABOVE_FREECELL', 'ABOVE_FREECELL' );
    $_add->( 'LEFTMOST_CARD',  'PARENT_0' );
    $_add->( 'LEFTMOST_CARD',  'PARENT_1' );
    $_add->( 'ABOVE_FREECELL', 'PARENT_0' );
    $_add->( 'ABOVE_FREECELL', 'PARENT_1' );
    $_add->( 'PARENT_0',       'LEFTMOST_CARD' );
    $_add->( 'PARENT_1',       'LEFTMOST_CARD' );
    $_add->( 'PARENT_0',       'ABOVE_FREECELL' );
    $_add->( 'PARENT_1',       'ABOVE_FREECELL' );
    $_add->( 'PARENT_0',       'PARENT_1' );
    $_add->( 'PARENT_1',       'PARENT_0' );
    $rec->CARD_PAIR_STATE_BASE( $rec->state_opt_next() );
    _assert_def( $rec->rev_single_card_states->[1] );
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
        : $_ eq 'LEFTMOST_CARD'  ? $OPT_TOPMOST
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

sub _calc_initial_possible_pair_states
{
    my ($rec) = @_;
    my $variant_states = $rec;
    return {
        map {
            my $pair = $_;
            $pair => +{
                map {
                    my $rank   = $_;
                    my $values = [ 0 .. ( $rec->CARD_PAIR_STATE_BASE() - 1 ) ];
                    if ( $rank == $RANK_KING )
                    {
                        @$values = (
                            grep {
                                my $i = $_;
                                not any
                                {
                                    $_ == $variant_states->single_card_states()
                                        ->{'PARENT_0'}
                                        or $_ ==
                                        $variant_states->single_card_states()
                                        ->{'PARENT_1'}
                                    } @{ $rec->REVERSE_CARD_PAIR_STATES_MAP
                                        ->[$i] }
                            } @$values
                        );

                        # say "values = @$values";
                    }
                    $rank => $values
                } ( 1 .. $RANK_KING )
            };
        } 0 .. 1
    };
}

sub _calc__remaining__single_options
{
    my ( $variant_states, $card_idx, $options ) = @_;

    my @single_options = sort { $a <=> $b } uniq(
        map {
            my $xx = $variant_states->REVERSE_CARD_PAIR_STATES_MAP()->[$_]
                ->[$card_idx];

            $xx
        } @$options
    );
    if ( @single_options > $variant_states->num_single_card_states )
    {
        die;
    }
    return \@single_options;
}

package FC_Solve::DeltaStater::FccFingerPrint;

my $positive_rec = FC_Solve::DeltaStater::FccFingerPrint::StatesRecord->new(
    { does_have_zero_freecells => '' } );
my $zero_rec = FC_Solve::DeltaStater::FccFingerPrint::StatesRecord->new(
    { does_have_zero_freecells => 1 } );
foreach my $rec ( $positive_rec, $zero_rec )
{
    foreach my $state (qw/ LEFTMOST_CARD ABOVE_FREECELL PARENT_0 PARENT_1 /)
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

sub _initialize_card_states
{
    my ( $self, $num_opts ) = @_;

    $self->_card_states(
        [
            map {
                do { my $x; \$x }
            } ( 0 .. $RANK_KING * 4 - 1 )
        ]
    );

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

__PACKAGE__->mk_acc_ref(
    [
        qw[
            _color
            _rank
            _possible_pair_states
            _variant_states
        ]
    ]
);

sub _set_opt
{
    my ( $self, $ref, $opt ) = @_;

    die if not $opt->isa('FC_Solve::DeltaStater::FccFingerPrint::OptRecord');

    $$ref = $opt;

    return;
}

use List::MoreUtils qw/ firstidx /;

sub _encode_single_uknown_info_card
{
    my ( $self, $card_idx, $state_writer, $opt, $is_king, $variant_states ) =
        @_;
    my $color                 = $self->_color;
    my $rank                  = $self->_rank;
    my $_possible_pair_states = $self->_possible_pair_states();
    my @options               = @{ $_possible_pair_states->{$color}{$rank} };
    my $single_options =
        $variant_states->_calc__remaining__single_options( $card_idx,
        \@options );
    $self->_set_card_opt( $color, $rank, $opt->sub_state );
    if ($is_king)
    {
        if (
            ( !$variant_states->does_have_zero_freecells() )
            and (
                any
                {
                    $_ == $variant_states->single_card_states()
                        ->{'ABOVE_FREECELL'}
                } @$single_options
            )
            )
        {
            my $state_o = _assert_def(
                scalar firstidx { $_ == $opt->sub_state }
                @$single_options
            );
            $state_writer->write( { base => 2, item => $state_o } );
        }
    }
    else
    {
        my $state_o = _assert_def(
            scalar firstidx { $_ == $opt->sub_state }
            @$single_options
        );
        $state_writer->write(
            {
                base => ( scalar @$single_options ),
                item => $state_o
            }
        );
    }
    return;
}

sub _encode_a_pair_of_uknown_info_cards
{
    my ( $self, $state_writer, $opt1, $opt2, $is_king, $variant_states ) = @_;
    my @states                = ( $opt1->sub_state, $opt2->sub_state );
    my $color                 = $self->_color;
    my $rank                  = $self->_rank;
    my $_possible_pair_states = $self->_possible_pair_states();
    foreach my $s (@states)
    {
        $self->_set_card_opt( $color, $rank, $s );
    }
    my @options = @{ $_possible_pair_states->{$color}{$rank} };
    if ( @options > $variant_states->CARD_PAIR_STATE_BASE )
    {
        die;
    }
    if ($is_king)
    {
        while ( my ( $card_idx, $state_o ) = each(@states) )
        {
            if (
                ( !$variant_states->does_have_zero_freecells() )
                and (
                    any
                    {
                        my $xx = $variant_states->REVERSE_CARD_PAIR_STATES_MAP()
                            ->[$_]->[$card_idx];
                        $xx == $variant_states->single_card_states()
                            ->{'ABOVE_FREECELL'}
                    } @options
                )
                )
            {
                $state_writer->write( { base => 2, item => $state_o } );
            }
        }
    }
    else
    {
        my $state_o = firstidx
        {
            $_ == $variant_states->CARD_PAIR_STATES_MAP()->[ $states[0] ]
                ->[ $states[1] ]
        }
        @options;
        $state_writer->write(
            {
                base => _assert_def( scalar(@options) ),
                item => _assert_def($state_o),
            }
        );
    }

    return;
}

sub _encode_cards_pair
{
    my ( $self, $state_writer, $opt1, $opt2, $is_king, $variant_states ) = @_;
    my $fingerprint1 = $opt1->fingerprint_state;
    my $fingerprint2 = $opt2->fingerprint_state;
    if ( $fingerprint1 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
    {
        if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
        {
        }
        else
        {
            $self->_encode_single_uknown_info_card( 1, $state_writer,
                $opt2, $is_king, $variant_states );
        }
    }
    else
    {
        if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
        {
            $self->_encode_single_uknown_info_card( 0, $state_writer,
                $opt1, $is_king, $variant_states );
        }
        else
        {
            $self->_encode_a_pair_of_uknown_info_cards( $state_writer,
                $opt1, $opt2, $is_king, $variant_states );
        }
    }
    return;
}

sub _calc_encoded_OptRecord
{
    my ( $self, $col, $height, $card, $variant_states ) = @_;

    if ( $height == 0 )
    {
        return FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
            {
                fingerprint_state => $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                sub_state         => _assert_def(
                    $variant_states->single_card_states()->{'LEFTMOST_CARD'}
                ),
            }
        );
    }
    else
    {
        my $parent_card = $col->pos( $height - 1 );
        if ( $self->_is_parent_card( $parent_card, $card ) )
        {
            return FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                {
                    fingerprint_state => $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                    sub_state         => (
                        ( $self->_get_suit_idx($parent_card) & 2 )
                        ? _assert_def(
                            $variant_states->single_card_states()->{'PARENT_1'}
                            )
                        : _assert_def(
                            $variant_states->single_card_states()->{'PARENT_0'}
                        )
                    ),
                }
            );
        }
        else
        {
            return FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                {
                    fingerprint_state => $ORIG_POS,
                    sub_state         => $INFERRED_SUB_STATE,
                }
            );
        }

    }
}

sub encode_composite
{
    my ($self) = @_;

    my $derived = $self->_derived_state;

    my $variant_states      = $self->_calc_variant_states();
    my $should_skip_is_king = $variant_states->should_skip_is_king();

    my $_possible_pair_states = $self->_possible_pair_states(
        $variant_states->_calc_initial_possible_pair_states() );

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
            my $o =
                $self->_calc_encoded_OptRecord( $col, $height, $card,
                $variant_states );

            $self->_mark_suit_rank_as_true( $self->_get_suit_idx($card),
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
                        sub_state         => _assert_def(
                            $variant_states->single_card_states()
                                ->{'ABOVE_FREECELL'}
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
        $self->_rank($rank);
        foreach my $color ( 0 .. 1 )
        {
            $self->_color($color);
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
                : ${ $self->_opt_by_suit_rank( $suit1, $rank ) };
            my $opt2 =
                $is_founds2
                ? FC_Solve::DeltaStater::FccFingerPrint::OptRecord->new(
                {
                    fingerprint_state => $IN_FOUNDATIONS,
                    sub_state         => $INFERRED_SUB_STATE,
                }
                )
                : ${ $self->_opt_by_suit_rank( $suit2, $rank ) };
            die if !( defined $opt1->fingerprint_state );

            if ( ( !defined $opt2 ) or !( defined $opt2->fingerprint_state ) )
            {
                $DB::single = 1;
                Carp::confess("opt2");
            }
            my $fingerprint1 = $opt1->fingerprint_state;
            my $fingerprint2 = $opt2->fingerprint_state;
            my $is_king      = ( $rank == $RANK_KING );

            if ( $rank > 1 && ( not( $is_king && $should_skip_is_king ) ) )
            {
                $self->_encode_cards_pair( $state_writer,
                    $opt1, $opt2, $is_king, $variant_states );
            }

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
    my ($self) = @_;

    # Carp::confess( "numfc = " . $self->_init_state->num_freecells );
    return $self->_variant_states(
        $self->_init_state->num_freecells ? $positive_rec : $zero_rec );
}

my @SUIT_INDEXES =
    ( map { my $color = $_; [ $color, ( $color | 2 ) ] } 0 .. 1 );

my $CARD_STATES_SKIP              = -1;
my $CARD_STATES__TO_BE_DETERMINED = -2;

sub _set_card_opt
{
    my ( $self, $color, $rank, $opt ) = @_;
    if ( $rank == $RANK_KING )
    {
        return $opt;
    }
    my $variant_states        = $self->_variant_states;
    my $_possible_pair_states = $self->_possible_pair_states();
    if ( $variant_states->does_have_zero_freecells() )
    {
        return $opt;
    }
    my $parentrank = $rank + 1;
    my $card_idx   = (
        _assert_def($opt) ==
            _assert_def( $variant_states->single_card_states->{'PARENT_1'} )
        ? 1
        : _assert_def($opt) ==
            _assert_def( $variant_states->single_card_states->{'PARENT_0'} ) ? 0
        : ( return $opt )
    );
    my $aref   = $_possible_pair_states->{ 0b1 ^ $color }{$parentrank};
    my @before = @$aref;
    @$aref = (
        grep {
            _assert_def(
                $variant_states->REVERSE_CARD_PAIR_STATES_MAP()->[$_]
                    ->[$card_idx] ) != _assert_def(
                $variant_states->single_card_states->{'ABOVE_FREECELL'} )
        } @before
    );

    return $opt;
}

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
    my $_possible_pair_states = $self->_possible_pair_states(
        $variant_states->_calc_initial_possible_pair_states() );

    my $foundations_obj = Games::Solitaire::Verify::Foundations->new(
        {
            num_decks => 1,
        },
    );
    foreach my $rank ( 1 .. $RANK_KING )
    {
        my $is_king = ( $rank == $RANK_KING );
        while ( my ( $color, $indexes ) = each(@SUIT_INDEXES) )
        {
            my @are_not_set;
            foreach my $suit_idx (@$indexes)
            {
                my $f   = $fingerprint_reader->read(3);
                my $val = $CARD_STATES_SKIP;
                my $newval;
                if ( $f == $IN_FOUNDATIONS )
                {
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
                        : $CARD_STATES__TO_BE_DETERMINED
                    );
                }
                if ( $rank > 1 )
                {
                    $val = $newval;
                }

                $card_states[$suit_idx][$rank]  = $val;
                $fingerprints[$suit_idx][$rank] = $f;

                push @are_not_set,
                    scalar( $val == $CARD_STATES__TO_BE_DETERMINED );
            }
            if ( $are_not_set[0] or $are_not_set[1] )
            {
                if ($is_king)
                {
                    my @options = @{ $_possible_pair_states->{$color}{$rank} };
                KINGS:
                    while ( my ( $card_idx, $state_o ) = each(@are_not_set) )
                    {
                        if ( !$state_o )
                        {
                            next KINGS;
                        }
                        my $data = 0;
                        if (
                            ( !$variant_states->does_have_zero_freecells() )
                            and (
                                any
                                {
                                    my $xx =
                                        $variant_states
                                        ->REVERSE_CARD_PAIR_STATES_MAP()->[$_]
                                        ->[$card_idx];
                                    $xx == $variant_states->single_card_states()
                                        ->{'ABOVE_FREECELL'}
                                } @options
                            )
                            )
                        {
                            $data = $state_reader->read(2);
                        }
                        my $single_options =
                            $variant_states->_calc__remaining__single_options(
                            $card_idx, \@options );
                        Carp::confess("outofbound $data [ @$single_options ]")
                            if $data >= @$single_options;
                        my $oval = _assert_def(
                            $variant_states->rev_single_card_states->[
                                $single_options->[$data]
                            ]
                        );
                        die "big $oval >= $NUM_KING_OPTS"
                            if $oval >= $NUM_KING_OPTS;
                        $card_states[ $indexes->[$card_idx] ][$rank] = $oval;
                    }
                }
                else
                {
                    my @options =
                        @{ $_possible_pair_states->{$color}{$rank} };
                    my $set = sub {
                        my ($opt) = @_;
                        my $v = $self->_set_card_opt( $color, $rank, $opt );
                        return _assert_def(
                            $variant_states->rev_single_card_states->[$v] );
                    };
                    if ( $are_not_set[0] and $are_not_set[1] )
                    {
                        my $s =
                            $options[ $state_reader->read( scalar(@options) ) ];
                        my @pos = @{
                            _assert_def(
                                $variant_states->REVERSE_CARD_PAIR_STATES_MAP()
                                    ->[$s]
                            )
                        };
                        for my $i ( keys @pos )
                        {
                            $card_states[ $indexes->[$i] ][$rank] =
                                $set->( $pos[$i] );
                        }
                    }
                    else
                    {
                        my $card_idx = _assert_def(
                            scalar( first { $are_not_set[$_] } 0 .. 1 ) );
                        my @single_options = sort { $a <=> $b } uniq(
                            map {
                                _assert_def(
                                    $variant_states
                                        ->REVERSE_CARD_PAIR_STATES_MAP(
                                    )->[$_]->[$card_idx]
                                )
                            } @options
                        );
                        my $s = $single_options[ $state_reader->read(
                                scalar @single_options ) ];

                        my $pos = _assert_def($s);

                        $card_states[ $indexes->[$card_idx] ][$rank] =
                            $set->($pos);
                    }
                }
            }
        }
    }

    foreach my $suit_idx ( 0 .. $#suits )
    {
        push @queue,
            [
            14, _assert_def( $foundations_obj->value( $suits[$suit_idx], 0 ) )
            ];
    }

    foreach my $rank ( 1 .. $self->_get_top_rank_for_iter() )
    {
        foreach my $suit_idx ( 0 .. $#suits )
        {
            my $val = $card_states[$suit_idx][$rank];
            if ( $val >= 0 )
            {
                my $base = $NUM_OPTS;
                if ( $rank == $RANK_KING )
                {
                    $base = $NUM_KING_OPTS;
                    die "big $val >= $NUM_KING_OPTS" if $val >= $NUM_KING_OPTS;
                }
                push @queue, [ _assert_def($base), _assert_def($val) ];
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
    if ( ref($base) ne "" or ( $base != int($base) ) )
    {
        Carp::confess("wrong $base");
    }

    my ( $retbase, $ret ) = @{ shift( @{ $self->{_q} } ) };

    die                             if $retbase ne $base;
    die                             if $ret ne int($ret);
    die                             if $ret < 0;
    Carp::confess("big $ret $base") if $ret >= $base;

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

