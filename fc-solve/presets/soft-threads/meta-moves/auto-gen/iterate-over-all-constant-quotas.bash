#!/bin/bash
for i in `seq 100 1000` ; do 
    perl process.pl --quotas-expr "(($i)x1000)" | 
    printf "%d\\t%d\\n" "$i" "$(perl -lne 'print "$i\t$1" if m/^total_iters = (\d+)/')" ; 
done
