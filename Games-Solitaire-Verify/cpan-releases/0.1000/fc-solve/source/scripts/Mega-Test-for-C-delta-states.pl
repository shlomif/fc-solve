#!/usr/bin/perl

use strict;
use warnings;

use Config;
use FindBin;

BEGIN {
    print "Foo ==", 
    LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    "\n";
}
use Inline (
    C => 'DATA',
    CLEAN_AFTER_BUILD => 0,
    LIBS => "-L$FindBin::Bin -lfcs_delta_states_test",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test", 
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

use lib './t/t/lib';
use Games::Solitaire::FC_Solve::DeltaStater;

open my $dump_fh, '<', '24.dump'
    or die "Cannot open 24.dump for reading - $!";

my $line_idx = 0;
sub read_state
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
    exit(0);
}

my $init_state_str = read_state();

my %encoded_counts;

while (my $state = read_state())
{
    my $got_state = enc_and_dec($init_state_str, $state); 

    print "From <<$state>> we got <<$got_state>>\n";
}
__DATA__
__C__

extern char * fc_solve_user_INTERNAL_delta_states_enc_and_dec(
    const char * init_state_str_proto,
    const char * derived_state_str_proto
    );

SV* enc_and_dec(char * init_state_s, char * derived_state_s) {
    SV * ret;
    char * s;
    s = fc_solve_user_INTERNAL_delta_states_enc_and_dec(init_state_s, derived_state_s);

    ret = newSVpv(s, 0);
    free(s);
    return ret;
}
