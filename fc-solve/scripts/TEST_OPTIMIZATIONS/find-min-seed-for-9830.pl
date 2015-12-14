#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
 25599	10069
 27188	10077
 25315	10108
 10660	10303
 15327	10372
  2288	10374
  1617	10417
  6182	10479
  1347	11265
 29122	11501
  9182	11614
 31326	11712
 17323	11939
 15227	12039
  4267	12086
  8613	12124
  8044	12193
 24106	12958
 17355	13463
 15592	14262
 31302	14917
EOF

my @deals = (<<'EOF' =~ /^ *([0-9]+)/gms);
  9830	16663
EOF

# 241 209 Verdict: Solved ; Iters: 102 ; Length: 133 ; --method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"
# 911 286 Verdict: Solved ; Iters: 93 ; Length: 129 ; --method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"
# 1291 958 Verdict: Solved ; Iters: 87 ; Length: 117 ; --method random-dfs -to "01[234579]"
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
    },
);
