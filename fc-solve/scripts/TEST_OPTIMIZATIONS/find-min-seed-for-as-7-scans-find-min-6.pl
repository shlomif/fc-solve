#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
EOF

my @deals = (<<'EOF' =~ /^ *([0-9]+)/gms);
 27046	9192
 23555	9495
  5986	9628
  8858	9810
 13304	9862
  3717	9892
 30741	9914
 25599	10262
 27188	10270
 25315	10301
 15327	10565
  2288	10567
  1617	10610
  6182	10672
 29122	11694
  9182	11807
 17323	12132
 15227	12232
  8613	12317
  8044	12386
 24106	13151
 17355	13656
 15592	14455
 31302	15110
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
        threshold => 6,
    },
);
