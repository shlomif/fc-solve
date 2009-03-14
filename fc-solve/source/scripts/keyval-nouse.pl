#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

foreach my $file (glob("*.c"))
{
    my @lines = io($file)->getlines();
    
    my @new;

    while (defined(my $l = shift(@lines)))
    {
        # A function definition
        if ($l =~ m{\A\w})
        {
            my %ptr_states = ();
            my $first_iter = 1;
            while ($first_iter || ($l = shift(@lines) && ($l !~ m{\A\{})))
            {
                $first_iter = 0;
                if ($l =~ m{fcs_state_with_locations_t *\* *(\w+),})
                {
                    $ptr_state{$1} = 1;
                }
                else
                {

            }
        }
        else
        {
            push @new, $l;
        }
    }
}

