#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
EOF

my @deals = (<<'EOF' =~ /^ *([0-9]+)/gms);
 23555	9565
  5986	9698
  8858	9880
 13304	9932
 30741	9984
 25599	10332
 27188	10340
 25315	10371
  2288	10637
  1617	10680
  6182	10742
 29122	11764
  9182	11877
 17323	12202
  8613	12387
  8044	12456
 24106	13221
 17355	13726
 15592	14525
 31302	15180
EOF

my @scans =
(
    q#--method random-dfs -to "01[2345789]"#,
    q#--method random-dfs -to "01[234579]"#,
    q#--method random-dfs -to "01[234589]"#,
    q#--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"#,
    q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]"#,
    q#--method random-dfs -to "[01][23457]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "7,[0123][456789]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "7,[0123][4567]"#,
);

FindSeed->find(
    {
        scan => \@scans,
        deals => \@deals,
        threshold => 4,
    },
);
