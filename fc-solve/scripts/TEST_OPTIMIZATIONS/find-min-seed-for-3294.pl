#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my @deals = (3294);

# my $scan = q#--method random-dfs -to "01[2345789]"#;
# my $scan = q#--method random-dfs -to "01[234579]"#;
# my $scan = q#--method random-dfs -to "01[234589]"#;
# 13100 6519 Verdict: Solved ; Iters: 85 ; Length: 136
my $scan = q#--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"#;

# my $scan = q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]"#;

FindSeed->find(
    {
        scan  => $scan,
        deals => \@deals,
    },
);
