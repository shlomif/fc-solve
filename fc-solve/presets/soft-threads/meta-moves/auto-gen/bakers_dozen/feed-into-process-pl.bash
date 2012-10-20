#!/bin/bash
# Run this from ${trunk}/fc-solve/presets/soft-threads/meta-moves/auto-gen
(
    ruby ../optimize-seq/scripts/process-optimal_quotas-dump.rb \
    find_optimal_quotas-300.dump) | \
    perl -I.. ../process.pl --quotas-expr="$(
        echo -n '('; 
        ruby -p -e 'sub(/\n/,",");END{print "((350)x#{300-$.}),";}' ; 
        echo -n ')'
        )" "$@"
