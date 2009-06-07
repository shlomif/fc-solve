#!/bin/sh
# Make sure your .ackrc contains:
# --type-set=cmake=.txt,.cmake
ack -f | grep -v 't/data/sample-solutions' | \
    xargs ruby -lpi -e 'sub(/\s+$/, "")'
