#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
 25599	10156
 27188	10164
 25315	10195
 10660	10390
 15327	10459
  2288	10461
  1617	10504
  6182	10566
  1347	11352
 29122	11588
  9182	11701
 31326	11799
 17323	12026
 15227	12126
  4267	12173
  8613	12211
  8044	12280
 24106	13045
 17355	13550
 15592	14349
 31302	15004
EOF

my @deals = (<<'EOF' =~ /^ *([0-9]+)/gms);
 25599	10156
 27188	10164
 25315	10195
 10660	10390
 15327	10459
  2288	10461
  1617	10504
  6182	10566
  1347	11352
 29122	11588
  9182	11701
 31326	11799
 17323	12026
 15227	12126
  4267	12173
  8613	12211
  8044	12280
 24106	13045
 17355	13550
 15592	14349
 31302	15004
EOF

# 4973 1120 [4267 Verdict: Solved ; Iters: 82 ; Length: 112 1120] [1347 Verdict: Solved ; Iters: 101 ; Length: 119 1120] [10660 Verdict: Solved ; Iters: 102 ; Length: 113 1120] [31326 Verdict: Solved ; Iters: 106 ; Length: 161 1120] ; --method random-dfs -to "01[234579]"
my @scans =
(
    q#--method random-dfs -to "01[2345789]"#,
    q#--method random-dfs -to "01[234579]"#,
    q#--method random-dfs -to "01[234589]"#,
    q#--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"#,
    q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]"#,
);

FindSeed->find(
    {
        scan => \@scans,
        deals => \@deals,
        threshold => 4,
    },
);
