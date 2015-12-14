#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my @deals = (1941);

# 1122 181 Verdict: Solved ; Iters: 726 ; Length: 140
# my $scan = q#--method random-dfs -to "01[2345789]"#;

# my $scan = q#--method random-dfs -to "01[234579]"#;

# my $scan = q#--method random-dfs -to "01[234589]"#;

# 13156 3894 Verdict: Solved ; Iters: 319 ; Length: 95
# my $scan = q#--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"#;

# 2250 363 Verdict: Solved ; Iters: 367 ; Length: 112
# my $scan = q#--method random-dfs -to "01[234567]" -dto2 "5,[0132][456789]"#;

# 2548 1493 Verdict: Solved ; Iters: 578 ; Length: 111
my $scan = q#--method random-dfs -to "01[234567]" -dto2 "7,[0132][4589]"#;

# my $scan = q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]"#;


FindSeed->find(
    {
        scan => $scan,
        deals => \@deals,
    },
);
