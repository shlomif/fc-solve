#!/usr/bin/perl

use strict;
use warnings;

while (<>)
{
    chomp;
    if (my ($lead, $star, $id) =
            m{\A(\s+)fcs_state_with_locations_t(\s*\*\s*)(\w+);\s*\z}
        )
    {
        print "${lead}fcs_state_t${star}${id}_key;\n";
        print "${lead}fcs_state_extra_info_t${star}${id}_val;\n";
    }
    else
    {
        print "$_\n";
    }
}
