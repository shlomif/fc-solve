#!/usr/bin/perl

use strict;
use warnings;

use Config;
use Cwd;

use Inline (
    C => 'DATA',
    CLEAN_AFTER_BUILD => 0,
    INC => ["-I$ENV{FCS_PATH}", "-I$ENV{FCS_SRC_PATH}",],
    LIBS => "-L" . $ENV{FCS_PATH} . " -lfcs_delta_states_test -lfcs_debondt_delta_states_test -lfcs_dbm_calc_derived_test -lgmp",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test",
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test",
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

use IO::Handle;
use lib './t/t/lib';
use Games::Solitaire::FC_Solve::DeltaStater;
use Games::Solitaire::FC_Solve::DeltaStater::DeBondt;

use Parallel::ForkManager;

STDOUT->autoflush(1);

my $MAX_ITERS = 1000;
my $max_num_bits = 0;

sub perl_debondt_enc_and_dec
{
    my ($init_state_str, $state) = @_;

    my $delta = Games::Solitaire::FC_Solve::DeltaStater::DeBondt->new(
        {
            init_state_str => $init_state_str,
        }
    );

    $delta->set_derived({state_str => $state,});

    my $token = $delta->encode_composite();

    if ($token->as_bin() =~ /\A0b0*(1[01]+)\z/)
    {
        my $num_bits = length($1);
        if ($num_bits > $max_num_bits)
        {
            $max_num_bits = $num_bits;
        }
    }
    else
    {
        die "Token is wrong: " . $token->as_bin() . "!";
    }

    return $delta->decode($token)->to_string();
}

my $which_encoding = ($ENV{FCS_ENC} || '');

my $is_c_debondt = ($which_encoding eq 'Cd');
my $is_debondt = ($is_c_debondt or $which_encoding eq 'd');

my $variant = ($ENV{FCS_VARIANT} || 'freecell');
my $is_bakers_dozen = ($variant eq 'bakers_dozen');

sub _debondt_normalize
{
    my $s = shift;
    return $is_debondt
        ? join('', sort { $a cmp $b } split(/^/, $s))
        : $s
        ;
}

sub test_freecell_deal
{
    my ($deal_idx) = @_;

    my $open_cmd =
        $is_bakers_dozen
        ? "./board_gen/make_pysol_freecell_board.py -t $deal_idx bakers_dozen | ./fc-solve -g bakers_dozen -p -t -sam -l three-eighty -mi $MAX_ITERS |"
        : "./board_gen/pi-make-microsoft-freecell-board -t $deal_idx | ./fc-solve --freecells-num 2 -p -t -sam -l three-eighty -mi $MAX_ITERS |"
        ;

    # We cannot use the "-s" and "-i" flags here any longer, because the state
    # canonization (which is now compulsory after the locations functionality
    # was removed) re-orders the positions of the stacks in the boards in the
    # run-time display, which causes the encoding and decoding to not operate
    # properly.
    open my $dump_fh, $open_cmd
        or die "Cannot open $deal_idx for reading - $!";

    my $line_idx = 0;
    my $read_state = sub
    {
        my $ret = '';
        while (my $line = <$dump_fh>)
        {
            $line_idx++;
            if ($line =~ /\AFoundations: /)
            {
                $ret .= $line;
                $line = <$dump_fh>;
                $line_idx++;
                while ($line =~ /\S/)
                {
                    $ret .= $line;
                    $line = <$dump_fh>;
                    $line_idx++;
                }
                return $ret;
            }
        }
        return;
    };

    my $init_state_str = $read_state->();

    # Solution is empty - we cannot continue
    if (!defined($init_state_str))
    {
        return;
    }

    my %encoded_counts;

    my $num_freecells = 2;

    my $two_fc_variant = Games::Solitaire::Verify::VariantsMap->new->get_variant_by_id('freecell');

    $two_fc_variant->num_freecells($num_freecells);

    my $delta_class = $is_debondt ? 'Games::Solitaire::FC_Solve::DeltaStater::DeBondt' :
        'Games::Solitaire::FC_Solve::DeltaStater';
    my $delta = $delta_class->new(
        {
            init_state_str => $init_state_str,
            ($is_bakers_dozen ? (variant => 'bakers_dozen') : ()),
        }
    );

    my $get_bitmask = sub {
        my $card = shift;
        if (!defined($card))
        {
            return 0;
        }
        else
        {
            return $delta->_get_card_bitmask($card);
        }
    };

    my $sort_by = sub {
        my ($idx) = @_;
        return $get_bitmask->($delta->_derived_state->get_freecell($idx));
    };

    my $count = 0;

    while (my $state = $read_state->())
    {
        my $got_state;

        if ($is_debondt)
        {
            $state = horne_prune($state);
            $got_state = $is_c_debondt
                ? debondt_enc_and_dec($init_state_str, $state)
                : perl_debondt_enc_and_dec($init_state_str, $state);
        }
        else
        {
            $got_state = enc_and_dec($init_state_str, $state);
        }

        $delta->set_derived({ state_str => $state, });

        my $expected_state =  $delta->_derived_state->clone();

        my $cols_indexes = $delta->_composite_get_cols_and_indexes()->{cols_indexes};
        my @fc_indexes = sort { $sort_by->($a) <=> $sort_by->($b) } (0 .. $num_freecells-1);

        if ($is_c_debondt)
        {
            # @fc_indexes = reverse(@fc_indexes);
            @fc_indexes =
            (
                (grep { $sort_by->($_) } @fc_indexes),
                (grep { !$sort_by->($_) } @fc_indexes),
            );
        }

        $expected_state->set_foundations($delta->_derived_state->_foundations->clone());
        foreach my $i (0 .. $#fc_indexes)
        {
            $expected_state->set_freecell($i, $delta->_derived_state->get_freecell($fc_indexes[$i]));
        }
        $expected_state->_columns([]);
        foreach my $i (0 .. $#$cols_indexes)
        {
            $expected_state->add_column($delta->_derived_state->get_column($cols_indexes->[$i]));
        }

        my $expected_str = $expected_state->to_string();

        # print "From <<$state>> we got <<$got_state>>\n";
        if (_debondt_normalize($expected_str) ne _debondt_normalize($got_state))
        {
            die "State was wrongly encoded+decoded: Deal=<<$deal_idx>>\nState=<<\n$state\n>> ; Got_state=<<\n$got_state\n>> ; expected_str=<<\n$expected_str\n>>!";
        }
        $count++;
        printf "\r%-70s", "Deal: $deal_idx ; Processed: $count ; MaxBits: $max_num_bits";
    }

    close($dump_fh);

    return;
}

for my $deal_idx (1 .. 32_000)
{
    print "Testing $deal_idx\n";
    test_freecell_deal($deal_idx);
}

my $pm = Parallel::ForkManager->new(4);
DEALS:
for my $deal_idx (1 .. 32_000)
{
    if ($pm->start())
    {
        next DEALS;
    }
    test_freecell_deal($deal_idx);

    $pm->finish;
}
$pm->wait_all_children;

__DATA__
__C__

#include "delta_states_iface.h"
#include "debondt_delta_states_iface.h"

SV* enc_and_dec(char * init_state_s, char * derived_state_s) {
    SV * ret;
    char * s;
    s = fc_solve_user_INTERNAL_delta_states_enc_and_dec(init_state_s, derived_state_s);

    ret = newSVpv(s, 0);
    free(s);
    return ret;
}

SV* debondt_enc_and_dec(char * init_state_s, char * derived_state_s) {
    SV * ret;
    char * s;
    s = fc_solve_user_INTERNAL_debondt_delta_states_enc_and_dec(init_state_s, derived_state_s);

    ret = newSVpv(s, 0);
    free(s);
    return ret;
}

SV* horne_prune(char * init_state_s) {
    SV * ret;
    char * s;
    int count_moves;
    count_moves = fc_solve_user_INTERNAL_perform_horne_prune(init_state_s, &s);

    ret = newSVpv(s, 0);
    free(s);
    return ret;
}
