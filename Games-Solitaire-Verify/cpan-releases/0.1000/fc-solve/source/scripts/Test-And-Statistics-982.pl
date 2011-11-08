#!/usr/bin/perl

use strict;
use warnings;

use lib './t/t/lib';
use Games::Solitaire::FC_Solve::DeltaStater;

open my $dump_fh, '<', '982.dump'
    or die "Cannot open 982.dump for reading - $!";

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
    my $delta = Games::Solitaire::FC_Solve::DeltaStater->new(
        {
            init_state_str => $init_state_str,
        }
    );
    
    $delta->set_derived(
        {
            state_str => $state,
        }
    );

    my $encoded = $delta->encode();

    $encoded_counts{length($encoded)}++;

    if ($delta->decode($encoded)->to_string() ne $state)
    {
        die "Wrong encoding/decoding process at line $line_idx!";
    }

    print "Counts:\n";
    print map { "$_ => $encoded_counts{$_}\n" } sort { $a <=> $b } keys(%encoded_counts);
}
