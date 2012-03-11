#!/usr/bin/perl

use strict;
use warnings;

use Config;
use Cwd;

use Inline (
    C => 'DATA',
    CLEAN_AFTER_BUILD => 0,
    LIBS => "-L" . Cwd::getcwd() . " -lfcs_delta_states_test",
    CCFLAGS => "-I" . Cwd::getcwd(),
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test", 
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

use IO::Handle;
use lib './t/t/lib';
use Games::Solitaire::FC_Solve::DeltaStater;

STDOUT->autoflush(1);

my $MAX_ITERS = 1000;

sub test_freecell_deal
{
    my ($deal_idx) = @_;

    # We cannot use the "-s" and "-i" flags here any longer, because the state
    # canonization (which is now compulsory after the locations functionality
    # was removed) re-orders the positions of the stacks in the boards in the 
    # run-time display, which causes the encoding and decoding to not operate
    # properly.
    open my $dump_fh, 
    "./board_gen/pi-make-microsoft-freecell-board -t $deal_idx | ./fc-solve --freecells-num 2 -p -t -sam -l eo -mi $MAX_ITERS -ni -l fools-gold |"
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

    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
        {
            init_state_str => $init_state_str,
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
        my $got_state = enc_and_dec($init_state_str, $state);

        $delta->set_derived({ state_str => $state, });

        my $expected_state =  $delta->_derived_state->clone();

        my $cols_indexes = $delta->_composite_get_cols_and_indexes()->{cols_indexes};
        my @fc_indexes = sort { $sort_by->($a) <=> $sort_by->($b) } (0 .. $num_freecells-1);

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
        if ($expected_str ne $got_state)
        {
            die "State was wrongly encoded+decoded: Deal=<<$deal_idx>>\nState=<<\n$state\n>> ; Got_state=<<\n$got_state\n>> ; expected_str=<<\n$expected_str\n>>!";
        }
        $count++;
        print "\rDeal: $deal_idx ; Processed: $count";
    }

    close($dump_fh);

    return;
}

for my $deal_idx (1 .. 32_000)
{
    test_freecell_deal($deal_idx);
}

__DATA__
__C__

#include "delta_states_iface.h"

SV* enc_and_dec(char * init_state_s, char * derived_state_s) {
    SV * ret;
    char * s;
    s = fc_solve_user_INTERNAL_delta_states_enc_and_dec(init_state_s, derived_state_s);

    ret = newSVpv(s, 0);
    free(s);
    return ret;
}
