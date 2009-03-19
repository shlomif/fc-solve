#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

my @vars = (qw(state_copy_ptr final_state first_state_to_check));
my $re = join("|", @vars);
while (<>)
{
    if (my ($pre, $in, $suf) = m[\A( *instance->)($re)( *,? *\n?)\z]ms)
    {
        print "$pre${in}_key,\n";
        print "$pre${in}_val$suf";
    }
    else
    {
        print;
    }
}
