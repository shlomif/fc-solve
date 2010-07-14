#!/bin/bash
cat ../scans.txt | perl -lne '/\t(.*)/ && print $1' | 
    (
        while read LINE; do
            perl time-scan.pl $LINE
        done
    )
