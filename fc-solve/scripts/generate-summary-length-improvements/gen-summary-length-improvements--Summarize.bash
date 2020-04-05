#!/bin/bash
get_nums()
{
    seed="$1"
    shift
    paste ~/Backup/Arcs/FC_SOLVE_SUMMARIZE_RESULTS--fif-10.cat.txt ~/Backup/Arcs/fcs-summary-len-seed/lens-theme1--seed="$seed".txt |  perl -lanE 'use List::Util qw/min/; my ($cur, @cands) = /(?<=Length: )(-?[0-9]+)/g; my $cand = min(grep { $_ > 0 } @cands); if ($cand > 0 and $cand < $cur) {push @x, $cur-$cand} END { print "@x" }'
}

summary()
{
    perl -lanE 'use List::Util qw/max sum/; my @n = /([0-9]+)/g; printf "N=%d Sum=%d Max=%d\n", (scalar@n), sum(@n), max(@n)'
}

total_summary()
{
    paste ~/Backup/Arcs/FC_SOLVE_SUMMARIZE_RESULTS--fif-10.cat.txt ~/Backup/Arcs/fcs-summary-len-seed/lens-theme1--seed={1..200}.txt | perl -lanE 'use List::Util qw/min/; my ($cur, @cands) = /(?<=Length: )(-?[0-9]+)/g; my $cand = min(grep { $_ > 0 } @cands); if ($cand > 0 and $cand < $cur) {push @x, $cur-$cand} END { print "@x" }' |     perl -lanE 'use List::Util qw/max sum/; my @n = /([0-9]+)/g; printf "N=%d Sum=%d Max=%d\n", (scalar@n), sum(@n), max(@n)'
}

for seed in $(seq 1 157) ; do
    printf '%i\t%s\n' "$seed" "$(get_nums "$seed" | summary)"
done
