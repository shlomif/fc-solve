#!/bin/bash
for (( i=100 ; i<=1000 ; ++i ))
do
    perl process.pl --quotas-expr "(($i)x1000)" |
        printf "%d\\t%d\\n" "$i" "$(perl -lne 'print "$1" if m/^total_iters = ([0-9]+)/')"
done
