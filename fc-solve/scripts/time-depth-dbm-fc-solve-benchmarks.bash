#!/bin/bash
# Time timestamper runs of the processes' logs and summarise the results.
{
    for i in "$@"
    do
        printf "%s\t%f\n" "$i" "$(perl -E 'say $ARGV[1]-$ARGV[0]' "$(head -1 "$i")" "$(tail -1 "$i")")"
    done
} | sort -n -k 2
