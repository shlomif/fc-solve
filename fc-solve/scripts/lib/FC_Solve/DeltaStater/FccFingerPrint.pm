package FC_Solve::DeltaStater::FccFingerPrint;

use strict;
use warnings;

use parent 'FC_Solve::DeltaStater::DeBondt';

use Carp ();
use List::Util qw/ max /;

use Games::Solitaire::Verify::Card        ();
use Games::Solitaire::Verify::Column      ();
use Games::Solitaire::Verify::Foundations ();
use Games::Solitaire::Verify::State       ();

use FC_Solve::DeltaStater::OptionsStruct ();
use FC_Solve::VarBaseDigitsReader        ();
use FC_Solve::VarBaseDigitsWriter        ();

=head1 ALGORITHM

See git/fc-solve/fc-solve/docs/debondt-compact-freecell-positions--document.asciidoc

=cut

__PACKAGE__->mk_acc_ref(
    [qw(_card_states _bakers_dozen_topmost_cards_lookup)] );

my $RANK_KING       = 13;
my $FOUNDATION_BASE = $RANK_KING + 1;

my $ORIG_POS                         = 0;
my $ABOVE_PARENT_CARD_OR_EMPTY_SPACE = 1;
my $IN_FOUNDATIONS                   = 2;

# my ( $LOWEST_CARD, $ABOVE_FREECELL, $PARENT_0, $PARENT_1 ) = ( 0 .. 3 );
my %states__with_positive_freecells_count;
my %states__with_zero_freecells_count;
foreach my $state (qw/ LOWEST_CARD ABOVE_FREECELL PARENT_0 PARENT_1/)
{
    $states__with_positive_freecells_count{$state} =
        ( ( max( values(%states__with_positive_freecells_count) ) // -1 ) + 1 );
    if ( $state ne 'ABOVE_FREECELL' )
    {
        $states__with_zero_freecells_count{$state} =
            ( ( max( values(%states__with_zero_freecells_count) ) // -1 ) + 1 );
    }
}
my %STATES_OPTS__zero;
my %STATES_OPTS__positive;
my %positive_rec = (
    in => \%states__with_positive_freecells_count,
    in_base =>
        ( ( max( values(%states__with_positive_freecells_count) ) // -1 ) + 1 ),
    STATES_OPTS    => \%STATES_OPTS__positive,
    state_opt_next => 0,
);
my %zero_rec = (
    in => \%states__with_zero_freecells_count,
    in_base =>
        ( ( max( values(%states__with_zero_freecells_count) ) // -1 ) + 1 ),
    STATES_OPTS    => \%STATES_OPTS__zero,
    state_opt_next => 0,
);
foreach my $rec ( \%positive_rec, \%zero_rec )
{
    my $_add = sub {
        my @input = @_;
        die if @input != 2;
        my @s = ( grep { !exists $rec->{in}->{$_} } @input );
        die if grep { $_ ne 'ABOVE_FREECELL' } @s;
        if ( not @s )
        {
            $rec->{STATES_OPTS}->{ join " ", @{ $rec->{in} }{@input} } =
                $rec->{state_opt_next}++;
        }
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
}

die if $zero_rec{state_opt_next} ne 7;

my $OPT_TOPMOST              = 0;
my $OPT_DONT_CARE            = $OPT_TOPMOST;
my $OPT_FREECELL             = 1;
my $OPT_ORIG_POS             = 2;
my $NUM_KING_OPTS            = 3;
my $OPT_PARENT_SUIT_MOD_IS_0 = 3;
my $OPT_PARENT_SUIT_MOD_IS_1 = 4;
my $NUM_OPTS                 = 5;
my $OPT_IN_FOUNDATION        = 5;
my $NUM_OPTS_FOR_READ        = 6;

my $OPT__BAKERS_DOZEN__ORIG_POS      = 0;
my $OPT__BAKERS_DOZEN__FIRST_PARENT  = 1;
my $NUM__BAKERS_DOZEN__OPTS          = $OPT__BAKERS_DOZEN__FIRST_PARENT + 4;
my $OPT__BAKERS_DOZEN__IN_FOUNDATION = $NUM__BAKERS_DOZEN__OPTS;
my $NUM__BAKERS_DOZEN__OPTS_FOR_READ = $OPT__BAKERS_DOZEN__IN_FOUNDATION + 1;

my @suits = @{ Games::Solitaire::Verify::Card->get_suits_seq() };

sub _init
{
    my ( $self, $args ) = @_;

    $self->SUPER::_init($args);

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

sub _mark_suit_rank_as_true
{
    my ( $self, $suit, $rank, $opt ) = @_;

    @{ $self->_opt_by_suit_rank( $suit, $rank ) } = @$opt;

    return;
}

sub _mark_opt_as_true
{
    my ( $self, $card, $opt ) = @_;

    @{ $self->_opt_by_card($card) } = (@$opt);

    return;
}

sub encode_composite
{
    my ($self) = @_;

    my $derived = $self->_derived_state;

    my $variant_states = $derived->num_freecells ? \%positive_rec : \%zero_rec;

    # die if $derived->num_freecells > 0;

    $self->_initialize_card_states(
          $self->_is_bakers_dozen()
        ? $NUM__BAKERS_DOZEN__OPTS
        : $NUM_OPTS
    );

    my $writer_state       = FC_Solve::VarBaseDigitsWriter->new;
    my $writer_fingerprint = FC_Solve::VarBaseDigitsWriter->new;

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
                $o = [
                    $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                    $variant_states->{in}->{'LOWEST_CARD'}
                ];
            }
            else
            {
                my $parent_card = $col->pos( $height - 1 );
                if ( $self->_is_parent_card( $parent_card, $card ) )
                {
                    $o = [
                        $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                        ( $self->_suit_get_suit_idx( $parent_card->suit ) & 2 )
                        ? $variant_states->{in}->{'PARENT_1'}
                        : $variant_states->{in}->{'PARENT_0'}
                    ];
                }
                else
                {
                    $o = [$ORIG_POS];
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
                [
                    $ABOVE_PARENT_CARD_OR_EMPTY_SPACE,
                    $variant_states->{in}->{'ABOVE_FREECELL'}
                ]
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
            my $fingerprint1;
            my $fingerprint2;
            my $is_founds1 = ( $rank1 >= $rank );
            my $is_founds2 = ( $rank2 >= $rank );
            my $opt1       = $self->_opt_by_suit_rank( $suit1, $rank );
            my $opt2       = $self->_opt_by_suit_rank( $suit2, $rank );
            $opt1->[0] = $IN_FOUNDATIONS if $is_founds1;
            die if !defined $opt1->[0];
            $opt2->[0] = $IN_FOUNDATIONS if $is_founds2;

            if ( !defined $opt2->[0] )
            {
                $DB::single = 1;
                Carp::confess "opt2";
            }
            $fingerprint1 = $opt1->[0];
            $fingerprint2 = $opt2->[0];
            my $is_king = ( $rank == $RANK_KING );

            if ( $rank > 1 )
            {
                if ( $fingerprint1 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                {
                    if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                    {
                    }
                    else
                    {
                        my $state_o = $opt2->[1];
                        if ($is_king)
                        {
                            $writer_state->write(
                                { base => 2, item => $state_o } );
                        }
                        else
                        {
                            $writer_state->write(
                                {
                                    base => $variant_states->{in_base},
                                    item => $state_o
                                }
                            );
                        }
                    }
                }
                else
                {
                    if ( $fingerprint2 != $ABOVE_PARENT_CARD_OR_EMPTY_SPACE )
                    {
                        my $state_o = $opt1->[1];
                        if ($is_king)
                        {
                            $writer_state->write(
                                { base => 2, item => $state_o } );
                        }
                        else
                        {
                            $writer_state->write(
                                {
                                    base => $variant_states->{in_base},
                                    item => $state_o
                                }
                            );
                        }
                    }
                    else
                    {
                        my @states = ( $opt1->[1], $opt2->[1] );
                        if ($is_king)
                        {
                            foreach my $state_o (@states)
                            {
                                $writer_state->write(
                                    { base => 2, item => $state_o } );
                            }
                        }
                        else
                        {
                            $writer_state->write(
                                {
                                    base => $variant_states->{state_opt_next},
                                    item => (
                                        $variant_states->{STATES_OPTS}
                                            ->{"@states"}
                                            // do { die "unknown key @states"; }
                                    ),
                                }
                            );
                        }
                    }
                }
            }

            # print("$rank $color $fingerprint1 $fingerprint2\n");

            $writer_fingerprint->write( { base => 3, item => $fingerprint1 } );
            $writer_fingerprint->write( { base => 3, item => $fingerprint2 } );
            $cnt += 2;
        }
    }

    if ( $cnt != 52 )
    {
        ...;
    }
    $self->_free_card_states;

    my $_data = sub {
        my $n   = shift()->get_data;
        my $ret = '';
        while ( $n > 0 )
        {
            $ret .= chr( $n & 255 );
            $n >>= 8;
        }
        return $ret;
    };

    my $ret = $_data->($writer_fingerprint);
    return [ $ret, $_data->($writer_state) ];
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
