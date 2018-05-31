use strict;
use warnings;

use FindBin;

use lib "$FindBin::Bin";
use PatsolveScan;

print <<"EOF";
#!/bin/sh
freecell-solver-range-parallel-solve 1 32000 1 \\
@{[pats_fast()]} --st-name p1 -nst \\
@{[pats([qw(5 1 6 -1 7 11 4 2 2 1 2)], [ 0.0032, 0.32, -3.0])]} --st-name p2 -nst \\
EOF

print <<'EOF';
--prelude "13000@p1,2000@p2"
EOF
