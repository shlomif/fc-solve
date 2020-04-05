#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
EOF

my @deals = ( <<'EOF' =~ /^ *([0-9]+)/gms );
   617	7992
 19677	8037
 13583	8166
 16768	8362
 26557	8468
   718	8484
 30887	8580
 12421	8702
 16837	8736
 28920	8738
 30898	8771
 13765	8806
  8858	10113
 13304	10165
 25599	10565
 27188	10573
 25315	10604
  2288	10870
  6182	10975
  9182	12110
 17323	12435
 24106	13454
 17355	13959
 15592	14758
 31302	15413
  1941	19934
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

FindSeed->parallel_find(
    {
        scan      => \@scans,
        deals     => \@deals,
        threshold => 10,
    },
);
