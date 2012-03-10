#!/bin/bash
# Run this from ${trunk}/fc-solve/presets/soft-threads/meta-moves/auto-gen
path="$(pwd)"
(cd optimize-seq && 
    ruby scripts/process-optimal_quotas-dump.rb \
    "${path}/find_optimal_quotas-300.dump") | \
    perl process.pl --quotas-expr="$(
        echo -n '('; 
        ruby -p -e 'sub(/\n/,",");END{print "((350)x#{300-$.}),";}' ; 
        echo -n ')'
        )" "$@"
