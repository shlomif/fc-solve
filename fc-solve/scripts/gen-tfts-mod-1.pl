use strict;
use warnings;

use FindBin;

use lib "$FindBin::Bin";
use PatsolveScan;

print <<"EOF";
#!/bin/sh
freecell-solver-range-parallel-solve 1 32000 1 \\
@{[pats_fast()]} --st-name p1 -nst \\
--method random-dfs -to "[01ABCDE]=asw(1)" -sp r:tf --st-name s1 -nst \\
EOF

print <<'EOF';
--prelude "40000@p1,4000@s1"
EOF
