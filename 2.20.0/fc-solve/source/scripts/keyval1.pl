#!/usr/bin/perl

use strict;
use warnings;

while (<>)
{
    chomp;
    if (my ($lead, $star, $id, $comma) =
            m{\A(\s+)fcs_state_with_locations_t(\s*\*(?:\s*\*)?\s*)(\w+)((?:,)?)\s*\z}
        )
    {
        print "${lead}fcs_state_t${star}${id}_key,\n";
        print "${lead}fcs_state_extra_info_t${star}${id}_val${comma}\n";
    }
    else
    {
        print "$_\n";
    }
}
