#!/usr/bin/env perl

use strict;
use warnings;

use FindSeed ();

my $x = <<'EOF';
EOF

my @deals = ( <<'EOF' =~ /^ *([0-9]+)/gms );
  4022	7556
 20358	7566
 28825	7566
 19763	7568
 23774	7573
 31119	7609
 21491	7610
 29866	7662
  4609	7699
 18320	7708
 14269	7754
 24341	7810
 23424	7871
 12957	8015
 20785	8018
  8604	8127
 16768	8628
 16837	9002
 28920	9004
 13765	9072
  8858	10379
 13304	10431
 25599	10831
 27188	10839
 25315	10870
  6182	11241
 17323	12701
 24106	13720
 17355	14225
 15592	15024
 31302	15679
  1941	20200
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
q#--method random-dfs -to "01[234567]" -dto2 "5,01[2345789]" -dto2 "10,[0123][4567]"#,
);

FindSeed->parallel_find(
    {
        scan      => \@scans,
        deals     => \@deals,
        threshold => 10,
    },
);
