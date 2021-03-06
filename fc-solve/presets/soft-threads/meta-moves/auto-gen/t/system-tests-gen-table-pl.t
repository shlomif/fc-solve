#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use Test::Differences qw( eq_or_diff );

# These results were tested with this commit:
#
# commit 743f2c3189d76b7363d26915f2f087d4e03bf3bd
# Author: Shlomi Fish <shlomif@shlomifish.org>
# Date:   Fri Mar 28 22:25:20 2014 +0300
#
# They may change later.

{
    # TEST
    eq_or_diff(
        scalar(`$^X gen-table.pl`),
        <<'EOF',
Deal No. | 1 Iters | 1 Len | 2 Iters | 2 Len | 3 Iters | 3 Len |
 1       |  123    | 127   |  1711   | 120   | 1285    | 161   |
 2       |   98    | 145   |    96   | 138   |   98    | 107   |
 3       |  125    | 115   |   104   | 109   |   93    | 110   |
 4       |  117    | 123   |   236   | 129   |  447    | 153   |
 5       |  110    | 143   |   101   | 136   |  691    | 175   |
 6       |  403    | 176   |   262   | 122   |  125    | 130   |
 7       | 1260    | 134   |   307   | 125   | 1823    | 165   |
 8       |   70    |  98   |    70   |  98   |  122    | 125   |
 9       |   -1    |  -1   | 47169   | 135   | 1097    | 136   |
10       |  189    | 135   |   115   | 125   | 3043    | 202   |
EOF
        'gen-table.pl with no arguments yields right results.',
    );
}

{
    # TEST
    eq_or_diff(
        scalar(`$^X gen-table.pl --top=20 --scans=10,2,3,1`),
        <<'EOF',
Deal No. | 10 Iters | 10 Len | 2 Iters | 2 Len | 3 Iters | 3 Len | 1 Iters | 1 Len |
 1       | 10179    | 133    |  1711   | 120   | 1285    | 161   |  123    | 127   |
 2       |   160    | 176    |    96   | 138   |   98    | 107   |   98    | 145   |
 3       |   117    | 119    |   104   | 109   |   93    | 110   |  125    | 115   |
 4       |   295    | 174    |   236   | 129   |  447    | 153   |  117    | 123   |
 5       |   149    | 162    |   101   | 136   |  691    | 175   |  110    | 143   |
 6       |   410    | 175    |   262   | 122   |  125    | 130   |  403    | 176   |
 7       |  1240    | 210    |   307   | 125   | 1823    | 165   | 1260    | 134   |
 8       |   163    | 155    |    70   |  98   |  122    | 125   |   70    |  98   |
 9       | 62151    | 197    | 47169   | 135   | 1097    | 136   |   -1    |  -1   |
10       |   214    | 145    |   115   | 125   | 3043    | 202   |  189    | 135   |
11       |   157    | 147    |   131   | 115   |  178    | 158   |  181    | 142   |
12       |  3174    | 169    |    96   | 133   |  867    | 135   |  121    | 133   |
13       |   299    | 141    |    78   |  97   |  131    | 115   |   87    |  97   |
14       |   383    | 139    |    84   |  98   | 1829    | 120   |   91    | 109   |
15       |   676    | 154    |    92   | 130   |  355    | 181   |   93    | 130   |
16       |   130    | 132    |    77   | 114   |  213    | 119   |   77    | 114   |
17       |   133    | 149    |   112   | 109   |  403    | 127   |  143    | 121   |
18       |   137    | 122    |    81   | 103   |  197    | 126   |  109    | 129   |
19       |   114    | 132    |  2743   | 118   |  259    | 144   | 1839    | 130   |
20       |   359    | 117    |    86   | 118   |  444    | 128   |   91    | 118   |
EOF
        'gen-table.pl with --top=20 and --scans',
    );
}
