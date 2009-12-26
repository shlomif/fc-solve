#!/bin/bash
# Run this from ${trunk}/fc-solve/presets/soft-threads/meta-moves/auto-gen
(cd optimize-seq && 
    ruby scripts/process-optimal_quotas-dump.rb \
    results/optimal_quotas-300.dump) | \
    perl process.pl --quotas-expr="$(
        echo -n '('; 
        ruby -p -e 'sub(/\n/,",");END{print "350," * (300-$.);}' ; 
        echo -n ')'
        )" "$@"
