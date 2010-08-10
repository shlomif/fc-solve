#!/bin/bash
cat ../scans.txt | perl -lne '/\t(.*)/ && print $1' | grep -vP '\b-dto\b' | 
    (
        while read LINE; do
            perl time-scan.pl $LINE -sp r:tf || exit -1
        done
    )
