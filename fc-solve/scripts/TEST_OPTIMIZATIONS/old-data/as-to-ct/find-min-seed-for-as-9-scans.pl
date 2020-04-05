#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
EOF

my @deals = ( <<'EOF' =~ /^ *([0-9]+)/gms );
 13583	8028
 16768	8224
 22298	8294
 31750	8296
 26557	8330
   718	8346
  4213	8369
 30887	8442
 12421	8564
 16837	8598
 28920	8600
 30898	8633
 13765	8668
 12344	8759
 30055	8965
  5986	9793
  8858	9975
 13304	10027
 30741	10079
 25599	10427
 27188	10435
 25315	10466
  2288	10732
  6182	10837
  9182	11972
 17323	12297
  8044	12551
 24106	13316
 17355	13821
 15592	14620
 31302	15275
  1941	19796
EOF

my @scans = (
    q#--method random-dfs -to "01[2345789]"#,
    q#--method random-dfs -to "01[234579]"#,
    q#--method random-dfs -to "01[234589]"#,
    q#--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]"#,
    q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]"#,
    q#--method random-dfs -to "[01][23457]"#,
    q#--method random-dfs -to "[0123457]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "7,[0123][456789]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "7,[0123][4567]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "5,[0123][4567]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "10,[0123][4567]"#,
    q#--method random-dfs -to "[01][23457]" -dto2 "8,[0123][4567]"#,
);

FindSeed->find(
    {
        scan      => \@scans,
        deals     => \@deals,
        threshold => 8,
    },
);
