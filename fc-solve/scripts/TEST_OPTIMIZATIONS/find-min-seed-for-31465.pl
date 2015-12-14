#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my @deals = (31465);

# 4086 923 Verdict: Solved ; Iters: 170 ; Length: 142
# my $scan = q#--method random-dfs -to "01[2345789]"#;
# 2463 694 Verdict: Solved ; Iters: 283 ; Length: 122
# my $scan = q#--method random-dfs -to "01[234579]"#;
# 1366 1276 Verdict: Solved ; Iters: 287 ; Length: 139
# my $scan = q#--method random-dfs -to "01[234589]"#;
# my $scan = q#--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"#;
# 9400 6747 Verdict: Solved ; Iters: 238 ; Length: 147
my $scan = q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]"#;

FindSeed->find(
    {
        scan => $scan,
        deals => \@deals,
    },
);
